#pragma once

class CBufferProc {

  CFileProc* m_pFP = nullptr;
  BYTE* m_pBuffer = nullptr;
  DWORD m_bufferSize = 0;
  bufferType m_type = bufferType::none;
  DWORD m_defaultIconGroupId = 0;

  PIMAGE_DOS_HEADER m_pDOSHdr = nullptr;
  PIMAGE_NT_HEADERS m_pNTHdr = nullptr;
  PIMAGE_IMPORT_DESCRIPTOR m_pImportDesc = nullptr;

  std::vector<std::string> m_usedLibs;
  std::map<std::string, std::vector<std::string>> m_foundFuncs;

 private:
  void parseImportDesc(PIMAGE_IMPORT_DESCRIPTOR pImpDesc = nullptr,
                       std::string libName = "") noexcept;

 public:
  CBufferProc(CFileProc* pFP) noexcept;
  ~CBufferProc(){};

  void attach(CFileProc* pFP) noexcept;

  void parseExecHeader() noexcept;
  void injectIcon(CFileProc* pFPIcon, const wchar_t* outputFile = L"") noexcept;

  CFileProc* getSource() noexcept;
  const std::vector<std::string>& libs() noexcept;
  const std::map<std::string, std::vector<std::string>>& funcs() noexcept;
};