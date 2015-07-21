#include "model.h"
#include "gl.h"
#include "textures.h"

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
static Vector VertexPos(Appearance::Type::FatVertex const& v) {
  return Read(v.x00_Position);
}
static Vector VertexNormal(Appearance::Type::FatVertex const& v) {
  return Vector(v.x0C_Normal.x00_X.value(),
    v.x0C_Normal.x01_Y.value(),
    v.x0C_Normal.x02_Z.value());
}
static Vector VertexTexCoord(Appearance::Type::FatVertex const& v) {
  return Vector(v.x18_TexCoords[0].x00_U.value(), v.x18_TexCoords[0].x02_V.value(), 0);
}

class Model : public GL {
  Appearance::Type const& app;
  Anim::Type const& anim;
  Vector center;
  int index;
  int appearance;
  uint32 prevTime;
  uint32 animCounter;
  bool paused;
  bool showBones;
public:
  Model(SnoFile<Appearance> const& appFile, SnoFile<Anim> const& animFile)
    : GL(animFile.name().c_str())
    , app(*appFile)
    , anim(*animFile)
    , index(0)
    , appearance(0)
    , prevTime(GetTickCount())
    , animCounter(0)
    , paused(true)
    , showBones(false)
  {
    int count = 0;
    File dd("ids.txt", "w");
    for (auto& object : app.x010_Structure.x088_GeoSets[0].x10_SubObjects) {
      for (auto& v : object.x010_FatVertexs) {
        center += VertexPos(v);
      }
      dd.printf("%d %d %s\n", HashName(object.x05C_Text), HashNameLower(object.x05C_Text), object.x05C_Text);
      dd.printf("%d %d %s\n", HashName(object.x0DC_Text), HashNameLower(object.x0DC_Text), object.x0DC_Text);
      count += object.x010_FatVertexs.size();
    }
    center /= float(count);

    bones.resize(app.x010_Structure.x010_BoneStructures.size());
    for (size_t i = 0; i < bones.size(); ++i) {
      auto& src = app.x010_Structure.x010_BoneStructures[i];
      auto& dst = bones[i];
      dst.name = src.x000_Text;
      boneMap[dst.name] = &dst;
      if (src.x040 != -1) {
        dst.parent = &bones[src.x040];
        dst.parent->children.push_back(&dst);
      }
      dst.invTransform =
        Matrix::scale(1.0f / src.x06C_PRSTransform.x1C) *
        Read(src.x06C_PRSTransform.x00_Quaternion).conj().matrix() *
        Matrix::translate(-Read(src.x06C_PRSTransform.x10_DT_VECTOR3D));
      dst.pos = Read(src.x06C_PRSTransform.x10_DT_VECTOR3D);
    }
  }

  struct Bone {
    std::string name;
    Bone* parent = nullptr;
    std::vector<Bone*> children;
    Vector pos;

    Matrix invTransform;
    Matrix curTransform;

    void reset() {
      curTransform = Matrix::identity();
    }
    void scale(float f) {
      curTransform *= Matrix::scale(f);
    }
    void rotate(Quaternion const& q) {
      curTransform *= q.matrix();
    }
    void translate(Vector const& v) {
      curTransform *= Matrix::translate(v);
    }
    void finish() {
      for (Bone* child : children) {
        child->curTransform = curTransform * child->curTransform;
      }
      curTransform *= invTransform;
    }
  };

  struct Vertex {
    Vector pos;
    Vector normal;
    Vector tex;

    Vertex() {}
    Vertex(Appearance::Type::FatVertex const& v)
      : pos(VertexPos(v))
      , normal(VertexNormal(v))
      , tex(VertexTexCoord(v))
    {}

    void transform(Matrix const& m) {
      pos = m * pos;
      normal = m.rotate(normal);
    }

    void draw() {
      glNormal3fv(normal);
      glTexCoord2fv(tex);
      glVertex3fv(pos);
    }
  };

  struct Triangle {
    int a, b, c;
    float z = 0.0f;
    Triangle() {}
    Triangle(int a, int b, int c)
      : a(a), b(b), c(c)
    {}

    static bool zsort(Triangle const& lhs, Triangle const& rhs) {
      return lhs.z < rhs.z;
    }
    static bool less(Triangle const& lhs, Triangle const& rhs) {
      if (lhs.a != rhs.a) return lhs.a < rhs.a;
      if (lhs.b != rhs.b) return lhs.b < rhs.b;
      return lhs.c < rhs.c;
    }
    static bool equal(Triangle const& lhs, Triangle const& rhs) {
      return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c;
    }
  };

  std::vector<Bone> bones;
  Map<Bone*> boneMap;

  void onKey(int vk) {
    switch (vk) {
    case VK_LEFT:
      index -= 1;
      if (index < 0) index = app.x010_Structure.x088_GeoSets[0].x10_SubObjects.size() - 1;
      break;
    case VK_RIGHT:
      index += 1;
      if (index >= app.x010_Structure.x088_GeoSets[0].x10_SubObjects.size()) index = 0;
      break;
    case VK_DOWN:
      appearance -= 1;
      if (appearance < 0) appearance = app.x1C0_AppearanceLooks.size() - 1;
      break;
    case VK_UP:
      appearance += 1;
      if (appearance >= app.x1C0_AppearanceLooks.size()) appearance = 0;
      break;
    case VK_SPACE:
      paused = !paused;
      break;
    case 'B':
      showBones = !showBones;
      break;
    }
  }

  template<class T>
  static uint32 findKey(Array<T> const& keys, uint32 frame) {
    if (!keys.size() || frame < keys[0].x00) return 0;
    uint32 L = 0;
    uint32 R = keys.size();
    while (R - L > 1) {
      uint32 M = (L + R) / 2;
      if (keys[M].x00 <= frame) {
        L = M;
      } else {
        R = M;
      }
    }
    return L;
  }

  void update(float time) {
    auto& perm = anim.x28_AnimPermutations[0];
    time *= 60.0f * perm.x048_Velocity;
    uint32 frame = static_cast<uint32>(time);
    time -= static_cast<float>(frame);
    frame %= perm.x090 - 1;
    float k0 = 1 - time, k1 = time;

    for (Bone& bone : bones) {
      bone.reset();
    }
    std::vector<Bone*> boneRef;
    for (uint32 i = 0; i < perm.x088_BoneNames.size(); ++i) {
      auto it = boneMap.find(perm.x088_BoneNames[i].x00_Text);
      boneRef.push_back(it == boneMap.end() ? nullptr : it->second);
    }
    for (uint32 i = 0; i < perm.x0A0_TranslationCurves.size(); ++i) {
      auto& curve = perm.x0A0_TranslationCurves[i];
      if (i >= boneRef.size() || !boneRef[i] || !curve.x10_TranslationKeies.size()) continue;
      Bone* bone = boneRef[i];
      uint32 pos = findKey(curve.x10_TranslationKeies, frame);
      if (pos < curve.x10_TranslationKeies.size() - 1) {
        bone->translate(Read(curve.x10_TranslationKeies[pos].x04_DT_VECTOR3D) * k0 +
          Read(curve.x10_TranslationKeies[pos + 1].x04_DT_VECTOR3D) * k1);
      } else {
        bone->translate(Read(curve.x10_TranslationKeies[pos].x04_DT_VECTOR3D));
      }
    }
    for (uint32 i = 0; i < perm.x0B0_RotationCurves.size(); ++i) {
      auto& curve = perm.x0B0_RotationCurves[i];
      if (i >= boneRef.size() || !boneRef[i] || !curve.x10_RotationKeies.size()) continue;
      Bone* bone = boneRef[i];
      uint32 pos = findKey(curve.x10_RotationKeies, frame);
      if (pos < curve.x10_RotationKeies.size() - 1) {
        bone->rotate(Quaternion::slerp(
          Read(curve.x10_RotationKeies[pos].x04_Quaternion16),
          Read(curve.x10_RotationKeies[pos + 1].x04_Quaternion16), time));
      } else {
        bone->rotate(Read(curve.x10_RotationKeies[pos].x04_Quaternion16));
      }
    }
    for (uint32 i = 0; i < perm.x0C0_ScaleCurves.size(); ++i) {
      auto& curve = perm.x0C0_ScaleCurves[i];
      if (i >= boneRef.size() || !boneRef[i] || !curve.x10_ScaleKeies.size()) continue;
      Bone* bone = boneRef[i];
      uint32 pos = findKey(curve.x10_ScaleKeies, frame);
      if (pos < curve.x10_ScaleKeies.size() - 1) {
        bone->scale(curve.x10_ScaleKeies[pos].x04 * k0 + curve.x10_ScaleKeies[pos + 1].x04 * k1);
      } else {
        bone->scale(curve.x10_ScaleKeies[pos].x04);
      }
    }

    for (auto& bone : bones) {
      bone.finish();
    }
  }

  struct Texture {
    Image image;
    uint32 id;
  };
  std::map<uint32, Texture> textures;
  uint32 getTexture(uint32 id) {
    auto it = textures.find(id);
    if (it != textures.end()) return it->second.id;
    auto& tex = textures[id];
    tex.image = GameTextures::get(id);
    //if (tex.image) tex.image.write(fmtstring("Tex/%s.png", Textures::name(id)));
    return tex.id = (tex.image ? genTexture(tex.image) : -1);
  }

  void render() {
    uint32 curTime = GetTickCount();
    if (!paused) animCounter += curTime - prevTime;
    prevTime = curTime;
    update(animCounter * 0.001f);
    auto& object = app.x010_Structure.x088_GeoSets[0].x10_SubObjects[index];
    Appearance::Type::SubObjectAppearance* mat = nullptr;
    for (auto entry : app.x1B0_AppearanceMaterials) {
      if (!strcmp(entry.x00_Text, object.x05C_Text) && entry.x88_SubObjectAppearances.size() > appearance) {
        mat = &entry.x88_SubObjectAppearances[appearance];
        break;
      }
    }

    glPushMatrix();
    glTranslatef(-center.x, -center.y, -center.z);

    uint32 tex = (mat && mat->x18_UberMaterial.x58_MaterialTextureEntries.size()
      ? getTexture(mat->x18_UberMaterial.x58_MaterialTextureEntries[0].x08_MaterialTexture.x00_TexturesSno)
      : -1);
    if (tex != -1) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex);
    }

    Matrix transform;
    glGetFloatv(GL_MODELVIEW_MATRIX, transform);
    transform.transpose();

    std::vector<Vertex> vertices;
    vertices.reserve(object.x010_FatVertexs.size());
    for (uint32 i = 0; i < object.x010_FatVertexs.size(); ++i) {
      vertices.emplace_back(object.x010_FatVertexs[i]);
      auto& v = vertices.back();
      Matrix mat;
      float remains = 1.0f;
      for (uint32 j = 0; j < 3; ++j) {
        auto& inf = object.x020_VertInfluences[i].x00_Influences[j];
        if (inf.x04) {
          mat += bones[inf.x00].curTransform * inf.x04;
          remains -= inf.x04;
        }
      }
      if (remains < 1.0f - 1e-6f) {
        if (remains > 1e-6f) {
          mat += Matrix::identity() * remains;
        }
        v.transform(mat);
      }
    }

    std::vector<Triangle> triangles;
    for (size_t i = 0; i < object.x038_short.size(); i += 3) {
      triangles.emplace_back(
        object.x038_short[i + 0],
        object.x038_short[i + 1],
        object.x038_short[i + 2]);
      auto& tri = triangles.back();
      tri.z = (transform * (vertices[tri.a].pos + vertices[tri.b].pos + vertices[tri.c].pos) / 3).z;
    }
    std::sort(triangles.begin(), triangles.end(), Triangle::zsort);
    glBegin(GL_TRIANGLES);
    for (auto& tri : triangles) {
      vertices[tri.a].draw();
      vertices[tri.b].draw();
      vertices[tri.c].draw();
    }
    glEnd();
    if (tex) {
      glDisable(GL_TEXTURE_2D);
    }

    if (showBones) {
      beginTrackables();
      for (Bone& bone : bones) {
        trackable(bone.curTransform * bone.pos, 0.1f, bone.name.c_str());
      }
      endTrackables();
    }

    glPopMatrix();

    startOrtho();
    glColor3d(1, 1, 1);
    text(10, 30, fmtstring("Object: %s", object.x0DC_Text).c_str());
    text(10, 50, fmtstring("Appearance: %s", app.x1C0_AppearanceLooks[appearance].x00_Text).c_str());
    //text(10, 70, fmtstring("Polys: %d to %d", origSize, newSize).c_str());
    endOrtho();
  }
};

void Render(SnoFile<Appearance> const& app, SnoFile<Anim> const& anim) {
  Model gl(app, anim);
  gl.loop();
}
