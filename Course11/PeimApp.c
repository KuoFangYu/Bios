/** @file
  Course11- Peim (Application) .c source code

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
#include <PiPei.h>
#include <Pi/PiHob.h>

#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>


#define VAR_GUID \
{0x80985828, 0x93b0, 0x4ad6, {0x99, 0xc1, 0xbc, 0xe5, 0x61, 0x1e, 0xe3, 0xff}}

#define MyHob_GUID \
{0x49b3b45f, 0xad0a, 0x4a1c, {0xb0, 0x69, 0xc5, 0x62, 0x6d, 0xd3, 0x6b, 0x7d}}

EFI_GUID mVariableGuid = VAR_GUID;
EFI_GUID mMyHobGuid = MyHob_GUID;

/**
  Stop the shell that can see the print data

  @param  EFI_SYSTEM_TABLE  *SystemTable

  @retval EFI_SUCCESS         success
  @retval other               problem
**/
VOID
StopShell (
  IN EFI_SYSTEM_TABLE  *SystemTable,
  IN UINT8             Choose
)
{
  UINTN          EventIndex;
  EFI_INPUT_KEY  Key;
  
  if (Choose == 0) {
    Print (L"Press any key to continue...\n");
  } else if (Choose == 1){
    Print (L"Press any key to see next batch of data \n");
  }

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
  Search print hob data

  @retval EFI_SUCCESS  success
  @retval other        problem
**/
EFI_STATUS
EFIAPI
DumpVar () {
  EFI_STATUS   Status;
  UINTN        VariableNameSize;
  UINTN        TempSize;
  UINT8        *VariableName;
  EFI_GUID     VendorGuid;
  CHAR16       *Temp;
  UINT16       i;
  BOOLEAN      Find;
  UINTN        DataSize;
  VOID         *Data;

  Temp = L"Hw_Hob";

  //
  // Allocate memory for the variable name
  //
  VariableNameSize = sizeof (CHAR16) * 1024; 
  VariableName = AllocateZeroPool (VariableNameSize);
  DataSize = sizeof (CHAR8) * 8;
  Data = AllocateZeroPool (DataSize);

  if (VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VariableName[0] = L'\0';
  Find = FALSE;

  //
  // Loop to enumerate all variables
  //
  while (TRUE) {
    TempSize = VariableNameSize;
    Status = gRT->GetNextVariableName (
                    &VariableNameSize, 
                    (CHAR16*)VariableName, 
                    &VendorGuid
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      VariableName = ReallocatePool (
                       TempSize, 
                       VariableNameSize, 
                       VariableName
                       );

      Status = gRT->GetNextVariableName (
                      &VariableNameSize, 
                      (CHAR16*)VariableName, 
                      &VendorGuid
                      );
    } // if
    
    if (EFI_ERROR (Status)) {
      break;
    }

    // 
    // compare GUID or Name and print
    //
    if (StrCmp (Temp, (CHAR16*)VariableName) == 0) {
      TempSize = DataSize;
      Status = gRT->GetVariable (
                      (CHAR16*)VariableName, 
                      &VendorGuid, 
                      NULL, 
                      &DataSize,
                      Data
                      );

      if (Status == EFI_BUFFER_TOO_SMALL) {
        Data = ReallocatePool (
                 TempSize, 
                 DataSize, 
                 Data
                 );

        //
        // Get the variable data
        //
        Status = gRT->GetVariable (
                        (CHAR16 *)VariableName, 
                        &VendorGuid, 
                        NULL, 
                        &DataSize, 
                        Data
                        );
      } // if

      if (EFI_ERROR (Status)) {
        FreePool (Data);
        break;
      } // if
      
      Print (
        L"Vendor GUID: %g   Variable Name: %s   DataSize : %d\n", 
        VendorGuid, 
        VariableName,
        DataSize
        );

      Print (L"Data:\n");
      for (i = 0; i < DataSize; i++) {
        Print(L"%08x\n\n", ((UINT8 *)Data)[i]);
      }

      Find = TRUE;
    } // if

    VariableNameSize = sizeof(CHAR16) * 1024;
    DataSize = sizeof(CHAR8) * 8;

  } // while
  
  if (!Find) {
    Print (L"Do not found anything\n");
  }
  
  Print (L"\n");
  FreePool (VariableName);
  FreePool (Data);

  return EFI_SUCCESS;
}

/**
  Create Variable

  @param  Attributes    Attributes types  
  @param  SystemTable   

  @retval EFI_SUCCESS  success
  @retval other        problem
**/
EFI_STATUS
EFIAPI
CreateVariable (
  IN UINT32            Attributes,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS  Status;
  CHAR16      VariableName[8];
  UINTN       DataSize;
  CHAR8       Data[8];
  UINT8       i;

  StrCpy (VariableName, L"Hw_Hob");
  DataSize = 8;
  
  for (i = 0; i < 8; i++ ) {
    Data[i] = i + 48;
  }

  SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
  //
  // Create or modify the variable
  //
  Status = gRT->SetVariable (
    VariableName,
    &mVariableGuid,
    Attributes,
    DataSize,
    Data
  );

  if (EFI_ERROR(Status))
    Print (L"ERROR : Fail to create Variable\n\n\n");
  else
    Print (L"Success to craete Variable\n\n\n");


  return Status;
}


/**
  Delete Variable  

  @retval EFI_SUCCESS  success
  @retval other        problem
**/
EFI_STATUS
EFIAPI
DeleteVariable () {
  CHAR16      VariableName[8];
  EFI_STATUS  Status;

  StrCpy (VariableName, L"Hw_Hob");
  //
  // Delete the variable by setting its data size to zero
  //
  Status = gRT->SetVariable (
                  VariableName,
                  &mVariableGuid,
                  0,
                  0,
                  NULL
                  );

  if (EFI_ERROR (Status))
    Print (L"ERROR : Fail to Delete Variable\n\n\n");
  else
    Print (L"Success to Delete Variable\n\n\n");

  return Status;
}

EFI_STATUS
EFIAPI
DumpHob () {
  EFI_HOB_GUID_TYPE *GuidHob;
  UINTN             HobDataSize;
  VOID              *HobList;
  UINT8             *HobData;
  UINTN             Index;

  HobList = GetHobList();
  //
  // Get the HOB data
  //
  GuidHob = GetNextGuidHob (
              &mMyHobGuid,
              HobList
              );
  if (GuidHob == NULL) {
    Print(L"Failed to find the HOB\n\n\n");
    return EFI_NOT_FOUND;
  }

  //
  // Get the data from the HOB
  //
  HobData = GET_GUID_HOB_DATA(GuidHob);
  HobDataSize = GET_GUID_HOB_DATA_SIZE(GuidHob);

  Print (L"Data size: %d bytes\n", HobDataSize);
  for (Index = 0; Index < HobDataSize; Index++) {
    Print (L"%08x\n\n", HobData[Index]);
  }

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
  UINTN          EventIndex;
  EFI_INPUT_KEY  Key;
  UINT32         Attributes;

  while (TRUE) {
    SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
    Print (L"[F1]: Create Variables\n");
    Print (L"[F2]: Dump Variable\n");
    Print (L"[F3]: Delete Variable\n");
    Print (L"[F4]: Dump HobData\n");
    Print (L"[ESC]: Exit.\n\n");
    //
    // wait key
    //
    Status = gBS->WaitForEvent (
                    1, 
                    &gST->ConIn->WaitForKey, 
                    &EventIndex
                    );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // read key
    //
    Status = gST->ConIn->ReadKeyStroke (
                            gST->ConIn, 
                            &Key
                            );
    if (EFI_ERROR (Status)) {
        return Status;
    }

    //
    // function1- Create Variable
    //
    if (Key.ScanCode == SCAN_F1) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                   EFI_VARIABLE_NON_VOLATILE;
      CreateVariable (Attributes, SystemTable);
      StopShell (SystemTable, 0);
    } // if

    //
    // function2 - Search handle By GUID
    //
    if (Key.ScanCode == SCAN_F2) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      DumpVar ();
      StopShell (SystemTable, 0);
    } // if

    //
    // function3 - Create Variable
    //
    if (Key.ScanCode == SCAN_F3) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      DeleteVariable ();
      StopShell (SystemTable, 0);       
    } // if

    //
    // function4 - Delete Variable
    //
    if (Key.ScanCode == SCAN_F4) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      DumpHob ();
      StopShell (SystemTable, 0);
    }

    //
    // end the program
    //
    if (Key.ScanCode == SCAN_ESC) {
        SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
        break;
    }
  }  // while
  
  return EFI_SUCCESS;

} // main