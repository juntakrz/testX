#include "pch.h"
#include "CFileProc.h"
#include "CBufferProc.h"

CBufferProc::CBufferProc(BYTE* pBuffer, DWORD size) noexcept
    : m_pBuffer(pBuffer), m_bufferSize(size) {}

CBufferProc::CBufferProc(CFileProc* pFW) noexcept
    : m_pFP(pFW),
      m_pBuffer(m_pFP->getBuffer()),
      m_bufferSize(m_pFP->getBufferSize()),
      m_type(m_pFP->getBufferType()) {
  //
}

void CBufferProc::attach(BYTE* pBuffer, DWORD size) noexcept {
  m_pBuffer = pBuffer;
  m_bufferSize = size;
}

void CBufferProc::attach(CFileProc* pFP) noexcept {
  m_pFP = pFP;
  m_pBuffer = m_pFP->getBuffer();
  m_bufferSize = m_pFP->getBufferSize();
  m_type = m_pFP->getBufferType();
}

void CBufferProc::setType(bufferType type) { m_type = type; }

void CBufferProc::parseExecHeader() noexcept {
  if (m_type == bufferType::exec) {

    m_pDOSHdr = (PIMAGE_DOS_HEADER)m_pBuffer;

    // "MZ" test for little endian x86 CPUs
    if (m_pDOSHdr->e_magic == IMAGE_DOS_SIGNATURE) {

      // get memory offset to IMAGE_NT_HEADERS
      m_pNTHdr = PIMAGE_NT_HEADERS((PBYTE)m_pDOSHdr + m_pDOSHdr->e_lfanew);

      // "PE" test for little endian x86 CPUs / optional 14th bit (is it EXE or DLL?) test
      if (m_pNTHdr->Signature == IMAGE_NT_SIGNATURE && !(m_pNTHdr->FileHeader.Characteristics & (1 << 14))) {

        // get import descriptor data
        PIMAGE_DATA_DIRECTORY pDataDir =
            &m_pNTHdr->OptionalHeader
                 .DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

        if (pDataDir->Size > 0) {
          m_pImportDesc = PIMAGE_IMPORT_DESCRIPTOR(
              (PBYTE)m_pDOSHdr +
              util::RVAToOffset(m_pNTHdr,
                                pDataDir->VirtualAddress));

          PIMAGE_IMPORT_DESCRIPTOR pImportDesc = m_pImportDesc;

          // step through descriptors and get libraries until there are none left
          while (pImportDesc->Characteristics != NULL) {
            LPSTR pLibName =
                (PCHAR)m_pDOSHdr + util::RVAToOffset(m_pNTHdr, pImportDesc->Name);
            m_usedLibs.emplace_back(pLibName);
            pImportDesc++;
          }

          return;
        } else {
          LOG("ERROR: Import table does not exist in the executable file.");
          return;
        }
      };
    }
  }

  LOG("WARNING: correct header data not found. Buffer is not of an executable type?");
  return;
}

void CBufferProc::injectIcon(CFileProc* pFP) noexcept {
  
  LOG("\nInjecting icon: " << pFP->getFilePathStr()
                         << "\n\tinto: " << m_pFP->getFilePathStr());

  PBYTE pIcon = pFP->getBuffer();
  DWORD iconSize = pFP->getBufferSize();

  HANDLE hTgtFile = BeginUpdateResourceW(m_pFP->getFilePath(), FALSE);

  GROUPICON_T gIcon;
  gIcon.imageCount = 1;
  gIcon.resType = 1;

  // force default icon group to have only one icon with id 1
  UpdateResourceW(hTgtFile, RT_GROUP_ICON, MAKEINTRESOURCEW(1),
                  MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), &gIcon,
                  sizeof(GROUPICON_T));

  // inject icon with id 1
  UpdateResourceW(hTgtFile, RT_ICON, MAKEINTRESOURCEW(1), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                  pIcon + 102,
                  iconSize - 102);

  LOG("Icon was of PNG format, adjusted offset and injected " << iconSize - 102
                                                              << " bytes.");

  EndUpdateResourceW(hTgtFile, FALSE);
}

void CBufferProc::parseImportDesc() noexcept {
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = m_pImportDesc;
  PIMAGE_NT_HEADERS pNTHdr = m_pNTHdr;
  PBYTE pBaseAddr = (PBYTE)m_pDOSHdr;
  
  PIMAGE_THUNK_DATA pThunkILT = nullptr;
  PIMAGE_THUNK_DATA pThunkIAT = nullptr;
  PIMAGE_IMPORT_BY_NAME pIBName = nullptr;
  
  // pImportDesc++
  // iterate through pImportDescs to get functions for all libraries
  
  pThunkILT = (PIMAGE_THUNK_DATA)((PBYTE)pBaseAddr + util::RVAToOffset(pNTHdr, pImportDesc->OriginalFirstThunk));
  pThunkIAT = (PIMAGE_THUNK_DATA)((PBYTE)pBaseAddr + util::RVAToOffset(pNTHdr, pImportDesc->FirstThunk));

  while (pThunkILT->u1.AddressOfData != 0) {
    // check if function is imported by name and not ordinal
    if (!(pThunkILT->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
      pIBName = (PIMAGE_IMPORT_BY_NAME)((PBYTE)pBaseAddr +
                                        util::RVAToOffset(
                                            pNTHdr, pThunkILT->u1.AddressOfData));
      m_usedFuncs.emplace_back(pIBName->Name);
    }
    pThunkILT++;
  }
}

void CBufferProc::showParsedData(bool verbose) noexcept {
  if (m_type == bufferType::exec && !m_usedLibs.empty()) {
    uint16_t index = 0, wNamed = 0;
    LOG("\nLibraries in the import table:\n");
    for (const auto& it : m_usedLibs) {
      std::cout << index << ".\t" << it.c_str() << "\n";
      if (it.find('W') != std::string::npos ||
          it.find('w') != std::string::npos) {
        wNamed++;
      }
      index++;
    }

    LOG("\nPossible WinAPI libraries found: " << wNamed);

    /*
    index = 0;
    LOG("\nFunctions used:\n");

    for (const auto& it : m_usedFuncs) {
      std::cout << index << ".\t" << it.c_str() << "\n";
      index++;
    }
    */
  }
}
