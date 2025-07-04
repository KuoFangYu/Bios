/** @file
 * Course2-07-SMBus .c source code
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

#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>

#include <Protocol/SmbusHc.h>


/**
  get the given address's data and check spd 

  @param[in] SlaveAddress    the salve asddress
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval EFI_DEVICE_ERROR  not the spd
  @retval other             Some error when calling protocol

**/
EFI_STATUS
EFIAPI
ScanSPD (
  IN EFI_SMBUS_DEVICE_ADDRESS SlaveAddress
  )
{

  EFI_SMBUS_HC_PROTOCOL *Smbus;
  EFI_STATUS Status;
  UINTN SmbusDataLength;
  UINT8 SmbusData[260];
  UINT8 I;

  Status = gBS->LocateProtocol (
                  &gEfiSmbusHcProtocolGuid, 
                  NULL, 
                  (VOID **)&Smbus
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  SmbusDataLength = 256;
  // Execute the write operation
  Status = Smbus->Execute(
                    Smbus,
                    SlaveAddress,
                    0,
                    EfiSmbusReadByte,
                    FALSE,
                    &SmbusDataLength,
                    &SmbusData
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  if (SmbusData[0] == 0XFF) {
    return EFI_DEVICE_ERROR;
  }

  Print (L"   ");
  for (I = 0; I < 16; I++) {
    Print (L" %02x", I);
  }
  
  Print (L"\n 00");

  for (I = 0; I < 256; I++) {
    Print (L" %02x", SmbusData[I]);
    if (I == 255) {
      break;
    }    
    if ((I+1) % 16 == 0) {
      Print (L"\n %02x", I+1);
    }
  }

  return Status;

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
UefiMain(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  
  EFI_STATUS Status;
  EFI_SMBUS_DEVICE_ADDRESS SlaveAddress;
  UINT8 Addr;

  for (Addr = 0xA0; Addr <= 0xAE; Addr+=2) {
    SlaveAddress.SmbusDeviceAddress = (Addr >> 1);
    Status = ScanSPD (SlaveAddress);
    if (!EFI_ERROR (Status)) {
      Print (L"\n");
      Print (L"Address : %x\n\n", Addr);
    }
  }

  return EFI_SUCCESS;

}