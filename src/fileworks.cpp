#include "pch.h"
#include "fileworks.h"

void FileWorks::openFile(const std::wstring& path) noexcept {
  m_hFile = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  if (DWORD lastError = GetLastError() == ERROR_FILE_NOT_FOUND) {
    std::cout << "ERROR: could not find the file specified.\n";
    Util::showUsage();
    exit(ERROR_FILE_NOT_FOUND);
  }
}