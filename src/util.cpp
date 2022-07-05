#include "pch.h"
#include "util.h"
#include "define.h"

namespace util {
void printHelp() noexcept {
  LOG("\nUSAGE: testX [path_to_executable]");
  LOG("Flags:\n  -i [path_to_icon]\t.ico file to replace original icon with");
  LOG("  -f\t\t\tlist functions used in libraries");
}

DWORD RVAToOffset(PIMAGE_NT_HEADERS pINH, DWORD RVA) noexcept {
  PIMAGE_SECTION_HEADER pISH = IMAGE_FIRST_SECTION(pINH);
  WORD numSections = pINH->FileHeader.NumberOfSections;

  if (RVA == 0) {
    return RVA;
  }

  for (WORD i = 0; i < numSections; i++) {
    if (pISH->VirtualAddress <= RVA &&
        (pISH->VirtualAddress + pISH->Misc.VirtualSize) > RVA) {
      break;
    }
    pISH++;
  }
  return RVA - pISH->VirtualAddress + pISH->PointerToRawData;
}
}  // namespace util