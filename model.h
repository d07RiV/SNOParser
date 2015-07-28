// model.h
//
// model viewer
//
// class Model
//   Model(char const* name) - create model from Appearance file
//   void appearance(uint32 subObject, uint32 index) - enable subObject with certain appearance material
//     (Appearance files consist of multiple sub objects, each object can be displayed with different materials)
//   void setAnimation(char const* name) - load animation from Anim file
//
// void ViewModel(Model& model)
//   run the model viewer

#pragma once
#include "types/Appearance.h"
#include "types/Anim.h"
#include <memory>
#include "gl.h"
#include "textures.h"

class Animation;

class Model {
public:
  Model(char const* name);

  struct Bone {
    std::string name;
    Bone* parent = nullptr;
    Vector pos;

    Matrix invTransform;
    Matrix curTransform;
    Matrix finalTransform;

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
      if (parent) curTransform = parent->curTransform * curTransform;
      finalTransform = curTransform * invTransform;
    }
    void finish_hp() {
      finalTransform = invTransform * curTransform;
      if (parent) finalTransform = parent->curTransform * finalTransform;
    }
  };

  char const* name() const {
    return app_.name().c_str();
  }
  Vector const& center() const {
    return center_;
  }

  uint32 numAppearances() const {
    return app_->x1C0_AppearanceLooks.size();
  }
  char const* appearanceName(uint32 index) const {
    return (index == -1 ? "None" : app_->x1C0_AppearanceLooks[index].x00_Text);
  }
  uint32 numSubObjects() const {
    return app_->x010_Structure.x088_GeoSets[0].x10_SubObjects.size();
  }
  char const* subObjectName(uint32 index) const {
    return app_->x010_Structure.x088_GeoSets[0].x10_SubObjects[index].x0DC_Text;
  }
  uint32 subObjectType(uint32 index) const {
    return app_->x010_Structure.x088_GeoSets[0].x10_SubObjects[index].x000_ObjectType;
  }

  uint32 appearance(uint32 subObject) const {
    return appearances_[subObject];
  }
  void appearance(uint32 subObject, uint32 index) {
    appearances_[subObject] = index;
  }
  void nextAppearance(uint32 subObject) {
    ++appearances_[subObject];
    if (appearances_[subObject] >= app_->x1C0_AppearanceLooks.size()) {
      appearances_[subObject] = -1;
    }
  }
  void prevAppearance(uint32 subObject) {
    --appearances_[subObject];
    if (appearances_[subObject] == -2) {
      appearances_[subObject] = app_->x1C0_AppearanceLooks.size() - 1;
    }
  }

  void setAnimation(Animation* anim) {
    animation_.reset(anim);
  }
  void setAnimation(char const* name);
  Animation* animation() {
    return animation_.get();
  }

  void attach(char const* hardpoint, Model* model);
  Model* attach(char const* hardpoint, char const* name) {
    Model* model = new Model(name);
    attach(hardpoint, model);
    return model;
  }

  void restore(json::Value const& value);
  void store(json::Value& value) const;

  void render(GL& gl, float time, bool showBones);

private:
  SnoFile<Appearance> app_;
  std::unique_ptr<Animation> animation_;
  std::vector<Bone> bones_;
  Map<Bone*> boneMap_;
  Map<Bone> hardpoints_;
  std::vector<uint32> appearances_;
  struct Texture {
    Image image;
    uint32 id;
  };
  Vector center_;
  struct Attachment {
    Bone* bone;
    std::unique_ptr<Model> model;
    Attachment() {}
    Attachment(Attachment&& att)
      : bone(att.bone)
      , model(std::move(att.model))
    {}
    Attachment(Bone* bone, Model* model)
      : bone(bone)
      , model(model)
    {
    }
  };
  std::vector<Attachment> attachments_;
  std::map<uint32, Texture> textures_;
  uint32 getTexture(GL& gl, uint32 id);
};

class Animation {
public:
  Animation(char const* name)
    : anim_(name)
    , permutation_(0)
  {}

  uint32 numPermutations() const {
    return anim_->x28_AnimPermutations.size();
  }
  void permutation(uint32 index) {
    permutation_ = index;
  }
  uint32 permutation() {
    return permutation_;
  }

  void update(float time, Map<Model::Bone*>& bones);

private:
  SnoFile<Anim> anim_;
  uint32 permutation_;
};

void ViewModel(Model& model);
