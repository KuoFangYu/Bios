/** @file
  Course08-non-driver module driver .c source code

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
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include "PciProtocol.h"


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
  EFI_STATUS     Status;
  PCI_SCAN_PROTOCOL   *PciScanner;

  Status = gBS->LocateProtocol (
                  &gPciScanProtocolGuid,
                  NULL, 
                  (VOID **)&PciScanner
                  );

  if (EFI_ERROR(Status)) {
    Print (L"ERROR%d : Couldn't find PCI Scanner Protocol\n", Status);
    return Status;
  }

  //
  // call ScanPciDevices print pci device
  //
  Status = PciScanner->ScanPciDevices ();

  if (EFI_ERROR(Status)) {
    Print (L"Failed to scan PCI devices\n");
  }

  return Status;
}
