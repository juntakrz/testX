#pragma once

#define LOG(x) std::cout << x << "\n"
#define wLOG(x) std::wcout << x << L"\n"

#define LOG_ERROR() std::cout << "Error code: " << GetLastError() << "\n"

enum bufferType { none = 0, exec, icon };

#pragma pack(push, 2)
struct ICON_T {
  BYTE width;
  BYTE height;
  BYTE colors;
  BYTE reserved = 0;
  WORD planes;
  WORD bpp;
  DWORD size;
  WORD resId;
};

struct GROUPICON_T {
  WORD reserved = 0;
  WORD resType;
  WORD imageCount;
};
#pragma pack(pop)