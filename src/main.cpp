#include "pch.h"
#include "util.h"
#include "fileworks.h"

void processArgs(int argc, wchar_t* argv[]) {

  std::vector<std::wstring> argList;

  for (int i = 1; i < argc; i++) {

	argList.emplace_back(argv[i]);
  }

  FileWorks fw1(argList[0]);
};

int wmain(int argc, wchar_t* argv[]) {

  if (argc < 4 || argc > 4) {
    Util::showUsage();
  }
  else {

	processArgs(argc, argv);
  }

  std::cout << "\nAll done. Press ENTER to exit...\n";
  std::cin.get();
  return 0;
}