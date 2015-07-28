#include "webgl.h"
#include "textures.h"
#include "types/GameBalance.h"
#include "types/Actor.h"
#include "types/Particle.h"
#include "types/AnimSet.h"
#include "zlib/zlib.h"
#include "itemlib.h"
#include "strings.h"
#include <set>
#include <bitset>
#include <algorithm>

static Vector Read(Anim::Type::DT_VECTOR3D const& v) {
  return Vector(v.x00_X, v.x04_Y, v.x08_Z);
}
static Vector Read(Appearance::Type::DT_VECTOR3D const& v) {
  return Vector(v.x00_X, v.x04_Y, v.x08_Z);
}
static Quaternion Read(Appearance::Type::Quaternion const& q) {
  return Quaternion(q.x00_DT_VECTOR3D.x00_X, q.x00_DT_VECTOR3D.x04_Y,
    q.x00_DT_VECTOR3D.x08_Z, q.x0C);
}
static Quaternion Read(Anim::Type::Quaternion16 const& q) {
  return Quaternion(q.x00.value(), q.x02.value(), q.x04.value(), q.x06.value());
}

std::map<std::string, uint32> get_item_icons(bool leg = false);

bool isParent(Appearance::Type::Structure& data, int from, int to) {
  while (from != to && from >= 0) {
    from = data.x010_BoneStructures[from].x040;
  }
  return from == to;
}
bool isRelated(Appearance::Type::Structure& data, int a, int b) {
  return isParent(data, a, b) || isParent(data, b, a);
}

namespace WebGL {
  struct Triangle {
    Index verts[3];
    BoneIndex bones[9];
    uint32 numBones = 0;
    uint32 group = -1;
    uint32 unused;
  };
  static const int MAX_BONES = 128;
  static const int MAX_GROUP_BONES = 24;
  struct TriangleSet {
    typedef std::pair<int, Triangle*> Ref;
    std::set<Ref> queue;
    std::vector<Triangle*> list[MAX_BONES];
    void add(Triangle* tri) {
      for (uint32 i = 0; i < tri->numBones; ++i) {
        list[tri->bones[i]].push_back(tri);
      }
      queue.emplace(tri->unused = tri->numBones, tri);
    }
    Triangle* pop() {
      auto it = queue.begin();
      Triangle* tri = it->second;
      queue.erase(it);
      return tri;
    }
    void reduce(BoneIndex bone) {
      for (Triangle* tri : list[bone]) {
        if (tri->group != -1) continue;
        auto it = queue.find(Ref(tri->unused, tri));
        if (it == queue.end()) throw Exception("uh oh");
        queue.erase(it);
        queue.emplace(--tri->unused, tri);
      }
    }
    bool empty() const {
      return queue.empty();
    }
  };

  struct ObjectData {
    struct Group {
      std::bitset<MAX_BONES> bones;
      std::map<Index, Index> vertices;
      std::vector<Index> indices;
    };
    std::vector<Group> groups;
    Appearance::Type::SubObject& original;

    ObjectData(Appearance::Type::SubObject& object, uint32 maxBones)
      : original(object)
    {
      if (!object.x020_VertInfluences.size()) {
        groups.emplace_back();
        auto& group = groups[0];
        for (uint32 i = 0; i < maxBones; ++i) group.bones.set(i);
        for (uint32 i = 0; i < object.x010_FatVertexs.size(); ++i) group.vertices[i] = i;
        for (auto idx : object.x038_short) group.indices.push_back(idx);
        return;
      }

      std::vector<Triangle> triangles;
      for (uint32 i = 0; i + 3 <= object.x038_short.size(); i += 3) {
        triangles.emplace_back();
        auto& tri = triangles.back();
        for (uint32 j = 0; j < 3; ++j) {
          tri.verts[j] = object.x038_short[i + j];
          for (auto& inf : object.x020_VertInfluences[tri.verts[j]].x00_Influences) {
            if (inf.x04) tri.bones[tri.numBones++] = inf.x00;
          }
        }
        std::sort(tri.bones, tri.bones + tri.numBones);
        tri.numBones = std::unique(tri.bones, tri.bones + tri.numBones) - tri.bones;
      }
      while (true) {
        TriangleSet tris;
        for (auto& tri : triangles) {
          if (tri.group == -1) {
            tris.add(&tri);
          }
        }
        if (tris.empty()) break;
        groups.emplace_back();
        auto& group = groups.back();
        uint32 used = 0;
        while (!tris.empty()) {
          Triangle* tri = tris.pop();
          if (tri->unused + used > MAX_GROUP_BONES) break;
          for (uint16 v : tri->verts) {
            auto it = group.vertices.find(v);
            if (it == group.vertices.end()) {
              Index index = group.vertices.size();
              group.vertices[v] = index;
              group.indices.push_back(index);
            } else {
              group.indices.push_back(it->second);
            }
          }
          tri->group = groups.size() - 1;
          for (uint32 i = 0; i < tri->numBones; ++i) {
            if (!group.bones.test(tri->bones[i])) {
              tris.reduce(tri->bones[i]);
              ++used;
              group.bones.set(tri->bones[i]);
            }
          }
        }
      }
    }
  };

  static Archive* texArchive = nullptr;

  uint32 AddTexture(uint32 texId, bool gray = false) {
    if (texArchive && texId != -1 && !texArchive->has(texId)) {
      Image image = GameTextures::get(texId);
      if (image) image.write(texArchive->create(texId), gray ? ImageFormat::PNGGrayscale : ImageFormat::PNG);
    }
    return texId == -1 ? 0 : texId;
  }
  void DoWriteModel(File& file, SnoFile<Appearance>& app) {
    ModelHeader header;
    header.numBones = app->x010_Structure.x010_BoneStructures.size();
    header.numHardpoints = app->x010_Structure.x0F0_Hardpoints.size();
    header.numAppearances = app->x1C0_AppearanceLooks.size();
    header.numMaterials = app->x1B0_AppearanceMaterials.size();
    header.numObjects = app->x010_Structure.x088_GeoSets[0].x10_SubObjects.size();
    header.boneOffset = sizeof header;
    header.hardpointOffset = header.boneOffset + header.numBones * sizeof(Bone);
    header.objectOffset = header.hardpointOffset + header.numHardpoints * sizeof(Hardpoint);
    header.materialOffset = header.objectOffset + header.numObjects * sizeof(Object);
    uint32 vCount = 0;
    for (auto& object : app->x010_Structure.x088_GeoSets[0].x10_SubObjects) {
      for (auto& v : object.x010_FatVertexs) {
        header.center += Read(v.x00_Position);
        ++vCount;
      }
    }
    if (vCount) header.center /= static_cast<float>(vCount);
    uint32 fileSize = header.materialOffset + sizeof(Material) * header.numAppearances * header.numMaterials;
    file.write(header);
    std::vector<Bone> bones;
    for (auto& src : app->x010_Structure.x010_BoneStructures) {
      bones.emplace_back();
      Bone& dst = bones.back();
      memset(dst.name, 0, sizeof dst.name);
      strcpy(dst.name, src.x000_Text);
      dst.parent = src.x040;
      dst.bind = Matrix::scale(1.0f / src.x06C_PRSTransform.x1C) *
                 Read(src.x06C_PRSTransform.x00_Quaternion).conj().matrix() *
                 Matrix::translate(-Read(src.x06C_PRSTransform.x10_DT_VECTOR3D));
      dst.bind.transpose();
      file.write(dst);
      dst.bind.transpose();
    }
    for (auto& src : app->x010_Structure.x0F0_Hardpoints) {
      Hardpoint dst;
      memset(dst.name, 0, sizeof dst.name);
      strcpy(dst.name, src.x00_Text);
      dst.parent = src.x40;
      dst.transform = Matrix::translate(Read(src.x44_PRTransform.x10_DT_VECTOR3D)) *
                      Read(src.x44_PRTransform.x00_Quaternion).matrix();
      if (dst.parent != -1) dst.transform = bones[dst.parent].bind * dst.transform;
      dst.transform.transpose();
      file.write(dst);
    }
    std::map<std::string, uint32> materials;
    for (uint32 i = 0; i < header.numMaterials; ++i) {
      materials[app->x1B0_AppearanceMaterials[i].x00_Text] = i;
    }
    std::vector<ObjectData> objects;
    for (auto& src : app->x010_Structure.x088_GeoSets[0].x10_SubObjects) {
      objects.emplace_back(src, app->x010_Structure.x010_BoneStructures.size());
      auto& data = objects.back();
      Object dst;
      dst.material = materials[src.x05C_Text];
      dst.numGroups = data.groups.size();
      dst.groupOffset = fileSize;
      fileSize += sizeof(ObjectGroup) * dst.numGroups;
      file.write(dst);
    }
    for (auto& mat : app->x1B0_AppearanceMaterials) {
      for (auto& sub : mat.x88_SubObjectAppearances) {
        uint32 fpos = file.tell();
        uint32 texDiffuse = -1, texSpecular = -1, texTintBase = -1, texTintMask = -1;
        for (auto& tex : sub.x18_UberMaterial.x58_MaterialTextureEntries) {
          switch (tex.x00) {
          case 1:
            texDiffuse = tex.x08_MaterialTexture.x00_TexturesSno;
            break;
          case 5:
            texSpecular = tex.x08_MaterialTexture.x00_TexturesSno;
            break;
          case 11:
            texTintBase = tex.x08_MaterialTexture.x00_TexturesSno;
            break;
          case 54:
            texTintMask = tex.x08_MaterialTexture.x00_TexturesSno;
            break;
          }
        }
        float alpha = 1.0f;
        for (size_t i = 0; i < sub.x10_TagMap.size() - 1; ++i) {
          if (sub.x10_TagMap[i] == 196864) {
            alpha = *(float*) &sub.x10_TagMap[i + 1];
          }
        }
        Material dst;
        dst.diffuse = AddTexture(texDiffuse);
        dst.specular = AddTexture(texSpecular);
        dst.tintBase = AddTexture(texTintBase);
        dst.tintMask = AddTexture(texTintMask);
        file.write(dst);
      }
    }
    for (auto& object : objects) {
      for (auto& group : object.groups) {
        uint32 pos = file.tell();
        ObjectGroup dst;
        dst.numBones = group.bones.count();
        dst.boneOffset = fileSize;
        fileSize += sizeof(BoneIndex) * dst.numBones;
        dst.numVertices = group.vertices.size();
        dst.vertexOffset = fileSize;
        fileSize += sizeof(Vertex) * dst.numVertices;
        dst.numIndices = group.indices.size();
        dst.indexOffset = fileSize;
        fileSize += sizeof(Index) * ((dst.numIndices + 1) / 2) * 2;
        file.write(dst);
      }
    }
    for (auto& object : objects) {
      for (auto& group : object.groups) {
        uint32 boneMap[MAX_BONES];
        uint32 numBones = 0;
        for (uint32 i = 0; i < app->x010_Structure.x010_BoneStructures.size(); ++i) {
          if (group.bones.test(i)) {
            file.write(BoneIndex(i));
            boneMap[i] = numBones++;
          }
        }
        std::vector<uint32> vertices(group.vertices.size());
        for (auto it : group.vertices) {
          vertices[it.second] = it.first;
        }
        for (uint32 index : vertices) {
          auto& src = object.original.x010_FatVertexs[index];
          auto* inf = (object.original.x020_VertInfluences.size() > index ?
            &object.original.x020_VertInfluences[index].x00_Influences : nullptr);
          Vertex dst;
          memset(&dst, 0, sizeof dst);
          dst.position = Read(src.x00_Position);
          dst.normal[0] = src.x0C_Normal.x00_X - 128;
          dst.normal[1] = src.x0C_Normal.x01_Y - 128;
          dst.normal[2] = src.x0C_Normal.x02_Z - 128;
          dst.texcoord[0] = src.x18_TexCoords[0].x00_U - 0x8000;
          dst.texcoord[1] = src.x18_TexCoords[0].x02_V - 0x8000;
          if (inf) {
            for (uint32 j = 0; j < 3; ++j) {
              dst.bone_idx[j] = boneMap[(*inf)[j].x00];
              dst.bone_weight[j] = (*inf)[j].x04;
            }
          } else {
            dst.bone_weight[0] = 1;
          }
          file.write(dst);
        }
        file.write(group.indices.data(), group.indices.size() * sizeof(Index));
        if (group.indices.size() & 1) {
          file.write16(0);
        }
      }
    }
  }
  void WriteModel(std::string const& name) {
    DoWriteModel(File("WebGL" / name + ".model", "wb"), SnoFile<Appearance>(name));
  }

  void DoWriteAnimation(File& file, Anim::Type& anim) {
    auto& perm = anim.x28_AnimPermutations[0];
    AnimationSequence header;
    header.numFrames = perm.x090;
    header.velocity = perm.x048_Velocity;
    header.numBones = perm.x088_BoneNames.size();
    header.animationOffset = sizeof header;
    uint32 fileSize = header.animationOffset + header.numBones * sizeof(AnimationCurve);
    file.write(header);
    for (size_t i = 0; i < header.numBones; ++i) {
      AnimationCurve curve;
      memset(&curve, 0, sizeof curve);
      strcpy(curve.bone, perm.x088_BoneNames[i].x00_Text);
      curve.numTranslations = perm.x0A0_TranslationCurves[i].x10_TranslationKeies.size();
      curve.numRotations = perm.x0B0_RotationCurves[i].x10_RotationKeies.size();
      curve.numScales = perm.x0C0_ScaleCurves[i].x10_ScaleKeies.size();
      curve.translationOffset = fileSize;
      fileSize += curve.numTranslations * sizeof(TranslationKey);
      curve.rotationOffset = fileSize;
      fileSize += curve.numRotations * sizeof(RotationKey);
      curve.scaleOffset = fileSize;
      fileSize += curve.numScales * sizeof(ScaleKey);
      file.write(curve);
    }
    for (size_t i = 0; i < header.numBones; ++i) {
      for (auto& key : perm.x0A0_TranslationCurves[i].x10_TranslationKeies) {
        file.write(key.x00);
        file.write(Read(key.x04_DT_VECTOR3D));
      }
      for (auto& key : perm.x0B0_RotationCurves[i].x10_RotationKeies) {
        file.write(key.x00);
        file.write(Read(key.x04_Quaternion16));
      }
      for (auto& key : perm.x0C0_ScaleCurves[i].x10_ScaleKeies) {
        file.write(key.x00);
        file.write(key.x04);
      }
    }
  }

  void WriteAnimation(std::string const& name) {
    SnoFile<Anim> anim(name);
    if (!anim) return;
    File file("WebGL" / name + ".anim", "wb");
    DoWriteAnimation(file, *anim);
  }

  void DumpActorData(Archive& mdl, Archive& ani, uint32 aid, uint32 raid = 0) {
    SnoFile<Actor> actor(Actor::name(aid));
    if (!actor) return;
    SnoFile<Appearance> app(actor->x014_AppearanceSno.name());
    if (app) DoWriteModel(mdl.create(raid ? raid : aid), app);
    File animFile = SnoLoader::Load<AnimSet>(actor->x068_AnimSetSno.name());
    if (animFile) {
      json::Value value;
      json::Visitor::printExStrings = false;
      json::BuilderVisitor visitor(value);
      AnimSet::parse(animFile, &visitor);
      visitor.onEnd();
      for (auto& sub : value) {
        if (sub.type() != json::Value::tObject) continue;
        for (auto& val : sub) {
          uint32 id = val.getInteger();
          SnoFile<Anim> anim(Anim::name(id));
          if (anim) {
            DoWriteAnimation(ani.create(id), *anim);
          }
        }
      }
    }
  }

  uint32 FixEmitter(SnoFile<Actor>& actor) {
    if (1 || actor->x014_AppearanceSno.name() == "Emitter") {
      if (!actor->x080_MsgTriggeredEvents.size()) return actor->x000_Header.id;
      auto& name = actor->x080_MsgTriggeredEvents[0].x004_TriggerEvent.x02C_SNOName;
      if (name.type() != "Particle") return actor->x000_Header.id;
      SnoFile<Particle> particle(name.name());
      if (!particle) return actor->x000_Header.id;
      if (!Actor::name(particle->x338_ActorSno)) return actor->x000_Header.id;
      //printf("Fixing emitter: %s -> %s\n", actor.name().c_str(), Actor::name(particle->x338_ActorSno));
      return particle->x338_ActorSno;
    }
  }

  bool DumpItemActor(Archive& mdl, Archive& ani, std::set<uint32>& done, uint32 aid, bool fixEmitter, uint32 raid = 0) {
    if (done.count(aid)) return true;
    SnoFile<Actor> actor(Actor::name(aid));
    if (!actor) return false;
    done.insert(aid);
    std::map<uint32, uint32> tags;
    for (uint32 i = 1; i + 3 <= actor->x060_TagMap.size(); i += 3) {
      tags[actor->x060_TagMap[i + 1]] = actor->x060_TagMap[i + 2];
    }
    if (tags[94240] && !raid) {
      bool hasSelf = false;
      bool hasAny = false;
      for (uint32 id = 94208; id <= 94219; ++id) {
        if (tags[id] == aid) { hasSelf = true; continue; }
        if (DumpItemActor(mdl, ani, done, tags[id], fixEmitter, tags[id])) hasAny = true;
      }
      for (uint32 id = 94720; id <= 94731; ++id) {
        if (tags[id] == aid) { hasSelf = true; continue; }
        if (DumpItemActor(mdl, ani, done, tags[id], fixEmitter, tags[id])) hasAny = true;
      }
      if (!hasSelf && hasAny) return true;
    }
    DumpActorData(mdl, ani, fixEmitter ? FixEmitter(actor) : aid, raid ? raid : aid);
    return true;
  }
  void DumpActorLook(json::Value& value, uint32 aid) {
    SnoFile<Actor> actor(Actor::name(aid));
    if (!actor) return;
    SnoFile<Appearance> app(actor->x014_AppearanceSno.name());
    if (!app) return;
    auto& val = value[fmtstring("%d", aid)]["looks"];
    uint32 index = 0;
    for (auto& look : app->x1C0_AppearanceLooks) {
      val[fmtstring("%u", HashName(look.x00_Text))] = index++;
    }
  }
  void ClassInfo() {
    SnoFile<GameBalance> gmb("Characters");
    json::Value value;
    for (auto& hero : gmb->x088_Heros) {
      DumpActorLook(value, hero.x108_ActorSno);
      DumpActorLook(value, hero.x10C_ActorSno);
    }
    json::write(File("webgl_look.js", "w"), value, json::mJS);
  }
  void AddActorInfo(json::Value& values, uint32 aid, bool hair, bool fixEmitter, uint32 orig = 0) {
    SnoFile<Actor> actor(Actor::name(aid));
    if (!actor) return;
    uint32 emitId = (fixEmitter ? FixEmitter(actor) : aid);
    if (emitId != actor->x000_Header.id) {
      AddActorInfo(values, emitId, hair, false, aid);
      return;
    }
    auto& value = values[fmtstring("%d", orig ? orig : aid)];
    if (hair) {
      std::map<uint32, uint32> tags;
      for (uint32 i = 1; i + 3 <= actor->x060_TagMap.size(); i += 3) {
        tags[actor->x060_TagMap[i + 1]] = actor->x060_TagMap[i + 2];
      }
      value["hair"] = tags[66564];
    }
    SnoFile<AnimSet> animSet(actor->x068_AnimSetSno.name());
    if (animSet) {
      uint32 anim = 0;
      for (auto& tm : animSet->x010_AnimSetTagMaps) {
        if (!tm.x08_TagMap.size() || tm.x08_TagMap.size() != tm.x08_TagMap[0] * 3 + 1) continue;
        for (uint32 i = 1; i + 3 <= tm.x08_TagMap.size(); i += 3) {
          if (Anim::name(tm.x08_TagMap[i + 2])) {
            anim = tm.x08_TagMap[i + 2];
          }
        }
      }
      if (anim) {
        value["animation"] = anim;
      }
    }
    SnoFile<Appearance> app(actor->x014_AppearanceSno.name());
    if (app) {
      json::Value enable(json::Value::tObject);
      uint32 index = 0;
      for (auto& object : app->x010_Structure.x088_GeoSets[0].x10_SubObjects) {
        if (strcmp(object.x05C_Text, "FX_EMIT")) {
          enable[fmtstring("%d", index)] = 0;
        }
        ++index;
      }
      value["enable"] = enable;
    }
  }
  json::Value MakeActor(SnoFile<Actor>& actor, std::map<uint32, uint32>& tags, json::Value& actors, bool fixEmitter, bool right = false, bool hair = false) {
    static int charActors[] = {3301, 3285, 6544, 6526, 6485, 6481, 4721, 4717, 75207, 74706, 238284, 238286};
    if (tags[94240]) {
      json::Value res(json::Value::tObject);
      for (uint32 id = (right ? 94720 : 94208), idx = 0; idx < 12; ++id, ++idx) {
        if (Actor::name(tags[id])) {
          res[fmtstring("%d", charActors[idx])] = tags[id];
          AddActorInfo(actors, tags[id], hair, fixEmitter);
        }
      }
      return res;
    } else {
      AddActorInfo(actors, actor->x000_Header.id, hair, fixEmitter);
      return actor->x000_Header.id;
    }
  }
  void FillItemInfo(json::Value& dst, json::Value& actors, GameBalance::Type::Item& item, std::string const& type, std::string const& slot) {
    SnoFile<Actor> actor(item.x108_ActorSno.name());
    if (!actor) return;
    std::map<uint32, uint32> tags;
    for (uint32 i = 1; i + 3 <= actor->x060_TagMap.size(); i += 3) {
      tags[actor->x060_TagMap[i + 1]] = actor->x060_TagMap[i + 2];
    }
    uint32 aid = 0;
    if (slot == "head") {
      dst["actor"] = MakeActor(actor, tags, actors, false, false, true);
    }
    if (slot == "legs" || slot == "feet" || slot == "torso" || slot == "hands") {
      dst["armortype"] = tags[66560];
      dst["look"] = tags[66561];
    }
    if (slot == "shoulders") {
      dst["actor"] = MakeActor(actor, tags, actors, false);
      dst["actor_r"] = MakeActor(actor, tags, actors, false, true);
    }
    if (slot == "mainhand" || slot == "offhand" || slot == "twohand" || slot == "onehand") {
      if (type == "quiver") return;
      dst["actor"] = MakeActor(actor, tags, actors, type == "source" || type == "mojo");
    }
  }
  void ItemInfo() {
    json::Value out;
    json::Value items;
    json::Value actors;
    json::parse(File("webgl_actors.js"), actors, json::mJS);
    json::parse(File("itemtypes.js"), items, json::mJS);
    Logger::begin(items["itemById"].getMap().size(), "Dumping items");
    std::set<uint32> done;
    for (auto& kv : items["itemById"].getMap()) {
      Logger::item(kv.first.c_str());
      std::string type = kv.second["type"].getString();
      std::string slot = items["itemTypes"][type]["slot"].getString();
      auto* item = ItemLibrary::get(kv.first);
      if (!item) continue;
      auto& dst = out[kv.first];
      FillItemInfo(dst, actors, *item, type, slot);
    }
    json::Value final;
    for (auto& kv : out.getMap()) {
      if (kv.second.type() == json::Value::tObject) {
        final[kv.first] = kv.second;
      }
    }
    json::write(File("webgl_items.js", "w"), final, json::mJS);
    json::write(File("webgl_actors.js", "w"), actors, json::mJS);
  }

  void DumpActorSets(Archive& ans, uint32 aid) {
    SnoFile<Actor> actor(Actor::name(aid));
    if (!actor) return;
    SnoFile<AnimSet> animSet(actor->x068_AnimSetSno.name());
    if (!animSet) return;
    File& dst = ans.create(aid);
    for (auto& tm : animSet->x010_AnimSetTagMaps) {
      uint32 count = tm.x08_TagMap[0];
      if (!count || tm.x08_TagMap.size() != count * 3 + 1) {
        dst.write32(0);
        continue;
      }
      dst.write32(count);
      for (uint32 i = 0; i < count; ++i) {
        dst.write32(tm.x08_TagMap[i * 3 + 2]);
        dst.write32(tm.x08_TagMap[i * 3 + 3]);
      }
    }
  }
  void AnimSets() {
    Archive ans;
    SnoFile<GameBalance> gmb("Characters");
    Logger::begin(gmb->x088_Heros.size(), "Dumping characters");
    for (auto& hero : gmb->x088_Heros) {
      Logger::item(hero.x000_Text);
      DumpActorSets(ans, hero.x108_ActorSno);
      DumpActorSets(ans, hero.x10C_ActorSno);
    }
    Logger::end();
    ans.write(File("animsets.wgz", "wb"), true);
  }

  void GenericItems() {
    Logger::begin(3, "Loading assets");
    Logger::item("textures");   Archive tex;// (File("textures.wgz"), false);
    Logger::item("models");     Archive mdl;// (File("models.wgz"), true);
    Logger::item("animations"); Archive ani;// (File("animations.wgz"), true);
    Logger::end();
    //texArchive = &tex;

    json::Value actors;
    json::parse(File("d3gl_actors.js"), actors, json::mJS);
    std::set<uint32> done;
    json::Value items;
    json::Value itemsout;
    json::parse(File("itemtypes.js"), items, json::mJS);
    Logger::begin(items["itemById"].getMap().size(), "Dumping items");
    for (auto& kv : items["itemById"].getMap()) {
      Logger::item(kv.first.c_str());
      auto* item = ItemLibrary::get(kv.first);
      if (!item) continue;
      std::string type = kv.second["type"].getString();
      std::string slot = items["itemTypes"][type]["slot"].getString();
      if (type != "mojo") continue;
      DumpItemActor(mdl, ani, done, item->x108_ActorSno, type == "source" || type == "mojo");
      FillItemInfo(itemsout[item->x000_Text], actors, *item, type, slot);
    }
    Logger::end();

    json::Value genitems;
    json::Value generic;
    json::parse(File("d3gl_items.js"), generic, json::mJS);
    for (auto& kv : generic.getMap()) {
      std::string id = kv.first;
      if (!kv.second.has("type")) continue;
      std::string type = kv.second["type"].getString();
      if (!items["itemTypes"].has(type)) {
        Logger::log("unknown type: %s", type.c_str());
        continue;
      }
      std::string slot = items["itemTypes"][type]["slot"].getString();
      auto* item = ItemLibrary::get(id);
      if (!item || !Actor::name(item->x108_ActorSno)) continue;
      if (type != "mojo") continue;
      auto& dst = genitems[id];
      dst["name"] = Strings::get("Items", id);
      dst["type"] = type;
      dst["promo"] = true;
      FillItemInfo(dst, actors, *item, type, slot);
    }
    json::write(File("extra_actors.js", "w"), actors, json::mJS);
    json::write(File("extra_items.js", "w"), genitems, json::mJS);
    json::write(File("extra_items_orig.js", "w"), itemsout, json::mJS);
    //return;

    Logger::begin(3, "Writing assets");
    //Logger::item("textures");   tex.write(File("textures.wgz", "wb"), false);
    //Logger::item("models");     mdl.write(File("models.wgz", "wb"), true);
    //Logger::item("animations"); ani.write(File("animations.wgz", "wb"), true);
    Logger::end();
    texArchive = nullptr;
  }

  void AllItems(bool models, bool info, bool load) {
    Archive tex, mdl, ani;
    if (models && load) {
      Logger::begin(3, "Loading assets");
      Logger::item("textures");   tex.load(File("textures.wgz"), false);
      Logger::item("models");     mdl.load(File("models.wgz"), true);
      Logger::item("animations"); ani.load(File("animations.wgz"), true);
      Logger::end();
    }
    //texArchive = &tex;

    SnoFile<GameBalance> gmb("Characters");
    Logger::begin(gmb->x088_Heros.size(), "Dumping characters");
    for (auto& hero : gmb->x088_Heros) {
      Logger::item(hero.x000_Text);
      DumpActorData(mdl, ani, hero.x108_ActorSno);
      DumpActorData(mdl, ani, hero.x10C_ActorSno);
    }
    Logger::end();

    json::Value items, itemsout, actors;
    if (info) json::parse(File("d3gl_actors.js"), actors, json::mJS);
    json::parse(File("itemtypes.js"), items, json::mJS);
    Logger::begin(items["itemById"].getMap().size(), "Dumping items");
    std::set<uint32> done;
    for (auto& kv : items["itemById"].getMap()) {
      Logger::item(kv.first.c_str());
      auto* item = ItemLibrary::get(kv.first);
      if (!item) continue;
      std::string type = kv.second["type"].getString();
      std::string slot = items["itemTypes"][type]["slot"].getString();
      if (models) DumpItemActor(mdl, ani, done, item->x108_ActorSno, type == "source" || type == "mojo");
      if (info) FillItemInfo(itemsout[item->x000_Text], actors, *item, type, slot);
    }
    Logger::end();

    json::Value genitems;
    json::Value generic;
    json::parse(File("d3gl_items.js"), generic, json::mJS);
    for (auto& kv : generic.getMap()) {
      std::string id = kv.first;
      if (!kv.second.has("type")) continue;
      std::string type = kv.second["type"].getString();
      if (!items["itemTypes"].has(type)) {
        Logger::log("unknown type: %s", type.c_str());
        continue;
      }
      std::string slot = items["itemTypes"][type]["slot"].getString();
      auto* item = ItemLibrary::get(id);
      if (!item || !Actor::name(item->x108_ActorSno)) continue;
      if (models) DumpItemActor(mdl, ani, done, item->x108_ActorSno, type == "source" || type == "mojo");
      auto& dst = genitems[id];
      dst["name"] = Strings::get("Items", id);
      dst["type"] = type;
      dst["promo"] = true;
      FillItemInfo(dst, actors, *item, type, slot);
    }
    if (info) {
      json::write(File("extra_actors.js", "w"), actors, json::mJS);
      json::write(File("extra_items.js", "w"), genitems, json::mJS);
      json::write(File("extra_items_orig.js", "w"), itemsout, json::mJS);
    }

    if (models) {
      Logger::begin(3, "Writing assets");
      //Logger::item("textures");   tex.write(File("textures.wgz", "wb"), false);
      Logger::item("models");     mdl.write(File("models.wgz", "wb"), true);
     // Logger::item("animations"); ani.write(File("animations.wgz", "wb"), true);
      Logger::end();
      texArchive = nullptr;
    }
  }

  File& Archive::create(uint32 id) {
    auto& file = files_[id];
    file.resize(0);
    return file;
  }
  void Archive::write(File& file, bool compression) {
    std::map<uint32, std::vector<uint8>> data;
    for (auto& kv : files_) {
      auto& vec = data[kv.first];
      uint32 outSize = kv.second.size();
      vec.resize(outSize);
      if (compression) {
        gzencode(kv.second.data(), kv.second.size(), vec.data(), &outSize);
        vec.resize(outSize);
      } else {
        memcpy(vec.data(), kv.second.data(), outSize);
      }
    }
    uint32 count = data.size();
    file.write32(count);
    uint32 offset = count * 12 + 4;
    for (auto& kv : data) {
      file.write32(kv.first);
      file.write32(offset);
      file.write32(kv.second.size());
      offset += kv.second.size();
    }
    for (auto& kv : data) {
      file.write(kv.second.data(), kv.second.size());
    }
  }
  void Archive::load(File& file, bool compression) {
    if (file) {
      uint32 count = file.read32();
      uint32 prev = count * 12 + 4;
      for (uint32 i = 0; i < count; ++i) {
        file.seek(i * 12 + 4);
        uint32 id = file.read32();
        uint32 offset = file.read32();
        uint32 size = file.read32();
        if (offset != prev) {
          Logger::log("%4d gap: %d bytes\n", i, offset - prev);
        }
        prev += size;
        file.seek(offset);
        MemoryFile& mem = files_[id];
        if (compression) {
          std::vector<uint8> temp(size);
          file.read(temp.data(), size);
          z_stream z;
          memset(&z, 0, sizeof z);
          z.next_in = temp.data();
          z.avail_in = size;
          z.total_in = size;
          z.next_out = nullptr;
          z.avail_out = 0;
          z.total_out = 0;

          int result = inflateInit2(&z, 16 + MAX_WBITS);
          if (result == Z_OK) {
            do {
              uint32 pos = mem.size();
              z.avail_out = size;
              z.next_out = mem.reserve(size);
              result = inflate(&z, Z_NO_FLUSH);
              mem.resize(pos + size - z.avail_out);
              if (result == Z_NEED_DICT || result == Z_DATA_ERROR || result == Z_MEM_ERROR) break;
            } while (result != Z_STREAM_END);
          }
        } else {
          file.read(mem.reserve(size), size);
        }
      }
    Logger::log("total: %d bytes\n", prev);
    }
  }
  bool Archive::has(uint32 id) {
    return files_.count(id) != 0;
  }

  void Archive::compare(File& diff, Archive& lhs, Archive& rhs, char const*(*Func)(uint32)) {
    std::set<uint32> files;
    for (auto& kv : lhs.files_) files.insert(kv.first);
    for (auto& kv : rhs.files_) files.insert(kv.first);
    for (uint32 id : files) {
      uint32 lsize = (lhs.has(id) ? lhs.files_[id].size() : 0);
      uint32 rsize = (rhs.has(id) ? rhs.files_[id].size() : 0);
      char const* name = (Func ? Func(id) : nullptr);
      if (lsize != rsize) diff.printf("%-8u %-8u %-8u%s\n", id, lsize, rsize, name ? name : "");
    }
  }
}
