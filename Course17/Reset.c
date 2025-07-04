/** @file
 * Course2-05-Reset .c source code
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
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/IoLib.h>

#define KBC_CMD_RESET 0xFE
#define KBC_COMMAND_PORT 0x64

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8 data;

  data = IoRead8 (KBC_COMMAND_PORT);
  while ((data & 0x02) != 0) {
    data = IoRead8 (KBC_COMMAND_PORT);
  }

  IoWrite8 (KBC_COMMAND_PORT, KBC_CMD_RESET);

  return EFI_SUCCESS;
}
