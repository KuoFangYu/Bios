/** @file
  Course12- SmmApp .c source code

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

#include <Protocol/SmmControl2.h>

#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#define NTC_HW_SW_SMI 0x74


/**
  The user Entry Point for SMM driver. The user code starts with this function
  as the real entry point for the SMM driver. Mainly to trigger the Software SMM.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS 
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_SMM_CONTROL2_PROTOCOL *SmmControl;
  UINTN ActivationInterval;
  EFI_STATUS Status;
  BOOLEAN Periodic;
  UINT8 Command;
  UINT8 Data;

  //
  // Locate the SmmControl2 Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmControl2ProtocolGuid, 
                  NULL, 
                  (VOID **)&SmmControl
                  );
  if (EFI_ERROR (Status)) {
    Print (L"Loacte Protocol : %r", Status);
    return Status;
  }

  //
  // set trigger data
  //
  Command = NTC_HW_SW_SMI;
  Data = 0;
  Periodic = FALSE;
  ActivationInterval = 0;
  
  //
  // trigger the software smm
  //
  Status = SmmControl->Trigger (
                         SmmControl,
                         &Command,
                         &Data,
                         Periodic,
                         ActivationInterval
                         );
  if (EFI_ERROR (Status)) {
    Print (L"Trigger SW SMI: %r\n", Status);
    return Status;
  }

  return EFI_SUCCESS;
}
