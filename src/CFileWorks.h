#pragma once

#include "util.h"
#include "define.h"

class CFileWorks {

  HANDLE m_hFile = nullptr;
  std::wstring m_filePath = L"";
  DWORD m_fileSize = 0;
  DWORD m_bufferSize = 0;
  std::unique_ptr<BYTE[]> m_pBuffer;
  DWORD m_flags = 0;
  bufType m_type = bufType::none;

public:
  CFileWorks(const std::wstring& path, DWORD flags = 0) noexcept;
 ~CFileWorks() noexcept;
  CFileWorks(const CFileWorks&) = delete;
  CFileWorks operator=(const CFileWorks&) = delete;

  void openFile(const std::wstring& path, DWORD flags = 0) noexcept;
  void getBuffer(BYTE* out_pBuffer, DWORD& out_bufferSize) noexcept;
  BYTE* getBuffer() noexcept;
  DWORD getBufferSize() noexcept;
  bufType getBufferType() noexcept;
};