/** @file
  Course12-Peim1 .h source code

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

#ifndef __PEIM1_H__
#define __PEIM1_H__

#include <Uefi.h>
#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>

#include <Ppi/ReadOnlyVariable2.h>

#define PEIM1_PPI_GUID \
{0x8a2af564, 0xfb3f, 0x43e7, {0x9d, 0x08, 0x56, 0xf5, 0x86, 0x47, 0xac, 0xf6}}

extern EFI_GUID  gPeim1PpiGuid;

typedef 
EFI_STATUS
(EFIAPI *GET_VARIABLE) (
  IN     CONST CHAR16   *VariableName,
  IN     CONST EFI_GUID *VendorGuid,
  OUT    UINT32         *Attributes,
  IN OUT UINTN          *DataSize,
  OUT    VOID           *Data
  );

typedef 
EFI_STATUS
(EFIAPI *CREATE_Hob) (
  IN  UINT16  Type,
  IN  UINT16  Length,
  OUT VOID    **Hob
  );

typedef struct _EFI_PEIM1_PROTOCOL {
  GET_VARIABLE GetVar;
  CREATE_Hob   CreateHob;
} EFI_PEIM1_PROTOCOL;

#endif // __PEIM1_H__