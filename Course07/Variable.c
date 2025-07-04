/** @file
  Course07-Variable Services .c source code

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
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HandleParsingLib.h>
#include <Library/ShellLib.h>

#define VARIABLE_SIZE 50


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
  get the GUID form Guid string

  @param  GuidString     string of GUID
  @param  Guid           store type of EFI_GUID

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS 
StringToGuid(
  CHAR16   *GuidString, 
  EFI_GUID *Guid
) 
{
    return StrToGuid (GuidString, Guid);
}


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
  } else if (Choose ==1){
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
  read GUID or Protocol Name

  @param  DataSize   writeable data size
  @param  Buffer     store the write data

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
  BOOLEAN       ValidInput;

  Print (L"[A~Z  a~z  0~9  '-'  '_'  ' '] : Input letter\n");
  Print (L"[Del] : Delete previous input\n");
  Print (L"[Right Arrow] : Done\n\n");
  Print (L"Input Data :\n");

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
    // read key and limit characters
    //
    ValidInput = FALSE;
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if ((Key.UnicodeChar >= L'0' && Key.UnicodeChar <= L'9') ||
        (Key.UnicodeChar >= L'A' && Key.UnicodeChar <= L'Z') ||
        (Key.UnicodeChar >= L'a' && Key.UnicodeChar <= L'z') ||
        Key.UnicodeChar == L'-' || Key.UnicodeChar == L'_' || Key.UnicodeChar == L' ') {
        ValidInput = TRUE;
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
    if (ValidInput && Index < DataSize) {
      Buffer[Index] = Key.UnicodeChar;
      Index++;
      Print (L"%c", Key.UnicodeChar);
    }
  }

  return EFI_SUCCESS;
}

/**
  print all UEFI variables

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
PrintAllVariables (
  IN EFI_SYSTEM_TABLE  *SystemTable
) 
{
  EFI_STATUS  Status;
  UINTN       VariableNameSize;
  UINTN       TempSize;
  UINT8       *VariableName;
  UINT8       Count;
  EFI_GUID    VendorGuid;
  
  //
  // Allocate memory for the variable name
  //
  VariableNameSize = sizeof(CHAR16) * 1024;
  VariableName = AllocateZeroPool (VariableNameSize);

  if (VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VariableName[0] = L'\0';
  Count = 0;

  //
  // Loop to enumerate all variables
  //
  while (TRUE) {
    Count++;
    if (Count == 36) {
      Count = 0;
      StopShell (SystemTable, 1);
    }

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

    if (EFI_ERROR(Status)) {
      break;
    }

    Print (
      L"Vendor GUID: %g   VariableName: %s\n", 
      VendorGuid, 
      VariableName
      );

    VariableNameSize = sizeof(CHAR16) * 1024;

  } // while

  FreePool(VariableName);

  return EFI_SUCCESS;
}


/**
  Search Variable By Name Or Guid

  @param  Choose       search by Name or GUID    

  @retval EFI_SUCCESS  success
  @retval other        problem
**/
EFI_STATUS
EFIAPI
SearchByNameOrGuid (
  IN UINT8             Choose
)
{
  EFI_GUID     SearchGuid;
  EFI_STATUS   Status;
  UINTN        VariableNameSize;
  UINTN        TempSize;
  UINT8        *VariableName;
  EFI_GUID     VendorGuid;
  UINT16       InputStr[VARIABLE_SIZE];
  CHAR16       *Temp;
  UINT16       i;
  BOOLEAN      Find;
  UINTN        DataSize;
  VOID         *Data;

  //
  // initialize the InputStr
  //
  for (i = 0; i < VARIABLE_SIZE; i++)
    InputStr[i] = '\0';

  if (Choose == 0)
    Print (L"Please Input a Name\n\n");
  else if (Choose == 1)
    Print (L"Please Input a Guid like 'XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX\n\n");

  Status = ReadData (
             InputStr, 
             VARIABLE_SIZE
             );

  Temp = InputStr;

  //
  // convert str to GUID
  //
  if (Choose == 1) {
    Status = StringToGuid (
              Temp, 
              &SearchGuid
              );
    
    if (Status != EFI_SUCCESS) {
      Print (L"It's not a vaild Guid\n");
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Allocate memory for the variable name
  //
  VariableNameSize = sizeof (CHAR16) * 1024;
  DataSize = sizeof (CHAR8) * 8; 
  VariableName = AllocateZeroPool (VariableNameSize);
  Data = AllocateZeroPool (DataSize);

  if (VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VariableName[0] = L'\0';
  Find = FALSE;
  Print (L"\n");

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
    
    if (EFI_ERROR(Status)) {
      break;
    }

    // 
    // compare GUID or Name and print
    //

    if ((Choose == 0 && StrCmp (Temp, (CHAR16*)VariableName) == 0) ||
        (Choose == 1 && CompareGuid (&VendorGuid, &SearchGuid))) {
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
      
      Print(L"Data: ");
      for (i = 0; i < DataSize; i++) {
        if (i%30 == 0) {
          Print (L"\n");
        }
        Print(L"%02x ", ((UINT8 *)Data)[i]);
      }

      Print (L"\n\n");
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
  CHAR16      Read[VARIABLE_SIZE];
  EFI_STATUS  Status;
  CHAR16      VariableName[VARIABLE_SIZE];
  EFI_GUID    VendorGuid;
  UINTN       DataSize;
  UINT8       *Data;
  UINT8       i;
  

  for (i = 0; i < VARIABLE_SIZE; i++) {
    Read[i] = L'\0';
    VariableName[i] = L'\0';
  }

  Print (L"Please Input a Variable Name\n\n");
  ReadData (VariableName, VARIABLE_SIZE);

  Print (L"\nPlease Input a Guid like 'XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX'\n\n");
  ReadData (Read, VARIABLE_SIZE);

  Status = StringToGuid (
          Read, 
          &VendorGuid
          );

  if (Status != EFI_SUCCESS) {
    Print (L"It's not a vaild Guid\n");
    return EFI_INVALID_PARAMETER;
  }
  
  SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
  Print (L"Please Input data Size\n\n");
  GetValue (&DataSize, 32, 1);

  for (i = 0; i < VARIABLE_SIZE; i++)
    Read[i] = L'\0';

  Print (L"\nPlease Input Writein data\n\n");
  ReadData (Read, DataSize);
  Data = (UINT8*)Read;

  //
  // Create or modify the variable
  //
  Status = gRT->SetVariable (
    VariableName,
    &VendorGuid,
    Attributes,
    DataSize,
    Data
  );

  if (EFI_ERROR(Status))
    Print (L"\nERROR : Fail to create Variable\n");
  else
    Print (L"\nSuccess to craete Variable\n");

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
  CHAR16      Read[VARIABLE_SIZE];
  EFI_STATUS  Status;
  EFI_GUID    VendorGuid;
  UINT8       i;

  for (i = 0; i < VARIABLE_SIZE; i++)
    Read[i] = L'\0';
  
  //
  // get Name and Guid
  //
  Print (L"\nPlease Input a Guid like 'XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX'\n\n");
  ReadData (Read, VARIABLE_SIZE);

  Status = StringToGuid (
          Read, 
          &VendorGuid
          );

  if (Status != EFI_SUCCESS) {
    Print (L"It's not a vaild Guid\n");
    return EFI_INVALID_PARAMETER;
  }

  for (i = 0; i < VARIABLE_SIZE; i++) {
    Read[i] = L'\0';
  }

  Print (L"\nPlease Input a Variable Name\n\n");
  ReadData (Read, VARIABLE_SIZE);

  //
  // Delete the variable by setting its data size to zero
  //
  Status = gRT->SetVariable (
                  Read,
                  &VendorGuid,
                  0,
                  0,
                  NULL
                  );

  if (EFI_ERROR(Status))
    Print (L"\nERROR : Fail to Delete Variable\n");
  else
    Print (L"\nSuccess to Delete Variable\n");

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

  EFI_STATUS     Status;
  UINTN          EventIndex;
  EFI_INPUT_KEY  Key;
  UINT32         Attributes;

  while (TRUE) {
    SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
    Print(L"[F1]: All Variables List\n");
    Print(L"[F2]: Search Variable by Name or GUID\n");
    Print(L"[F3]: Create Variable\n");
    Print(L"[F4]: Delete Variable\n");
    Print(L"[ESC]: Exit.\n\n");
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
    // function1- print all Variable and Guid
    //
    if (Key.ScanCode == SCAN_F1) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      PrintAllVariables (SystemTable);
      StopShell (SystemTable, 0);
    } // if

    //
    // function2 - Search handle By GUID
    //
    if (Key.ScanCode == SCAN_F2) {
      while (TRUE) {
        SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
        Print(L"[F1]: Search by Name\n");
        Print(L"[F2]: Search by GUID\n");
        Print(L"[ESC]: Exit.\n\n");

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
        // function1- search by Name
        //
        if (Key.ScanCode == SCAN_F1) {
          SearchByNameOrGuid(0);
          StopShell (SystemTable, 0);
        } // if

        //
        // function2 - Search By GUID
        //
        if (Key.ScanCode == SCAN_F2) {
          SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
          SearchByNameOrGuid(1);
          StopShell (SystemTable, 0);
        } // if

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
    // function3 - Create Variable
    //
    if (Key.ScanCode == SCAN_F3) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      Print(L"Select Attributes :\n");
      Print(L"[F1]: BootService\n");
      Print(L"[F2]: BootService + Non-Volatile\n");
      Print(L"[F3]: BootService + RunTime\n");
      Print(L"[F4]: BootService + RunTime + Non-Volatile\n");
      Print(L"[Other]: Exit\n\n");
  
      //
      // wait key
      //
      Status = gBS->WaitForEvent (
                      1, 
                      &gST->ConIn->WaitForKey, 
                      &EventIndex
                      );

      if (EFI_ERROR(Status))
          return Status;
      
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
      // choose Attributes
      //
      Attributes = 0;
      if (Key.ScanCode == SCAN_F1) {
        Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
      } else if (Key.ScanCode == SCAN_F2) {
        Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;
      } else if (Key.ScanCode == SCAN_F3) {
        Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
      } else if (Key.ScanCode == SCAN_F4) {
        Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                     EFI_VARIABLE_NON_VOLATILE;
      }

      if (Key.ScanCode >= SCAN_F1 && Key.ScanCode <= SCAN_F4) {
        SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
        CreateVariable (Attributes, SystemTable);
      } // if           

      StopShell (SystemTable, 0);
      Key.ScanCode = 0;
    } // if

    //
    // function4 - Delete Variable
    //
    if (Key.ScanCode == SCAN_F4) {
        DeleteVariable ();
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