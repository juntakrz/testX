#pragma once

void processArgs(int argc, wchar_t* argv[]);
void presentResults(CBufferProc* execBuffer, CFileProc* iconFile = nullptr,
                    bool isDetailed = true) noexcept;