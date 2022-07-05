#include "pch.h"
#include "CFileWorks.h"

CFileWorks::CFileWorks(const std::wstring& path) noexcept
    : m_filePath(path) {
  CFileWorks::openFile(m_filePath);
}

CFileWorks::~CFileWorks() noexcept {
  if (m_pBuffer) {
    m_pBuffer.release();
  }
}

void CFileWorks::openFile(const std::wstring& path) noexcept {
  DWORD lastError = 0;

  m_hFile = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  // check if file exists
  if (lastError = GetLastError() == ERROR_FILE_NOT_FOUND) {
    LOG("ERROR: could not find the file specified.");
    util::printHelp();
    exit(ERROR_FILE_NOT_FOUND);
  }

  // get file size
  m_fileSize = GetFileSize(m_hFile, 0);

  // prepare buffer for writing to
  m_pBuffer = std::make_unique<BYTE[]>(m_fileSize);

  // check if file was read successfully
  if (!ReadFile(m_hFile, m_pBuffer.get(), m_fileSize, &m_bufferSize, 0) &&
      m_bufferSize != m_fileSize) {
    lastError = GetLastError();
    LOG("ERROR: opened but failed to read the file. Error code: " << lastError);
    exit(lastError);
  };

  CloseHandle(m_hFile);

  // basic format checking
  {
    PWORD pSOF = (PWORD)m_pBuffer.get();  // check for MZ
    if (*pSOF == IMAGE_DOS_SIGNATURE) {
      m_type = bufferType::exec;
    } else {
      // check for 00 01 00 00
      PDWORD pIconSOF = (PDWORD)m_pBuffer.get();
      if (*pIconSOF == 0x00010000) {
        m_type = bufferType::icon;
      }
    }

    if (m_type == bufferType::none) {
      LOG("WARNING: couldn't determine buffer type, doesn't seem to be an executable or an icon.");
    }
  }

  LOG("\nSuccessfuly read '" <<
                   std::string(m_filePath.begin(), m_filePath.end()) <<
                   "', size: " << (uint32_t)m_bufferSize << " bytes.");
}

void CFileWorks::getBuffer(BYTE* out_pBuffer, DWORD& out_bufferSize) noexcept {
  out_pBuffer = m_pBuffer.get();
  out_bufferSize = m_bufferSize;
}

BYTE* CFileWorks::getBuffer() noexcept { return m_pBuffer.get(); }

DWORD CFileWorks::getBufferSize() noexcept { return m_bufferSize; }

bufferType CFileWorks::getBufferType() noexcept { return m_type; }
