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

const PIMAGE_DOS_HEADER CBufferProc::procDOSHdr() noexcept {
  if (m_type == bufType::exec) {
    m_pIDH = (PIMAGE_DOS_HEADER)m_pBuffer;

    // MZ test for little endian x86 CPUs
    if (m_pIDH->e_magic == 0x5A4D) {
      m_NTHOffset = m_pIDH->e_lfanew;      // get memory offset to IMAGE_NT_HEADERS
      return m_pIDH;
    }
  }

  std::cout << "WARNING: DOS header data not found. Buffer is not of an executable type?\n";
  return nullptr;
}

const PIMAGE_DOS_HEADER CBufferProc::DOSHdr() const noexcept {
  if (m_pIDH) {
    return m_pIDH;
  }

  std::cout << "WARNING: DOS header data not found.\n";
  return nullptr;
}
