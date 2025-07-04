/** @file
  Course14 File System .h source code

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

#ifndef __FILE_SYS_H__
#define __FILE_SYS_H__

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/ShellParameters.h>

#include <Guid/FileSystemInfo.h>
#include <Guid/FileInfo.h>

#define FILE_NAME_SIZE 26
#define FILE_DATA_SIZE 1024

//
// Function prototypes
//
BOOLEAN
EFIAPI
IsFileName (
  IN CHAR16 *FileName,
  IN UINTN  NameSize
  );

EFI_STATUS
EFIAPI
CreateFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  );

EFI_STATUS
EFIAPI
CopyFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *SourceName,
  IN CHAR16            *DestinName,
  IN BOOLEAN           Cover
  );

EFI_STATUS
EFIAPI
PrintFileData (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *SourceName
  );

EFI_STATUS
EFIAPI
DeleteFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *FileName
  );

EFI_STATUS
EFIAPI
PrintFileInfo (
  IN EFI_FILE_SYSTEM_INFO *FileSysInfo,
  IN EFI_FILE_INFO *FileInfo
  );

EFI_STATUS
EFIAPI
ShowFileInfo (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *SourceName
  );

EFI_STATUS
EFIAPI
MergeFile (
  IN EFI_FILE_PROTOCOL *Root,
  IN CHAR16            *SourceName,
  IN CHAR16            *SourceName2,
  IN UINTN             NameSize
  );

#endif // __FILE_SYS_H__