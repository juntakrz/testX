#pragma once

#include "util.h"
#include "def.h"

class FileWorks {

  HANDLE m_hFile = nullptr;
  std::wstring m_filePath = L"";
  DWORD m_fileSize = 0;
  DWORD m_bufferSize = 0;
  std::unique_ptr<BYTE[]> m_pBuffer;
  DWORD m_flags = 0;

public:
  FileWorks(const std::wstring& path, DWORD flags = 0) noexcept;
 ~FileWorks() noexcept;
  FileWorks(const FileWorks&) = delete;
  FileWorks operator=(const FileWorks&) = delete;

  void openFile(const std::wstring& path, DWORD flags = 0) noexcept;
  void getBuffer(BYTE* out_pBuffer, DWORD& out_bufferSize) noexcept;
  BYTE* getBuffer() noexcept;
  DWORD getBufferSize() noexcept;
};