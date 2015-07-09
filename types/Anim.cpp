#include "Anim.h"
using namespace std;

pair<uint32, vector<float>> GetFPA(char const* name) {
  SnoFile<Anim> animFile(name);
  pair<uint32, vector<float>> res;
  res.first = animFile->x00_Header.id;
  for (auto& anim : animFile->x28_AnimPermutations) {
    //res.second.first = anim.x090_frames;
    //res.second.second = anim.x048_velocity;
    res.second.push_back(float(anim.x090 - 1) / float(anim.x048_Velocity));
    //return res;
  }
  return res;
}

map<uint32, vector<float>> const& BaseFPA() {
  static map<uint32, vector<float>> baseFpa_;
  static bool baseParsed_ = false;
  if (baseParsed_) return baseFpa_;
  for (auto& name : Logger::Loop(SnoLoader::default->list<Anim>(), "Parsing FPA")) {
    baseFpa_.insert(GetFPA(name.c_str()));
  }
  baseParsed_ = true;
  return baseFpa_;
}
