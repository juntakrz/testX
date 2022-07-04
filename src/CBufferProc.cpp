#include "pch.h"
#include "CFileWorks.h"
#include "CBufferProc.h"

CBufferProc::CBufferProc(BYTE* pBuffer, DWORD size) noexcept
    : m_pBuffer(pBuffer), m_bufferSize(size) {}

CBufferProc::CBufferProc(CFileWorks* pFW) noexcept
    : m_pFW(pFW),
      m_pBuffer(m_pFW->getBuffer()),
      m_bufferSize(m_pFW->getBufferSize()),
      m_type(m_pFW->getBufferType()) {
  //
}

void CBufferProc::attach(BYTE* pBuffer, DWORD size) noexcept {
  m_pBuffer = pBuffer;
  m_bufferSize = size;
}

void CBufferProc::attach(CFileWorks* pFW) noexcept {
  m_pFW = pFW;
  m_pBuffer = m_pFW->getBuffer();
  m_bufferSize = m_pFW->getBufferSize();
  m_type = m_pFW->getBufferType();
}

void CBufferProc::setType(bufType type) { m_type = type; }

void CBufferProc::parseExecHeader() noexcept {
  if (m_type == bufType::exec) {

    m_pIDH = (PIMAGE_DOS_HEADER)m_pBuffer;

    // "MZ" test for little endian x86 CPUs
    if (m_pIDH->e_magic == IMAGE_DOS_SIGNATURE) {

      // get memory offset to IMAGE_NT_HEADERS
      m_pINH = PIMAGE_NT_HEADERS((PBYTE)m_pIDH + m_pIDH->e_lfanew);

      // "PE" test for little endian x86 CPUs / optional 14th bit (is it EXE or DLL?) test
      if (m_pINH->Signature == IMAGE_NT_SIGNATURE && !(m_pINH->FileHeader.Characteristics & (1 << 14))) {

        // get import descriptor data
        if (m_pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                .Size > 0) {
          m_pIID = PIMAGE_IMPORT_DESCRIPTOR(
              (PBYTE)m_pIDH +
              util::RVAToOffset(m_pINH,
                                m_pINH->OptionalHeader
                                    .DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                                    .VirtualAddress));

          PIMAGE_IMPORT_DESCRIPTOR pIID = m_pIID;

          // step through descriptors and get libraries until there are none
          
          while (pIID->Name != NULL) {
            LPSTR pLibName =
                (PCHAR)m_pIDH + util::RVAToOffset(m_pINH, pIID->Name);
            m_usedLibs.emplace_back(pLibName);
            pIID++;
          }

          /*
          WORD index = 0;
          while (pIID[index].Characteristics != 0) {
            LPSTR pLibName = (PCHAR)m_pIDH + util::RVAToOffset(m_pINH, pIID[index].Name);
            m_usedLibs.emplace_back(pLibName);
            index++;
          }*/
          return;
        } else {
          std::cout
              << "ERROR: Import table does not exist in the executable file.\n";
          return;
        }
      };
    }
  }

  std::cout << "WARNING: correct header data not found. Buffer is not of an executable type?\n";
  return;
}

const PIMAGE_DOS_HEADER CBufferProc::DOSHdr() const noexcept {
  if (m_pIDH) {
    return m_pIDH;
  }

  std::cout << "WARNING: DOS header data not found.\n";
  return nullptr;
}

void CBufferProc::showParsedData() noexcept {
  if (m_type == bufType::exec && !m_usedLibs.empty()) {
    uint16_t index = 0, wNamed = 0;
    std::cout << "\nLibraries used:\n\n";
    for (const auto& it : m_usedLibs) {
      std::cout << index << ".\t" << it.c_str() << "\n";
      if (it.find('W') != std::string::npos ||
          it.find('w') != std::string::npos) {
        wNamed++;
      }
      index++;
    }

    std::cout << "\nPossible WinAPI libraries found: " << wNamed << "\n";
  }
}
