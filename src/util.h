#pragma once

namespace util {

template <typename T>
T RVAToOffset(PIMAGE_NT_HEADERS pNTHdr, T RVA) noexcept {
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

template <typename T>
float calcShannonEntropy(PBYTE pBuffer, T bufferSize) noexcept {
  // e = E(0-255) -p*log2(p)
  float sigma = 0.0f;
  float p = 0.0f;
  uint32_t pData[256] = {0};

  for (T i = 0; i < bufferSize; i++) {
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