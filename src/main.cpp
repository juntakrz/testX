#include "pch.h"
#include "util.h"
#include "CFileProc.h"
#include "CBufferProc.h"
#include "define.h"

void processArgs(int argc, wchar_t* argv[]) {

  std::vector<std::wstring> argList;
  std::wstring iconPath = L"";
  bool isDetailed = false;

  for (int i = 1; i < argc; i++) {

	argList.emplace_back(argv[i]);
  }

  // basic argument parser
  for (uint8_t args = 1; args < argList.size(); args++) {
    if (argList[args] == L"-i" && argList.size() > args) {
      iconPath = argList[args + 1];
      args++; // skip next argument
    }
    if (argList[args] == L"-d") {
      isDetailed = true;
    }
  }

  CFileProc fExec(argList[0]);
  CBufferProc bExec(&fExec);
  if (iconPath != L"") {
    CFileProc fIcon(iconPath);
    bExec.injectIcon(&fIcon);
  }
  bExec.parseExecHeader();
  bExec.showParsedData(isDetailed);
};

int wmain(int argc, wchar_t* argv[]) {

  if (argc < 2) {
    util::printHelp();
  }
  else {
	processArgs(argc, argv);
  }

  LOG("\nAll done. Press ENTER to exit...");
  std::cin.get();
  return 0;
}