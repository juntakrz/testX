#pragma once

namespace util {
void printHelp() noexcept;

template<typename T>
bool checkFlag(T flags, uint8_t bitPos) noexcept {
  return (bitPos < sizeof(T) * 8) ? flags & (1 << bitPos) : false;
}
}