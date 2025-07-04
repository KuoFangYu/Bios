/** @file
  Course2-8 IRQ .c source code

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

#include <IRQTable.h>

/**
  Stop the shell that can see the print data

  @param  EFI_SYSTEM_TABLE  *SystemTable

  @retval EFI_SUCCESS         success
  @retval other               problem
**/
VOID
EFIAPI
StopShell (
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINTN          EventIndex;
  EFI_INPUT_KEY  Key;

  Print (L"Press any key to continue...\n");

  SystemTable->BootServices->WaitForEvent (
                               1, 
                               &(SystemTable->ConIn->WaitForKey), 
                               &EventIndex
                               );

  if (SystemTable->ConIn->ReadKeyStroke (
                            SystemTable->ConIn, 
                            &Key
                            ) == EFI_SUCCESS) {
    SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
  } // if
}


/**
  read all of IRQ data

  @retval EFI_SUCCESS         success
  @retval EFI_NOT_FOUND       can not find $PIR 
**/
EFI_STATUS
EFIAPI
ScanData (
  VOID
  )
{

  UINT64 Addr;
  UINT8  *MemoryAddr;
  UINT8  Value;
  UINT8  Sign[4];
  UINT16 Offset;
  UINT16 SlotSize;
  UINT8  SlotOffset;
  UINT16  SlotNum;
  BOOLEAN Find;

  //
  // find the address by $PIR
  //
  Find = FALSE;
  for (Addr = START_ADDR; Addr <= MAX_ADDR; Addr++) {
    MemoryAddr = (UINT8*)Addr;
    Value = *MemoryAddr;

    if (Value == '$') {
      Sign[0] = '$';
      for (Offset = 1; Offset < 4; Offset++) {
        MemoryAddr = (UINT8*)(Addr + Offset);
        Sign[Offset] = *MemoryAddr;
      }

      if (Sign[1] == 'P' && Sign[2] == 'I' && Sign[3] == 'R') {
        Print (L"%x\n", Addr);
        Find = TRUE;
        break;
      }
    } // if
  } // for

  //
  // Not Find return
  //
  if (!Find) {
    Print (L"Can not find '$PIR'\n");
    return EFI_NOT_FOUND;
  }

  //
  // write data to Header
  //
  for (Offset = 0; Offset < 32; Offset++) {
    MemoryAddr = (UINT8*)(Addr + Offset);
    Header.Byte32[Offset] = *MemoryAddr;
  }

  //
  // write data to all Slot
  //
  Addr = Addr + 32;
  SlotSize = (Header.Item.TableSize[0] + Header.Item.TableSize[1] * 256) - 32;
  SlotNum = 0;
  SlotOffset = 0;
  Slots = AllocatePool (SlotSize);
  
  for (Offset = 0; Offset < SlotSize; Offset++) {
    SlotOffset = Offset % 16;
    SlotNum = Offset / 16;
    MemoryAddr = (UINT8*)(Addr + Offset);
    Slots[SlotNum].Byte16[SlotOffset] = *MemoryAddr;
  }
  mMaxSlots = SlotNum;
  
  return EFI_SUCCESS;
}

/**
  print IRQ Header data

  @retval EFI_SUCCESS    success
**/
EFI_STATUS
EFIAPI
DisplayHeader (
  VOID
  )
{
  UINT16 TotalSize;
  UINT8  Dev;
  UINT8  Func;

  Print (L" IRQ Routing Table Header :\n");

  Print (
    L"   Signature                                 : %c%c%c%c\n", 
    Header.Item.Signature[0],
    Header.Item.Signature[1],
    Header.Item.Signature[2],
    Header.Item.Signature[3]
    );
  Print (L"   Minor Version                             : 0x%02x\n", Header.Item.Version[0]);
  Print (L"   Major Version                             : 0x%02x\n", Header.Item.Version[1]);

  TotalSize = Header.Item.TableSize[0] + Header.Item.TableSize[1] * 256;
  Print (L"   Table Size                                : %d Bytes\n", TotalSize);
  Print (L"   PCI Interrupt Router's Bus                : 0x%02x\n", Header.Item.PciIrqBus);

  Dev = (Header.Item.PciIrqDevFunc) >> 3;
  Func = (Header.Item.PciIrqDevFunc) << 5;
  Func = Func >> 5;
  Print (L"   PCI Interrupt Router's DEV                : 0x%02x\n", Dev);
  Print (L"   PCI Interrupt Router's FUNC               : 0x%02x\n", Func);

  Print (
    L"   PCI Exclusive IRQs                        : 0x%02x%02x\n", 
    Header.Item.PciExcluIrq[1],
    Header.Item.PciExcluIrq[0]
    );
  Print (
    L"   Compatible PCI Interrupt Router Vendor ID : 0x%02x%02x\n",
     Header.Item.CompPciRouter[1],
     Header.Item.CompPciRouter[0]
     );

  Print (
    L"   Compatible PCI Interrupt Router Device ID : 0x%02x%02x\n",
     Header.Item.CompPciRouter[3],
     Header.Item.CompPciRouter[2]
     );

  Print (
    L"   Miniport Data                             : 0x%02x%02x%02x%02x\n", 
    Header.Item.Miniport[3],
    Header.Item.Miniport[2],
    Header.Item.Miniport[1],
    Header.Item.Miniport[0]
    );

  Print (L"   Reserved                                  : 00 00 00 00 00 00 00 00 00 00 00\n");
  Print (L"   CheckSum                                  : 0x%02x\n", Header.Item.CheckSum);

  return EFI_SUCCESS;
}

/**
  print one IRQ slot data

  @retval EFI_SUCCESS    success
**/
EFI_STATUS
EFIAPI
DisplaySlot (
  IN UINT16 SlotNum
  )
{
  Print (L" %02dth Slot IRQ Routing :\n", SlotNum);
  Print (L"   PCI Bus Number       : 0x%02x\n", Slots[SlotNum].Item.Bus);
  Print (L"   PCI Device Number    : 0x%02x\n", (Slots[SlotNum].Item.Device) >> 3);
  Print (L"   Link Value for INTA# : 0x%02x\n", Slots[SlotNum].Item.VIntA);
  Print (
    L"   IRQ Bitmap for INTA# : 0x%02x%02x\n", 
    Slots[SlotNum].Item.BIntA[1],
    Slots[SlotNum].Item.BIntA[0]
    );
  Print (L"   Link Value for INTB# : 0x%02x\n", Slots[SlotNum].Item.VIntB);
  Print (
    L"   IRQ Bitmap for INTB# : 0x%02x%02x\n", 
    Slots[SlotNum].Item.BIntB[1],
    Slots[SlotNum].Item.BIntB[0]
    );
  Print (L"   Link Value for INTC# : 0x%02x\n", Slots[SlotNum].Item.VIntC);
  Print (
    L"   IRQ Bitmap for INTC# : 0x%02x%02x\n", 
    Slots[SlotNum].Item.BIntC[1],
    Slots[SlotNum].Item.BIntC[0]
    );
  Print (L"   Link Value for INTD# : 0x%02x\n", Slots[SlotNum].Item.VIntD);
  Print (
    L"   IRQ Bitmap for INTD# : 0x%02x%02x\n", 
    Slots[SlotNum].Item.BIntD[1],
    Slots[SlotNum].Item.BIntD[0]
    );
  Print (L"   Slot Number          : 0x%02x\n", Slots[SlotNum].Item.Slot);
  Print (L"   Reserved             : 0x00\n");

  return EFI_SUCCESS;
}


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

  EFI_STATUS     Status;
  EFI_INPUT_KEY  Key;
  UINTN          EventIndex;
  UINT16         SlotNum;

  //
  // get data
  //
  Status = ScanData ();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  SlotNum = 0;
  while (TRUE) {
    SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
    Print(L"[F1] : Display IRQ Routing Header\n");
    Print(L"[F2] : Display IRQ Routing Slot\n");
    Print(L"[ESC]: Exit\n\n");
    //
    // wait key
    //
    Status = gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &EventIndex);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // read key
    //
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // function1- print IRQ Routing Header
    //
    if (Key.ScanCode == SCAN_F1) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      DisplayHeader();
      Print (L"\n");
      StopShell (SystemTable);
    } // if

    //
    // function2 - print IRQ Routing Slot
    //
    if (Key.ScanCode == SCAN_F2) {
      while (TRUE) {
        //
        // move the cursor to upper left corner
        //
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
        Print(L"[Right Arrow] : Next Plot\n");
        Print(L"[Left Arrow]  : Last Plot\n");
        Print(L"[ESC]         : Exit\n\n");

        Status = DisplaySlot (SlotNum);
        if (EFI_ERROR(Status)) {
          return Status;
        }

        //
        // wait key
        //
        Status = gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &EventIndex);
        if (EFI_ERROR(Status)) {
          return Status;
        }

        //
        // read key
        //
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        if (EFI_ERROR(Status)) {
          return Status;
        }

        //
        // SlotNum + 1
        //
        if (Key.ScanCode == SCAN_RIGHT) {
          if (SlotNum < mMaxSlots) {
            SlotNum++;
          } else {
            SlotNum = 0;
          }
        }

        //
        // SlotNum - 1
        //
        if (Key.ScanCode == SCAN_LEFT) {
          if (SlotNum > 0) {
            SlotNum--;
          } else {
            SlotNum = mMaxSlots;
          }
        }

        //
        // function3 - EXIT
        //
        if (Key.ScanCode == SCAN_ESC) {
          SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
          Key.ScanCode = 0;
          break;
        } // if
      } // while
    } // if

    //
    // end the program
    //
    if (Key.ScanCode == SCAN_ESC) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      break;
    }
  } // while

  return EFI_SUCCESS;
}