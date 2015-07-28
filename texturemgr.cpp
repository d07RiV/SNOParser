#include "textures.h"

GameTextures& GameTextures::instance() {
  static GameTextures inst;
  return inst;
}

GameTextures::GameTextures() {
  File src(path::work() / fmtstring("sno_%08x", SnoLoader::default->hash()) / "TextureMap.dat");
  if (!src) {
    for (auto& tex : SnoLoader::All<Textures>()) {
      for (auto& frame : tex->frames) {
        dir_[HashName(frame.name)] = tex->header.id;
      }
      files_[tex->header.id] = tex.name();
    }
    File dst(path::work() / fmtstring("sno_%08x", SnoLoader::default->hash()) / "TextureMap.dat", "wb");
    dst.write32(files_.size());
    dst.write32(dir_.size());
    for (auto& f : files_) {
      dst.write32(f.first);
      dst.write32(f.second.length());
      dst.write(f.second.data(), f.second.length());
    }
    for (auto& d : dir_) {
      dst.write32(d.first);
      dst.write32(d.second);
    }
  } else {
    uint32 files = src.read32();
    uint32 dir = src.read32();
    while (files--) {
      uint32 id = src.read32();
      uint32 len = src.read32();
      std::string str(len, 0);
      src.read(&str[0], len);
      files_[id] = str;
    }
    while (dir--) {
      uint32 id = src.read32();
      dir_[id] = src.read32();
    }
  }
}

Image GameTextures::get(uint32 id) {
  GameTextures& inst = instance();
  auto it = inst.dir_.find(id);
  if (it == inst.dir_.end()) {
    auto fit = inst.textures_.find(id);
    if (fit == inst.textures_.end()) {
      auto fid = inst.files_.find(id);
      if (fid == inst.files_.end()) return Image();
      auto& tex = inst.textures_[id];
      SnoFile<Textures> texture(inst.files_[id]);
      if (texture) tex.image = texture->load();
      return tex.image;
    }
    return fit->second.image;
  }
  auto fit = inst.textures_.find(it->second);
  if (fit == inst.textures_.end()) {
    auto& tex = inst.textures_[it->second];
    SnoFile<Textures> texture(inst.files_[it->second]);
    tex.image = texture->load();
    for (auto& frame : texture->frames) {
      tex.frames[HashName(frame.name)] = frame;
    }
    fit = inst.textures_.find(it->second);
  }
  auto& frame = fit->second.frames[id];
  return fit->second.image.subimagef(frame.x0, frame.y0, frame.x1, frame.y1);
}
