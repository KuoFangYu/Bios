/** @file
  Course12 Peim2 .h source code

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

#ifndef __PEIM2_H__
#define __PEIM2_H__

#include <Uefi.h>
#include <PiPei.h>

#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Ppi/ReadOnlyVariable2.h>


#define PEIM2_PPI_GUID \
{0xdbed1332, 0x1c7b, 0x4b77, {0xba, 0xf4, 0x89, 0x56, 0xca, 0x82, 0x4e, 0x59}}

#define VAR_GUID \
{0x80985828, 0x93b0, 0x4ad6, {0x99, 0xc1, 0xbc, 0xe5, 0x61, 0x1e, 0xe3, 0xff}}

#define Hob_GUID \
{0x49b3b45f, 0xad0a, 0x4a1c, {0xb0, 0x69, 0xc5, 0x62, 0x6d, 0xd3, 0x6b, 0x7d}}

extern EFI_GUID gPeim2PpiGuid;
extern EFI_GUID gVariableGuid = VAR_GUID;
extern EFI_GUID gMyHobGuid = Hob_GUID;

typedef 
EFI_STATUS
(EFIAPI *GET_SET_VARIABLE) ();

typedef struct _EFI_PEIM2_PROTOCOL {
  GET_SET_VARIABLE GetSetVar;
} EFI_PEIM2_PROTOCOL;

#endif // __PEIM2_H__