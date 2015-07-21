#include <stdio.h>
#include <iostream>
#include <clocale>
#include <set>
#include "snomap.h"
#include "strings.h"
#include "stringmgr.h"
#include "parser.h"
#include "json.h"
#include "path.h"
#include "image.h"
#include "textures.h"
#include "types/Textures.h"
#include "types/Appearance.h"
#include "types/Monster.h"
#include "types/StringList.h"
#include "regexp.h"
#include "translations.h"
#include "description.h"
#include "miner.h"
#include "powertag.h"
#include "model.h"
#include "itemlib.h"
#include <map>
#include <vector>
#include <algorithm>
#include <stdarg.h>
using namespace std;

namespace path {
  std::vector<std::string> roots {
    "C:\\Work\\junk",
    "C:\\Webroot\\game",
    "C:\\tmp",
  };
  std::vector<std::string> cascs {
#ifdef PTR
    "E:\\D3Ptr\\Data",
    "G:\\D3Ptr\\Data",
#else
    "E:\\D3Live\\Data",
    "G:\\D3Live\\Data",
#endif
  };
}

#include "zlib/zlib.h"
int do_main() {
  SnoCascLoader casc(path::casc(), "enUS");
  SnoLoader::default = &casc;

  SnoFile<Appearance> app("Wizard_Female");
  SnoFile<Anim> anim("Wizard_Female_1HS_Orb_Attack_01");
  Render(app, anim);

  return 0;
}

int main() {
  std::setlocale(LC_ALL, "eu_US.UTF-8");
  try {
    return do_main();
  } catch (Exception& ex) {
    fprintf(stderr, "Exception: %s\n", ex.what());
    return 1;
  }
}
