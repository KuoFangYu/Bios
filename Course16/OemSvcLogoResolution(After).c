/** @file
  Provide OEM to customize resolution and SCU resolution. .

;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <ChipsetSetupConfig.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/OEMBadgingSupport.h>
#include <Protocol/JpegDecoder.h>
#include <Protocol/PcxDecoder.h>
#include <Protocol/TgaDecoder.h>
#include <Protocol/GifDecoder.h>
#include <Protocol/BmpDecoder.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VariableLib.h>
//
// data type definitions
//
#define  OEM_LOGO_RESOULTION              0x00

//
// module variables
//
OEM_LOGO_RESOLUTION_DEFINITION mOemLogoResolutionTable = {
  //
  // If set logo resolution to 0, the logo resolution will be get from GetLogoResolution() function.
  // Otherwise, it will use user prefer Resolution: 1024x768, 800x600, 640x480... .
  // OEM_LOGO_RESOULTION(resoultion depend on OEM logo)
  //
  OEM_LOGO_RESOULTION,       // LogoResolutionX;
  OEM_LOGO_RESOULTION,       // LogoResolutionY;
  //
  // SCU can support different resolutions ex:640x480, 800x600, 1024x768...
  // If user changes the resolution, please make sure
  // $(EFI_SOURCE)\$(PROJECT_FAMILY)\$(PROJECT_FAMILY)\OemServices\Dxe\UpdateFormLen\UpdateFormLen.c
  // also updating to correct value.
  //
  800,                       // SCUResolutionX;
  600                        // SCUResolutionY;
};

BOOLEAN   mUpdateLogoResolution = FALSE;
STATIC OEM_LOGO_RESOLUTION_DEFINITION mReturnResolutionTable;

STATIC
VOID
SwapResolutionTableXY (
  OEM_LOGO_RESOLUTION_DEFINITION        *ResolutionTable
  )
{
  UINT32                                TmpX;

  TmpX = ResolutionTable->LogoResolutionX;
  ResolutionTable->LogoResolutionX = ResolutionTable->LogoResolutionY;
  ResolutionTable->LogoResolutionY = TmpX;

  TmpX = ResolutionTable->ScuResolutionX;
  ResolutionTable->ScuResolutionX = ResolutionTable->ScuResolutionY;
  ResolutionTable->ScuResolutionY = TmpX;
}



//
// helper function prototypes
//
EFI_STATUS
GetLogoResolution (
  OUT UINT32                            *LogoResolutionX,
  OUT UINT32                            *LogoResolutionY
  );

/**
  Initialize module variable mPostKeyToOperation.

  @param  LogoResolutionX       Horizontal Logo resolution.
  @param  LogoResolutionY       Vertical Logo resolution.

  @retval EFI_UNSUPPORTED       Unsupported logo format.
**/
EFI_STATUS
GetLogoResolution (
  OUT UINT32                            *LogoResolutionX,
  OUT UINT32                            *LogoResolutionY
  )
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                 *Blt;
  UINTN                                         BltSize;
  EFI_JPEG_DECODER_PROTOCOL                     *JpegDecoder;
  EFI_JPEG_DECODER_STATUS                       DecoderStatus;
  EFI_PCX_DECODER_PROTOCOL                      *PcxDecoder;
  EFI_TGA_DECODER_PROTOCOL                      *TgaDecoder;
  EFI_BMP_DECODER_PROTOCOL                      *BmpDecoder;
  TGA_FORMAT                                    TgaFormate;
  BOOLEAN                                       HasAlphaChannel;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL              *Badging;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         Attribute;
  EFI_BADGING_SUPPORT_FORMAT                    Format;
  EFI_BADGING_SUPPORT_IMAGE_TYPE                Type;
  UINT8                                         *ImageData;
  UINTN                                         ImageSize;
  UINTN                                         CoordinateX;
  UINTN                                         CoordinateY;
  UINT32                                        Instance;
  UINTN                                         Height;
  UINTN                                         Width;
  UINTN                                         LocX;
  UINTN                                         LocY;
  EFI_STATUS                                    Status;
  EFI_GIF_DECODER_PROTOCOL                      *GifDecoder;
  ANIMATION                                     *Animation;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Badging = NULL;
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Set the request for the first image of type Logo
  //
  Instance = 0;
  Type = EfiBadgingSupportImageLogo;
  Format = EfiBadgingSupportFormatBMP;

  while (TRUE) {
    ImageData = NULL;
    ImageSize = 0;

    Status = Badging->GetImage (
                        Badging,
                        &Instance,
                        &Type,
                        &Format,
                        &ImageData,
                        &ImageSize,
                        &Attribute,
                        &CoordinateX,
                        &CoordinateY
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Determine and video resolution based on the logo
    //
    if (Type != EfiBadgingSupportImageLogo) {
      gBS->FreePool (ImageData);
      continue;
    }
    Blt             = NULL;
    BltSize         = 0;
    Height          = 0;
    Width           = 0;
    TgaFormate      = UnsupportedTgaFormat;
    HasAlphaChannel = FALSE;

    switch (Format) {

    case EfiBadgingSupportFormatBMP:
      Status = gBS->LocateProtocol (
                      &gEfiBmpDecoderProtocolGuid,
                      NULL,
                      (VOID **)&BmpDecoder
                      );
      if (!EFI_ERROR (Status)) {
        Status = BmpDecoder->DecodeImage (
                             BmpDecoder,
                             ImageData,
                             ImageSize,
                             (UINT8 **)&Blt,
                             &BltSize,
                             &Height,
                             &Width
                             );
      }
      break;

    case EfiBadgingSupportFormatTGA:
      Status = gBS->LocateProtocol (
                      &gEfiTgaDecoderProtocolGuid,
                      NULL,
                      (VOID **)&TgaDecoder
                      );
      if (!EFI_ERROR (Status)) {
        Status = TgaDecoder->DecodeImage (
                               TgaDecoder,
                               ImageData,
                               ImageSize,
                               (UINT8 **)&Blt,
                               &BltSize,
                               &Height,
                               &Width,
                               &TgaFormate,
                               &HasAlphaChannel
                               );
      }
      break;

    case EfiBadgingSupportFormatJPEG:
      Status = gBS->LocateProtocol (
                      &gEfiJpegDecoderProtocolGuid,
                      NULL,
                      (VOID **)&JpegDecoder
                      );
      if (!EFI_ERROR (Status)) {
        Status = JpegDecoder->DecodeImage (
                                JpegDecoder,
                                ImageData,
                                ImageSize,
                                (UINT8 **)&Blt,
                                &BltSize,
                                &Height,
                                &Width,
                                &DecoderStatus
                                );
      }
      break;

    case EfiBadgingSupportFormatPCX:
      Status = gBS->LocateProtocol (
                      &gEfiPcxDecoderProtocolGuid,
                      NULL,
                      (VOID **)&PcxDecoder
                      );
      if (!EFI_ERROR (Status)) {
        Status = PcxDecoder->DecodeImage (
                             PcxDecoder,
                             ImageData,
                             ImageSize,
                             (UINT8 **)&Blt,
                             &BltSize,
                             &Height,
                             &Width
                             );
      }
      break;

    case EfiBadgingSupportFormatGIF:
      Status = gBS->LocateProtocol (
                      &gEfiGifDecoderProtocolGuid,
                      NULL,
                      (VOID **)&GifDecoder
                      );
      if (!EFI_ERROR (Status)) {
        Status = GifDecoder->CreateAnimationFromMem (
                               GifDecoder,
                               ImageData,
                               ImageSize,
                               NULL,
                               &Animation
                               );
        if (!EFI_ERROR (Status)) {
          Width  = Animation->Width;
          Height = Animation->Height;

          GifDecoder->DestroyAnimation (
                        GifDecoder,
                        Animation
                        );
        }
      }
      break;

    default:
      Status = EFI_UNSUPPORTED;
      break;
    }

    if (EFI_ERROR (Status)) {
      gBS->FreePool (ImageData);
      continue;
    }

    gBS->FreePool (ImageData);
    gBS->FreePool (Blt);

    if (Badging->OemVideoModeScrStrXY (Badging, OemSupportedVideoMode, (UINT32)Width, (UINT32)Height, &LocX, &LocY)) {
      *LogoResolutionX = (UINT32) Width;
      *LogoResolutionY = (UINT32) Height;
      break;
    }
  }

  return Status;
}

/**
  Get OEM customization resolution and SCU resolution.
  Customization resolution provides OEM to define logo resolution directly or call GetLogoResolution () function to get logo resolution automatically.

  @param[out]  **OemLogoResolutionTable    A double pointer to OEM_LOGO_RESOLUTION_DEFINITION

  @retval      EFI_MEDIA_CHANGED           Get resolution information successful.
  @retval      Others                      Base on OEM design.
**/
EFI_STATUS
OemSvcLogoResolution (
  IN OUT OEM_LOGO_RESOLUTION_DEFINITION    **OemLogoResolutionTable
  )
{
  CHIPSET_CONFIGURATION                  *SystemConfiguration = NULL;
  EFI_STATUS                            Status;
  EFI_BOOT_MODE                         BootMode;
  UINT32                                OemLogoX;
  UINT32                                OemLogoY;

  Status = EFI_SUCCESS;

  if (*OemLogoResolutionTable != NULL) {
    if ((*OemLogoResolutionTable)->LogoResolutionX != 800 &&
        (*OemLogoResolutionTable)->LogoResolutionY != 600) {
      //
      // Please refer to gInsydeTokenSpaceGuid.PcdDefaultLogoResolution for default resolution
      //

      //
      // If the resolution is different from default vaules,
      // that means native resolution was found, and we only provides SCU resolution here
      //
      (*OemLogoResolutionTable)->ScuResolutionX = mOemLogoResolutionTable.ScuResolutionX;
      (*OemLogoResolutionTable)->ScuResolutionY = mOemLogoResolutionTable.ScuResolutionY;
      if (FeaturePcdGet (PcdH2ORotateScreenSupported) && PcdGetBool (PcdH2ORotateScreenRotateLogo)) {
        SwapResolutionTableXY (*OemLogoResolutionTable);
      }
      return EFI_MEDIA_CHANGED;
    }
  }
  if (!mUpdateLogoResolution) {
    BootMode = GetBootModeHob ();
    if (EFI_ERROR (Status)) {
      if (*OemLogoResolutionTable == NULL) {
        (*OemLogoResolutionTable) = &mOemLogoResolutionTable;
        if (FeaturePcdGet (PcdH2ORotateScreenSupported) && PcdGetBool (PcdH2ORotateScreenRotateLogo)) {
          CopyMem (&mReturnResolutionTable, &mOemLogoResolutionTable, sizeof (OEM_LOGO_RESOLUTION_DEFINITION));
          SwapResolutionTableXY (&mReturnResolutionTable);
          (*OemLogoResolutionTable) = &mReturnResolutionTable;
        }
      }
      return EFI_UNSUPPORTED;
    }
    SystemConfiguration = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
    if (SystemConfiguration == NULL) {
      return EFI_UNSUPPORTED;
    }

    if ( ( BootMode == BOOT_IN_RECOVERY_MODE ) || ( SystemConfiguration->QuietBoot == FALSE ) ) {
      mOemLogoResolutionTable.LogoResolutionX = mOemLogoResolutionTable.ScuResolutionX;
      mOemLogoResolutionTable.LogoResolutionY = mOemLogoResolutionTable.ScuResolutionY;
    } else if ( mOemLogoResolutionTable.LogoResolutionX == OEM_LOGO_RESOULTION ) {
      Status = GetLogoResolution (&OemLogoX, &OemLogoY);
      if (!EFI_ERROR (Status)) {
        mOemLogoResolutionTable.LogoResolutionX = OemLogoX;
        mOemLogoResolutionTable.LogoResolutionY = OemLogoY;
        if (*OemLogoResolutionTable != NULL) {
          (*OemLogoResolutionTable)->LogoResolutionX = mOemLogoResolutionTable.LogoResolutionX;
          (*OemLogoResolutionTable)->LogoResolutionY = mOemLogoResolutionTable.LogoResolutionY;
        }
      } else {
        mOemLogoResolutionTable.LogoResolutionX = 800;
        mOemLogoResolutionTable.LogoResolutionY = 600;
      }
    }
    mUpdateLogoResolution = TRUE;
    gBS->FreePool (SystemConfiguration);
  }

  if (*OemLogoResolutionTable == NULL) {
    //
    // The table is only NULL when the caller is an Ecp driver,
    // this case can be removed when there's no Ecp caller.
    //

    //
    // No native resolution is found
    // Use resolution table above
    //
    (*OemLogoResolutionTable) = &mOemLogoResolutionTable;
    if (FeaturePcdGet (PcdH2ORotateScreenSupported) && PcdGetBool (PcdH2ORotateScreenRotateLogo)) {
      CopyMem (&mReturnResolutionTable, &mOemLogoResolutionTable, sizeof (OEM_LOGO_RESOLUTION_DEFINITION));
      SwapResolutionTableXY (&mReturnResolutionTable);
      (*OemLogoResolutionTable) = &mReturnResolutionTable;
    }
  } else {
    (*OemLogoResolutionTable)->ScuResolutionX = mOemLogoResolutionTable.ScuResolutionX;
    (*OemLogoResolutionTable)->ScuResolutionY = mOemLogoResolutionTable.ScuResolutionY;
    if (FeaturePcdGet (PcdH2ORotateScreenSupported) && PcdGetBool (PcdH2ORotateScreenRotateLogo)) {
      SwapResolutionTableXY (*OemLogoResolutionTable);
    }
  }
  return EFI_MEDIA_CHANGED;
}

