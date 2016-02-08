#include "model.h"
#include "frameui/searchlist.h"
#include "types/Actor.h"
#include "types/AnimSet.h"
#include "types/Monster.h"
#include "strings.h"
#include <set>

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

Model::Model(char const* name)
  : app_(name)
  , appearances_(app_->x010_Structure.x088_GeoSets[0].x10_SubObjects.size(), -1)
{
  int count = 0;
  for (auto& object : app_->x010_Structure.x088_GeoSets[0].x10_SubObjects) {
    for (auto& v : object.x010_FatVertexs) {
      center_ += VertexPos(v);
    }
    count += object.x010_FatVertexs.size();
  }
  center_ /= float(count);
  //File ff(fmtstring("%s.txt", name), "w");
  //for (auto& name : app_->x010_Structure.x088_GeoSets[0].x10_SubObjects) {
  //  ff.printf("%s %s\n", name.x05C_Text, name.x0DC_Text);
  //}
  //File ff2(fmtstring("app.txt", name), "w");
  //for (auto& name : app_->x1C0_AppearanceLooks) {
  //  ff2.printf("%d %s\n", HashName(name.x00_Text), name.x00_Text);
  //}
  //ff = File(fmtstring("hardpoints.txt", name), "w");
  //for (auto& name : app_->x010_Structure.x0F0_Hardpoints) {
  //  ff.printf("%s\n", name.x00_Text);
  //}

  bones_.resize(app_->x010_Structure.x010_BoneStructures.size());
  for (size_t i = 0; i < bones_.size(); ++i) {
    auto& src = app_->x010_Structure.x010_BoneStructures[i];
    auto& dst = bones_[i];
    dst.name = src.x000_Text;
    boneMap_[dst.name] = &dst;
    if (src.x040 != -1) dst.parent = &bones_[src.x040];
    dst.pos = Read(src.x06C_PRSTransform.x10_DT_VECTOR3D);
    dst.curTransform = Matrix::translate(dst.pos) *
      Read(src.x06C_PRSTransform.x00_Quaternion).matrix() *
      Matrix::scale(src.x06C_PRSTransform.x1C);
    dst.invTransform = dst.curTransform.inverse();
    dst.finalTransform = Matrix::identity();
    if (src.x118_CollisionShapes.size()) {
      dst.capsule = true;
      dst.capA = Read(src.x118_CollisionShapes[0].x30_DT_VECTOR3D);
      dst.capB = Read(src.x118_CollisionShapes[0].x3C_DT_VECTOR3D);
      dst.capR = src.x118_CollisionShapes[0].x48;
    }
  }

  for (auto& src : app_->x010_Structure.x0F0_Hardpoints) {
    auto& dst = hardpoints_[src.x00_Text];
    dst.name = src.x00_Text;
    if (src.x40 != -1) dst.parent = &bones_[src.x40];
    dst.pos = Read(src.x44_PRTransform.x10_DT_VECTOR3D);
    dst.curTransform = Matrix::translate(dst.pos) *
      Read(src.x44_PRTransform.x00_Quaternion).matrix();
    dst.invTransform = Matrix::identity();
    if (dst.parent) dst.invTransform = dst.parent->invTransform;
    dst.finalTransform = dst.curTransform;
  }
}

void Model::restore(json::Value const& value) {
  auto const& src = value["appearances"];
  for (size_t i = 0; i < src.length() && i < appearances_.size(); ++i) {
    appearances_[i] = src[i].getInteger();
  }
  if (attachments_.size() && value.has("attachments")) {
    auto const& src = value["attachments"];
    for (auto& att : attachments_) {
      if (src.has(att.bone->name)) {
        att.model->restore(src[att.bone->name]);
      }
    }
  }
}
void Model::store(json::Value& value) const {
  auto& dst = value["appearances"];
  for (uint32 app : appearances_) {
    dst.append(int(app));
  }
  if (attachments_.size()) {
    auto& dst = value["attachments"];
    for (auto const& att : attachments_) {
      att.model->store(dst[att.bone->name]);
    }
  }
}

void Model::setAnimation(char const* name) {
  animation_.reset(name ? new Animation(name) : nullptr);
}

void Model::attach(char const* hardpoint, Model* model) {
  auto it = hardpoints_.find(hardpoint);
  if (it != hardpoints_.end()) {
    attachments_.emplace_back(&it->second, model);
  } else {
    delete model;
  }
}

uint32 Model::getTexture(GL& gl, uint32 id) {
  auto it = textures_.find(id);
  if (it != textures_.end()) return it->second.id;
  auto& tex = textures_[id];
  tex.image = GameTextures::get(id);
  //if (tex.image) tex.image.write(fmtstring("Tex/%s.png", Textures::name(id)));
  return tex.id = (tex.image ? gl.genTexture(tex.image) : -1);
}

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

void Model::render(GL& gl, float time, bool showBones) {
  Matrix transform;
  glGetFloatv(GL_MODELVIEW_MATRIX, transform);
  transform.transpose();

  if (animation_) {
    for (auto& bone : bones_) {
      bone.reset();
    }
    animation_->update(time, boneMap_);
    for (auto& bone : bones_) {
      bone.finish();
    }
    for (auto& hp : hardpoints_) {
      hp.second.finish_hp();
    }
  }

  std::vector<uint32> order;
  for (uint32 i = 0; i < appearances_.size(); ++i) order.push_back(i);
  if (appearances_.size() > 10) {
    std::reverse(order.begin(), order.end());
  }

  for (uint32 subObject : order) {
    if (appearances_[subObject] == -1) continue;
    auto& object = app_->x010_Structure.x088_GeoSets[0].x10_SubObjects[subObject];
    Appearance::Type::SubObjectAppearance* mat = nullptr;
    for (auto entry : app_->x1B0_AppearanceMaterials) {
      if (!strcmp(entry.x00_Text, object.x05C_Text) &&
          entry.x88_SubObjectAppearances.size() > appearances_[subObject])
      {
        mat = &entry.x88_SubObjectAppearances[appearances_[subObject]];
        break;
      }
    }

    float alpha = 1.0f;
    if (mat) {
      for (size_t i = 0; i < mat->x10_TagMap.size() - 1; ++i) {
        if (mat->x10_TagMap[i] == 196864) {
          alpha = *(float*) &mat->x10_TagMap[i + 1];
        }
      }
    }
    glColor4f(1, 1, 1, alpha);

    uint32 tex = (mat && mat->x18_UberMaterial.x58_MaterialTextureEntries.size()
      ? getTexture(gl, mat->x18_UberMaterial.x58_MaterialTextureEntries[0].x08_MaterialTexture.x00_TexturesSno)
      : -1);
    if (tex != -1) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex);
    }

    std::vector<Vertex> vertices;
    vertices.reserve(object.x010_FatVertexs.size());
    for (uint32 i = 0; i < object.x010_FatVertexs.size(); ++i) {
      vertices.emplace_back(object.x010_FatVertexs[i]);
    }

    if (animation_) {
      for (size_t i = 0; i < vertices.size(); ++i) {
        Matrix mat;
        for (auto& inf : object.x020_VertInfluences[i].x00_Influences) {
          if (inf.x04) {
            mat += bones_[inf.x00].finalTransform * inf.x04;
          }
        }
        vertices[i].transform(mat);
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
  }

  for (auto& att : attachments_) {
    glPushMatrix();
    glMultMatrixf(att.bone->finalTransform.transposed());
    att.model->render(gl, time, showBones);
    glPopMatrix();
  }

  if (showBones) {
    gl.beginTrackables();
    for (Bone& bone : bones_) {
      gl.trackable(bone.finalTransform * bone.pos, 0.1f, bone.name.c_str());
    }
    gl.endTrackables();
    for (Bone& bone : bones_) {
      size_t index = &bone - &bones_[0];
      auto& params = app_->x010_Structure.x010_BoneStructures[index].x128_ConstraintParameters;
      if (params.size() && bone.parent) {
        auto& data = params[0];
        auto& pr1 = data.x050_PRTransform;
        auto& pr2 = data.x078_PRTransform;
        auto& pr3 = data.x094_PRTransform;
        Matrix mat1 = /*bone.parent->curTransform * */Matrix::translate(Read(pr1.x10_DT_VECTOR3D)) * Read(pr1.x00_Quaternion).matrix();
        Matrix mat2 = bone.parent->curTransform * Matrix::translate(Read(pr2.x10_DT_VECTOR3D)) * Read(pr2.x00_Quaternion).matrix();
        Matrix mat3 = bone.curTransform * Matrix::translate(Read(pr3.x10_DT_VECTOR3D)) * Read(pr3.x00_Quaternion).matrix();
        Matrix mat;
        Vector v0(0, 0, 0), vx(1, 0, 0), vy(0, 1, 0), vz(0, 0, 1);
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        if (GetTickCount() % 3000 < 1000) {
          mat = mat1;
          glColor3d(1, 0, 0);
        } else if (GetTickCount() % 3000 < 2000) {
          mat = mat2;
          glColor3d(0, 1, 0);
        } else {
          mat = mat3;
          glColor3d(1, 1, 1);
        }
        glVertex3fv(mat * v0); glVertex3fv(mat * vx);
        glVertex3fv(mat * v0); glVertex3fv(mat * vy);
        glVertex3fv(mat * v0); glVertex3fv(mat * vz);
        glEnd();
        glColor3f(1, 1, 1);
        gl.text(mat * vx, "x");
        gl.text(mat * vy, "y");
        gl.text(mat * vz, "z");
        glPopAttrib();
      }
      if (bone.capsule) gl.capsule(bone.curTransform * bone.capA, bone.curTransform * bone.capB, bone.capR);
    }
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

void Apply(Model::Bone* bone, Anim::Type::TranslationKey const& key1,
  Anim::Type::TranslationKey const& key2, float t)
{
  bone->translate(Read(key1.x04_DT_VECTOR3D) * (1 - t) + Read(key2.x04_DT_VECTOR3D) * t);
}
void Apply(Model::Bone* bone, Anim::Type::RotationKey const& key1,
  Anim::Type::RotationKey const& key2, float t)
{
  bone->rotate(Quaternion::slerp(Read(key1.x04_Quaternion16), Read(key2.x04_Quaternion16), t));
}
void Apply(Model::Bone* bone, Anim::Type::ScaleKey const& key1,
  Anim::Type::ScaleKey const& key2, float t)
{
  bone->scale(key1.x04 * (1 - t) + key2.x04 * t);
}

template<class T>
void Apply(Model::Bone* bone, Array<T> const& keys, float frame) {
  uint32 pos = findKey(keys, static_cast<uint32>(frame));
  if (pos < keys.size() - 1) {
    Apply(bone, keys[pos], keys[pos + 1], (frame - keys[pos].x00) / (keys[pos + 1].x00 - keys[pos].x00));
  } else {
    Apply(bone, keys[pos], keys[pos], 0);
  }
}

void Animation::update(float time, Map<Model::Bone*>& bones) {
  auto& perm = anim_->x28_AnimPermutations[permutation_];
  float frame = time * 60.0f * perm.x048_Velocity;
  uint32 base = (static_cast<uint32>(frame) / (perm.x090 - 1)) * (perm.x090 - 1);
  frame -= static_cast<float>(base);

  std::vector<Model::Bone*> boneRef;
  for (uint32 i = 0; i < perm.x088_BoneNames.size(); ++i) {
    auto it = bones.find(perm.x088_BoneNames[i].x00_Text);
    boneRef.push_back(it == bones.end() ? nullptr : it->second);
  }

  for (uint32 i = 0; i < perm.x0A0_TranslationCurves.size(); ++i) {
    auto& curve = perm.x0A0_TranslationCurves[i];
    if (i >= boneRef.size() || !boneRef[i] || !curve.x10_TranslationKeies.size()) continue;
    Apply(boneRef[i], curve.x10_TranslationKeies, frame);
  }
  for (uint32 i = 0; i < perm.x0B0_RotationCurves.size(); ++i) {
    auto& curve = perm.x0B0_RotationCurves[i];
    if (i >= boneRef.size() || !boneRef[i] || !curve.x10_RotationKeies.size()) continue;
    Apply(boneRef[i], curve.x10_RotationKeies, frame);
  }
  for (uint32 i = 0; i < perm.x0C0_ScaleCurves.size(); ++i) {
    auto& curve = perm.x0C0_ScaleCurves[i];
    if (i >= boneRef.size() || !boneRef[i] || !curve.x10_ScaleKeies.size()) continue;
    Apply(boneRef[i], curve.x10_ScaleKeies, frame);
  }
}

class ModelViewer : public GL {
public:
  ModelViewer(Frame* parent, Model& model)
    : GL(parent)
    , model_(model)
    , prevTime_(GetTickCount())
    , animCounter_(0)
    , subObject_(0)
    , paused_(false)
    , showBones_(false)
  {
  }

  void onKey(int vk) {
    switch (vk) {
    case VK_LEFT:
      --subObject_;
      if (subObject_ == -1) subObject_ = model_.numSubObjects() - 1;
      break;
    case VK_RIGHT:
      ++subObject_;
      if (subObject_ >= model_.numSubObjects()) subObject_ = 0;
      break;
    case VK_DOWN:
      model_.prevAppearance(subObject_);
      break;
    case VK_UP:
      model_.nextAppearance(subObject_);
      break;
    case VK_SPACE:
      paused_ = !paused_;
      break;
    case 'B':
      showBones_ = !showBones_;
      break;
    }
  }

  void doRender() {
    uint32 curTime = GetTickCount();
    if (!paused_) animCounter_ += curTime - prevTime_;
    prevTime_ = curTime;

    glPushMatrix();
    Vector center = model_.center();
    glTranslatef(-center.x, -center.y, -center.z);

    model_.render(*this, animCounter_ * 0.001f, showBones_);

    glPopMatrix();

    startOrtho();
    glColor3d(1, 1, 1);
    text(10, 30, fmtstring("Object: %s", model_.subObjectName(subObject_)).c_str());
    text(10, 50, fmtstring("Appearance: %s", model_.appearanceName(model_.appearance(subObject_))).c_str());
    //text(10, 70, fmtstring("Polys: %d to %d", origSize, newSize).c_str());
    endOrtho();
  }
private:
  Model& model_;
  uint32 prevTime_;
  uint32 animCounter_;
  uint32 subObject_;
  bool paused_;
  bool showBones_;
};

class SimpleModelViewer : public RootWindow {
  ModelViewer* viewer;
public:
  SimpleModelViewer(Model& model) {
    if (WNDCLASSEX* wcx = createclass("MainWndClass")) {
      wcx->hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
      wcx->hCursor = LoadCursor(NULL, IDC_ARROW);
      RegisterClassEx(wcx);
    }
    create(CW_USEDEFAULT, 0, 400, 700, "Model Viewer",
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0);

    viewer = new ModelViewer(this, model);
    viewer->setAllPoints();
  }

  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override {
    if (message == WM_KEYDOWN) {
      viewer->onKey(wParam);
      return 0;
    }
    if (message == WM_MOUSEWHEEL) {
      SetFocus(viewer->getHandle());
      return 0;
    }
    return M_UNHANDLED;
  }

  void loop() {
    ShowWindow(hWnd, SW_SHOW);
    while (hWnd) {
      MSG msg;
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
          DestroyWindow(hWnd);
        }
      }
      viewer->render();
    }
  }
};

class ModelViewer2 : public GL {
public:
  ModelViewer2(Frame* parent)
    : GL(parent)
    , model_(nullptr)
    , prevTime_(GetTickCount())
    , animCounter_(0)
    , paused_(false)
    , showBones_(false)
  {
  }
  ~ModelViewer2() {
    delete model_;
  }

  Model* model() const {
    return model_;
  }
  void setModel(char const* name) {
    if (model_) delete model_;
    model_ = nullptr;
    if (name) model_ = new Model(name);
    if (model_ && model_->numAppearances() && model_->numSubObjects() == 1) {
      model_->appearance(0, 0);
    }
  }

  void onKey(int vk) {
    switch (vk) {
    case VK_SPACE:
      paused_ = !paused_;
      break;
    case 'B':
      showBones_ = !showBones_;
      break;
    }
  }

  void doRender() {
    uint32 curTime = GetTickCount();
    if (!paused_) animCounter_ += curTime - prevTime_;
    prevTime_ = curTime;

    if (model_) {
      glPushMatrix();
      Vector center = model_->center();
      glTranslatef(-center.x, -center.y, -center.z);
      model_->render(*this, animCounter_ * 0.001f, showBones_);
      glPopMatrix();
    }
  }
private:
  Model* model_;
  uint32 prevTime_;
  uint32 animCounter_;
  uint32 subObject_;
  bool paused_;
  bool showBones_;
};

class SuperModelViewer : public RootWindow {
  ModelViewer2* viewer;
  ComboFrame* mode;
  EditFrame* actorName;
  SearchList* actors;
  EditFrame* animName;
  SearchList* anims;
  OptionList* objects;
  ColorFrame* bgcolor;
  ButtonFrame* capBtn;
  EditFrame* capSize;
public:
  SuperModelViewer() {
    if (WNDCLASSEX* wcx = createclass("MainWndClass")) {
      wcx->hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
      wcx->hCursor = LoadCursor(NULL, IDC_ARROW);
      RegisterClassEx(wcx);
    }
    create(CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, "Model Viewer",
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0);

    mode = new ComboFrame(this, 99);
    mode->setPoint(PT_TOPLEFT, 0, 0);
    mode->setWidth(300);
    mode->addString("All actors", 0);
    mode->addString("Monster names", 1);
    mode->addString("All appearances", 2);
    mode->setCurSel(0);

    actorName = new EditFrame(this, 100);
    actorName->setPoint(PT_TOPLEFT, mode, PT_BOTTOMLEFT, 0, 4);
    actorName->setSize(300, 21);
    actors = new SearchList(this, 101);
    actors->setPoint(PT_TOPLEFT, actorName, PT_BOTTOMLEFT, 0, 4);
    actors->setPoint(PT_TOPRIGHT, actorName, PT_BOTTOMRIGHT, 0, 4);
    actors->setPointEx(PT_BOTTOM, 0, 0.38, 0, 0);

    animName = new EditFrame(this, 102);
    animName->setPoint(PT_TOPLEFT, actors, PT_BOTTOMLEFT, 0, 4);
    animName->setSize(300, 21);
    anims = new SearchList(this, 103);
    anims->setPoint(PT_TOPLEFT, animName, PT_BOTTOMLEFT, 0, 4);
    anims->setPoint(PT_TOPRIGHT, animName, PT_BOTTOMRIGHT, 0, 4);
    anims->setPointEx(PT_BOTTOM, 0, 0.6, 0, 0);

    objects = new OptionList(this, 104);
    objects->setPoint(PT_TOPLEFT, anims, PT_BOTTOMLEFT, 0, 4);

    bgcolor = new ColorFrame(0x000000, this, 105);
    bgcolor->setPoint(PT_BOTTOMLEFT, 0, 0);
    bgcolor->setSize(300, 21);
    bgcolor->setText("Background");

    capSize = new EditFrame(this, 98, ES_NUMBER | ES_AUTOHSCROLL);
    capSize->setText("256");
    capBtn = new ButtonFrame("Save", this, 97);
    capSize->setSize(160, 21);
    capBtn->setSize(136, 21);
    capSize->setPoint(PT_BOTTOMLEFT, bgcolor, PT_TOPLEFT, 0, -4);
    capBtn->setPoint(PT_BOTTOMRIGHT, bgcolor, PT_TOPRIGHT, 0, -4);

    objects->setPoint(PT_BOTTOMRIGHT, capBtn, PT_TOPRIGHT, 0, -4);

    viewer = new ModelViewer2(this);
    viewer->setPoint(PT_TOPLEFT, actorName, PT_TOPRIGHT, 4, 0);
    viewer->setPoint(PT_BOTTOMRIGHT, 0, 0);

    listActors();
    for (auto& ani : SnoManager::get<Anim>().get()) {
      anims->insertEx(ani.first, ani.second);
    }
    anims->sortEx();
  }

  void listActors() {
    int m = mode->getCurSel();
    actors->clear();
    if (m == 2) {
      for (auto& acr : SnoManager::get<Appearance>().get()) {
        actors->insert(acr.first, acr.second);
      }
    } else if (m == 1) {
      auto stl = Strings::list("Monsters");
      for (auto& mon : SnoLoader::All<Monster>()) {
        uint32 id = mon->x010_ActorSno;
        std::string name = mon.name();
        if (stl.has(name)) name = stl[name];
        if (!name.empty()) actors->insert(id, name);
      }
    } else {
      for (auto& acr : SnoManager::get<Actor>().get()) {
        actors->insert(acr.first, acr.second);
      }
    }
    actors->sort();
    actors->update();
  }

  void setActor(uint32 id) {
    uint32 appId = 0;
    anims->clear();
    objects->clear();
    viewer->setModel(nullptr);
    if (mode->getCurSel() != 2) {
      SnoFile<Actor> actor(Actor::name(id));
      if (actor) {
        SnoFile<AnimSet> animSet(AnimSet::name(actor->x068_AnimSetSno));
        if (animSet) {
          std::set<uint32> ids;
          for (auto& tm : animSet->x010_AnimSetTagMaps) {
            for (uint32 i = 0; i < tm.x08_TagMap[0]; ++i) {
              ids.insert(tm.x08_TagMap[i * 3 + 3]);
            }
          }
          for (uint32 id : ids) {
            char const* name = Anim::name(id);
            if (name) {
              anims->insert(id, name);
            }
          }
          anims->sort();
        }
        appId = actor->x014_AppearanceSno;
      }
    } else {
      appId = id;
    }
    viewer->setModel(Appearance::name(appId));
    Model* model = viewer->model();
    if (model) {
      for (uint32 i = 0; i < model->numSubObjects(); ++i) {
        objects->addItem(model->subObjectName(i));
      }
      for (uint32 i = 0; i < model->numAppearances(); ++i) {
        objects->addOption(model->appearanceName(i));
      }
    }
    anims->update();
    objects->update();
  }

  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override {
    int id;
    switch (message) {
    case WM_COMMAND:
      id = LOWORD(wParam);
      if (id == 99 && HIWORD(wParam) == CBN_SELCHANGE) {
        listActors();
      } else if (id == 100 && HIWORD(wParam) == EN_CHANGE) {
        actors->search(actorName->getText());
      } else if (id == 101) {
        int sel = actors->selected();
        setActor(sel >= 0 ? actors->itemdata(sel) : 0);
      } else if (id == 103) {
        int sel = anims->selected();
        char const* name = Anim::name(sel >= 0 ? anims->itemdata(sel) : 0);
        if (viewer->model()) {
          viewer->model()->setAnimation(name);
        }
      } else if (id == 102 && HIWORD(wParam) == EN_CHANGE) {
        anims->search(animName->getText());
      } else if (id == 105) {
        viewer->setColor(bgcolor->getColor());
      } else if (id == 97 && HIWORD(wParam) == BN_CLICKED) {
        Image img = viewer->capture();
        static char path[512];
        path[0] = 0;
        OPENFILENAME ofn;
        memset(&ofn, 0, sizeof ofn);
        ofn.lStructSize = sizeof ofn;
        ofn.hwndOwner = hWnd;
        ofn.lpstrFilter = "PNG files (*.png)\0*.png\0All Files\0*\0\0";
        ofn.lpstrFile = path;
        ofn.nMaxFile = 512;
        ofn.lpstrDefExt = "png";
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
        if (!GetSaveFileName(&ofn)) return 0;
        int size = atoi(capSize->getText().c_str());
        if (!size) size = 256;
        int osize = std::min(img.width(), img.height());
        Image dst(osize, osize);
        dst.blt(int(osize - img.width()) / 2, int(osize - img.height()) / 2, img);
        dst.resize(size, size).write(path, ImageFormat::PNG);
      }
      return 0;
    case WM_OPTIONCHANGE:
      if (viewer->model()) {
        viewer->model()->appearance(lParam, objects->getOption(lParam));
      }
      return 0;
    case WM_KEYDOWN:
      viewer->onKey(wParam);
      return 0;
    case WM_MOUSEWHEEL:
      SetFocus(viewer->getHandle());
      return 0;
    }
    return M_UNHANDLED;
  }

  void loop() {
    ShowWindow(hWnd, SW_SHOW);
    while (hWnd) {
      MSG msg;
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
          DestroyWindow(hWnd);
        }
      }
      viewer->render();
    }
  }
};

void ViewModel(Model& model) {
  SimpleModelViewer viewer(model);
  viewer.loop();
}
void ViewModels() {
  SuperModelViewer viewer;
  viewer.loop();
}
