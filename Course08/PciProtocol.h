/** @file
  Course08-non-driver module driver (Protocol) .h source code

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

#ifndef __PCI_PROTOCOL_H__
#define __PCI_PROTOCOL_H__
#include <Uefi.h>

extern EFI_GUID gPciScanProtocolGuid;

typedef struct _PCI_SCAN_PROTOCOL {
  EFI_STATUS (EFIAPI *ScanPciDevices)();
} PCI_SCAN_PROTOCOL;

#endif // __PCI_PROTOCOL_H__
