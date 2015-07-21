#pragma once
#include "types/Textures.h"
#include "common.h"

class GameTextures {
public:
  static Image get(uint32 id);
private:
  GameTextures();
  struct Texture {
    Image image;
    std::map<uint32, Textures::Type::TexFrame> frames;
  };
  std::map<uint32, std::string> files_;
  std::map<uint32, Texture> textures_;
  std::map<uint32, uint32> dir_;
  static GameTextures& instance();
};
