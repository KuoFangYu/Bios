/** @file
 * Course2-06-Cpuid .c source code
;******************************************************************************
;* Copyright (c) 2024, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include <Uefi.h>

#include <Register/Intel/Cpuid.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>


UINT32  gMaxBasic;
UINT32  gMaxExtended;

typedef struct {
  UINT8  Addr;
  CHAR8  *Type;
  CHAR8  *Description;
} CACHE_INFO_DESCRIPTION;

CACHE_INFO_DESCRIPTION gCacheInfoDesc [] = {
  { 0x00 , "General"  , "Null descriptor, this byte contains no information" },
  { 0x01 , "TLB"      , "Instruction TLB: 4 KByte pages, 4-way set associative, 32 entries" },
  { 0x02 , "TLB"      , "Instruction TLB: 4 MByte pages, fully associative, 2 entries" },
  { 0x03 , "TLB"      , "Data TLB: 4 KByte pages, 4-way set associative, 64 entries" },
  { 0x04 , "TLB"      , "Data TLB: 4 MByte pages, 4-way set associative, 8 entries" },
  { 0x05 , "TLB"      , "Data TLB1: 4 MByte pages, 4-way set associative, 32 entries" },
  { 0x06 , "Cache"    , "1st-level instruction cache: 8 KBytes, 4-way set associative, 32 byte line size" },
  { 0x08 , "Cache"    , "1st-level instruction cache: 16 KBytes, 4-way set associative, 32 byte line size" },
  { 0x09 , "Cache"    , "1st-level instruction cache: 32KBytes, 4-way set associative, 64 byte line size" },
  { 0x0A , "Cache"    , "1st-level data cache: 8 KBytes, 2-way set associative, 32 byte line size" },
  { 0x0B , "TLB"      , "Instruction TLB: 4 MByte pages, 4-way set associative, 4 entries" },
  { 0x0C , "Cache"    , "1st-level data cache: 16 KBytes, 4-way set associative, 32 byte line size" },
  { 0x0D , "Cache"    , "1st-level data cache: 16 KBytes, 4-way set associative, 64 byte line size" },
  { 0x0E , "Cache"    , "1st-level data cache: 24 KBytes, 6-way set associative, 64 byte line size" },
  { 0x1D , "Cache"    , "2nd-level cache: 128 KBytes, 2-way set associative, 64 byte line size" },
  { 0x21 , "Cache"    , "2nd-level cache: 256 KBytes, 8-way set associative, 64 byte line size" },
  { 0x22 , "Cache"    , "3rd-level cache: 512 KBytes, 4-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x23 , "Cache"    , "3rd-level cache: 1 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x24 , "Cache"    , "2nd-level cache: 1 MBytes, 16-way set associative, 64 byte line size" },
  { 0x25 , "Cache"    , "3rd-level cache: 2 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x29 , "Cache"    , "3rd-level cache: 4 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x2C , "Cache"    , "1st-level data cache: 32 KBytes, 8-way set associative, 64 byte line size" },
  { 0x30 , "Cache"    , "1st-level instruction cache: 32 KBytes, 8-way set associative, 64 byte line size" },
  { 0x40 , "Cache"    , "No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache" },
  { 0x41 , "Cache"    , "2nd-level cache: 128 KBytes, 4-way set associative, 32 byte line size" },
  { 0x42 , "Cache"    , "2nd-level cache: 256 KBytes, 4-way set associative, 32 byte line size" },
  { 0x43 , "Cache"    , "2nd-level cache: 512 KBytes, 4-way set associative, 32 byte line size" },
  { 0x44 , "Cache"    , "2nd-level cache: 1 MByte, 4-way set associative, 32 byte line size" },
  { 0x45 , "Cache"    , "2nd-level cache: 2 MByte, 4-way set associative, 32 byte line size" },
  { 0x46 , "Cache"    , "3rd-level cache: 4 MByte, 4-way set associative, 64 byte line size" },
  { 0x47 , "Cache"    , "3rd-level cache: 8 MByte, 8-way set associative, 64 byte line size" },
  { 0x48 , "Cache"    , "2nd-level cache: 3MByte, 12-way set associative, 64 byte line size" },
  { 0x49 , "Cache"    , "3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H). 2nd-level cache: 4 MByte, 16-way set associative, 64 byte line size" },
  { 0x4A , "Cache"    , "3rd-level cache: 6MByte, 12-way set associative, 64 byte line size" },
  { 0x4B , "Cache"    , "3rd-level cache: 8MByte, 16-way set associative, 64 byte line size" },
  { 0x4C , "Cache"    , "3rd-level cache: 12MByte, 12-way set associative, 64 byte line size" },
  { 0x4D , "Cache"    , "3rd-level cache: 16MByte, 16-way set associative, 64 byte line size" },
  { 0x4E , "Cache"    , "2nd-level cache: 6MByte, 24-way set associative, 64 byte line size" },
  { 0x4F , "TLB"      , "Instruction TLB: 4 KByte pages, 32 entries" },
  { 0x50 , "TLB"      , "Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 64 entries" },
  { 0x51 , "TLB"      , "Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 128 entries" },
  { 0x52 , "TLB"      , "Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 256 entries" },
  { 0x55 , "TLB"      , "Instruction TLB: 2-MByte or 4-MByte pages, fully associative, 7 entries" },
  { 0x56 , "TLB"      , "Data TLB0: 4 MByte pages, 4-way set associative, 16 entries" },
  { 0x57 , "TLB"      , "Data TLB0: 4 KByte pages, 4-way associative, 16 entries" },
  { 0x59 , "TLB"      , "Data TLB0: 4 KByte pages, fully associative, 16 entries" },
  { 0x5A , "TLB"      , "Data TLB0: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries" },
  { 0x5B , "TLB"      , "Data TLB: 4 KByte and 4 MByte pages, 64 entries" },
  { 0x5C , "TLB"      , "Data TLB: 4 KByte and 4 MByte pages,128 entries" },
  { 0x5D , "TLB"      , "Data TLB: 4 KByte and 4 MByte pages,256 entries" },
  { 0x60 , "Cache"    , "1st-level data cache: 16 KByte, 8-way set associative, 64 byte line size" },
  { 0x61 , "TLB"      , "Instruction TLB: 4 KByte pages, fully associative, 48 entries" },
  { 0x63 , "TLB"      , "Data TLB: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries and a separate array with 1 GByte pages, 4-way set associative, 4 entries" },
  { 0x64 , "TLB"      , "Data TLB: 4 KByte pages, 4-way set associative, 512 entries" },
  { 0x66 , "Cache"    , "1st-level data cache: 8 KByte, 4-way set associative, 64 byte line size" },
  { 0x67 , "Cache"    , "1st-level data cache: 16 KByte, 4-way set associative, 64 byte line size" },
  { 0x68 , "Cache"    , "1st-level data cache: 32 KByte, 4-way set associative, 64 byte line size" },
  { 0x6A , "Cache"    , "uTLB: 4 KByte pages, 8-way set associative, 64 entries" },
  { 0x6B , "Cache"    , "DTLB: 4 KByte pages, 8-way set associative, 256 entries" },
  { 0x6C , "Cache"    , "DTLB: 2M/4M pages, 8-way set associative, 128 entries" },
  { 0x6D , "Cache"    , "DTLB: 1 GByte pages, fully associative, 16 entries" },
  { 0x70 , "Cache"    , "Trace cache: 12 K-uop, 8-way set associative" },
  { 0x71 , "Cache"    , "Trace cache: 16 K-uop, 8-way set associative" },
  { 0x72 , "Cache"    , "Trace cache: 32 K-uop, 8-way set associative" },
  { 0x76 , "TLB"      , "Instruction TLB: 2M/4M pages, fully associative, 8 entries" },
  { 0x78 , "Cache"    , "2nd-level cache: 1 MByte, 4-way set associative, 64byte line size" },
  { 0x79 , "Cache"    , "2nd-level cache: 128 KByte, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x7A , "Cache"    , "2nd-level cache: 256 KByte, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x7B , "Cache"    , "2nd-level cache: 512 KByte, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x7C , "Cache"    , "2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size, 2 lines per sector" },
  { 0x7D , "Cache"    , "2nd-level cache: 2 MByte, 8-way set associative, 64byte line size" },
  { 0x7F , "Cache"    , "2nd-level cache: 512 KByte, 2-way set associative, 64-byte line size" },
  { 0x80 , "Cache"    , "2nd-level cache: 512 KByte, 8-way set associative, 64-byte line size" },
  { 0x82 , "Cache"    , "2nd-level cache: 256 KByte, 8-way set associative, 32 byte line size" },
  { 0x83 , "Cache"    , "2nd-level cache: 512 KByte, 8-way set associative, 32 byte line size" },
  { 0x84 , "Cache"    , "2nd-level cache: 1 MByte, 8-way set associative, 32 byte line size" },
  { 0x85 , "Cache"    , "2nd-level cache: 2 MByte, 8-way set associative, 32 byte line size" },
  { 0x86 , "Cache"    , "2nd-level cache: 512 KByte, 4-way set associative, 64 byte line size" },
  { 0x87 , "Cache"    , "2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size" },
  { 0xA0 , "DTLB"     , "DTLB: 4k pages, fully associative, 32 entries" },
  { 0xB0 , "TLB"      , "Instruction TLB: 4 KByte pages, 4-way set associative, 128 entries" },
  { 0xB1 , "TLB"      , "Instruction TLB: 2M pages, 4-way, 8 entries or 4M pages, 4-way, 4 entries" },
  { 0xB2 , "TLB"      , "Instruction TLB: 4KByte pages, 4-way set associative, 64 entries" },
  { 0xB3 , "TLB"      , "Data TLB: 4 KByte pages, 4-way set associative, 128 entries" },
  { 0xB4 , "TLB"      , "Data TLB1: 4 KByte pages, 4-way associative, 256 entries" },
  { 0xB5 , "TLB"      , "Instruction TLB: 4KByte pages, 8-way set associative, 64 entries" },
  { 0xB6 , "TLB"      , "Instruction TLB: 4KByte pages, 8-way set associative, 128 entries" },
  { 0xBA , "TLB"      , "Data TLB1: 4 KByte pages, 4-way associative, 64 entries" },
  { 0xC0 , "TLB"      , "Data TLB: 4 KByte and 4 MByte pages, 4-way associative, 8 entries" },
  { 0xC1 , "STLB"     , "Shared 2nd-Level TLB: 4 KByte/2MByte pages, 8-way associative, 1024 entries" },
  { 0xC2 , "DTLB"     , "DTLB: 4 KByte/2 MByte pages, 4-way associative, 16 entries" },
  { 0xC3 , "STLB"     , "Shared 2nd-Level TLB: 4 KByte /2 MByte pages, 6-way associative, 1536 entries. Also 1GBbyte pages, 4-way, 16 entries." },
  { 0xC4 , "DTLB"     , "DTLB: 2M/4M Byte pages, 4-way associative, 32 entries" },
  { 0xCA , "STLB"     , "Shared 2nd-Level TLB: 4 KByte pages, 4-way associative, 512 entries" },
  { 0xD0 , "Cache"    , "3rd-level cache: 512 KByte, 4-way set associative, 64 byte line size" },
  { 0xD1 , "Cache"    , "3rd-level cache: 1 MByte, 4-way set associative, 64 byte line size" },
  { 0xD2 , "Cache"    , "3rd-level cache: 2 MByte, 4-way set associative, 64 byte line size" },
  { 0xD6 , "Cache"    , "3rd-level cache: 1 MByte, 8-way set associative, 64 byte line size" },
  { 0xD7 , "Cache"    , "3rd-level cache: 2 MByte, 8-way set associative, 64 byte line size" },
  { 0xD8 , "Cache"    , "3rd-level cache: 4 MByte, 8-way set associative, 64 byte line size" },
  { 0xDC , "Cache"    , "3rd-level cache: 1.5 MByte, 12-way set associative, 64 byte line size" },
  { 0xDD , "Cache"    , "3rd-level cache: 3 MByte, 12-way set associative, 64 byte line size" },
  { 0xDE , "Cache"    , "3rd-level cache: 6 MByte, 12-way set associative, 64 byte line size" },
  { 0xE2 , "Cache"    , "3rd-level cache: 2 MByte, 16-way set associative, 64 byte line size" },
  { 0xE3 , "Cache"    , "3rd-level cache: 4 MByte, 16-way set associative, 64 byte line size" },
  { 0xE4 , "Cache"    , "3rd-level cache: 8 MByte, 16-way set associative, 64 byte line size" },
  { 0xEA , "Cache"    , "3rd-level cache: 12MByte, 24-way set associative, 64 byte line size" },
  { 0xEB , "Cache"    , "3rd-level cache: 18MByte, 24-way set associative, 64 byte line size" },
  { 0xEC , "Cache"    , "3rd-level cache: 24MByte, 24-way set associative, 64 byte line size" },
  { 0xF0 , "Prefetch" , "64-Byte prefetching" },
  { 0xF1 , "Prefetch" , "128-Byte prefetching" },
  { 0xFE , "General"  , "CPUID leaf 2 does not report TLB descriptor information; use CPUID leaf 18H to query TLB and other address translation parameters." },
  { 0xFF , "General"  , "CPUID leaf 2 does not report cache descriptor information, use CPUID leaf 4 to query cache parameters" }
};

UINT8
EFIAPI
SearchCacheDesc (
  IN UINT8 CacheDesc  
  )
{
  UINT8 Max;
  UINT8 Index;

  Max = sizeof(gCacheInfoDesc) / sizeof(gCacheInfoDesc[0]);
  for (Index = 0; Index <= Max; Index++) {
    if (gCacheInfoDesc[Index].Addr == CacheDesc) {
      return Index;
    }
  }

  return 0;
}

VOID
EFIAPI
Signature (
  VOID
  )
{
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;

  //
  // EAX = 0h
  //
  AsmCpuid (CPUID_SIGNATURE, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"Function 0x00 : \n");
  Print (L"  Largest standard function number supported in EAX : %X\n", Eax);
  Print (L"  signature : ");
  Print (L"%c", (Ebx >> 0) & 0xFF);
  Print (L"%c", (Ebx >> 8) & 0xFF);
  Print (L"%c", (Ebx >> 16) & 0xFF);
  Print (L"%c", (Ebx >> 24) & 0xFF);

  Print (L"%c", (Edx >> 0) & 0xFF);
  Print (L"%c", (Edx >> 8) & 0xFF);
  Print (L"%c", (Edx >> 16) & 0xFF);
  Print (L"%c", (Edx >> 24) & 0xFF);

  Print (L"%c", (Ecx >> 0) & 0xFF);
  Print (L"%c", (Ecx >> 8) & 0xFF);
  Print (L"%c", (Ecx >> 16) & 0xFF);
  Print (L"%c\n", (Ecx >> 24) & 0xFF);
  Print (L"\n");

  gMaxBasic = Eax;
}


VOID
EFIAPI
VersionInfo (
  VOID
  )
{
  CPUID_VERSION_INFO_EAX  Eax;
  CPUID_VERSION_INFO_EBX  Ebx;
  CPUID_VERSION_INFO_ECX  Ecx;
  CPUID_VERSION_INFO_EDX  Edx;
  UINT32                  DisplayFamily;
  UINT32                  DisplayModel;

  if (CPUID_VERSION_INFO > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_VERSION_INFO, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);

  DisplayFamily = Eax.Bits.FamilyId;
  if (Eax.Bits.FamilyId == 0x0F) {
    DisplayFamily |= (Eax.Bits.ExtendedFamilyId << 4);
  }

  DisplayModel = Eax.Bits.Model;
  if (Eax.Bits.FamilyId == 0x06 || Eax.Bits.FamilyId == 0x0f) {
    DisplayModel |= (Eax.Bits.ExtendedModelId << 4);
  }

  Print (L"Function 0x01 - CPUID_VERSION_INFO :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
         Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32
         );
  Print (L"  Family = %x  Model = %x  Stepping = %x\n", 
         DisplayFamily, DisplayModel, Eax.Bits.SteppingId
         );

  Print (L"  EAX :\n");
  Print (L"    SteppingId       : %x\n", Eax.Bits.SteppingId);
  Print (L"    Model            : %x\n", Eax.Bits.Model);
  Print (L"    FamilyId         : %x\n", Eax.Bits.FamilyId);
  Print (L"    ProcessorType    : %x\n", Eax.Bits.ProcessorType);
  Print (L"    ExtendedModelId  : %x\n", Eax.Bits.ExtendedModelId);
  Print (L"    ExtendedFamilyId : %x\n", Eax.Bits.ExtendedFamilyId);
  
  Print (L"  EBX :\n");
  Print (L"    BrandIndex                                : %x\n", Ebx.Bits.BrandIndex);
  Print (L"    CacheLineSize                             : %x\n", Ebx.Bits.CacheLineSize);
  Print (L"    MaximumAddressableIdsForLogicalProcessors : %x\n", Ebx.Bits.MaximumAddressableIdsForLogicalProcessors);
  Print (L"    InitialLocalApicId                        : %x\n", Ebx.Bits.InitialLocalApicId);

  Print (L"  ECX :\n");
  Print (L"    SSE3                : %x\n", Ecx.Bits.SSE3);
  Print (L"    PCLMULQDQ           : %x\n", Ecx.Bits.PCLMULQDQ);
  Print (L"    DTES64              : %x\n", Ecx.Bits.DTES64);
  Print (L"    MONITOR             : %x\n", Ecx.Bits.MONITOR);
  Print (L"    DS_CPL              : %x\n", Ecx.Bits.DS_CPL);
  Print (L"    VMX                 : %x\n", Ecx.Bits.VMX);
  Print (L"    SMX                 : %x\n", Ecx.Bits.SMX);
  Print (L"    TM2                 : %x\n", Ecx.Bits.TM2);
  Print (L"    SSSE3               : %x\n", Ecx.Bits.SSSE3);
  Print (L"    CNXT_ID             : %x\n", Ecx.Bits.CNXT_ID);
  Print (L"    SDBG                : %x\n", Ecx.Bits.SDBG);
  Print (L"    FMA                 : %x\n", Ecx.Bits.FMA);
  Print (L"    CMPXCHG16B          : %x\n", Ecx.Bits.CMPXCHG16B);
  Print (L"    xTPR_Update_Control : %x\n", Ecx.Bits.xTPR_Update_Control);
  Print (L"    PDCM                : %x\n", Ecx.Bits.PDCM);
  Print (L"    PCID                : %x\n", Ecx.Bits.PCID);
  Print (L"    DCA                 : %x\n", Ecx.Bits.DCA);
  Print (L"    SSE4_1              : %x\n", Ecx.Bits.SSE4_1);
  Print (L"    SSE4_2              : %x\n", Ecx.Bits.SSE4_2);
  Print (L"    x2APIC              : %x\n", Ecx.Bits.x2APIC);
  Print (L"    MOVBE               : %x\n", Ecx.Bits.MOVBE);
  Print (L"    POPCNT              : %x\n", Ecx.Bits.POPCNT);
  Print (L"    TSC_Deadline        : %x\n", Ecx.Bits.TSC_Deadline);
  Print (L"    AESNI               : %x\n", Ecx.Bits.AESNI);
  Print (L"    XSAVE               : %x\n", Ecx.Bits.XSAVE);
  Print (L"    OSXSAVE             : %x\n", Ecx.Bits.OSXSAVE);
  Print (L"    AVX                 : %x\n", Ecx.Bits.AVX);
  Print (L"    F16C                : %x\n", Ecx.Bits.F16C);
  Print (L"    RDRAND              : %x\n", Ecx.Bits.RDRAND);

  Print (L"  EDX :\n");
  Print (L"    FPU    : %x\n",Edx.Bits.FPU);
  Print (L"    VME    : %x\n",Edx.Bits.VME);
  Print (L"    DE     : %x\n",Edx.Bits.DE);
  Print (L"    PSE    : %x\n",Edx.Bits.PSE);
  Print (L"    TSC    : %x\n",Edx.Bits.TSC);
  Print (L"    MSR    : %x\n",Edx.Bits.MSR);
  Print (L"    PAE    : %x\n",Edx.Bits.PAE);
  Print (L"    MCE    : %x\n",Edx.Bits.MCE);
  Print (L"    CX8    : %x\n",Edx.Bits.CX8);
  Print (L"    APIC   : %x\n",Edx.Bits.APIC);
  Print (L"    SEP    : %x\n",Edx.Bits.SEP);
  Print (L"    MTRR   : %x\n",Edx.Bits.MTRR);
  Print (L"    PGE    : %x\n",Edx.Bits.PGE);
  Print (L"    MCA    : %x\n",Edx.Bits.MCA);
  Print (L"    CMOV   : %x\n",Edx.Bits.CMOV);
  Print (L"    PAT    : %x\n",Edx.Bits.PAT);
  Print (L"    PSE_36 : %x\n",Edx.Bits.PSE_36);
  Print (L"    PSN    : %x\n",Edx.Bits.PSN);
  Print (L"    CLFSH  : %x\n",Edx.Bits.CLFSH);
  Print (L"    DS     : %x\n",Edx.Bits.DS);
  Print (L"    ACPI   : %x\n",Edx.Bits.ACPI);
  Print (L"    MMX    : %x\n",Edx.Bits.MMX);
  Print (L"    FXSR   : %x\n",Edx.Bits.FXSR);
  Print (L"    SSE    : %x\n",Edx.Bits.SSE);
  Print (L"    SSE2   : %x\n",Edx.Bits.SSE2);
  Print (L"    SS     : %x\n",Edx.Bits.SS);
  Print (L"    HTT    : %x\n",Edx.Bits.HTT);
  Print (L"    TM     : %x\n",Edx.Bits.TM);
  Print (L"    PBE    : %x\n",Edx.Bits.PBE);
  Print (L"\n");
}


VOID
EFIAPI
CacheInfo (
  VOID
  )
{
  CPUID_CACHE_INFO_CACHE_TLB  Eax;
  CPUID_CACHE_INFO_CACHE_TLB  Ebx;
  CPUID_CACHE_INFO_CACHE_TLB  Ecx;
  CPUID_CACHE_INFO_CACHE_TLB  Edx;
  UINT8 Index;
  UINT8 I;

  if (CPUID_CACHE_INFO > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_CACHE_INFO, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  
  Print (L"Function 0x02 - CPUID_CACHE_INFO : ");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
         Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32
         );
  
  if (Eax.Bits.NotValid == 0) {
    Print (L"  Eax CacheDescriptor :\n");
    for (I = 1; I < 4; I++) {
      Index = SearchCacheDesc (Eax.CacheDescriptor[I]);
      Print (
        L"    %02x  %-8a  %a\n",
        Eax.CacheDescriptor[I], 
        gCacheInfoDesc[Index].Type, 
        gCacheInfoDesc[Index].Description
        );
    }
  }

  if (Ebx.Bits.NotValid == 0) {
    Print (L"  Ebx CacheDescriptor :\n");
    for (I = 0; I < 4; I++) {
      Index = SearchCacheDesc (Ebx.CacheDescriptor[I]);
      Print (
        L"    %02x  %-8a  %a\n",
        Ebx.CacheDescriptor[I], 
        gCacheInfoDesc[Index].Type, 
        gCacheInfoDesc[Index].Description
        );
    }
  }

  if (Ecx.Bits.NotValid == 0) {
    Print (L"  Ecx CacheDescriptor :\n");
    for (I = 0; I < 4; I++) {
      Index = SearchCacheDesc (Ecx.CacheDescriptor[I]);
      Print (
        L"    %02x  %-8a  %a\n",
        Ecx.CacheDescriptor[I], 
        gCacheInfoDesc[Index].Type, 
        gCacheInfoDesc[Index].Description
        );
    }
  }

  if (Edx.Bits.NotValid == 0) {
    Print (L"  Edx CacheDescriptor :\n");
    for (I = 0; I < 4; I++) {
      Index = SearchCacheDesc (Edx.CacheDescriptor[I]);
      Print (
        L"    %02x  %-8a  %a\n",
        Edx.CacheDescriptor[I], 
        gCacheInfoDesc[Index].Type,
        gCacheInfoDesc[Index].Description
        );
    }
  }
  Print (L"\n");
}


VOID
EFIAPI
SerialNumber (
  VOID
  )
{
  CPUID_VERSION_INFO_EDX  VersionInfoEdx;
  UINT32                  Ecx;
  UINT32                  Edx;

  if (CPUID_VERSION_INFO > gMaxBasic) {
    return;
  }

  Print (L"Function 0x03 - CPUID_SERIAL_NUMBER :\n");
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, &VersionInfoEdx.Uint32);
  if (VersionInfoEdx.Bits.PSN == 0) {
    Print (L"  Not Supported\n");
    return;
  }

  AsmCpuid (CPUID_SERIAL_NUMBER, NULL, NULL, &Ecx, &Edx);
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 0, 0, Ecx, Edx);
  Print (L"  Processor Serial Number = %08x%08x%08x\n", 0, Edx, Ecx);
  Print (L"\n");
}

VOID
EFIAPI
CacheParams (
  VOID
  ) 
{
  UINT32                  CacheLevel;
  CPUID_CACHE_PARAMS_EAX  Eax;
  CPUID_CACHE_PARAMS_EBX  Ebx;
  UINT32                  Ecx;
  CPUID_CACHE_PARAMS_EDX  Edx;

  if (CPUID_CACHE_PARAMS > gMaxBasic) {
    return;
  }

  CacheLevel = 0;
  Print (L"Function 0x04 - CPUID_CACHE_PARAMS :\n");
  do {
    AsmCpuidEx (
      CPUID_CACHE_PARAMS, CacheLevel,
      &Eax.Uint32, &Ebx.Uint32, &Ecx, &Edx.Uint32
      );
    if (Eax.Bits.CacheType != CPUID_CACHE_PARAMS_CACHE_TYPE_NULL) {
      Print (L"CacheLevel %x:\n", CacheLevel);
      Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
             Eax.Uint32, Ebx.Uint32, Ecx, Edx.Uint32
             );
      Print (L"  EAX :\n");
      Print (L"    CacheType                                 : %x\n", Eax.Bits.CacheType);
      Print (L"    CacheLevel                                : %x\n", Eax.Bits.CacheLevel);
      Print (L"    SelfInitializingCache                     : %x\n", Eax.Bits.SelfInitializingCache);
      Print (L"    FullyAssociativeCache                     : %x\n", Eax.Bits.FullyAssociativeCache);
      Print (L"    MaximumAddressableIdsForLogicalProcessors : %x\n", Eax.Bits.MaximumAddressableIdsForLogicalProcessors);
      Print (L"    MaximumAddressableIdsForProcessorCores    : %x\n", Eax.Bits.MaximumAddressableIdsForProcessorCores);
      Print (L"  EBX :\n");
      Print (L"    LineSize                                  : %x\n", Ebx.Bits.LineSize);
      Print (L"    LinePartitions                            : %x\n", Ebx.Bits.LinePartitions);
      Print (L"    Ways                                      : %x\n", Ebx.Bits.Ways);
      Print (L"  ECX :\n");
      Print (L"    NumberOfSets                              : %x\n", Ecx);
      Print (L"  EDX :\n");
      Print (L"    Invalidate                                : %x\n", Edx.Bits.Invalidate);
      Print (L"    CacheInclusiveness                        : %x\n", Edx.Bits.CacheInclusiveness);
      Print (L"    ComplexCacheIndexing                      : %x\n", Edx.Bits.ComplexCacheIndexing);
    }
    CacheLevel++;
  } while (Eax.Bits.CacheType != CPUID_CACHE_PARAMS_CACHE_TYPE_NULL);
  Print (L"\n");
}


VOID
EFIAPI
MonitorMwait (
  VOID
  )
{
  CPUID_MONITOR_MWAIT_EAX  Eax;
  CPUID_MONITOR_MWAIT_EBX  Ebx;
  CPUID_MONITOR_MWAIT_ECX  Ecx;
  CPUID_MONITOR_MWAIT_EDX  Edx;

  if (CPUID_MONITOR_MWAIT > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_MONITOR_MWAIT, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);

  Print (L"Function 0x05 - CPUID_MONITOR_MWAIT :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
           Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32
           );
  Print (L"  EAX :\n");
  Print (L"    SmallestMonitorLineSize : %x\n", Eax.Bits.SmallestMonitorLineSize);
  Print (L"  EBX :\n");
  Print (L"    LargestMonitorLineSize  : %x\n", Ebx.Bits.LargestMonitorLineSize);
  Print (L"  ECX :\n");
  Print (L"    ExtensionsSupported     : %x\n", Ecx.Bits.ExtensionsSupported);
  Print (L"    InterruptAsBreak        : %x\n", Ecx.Bits.InterruptAsBreak);
  Print (L"  EDX :\n");
  Print (L"    C0States                : %x\n", Edx.Bits.C0States);
  Print (L"    C1States                : %x\n", Edx.Bits.C1States);
  Print (L"    C2States                : %x\n", Edx.Bits.C2States);
  Print (L"    C3States                : %x\n", Edx.Bits.C3States);
  Print (L"    C4States                : %x\n", Edx.Bits.C4States);
  Print (L"    C5States                : %x\n", Edx.Bits.C5States);
  Print (L"    C6States                : %x\n", Edx.Bits.C6States);
  Print (L"    C7States                : %x\n", Edx.Bits.C7States);
  Print (L"\n");
}

VOID
EFIAPI
ThermalPowerManagement (
  VOID
  )
{
  CPUID_THERMAL_POWER_MANAGEMENT_EAX  Eax;
  CPUID_THERMAL_POWER_MANAGEMENT_EBX  Ebx;
  CPUID_THERMAL_POWER_MANAGEMENT_ECX  Ecx;

  if (CPUID_THERMAL_POWER_MANAGEMENT > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_THERMAL_POWER_MANAGEMENT, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, NULL);

  Print (L"Function 0x06 - CPUID_THERMAL_POWER_MANAGEMENT :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx.Uint32, Ecx.Uint32, 0);
  Print (L"  EAX :\n");
  Print (L"    DigitalTemperatureSensor               : %x\n", Eax.Bits.DigitalTemperatureSensor);
  Print (L"    TurboBoostTechnology                   : %x\n", Eax.Bits.TurboBoostTechnology);
  Print (L"    ARAT                                   : %x\n", Eax.Bits.ARAT);
  Print (L"    PLN                                    : %x\n", Eax.Bits.PLN);
  Print (L"    ECMD                                   : %x\n", Eax.Bits.ECMD);
  Print (L"    PTM                                    : %x\n", Eax.Bits.PTM);
  Print (L"    HWP                                    : %x\n", Eax.Bits.HWP);
  Print (L"    HWP_Notification                       : %x\n", Eax.Bits.HWP_Notification);
  Print (L"    HWP_Activity_Window                    : %x\n", Eax.Bits.HWP_Activity_Window);
  Print (L"    HWP_Energy_Performance_Preference      : %x\n", Eax.Bits.HWP_Energy_Performance_Preference);
  Print (L"    HWP_Package_Level_Request              : %x\n", Eax.Bits.HWP_Package_Level_Request);
  Print (L"    HDC                                    : %x\n", Eax.Bits.HDC);
  Print (L"    TurboBoostMaxTechnology30              : %x\n", Eax.Bits.TurboBoostMaxTechnology30);
  Print (L"    HWPCapabilities                        : %x\n", Eax.Bits.HWPCapabilities);
  Print (L"    HWPPECIOverride                        : %x\n", Eax.Bits.HWPPECIOverride);
  Print (L"    FlexibleHWP                            : %x\n", Eax.Bits.FlexibleHWP);
  Print (L"    FastAccessMode                         : %x\n", Eax.Bits.FastAccessMode);
  Print (L"    IgnoringIdleLogicalProcessorHWPRequest : %x\n", Eax.Bits.IgnoringIdleLogicalProcessorHWPRequest);
  Print (L"  EBX :\n");
  Print (L"    InterruptThresholds                    : %x\n", Ebx.Bits.InterruptThresholds);
  Print (L"  ECX :\n");
  Print (L"    HardwareCoordinationFeedback           : %x\n", Ecx.Bits.HardwareCoordinationFeedback);
  Print (L"    PerformanceEnergyBias                  : %x\n", Ecx.Bits.PerformanceEnergyBias);
  Print (L"\n");
}


VOID
EFIAPI
StructuredExtendedFeatureFlags (
  VOID
  )
{
  UINT32                                       Eax;
  CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_EBX  Ebx;
  CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_ECX  Ecx;
  CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_EDX  Edx;
  UINT32                                       Index;

  if (CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS,
    CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO,
    &Eax, NULL, NULL, NULL
    );

  Print (L"Function 0x07 - CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS \n");
  for (Index = 0; Index <= Eax; Index++) {
    AsmCpuidEx (
      CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS,
      Index,
      NULL, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32
      );
    if (Ebx.Uint32 != 0 || Ecx.Uint32 != 0 || Edx.Uint32 != 0) {
      Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, Ebx.Uint32, Ecx.Uint32, Edx.Uint32);
      Print (L"  EBX :\n");
      Print (L"    FSGSBASE              : %x\n", Ebx.Bits.FSGSBASE);
      Print (L"    IA32_TSC_ADJUST       : %x\n", Ebx.Bits.IA32_TSC_ADJUST);
      Print (L"    SGX                   : %x\n", Ebx.Bits.SGX);
      Print (L"    BMI1                  : %x\n", Ebx.Bits.BMI1);
      Print (L"    HLE                   : %x\n", Ebx.Bits.HLE);
      Print (L"    AVX2                  : %x\n", Ebx.Bits.AVX2);
      Print (L"    FDP_EXCPTN_ONLY       : %x\n", Ebx.Bits.FDP_EXCPTN_ONLY);
      Print (L"    SMEP                  : %x\n", Ebx.Bits.SMEP);
      Print (L"    BMI2                  : %x\n", Ebx.Bits.BMI2);
      Print (L"    EnhancedRepMovsbStosb : %x\n", Ebx.Bits.EnhancedRepMovsbStosb);
      Print (L"    INVPCID               : %x\n", Ebx.Bits.INVPCID);
      Print (L"    RTM                   : %x\n", Ebx.Bits.RTM);
      Print (L"    RDT_M                 : %x\n", Ebx.Bits.RDT_M);
      Print (L"    DeprecateFpuCsDs      : %x\n", Ebx.Bits.DeprecateFpuCsDs);
      Print (L"    MPX                   : %x\n", Ebx.Bits.MPX);
      Print (L"    RDT_A                 : %x\n", Ebx.Bits.RDT_A);
      Print (L"    AVX512F               : %x\n", Ebx.Bits.AVX512F);
      Print (L"    AVX512DQ              : %x\n", Ebx.Bits.AVX512DQ);
      Print (L"    RDSEED                : %x\n", Ebx.Bits.RDSEED);
      Print (L"    ADX                   : %x\n", Ebx.Bits.ADX);
      Print (L"    SMAP                  : %x\n", Ebx.Bits.SMAP);
      Print (L"    AVX512_IFMA           : %x\n", Ebx.Bits.AVX512_IFMA);
      Print (L"    CLFLUSHOPT            : %x\n", Ebx.Bits.CLFLUSHOPT);
      Print (L"    CLWB                  : %x\n", Ebx.Bits.CLWB);
      Print (L"    IntelProcessorTrace   : %x\n", Ebx.Bits.IntelProcessorTrace);
      Print (L"    AVX512PF              : %x\n", Ebx.Bits.AVX512PF);
      Print (L"    AVX512ER              : %x\n", Ebx.Bits.AVX512ER);
      Print (L"    AVX512CD              : %x\n", Ebx.Bits.AVX512CD);
      Print (L"    SHA                   : %x\n", Ebx.Bits.SHA);
      Print (L"    AVX512BW              : %x\n", Ebx.Bits.AVX512BW);
      Print (L"    AVX512VL              : %x\n", Ebx.Bits.AVX512VL);
      Print (L"  ECX :\n");
      Print (L"    PREFETCHWT1      : %x\n", Ecx.Bits.PREFETCHWT1);
      Print (L"    AVX512_VBMI      : %x\n", Ecx.Bits.AVX512_VBMI);
      Print (L"    UMIP             : %x\n", Ecx.Bits.UMIP);
      Print (L"    PKU              : %x\n", Ecx.Bits.PKU);
      Print (L"    OSPKE            : %x\n", Ecx.Bits.OSPKE);
      Print (L"    AVX512_VPOPCNTDQ : %x\n", Ecx.Bits.AVX512_VPOPCNTDQ);
      Print (L"    MAWAU            : %x\n", Ecx.Bits.MAWAU);
      Print (L"    RDPID            : %x\n", Ecx.Bits.RDPID);
      Print (L"    SGX_LC           : %x\n", Ecx.Bits.SGX_LC);
      Print (L"  EDX :\n");
      Print (L"    AVX512_4VNNIW                   : %x\n", Edx.Bits.AVX512_4VNNIW);
      Print (L"    AVX512_4VNNIW                   : %x\n", Edx.Bits.AVX512_4FMAPS);
      Print (L"    EnumeratesSupportForIBRSAndIBPB : %x\n", Edx.Bits.EnumeratesSupportForIBRSAndIBPB);
      Print (L"    EnumeratesSupportForSTIBP       : %x\n", Edx.Bits.EnumeratesSupportForSTIBP);
      Print (L"    EnumeratesSupportForL1D_FLUSH   : %x\n", Edx.Bits.EnumeratesSupportForL1D_FLUSH);
      Print (L"    EnumeratesSupportForCapability  : %x\n", Edx.Bits.EnumeratesSupportForCapability);
      Print (L"    EnumeratesSupportForSSBD        : %x\n", Edx.Bits.EnumeratesSupportForSSBD);
      Print (L"\n");
    }
  }
}


VOID
EFIAPI
DirectCacheAccessInfo (
  VOID
  )
{
  UINT32  Eax;

  if (CPUID_DIRECT_CACHE_ACCESS_INFO > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_DIRECT_CACHE_ACCESS_INFO, &Eax, NULL, NULL, NULL);
  Print (L"Function 0x09 - CPUID_DIRECT_CACHE_ACCESS_INFO :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, 0, 0, 0);
  Print (L"\n");
}


VOID
EFIAPI
ArchitecturalPerformanceMonitoring (
  VOID
  )
{
  CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING_EAX  Eax;
  CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING_EBX  Ebx;
  CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING_EDX  Edx;

  if (CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING, &Eax.Uint32, &Ebx.Uint32, NULL, &Edx.Uint32);
  Print (L"Function 0x0A - CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx.Uint32, 0, Edx.Uint32);
  Print (L"  EAX :\n");
  Print (L"    ArchPerfMonVerID               : %x\n", Eax.Bits.ArchPerfMonVerID);
  Print (L"    PerformanceMonitorCounters     : %x\n", Eax.Bits.PerformanceMonitorCounters);
  Print (L"    PerformanceMonitorCounterWidth : %x\n", Eax.Bits.PerformanceMonitorCounterWidth);
  Print (L"    EbxBitVectorLength             : %x\n", Eax.Bits.EbxBitVectorLength);
  Print (L"  EBX :\n");
  Print (L"    UnhaltedCoreCycles         : %x\n", Ebx.Bits.UnhaltedCoreCycles);
  Print (L"    InstructionsRetired        : %x\n", Ebx.Bits.InstructionsRetired);
  Print (L"    UnhaltedReferenceCycles    : %x\n", Ebx.Bits.UnhaltedReferenceCycles);
  Print (L"    LastLevelCacheReferences   : %x\n", Ebx.Bits.LastLevelCacheReferences);
  Print (L"    LastLevelCacheMisses       : %x\n", Ebx.Bits.LastLevelCacheMisses);
  Print (L"    BranchInstructionsRetired  : %x\n", Ebx.Bits.BranchInstructionsRetired);
  Print (L"    AllBranchMispredictRetired : %x\n", Ebx.Bits.AllBranchMispredictRetired);
  Print (L"  EDX :\n");
  Print (L"    FixedFunctionPerformanceCounters     : %x\n", Edx.Bits.FixedFunctionPerformanceCounters);
  Print (L"    FixedFunctionPerformanceCounterWidth : %x\n", Edx.Bits.FixedFunctionPerformanceCounterWidth);
  Print (L"    AnyThreadDeprecation                 : %x\n", Edx.Bits.AnyThreadDeprecation);
  Print (L"\n");
}


VOID
EFIAPI
ExtendedTopology (
  VOID
  )
{
  CPUID_EXTENDED_TOPOLOGY_EAX  Eax;
  CPUID_EXTENDED_TOPOLOGY_EBX  Ebx;
  CPUID_EXTENDED_TOPOLOGY_ECX  Ecx;
  UINT32                       Edx;
  UINT32                       LevelNumber;

  if (CPUID_EXTENDED_TOPOLOGY > gMaxBasic) {
    return;
  }
  
  Print (L"Function 0x0B - CPUID_EXTENDED_TOPOLOGY :\n");

  LevelNumber = 0;
  for (LevelNumber = 0; ; LevelNumber++) {
    AsmCpuidEx (
      CPUID_EXTENDED_TOPOLOGY, LevelNumber,
      &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx
      );
    if (Ecx.Bits.LevelType == CPUID_EXTENDED_TOPOLOGY_LEVEL_TYPE_INVALID) {
      break;
    }
    Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx);
    Print (L"    ApicIdShift       : %x\n", Eax.Bits.ApicIdShift);
    Print (L"    LogicalProcessors : %x\n", Ebx.Bits.LogicalProcessors);
    Print (L"    LevelNumber       : %x\n", Ecx.Bits.LevelNumber);
    Print (L"    LevelType         : %x\n", Ecx.Bits.LevelType);
    Print (L"    x2APIC_ID         : %x\n", Edx);
  }
  Print (L"\n");
}


VOID
ExtendedStateSub (
  VOID
  )
{
  CPUID_EXTENDED_STATE_SUB_LEAF_EAX  Eax;
  UINT32                             Ebx;
  CPUID_EXTENDED_STATE_SUB_LEAF_ECX  Ecx;
  UINT32                             Edx;

  AsmCpuidEx (
    CPUID_EXTENDED_STATE, CPUID_EXTENDED_STATE_SUB_LEAF,
    &Eax.Uint32, &Ebx, &Ecx.Uint32, &Edx
    );
  Print (L"Function 0x0D - CPUID_EXTENDED_STATE_SUB_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx, Ecx.Uint32, Edx);
  Print (L"  EAX :\n");
  Print (L"    XSAVEOPT : %x\n", Eax.Bits.XSAVEOPT);
  Print (L"    XSAVEC   : %x\n", Eax.Bits.XSAVEC);
  Print (L"    XGETBV   : %x\n", Eax.Bits.XGETBV);
  Print (L"    XSAVES   : %x\n", Eax.Bits.XSAVES);
  Print (L"  EBX :\n");
  Print (L"    EnabledSaveStateSize_XCR0_IA32_XSS : %x\n", Ebx);
  Print (L"  ECX :\n");
  Print (L"    XCR0     : %x\n", Ecx.Bits.XCR0);
  Print (L"    HWPState : %x\n", Ecx.Bits.HWPState);
  Print (L"    PT       : %x\n", Ecx.Bits.PT);
  Print (L"    XCR0_1   : %x\n", Ecx.Bits.XCR0_1);
  Print (L"  EDX :\n");
  Print (L"    IA32_XSS_Supported : %x\n", Edx);
  Print (L"\n");
}


VOID
ExtendedStateSizeOffset (
  VOID
  )
{
  UINT32                                Eax;
  UINT32                                Ebx;
  CPUID_EXTENDED_STATE_SIZE_OFFSET_ECX  Ecx;
  UINT32                                Edx;
  UINT32                                Sub;
  BOOLEAN                               First;
  
  First = TRUE;
  for (Sub = CPUID_EXTENDED_STATE_SIZE_OFFSET; Sub < 32; Sub++) {
    AsmCpuidEx (
      CPUID_EXTENDED_STATE, Sub,
      &Eax, &Ebx, &Ecx.Uint32, &Edx
      );
    if (Edx != 0) {
      if (First) {
        Print (L"Function 0x0D - CPUID_EXTENDED_STATE_SIZE_OFFSET\n");
        First = FALSE;
      }
      Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, Ebx, Ecx.Uint32, Edx);
      Print (L"    FeatureSaveStateSize   : %x\n", Eax);
      Print (L"    FeatureSaveStateOffset : %x\n", Ebx);
      Print (L"    XSS                    : %x\n", Ecx.Bits.XSS);
      Print (L"    Compacted              : %x\n", Ecx.Bits.Compacted);
    }
  }

  if (!First) {
    Print (L"\n");
  }
}


VOID
EFIAPI
ExtendedStateMain (
  VOID
  )
{
  CPUID_EXTENDED_STATE_MAIN_LEAF_EAX  Eax;
  UINT32                              Ebx;
  UINT32                              Ecx;
  UINT32                              Edx;

  if (CPUID_EXTENDED_STATE > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_EXTENDED_STATE, CPUID_EXTENDED_STATE_MAIN_LEAF,
    &Eax.Uint32, &Ebx, &Ecx, &Edx
    );
  Print (L"Function 0x0D - CPUID_EXTENDED_STATE_MAIN_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx, Ecx, Edx);
  Print (L"    x87                    : %x\n", Eax.Bits.x87);
  Print (L"    SSE                    : %x\n", Eax.Bits.SSE);
  Print (L"    AVX                    : %x\n", Eax.Bits.AVX);
  Print (L"    MPX                    : %x\n", Eax.Bits.MPX);
  Print (L"    AVX_512                : %x\n", Eax.Bits.AVX_512);
  Print (L"    IA32_XSS               : %x\n", Eax.Bits.IA32_XSS);
  Print (L"    PKRU                   : %x\n", Eax.Bits.PKRU);
  Print (L"    IA32_XSS_2             : %x\n", Eax.Bits.IA32_XSS_2);
  Print (L"    EnabledSaveStateSize   : %x\n", Ebx);
  Print (L"    SupportedSaveStateSize : %x\n", Ecx);
  Print (L"    XCR0_Supported_32_63   : %x\n", Edx);
  Print (L"\n");

  ExtendedStateSub ();
  ExtendedStateSizeOffset ();
}


VOID
EFIAPI
IntelRdtMonitoringEnumeration (
  VOID
  )
{
  UINT32                                                  Ebx;
  CPUID_INTEL_RDT_MONITORING_ENUMERATION_SUB_LEAF_EDX     Edx;

  if (CPUID_INTEL_RDT_MONITORING > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_INTEL_RDT_MONITORING, CPUID_INTEL_RDT_MONITORING_ENUMERATION_SUB_LEAF,
    NULL, &Ebx, NULL, &Edx.Uint32
    );
  Print (L"Function 0x0F - CPUID_INTEL_RDT_MONITORING_ENUMERATION_SUB_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 0, Ebx, 0, Edx.Uint32);
  Print (L"    Maximum_RMID_Range : %x\n", Ebx);
  Print (L"    L3CacheRDT_M       : %x\n", Edx.Bits.L3CacheRDT_M);
  Print (L"\n");
}


VOID
IntelRdtMonitoringL3Cache (
  VOID
  )
{
  UINT32                                                 Ebx;
  UINT32                                                 Ecx;
  CPUID_INTEL_RDT_MONITORING_L3_CACHE_SUB_LEAF_EDX       Edx;

  if (CPUID_INTEL_RDT_MONITORING > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_INTEL_RDT_MONITORING, CPUID_INTEL_RDT_MONITORING_L3_CACHE_SUB_LEAF,
    NULL, &Ebx, &Ecx, &Edx.Uint32
    );
  Print (L"Function 0x0F - CPUID_INTEL_RDT_MONITORING_L3_CACHE_SUB_LEAF\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 0, Ebx, Ecx, Edx.Uint32);
  Print (L"    OccupancyConversionFactor       : %x\n", Ebx);
  Print (L"    Maximum_RMID_Range              : %x\n", Ecx);
  Print (L"    L3CacheOccupancyMonitoring      : %x\n", Edx.Bits.L3CacheOccupancyMonitoring);
  Print (L"    L3CacheTotalBandwidthMonitoring : %x\n", Edx.Bits.L3CacheTotalBandwidthMonitoring);
  Print (L"    L3CacheLocalBandwidthMonitoring : %x\n", Edx.Bits.L3CacheLocalBandwidthMonitoring);
  Print (L"\n");
}


VOID
EFIAPI
IntelRdtAllocationMemoryBandwidth (
  VOID
  )
{
  CPUID_INTEL_RDT_ALLOCATION_MEMORY_BANDWIDTH_SUB_LEAF_EAX  Eax;
  UINT32                                                    Ebx;
  CPUID_INTEL_RDT_ALLOCATION_MEMORY_BANDWIDTH_SUB_LEAF_ECX  Ecx;
  CPUID_INTEL_RDT_ALLOCATION_MEMORY_BANDWIDTH_SUB_LEAF_EDX  Edx;

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION, CPUID_INTEL_RDT_ALLOCATION_MEMORY_BANDWIDTH_SUB_LEAF,
    &Eax.Uint32, &Ebx, &Ecx.Uint32, &Edx.Uint32
    );
  Print (L"Function 0x10 - CPUID_INTEL_RDT_ALLOCATION_MEMORY_BANDWIDTH_SUB_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx, Ecx.Uint32, Edx.Uint32);
  Print (L"    MaximumMBAThrottling : %x\n", Eax.Bits.MaximumMBAThrottling);
  Print (L"    AllocationUnitBitMap : %x\n", Ebx);
  Print (L"    Liner                : %x\n", Ecx.Bits.Liner);
  Print (L"    HighestCosNumber     : %x\n", Edx.Bits.HighestCosNumber);
  Print (L"\n");
}


VOID
IntelRdtAllocationL3Cache (
  VOID
  )
{
  CPUID_INTEL_RDT_ALLOCATION_L3_CACHE_SUB_LEAF_EAX  Eax;
  UINT32                                            Ebx;
  CPUID_INTEL_RDT_ALLOCATION_L3_CACHE_SUB_LEAF_ECX  Ecx;
  CPUID_INTEL_RDT_ALLOCATION_L3_CACHE_SUB_LEAF_EDX  Edx;

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION, CPUID_INTEL_RDT_ALLOCATION_L3_CACHE_SUB_LEAF,
    &Eax.Uint32, &Ebx, &Ecx.Uint32, &Edx.Uint32
    );
  Print (L"Function 0x10 - CPUID_INTEL_RDT_ALLOCATION_L3_CACHE_SUB_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx, Ecx.Uint32, Edx.Uint32);
  Print (L"    CapacityLength         : %x\n", Eax.Bits.CapacityLength);
  Print (L"    AllocationUnitBitMap   : %x\n", Ebx);
  Print (L"    CodeDataPrioritization : %x\n", Ecx.Bits.CodeDataPrioritization);
  Print (L"    HighestCosNumber       : %x\n", Edx.Bits.HighestCosNumber);
  Print (L"\n");
}


VOID
EFIAPI
IntelRdtAllocationL2Cache (
  VOID
  )
{
  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EAX  Eax;
  UINT32                                            Ebx;
  CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF_EDX  Edx;

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION, CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF,
    &Eax.Uint32, &Ebx, NULL, &Edx.Uint32
    );
  Print (L"Function 0x10 - CPUID_INTEL_RDT_ALLOCATION_L2_CACHE_SUB_LEAF\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx, 0, Edx.Uint32);
  Print (L"    CapacityLength       : %x\n", Eax.Bits.CapacityLength);
  Print (L"    AllocationUnitBitMap : %x\n", Ebx);
  Print (L"    HighestCosNumber     : %x\n", Edx.Bits.HighestCosNumber);
  Print (L"\n");
}


VOID
EFIAPI
IntelRdtAllocationMain (
  VOID
  )
{
  CPUID_INTEL_RDT_ALLOCATION_ENUMERATION_SUB_LEAF_EBX  Ebx;

  if (CPUID_INTEL_RDT_ALLOCATION > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_INTEL_RDT_ALLOCATION, CPUID_INTEL_RDT_ALLOCATION_ENUMERATION_SUB_LEAF,
    NULL, &Ebx.Uint32, NULL, NULL
    );
  Print (L"Function 0x10 - CPUID_INTEL_RDT_ALLOCATION :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 0, Ebx.Uint32, 0, 0);
  Print (L"    L3CacheAllocation : %x\n", Ebx.Bits.L3CacheAllocation);
  Print (L"    L2CacheAllocation : %x\n", Ebx.Bits.L2CacheAllocation);
  Print (L"    MemoryBandwidth   : %x\n", Ebx.Bits.MemoryBandwidth);
  Print (L"\n");
  IntelRdtAllocationL3Cache ();
  IntelRdtAllocationL2Cache ();
  IntelRdtAllocationMemoryBandwidth ();
}


VOID
EFIAPI
EnumerationOfIntelSgxCapabilities0 (
  VOID
  )
{
  CPUID_INTEL_SGX_CAPABILITIES_0_SUB_LEAF_EAX  Eax;
  UINT32                                       Ebx;
  CPUID_INTEL_SGX_CAPABILITIES_0_SUB_LEAF_EDX  Edx;

  AsmCpuidEx (
    CPUID_INTEL_SGX, CPUID_INTEL_SGX_CAPABILITIES_0_SUB_LEAF,
    &Eax.Uint32, &Ebx, NULL, &Edx.Uint32
    );
  Print (L"Function 0x12 - CPUID_INTEL_SGX_CAPABILITIES_0_SUB_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx, 0, Edx.Uint32);
  Print (L"  EAX :\n");
  Print (L"    SGX1  : %x\n", Eax.Bits.SGX1);
  Print (L"    SGX2  : %x\n", Eax.Bits.SGX2);
  Print (L"    ENCLV : %x\n", Eax.Bits.ENCLV);
  Print (L"    ENCLS : %x\n", Eax.Bits.ENCLS);
  Print (L"  EDX :\n");
  Print (L"    MaxEnclaveSize_Not64 : %x\n", Edx.Bits.MaxEnclaveSize_Not64);
  Print (L"    MaxEnclaveSize_64    : %x\n", Edx.Bits.MaxEnclaveSize_64);
  Print (L"\n");
}


VOID
EFIAPI
EnumerationOfIntelSgxCapabilities1 (
  VOID
  )
{
  UINT32                                       Eax;
  UINT32                                       Ebx;
  UINT32                                       Ecx;
  UINT32                                       Edx;

  AsmCpuidEx (
    CPUID_INTEL_SGX, CPUID_INTEL_SGX_CAPABILITIES_1_SUB_LEAF,
    &Eax, &Ebx, &Ecx, &Edx
    );
  Print (L"Function 0x12 - CPUID_INTEL_SGX_CAPABILITIES_1_SUB_LEAF :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, Ebx, Ecx, Edx);
  Print (L"\n");
}


VOID
EFIAPI
EnumerationOfIntelSgxResources (
  VOID
  )
{
  CPUID_INTEL_SGX_CAPABILITIES_RESOURCES_SUB_LEAF_EAX  Eax;
  CPUID_INTEL_SGX_CAPABILITIES_RESOURCES_SUB_LEAF_EBX  Ebx;
  CPUID_INTEL_SGX_CAPABILITIES_RESOURCES_SUB_LEAF_ECX  Ecx;
  CPUID_INTEL_SGX_CAPABILITIES_RESOURCES_SUB_LEAF_EDX  Edx;
  UINT32                                               SubLeaf;

  SubLeaf = CPUID_INTEL_SGX_CAPABILITIES_RESOURCES_SUB_LEAF;
  Print (L"Function 0x12 - CPUID_INTEL_SGX_CAPABILITIES_RESOURCES_SUB_LEAF :\n");
  do {
    AsmCpuidEx (
      CPUID_INTEL_SGX, SubLeaf,
      &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32
      );
    if (Eax.Bits.SubLeafType == 0x1) {
      Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
             Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32);
      Print (L"    SubLeafType             : %x\n", Eax.Bits.SubLeafType);
      Print (L"    LowAddressOfEpcSection  : %x\n", Eax.Bits.LowAddressOfEpcSection);
      Print (L"    HighAddressOfEpcSection : %x\n", Ebx.Bits.HighAddressOfEpcSection);
      Print (L"    EpcSection              : %x\n", Ecx.Bits.EpcSection);
      Print (L"    LowSizeOfEpcSection     : %x\n", Ecx.Bits.LowSizeOfEpcSection);
      Print (L"    HighSizeOfEpcSection    : %x\n", Edx.Bits.HighSizeOfEpcSection);
    }
    SubLeaf++;
  } while (Eax.Bits.SubLeafType == 0x1);
  Print (L"\n");
}


VOID
EFIAPI
EnumerationOfIntelSgx (
  VOID
  )
{
  CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_EBX  Ebx;

  if (CPUID_INTEL_SGX > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS,
    CPUID_STRUCTURED_EXTENDED_FEATURE_FLAGS_SUB_LEAF_INFO,
    NULL, &Ebx.Uint32, NULL, NULL
    );
  if (Ebx.Bits.SGX != 1) {
    return;
  }

  EnumerationOfIntelSgxCapabilities0 ();
  EnumerationOfIntelSgxCapabilities1 ();
  EnumerationOfIntelSgxResources ();
}


VOID
EFIAPI
IntelProcessorTraceSub (
  UINT32  MaxSub
  )
{
  UINT32                                    SubLeaf;
  CPUID_INTEL_PROCESSOR_TRACE_SUB_LEAF_EAX  Eax;
  CPUID_INTEL_PROCESSOR_TRACE_SUB_LEAF_EBX  Ebx;

  Print (L"Function 0x14 - CPUID_INTEL_PROCESSOR_TRACE_SUB_LEAF :\n");
  for (SubLeaf = CPUID_INTEL_PROCESSOR_TRACE_SUB_LEAF; SubLeaf <= MaxSub; SubLeaf++) {
    AsmCpuidEx (
      CPUID_INTEL_PROCESSOR_TRACE, SubLeaf,
      &Eax.Uint32, &Ebx.Uint32, NULL, NULL
      );
    Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx.Uint32, 0, 0);
    Print (L"    ConfigurableAddressRanges : %x\n", Eax.Bits.ConfigurableAddressRanges);
    Print (L"    MtcPeriodEncodings        : %x\n", Eax.Bits.MtcPeriodEncodings);
    Print (L"    CycleThresholdEncodings   : %x\n", Ebx.Bits.CycleThresholdEncodings);
    Print (L"    PsbFrequencyEncodings     : %x\n", Ebx.Bits.PsbFrequencyEncodings);
  }
  Print (L"\n");
}


VOID
EFIAPI
IntelProcessorTraceMain (
  VOID
  )
{
  UINT32                                     Eax;
  CPUID_INTEL_PROCESSOR_TRACE_MAIN_LEAF_EBX  Ebx;
  CPUID_INTEL_PROCESSOR_TRACE_MAIN_LEAF_ECX  Ecx;

  if (CPUID_INTEL_PROCESSOR_TRACE > gMaxBasic) {
    return;
  }

  AsmCpuidEx (
    CPUID_INTEL_PROCESSOR_TRACE, CPUID_INTEL_PROCESSOR_TRACE_MAIN_LEAF,
    &Eax, &Ebx.Uint32, &Ecx.Uint32, NULL
    );
  Print (L"Function 0x14 - CPUID_INTEL_PROCESSOR_TRACE :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, Ebx.Uint32, Ecx.Uint32, 0);
  Print (L"    MaximumSubLeaf : %x\n", Eax);
  Print (L"  EBX :\n");
  Print (L"    Cr3Filter            : %x\n", Ebx.Bits.Cr3Filter);
  Print (L"    ConfigurablePsb      : %x\n", Ebx.Bits.ConfigurablePsb);
  Print (L"    IpTraceStopFiltering : %x\n", Ebx.Bits.IpTraceStopFiltering);
  Print (L"    Mtc                  : %x\n", Ebx.Bits.Mtc);
  Print (L"    PTWrite              : %x\n", Ebx.Bits.PTWrite);
  Print (L"    PowerEventTrace      : %x\n", Ebx.Bits.PowerEventTrace);
  Print (L"  ECX :\n");
  Print (L"    RTIT                    : %x\n", Ecx.Bits.RTIT);
  Print (L"    ToPA                    : %x\n", Ecx.Bits.ToPA);
  Print (L"    SingleRangeOutput       : %x\n", Ecx.Bits.SingleRangeOutput);
  Print (L"    TraceTransportSubsystem : %x\n", Ecx.Bits.TraceTransportSubsystem);
  Print (L"    LIP                     : %x\n", Ecx.Bits.LIP);
  Print (L"\n");
  IntelProcessorTraceSub (Eax);
}


VOID
EFIAPI
TimeStampCounter (
  VOID
  )
{
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;

  if (CPUID_TIME_STAMP_COUNTER > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_TIME_STAMP_COUNTER, &Eax, &Ebx, &Ecx, NULL);
  Print (L"Function 0x15 - CPUID_TIME_STAMP_COUNTER :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, Ebx, Ecx, 0);
  Print (L"\n");
}

/**
  Display CPUID_PROCESSOR_FREQUENCY leaf.

**/
VOID
EFIAPI
ProcessorFrequency (
  VOID
  )
{
  CPUID_PROCESSOR_FREQUENCY_EAX  Eax;
  CPUID_PROCESSOR_FREQUENCY_EBX  Ebx;
  CPUID_PROCESSOR_FREQUENCY_ECX  Ecx;

  if (CPUID_PROCESSOR_FREQUENCY > gMaxBasic) {
    return;
  }

  AsmCpuid (CPUID_PROCESSOR_FREQUENCY, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, NULL);
  Print (L"Function 0x16 - CPUID_PROCESSOR_FREQUENCY :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, Ebx.Uint32, Ecx.Uint32, 0);
  Print (L"    ProcessorBaseFrequency : %x\n", Eax.Bits.ProcessorBaseFrequency);
  Print (L"    MaximumFrequency       : %x\n", Ebx.Bits.MaximumFrequency);
  Print (L"    BusFrequency           : %x\n", Ecx.Bits.BusFrequency);
  Print (L"\n");
}


VOID
EFIAPI
ExtendedFunction (
  VOID
  )
{
  UINT32  Eax;

  AsmCpuid (CPUID_EXTENDED_FUNCTION, &Eax, NULL, NULL, NULL);
  Print (L"Extended Function 80000000 - CPUID_EXTENDED_FUNCTION\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, 0, 0, 0);
  Print (L"  MaximumExtendedFunction : %x\n", Eax);
  Print (L"\n");

  gMaxExtended = Eax;
}


VOID
EFIAPI
ExtendedCpuSig (
  VOID
  )
{
  UINT32                      Eax;
  CPUID_EXTENDED_CPU_SIG_ECX  Ecx;
  CPUID_EXTENDED_CPU_SIG_EDX  Edx;

  if (CPUID_EXTENDED_CPU_SIG > gMaxExtended) {
    return;
  }

  AsmCpuid (CPUID_EXTENDED_CPU_SIG, &Eax, NULL, &Ecx.Uint32, &Edx.Uint32);
  Print (L"Extended Function 80000001 - CPUID_EXTENDED_CPU_SIG :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax, 0, Ecx.Uint32, Edx.Uint32);
  Print (L"  ECX :\n");
  Print (L"    LAHF_SAHF      : %x\n", Ecx.Bits.LAHF_SAHF);
  Print (L"    LZCNT          : %x\n", Ecx.Bits.LZCNT);
  Print (L"    PREFETCHW      : %x\n", Ecx.Bits.PREFETCHW);
  Print (L"  EDX :\n");
  Print (L"    SYSCALL_SYSRET : %x\n", Edx.Bits.SYSCALL_SYSRET);
  Print (L"    NX             : %x\n", Edx.Bits.NX);
  Print (L"    Page1GB        : %x\n", Edx.Bits.Page1GB);
  Print (L"    RDTSCP         : %x\n", Edx.Bits.RDTSCP);
  Print (L"    LM             : %x\n", Edx.Bits.LM);
  Print (L"\n");
}


VOID
EFIAPI
BrandString (
  VOID
  )
{
  CPUID_BRAND_STRING_DATA  Eax;
  CPUID_BRAND_STRING_DATA  Ebx;
  CPUID_BRAND_STRING_DATA  Ecx;
  CPUID_BRAND_STRING_DATA  Edx;
  UINT32 BrandString[13];

  if (CPUID_BRAND_STRING1 > gMaxExtended) {
    return;
  }

  AsmCpuid (CPUID_BRAND_STRING1, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  Print (L"Extended Function 80000002 - CPUID_BRAND_STRING1 :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
         Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32
         );
  BrandString[0] = Eax.Uint32;
  BrandString[1] = Ebx.Uint32;
  BrandString[2] = Ecx.Uint32;
  BrandString[3] = Edx.Uint32;

  AsmCpuid (CPUID_BRAND_STRING2, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  Print (L"Extended Function 80000003 - CPUID_BRAND_STRING2 :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
         Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32
         );
  BrandString[4] = Eax.Uint32;
  BrandString[5] = Ebx.Uint32;
  BrandString[6] = Ecx.Uint32;
  BrandString[7] = Edx.Uint32;

  AsmCpuid (CPUID_BRAND_STRING3, &Eax.Uint32, &Ebx.Uint32, &Ecx.Uint32, &Edx.Uint32);
  Print (L"Extended Function 80000004 - CPUID_BRAND_STRING3 :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 
         Eax.Uint32, Ebx.Uint32, Ecx.Uint32, Edx.Uint32
         );
  BrandString[8]  = Eax.Uint32;
  BrandString[9]  = Ebx.Uint32;
  BrandString[10] = Ecx.Uint32;
  BrandString[11] = Edx.Uint32;

  BrandString[12] = 0;
  
  Print (L"Extended Function 80000002 ~ 80000004 - CPUID_BRAND_STRING :\n");
  Print (L"  Brand String : %a\n", (CHAR8*)BrandString);
  Print (L"\n");
}


VOID
EFIAPI
ExtendedCacheInfo (
  VOID
  )
{
  CPUID_EXTENDED_CACHE_INFO_ECX  Ecx;

  if (CPUID_EXTENDED_CACHE_INFO > gMaxExtended) {
    return;
  }

  AsmCpuid (CPUID_EXTENDED_CACHE_INFO, NULL, NULL, &Ecx.Uint32, NULL);
  Print (L"Extended Function 80000006 - CPUID_EXTENDED_CACHE_INFO :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 0, 0, Ecx.Uint32, 0);
  Print (L"  CacheLineSize   : %x\n", Ecx.Bits.CacheLineSize);
  Print (L"  L2Associativity : %x\n", Ecx.Bits.L2Associativity);
  Print (L"  CacheSize       : %x\n", Ecx.Bits.CacheSize);
  Print (L"\n");
}


VOID
EFIAPI
ExtendedTimeStampCounter (
  VOID
  )
{
  CPUID_EXTENDED_TIME_STAMP_COUNTER_EDX  Edx;

  if (CPUID_EXTENDED_TIME_STAMP_COUNTER > gMaxExtended) {
    return;
  }

  AsmCpuid (CPUID_EXTENDED_TIME_STAMP_COUNTER, NULL, NULL, NULL, &Edx.Uint32);
  Print (L"Extended Function 80000007 - CPUID_EXTENDED_TIME_STAMP_COUNTER\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", 0, 0, 0, Edx.Uint32);
  Print (L"    InvariantTsc : %x\n", Edx.Bits.InvariantTsc);
  Print (L"\n");
}


VOID
EFIAPI
VirPhyAddressSize (
  VOID
  )
{
  CPUID_VIR_PHY_ADDRESS_SIZE_EAX  Eax;

  if (CPUID_VIR_PHY_ADDRESS_SIZE > gMaxExtended) {
    return;
  }

  AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &Eax.Uint32, NULL, NULL, NULL);
  Print (L"Extended Function 80000008 - CPUID_VIR_PHY_ADDRESS_SIZE :\n");
  Print (L"  EAX:%08x  EBX:%08x  ECX:%08x  EDX:%08x\n", Eax.Uint32, 0, 0, 0);
  Print (L"  PhysicalAddressBits : %x\n", Eax.Bits.PhysicalAddressBits);
  Print (L"  LinearAddressBits   : %x\n", Eax.Bits.LinearAddressBits);
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Signature ();
  VersionInfo ();
  CacheInfo ();
  SerialNumber ();
  CacheParams ();
  MonitorMwait ();
  ThermalPowerManagement ();
  StructuredExtendedFeatureFlags ();
  DirectCacheAccessInfo ();
  ExtendedTopology ();
  ExtendedStateMain ();
  IntelRdtMonitoringEnumeration ();
  IntelRdtMonitoringL3Cache ();
  IntelRdtAllocationMain ();
  EnumerationOfIntelSgx ();
  IntelProcessorTraceMain ();
  TimeStampCounter ();
  ProcessorFrequency ();
  ExtendedFunction ();
  ExtendedCpuSig ();
  BrandString ();
  ExtendedCacheInfo ();
  ExtendedTimeStampCounter ();
  VirPhyAddressSize ();

  return EFI_SUCCESS;
}