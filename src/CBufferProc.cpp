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

void CBufferProc::procHeader() noexcept {
  if (m_type == bufType::exec) {
    m_pIDH = (PIMAGE_DOS_HEADER)m_pBuffer;

    // "MZ" test for little endian x86 CPUs
    if (m_pIDH->e_magic == IMAGE_DOS_SIGNATURE) {
      // get memory offset to IMAGE_NT_HEADERS
      m_pINH = PIMAGE_NT_HEADERS((BYTE*)m_pIDH + m_pIDH->e_lfanew);
      // "PE" test for little endian x86 CPUs / optional 14th bit (is it EXE or DLL?) test
      if (m_pINH->Signature == IMAGE_NT_SIGNATURE && !(m_pINH->FileHeader.Characteristics & (1 << 14))) {
        for (const auto& it : m_pINH->OptionalHeader.DataDirectory) {
          LONG offset = NULL;
          if (util::calcOffsetFromRVA(m_pINH, it.VirtualAddress, offset)) {
            m_pIIDs.emplace_back(PIMAGE_IMPORT_DESCRIPTOR((BYTE*)m_pIDH + offset));
          }
        }
        return;
      };
    }
  }

  std::cout << "WARNING: DOS header data not found. Buffer is not of an executable type?\n";
  return;
}

const PIMAGE_DOS_HEADER CBufferProc::DOSHdr() const noexcept {
  if (m_pIDH) {
    return m_pIDH;
  }

  std::cout << "WARNING: DOS header data not found.\n";
  return nullptr;
}
