#include "textures.h"
#include "snomap.h"

GameTextures& GameTextures::instance() {
  static GameTextures inst;
  return inst;
}

GameTextures::GameTextures() {
  File src(path::work() / fmtstring("sno_%s/TextureMap.dat", SnoLoader::default->version().c_str()));
  if (!src) {
    auto const& files = SnoManager::get<Textures>().get();
    size_t count = 0;
    for (auto const& kv : files) {
      if (kv.second.substr(0, 2) == "2D") {
        ++count;
      }
    }
    Logger::begin(count, "Parsing textures");
    for (auto const& kv : files) {
      if (kv.second.substr(0, 2) == "2D") {
        Logger::item(kv.second.c_str());
        SnoFile<Textures> tex(kv.second);
        for (auto& frame : tex->frames) {
          if (frame.name[0]) {
            dir_[HashName(frame.name)] = kv.first;
          }
        }
      }
    }
    Logger::end();
    File dst(path::work() / fmtstring("sno_%s/TextureMap.dat", SnoLoader::default->version().c_str()), "wb");
    dst.write32(dir_.size());
    for (auto& d : dir_) {
      dst.write32(d.first);
      dst.write32(d.second);
    }
  } else {
    uint32 dir = src.read32();
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
      auto const& files = SnoManager::get<Textures>().get();
      auto fid = files.find(id);
      if (fid == files.end()) return Image();
      auto& tex = inst.textures_[id];
      SnoFile<Textures> texture(fid->second);
      if (texture) tex.image = texture->load();
      return tex.image;
    }
    return fit->second.image;
  }
  auto fit = inst.textures_.find(it->second);
  if (fit == inst.textures_.end()) {
    auto& tex = inst.textures_[it->second];
    SnoFile<Textures> texture(Textures::name(it->second));
    tex.image = texture->load();
    for (auto& frame : texture->frames) {
      tex.frames[HashName(frame.name)] = frame;
    }
    fit = inst.textures_.find(it->second);
  }
  auto& frame = fit->second.frames[id];
  return fit->second.image.subimagef(frame.x0, frame.y0, frame.x1, frame.y1);
}
