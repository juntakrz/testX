#pragma once

namespace util {
void printHelp() noexcept;

template<typename T>
bool checkFlag(T flags, uint8_t bitPos) noexcept {
  return (bitPos < sizeof(T) * 8) ? flags & (1 << bitPos) : false;
}

template<typename T, typename R>
bool calcOffsetFromRVA(PIMAGE_NT_HEADERS pINH, T RVA, R& out_Offset) {
  PIMAGE_SECTION_HEADER pISH = IMAGE_FIRST_SECTION(pINH);
  WORD numSections = pINH->FileHeader.NumberOfSections;

  // find the correct section
  for (WORD i = 0; i < numSections; i++) {
    if (pISH->VirtualAddress <= RVA &&
        (pISH->VirtualAddress + pISH->Misc.VirtualSize) > RVA) {

      out_Offset = RVA - pISH->VirtualAddress + pISH->PointerToRawData;
      return true;  // success
    }

    pISH++;
  }

  return false;     // not found
}
}