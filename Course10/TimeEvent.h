/** @file
  Course11 Timer(Protocol) .h source code

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

#ifndef __TIME_EVENT_H__
#define __TIME_EVENT_H__

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/SimpleTextOut.h>

extern EFI_GUID  gTimeEventProtocolGuid;

typedef
struct
_TIME_EVENT_PROTOCOL {
  EFI_STATUS (EFIAPI *DisplayTime) (
    IN EFI_EVENT  Event,
    IN VOID       *Context
    );
} TIME_EVENT_PROTOCOL;

#endif // __TIME_EVENT_H__