#include "pch.h"
#include "define.h"
#include "util.h"
#include "CFileProc.h"
#include "CBufferProc.h"

void processArgs(int argc, wchar_t* argv[]) {

  std::vector<std::wstring> argList;
  std::wstring iconPath = L"", outputPath = L"";
  bool isDetailed = true;

  for (int i = 1; i < argc; i++) {

	argList.emplace_back(argv[i]);
  }

  // basic argument parser
  for (uint8_t args = 1; args < argList.size(); args++) {

    if (argList[args] == L"-i" && argList.size() > args) {
      iconPath = argList[args + 1];
      args++; // skip next argument
    }

    if (argList[args] == L"-o" && argList.size() > args) {
      outputPath = argList[args + 1];
      args++;  // skip next argument
    }

    if (argList[args] == L"-s") {
      isDetailed = false;
    }
  }

  CFileProc fExec(argList[0]);
  CBufferProc bExec(&fExec);

  if (iconPath != L"") {
    CFileProc fIcon(iconPath);
    bExec.injectIcon(&fIcon, outputPath.c_str());
  }

  bExec.parseExecHeader();
  bExec.showParsedData(isDetailed);
};

int wmain(int argc, wchar_t* argv[]) {

  if (argc < 2 || argc > 7) {
    util::printHelp();
  }
  else {
	processArgs(argc, argv);
  }

  LOG("\nAll done. Press ENTER to exit...");
  std::cin.get();
  return 0;
}