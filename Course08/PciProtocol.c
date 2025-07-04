/** @file
  Course08-non-driver module driver (Protocol) .c source code

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
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Pci22.h>
#include "PciProtocol.h"

/**
  read all PCI device and print the device which need SSID/SVID.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
ScanPciDevices () {
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo;
  EFI_STATUS                       Status;
  UINT64                           PciAddress;
  UINT16                           Bus;
  UINT16                           Dev;
  UINT16                           Func;
  PCI_TYPE00                       Type0Data;
  
  //
  // get the PciRootBridgeIo Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **)&PciRootBridgeIo
                  );

  if (EFI_ERROR(Status)) {
    Print (L"Couldn't find Protocol\n");
    return Status;
  }

  Print (L"Pci Device whilch required to fill SSID/SVID\n");
  Print (L"Bus    Device    Function    VendorId    DeviceID    BaseClass    SubClass\n");

  for (Bus = 0; Bus <= PCI_MAX_BUS; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Func= 0; Func <= PCI_MAX_FUNC; ++Func) {
        //
        // get start address
        //
        PciAddress = EFI_PCI_ADDRESS (
                       Bus, 
                       Dev, 
                       Func, 
                       0     // offset 0
                       );

        Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,    // This
                                    EfiPciWidthUint8,   // Width
                                    PciAddress,         // Address
                                    sizeof(PCI_TYPE00), // Count
                                    &Type0Data          // return data
                                    );

        if (EFI_ERROR(Status)) {
          continue;
        }

        //
        // shift left 1 make the bit 7 keep 0 so that only to compare HeaderType is 0 or not
        //
        Type0Data.Hdr.HeaderType = Type0Data.Hdr.HeaderType << 1;

        //
        // check the VendorId is exist and HeaderType is 0
        //
        if (Type0Data.Hdr.VendorId != 0XFFFF && Type0Data.Hdr.HeaderType == 0X00) {

          //
          // skip (base class = 6 or sub class = 0~4) and (base class = 8 or sub class = 0~3)
          //
          if ((Type0Data.Hdr.ClassCode[2] != 0X6 || Type0Data.Hdr.ClassCode[1] >= 0X5) &&
              (Type0Data.Hdr.ClassCode[2] != 0X8 || Type0Data.Hdr.ClassCode[1] >= 0X4)) {
            Print (
              L"%3d %9d %11d %11x %11x %12d %11d\n",
              Bus,
              Dev,
              Func,
              Type0Data.Hdr.VendorId,
              Type0Data.Hdr.DeviceId,
              Type0Data.Hdr.ClassCode[2],
              Type0Data.Hdr.ClassCode[1]
              );
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

PCI_SCAN_PROTOCOL gPciScanProtocol = {
  ScanPciDevices
};


/**
  The user Entry Point for Driver. The user code starts with this function
  as the real entry point for the Driver.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
PciProtocolEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  ) 
{
  EFI_STATUS Status;

  Status = gBS->InstallProtocolInterface(
                  &ImageHandle,
                  &gPciScanProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gPciScanProtocol
                  );

  if (EFI_ERROR(Status)) {
    Print(L"Failed to install PCI Scanner Protocol: %r\n", Status);
  } else {
    Print(L"PCI Scanner Protocol installed successfully.\n");
  }

  return Status;
}


/**
  The user unload for Driver.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
PciProtocolUnload (
  IN EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS Status;

  Status = gBS->UninstallProtocolInterface (
                  ImageHandle,
                  &gPciScanProtocolGuid,
                  &gPciScanProtocol
                  );

  if (EFI_ERROR(Status)) {
    Print(L"Failed to uninstall PCI Protocol: %r\n", Status);
  } else {
    Print(L"Success to uninstall PCI Protocol\n");
  }

  return Status;
}
