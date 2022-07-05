#include "pch.h"
#include "util.h"
#include "CFileProc.h"
#include "CBufferProc.h"
#include "define.h"

void processArgs(int argc, wchar_t* argv[]) {

  std::vector<std::wstring> argList;

  for (int i = 1; i < argc; i++) {

	argList.emplace_back(argv[i]);
  }

  CFileProc fExec(argList[0]);
  CFileProc fIcon(argList[2]);
  CBufferProc bExec(&fExec);
  bExec.parseExecHeader();
  bExec.parseImportDesc();
  bExec.showParsedData();
  bExec.injectIcon(&fIcon);
};

int wmain(int argc, wchar_t* argv[]) {
  
  LOG("GO!");

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