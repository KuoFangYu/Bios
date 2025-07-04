/** @file
  Course11- Peim2 .c source code

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

#include "Peim1.h"

EFI_STATUS
EFIAPI
MyGetVariable (
  IN     CONST CHAR16   *VariableName,
  IN     CONST EFI_GUID *VendorGuid,
  OUT    UINT32         *Attributes,
  IN OUT UINTN          *DataSize,
  OUT    VOID           *Data
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
  EFI_STATUS Status;

  Status = PeiServicesLocatePpi (
            &gEfiPeiReadOnlyVariable2PpiGuid,
            0,
            NULL,
            (VOID **) &VariablePpi
            );

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Locate 'gEfiPeiReadOnlyVariable2PpiGuid' Status : %r\n", Status));
    return Status;
  }

  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          VariableName, 
                          VendorGuid, 
                          Attributes, 
                          DataSize, 
                          Data
                          );
  return Status;
}


EFI_STATUS
EFIAPI
MyCreateHob (
  IN  UINT16 Type,
  IN  UINT16  Length,
  OUT VOID   **Hob
  )
{
  return PeiServicesCreateHob (
           Type, 
           Length,
           Hob
           );
}

EFI_PEIM1_PROTOCOL gMyPeim1ppi = {
  MyGetVariable,
  MyCreateHob
};

EFI_PEI_PPI_DESCRIPTOR mPeim1PpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeim1PpiGuid,
  &gMyPeim1ppi
};


EFI_STATUS
EFIAPI
Peim1EntryPoint (
  IN EFI_PEI_FILE_HANDLE FileHandle,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_STATUS Status;

  DEBUG((DEBUG_INFO, "\n--------------------------------------------------------------------------------\n"));
  DEBUG((DEBUG_INFO, "----------------------                Peim1                ----------------------\n"));
  DEBUG((DEBUG_INFO, "--------------------------------------------------------------------------------\n\n"));
  //
  // Install the PPI
  //
  Status = PeiServicesInstallPpi (&mPeim1PpiList);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Install 'mPeim1PpiList' Status : %r\n", Status));
    return Status;
  } else {
    DEBUG((DEBUG_INFO, "Install 'mPeim1PpiList' Status : %r\n", Status));
  }

  return EFI_SUCCESS;
}
