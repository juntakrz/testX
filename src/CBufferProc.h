#pragma once

class CBufferProc {

  CFileProc* m_pFP = nullptr;
  BYTE* m_pBuffer = nullptr;
  DWORD m_bufferSize = 0;
  bufferType m_type = bufferType::none;

  PIMAGE_DOS_HEADER m_pDOSHdr = nullptr;
  PIMAGE_NT_HEADERS m_pNTHdr = nullptr;
  PIMAGE_IMPORT_DESCRIPTOR m_pImportDesc = nullptr;

  std::vector<std::string> m_usedLibs;
  std::map<std::string, std::vector<std::string>> m_foundFuncs;

 private:
  void parseImportDesc(PIMAGE_IMPORT_DESCRIPTOR pImpDesc = nullptr,
                       std::string libName = "") noexcept;

 public:
  CBufferProc(BYTE* pBuffer, DWORD size) noexcept;
  CBufferProc(CFileProc* pFW) noexcept;
  ~CBufferProc(){};

  void attach(BYTE* pBuffer, DWORD size) noexcept;
  void attach(CFileProc* pFW) noexcept;

  void setType(bufferType type);

  void parseExecHeader() noexcept;
  void injectIcon(CFileProc* pFP) noexcept;

  void showParsedData(bool isDetailed = false) noexcept;
};