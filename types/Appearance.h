#pragma once
#include "snotypes.h"
#include "serialize.h"
#include "snocommon.h"

#pragma pack(push, 1)

declstruct(Appearance::Type) {
  declstruct(DT_VECTOR3D) {
    float x00_X;
    float x04_Y;
    float x08_Z;
    void dumpfunc() {
      dumpval(x00_X, x04_Y, x08_Z);
    }
  };
  declstruct(DT_NORMAL3D) {
    float8 x00_X;
    float8 x01_Y;
    float8 x02_Z;
    float8 x03_;
    void dumpfunc() {
      dumpval(x00_X, x01_Y, x02_Z);
    }
  };
  struct TexCoordValue {
    uint16 raw;
    operator uint16() const {
      return raw;
    }
    float value() const {
      return (float(raw) - 32768.0f) / 512.0f;
    }
    void serialize(json::Visitor* visitor) {
      visitor->onNumber(value());
    }
  };
  declstruct(DT_TEXCOORD) {
    TexCoordValue x00_U;
    TexCoordValue x02_V;
    void dumpfunc() {
      dumpval(x00_U, x02_V);
    }
  };

  declstruct(AABB) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D);
    }
  };

  declstruct(BonePulseData) {
    char x00_Text[64];
    float x40;
    float x44;
    float x48_Angle;
    void dumpfunc() {
      dumpval(x00_Text, x40, x44, x48_Angle);
    }
  };

  declstruct(dmFloat3) {
    float x00;
    float x04;
    float x08;
    void dumpfunc() {
      dumpval(x00, x04, x08);
    }
  };

  declstruct(dmPlane) {
    dmFloat3 x00_dmFloat3;
    float x0C;
    void dumpfunc() {
      dumpval(x00_dmFloat3, x0C);
    }
  };

  declstruct(dmSubEdge) {
    uint8 x00;
    uint8 x01;
    uint8 x02;
    uint8 x03;
    void dumpfunc() {
      dumpval(x00, x01, x02, x03);
    }
  };

  declstruct(PolytopeData) {
    Array<dmFloat3> x00_dmFloat3s = x40_SerializeData;
    Array<dmPlane> x08_dmPlanes = x48_SerializeData;
    Array<dmSubEdge> x10_dmSubEdges = x50_SerializeData;
    Array<uint8> x18_byte = x58_SerializeData;
    dmFloat3 x20_dmFloat3;
    int x2C;
    int x30;
    int x34;
    float x38;
    float x3C;
    SerializeData x40_SerializeData;
    SerializeData x48_SerializeData;
    SerializeData x50_SerializeData;
    SerializeData x58_SerializeData;
    void dumpfunc() {
      dumpval(x00_dmFloat3s, x08_dmPlanes, x10_dmSubEdges, x18_byte, x20_dmFloat3, x2C);
      dumpval(x30, x34, x38, x3C);
    }
  };

  declstruct(CollisionShape) {
    int x00;
    int x04;
    int x08;
    int x0C;
    float x10;
    float x14;
    float x18;
    SerializeData x1C_SerializeData;
    uint32 x24_;
    Array<PolytopeData> x28_PolytopeDatas = x1C_SerializeData;
    DT_VECTOR3D x30_DT_VECTOR3D;
    DT_VECTOR3D x3C_DT_VECTOR3D;
    float x48;
    uint32 x4C_;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x28_PolytopeDatas, x30_DT_VECTOR3D, x3C_DT_VECTOR3D, x48);
    }
  };

  declstruct(Quaternion) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    float x0C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C);
    }
  };

  declstruct(PRTransform) {
    Quaternion x00_Quaternion;
    DT_VECTOR3D x10_DT_VECTOR3D;
    void dumpfunc() {
      dumpval(x00_Quaternion, x10_DT_VECTOR3D);
    }
  };

  declstruct(ConstraintParameters) {
    char x000_Text[64];
    int x040;
    int x044;
    int x048;
    int x04C;
    PRTransform x050_PRTransform;
    DT_VECTOR3D x06C_DT_VECTOR3D;
    PRTransform x078_PRTransform;
    PRTransform x094_PRTransform;
    float x0B0;
    float x0B4;
    float x0B8;
    float x0BC;
    float x0C0;
    float x0C4;
    float x0C8;
    float x0CC;
    float x0D0;
    float x0D4;
    float x0D8;
    float x0DC;
    float x0E0;
    char x0E4_Text[64];
    void dumpfunc() {
      dumpval(x000_Text, x040, x044, x048, x04C, x050_PRTransform);
      dumpval(x06C_DT_VECTOR3D, x078_PRTransform, x094_PRTransform, x0B0, x0B4, x0B8);
      dumpval(x0BC, x0C0, x0C4, x0C8, x0CC, x0D0);
      dumpval(x0D4, x0D8, x0DC, x0E0, x0E4_Text);
    }
  };

  declstruct(PRSTransform) {
    Quaternion x00_Quaternion;
    DT_VECTOR3D x10_DT_VECTOR3D;
    float x1C;
    void dumpfunc() {
      dumpval(x00_Quaternion, x10_DT_VECTOR3D, x1C);
    }
  };

  declstruct(Sphere) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    float x0C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C);
    }
  };

  declstruct(BoneStructure) {
    char x000_Text[64];
    int x040;
    AABB x044_AABB;
    Sphere x05C_Sphere;
    PRSTransform x06C_PRSTransform;
    PRSTransform x08C_PRSTransform;
    PRSTransform x0AC_PRSTransform;
    PRSTransform x0CC_PRSTransform;
    PRSTransform x0EC_PRSTransform;
    int x10C;
    SerializeData x110_SerializeData;
    Array<CollisionShape> x118_CollisionShapes = x110_SerializeData;
    SerializeData x120_SerializeData;
    Array<ConstraintParameters> x128_ConstraintParameters = x120_SerializeData;
    Sno<Particle> x130_ParticleSno;
    uint32 x134_;
    void dumpfunc() {
      dumpval(x000_Text, x040, x044_AABB, x05C_Sphere, x06C_PRSTransform, x08C_PRSTransform);
      dumpval(x0AC_PRSTransform, x0CC_PRSTransform, x0EC_PRSTransform, x10C, x118_CollisionShapes, x128_ConstraintParameters);
      dumpval(x130_ParticleSno);
    }
  };

  declstruct(Hardpoint) {
    char x00_Text[64];
    int x40;
    PRTransform x44_PRTransform;
    void dumpfunc() {
      dumpval(x00_Text, x40, x44_PRTransform);
    }
  };

  declstruct(CollisionCapsule) {
    float x00;
    float x04;
    Hardpoint x08_Hardpoint;
    void dumpfunc() {
      dumpval(x00, x04, x08_Hardpoint);
    }
  };

  declstruct(ClothConstraintSerializable) {
    int x00;
    int x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
    }
  };

  declstruct(ClothFaceSerializable) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    float x0C;
    int x10;
    int x14;
    int x18;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C, x10, x14, x18);
    }
  };

  declstruct(ClothStaple) {
    int x00;
    int x04_int[3];
    float x10_float[3];
    void dumpfunc() {
      dumpval(x00, x04_int, x10_float);
    }
  };

  declstruct(ClothVertexSerializable) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    DT_VECTOR3D x0C_DT_VECTOR3D;
    DT_VECTOR3D x18_DT_VECTOR3D;
    DT_VECTOR3D x24_DT_VECTOR3D;
    float x30;
    int x34;
    int x38;
    int x3C;
    int x40;
    DT_VECTOR3D x44_DT_VECTOR3D;
    int x50;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C_DT_VECTOR3D, x18_DT_VECTOR3D, x24_DT_VECTOR3D, x30, x34);
      dumpval(x38, x3C, x40, x44_DT_VECTOR3D, x50);
    }
  };

  declstruct(ClothStructure) {
    int x00;
    SerializeData x04_SerializeData;
    uint32 x0C_;
    Array<ClothVertexSerializable> x10_ClothVertexSerializables = x04_SerializeData;
    int x18;
    SerializeData x1C_SerializeData;
    uint32 x24_;
    Array<ClothFaceSerializable> x28_ClothFaceSerializables = x1C_SerializeData;
    int x30;
    SerializeData x34_SerializeData;
    uint32 x3C_;
    Array<ClothStaple> x40_ClothStaples = x34_SerializeData;
    int x48;
    SerializeData x4C_SerializeData;
    uint32 x54_;
    Array<ClothConstraintSerializable> x58_ClothConstraintSerializables = x4C_SerializeData;
    int x60;
    SerializeData x64_SerializeData;
    uint32 x6C_;
    Array<ClothConstraintSerializable> x70_ClothConstraintSerializables = x64_SerializeData;
    int x78;
    float x7C;
    void dumpfunc() {
      dumpval(x00, x10_ClothVertexSerializables, x18, x28_ClothFaceSerializables, x30, x40_ClothStaples);
      dumpval(x48, x58_ClothConstraintSerializables, x60, x70_ClothConstraintSerializables, x78, x7C);
    }
  };

  declstruct(DT_RGBACOLOR) {
    uint8 x00;
    uint8 x01;
    uint8 x02;
    uint8 x03;
    void dumpfunc() {
      dumpval(x00, x01, x02, x03);
    }
  };

  declstruct(FatVertex) {
    DT_VECTOR3D x00_Position;
    DT_NORMAL3D x0C_Normal;
    DT_RGBACOLOR x10_Colors[2];
    DT_TEXCOORD x18_TexCoords[2];
    DT_RGBACOLOR x20_DT_RGBACOLOR;
    DT_RGBACOLOR x24_DT_RGBACOLOR;
    int16 x28;
    int16 x2A;
    void dumpfunc() {
      dumpval(x00_Position, x0C_Normal, x10_Colors, x18_TexCoords, x20_DT_RGBACOLOR, x24_DT_RGBACOLOR);
      dumpval(x28, x2A);
    }
  };

  declstruct(Influence) {
    int x00;
    float x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(VertInfluences) {
    Influence x00_Influences[3];
    void dumpfunc() {
      dumpval(x00_Influences);
    }
  };

  declstruct(SubObject) {
    int x000_ObjectType; // type
    int x004; // nVerts
    SerializeData x008_SerializeData;
    Array<FatVertex> x010_FatVertexs = x008_SerializeData;
    SerializeData x018_SerializeData;
    Array<VertInfluences> x020_VertInfluences = x018_SerializeData;
    int x028; // nIndices
    SerializeData x02C_SerializeData;
    uint32 x034_;
    Array<int16> x038_short = x02C_SerializeData;
    SerializeData x040_SerializeData;
    Array<ClothStructure> x048_ClothStructures = x040_SerializeData;
    Sno<Surface> x050_SurfaceSno;
    int x054;
    float x058;
    char x05C_Text[128];
    char x0DC_Text[128];
    AABB x15C_AABB;
    int x174;
    SerializeData x178_SerializeData;
    Array<CollisionShape> x180_CollisionShapes = x178_SerializeData;
    int64 x188;
    void dumpfunc() {
      dumpval(x000_ObjectType, x004, /*x010_FatVertexs, x020_VertInfluences,*/ x028/*, x038_short*/);
      dumpval(x048_ClothStructures, x050_SurfaceSno, x054, x058, x05C_Text, x0DC_Text);
      dumpval(x15C_AABB, x174, x180_CollisionShapes, x188);
    }
  };

  declstruct(GeoSet) {
    int x00;
    SerializeData x04_SerializeData;
    uint32 x0C_;
    Array<SubObject> x10_SubObjects = x04_SerializeData;
    void dumpfunc() {
      dumpval(x00, x10_SubObjects);
    }
  };

  declstruct(LookAtData) {
    int x00;
    char x04_Text[64];
    float x44_Angle;
    float x48_Angle;
    float x4C_Angle;
    float x50_Angle;
    float x54_AngularVelocity;
    float x58;
    void dumpfunc() {
      dumpval(x00, x04_Text, x44_Angle, x48_Angle, x4C_Angle, x50_Angle);
      dumpval(x54_AngularVelocity, x58);
    }
  };

  declstruct(OctreeCube) {
    DT_VECTOR3D x00_DT_VECTOR3D;
    float x0C;
    void dumpfunc() {
      dumpval(x00_DT_VECTOR3D, x0C);
    }
  };

  declstruct(OctreeLeaf) {
    OctreeCube x00_OctreeCube;
    int x10;
    int x14;
    void dumpfunc() {
      dumpval(x00_OctreeCube, x10, x14);
    }
  };

  declstruct(OctreeNode) {
    OctreeCube x00_OctreeCube;
    int x10_int[8];
    void dumpfunc() {
      dumpval(x00_OctreeCube, x10_int);
    }
  };

  declstruct(OctreePrimitive) {
    int x00;
    int x04;
    void dumpfunc() {
      dumpval(x00, x04);
    }
  };

  declstruct(Octree) {
    int x00;
    int x04;
    int x08;
    int x0C;
    int x10;
    int x14;
    SerializeData x18_SerializeData;
    Array<OctreeNode> x20_OctreeNodes = x18_SerializeData;
    SerializeData x28_SerializeData;
    Array<OctreeLeaf> x30_OctreeLeafs = x28_SerializeData;
    SerializeData x38_SerializeData;
    Array<OctreePrimitive> x40_OctreePrimitives = x38_SerializeData;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x20_OctreeNodes, x30_OctreeLeafs, x40_OctreePrimitives);
    }
  };

  declstruct(Structure) {
    int x000;
    int x004;
    SerializeData x008_SerializeData;
    Array<BoneStructure> x010_BoneStructures = x008_SerializeData;
    LookAtData x018_LookAtData;
    int x074;
    Array<BonePulseData> x078_BonePulseDatas = x080_SerializeData;
    SerializeData x080_SerializeData;
    GeoSet x088_GeoSets[2];
    Sphere x0B8_Sphere;
    int x0C8;
    SerializeData x0CC_SerializeData;
    uint32 x0D4_;
    Array<CollisionCapsule> x0D8_CollisionCapsules = x0CC_SerializeData;
    int x0E0;
    SerializeData x0E4_SerializeData;
    uint32 x0EC_;
    Array<Hardpoint> x0F0_Hardpoints = x0E4_SerializeData;
    DT_VECTOR3D x0F8_DT_VECTOR3D;
    uint32 x104_;
    Octree x108_Octree;
    AABB x150_AABB;
    int x168;
    SerializeData x16C_SerializeData;
    uint32 x174_;
    Array<ConstraintParameters> x178_ConstraintParameters = x16C_SerializeData;
    int x180;
    uint32 x184_;
    int64 x188;
    void dumpfunc() {
      dumpval(x000, x004, x010_BoneStructures, x018_LookAtData, x074, x078_BonePulseDatas);
      dumpval(x088_GeoSets, x0B8_Sphere, x0C8, x0D8_CollisionCapsules, x0E0, x0F0_Hardpoints);
      dumpval(x0F8_DT_VECTOR3D, x108_Octree, x150_AABB, x168, x178_ConstraintParameters, x180);
      dumpval(x188);
    }
  };

  declstruct(DT_RGBACOLORVALUE) {
    float x00;
    float x04;
    float x08;
    float x0C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C);
    }
  };

  declstruct(Material) {
    DT_RGBACOLORVALUE x00_DT_RGBACOLORVALUE;
    DT_RGBACOLORVALUE x10_DT_RGBACOLORVALUE;
    DT_RGBACOLORVALUE x20_DT_RGBACOLORVALUE;
    DT_RGBACOLORVALUE x30_DT_RGBACOLORVALUE;
    float x40;
    int x44;
    void dumpfunc() {
      dumpval(x00_DT_RGBACOLORVALUE, x10_DT_RGBACOLORVALUE, x20_DT_RGBACOLORVALUE, x30_DT_RGBACOLORVALUE, x40, x44);
    }
  };

  declstruct(FrameAnim) {
    int x00;
    float x04_Velocity;
    float x08_Velocity;
    int x0C;
    int x10;
    void dumpfunc() {
      dumpval(x00, x04_Velocity, x08_Velocity, x0C, x10);
    }
  };

  declstruct(Matrix4x4) {
    float x00;
    float x04;
    float x08;
    float x0C;
    float x10;
    float x14;
    float x18;
    float x1C;
    float x20;
    float x24;
    float x28;
    float x2C;
    float x30;
    float x34;
    float x38;
    float x3C;
    void dumpfunc() {
      dumpval(x00, x04, x08, x0C, x10, x14);
      dumpval(x18, x1C, x20, x24, x28, x2C);
      dumpval(x30, x34, x38, x3C);
    }
  };

  declstruct(TexAnimParams) {
    int x00;
    Matrix4x4 x04_Matrix4x4;
    float x44;
    float x48_Velocity;
    float x4C_Velocity;
    float x50;
    float x54_Velocity;
    float x58_Velocity;
    float x5C_Angle;
    float x60_AngularVelocity;
    float x64_AngularVelocity;
    int x68;
    int x6C;
    int x70;
    int x74;
    FrameAnim x78_FrameAnim;
    int x8C;
    void dumpfunc() {
      dumpval(x00, x04_Matrix4x4, x44, x48_Velocity, x4C_Velocity, x50);
      dumpval(x54_Velocity, x58_Velocity, x5C_Angle, x60_AngularVelocity, x64_AngularVelocity, x68);
      dumpval(x6C, x70, x74, x78_FrameAnim, x8C);
    }
  };

  declstruct(MaterialTexture) {
    Sno<Textures> x00_TexturesSno;
    TexAnimParams x04_TexAnimParams;
    int x94;
    void dumpfunc() {
      dumpval(x00_TexturesSno, x04_TexAnimParams, x94);
    }
  };

  declstruct(MaterialTextureEntry) {
    int x00;
    int x04;
    MaterialTexture x08_MaterialTexture;
    void dumpfunc() {
      dumpval(x00, x04, x08_MaterialTexture);
    }
  };

  declstruct(UberMaterial) {
    Sno<ShaderMap> x00_ShaderMapSno;
    Material x04_Material;
    SerializeData x4C_SerializeData;
    uint32 x54_;
    Array<MaterialTextureEntry> x58_MaterialTextureEntries = x4C_SerializeData;
    uint32 x60_;
    uint32 x64_;
    void dumpfunc() {
      dumpval(x00_ShaderMapSno, x04_Material, x58_MaterialTextureEntries);
    }
  };

  declstruct(SubObjectAppearance) {
    int x00;
    Sno<Cloth> x04_ClothSno;
    SerializeData x08_SerializeData;
    Array<TagValue> x10_TagMap = x08_SerializeData; // type: 0x000A0000
    UberMaterial x18_UberMaterial;
    Sno<::Material> x80_MaterialSno;
    uint32 x84_;
    UberMaterial x88_UberMaterial;
    uint32 xF0_;
    uint32 xF4_;
    void dumpfunc() {
      dumpval(x00, x04_ClothSno, x10_TagMap, x18_UberMaterial, x80_MaterialSno, x88_UberMaterial);
    }
  };

  declstruct(AppearanceMaterial) {
    char x00_Text[128];
    SerializeData x80_SerializeData;
    Array<SubObjectAppearance> x88_SubObjectAppearances = x80_SerializeData;
    void dumpfunc() {
      dumpval(x00_Text, x88_SubObjectAppearances);
    }
  };

  declstruct(AppearanceLook) {
    char x00_Text[64];
    void dumpfunc() {
      dumpval(x00_Text);
    }
  };

  declstruct(StaticLight) {
    int x00;
    int x04;
    DT_VECTOR3D x08_DT_VECTOR3D;
    DT_VECTOR3D x14_DT_VECTOR3D;
    float x20;
    int x24;
    float x28_float[3];
    float x34;
    float x38;
    float x3C;
    float x40;
    int x44;
    int x48;
    DT_RGBACOLORVALUE x4C_DT_RGBACOLORVALUE;
    void dumpfunc() {
      dumpval(x00, x04, x08_DT_VECTOR3D, x14_DT_VECTOR3D, x20, x24);
      dumpval(x28_float, x34, x38, x3C, x40, x44);
      dumpval(x48, x4C_DT_RGBACOLORVALUE);
    }
  };

  SnoHeader x000_Header;
  uint32 x00C_;
  Structure x010_Structure;
  int x1A0;
  int x1A4;
  SerializeData x1A8_SerializeData;
  Array<AppearanceMaterial> x1B0_AppearanceMaterials = x1A8_SerializeData;
  SerializeData x1B8_SerializeData;
  Array<AppearanceLook> x1C0_AppearanceLooks = x1B8_SerializeData;
  int x1C8;
  SerializeData x1CC_SerializeData;
  uint32 x1D4_;
  Array<StaticLight> x1D8_StaticLights = x1CC_SerializeData;
  int x1E0;
  float x1E4_Time;
  float x1E8_Time;
  float x1EC_Time;
  float x1F0_Time;
  float x1F4_Time;
  int x1F8;
  float x1FC;
  int x200;
  int x204;
  int64 x208;
  int x210;
  uint32 x214_;
  void dumpfunc() {
    dumpval(x000_Header, x010_Structure, x1A0, x1A4, x1B0_AppearanceMaterials, x1C0_AppearanceLooks);
    dumpval(x1C8, x1D8_StaticLights, x1E0, x1E4_Time, x1E8_Time, x1EC_Time);
    dumpval(x1F0_Time, x1F4_Time, x1F8, x1FC, x200, x204);
    dumpval(x208, x210);
  }
};

#pragma pack(pop)
