#include "pch.h"
#include "util.h"
#include "CFileWorks.h"
#include "CBufferProc.h"
#include "define.h"

void processArgs(int argc, wchar_t* argv[]) {

  std::vector<std::wstring> argList;

  for (int i = 1; i < argc; i++) {

	argList.emplace_back(argv[i]);
  }

  CFileWorks fExec(argList[0]);
  CBufferProc bExec(&fExec);
  bExec.parseExecHeader();
  bExec.parseImportDesc();
  bExec.showParsedData();
  CFileWorks fIcon(argList[2]);
};

int wmain(int argc, wchar_t* argv[]) {

  if (argc < 4 || argc > 4) {
    util::printHelp();
  }
  else {
	processArgs(argc, argv);
  }

  LOG("\nAll done. Press ENTER to exit...");
  std::cin.get();
  return 0;
}