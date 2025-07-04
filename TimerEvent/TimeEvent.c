/** @file
  Course11 Timer(Protocol) .c source code

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

#include "TimeEvent.h"

#define TIME_DISPLAY_INTERVAL 10000000
                              
EFI_EVENT gEvent;

/**
  Display current time and pause aany key to exit 

  @param Event    Time event
  @param Context  EFI_EVENT_NOTIFY struct need

  @retval EFI_SUCCESS    success
  @retval other          problem
**/
EFI_STATUS
EFIAPI
DisplayTime (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS    Status;
  EFI_TIME      Time;
  CHAR16        TimeString[32];
  UINTN         Row;
  UINTN         Column;

  //
  // get current time
  //
  Status = gRT->GetTime (&Time, NULL);

  UnicodeSPrint ( 
    TimeString, 
    sizeof(TimeString), 
    L"Date&Time  %04u-%02u-%02u  %02u:%02u:%02u",
    Time.Year, 
    Time.Month, 
    Time.Day, 
    Time.Hour, 
    Time.Minute, 
    Time.Second
    );

  Status = gST->ConOut->QueryMode (
                          gST->ConOut, 
                          gST->ConOut->Mode->Mode, 
                          &Column, 
                          &Row
                          );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Calculate position (upper right corner)
  //
  Column = Column - StrLen (TimeString) - 1;
  Row = 0;
  
  //
  // Set cursor position and print time
  //
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  Print (L"%s", TimeString);

  return Status;
}

TIME_EVENT_PROTOCOL gTimeEvent = {
  DisplayTime
};


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
TimeEventEntryPoint (                                         
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Dummy;

  Status = gBS->LocateProtocol (
                  &gTimeEventProtocolGuid, 
                  NULL, 
                  (VOID **)&Dummy
                  );
  if (!EFI_ERROR(Status)) {
    return EFI_ABORTED;
  }

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gTimeEventProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gTimeEvent
                  );
  if (EFI_ERROR(Status)) {
    Print (L"Failed to install TimeEvent Protocol: %r\n", Status);
    return Status;
  } else {
    Print (L"Success to install TimeEvent Protocol.\n");
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL | EVT_TIMER,
                  TPL_CALLBACK,
                  DisplayTime,
                  NULL,
                  &gEvent
                  );
  if (EFI_ERROR (Status)) {
    Print (L"Failed to create TimeEvent: %r\n", Status);
    return Status;
  } else {
    Print (L"Success to create TimeEvent.\n");
  }

  Status = gBS->SetTimer (
                  gEvent,
                  TimerPeriodic,
                  TIME_DISPLAY_INTERVAL
                  );
  if (EFI_ERROR(Status)) {
    Print (L"Failed to set timer: %r\n", Status);
    gBS->CloseEvent (gEvent);
    return Status;
  } else {
    Print (L"Success to set timer.\n");
  }
  
  return Status;
}

/**
  The user unload, user code end with this function
  as the real end point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
TimeEventUnload (
  IN EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS Status;

  Status = gBS->UninstallProtocolInterface (
                  ImageHandle,
                  &gTimeEventProtocolGuid,
                  &gTimeEvent
                  );
  if (EFI_ERROR(Status)) {
    Print (L"Failed to uninstall TimeEvent Protocol: %r\n", Status);
    return Status;
  } else {
    Print (L"Success to uninstall TimeEvent Protocol\n");
  }

  Status = gBS->SetTimer (
                  gEvent, 
                  TimerCancel, 
                  0
                  );
  if (EFI_ERROR(Status)) {
    Print (L"Failed to cancel Timer: %r\n", Status);
    return Status;
  } else {
    Print (L"Success to cancel Timer\n");
  }

  Status = gBS->CloseEvent (gEvent);
  if (EFI_ERROR(Status)) {
    Print (L"Failed to close event: %r\n", Status);
    return Status;
  } else {
    Print (L"Success to close event\n");
  }

  return Status;
}