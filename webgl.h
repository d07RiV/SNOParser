// webgl.h
//
// dump character/item models, textures and animations into binary archives

#pragma once
#include "common.h"
#include "math3d.h"
#include "types/Appearance.h"
#include "types/Anim.h"

namespace WebGL {
#pragma pack(push, 1)
  struct ModelHeader {
    uint32 numBones;
    uint32 boneOffset;
    uint32 numHardpoints;
    uint32 hardpointOffset;
    uint32 numObjects;
    uint32 objectOffset;
    uint32 numMaterials;
    uint32 numAppearances;
    uint32 materialOffset;
    Vector center;
  };
  struct PRSTransform {
    Quaternion rotate;
    Vector translate;
    float scale;
  };
  struct PRTransform {
    Quaternion rotate;
    Vector translate;
  };
  struct CapsuleInfo {
    Vector start, end;
    float radius;
  };
  struct Constraint {
    PRTransform parent;
    PRTransform local;
    float angles[5];
  };
  struct Bone {
    char name[64];
    uint32 parent;
    PRSTransform transform;
    uint32 capsuleOffset;
    uint32 constraintOffset;
  };
  struct Hardpoint {
    char name[64];
    uint32 parent;
    Matrix transform;
  };
  struct Material {
    uint32 diffuse;
    uint32 specular;
    uint32 tintBase;
    uint32 tintMask;
  };
  struct Object {
    uint32 material;
    uint32 numGroups;
    uint32 groupOffset;
  };
  struct ObjectGroup {
    uint32 numBones;
    uint32 boneOffset;
    uint32 numVertices;
    uint32 vertexOffset;
    uint32 numIndices;
    uint32 indexOffset;
  };
  struct Vertex {
    Vector position;
    int8 normal[4];
    int16 texcoord[2];
    uint8 bone_idx[4];
    float bone_weight[3];
  };
  typedef uint32 BoneIndex;
  typedef uint16 Index;

  struct TranslationKey {
    uint32 frame;
    Vector value;
  };
  struct RotationKey {
    uint32 frame;
    Quaternion value;
  };
  struct ScaleKey {
    uint32 frame;
    float value;
  };
  struct AnimationCurve {
    char bone[64];
    uint32 numTranslations;
    uint32 translationOffset;
    uint32 numRotations;
    uint32 rotationOffset;
    uint32 numScales;
    uint32 scaleOffset;
  };
  struct AnimationSequence {
    uint32 numFrames;
    float velocity;
    uint32 numBones;
    uint32 animationOffset;
  };
#pragma pack(pop)

  void WriteModel(std::string const& name);
  void WriteAnimation(std::string const& name);

  /*******************************

    to generate models/item data:
    build in 64 bit!!!
    AllItems(true, true, false)

    required files:

    itemtypes.js:
    "itemById.*.type",
    "itemTypes.*.slot",
    "itemTypes.*.generic",
    "legendaryGems.*.id",
    "gemQualities",
    "gemColors.*.id",

    extraitems.js:
    id: {type: "type"} for promo items

    output:
    textures.wgz
    models.wgz
    animations.wgz
    webgl_actors.js
    webgl_items.js

   *******************************/

  void AllItems(bool models, bool info, bool load);
  void AnimSets();
  void ItemInfo();
  void ClassInfo();
  void GenericItems();

  void AddPhysics();
}
