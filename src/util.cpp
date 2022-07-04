#include "pch.h"
#include "util.h"
#include "define.h"

namespace util {
void printHelp() noexcept {
  LOG("USAGE: tie [path_to_executable]");
  LOG("Arguments:\n  -i [path_to_icon]\t.ico file to replace original icon with");
  LOG("  -o [output_filename]\toutput .exe file, will write to the same executable if not defined");
  LOG("  -s\t\t\tshort report");
  LOG("\nEXAMPLE: tie test.exe -o test_copy.exe -i icon1.ico");

  exit(0);
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

float calcShannonEntropy(PBYTE pBuffer, DWORD bufferSize) noexcept {

  // e = Σ -p*log₂(p)
  float sigma = 0.0f;
  float p = 0.0f;
  uint32_t pData[256] = {0};

  for (DWORD i = 0; i < bufferSize; i++) {
    pData[*pBuffer]++;
    pBuffer++;
  }

  for (uint16_t j = 0; j < 256; j++) {
    p = (float)pData[j] / bufferSize;

    if (p > 0) {
      sigma += -p * log2f(p);
    }
  }

  return sigma;
}

}  // namespace util