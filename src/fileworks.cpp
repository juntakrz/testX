#include "pch.h"
#include "fileworks.h"

FileWorks::FileWorks(const std::wstring& path) noexcept : m_filePath(path) {
  FileWorks::openFile(m_filePath);
}

FileWorks::~FileWorks() noexcept {
  if (m_pBuffer) {
    m_pBuffer.release();
  }
}

void FileWorks::openFile(const std::wstring& path) noexcept {
  DWORD lastError = 0;

  m_hFile = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  // check if file exists
  if (lastError = GetLastError() == ERROR_FILE_NOT_FOUND) {
    std::cout << "ERROR: could not find the file specified.\n";
    Util::showUsage();
    exit(ERROR_FILE_NOT_FOUND);
  }

  // get file size
  m_fileSize = GetFileSize(m_hFile, 0);

  // prepare buffer for writing to
  m_pBuffer = std::make_unique<char[]>(m_fileSize);

  // check if file was read successfully
  if (!ReadFile(m_hFile, m_pBuffer.get(), m_fileSize, &m_bufferSize, 0)) {
    lastError = GetLastError();
    std::cout << "ERROR: opened but failed to read the file. Error code: "
              << lastError << "\n";
    exit(lastError);
  };
}