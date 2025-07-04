/** @file
  Homework-UsbInfo .c source code

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
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>

#include <Protocol/UsbIo.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_DEVICE_DESCRIPTOR     DeviceDescriptor;

  //
  // Locate all handles support the USB I/O protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
      return Status;
  }

  //
  // Iterate through the handles and get the USB I/O protocol
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **)&UsbIo
                    );
    if (EFI_ERROR (Status)) {
        continue;
    }

    //
    // Get the device descriptor
    //
    Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DeviceDescriptor);
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Print device descriptor information
    //
    Print(L"USB Device %d:\n", Index);
    Print(L"  Length:          0x%02x\n", DeviceDescriptor.Length);
    Print(L"  Descriptor Type: 0x%02x\n", DeviceDescriptor.DescriptorType);
    Print(L"  BcdUSB:          0x%04x\n", DeviceDescriptor.BcdUSB);
    Print(L"  Device Class:    0x%02x\n", DeviceDescriptor.DeviceClass);
    Print(L"  Device SubClass: 0x%02x\n", DeviceDescriptor.DeviceSubClass);
    Print(L"  Device Protocol: 0x%02x\n", DeviceDescriptor.DeviceProtocol);
    Print(L"  Max Packet Size: %d\n", DeviceDescriptor.MaxPacketSize0);
    Print(L"  Vendor ID:       0x%04x\n", DeviceDescriptor.IdVendor);
    Print(L"  Product ID:      0x%04x\n", DeviceDescriptor.IdProduct);
    Print(L"  BcdDevice:       0x%04x\n", DeviceDescriptor.BcdDevice);
    Print(L"  StrManufacturer: 0x%02x\n", DeviceDescriptor.StrManufacturer);
    Print(L"  StrProduct:      0x%02x\n", DeviceDescriptor.StrProduct);
    Print(L"  StrSerialNumber: 0x%02x\n", DeviceDescriptor.StrSerialNumber);
    Print(L"  Configurations:  0x%02x\n\n", DeviceDescriptor.NumConfigurations);

  } // for

  //
  // Free buffer
  //
  if (HandleBuffer != NULL) {
      FreePool (HandleBuffer);
  }

  return EFI_SUCCESS;
}
