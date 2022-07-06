#include "pch.h"
#include "CFileProc.h"
#include "CBufferProc.h"

CBufferProc::CBufferProc(CFileProc* pFP) noexcept
    : m_pFP(pFP),
      m_pBuffer(m_pFP->getBuffer()),
      m_bufferSize(m_pFP->getBufferSize()),
      m_type(m_pFP->getBufferType()) {
  //
}

void CBufferProc::attach(CFileProc* pFP) noexcept {
  m_pFP = pFP;
  m_pBuffer = m_pFP->getBuffer();
  m_bufferSize = m_pFP->getBufferSize();
  m_type = m_pFP->getBufferType();
}

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
            parseImportDesc(pImportDesc, pLibName);
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

void CBufferProc::injectIcon(CFileProc* pFPIcon,
                             const wchar_t* outputFile) noexcept {
  if (pFPIcon && pFPIcon->getBufferType() == bufferType::icon) {
    PBYTE pIcon = pFPIcon->getBuffer();
    DWORD iconSize = pFPIcon->getBufferSize();
    std::wstring outputPath =
        (outputFile != L"") ? outputFile : m_pFP->getFilePath();

    wLOG(L"\nInjecting icon: " << pFPIcon->getFilePath() << L"\n\t-> into: "
                               << outputPath);

    // create new file if -o commandline argument is used
    if (outputFile != L"") {
      m_pFP->saveFile(outputPath);
    }

    HANDLE hTgtFile = BeginUpdateResourceW(outputPath.c_str(), FALSE);

    GROUPICON_T gIcon;
    gIcon.imageCount = 1;
    gIcon.resType = 1;

    // force default icon group to have only one icon with id 1
    UpdateResourceW(hTgtFile, RT_GROUP_ICON, MAKEINTRESOURCEW(1),
                    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), &gIcon,
                    sizeof(GROUPICON_T));

    UpdateResourceW(hTgtFile, RT_GROUP_ICON, L"MAINICON",
                    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), &gIcon,
                    sizeof(GROUPICON_T));

    // inject icon with id 1 using correct data offset
    UpdateResourceW(hTgtFile, RT_ICON, MAKEINTRESOURCEW(1),
                    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                    pIcon + pFPIcon->getBufferOffset(),
                    iconSize - pFPIcon->getBufferOffset());

    LOG("Applied icon data offset of "
        << pFPIcon->getBufferOffset() << " bytes and injected "
        << iconSize - pFPIcon->getBufferOffset() << " bytes.");

    EndUpdateResourceW(hTgtFile, FALSE);

    return;
  }

  if (pFPIcon) {
 
    LOG("ERROR: file '" << pFPIcon->getFilePathStr()
                      << "' doesn't seem to be an icon and won't be injected.");
    if (outputFile != L"") {
      wLOG("Because of this '" << outputFile << "' will not be created.");
    }
  }
}

void CBufferProc::parseImportDesc(PIMAGE_IMPORT_DESCRIPTOR pImportDesc, std::string libName) noexcept {
  if (pImportDesc && libName != "") {
    PIMAGE_NT_HEADERS pNTHdr = m_pNTHdr;
    PBYTE pBaseAddr = (PBYTE)m_pDOSHdr;
    std::vector<std::string> collectedFuncs;

    // import lookup table ptrs
    PIMAGE_THUNK_DATA pThunkILT = nullptr;
    PIMAGE_IMPORT_BY_NAME pIBName = nullptr;

    pThunkILT =
        (PIMAGE_THUNK_DATA)((PBYTE)pBaseAddr +
                            util::RVAToOffset(pNTHdr,
                                              pImportDesc->OriginalFirstThunk));

    while (pThunkILT->u1.AddressOfData != 0) {

      // check if function is imported by name and not ordinal
      if (!(pThunkILT->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
        pIBName =
            (PIMAGE_IMPORT_BY_NAME)((PBYTE)pBaseAddr +
                                    util::RVAToOffset(
                                        pNTHdr, pThunkILT->u1.AddressOfData));
        collectedFuncs.emplace_back(pIBName->Name);
      } else if (IMAGE_ORDINAL(pThunkILT->u1.Ordinal)) {
      
        std::ostringstream sstr;
        sstr << "<Ordinal> " << (pThunkILT->u1.Function & 0xffff);
        collectedFuncs.emplace_back(sstr.str());
      }

      pThunkILT++;
    }

    if (!collectedFuncs.empty()) {
      if (m_foundFuncs.try_emplace(libName).second) {
        m_foundFuncs.at(libName) = std::move(collectedFuncs);
      }
    }
  }
}

CFileProc* CBufferProc::getSource() noexcept { return m_pFP; }

const std::vector<std::string>& CBufferProc::libs() noexcept {
  return m_usedLibs;
}

const std::map<std::string, std::vector<std::string>>& CBufferProc::funcs() noexcept {
  return m_foundFuncs;
}