/** @file
  Course14 File System .c source code

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

#include "FileSys.h"


/**
  check filename is right or not

  @param  FileName   the file name whilch file need to check
  @param  NameSize   file name size including '\0'

  @retval TRUE       is a file name
  @retval other      not a file name  
**/
BOOLEAN
EFIAPI
IsFileName (
  IN CHAR16 *FileName,
  IN UINTN  NameSize
) 
{
  UINT8 Index;

  for (Index = 0; Index < NameSize; Index++) {
    if (FileName[Index] == '.' && Index + 4 < NameSize) {
      if (FileName[Index+1] == 't' && FileName[Index+2] == 'x' && 
          FileName[Index+3] == 't' && FileName[Index+4] == '\0') {
        return TRUE;
      } // if
    } // if
  } // for

  return FALSE;
}


/**
  create a file

  @param  Root       for use file service
  @param  FileName   the file name whilch file need to create

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
CreateFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  ) 
{
  EFI_STATUS Status;
  EFI_FILE_PROTOCOL *NewFile;

  //
  // Create the file
  //
  Status = Root->Open (
                   Root, 
                   &NewFile, 
                   FileName,
                   EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
                   0
                   );
  if (EFI_ERROR (Status)) {
    Print (L"Create file : %r\n", Status);
    return Status;
  } else {
    Print (L"Create file : %r\n", Status);
  }

  //
  // Close the file
  //
  NewFile->Close (NewFile);

  return EFI_SUCCESS;
}


/**
  copy data from Source fil to Destin file

  @param  Root         for use file service
  @param  SourceName   the file name whilch file copy by
  @param  DestinName   the file name whilch file copy to
  @param  Cover        cover orgin data or not

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
CopyFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *SourceName,
  IN CHAR16            *DestinName,
  IN BOOLEAN           Cover
  )
{
  EFI_STATUS Status;
  EFI_FILE_PROTOCOL *SourceFile;
  EFI_FILE_PROTOCOL *DestinFile;
  UINT8 *Buffer;
  UINTN BufferSize;

  //
  // Open the source file
  //
  Status = Root->Open (
                   Root, 
                   &SourceFile,
                   SourceName, 
                   EFI_FILE_MODE_READ, 
                   0
                   );
  if (EFI_ERROR (Status)) {
      Print (L"Open source file : %r\n", Status);
      return Status;
  }


  //
  // Create the destination file
  //
  Status = Root->Open (
                   Root,
                   &DestinFile,
                   DestinName, 
                   EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
                   0
                   );
  if (EFI_ERROR (Status)) {
      Print (L"Create destination file : %r\n", Status);
      SourceFile->Close (SourceFile);
      return Status;
  }

  //
  // Cover destination file
  //
  if (!Cover) {
    Status = DestinFile->SetPosition (
                          DestinFile,
                          0xFFFFFFFFFFFFFFFF
                          );
    if (EFI_ERROR (Status)) {
      Print(L"Set Position : %r\n", Status);
      return Status;
    }
  }

  //
  // Copy data from source to destination
  //
  Buffer = AllocatePool (FILE_DATA_SIZE);
  while (TRUE) {
    BufferSize = sizeof (Buffer);
    Status = SourceFile->Read (
                            SourceFile, 
                            &BufferSize, 
                            Buffer
                            );
    if (EFI_ERROR (Status) || BufferSize == 0) {
      break;
    }
    
    Status = DestinFile->Write (DestinFile, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      Print (L"Write to destination file : %r\n", Status);
      SourceFile->Close (SourceFile);
      DestinFile->Close (DestinFile);
      FreePool (Buffer);
      return Status;
    }
  }
  
  //
  // Close the files
  //
  SourceFile->Close (SourceFile);
  DestinFile->Close (DestinFile);
  FreePool (Buffer);
  
  Print (L"Success copy to file\n");
  return EFI_SUCCESS;
}


/**
  print the file data

  @param  Root       for use file service
  @param  FileName   the file name whilch file need to print

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
PrintFileData (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_STATUS Status;
  EFI_FILE_PROTOCOL *SourceFile;
  CHAR8 *Buffer;
  CHAR16 *UnicodeContext;
  UINTN BufferSize;

  //
  // Open the source file
  //
  Status = Root->Open (
                   Root, 
                   &SourceFile,
                   FileName, 
                   EFI_FILE_MODE_READ, 
                   0
                   );
  if (EFI_ERROR (Status)) {
    Print (L"Open source file : %r\n", Status);
    return Status;
  }

  //
  // Get data from source file and print
  //
  Buffer = AllocatePool (FILE_DATA_SIZE);
  BufferSize = FILE_DATA_SIZE;
  Status = SourceFile->Read (
                        SourceFile, 
                        &BufferSize, 
                        Buffer
                        );
  if (EFI_ERROR (Status)) {
    Print (L"Open source file : %r\n", Status);
    return Status;
  }

  //
  // trsnsfer ascii to unicode
  //
  BufferSize = (BufferSize + 1 ) * 2;
  UnicodeContext = AllocatePool (BufferSize);
  UnicodeSPrintAsciiFormat (UnicodeContext, BufferSize, Buffer);

  Print (L"Source file Data:\n");
  Print (L"%s\n", UnicodeContext);

  //
  // free memory
  //
  SourceFile->Close (SourceFile);
  FreePool (Buffer);
  FreePool (UnicodeContext);

  return EFI_SUCCESS;
}

/**
  delete the file data

  @param  Root       for use file service
  @param  FileName   the file name whilch file need to delete

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
DeleteFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_STATUS Status;
  EFI_FILE_PROTOCOL *DFile;

  //
  // Open the source file
  //
  Status = Root->Open (
                    Root, 
                    &DFile,
                    FileName, 
                    EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                    0
                    );
  if (EFI_ERROR(Status)) {
      Print(L"Open file : %r\n", Status);
      return Status;
  }

  //
  // Delete the source file
  //
  Status = DFile->Delete (DFile);
  if (EFI_ERROR (Status)) {
      Print (L"Delete file : %r\n", Status);
      return Status;
  } else {
    Print (L"Delete file : %r\n", Status);
  }

  return EFI_SUCCESS;
}


/**
  print the file information

  @param  FileSysInfo  file system information struct
  @param  FileInfo     file information struct

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
PrintFileInfo (
  IN EFI_FILE_SYSTEM_INFO *FileSysInfo,
  IN EFI_FILE_INFO *FileInfo
  )
{
  //
  // print file info
  //
  Print (L"\n");
  Print (L"EFI_FILE_INFO Size : %ld Bytes\n", FileInfo->Size);
  Print (L"File Size          : %ld Bytes\n", FileInfo->FileSize);
  Print (L"PhysicalSize Size  : %ld Bytes\n", FileInfo->PhysicalSize);
  Print (
    L"Create Time        : %d/%02d/%02d  %02d:%02d:%02d\n",
    FileInfo->CreateTime.Year,
    FileInfo->CreateTime.Month,
    FileInfo->CreateTime.Day,
    FileInfo->CreateTime.Hour,
    FileInfo->CreateTime.Minute,
    FileInfo->CreateTime.Second
    );

  Print (
    L"Last Access Time   : %d/%02d/%02d  %02d:%02d:%02d\n",
    FileInfo->CreateTime.Year,
    FileInfo->CreateTime.Month,
    FileInfo->CreateTime.Day,
    FileInfo->CreateTime.Hour,
    FileInfo->CreateTime.Minute,
    FileInfo->CreateTime.Second
    );

  Print (
    L"Modification Time  : %d/%02d/%02d  %02d:%02d:%02d\n",
    FileInfo->CreateTime.Year,
    FileInfo->CreateTime.Month,
    FileInfo->CreateTime.Day,
    FileInfo->CreateTime.Hour,
    FileInfo->CreateTime.Minute,
    FileInfo->CreateTime.Second
    );

  Print (L"Attribute          : ");
  if (FileInfo->Attribute == 0x0000000000000001 ) {
    Print (L"EFI_FILE_READ_ONLY\n");
  } else if (FileInfo->Attribute == 0x0000000000000002) {
    Print (L"EFI_FILE_HIDDEN\n");
  } else if (FileInfo->Attribute == 0x0000000000000004) {
    Print (L"EFI_FILE_SYSTEM\n");
  } else if (FileInfo->Attribute == 0x0000000000000008) {
    Print (L"EFI_FILE_RESERVED\n");
  } else if (FileInfo->Attribute == 0x0000000000000010) {
    Print (L"EFI_FILE_DIRECTORY\n");
  } else if (FileInfo->Attribute == 0x0000000000000020) {
    Print (L"EFI_FILE_ARCHIVE\n");
  } else if (FileInfo->Attribute == 0x0000000000000037) {
    Print (L"EFI_FILE_VALID_ATTR\n");
  } else {
    Print (L"Null\n");
  }

  Print (L"File Name          : %s\n\n", FileInfo->FileName);

  //
  // print file system info
  //
  Print (L"FILE_SYSTEM_INFO Size : %ld\n", FileSysInfo->Size);
  Print (L"ReadOnly              : ");
  if (FileSysInfo->ReadOnly) {
    Print (L"TRUE\n");
  } else {
    Print (L"FALSE\n");
  }

  Print (L"Volume Size           : %ld Bytes\n", FileSysInfo->VolumeSize);
  Print (L"Free Space            : %ld Bytes\n", FileSysInfo->FreeSpace);
  Print (L"Block Size            : %ld Bytes\n", FileSysInfo->BlockSize);
  Print (L"Volume Label          : %s\n", FileSysInfo->VolumeLabel);

  return EFI_SUCCESS;
}


/**
  get the file information

  @param  Root       for use file service
  @param  FileName   the file name whilch file need to print information

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
ShowFileInfo (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  )
{
  EFI_FILE_SYSTEM_INFO *FileSysInfo;
  EFI_FILE_INFO *FileInfo;
  EFI_FILE_PROTOCOL *SourceFile;
  UINTN  bufferSize;
  EFI_STATUS Status;

  //
  // Open the source file
  //
  Status = Root->Open (
                   Root, 
                   &SourceFile,
                   FileName, 
                   EFI_FILE_MODE_READ, 
                   0
                   );
  if (EFI_ERROR (Status)) {
    Print (L"Open source file : %r\n", Status);
    return Status;
  } else {
    Print (L"Open source file : %r\n", Status);
  }


  //
  // File Info
  //
  FileInfo = AllocatePool (1);
  Status = SourceFile->GetInfo (
                         SourceFile,
                         &gEfiFileInfoGuid,
                         &bufferSize,
                         FileInfo
                         );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    FileInfo = AllocatePool (bufferSize);
    Status = SourceFile->GetInfo (
                           SourceFile,
                           &gEfiFileInfoGuid,
                           &bufferSize,
                           FileInfo
                           );
    if (EFI_ERROR (Status)) {
      Print (L"Get file Info: %r\n", Status);
      return Status;
    } // if
  } // if                  


  //
  // File System Info
  //
  bufferSize = 1;
  FileSysInfo = AllocatePool (1);
  Status = SourceFile->GetInfo (
                         SourceFile,
                         &gEfiFileSystemInfoGuid,
                         &bufferSize,
                         FileSysInfo
                         );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    FileSysInfo = AllocatePool (bufferSize);
    Status = SourceFile->GetInfo (
                           SourceFile,
                           &gEfiFileSystemInfoGuid,
                           &bufferSize,
                           FileSysInfo
                           );
    if (EFI_ERROR (Status)) {
      Print (L"Get file Info: %r\n", Status);
      return Status;
    }
  } // if

  PrintFileInfo (FileSysInfo, FileInfo);
  SourceFile->Close (SourceFile);

  return EFI_SUCCESS;
}


/**
  merge the file

  @param  Root         for use file service
  @param  SourceName   file1 name whilch file need to merge
  @param  SourceName2  file2 name whilch file need to merge
  @param  NameSize     file1 name size

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
MergeFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *SourceName,
  IN CHAR16            *SourceName2,
  IN UINTN             NameSize
  )
{
  EFI_STATUS Status;
  CHAR16 *MergeFileName;
  UINTN NameSize3;
  UINTN Index;

  //
  // merge file name like "aa_bb.txt"
  //
  MergeFileName = AllocatePool (FILE_NAME_SIZE * 2);
  for (Index = 0; Index < NameSize; Index++) {
    if (SourceName[Index] == '.') {
      MergeFileName[Index] = '\0';
      break;
    } else {
      MergeFileName[Index] = SourceName[Index];
    }
  }

  StrCat (MergeFileName, L"_");
  StrCat (MergeFileName, SourceName2);
  NameSize3 = StrLen (MergeFileName) + 1;

  Status = CreateFile (
              Root,
              MergeFileName
              ); 
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // copy file1 data to new file
  //
  Status = CopyFile (
            Root, 
            SourceName,
            MergeFileName,
            TRUE
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // copy file2 data to new file
  //
  Status = CopyFile (
            Root, 
            SourceName2,
            MergeFileName,
            FALSE
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Print (L"Success merge to file\n");

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
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
  EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;
  EFI_FILE_PROTOCOL *Root;
  EFI_STATUS Status;
  CHAR16 *SourceName;
  CHAR16 *DestinName;
  CHAR16 **Argv;
  UINTN NameSize;
  UINTN NameSize2;
  UINTN Argc;

  //
  // Locate the Simple File System Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSimpleFileSystemProtocolGuid, 
                  NULL, 
                  (void**)&FileSystem
                  );
  if (EFI_ERROR (Status)) {
    Print (L"Locate Simple File System : %r\n", Status);
    return Status;
  }

  //
  // Open the volume
  //
  Status = FileSystem->OpenVolume (FileSystem, &Root);
  if (EFI_ERROR (Status)) {
    Print (L"Open volume : %r\n", Status);
    return Status;
  }

  //
  // Locate gEfi Shell Parameters Protocol
  //
  Status = gBS->OpenProtocol (
                gImageHandle,
                &gEfiShellParametersProtocolGuid,
                (VOID**)&ShellParameters,
                gImageHandle,
                NULL,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
                );
  if (EFI_ERROR (Status)) {
    Print (L"Locate Shell Parameters : %r\n", Status);
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;

  if (Argc < 3) {
    Print (L"Too few Parameters\n"); 
    return EFI_INVALID_PARAMETER;
  } else if (Argc > 4) {
    Print (L"Too many Parameters\n");
    return EFI_INVALID_PARAMETER;
  }

  //
  // allocate name memory
  //
  SourceName = AllocatePool (FILE_NAME_SIZE);
  DestinName = AllocatePool (FILE_NAME_SIZE);
  
  NameSize = StrLen (Argv[2]) + 1;
  StrCpy (SourceName, Argv[2]);

  //
  // check name format
  //
  if (!IsFileName (SourceName, NameSize)) {
    Print (L"Wrong FileName\n");
    return EFI_INVALID_PARAMETER;
  }

  if (Argc == 4) {
    NameSize2 = StrLen (Argv[3]) + 1;
    StrCpy (DestinName, Argv[3]);

    if (!IsFileName (DestinName, NameSize2)) {
      Print (L"Wrong FileName\n");
      return EFI_INVALID_PARAMETER;
    }    
  }


  if (Argc == 3) {
    //
    // 3 PARAMETER
    //
    if (StrCmp(Argv[1], L"-crt") == 0) {  // create
      Status = CreateFile (
                 Root, 
                 SourceName
                 );
      return Status;

    } else if (StrCmp (Argv[1], L"-d") == 0) {  // delete 
      Status = DeleteFile (
                 Root, 
                 SourceName
                 );
      return Status;
 
    } else if (StrCmp (Argv[1], L"-pd") == 0) {  // print data
      Status = PrintFileData (
                 Root, 
                 SourceName
                 );
      return Status;

    } else if (StrCmp (Argv[1], L"-i") == 0) {  // show info
      Status = ShowFileInfo (
                 Root, 
                 SourceName
                 );
      return Status;

    } else {
      Print (L"Undefind Parameters : %s\n", Argv[1]);
    }
  } else if (Argc == 4) {
    //
    // 4 PARAMETER
    //
    if (StrCmp (Argv[1], L"-cp") == 0) {  // copy
      Status = CopyFile (
                 Root, 
                 SourceName,
                 DestinName,
                 TRUE
                 );
      return Status;

    } else if (StrCmp (Argv[1], L"-m") == 0) {  // merge
      Status = MergeFile (
                 Root,
                 SourceName,
                 DestinName,
                 NameSize
                 );
      return Status;
    } else {
      Print (L"Undefind Parameters : %s\n", Argv[1]);
    }
  } // else if

  return EFI_SUCCESS;
}

