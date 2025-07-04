/** @file
  Course11- Peim1 .c source code

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
#include "Peim2.h"

EFI_STATUS
EFIAPI
Peim2GetSetVar () {
  EFI_PEIM1_PROTOCOL *Peim1Ppi;
  EFI_HOB_GUID_TYPE  *GuidHob;
  EFI_STATUS         Status;
  CHAR16             *VariableName;
  UINT32             *Attributes;
  UINTN              DataSize;
  VOID               *HobPointer;
  CHAR8              Data[8];

  VariableName = L"Hw_Hob";

  Status = PeiServicesLocatePpi (
             &gPeim1PpiGuid,
             0,
             NULL,
             (VOID **) &Peim1Ppi
             );

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Locate gPeim1PpiGuid status: %r\n", Status));
    return Status;
  }

  Attributes = 0;
  Status = Peim1Ppi->GetVar (
                       VariableName, 
                       &gVariableGuid,
                       Attributes, 
                       &DataSize,
                       &Data
                       );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Get Variable Status : %r\n", Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "Get Variable Status : %r\n", Status));
  }
  

  Status = Peim1Ppi->CreateHob (
                       EFI_HOB_TYPE_GUID_EXTENSION,
                       (UINT16)(sizeof(EFI_HOB_GUID_TYPE)) + (UINT16)DataSize,
                       (VOID**)&GuidHob
                       );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Create Hob Status : %r\n", Status));
    return Status;
  } else {
    DEBUG ((DEBUG_INFO, "Create Hob Status : %r\n", Status));
  }
  
  CopyGuid (&GuidHob->Name, &gMyHobGuid);
  HobPointer = GET_GUID_HOB_DATA (GuidHob);
  CopyMem (HobPointer, Data, DataSize);

  return Status;
}


EFI_PEIM2_PROTOCOL gMyPeim2ppi = {
  Peim2GetSetVar
};

EFI_PEI_PPI_DESCRIPTOR mPeim2PpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeim2PpiGuid,
  &gMyPeim2ppi
};


EFI_STATUS
EFIAPI
Peim2EntryPoint (
  IN EFI_PEI_FILE_HANDLE FileHandle,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_STATUS Status;

  DEBUG((DEBUG_INFO, "\n--------------------------------------------------------------------------------\n"));
  DEBUG((DEBUG_INFO, "----------------------                Peim2                ----------------------\n"));
  DEBUG((DEBUG_INFO, "--------------------------------------------------------------------------------\n\n"));
  //
  // Install the PPI
  //
  Status = Peim2GetSetVar();
  if (!EFI_ERROR(Status)) {
    Status = PeiServicesInstallPpi (&mPeim2PpiList);
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Install 'mPeim2PpiList' Status: %r\n", Status));
      return Status;
    } else {
      DEBUG ((DEBUG_INFO, "Install 'mPeim2PpiList' Status: %r\n", Status));
    }
  } // if

  return EFI_SUCCESS;
}
