#include "pch.h"
#include "CFileWorks.h"

CFileWorks::CFileWorks(const std::wstring& path, DWORD flags) noexcept
    : m_filePath(path), m_flags(flags) {
  CFileWorks::openFile(m_filePath, m_flags);
}

CFileWorks::~CFileWorks() noexcept {
  if (m_pBuffer) {
    m_pBuffer.release();
  }
}

void CFileWorks::openFile(const std::wstring& path, DWORD flags) noexcept {
  DWORD lastError = 0;

  m_hFile = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  // check if file exists
  if (lastError = GetLastError() == ERROR_FILE_NOT_FOUND) {
    std::cout << "ERROR: could not find the file specified.\n";
    util::printHelp();
    exit(ERROR_FILE_NOT_FOUND);
  }

  // get file size
  m_fileSize = GetFileSize(m_hFile, 0);

  // prepare buffer for writing to
  m_pBuffer = std::make_unique<BYTE[]>(m_fileSize);

  // check if file was read successfully
  if (!ReadFile(m_hFile, m_pBuffer.get(), m_fileSize, &m_bufferSize, 0) && m_bufferSize != m_fileSize) {
    lastError = GetLastError();
    std::cout << "ERROR: opened but failed to read the file. Error code: "
              << lastError << "\n";
    exit(lastError);
  };

  CloseHandle(m_hFile);

  // basic format checking
  if (m_flags) {
    if (util::checkFlag(m_flags, (uint8_t)bufType::exec - 1)) {
      // Mark Zbikowski and minimum possible PE size check
      if (m_pBuffer[0] != 'M' && m_pBuffer[1] != 'Z' || m_bufferSize < 97) {
        std::cout << "ERROR: this does not seem to be a Windows executable.\n";
        exit(404);
      } else {
        m_type = bufType::exec;
      }
    }
    if (util::checkFlag(m_flags, (uint8_t)bufType::icon - 1)) {
      std::cout << "check if icon\n";
      m_type = bufType::icon;
    }
  }

  std::cout << "Successfuly read '" <<
                   std::string(m_filePath.begin(), m_filePath.end()) <<
                   "', size: " << (uint32_t)m_bufferSize << " bytes.\n";
}

void CFileWorks::getBuffer(BYTE* out_pBuffer, DWORD& out_bufferSize) noexcept {
  out_pBuffer = m_pBuffer.get();
  out_bufferSize = m_bufferSize;
}

BYTE* CFileWorks::getBuffer() noexcept { return m_pBuffer.get(); }

DWORD CFileWorks::getBufferSize() noexcept { return m_bufferSize; }

bufType CFileWorks::getBufferType() noexcept { return m_type; }
