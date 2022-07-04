#pragma once

class CBufferProc {

  CFileWorks* m_pFW = nullptr;
  BYTE* m_pBuffer = nullptr;
  DWORD m_bufferSize = 0;
  bufType m_type = bufType::none;

  PIMAGE_DOS_HEADER m_pIDH = nullptr;
  LONG m_NTHOffset = 0;

 public:
  CBufferProc(BYTE* pBuffer, DWORD size) noexcept;
  CBufferProc(CFileWorks* pFW) noexcept;
  ~CBufferProc(){};

  void attach(BYTE* pBuffer, DWORD size) noexcept;
  void attach(CFileWorks* pFW) noexcept;

  void setType(bufType type);

  const PIMAGE_DOS_HEADER procDOSHdr() noexcept;
  const PIMAGE_DOS_HEADER DOSHdr() const noexcept;
};