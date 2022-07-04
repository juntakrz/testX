#pragma once

#include "util.h"

class FileWorks {

	HANDLE m_hFile = nullptr;

public:
	static FileWorks& get() {

		static FileWorks _sInstance;
		return _sInstance;
	}

	void openFile(const std::wstring& path) noexcept;
};