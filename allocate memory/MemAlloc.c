/** @file
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
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#define MAX_POOL_NUMBER 2047
#define MAX_PAGE_NUMBER 5
#define PAGE_SIZE       4096

/**

  get a number between MinNum and MaxNum

  @param  Number    store the input
  @param  MaxNum    input max num
  @param  MinNum    input min num

  @retval EFI_SUCCESS    success
  @retval other          problem
  
**/
EFI_STATUS
EFIAPI
GetValue (
  OUT UINTN *Number,
  IN  UINTN MaxNum,
  IN  UINTN MinNum
  )
{
    EFI_STATUS    Status;
    EFI_INPUT_KEY Key;
    UINTN         EventIndex;
    UINTN         CurrentNumber;

    if (Number == NULL) {
        return EFI_INVALID_PARAMETER;
    }
    //
    // print content
    //
    CurrentNumber = 0;
    Print (L"[Up Arrow] : Add (Max: %d)\n", MaxNum);
    Print (L"[Down Arrow] : Decrease (Min: 0)\n");
    Print (L"[Right Arrow] : Save Change\n");
    Print (L"\rCurrent numbers : %d  ", CurrentNumber);

    while (TRUE) {
        //
        // wait key
        //
        Status = gBS->WaitForEvent (
                        1, 
                        &gST->ConIn->WaitForKey,
                        &EventIndex
                        );
                      
        if (EFI_ERROR(Status)) {
            return Status;
        }
        //
        //read key
        //
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
        if (EFI_ERROR(Status)) {
            return Status;
        }

        //
        //UP key(add num)
        // 
        if (Key.ScanCode == SCAN_UP) {
            if (CurrentNumber < MaxNum)
                CurrentNumber++;
            
            Print (L"\rCurrent numbers : %d  ", CurrentNumber); // 更新顯示
            
        }

        //
        //Down key(minus num)
        //
        if (Key.ScanCode == SCAN_DOWN) {
            if (CurrentNumber > MinNum) {
                CurrentNumber--;
                Print (L"\rCurrent numbers : %d  ", CurrentNumber); // 更新顯示
            }
        }

        //
        //Right key(get num)
        //
        if (Key.ScanCode == SCAN_RIGHT) {
            Print(L"\n");
            break;
        }
    }

    *Number = CurrentNumber;
    return EFI_SUCCESS;
}

/**
  
  read F1 F2 F3 to decide allocate Pool or Page or EXIT 

  @param SystemTable  for shell
  @param PageBuffer   page start address
  @param PoolBuffer   pool start address
  @param NumOfPage    how many page need to allocate
  @param NumOfPool    how many pool need to allocate

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
EFIAPI
AllocateInterface (
  IN  EFI_SYSTEM_TABLE      *SystemTable,
  OUT EFI_PHYSICAL_ADDRESS  *PageBuffer,
  OUT VOID                  **PoolBuffer,
  OUT UINTN                 *NumOfPage,
  OUT UINTN                 *NumOfPool
  )
{
  EFI_STATUS    Status;
  EFI_INPUT_KEY Key;
  UINTN         EventIndex;
    
  while (TRUE) {
    SystemTable->ConOut->ClearScreen (SystemTable->ConOut);

    Print (L"Allocate Page or Pool\n");
    Print (L"[F1]: Set Number of Pages\n");
    Print (L"[F2]: Set Number of pool size\n");
    Print (L"[F3]: Allocate memory\n");
    Print (L"[ESC]: Exit.\n");
    Print (L"Desire to allocate %d Pages %d Pools now\n", *NumOfPage, *NumOfPool);
    
    //
    // wait key
    //
    Status = gBS->WaitForEvent (
                    1, 
                    &gST->ConIn->WaitForKey,
                    &EventIndex
                    );

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
    // F1 get the allocate page num
    //
    if (Key.ScanCode == SCAN_F1) {
      //
      // claer the shell
      //
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      Print(L"How many Pages of memory desire to allocate?\n");

      Status = GetValue (
                 &*NumOfPage, 
                 MAX_PAGE_NUMBER, 
                 0
                 );

      if (EFI_ERROR(Status)) {
          Print (L"Error setting pages: %r\n", Status);
      }
    }

    //
    // F2 get the allocate pool num
    //
    if (Key.ScanCode == SCAN_F2) {
      //
      // claer the shell
      //
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      Print (L"How many Pools of memory desire to allocate?\n");

      Status = GetValue (
                 &*NumOfPool, 
                 MAX_POOL_NUMBER, 
                 0
                 );

      if (EFI_ERROR(Status)) {
          Print(L"Error setting pool size: %r\n", Status);
      }
    }


    //
    // F3 allocate page and pool
    //
    if (Key.ScanCode == SCAN_F3) {
      Status = gBS->AllocatePages (
                      AllocateAnyPages,     // type
                      EfiBootServicesData,  
                      *NumOfPage,           // size 
                      &*PageBuffer
                      );

      if (EFI_ERROR (Status))
        Print (L"ERROR : Failed to allocate the memory.\n");
      else {
        Print (L"Prepare to allocate %d Page.\n", *NumOfPage);
        Print (L"Page has been allocate and initialized with all zeors successfully\n");
      }


      Status = gBS->AllocatePool (
                      EfiBootServicesData, 
                      *NumOfPool, 
                      &*PoolBuffer
                      );

      if (EFI_ERROR (Status))
        Print (L"ERROR : Failed to allocate the memory.\n");
      else {
        Print (L"Prepare to allocate %d Pool.\n", *NumOfPool);
        Print (L"Pool has been allocate and initialized with all zeors successfully\n");
      }
      
      Print (L"Press any key to continue.........\n");
      //
      // stop the shell that can read print
      //      
      SystemTable->BootServices->WaitForEvent(1, 
                                   &(SystemTable->ConIn->WaitForKey), 
                                   &EventIndex
                                   );
                                   
      if (SystemTable->ConIn->ReadKeyStroke (
                                SystemTable->ConIn, 
                                &Key) == EFI_SUCCESS) {
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
      }

      break;
    }

    //
    // ESC exit
    //
    if (Key.ScanCode == SCAN_ESC) {
      SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
      return EFI_NOT_READY;
    }
  }

  return EFI_SUCCESS;

}


/**
  
  read Page Offset, Page start and the size which write in page

  @param NumPages      the allocate page num
  @param Offset        page offset
  @param PageStart     page start num
  @param SetDataSize   the size which write in page   

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
GetPageOffset (
  IN  UINTN  NumPages,
  OUT UINTN  *Offset,
  OUT UINTN  *PageStart,
  OUT UINTN  *SetDataSize
  )
{
  UINTN DataSizeMax;
  //
  // get page start
  //
  Print (L"Write data start from page ???.\n\n");
  GetValue (&*PageStart, NumPages-1, 0);
  //
  // get page offset
  //
  Print (L"\nWrite data start from offset ??? of Page.\n\n");
  GetValue (&*Offset, 2047, 0);
  //
  // get page write data size
  //
  DataSizeMax = 2048 - *Offset;
  Print (L"\nData Length ???.\n\n");
  GetValue (&*SetDataSize, DataSizeMax, 1);

  return EFI_SUCCESS;
}


/**
  
  read pool Offset and the size which write in page

  @param NumPool       the allocate pool num
  @param Offset        pool offset
  @param SetDataSize   the size which write in pool   

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
GetPoolOffset (
  IN  UINTN  NumPool,
  OUT UINTN  *Offset,
  OUT UINTN  *SetDataSize
  )
{
  UINTN DataSizeMax;

  Print (L"Write data start from offset ???.\n\n");
  GetValue (&*Offset, NumPool-1, 0);

  DataSizeMax = NumPool - *Offset;
  Print (L"\nData Length ???.\n\n");
  GetValue (&*SetDataSize, DataSizeMax, 1);

  return EFI_SUCCESS;
}


/**

  read the Write data

  @param  Buffer     store the write data
  @param  DataSize   write data size

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
EFIAPI
ReadData (
  OUT UINT16 *Buffer,
  IN  UINTN  DataSize
  )
{
  EFI_STATUS    Status;
  EFI_INPUT_KEY Key;
  UINTN         Index;
  UINTN         EventIndex;

  //
  // check buffer and datasize correct
  //
  if (Buffer == NULL || DataSize == 0) {
      return EFI_INVALID_PARAMETER;
  }

  Print (L"[A~Z  a~z  0~9] : Input\n");
  Print (L"[Del] : Delete previous input\n");
  Print (L"[Right Arrow] : Done\n");
  Print (L"Input Data : ");

  Index = 0;
  
  while (TRUE) {
      //
      // wait for key
      //
      Status = gBS->WaitForEvent (
                      1, 
                      &gST->ConIn->WaitForKey, 
                      &EventIndex
                      );

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
      // input complete 
      //
      if (Key.ScanCode == SCAN_RIGHT) {
          Print (L"\n");
          break;
      }

      //
      // delete letter
      //
      if (Key.ScanCode == SCAN_DELETE) {
          if (Index > 0) {
              Index--;
              Buffer[Index] = L'\0';
              Print (L"\b \b");
          }
          continue;
      }

      //
      // store letter to Buffer
      //
      if (Key.UnicodeChar != 0) {
        if (Index < DataSize) {
            Buffer[Index++] = Key.UnicodeChar;
            Print (L"%c", Key.UnicodeChar);
        }
      }
  }

  return EFI_SUCCESS;
}

/**

  read the Write data

  @param  NumPage       allocate page num
  @param  PageOffset    page offset
  @param  PageStart     the write data start from
  @param  PageDataSize  write data size
  @param  Buffer        data
  @param  PageAddr      allocate page start address

  @retval EFI_SUCCESS   success
  @retval other         problem

**/
EFI_STATUS
EFIAPI
SetPageData (
  IN     UINTN                 NumPage,
  IN OUT UINTN                 PageOffset,
  IN OUT UINTN                 PageStart,
  IN OUT UINTN                 PageDataSize,
  IN     UINT16                *Buffer,
  IN     EFI_PHYSICAL_ADDRESS  PageAddr
  )
{
  
  UINT16 *StartAddr;
  UINTN  i;

  Print (
    L"Dump memmory from Page %d Offset(%d) to Offset(%d) + Length(%d)-1\n", 
    PageStart, 
    PageOffset, 
    PageOffset, 
    PageDataSize
    );


  StartAddr = (UINT16 *)(PageAddr + PAGE_SIZE * PageStart + PageOffset);

  SetMem (
    StartAddr, 
    PageDataSize * sizeof(CHAR16), 
    0
    );
  
  for (i = 0; i < PageDataSize; i++) {
    StartAddr[i] = Buffer[i];
  }

  Print (L"\nHEX\n");

  for (i = 0; i < PageDataSize; i++) {
    Print (L"%04x ", StartAddr[i]);
  }

  Print (L"\n\nUnicode:\n");
  Print (L"%s\n", Buffer);

  return EFI_SUCCESS;

}

/**

  read the Write data

  @param  NumPool       allocate page num
  @param  PoolOffset    pool offset
  @param  PoolDataSize  write data size
  @param  Buffer        data
  @param  PageAddr      allocate pool start address

  @retval EFI_SUCCESS   success
  @retval other         problem

**/
EFI_STATUS
EFIAPI
SetPoolData (
  IN     UINTN  NumPool,
  IN OUT UINTN  PoolOffset,
  IN OUT UINTN  PoolDataSize,
  IN     UINT16 *Buffer,
  IN     VOID   *PoolAddr
  )
{

  UINT16 *StartAddr;
  UINTN  i;

  Print (
    L"Dump memmory from Offset(%d) to Offset(%d) + Length(%d)-1\n", 
    PoolOffset, 
    PoolOffset, 
    PoolDataSize
    );

  StartAddr = (UINT16 *)((UINT8*)PoolAddr + PoolOffset);
  //
  // check if over pool size
  //
  if (PoolOffset + PoolDataSize * sizeof(UINT8) > NumPool) {
      Print (L"Error: Offset + DataSize exceeds pool size.\n");
      return EFI_INVALID_PARAMETER;
  }
  //
  // write data in pool
  //  
  SetMem (
    StartAddr, 
    PoolDataSize * sizeof(UINT8), 
    0
    );
  
  for (i = 0; i < PoolDataSize; i++) {
    StartAddr[i] = Buffer[i];
  }

  Print (L"\nHEX\n");

  for (i = 0; i < PoolDataSize; i++)
    Print (L"%04x ", StartAddr[i]);
  

  Print (L"\n\nUnicode:\n");
  Print (L"%s\n\n", Buffer);

  return EFI_SUCCESS;

}


const CHAR16 *MemTypes[] = { 
    L"Reserved", 
    L"LoaderCode", 
    L"LoaderData", 
    L"BS_Code", 
    L"BS_Data", 
    L"RT_Code", 
    L"RT_Data", 
    L"Available", 
    L"EfiUnusableMemory",
    L"ACPI_Recl", 
    L"ACPI_NVS", 
    L"MMIO", 
    L"MMIO_Port", 
    L"PalCode",
    L"Persistent",
    L"EfiUnacceptedMemoryType",
    L"EfiMaxMemoryType"
};


/**

  transfer the MemTypes to string

  @param  Type      is enum for MemType

  @retval MemTypes  string MemType

**/
EFI_BOOT_SERVICES *gBS;  
const CHAR16 * 
MemTypesToString (
  UINT32 Type
  ) 
{

  if (Type > sizeof (MemTypes)/sizeof (CHAR16 *)) 
      return L"Unknown"; 

  return MemTypes[Type]; 
} 


/**

  use GetMemoryMap to get value and allocate enough pool

  @param  MapBuffer     Map allocate start address
  @param  MapSize       Map allocate size
  @param  MapKey        location in which firmware returns the key for the current memory map
  @param  DescSize      firmware returns the size, an individual EFI_MEMORY_DESCRIPTOR
  @param  DescVer       firmware returns the version number associated with the EFI_MEMORY_DESCRIPTOR.

  @retval EFI_SUCCESS   success
  @retval other         problem

**/
EFI_STATUS 
MemoryMap (
  EFI_MEMORY_DESCRIPTOR **MapBuffer, 
  UINTN                 *MapSize, 
  UINTN                 *MapKey, 
  UINTN                 *DescSize, 
  UINT32                *DescVer
  ) 
{ 
  EFI_STATUS Status;

  *MapSize = sizeof(**MapBuffer) * 31;

//
// allocate pool until enough
//
GetMap:
  *MapSize += sizeof(**MapBuffer);
  Status = EFI_SUCCESS;
  //
  // allocate pool
  //
  Status = gBS->AllocatePool (
                  EfiLoaderData, 
                  *MapSize, 
                  (void **)MapBuffer
                  );

  if (Status != EFI_SUCCESS) { 
      Print (L"ERROR: Failed to allocate pool for memory map"); 
      return Status;
  } 

  Status = gBS->GetMemoryMap (
                  MapSize, 
                  *MapBuffer, 
                  MapKey, 
                  DescSize, 
                  DescVer
                  );
  //
  // if not enough then free pool and allocate again
  //
  if (Status != EFI_SUCCESS) {
    if (Status == EFI_BUFFER_TOO_SMALL) { 
      gBS->FreePool ((void *)*MapBuffer);
      goto GetMap;
    }
    Print (L"Error: Failed to get memory map");
  }

  return Status;
} 

/**

  Print the get value

  @param  Buffer        Map allocate start address
  @param  Desc
  @param  DescSize      firmware returns the size, an individual EFI_MEMORY_DESCRIPTOR
  @param  DescVer       firmware returns the version number associated with the EFI_MEMORY_DESCRIPTOR
  @param  MapSize       Map allocate size
  @param  MapKey        location in which firmware returns the key for the current memory map
  @param  MappingSize   each memtype size (bytes) 
  @param  TotalPage     all memtype page usage amount
  @param  UsePage       each memtype page usage amount

  @retval EFI_SUCCESS   success
  @retval other         problem

**/
EFI_STATUS  
PrintMemMap(void) 
{ 
    EFI_MEMORY_DESCRIPTOR *Buffer;
    EFI_MEMORY_DESCRIPTOR *Desc;
    UINTN                 DescSize; 
    UINT32                DescVer; 
    UINTN                 MapSize;
    UINTN                 MapKey;
    UINTN                 MappingSize;
    UINTN                 TotalPage;
    UINTN                 UsePage[17];

    EFI_STATUS Status; 
    int        i; 

    Status = EFI_SUCCESS;
    Status = MemoryMap (
               &Buffer, 
               &MapSize, 
               &MapKey, 
               &DescSize, 
               &DescVer
               );

    if (Status != EFI_SUCCESS) 
        return Status; 
    
    i = 0;
    TotalPage = 0;
    Desc = Buffer; 

    Print (L"Type            start            End                Pages              Attributes\n");
    //
    // initialization UsePage
    //
    for(i = 0; i < 17; i++ )
      UsePage[i] = 0;

    //
    // Print all type memory data
    //
    while ((UINT8*)Desc < (UINT8*)Buffer + MapSize) { 
        MappingSize =(UINTN)Desc->NumberOfPages * PAGE_SIZE;
        TotalPage = TotalPage + (UINTN)Desc->NumberOfPages;
        UsePage[Desc->Type] = UsePage[Desc->Type] + (UINTN)Desc->NumberOfPages;

        Print (
          L"%-15s %016llx-%016llx   %016x",
          MemTypesToString (Desc->Type), 
          Desc->PhysicalStart, 
          Desc->PhysicalStart + MappingSize-1, 
          Desc->NumberOfPages
          );

        if (Desc->Type == 5 || Desc->Type == 6 || Desc->Type == 11)
          Print (L"   8%015x\n", Desc->Attribute);
        else
          Print (L"   %016x\n", Desc->Attribute);
         

        Desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)Desc + DescSize);  
    }
    Print (L"\n");

    //
    // Print statistical data for each type memory
    //
    for (i = 0; i < 17; i++ ) {
      if (i != 7 && i != 15 && i != 8 && i != 16) {
        Print (L"  %-12s  %u (%d Bytes)\n", 
          MemTypesToString (i), 
          UsePage[i], 
          UsePage[i] * PAGE_SIZE
          );
      }
    }

    //
    // available print at last
    //
    Print (L"  %-12s  %u (%d Bytes)\n", 
      MemTypesToString (7),
      UsePage[7], 
      UsePage[7] * PAGE_SIZE
      );

    Print (L"              ---------------------------\n");
    Print (L"Total Memory:   %d MB (%d Bytes)\n", 
      TotalPage / 256, 
      TotalPage * PAGE_SIZE
      );

    gBS->FreePool (Buffer); 

    return Status; 
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
  EFI_PHYSICAL_ADDRESS  PageAddr;
  EFI_PHYSICAL_ADDRESS  CurAddr;
  EFI_INPUT_KEY         Key;
  VOID                  *PoolAddr;
  EFI_STATUS            Status;
  UINTN                 NumPool;
  UINTN                 NumPage;
  UINTN                 PoolOffset;
  UINTN                 PageOffset;
  UINTN                 PageStart;
  UINTN                 PageDataSize;
  UINTN                 PoolDataSize;
  UINTN                 EventIndex;
  UINT16                Array[2048];
  
  
  NumPool = 0;
  NumPage = 0;
  PoolAddr = 0;

  //
  // get value and allocate
  //
  Status = AllocateInterface (
             SystemTable,
             &PageAddr, 
             &PoolAddr, 
             &NumPage, 
             &NumPool
             );
  //
  // none of memory allocate then exit
  //
  if (Status != EFI_NOT_READY) {
    if (NumPage == 0 && NumPool == 0) {
      Print (L"Do Nothing\n");
    } else {

      while (TRUE) {
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
        Print(L"More Operation\n");
        Print(L"[F1]: Write data to the allocate memory\n");
        Print(L"[F2]: Free the allocate memory\n");
        Print(L"[F3]: Dump Memory Map\n");
        Print(L"[ESC]: Exit.\n\n");
        Print(L"There are %d Pages and %d Pools has been allocated\n", NumPage, NumPool);
        //
        // wait key
        //
        Status = gBS->WaitForEvent (
                        1, 
                        &gST->ConIn->WaitForKey, 
                        &EventIndex
                        );

        if (EFI_ERROR(Status)) {
            return Status;
        }

        //
        // read key
        //
        Status = gST->ConIn->ReadKeyStroke (
                               gST->ConIn, 
                               &Key
                               );

        if (EFI_ERROR(Status)) {
            return Status;
        }

        // function1 - Write to Page or Pool
        if (Key.ScanCode == SCAN_F1) {
          SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
          Print (L"More Operation\n");
          Print (L"[F1]: Write to Pages\n");
          Print (L"[F2]: Write to Pools\n");
          Print (L"[ESC]: Exit.\n\n");
          Print (L"There are %d Pages and %d Pools has been allocated\n", 
            NumPage, 
            NumPool
            );

          while (TRUE) {
            //
            // wait key
            //
            Status = gBS->WaitForEvent (
                            1, 
                            &gST->ConIn->WaitForKey, 
                            &EventIndex
                            );

            if (EFI_ERROR(Status)) {
                return Status;
            }

            //
            // read key
            //
            Status = gST->ConIn->ReadKeyStroke (
                                   gST->ConIn, 
                                   &Key
                                   );

            if (EFI_ERROR(Status)) {
                return Status;
            }
            //
            // F1 Write to Page
            //
            if (Key.ScanCode == SCAN_F1) {
              if (NumPage > 0) {
                SystemTable->ConOut->ClearScreen (SystemTable->ConOut);

                PageOffset = 0;
                PageStart = 0;
                PageDataSize = 0;

                GetPageOffset (
                  NumPage,
                  &PageOffset,
                  &PageStart,
                  &PageDataSize
                  );

                SystemTable->ConOut->ClearScreen (SystemTable->ConOut);

                Print (L"Write data start from Page %d Offset(%d) with data length(%d)\n", 
                  PageStart, 
                  PageOffset, 
                  PageDataSize
                  );

                ReadData (Array, PageDataSize);

                SetPageData (
                  NumPage,
                  PageOffset,
                  PageStart,
                  PageDataSize,
                  Array,
                  PageAddr
                  );

                //
                // stop the shell that can see print data
                //
                Print (L"Press any key to continue.........\n");
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
                }
                  break;
              } else {
                Print (L"None of Allocate memory\n");
              }
            } // F1 write page 
            //
            // Write to Pool
            //
            if (Key.ScanCode == SCAN_F2) {
              if (NumPool > 0) {
                PoolOffset = 0;
                PoolDataSize = 0;

                SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
                GetPoolOffset(
                  NumPool,
                  &PoolOffset,
                  &PoolDataSize
                  );

                SetMem(Array, 2048, 0);
                SystemTable->ConOut->ClearScreen (SystemTable->ConOut);

                Print (L"Write data start from Offset(%d) with data length(%d)\n", 
                  PoolOffset, 
                  PoolDataSize
                  );

                ReadData (Array, PoolDataSize);

                SetPoolData (
                  NumPool,
                  PoolOffset,
                  PoolDataSize,
                  Array,
                  PoolAddr
                  );
                //
                // stop the shell that can see print data
                //
                Print (L"Press any key to continue.........\n");
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
                }

                break;
              } else {
              Print (L"None of Allocate memory\n");
              }
            } // F2 write pool 
            //
            // Exit Write to Pool or page
            //
            if (Key.ScanCode == SCAN_ESC) {
                Print(L"Exiting...\n");
                Key.ScanCode = 0;
                break;
            }
          }
        }
        //
        // function2 - Free page or pool
        //
        if (Key.ScanCode == SCAN_F2) {
          
          while (TRUE) {
            SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
            Print (L"Select Free Memory Type\n");
            Print (L"[F1]: Pages\n");
            Print (L"[F2]: Pools\n");
            Print (L"[ESC]: Exit.\n\n");
            Print (L"There are %d Pages and %d Pools has been allocated\n", NumPage, NumPool);
            //
            // wait key
            //
            Status = gBS->WaitForEvent (
                            1, 
                            &gST->ConIn->WaitForKey, 
                            &EventIndex
                            );

            if (EFI_ERROR(Status)) {
                return Status;
            }
            //
            //
            //
            Status = gST->ConIn->ReadKeyStroke (
                                  gST->ConIn, 
                                  &Key
                                  );

            if (EFI_ERROR(Status)) {
                return Status;
            }
            //
            // free page
            //
            if (Key.ScanCode == SCAN_F1) {
              if ( NumPage != 0 ) {
                Print (L"\nFree Page from ???? to the End.\n\n");
                //
                // get the value what is the start page
                //
                GetValue (
                  &PageStart, 
                  NumPage-1, 
                  0
                  );

                CurAddr = PageAddr + EFI_PAGE_SIZE * PageStart;
                PageStart = NumPage - PageStart;
                NumPage = NumPage - PageStart;

                Status = gBS->FreePages (
                                CurAddr,
                                PageStart
                                );

                if (EFI_ERROR (Status)) {
                  Print (L"ERROR%d : Failed to free the Page memory.\n", Status);
                } else {
                  Print (L"Success to free the Page memory.\n");
                }
              } else {
                Print (L"\nNothing to Free\n");
              }

              Print (L"Press any key to continue.........\n");
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
              }
            } // F1 free page

            //
            // free pool
            //
            if (Key.ScanCode == SCAN_F2) {
              if ( NumPool != 0 ) {
                Status = gBS->FreePool (PoolAddr);
                if (EFI_ERROR (Status)) {
                  Print (L"ERROR%d : Failed to free the Pool memory.\n", Status);
                } else {
                  Print (L"Success to free the Pool memory.\n");
                }

                NumPool = 0;
              } else {
                Print (L"\nNothing to Free\n");
              }

              Print (L"Press any key to continue.........\n");
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
              }

            } // if

            if (Key.ScanCode == SCAN_ESC) {
              Key.ScanCode = 0;
              break;
            }
          } // while
        } // if F2 free

        //
        // function3 - Dump memory
        //
        if (Key.ScanCode == SCAN_F3) {
          PrintMemMap ();

          Print (L"Press any key to continue.........\n");
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
          }
          
          Key.ScanCode = 0;
          
          if (EFI_ERROR(Status)) {
            Print (L"Failed to print memory usage: %r\n", Status);
          }
        }

        //
        // end the program
        //
        if (Key.ScanCode == SCAN_ESC) {
            SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
            break;
        }
      }  // while
    } // else
  } // if
  
  return EFI_SUCCESS;

} // main