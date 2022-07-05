#pragma once

#include "util.h"
#include "define.h"

class CFileWorks {

  HANDLE m_hFile = nullptr;
  std::wstring m_filePath = L"";
  DWORD m_fileSize = 0;
  DWORD m_bufferSize = 0;
  std::unique_ptr<BYTE[]> m_pBuffer;
  bufferType m_type = bufferType::none;

public:
  CFileWorks(const std::wstring& path) noexcept;
 ~CFileWorks() noexcept;
  CFileWorks(const CFileWorks&) = delete;
  CFileWorks operator=(const CFileWorks&) = delete;

  void openFile(const std::wstring& path) noexcept;
  void getBuffer(BYTE* out_pBuffer, DWORD& out_bufferSize) noexcept;
  BYTE* getBuffer() noexcept;
  DWORD getBufferSize() noexcept;
  bufferType getBufferType() noexcept;
};