#include "pch.h"
#include "util.h"
#include "define.h"

namespace util {
void printHelp() noexcept {
  LOG("\nUSAGE: testX [path_to_executable]");
  LOG("Flags:\n  -i [path_to_icon]\t.ico file to replace original icon with");
  LOG("  -o [output_filename]\toutput .exe file, will write to the same executable if not defined");
  LOG("  -s\t\t\tshort report");
  LOG("\nEXAMPLE: testX test.exe -o test_copy.exe -i icon1.ico");
}

DWORD RVAToOffset(PIMAGE_NT_HEADERS pNTHdr, DWORD RVA) noexcept {
  PIMAGE_SECTION_HEADER pSecHdr = IMAGE_FIRST_SECTION(pNTHdr);
  WORD numSections = pNTHdr->FileHeader.NumberOfSections;

  if (RVA == 0) {
    return RVA;
  }

  for (WORD i = 0; i < numSections; i++) {
    if (pSecHdr->VirtualAddress <= RVA &&
        (pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize) > RVA) {
      break;
    }
    pSecHdr++;
  }
  return RVA - pSecHdr->VirtualAddress + pSecHdr->PointerToRawData;
}
}  // namespace util