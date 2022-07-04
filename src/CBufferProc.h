#pragma once

class CBufferProc {

  CFileWorks* m_pFW = nullptr;
  BYTE* m_pBuffer = nullptr;
  DWORD m_bufferSize = 0;
  bufType m_type = bufType::none;

  PIMAGE_DOS_HEADER m_pIDH = nullptr;
  PIMAGE_NT_HEADERS m_pINH = nullptr;
  PIMAGE_IMPORT_DESCRIPTOR m_pIID = nullptr;

  std::vector<std::string> m_usedLibs;

 public:
  CBufferProc(BYTE* pBuffer, DWORD size) noexcept;
  CBufferProc(CFileWorks* pFW) noexcept;
  ~CBufferProc(){};

  void attach(BYTE* pBuffer, DWORD size) noexcept;
  void attach(CFileWorks* pFW) noexcept;

  void setType(bufType type);

  void parseExecHeader() noexcept;
  const PIMAGE_DOS_HEADER DOSHdr() const noexcept;

  void showParsedData() noexcept;
};