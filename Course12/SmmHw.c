/** @file
  Course12- SmmHw .c source code

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

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>

#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


EFI_STATUS 
EFIAPI 
SmmHwDriverEntry (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  );

EFI_STATUS 
EFIAPI
PowerButtonCallBack (
  IN EFI_HANDLE DispatchHandle, 
  IN CONST VOID *Context, 
  IN OUT VOID *CommBuffer, 
  IN OUT UINTN *CommBufferSize
  );


/**
  The user Entry Point for SMM driver. The user code starts with this function
  as the real entry point for the SMM driver.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS 
EFIAPI 
SmmHwDriverEntry (
  IN EFI_HANDLE ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  BOOLEAN                                 InSmm;
  EFI_STATUS                              Status;
  EFI_HANDLE                              DispatchHandle;
  EFI_SMM_SYSTEM_TABLE2                   *gSmst;
  EFI_SMM_BASE2_PROTOCOL                  *SmmBase2;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   PowerButtonContext;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *PowerButtonDispatch;
  
  //
  // Locate SmmBase2 Protocol
  //
  Status = SystemTable->BootServices->LocateProtocol (
                                        &gEfiSmmBase2ProtocolGuid, 
                                        NULL, 
                                        (VOID **)&SmmBase2
                                        );
  if (EFI_ERROR(Status)) {
      return Status;
  }
  
  //
  // check is in SMM now
  //
  Status = SmmBase2->InSmm (SmmBase2, &InSmm);
  if (EFI_ERROR(Status)) {
    Print (L"Get InSmm : %r\n", Status);
    return Status;
  }

  if (!InSmm) {
    Print (L"Not in Smm\n");
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the SMM System Table
  //
  SmmBase2->GetSmstLocation (SmmBase2, &gSmst);

  //
  // locate SmmPowerButton protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid, 
                    NULL, 
                    (VOID **)&PowerButtonDispatch
                    );
  if (EFI_ERROR(Status)) {
    Print (L"Locate Protocol : %r", Status);
    return Status;
  }

  //
  // set trigger value
  //
  PowerButtonContext.Phase = EfiPowerButtonEntry;

  //
  // Register HW Call Back Function
  //
  Status = PowerButtonDispatch->Register (
                                  PowerButtonDispatch, 
                                  PowerButtonCallBack, 
                                  &PowerButtonContext, 
                                  &DispatchHandle
                                  );
  if (EFI_ERROR(Status)) {
    Print (L"Set Register : %r", Status);
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Hardware SMM Call Back function, Mainly to read the RTC reconds and display it on 80port.

  @param[in]     HandlerType     Points to the handler type or NULL for root MMI handlers.
  @param[in]     Context         Points to an optional context buffer.
  @param[in,out] CommBuffer      Points to the optional communication buffer.
  @param[in,out] CommBufferSize  Points to the size of the optional communication buffer.  

  @retval EFI_SUCCESS   success
  @retval other         problem
**/
EFI_STATUS
EFIAPI
PowerButtonCallBack (
  IN EFI_HANDLE DispatchHandle, 
  IN CONST VOID *Context, 
  IN OUT VOID *CommBuffer, 
  IN OUT UINTN *CommBufferSize
  ) 
{
  EFI_TIME Time;
  EFI_STATUS Status;
  
  //
  // get RTC seconds
  //
  Status = gRT->GetTime (&Time, NULL);
  if (EFI_ERROR (Status)) {
    Print (L"Get Time : %r", Status);
    return Status;
  }

  //
  // write to port80
  //
  IoWrite8 (0x80, Time.Second);

  return EFI_SUCCESS;
}
