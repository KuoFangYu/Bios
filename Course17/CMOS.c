/** @file
 * Course2-05-CMOS .c source code
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

#include "CMOS.h"

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
  UINT8 Data;
  UINT8 Addr;
  UINT8 Count;
  UINT8 Row;
  EFI_STATUS Status;
  EFI_CPU_IO2_PROTOCOL *CpuIo2;

  Status = gBS->LocateProtocol (
                  &gEfiCpuIo2ProtocolGuid, 
                  NULL,
                  (VOID**)&CpuIo2
                  );
  if (EFI_ERROR (Status)) {
    Print (L"Locate protocol: %r\n", Status);
    return Status;
  }

  Count = 0;
  Print (L"   ");
  for (Count = 0; Count < 16; Count++) {
    Print (L"%02x ", Count);
  }

  Count = 0;
  Row = 0;
  Print (L"\n%02d ", Row);

  for (Addr = 0x00; Addr <= 0x7F; Addr++) {
    CpuIo2->Io.Write (
                 CpuIo2,
                 EfiCpuIoWidthUint8, 
                 0x70,
                 1,
                 &Addr
                 );

    CpuIo2->Io.Read (
                 CpuIo2, 
                 EfiCpuIoWidthUint8, 
                 0x71, 
                 1, 
                 &Data
                 );
    Print (L"%02x ", Data);
    
    Count++;
    if (Count == 16) {
      Count = 0;
      Row = Row + 10;
      Print (L"\n%02d ", Row);
    } // if
  } // for

  return Status;
}