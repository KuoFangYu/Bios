/** @file
  Course2-8 IRQ .h source code

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

#ifndef __IRQ_TABLE_H__
#define __IRQ_TABLE_H__

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>

#define MAX_ADDR 0xFFFFF
#define START_ADDR 0xF0000

//
// IRQ Header - 32Bytes
//
typedef union {
  struct {
    UINT8 Signature[4];
    UINT8 Version[2];
    UINT8 TableSize[2];
    UINT8 PciIrqBus;
    UINT8 PciIrqDevFunc;
    UINT8 PciExcluIrq[2];
    UINT8 CompPciRouter[4];
    UINT8 Miniport[4];
    UINT8 Reverse[11];
    UINT8 CheckSum;
  } Item;

  UINT8 Byte32[32];
} IRQ_HEADER;

//
// IRQ one Slot - 16Bytes
//
typedef union {
  struct {
    UINT8 Bus;
    UINT8 Device;
    UINT8 VIntA;     // Link Value for INTA# 
    UINT8 BIntA[2];  // IRQ Bitmap for INTA#
    UINT8 VIntB;     // Link Value for INTB#
    UINT8 BIntB[2];  // IRQ Bitmap for INTB#
    UINT8 VIntC;     // Link Value for INTC#
    UINT8 BIntC[2];  // IRQ Bitmap for INTC#
    UINT8 VIntD;     // Link Value for INTD#
    UINT8 BIntD[2];  // IRQ Bitmap for INTD#
    UINT8 Slot;
    UINT8 Reverse;
  } Item;

  UINT8 Byte16[16];
} IRQ_SLOTS;


IRQ_HEADER Header;
IRQ_SLOTS  *Slots;
UINT16     mMaxSlots;

//
// Function prototypes
//
VOID
EFIAPI
StopShell (
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
ScanData (
  VOID
  );

EFI_STATUS
EFIAPI
DisplayHeader (
  VOID
  );

EFI_STATUS
EFIAPI
DisplaySlot (
  IN UINT16 SlotNum
  );

#endif // __IRQ_TABLE_H__