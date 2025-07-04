/** @file
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
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/pci22.h>


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

  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo;
  EFI_STATUS                       Status;
  UINT64                           PciAddress;
  UINT16                           Bus;
  UINT16                           Dev;
  UINT16                           Func;
  PCI_TYPE00                       Type0Data;

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &PciRootBridgeIo
                  );

  if (EFI_ERROR (Status)) {
    Print (L"Couldn't find Protocol\n");
    return Status;
  }

  Print (L"  Bus    Device    Function    VendorId    SubsystemVendorID    SubsystemID\n");

  for (Bus = 0; Bus <= PCI_MAX_BUS; ++Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Func= 0; Func <= PCI_MAX_FUNC; ++Func) {

        PciAddress = EFI_PCI_ADDRESS (
                       Bus,
                       Dev,
                       Func,
                       0
                       );

        Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo, // This
                                    EfiPciWidthUint8, // Width
                                    PciAddress, // Address
                                    sizeof(PCI_TYPE00), // Count
                                    &Type0Data  // return data
                                    );

        Type0Data.Hdr.HeaderType = Type0Data.Hdr.HeaderType << 1;

        if (Type0Data.Hdr.VendorId != 0XFFFF && Type0Data.Hdr.HeaderType == 0X00) {
          if ((Type0Data.Hdr.ClassCode[2] != 0X6 || Type0Data.Hdr.ClassCode[1] >= 0X5) &&     // base || sub
             (Type0Data.Hdr.ClassCode[2] != 0X8 || Type0Data.Hdr.ClassCode[1] >= 0X4)) {

            Print (L"  %d        %d         %d          0X%x          0X%x            0x%x\n",
              Bus,
              Dev,
              Func,
              Type0Data.Hdr.VendorId,
              Type0Data.Device.SubsystemVendorID,
              Type0Data.Device.SubsystemID
              );
              
          } // if
        } // if
      } // for
    } // for
  } // for

  return EFI_SUCCESS;
  
} // main