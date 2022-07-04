#pragma once

#include "util.h"

class FileWorks {

  HANDLE m_hFile = nullptr;
  std::wstring m_filePath = L"";
  DWORD m_fileSize = 0;
  DWORD m_bufferSize = 0;
  std::unique_ptr<char[]> m_pBuffer;

public:
  FileWorks(const std::wstring& path) noexcept;
 ~FileWorks() noexcept;
  FileWorks(const FileWorks&) = delete;
  FileWorks operator=(const FileWorks&) = delete;

  void openFile(const std::wstring& path) noexcept;
};