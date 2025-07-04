/** @file
 VFR application

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

//;*****************************************************************************
// change at line 147-150 and 181-187
//;*****************************************************************************

#ifndef _VFR_APP_NVDATASTRUC_H_
#define _VFR_APP_NVDATASTRUC_H_

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/HiiFormMapMethodGuid.h>
#include <Guid/DriverSampleHii.h>
#include <Guid/ZeroGuid.h>

#define CONFIGURATION_VARSTORE_ID    0x1000

#define VFR_APP_FORMSET_GUID \
  { \
    0xA04A27f4, 0xDF00, 0x1234, {0xB5, 0x52, 0x39, 0x51, 0x13, 0x02, 0x11, 0x3F} \
  }

#pragma pack(1)
typedef struct {
  UINT8   TestH2ODialog;
  UINT8   ActiveDisplayEngine;

  UINT8   Numeric001;
  UINT64  Numeric002;
  UINT64  Numeric003;
  UINT8   Numeric004;
  UINT8   Numeric005;
  UINT8   Numeric006;

  UINT8   OneOf001;
  UINT8   OneOf002;
  UINT8   OneOf003;
  UINT8   OneOf004;

  UINT16  String001[10];
  UINT16  String002[200];
  UINT16  String003[10];

  UINT16  Password001[20];


  UINT8   OneOf1;
  UINT8   OneOf2;
  UINT8   OneOf3;
  UINT8   OneOf4;

  UINT8   Order1[37];
  UINT8   Order2[24];

  UINT8   Checkbox0001;


  // Setup Page panel related questions
  UINT8   SetupPagePanelOneOf0001;
  UINT8   SetupPagePanelOneOf0002;
  UINT8   SetupPagePanelOneOf0003;
  UINT8   SetupPagePanelOneOf0004;
  UINT8   SetupPagePanelOneOf0005;
  UINT8   SetupPagePanelOneOf0006;
  UINT8   SetupPagePanelOneOf0007;
  UINT8   SetupPagePanelOneOf0008;
  UINT8   SetupPagePanelOneOf0009;
  UINT8   SetupPagePanelOneOf0010;
  UINT8   SetupPagePanelOneOf0011;
  UINT8   SetupPagePanelOneOf0012;
  UINT8   SetupPagePanelOneOf0013;
  UINT8   SetupPagePanelOneOf0014;
  // Help Text panel related questions
  UINT8   HelpTextPanelOneOf0001;
  UINT8   HelpTextPanelOneOf0002;
  UINT8   HelpTextPanelOneOf0003;
  UINT8   HelpTextPanelOneOf0004;
  UINT8   HelpTextPanelOneOf0005;
  // Hot Key panel related questions
  UINT8   HotKeyPanelNumeric0001;
  UINT8   HotKeyPanelNumeric0002;
  UINT8   HotKeyPanelNumeric0003;
  UINT8   HotKeyPanelNumeric0004;
  UINT8   HotKeyPanelNumeric0005;
  UINT8   HotKeyPanelNumeric0006;
  UINT8   HotKeyPanelNumeric0007;
  UINT8   HotKeyPanelNumeric0008;
  UINT8   HotKeyPanelOneOf0001;
  UINT8   HotKeyPanelOneOf0002;
  UINT8   HotKeyPanelOneOf0003;
  UINT8   HotKeyPanelOneOf0004;
  UINT8   HotKeyPanelOneOf0005;
  UINT8   HotKeyPanelOneOf0006;
  UINT16  HotKeyPanelString0001[10];
  UINT16  HotKeyPanelPassword0001[20];
  UINT8   HotKeyPanelOrder1[10];
  UINT8   HotKeyPanelCheckbox0001;

  //
  //
  UINT16  WhatIsThePassword[20];
  UINT16  WhatIsThePassword2[20];
  UINT16  MyStringData[40];
  UINT16  PasswordClearText[20];
  UINT16  SomethingHiddenForHtml;
  UINT8   HowOldAreYouInYearsManual;
  UINT16  HowTallAreYouManual;
  UINT8   HowOldAreYouInYears;
  UINT16  HowTallAreYou;
  UINT8   MyFavoriteNumber;
  UINT8   TestLateCheck;
  UINT8   TestLateCheck2;
  UINT8   QuestionAboutTreeHugging;
  UINT8   ChooseToActivateNuclearWeaponry;
  UINT8   SuppressGrayOutSomething;
  UINT8   OrderedList[8];
  UINT16  BootOrder[8];
  UINT8   BootOrderLarge;
  UINT8   DynamicRefresh;
  UINT8   DynamicOneof;
  UINT8   DynamicOrderedList[5];
  UINT8   Reserved;
  UINT8   NameValueVar0;
  UINT16  NameValueVar1;
  UINT16  NameValueVar2[20];
  UINT8   SerialPortNo;
  UINT8   SerialPortStatus;
  UINT16  SerialPortIo;
  UINT8   SerialPortIrq;
  UINT8   GetDefaultValueFromCallBack;
  UINT8   GetDefaultValueFromAccess;
  UINT8   RefreshGuidCount;

  // NTC HW
  UINT8   NTCNumber;
  UINT8   Operater;
  UINT8   NTCNumber2;
  UINT16  NTCTotal;

} VFR_APP_CONFIGURATION;

//
// 2nd NV data structure definition
//
typedef struct {
  UINT8         Field8;
  UINT16        Field16;
  UINT8         OrderedList[3];
} VFR_APP_EFI_VARSTORE_DATA;

//
// Labels definition
//
#define LABEL_UPDATE1               0x1234
#define LABEL_UPDATE2               0x2234
#define LABEL_UPDATE3               0x3234
#define LABEL_END                   0x2223

#define VFR_APP_ACTIVE_DISPLAY_ENGINE_ALL     0
#define VFR_APP_ACTIVE_DISPLAY_ENGINE_METRO   1
#define VFR_APP_ACTIVE_DISPLAY_ENGINE_TEXT    2

//
// Question ID definition
//
#define VFR_APP_Q_ID_FB_FUNC_VFR_REFRESH_INTERVAL_ONE       0x8000
#define VFR_APP_Q_ID_FB_FUNC_VFR_REFRESH_INTERVAL_TWO       0x8001

//
// NTC hw
//
#define NTC_HW_INTERVAL_NUM1       0x8002
#define NTC_HW_INTERVAL_NUM2       0x8003
#define NTC_HW_INTERVAL_OPERATOR   0x8004
#define NTC_HW_INTERVAL_TOTAL      0x8005

#pragma pack()

#endif
