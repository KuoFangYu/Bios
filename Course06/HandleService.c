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
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/HandleParsingLib.h>
#include <Library/ShellLib.h>

#define PROTOCOL_NAME_SIZE 60

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

  Print (L"[A~Z  a~z  0~9  '-'  '_'] : Input\n");
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
        Key.UnicodeChar == L'-' || Key.UnicodeChar == L'_') {
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

  Print the handle and protocol data

  @param  HandleCount     Number of handle
  @param  HandleBuffer    store the handle address

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
EFIAPI
PrintHandle (
  UINTN      HandleCount,
  EFI_HANDLE *HandleBuffer
) 
{

  UINTN       i;
  UINTN       j;
  UINTN       ProtocolCount;
  EFI_STATUS  Status;
  EFI_GUID    **ProtocolGuidArray;
  CHAR16      *Name;
  UINT16      *ProtocolName;

  ProtocolName = NULL;

  for (i = 0; i < HandleCount; i++) {
    Print (L"Handle : %d (%x) :\n", i, HandleBuffer[i] );
    //
    // get each handle's protocol guid
    //
    Status = gBS->ProtocolsPerHandle (
                    HandleBuffer[i],
                    &ProtocolGuidArray,
                    &ProtocolCount
                    );
    //
    // print each Protocol Name and GUID
    //
    for (j = 0; j < ProtocolCount; j++) {
      //
      // get each Protocol Name
      //
      Name = GetStringNameFromGuid (
               ProtocolGuidArray[j], 
               NULL
               );

      if (Name == NULL) 
        Name = L"UnknowDevice";

      Print (L"   %s\n", Name); 
      Print (L"   %g\n\n", ProtocolGuidArray[j]);
    } // for      
  } // for
  

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

  give the protocol guid and find which Handles are support 

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
EFIAPI
SearchByGuid () {

  UINTN       i;
  UINTN       HandleCount;
  EFI_STATUS  Status;
  EFI_HANDLE  *HandleBuffer;
  EFI_GUID    ProtocolGuid;
  UINT16      GuidStr[40];
  CHAR16      *Temp;

  //
  // initialize the GuidStr
  //
  for (i = 0; i < 40; i++)
    GuidStr[i] = '\0'; 
  
  Print (L"[Func2] : Please Input a Guid like 'XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX'\n\n");

  Status = ReadData (
             GuidStr, 
             40
             );

  Temp = GuidStr;
  
  //
  // convert the str to GUID
  //
  Status = StringToGuid (
             Temp, 
             &ProtocolGuid
             );
  
  if (Status != EFI_SUCCESS) {
    Print (L"It's not a vaild Guid\n");
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // use Protocol Guid to find whilch handle support
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
                  
  if (HandleCount == 0) {
    Print (L"Handle Not Found\n");
    return EFI_NOT_FOUND;
  }
  
  //
  // print data
  //
  Print (L"\n");
  PrintHandle (HandleCount, HandleBuffer);

  return EFI_SUCCESS;
}


/**

  give the protocol Name and find which Handles are support 

  @retval EFI_SUCCESS    success
  @retval other          problem

**/
EFI_STATUS
EFIAPI
SearchByName () {
  UINTN       HandleCount;
  UINTN       i;
  EFI_STATUS  Status;
  EFI_HANDLE  *HandleBuffer;
  EFI_GUID    *ProtocolGuid;
  UINT16      NameStr[PROTOCOL_NAME_SIZE];

  //
  // initialize the NameStr
  //
  for (i = 0; i < PROTOCOL_NAME_SIZE; i++)
    NameStr[i] = '\0';

  Print (L"[Func3] : Please Input a Name\n\n");

  Status = ReadData (
             NameStr, 
             PROTOCOL_NAME_SIZE
             );

  //
  // convert name to guid
  //
  Status = GetGuidFromStringName(
             NameStr,
             NULL,
             &ProtocolGuid
             );

  if (ProtocolGuid == NULL ) {
    Print (L"Null of Protocol Guid, Please try again\n");
    return Status;
  }

  //
  // use Protocol Guid to find whilch handle support
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  //
  // print data
  //
  Print (L"\n");  
  PrintHandle (HandleCount, HandleBuffer);

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

  EFI_INPUT_KEY  Key;
  EFI_STATUS     Status;
  UINTN          EventIndex;
  UINTN          HandleCount;
  EFI_HANDLE     *HandleBuffer;


  while (TRUE) {
    SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
    Print(L"[F1]: All Handle List\n");
    Print(L"[F2]: Search Handle by Protocol GUID\n");
    Print(L"[F3]: Search Handle by Protocol Name\n");
    Print(L"[ESC]: Exit.\n\n");
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

    // function1- print all handle and protocol
    if (Key.ScanCode == SCAN_F1) {
      //
      // get all handle address
      //
      Status = gBS->LocateHandleBuffer (
                      AllHandles,
                      NULL,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                      );

      if (EFI_ERROR (Status)) {
        Print (L"LocateHandleBuffer fail\n");
        return Status;
      }

      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);
      PrintHandle (HandleCount, HandleBuffer);

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
      } // if
    } // if

    //
    // function2 - Search handle By GUID
    //
    if (Key.ScanCode == SCAN_F2) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);

      SearchByGuid ();

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
    } // if

    //
    // function3 - Search handle By Name
    //
    if (Key.ScanCode == SCAN_F3) {
      SystemTable->ConOut->ClearScreen (SystemTable->ConOut);

      SearchByName ();

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
      } // if
    } // if

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