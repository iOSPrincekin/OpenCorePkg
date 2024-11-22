/** @file
 Bootstrap OpenCore driver.
 
 Copyright (c) 2018, vit9696. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php
 
 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
 **/
#include "string.h"
#include <Library/OcMainLib.h>
#include <Uefi.h>


#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/Cpu.h>
#include <Protocol/AppleRtcRam.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/OSInfo.h>
#include <Protocol/DataHub.h>
#include <Protocol/BlockIoCrypto.h>
#include <Protocol/DiskIo.h>
#include <Protocol/AppleSmcIo.h>
#include <Protocol/AppleSecureBoot.h>
#include <Protocol/LoadFile.h>
#include <Protocol/UserInterfaceTheme.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/AppleFramebufferInfo.h>
#include <Protocol/AppleImageConversion.h>


#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/OcDebugLogLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OcDevicePathLib.h>
#include <Library/OcFileLib.h>
#include <Library/OcStringLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PrintLib.h>
#include <Guid/HobList.h>
#include <Guid/OcVariable.h>
#include <Guid/GlobalVariable.h>
#include <Guid/AppleOSLoaded.h>
#include <Guid/AppleApfsInfo.h>
#include <Guid/SmBios.h>
#include <IndustryStandard/AppleSmBios.h>

//#include "UefiShellDebug1CommandsLib.h"


#include "defs.h"

#pragma mark ============================== macro define begin ================================

#define _AUPBND 1
#define _ADNBND 1
#define _Bnd(X, bnd) (sizeof(X) + ((bnd) & ~(bnd)))

typedef char *va_list;
#define va_arg(ap, T) \
(*(T *)(((ap) += _Bnd(T, _AUPBND)) - _Bnd(T, _ADNBND)))
#define va_end(ap) (void)0
#define va_start(ap, A) \
(void)((ap) = (char *)&(A) + _Bnd(A, _AUPBND))

#define va_copy(d, s)        __builtin_va_copy(d, s)


#define SETHIWORD(x,y) x = (((UINT32)(x) & 0x0000FFFF) | (((UINT32)(y) << 16) & 0xFFFF0000))

#define SETLOWORD(x,y) x = (((UINT32)(x) & 0xFFFF0000) | (((UINT32)(y) >> 16) & 0x0000FFFF))

#define __ROL__(value, count) \
((value << count) | (value >> (sizeof(value)*8 - count)))

#define __ROL1__(value, count) __ROL__((UINT8)value, count)

#define __ROL2__(value, count) __ROL__((UINT16)value, count)

#define __ROL4__(value, count) __ROL__((UINT32)value, count)

#define __ROL8__(value, count) __ROL__((UINT64)value, count)

#define __ROR1__(value, count) __ROL__((UINT8)value, -count)

#define __ROR2__(value, count) __ROL__((UINT16)value, -count)

#define __ROR4__(value, count) __ROL__((UINT32)value, -count)

#define __ROR8__(value, count) __ROL__((UINT64)value, -count)

#define NULL_GUID { 0xAAAAAAAA, 0xAAAA, 0xAAAA, { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA }}

#define DEFAULT_GUID { 0xAAAAAAAA, 0xAAAA, 0xAAAA, { 0xA8, 0x80, 0xFE, 0x41, 0x99, 0x5C, 0x9F, 0x82 }}



#pragma mark ======================================= macro define end ================================




#pragma mark =============================== data struct define begin ================================

typedef struct StringStruct1 {
    void* string1;
    UINT64 size;
    void* next1;
    char v1;
    void* next3;
}StringStruct1;

typedef struct StringStruct2 {
    StringStruct1* next0;
    void* next1;
    void* next2;
    void* next3;
}StringStruct2;

typedef struct LOG_CONFIG_INFO{
    char* name;
    UINT64 config1;
    UINT32 config2;
    UINT32 config3;
    UINT64* config4;
    UINT64* sign;
}LOG_CONFIG_INFO;


#pragma mark ==================================== data struct define end ================================





#pragma mark ========================================variable data begin ================================



STATIC EFI_HANDLE  mImageHandle;

STATIC EFI_SYSTEM_TABLE* mSystemTable;

STATIC EFI_BOOT_SERVICES* mBootServices;

STATIC EFI_RUNTIME_SERVICES* mRuntimeServices;

STATIC SMBIOS_TABLE_ENTRY_POINT *mSmbiosTable = NULL;

void *GuidHob_24 = NULL;

EFI_CPU_ARCH_PROTOCOL *mCpu = NULL;

/*
 #define gEfiHobListGuid { 0x7739F24C, 0x93D7, 0x11D4, { 0x9A, 0x3A, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }}
 
 */
EFI_GUID  unknown_hob_guid = {
    0xef4ae2dd, 0xb736, 0x40e3, {0x80, 0x61, 0xa7, 0x46, 0x33, 0x34, 0x7f, 0x23 }
};

typedef struct {
    UINT8 byteValue;
    UINT64 values[4];
    UINT8 additionalBytes[2];
} Byte_E467_Struct;

Byte_E467_Struct byte_E467 = {
    0x0F,
    {0x220F1FF0BA0FC020,0x80B9D789CB89C0,0x8F0BA0F320FC000,0xE7FFD88900EB300F},
    0xF4, 0xC3
};

typedef struct {
    UINT32 dwordValue;
    UINT16 wordValue;
} Data_E48A_Struct;

Data_E48A_Struct dword_E48A = {
    0,
    8
};

char byte_E490[10] = {0x18,0,0,0,0,0,0,0,0,0};

typedef struct {
    UINT16 word_E49A[3];
    UINT64 dq_values[2];
    UINT8 additionalBytes[2];
} Word_E49A_Struct;

Word_E49A_Struct word_E49A = {
    {0,0,0},
    {0x9E000000FFFF0000,0x92000000FFFF00CF},
    {0xCF,0}
};

void sub_44281(void);

__int64 sub_4424F(int a1, int a2, int a3, int a4);

void sub_447BC(void);

void sub_44808(void);

void sub_448E2(void);

void sub_4421D(void);

void sub_44A5B(void);


void* funcs_4421A[7] = {
    sub_44281,
    sub_4424F,
    sub_447BC,
    sub_44808,
    sub_448E2,
    sub_4421D,
    sub_44A5B
};

UINT64* qword_4C5F8 = 0;

UINT64* qword_A9CD0 = 0;

UINT64 qword_A9D50 = 0;

UINT64 qword_AA550 = 0;

UINT64* qword_AA9D0 = NULL;

UINT64* qword_AAA10 = NULL;

UINT64* qword_AAA60 = NULL;

UINT64* qword_AAA98 = NULL;

UINT64* qword_AAAF0 = NULL;

UINT64 unk_AC480 = 0x0;

EFI_GUID unk_AD140 = {
    0xef4ae2dd, 0xb736, 0x40e3, {0x80, 0x61, 0xa7, 0x46, 0x33, 0x34, 0x7f, 0x23 }
};

char byte_AD160 = 0;

UINT8 byte_AD1D8 = 0;

UINT8 byte_AD1D9 = 0;

UINT8 byte_AD1DA = 0;

UINT8 byte_AD1DB = 0;

UINT32 dword_AD218 = 0;

UINT8 byte_AD220 = 0;

UINT8 byte_AD221 = 0;

UINT32 dword_AD244 = 0;

char off_AC720[0x18] = {0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0x3c,0,0,0,0,0,0,0};

UINT64 off_AD210 = 0;

#define off_AD250_count 10

const char* off_AD250[off_AD250_count] = {
    "Mac-35C1E88140C3E6CF",
    "Mac-7DF21CB3ED6977E5",
    "Mac-3CBD00234E554E41",
    "Mac-2BD1B31983FE1663",
    "Mac-031B6874CF7F642A",
    "Mac-77EB7D7DAF985301",
    "Mac-27ADBB7B4CEE8E61",
    "Mac-189A3D4F975D5FFC",
    "Mac-742912EFDBEE19B3",
    "Mac-C08A6BB70A942AC2"
};

UINT64 unk_AD370 = 0x2;

UINT64 unk_AD378 = 0x2;

UINT64 unk_AD3A8 = 0x0;

UINT64 qword_AD380 = 0x1;

UINT64 qword_AD388 = 0x1;

UINT64 qword_AD390 = 0x0;

UINT64 qword_AD398 = 0x0;

UINT64 qword_AD3A0 = 0x1;

UINT64 qword_AD3B0 = 0x100000;

UINT64 qword_AD3B8 = 0x0;

UINT64 unk_AD620 = 0x0101;

UINT64 off_AD628 = 0x0201;

UINT32 dword_AD818 = 0x0BFBFBF;

UINT32 dword_AD81C = 0xFFFFFFFF;

UINT32 dword_AD820 = 0xFFFFFFFF;

#define off_AD8F0_count 4

const char* off_AD8F0[off_AD8F0_count] = {
    "debug",
    "kdp_match_name",
    "-x",
    "-v",
};

const char* off_AD920 = "-s";

#define off_AD930_count 4

const char* off_AD930[off_AD930_count] = {
    "-s",
    "chunklist-security-epoch",
    "-chunklist-no-rev1",
    "-chunklist-no-rev2-dev"
};


char* byte_AD960 = NULL;

// gAppleDiskIoProtocolGuid
EFI_GUID unk_ADB80 = { 0x5B27263B, 0x9083, 0x415E, { 0x88, 0x9E, 0x64, 0x32, 0xCA, 0xA9, 0xB8, 0x13 }};

EFI_GUID unk_ADB90 = {0xc7db7e1e,0x0dd0,0x4796,{0x90,0xd8,0x59,0x6d,0x89,0xa2,0x88,0x16}};

EFI_GUID qword_ADBB0 = {0x47022197,0x24B7,0x3556,{0xF2,0xFB,0xDA,0x37,0x13,0x3E,0xA8,0x82}};

// gAppleFirmwarePasswordProtocolGuid
EFI_GUID unk_ADBC0 = { 0x8FFEEB3A, 0x4C98, 0x4630, { 0x80, 0x3F, 0x74, 0x0F, 0x95, 0x67, 0x09, 0x1D }};

EFI_GUID qword_ADC40 = {0x430BA6D4,0x8ECE,0x08D8,{0x4A,0x88,0xE7,0x18,0xF3,0x2D,0xB0,0xA7}};

UINT64 qword_ADFE8 = 0;
//GraphConfig
EFI_GUID unk_AE8DC = {0x03622D6D,0x362A,0x4E47,{0x97,0x10,0xC2,0x38,0xB2,0x37,0x55,0xC1}};

UINT32 dword_AD958 = 0;

void* qword_AE970 = 0;

UINT8 byte_AE978 = 0;

char unk_AE980[0x3e] = {0};

char *qword_AE9C0 = NULL;

UINT64 qword_AEA20 = 0;

char *qword_AEE30 = NULL;

char* qword_AEE38 = NULL;

UINT64 qword_AEE40 = 0;

UINT8 byte_AEF88 = 0;

void* qword_AEF90 = NULL;

char byte_AEF98 = 0;

StringStruct1* qword_AEFA0 = NULL;

StringStruct1* qword_AEFA8 = NULL;

StringStruct2* qword_AEFB0 = NULL;

StringStruct2* qword_AEFB8 = NULL;

StringStruct2* qword_AEFC0 = NULL;

UINT32 dword_AEFC8 = 0;

UINT32 dword_AEFCC = 0;

UINT32 dword_AEFD0 = 0;

UINT8 byte_AEFE0 = 0;

EFI_PHYSICAL_ADDRESS qword_AEFE8 = 0;

UINT64 qword_AEFF0 = 0;

UINT64 qword_AEFF8 = 0;

UINT64 qword_AF000 = 0;

UINT64 qword_AF008 = 0;

UINT32 dword_AF110 = 0;

UINT32 dword_AF114 = 0;

UINT64 qword_AF118 = 0;

UINT64 qword_AF120 = 0;

UINT64 qword_AF128 = 0;

UINT32 dword_AF130 = 0;

UINT32 dword_AF134 = 0;

UINT64 qword_AF138 = 0;

UINT8 byte_AF140 = 0;

VOID* qword_AF148 = NULL;

UINT32 dword_AF150 = 0;

UINT32 dword_AF154 = 0;


EFI_CONSOLE_CONTROL_SCREEN_MODE dword_AF158 = 0;

EFI_CONSOLE_CONTROL_SCREEN_MODE dword_AF15C = 0;

UINT64 qword_AF160 = 0;

UINT64 qword_AF168 = 0;

UINT32 dword_AF170 = 0;

UINT64 qword_AF178 = 0;

UINT64 qword_AF180 = 0;

char* byte_AF1B0 = NULL;

UINT64** qword_B01E8 = 0;

UINT64* qword_B01F0 = 0;

UINT64* qword_B01F8 = 0;

UINT8 byte_B0220[0x20] = {};

UINT64 qword_B0200 = 0;

void* qword_B0210 = NULL;

UINT8 unk_B022E[0x30] = {};

UINT16 word_B025E = 0;

UINT16 word_B0278 = 0;

UINT8 byte_B0320 = 0;

UINT8 byte_B0321 = 0;

UINT64 qword_B0328 = 0;

UINT8 byte_B0331 = 0;

UINT8 byte_B0332 = 0;

UINT8 byte_B0333 = 0;

UINT8 byte_B0340 = 0;

UINT8 byte_B0341 = 0;

UINT8 byte_B0342 = 0;

UINT64 qword_B0348 = 0;

UINT8 byte_B0350 = 0;

UINT32 dword_B0354 = 0;

UINT64* qword_B0358 = 0;

UINT32* qword_B0360 = (UINT32*)4261634048LL;

UINT8 byte_B0370 = 0;

UINT32 dword_B1D7C = 0;

UINT8 byte_B1DC9 = 0;

EFI_HANDLE qword_B1DD0 = NULL;

void* qword_B1DD8 = NULL;

UINT64 qword_B1DE0 = 0;

UINT64 qword_B1DE8 = 0;

UINT64 qword_B1DF0 = 0;

UINT64 qword_B1DF8 = 0;

UINT64 qword_B1E00 = 0;

UINT64 qword_B1E08 = 0;

char byte_B1E10 = 0;

void* qword_B1E18 = NULL;

void* qword_B1E20 = 0;

EFI_FILE_PROTOCOL* qword_B1E28 = NULL;

void* qword_B1E38 = 0;

UINT32 dword_B1E48 = 0;

UINT64 qword_B1E48 = 0;

UINT32 dword_B1E50 = 0;

UINT64 qword_B1E50 = 0;

UINT64 qword_B1E58 = 0;

UINT64 qword_B1F18 = 0;

UINT64 qword_B1F20 = 0;

VOID* qword_B1F28 = NULL;

UINT8 byte_B1F30 = 0;

UINT8 byte_B1F31 = 0;

UINT64 qword_B1F38 = 0;

EFI_DISK_IO_PROTOCOL* qword_B1F50 = NULL;

void* qword_B1F58 = NULL;

void* qword_B1F60 = NULL;

UINT32 dword_B1F68 = 0;

UINT64 qword_B1F70 = 0;

EFI_DEVICE_PATH_PROTOCOL* qword_B1F78 = 0;

EFI_CONSOLE_CONTROL_PROTOCOL* qword_B1F40 = NULL;

EFI_DISK_IO_PROTOCOL* qword_B1F80 = NULL;

UINT8 byte_B1F90[0x20] = {};

UINT8 unk_B1FB0[0x20] = {};

UINT64 qword_B1FD0 = 0;

UINT64 qword_B1FD8 = 0;

UINT64 qword_B1FE0 = 0;

UINT8 unk_B1FE8[20] = {};

UINT8 unk_B1FFC[28] = {};

UINT64 qword_B2028 = 0;

UINT64 qword_B2038 = 0;

UINT64 qword_B2040 = 0;

void* qword_B2058 = 0;

EFI_EVENT qword_B2060 = NULL;

UINT64 qword_B2068 = 0;

UINT64 qword_B2070 = 0;

char* qword_B2078 = 0;

char* qword_B2080 = 0;

UINT64 qword_B2098 = 0;

UINT32 addr_FE03401C = 0xFE03401C;

#define log_config_list_count  12

LOG_CONFIG_INFO log_config_list[log_config_list_count] = {
    {
        "boot-save-log",
        0x2,
        0x00000001,
        0x00000001,
        (UINT64*)0xffffffffffffffff,
        &unk_AD370
    },
    {
        "wake-save-log",
        0x2,
        0x00000001,
        0x00000001,
        (UINT64*)0x2,
        &unk_AD378
    },
    {
        "console",
        0x1,
        0x00000001,
        0x00000001,
        (UINT64*)0x1,
        &qword_AD380
    },
    {
        "serial",
        0x1,
        0x00000001,
        0x00000001,
        (UINT64*)0x0,
        &qword_AD388
    },
    {
        "embed-log-dt",
        0x0,
        0x00000001,
        0x00000001,
        (UINT64*)0x0,
        &qword_AD390
    },
    {
        "timestamps",
        0x0,
        0x00000001,
        0x00000001,
        (UINT64*)0xffffffffffffffff,
        &qword_AD398
    },
    {
        "log-level",
        0x1,
        0x00000001,
        0x00000000,
        (UINT64*)0x0000000000000021,
        &qword_AD3A0
    },
    {
        "breakpoint",
        0x0,
        0x00000001,
        0x00000000,
        0x0,
        &unk_AD3A8
    },
    {
        "kc-read-size",
        0x100000,
        0x00000001,
        0x00000001,
        (UINT64*)0xffffffffffffffff,
        &qword_AD3B0
    },
    {
        "log",
        0x0,
        0x00000001,
        0x00000002,
        0x0,
        NULL
    },
    {
        "debug",
        0x0,
        0x00000001,
        0x00000002,
        0x0,
        NULL
    },
    {
        "level",
        0x0,
        0x00000001,
        0x00000002,
        0x0,
        NULL
    },
};

#pragma mark ============================== variable data end ================================

#pragma mark ============================== functions begin ================================

EFI_STATUS
EFIAPI
HobLibConstructorPtr (
                      IN EFI_SYSTEM_TABLE  *SystemTable,
                      VOID** mHobListPtr
                      )
{
    UINTN  Index;
    
    for (Index = 0; Index < SystemTable->NumberOfTableEntries; Index++) {
        if (CompareGuid (&gEfiHobListGuid, &(SystemTable->ConfigurationTable[Index].VendorGuid))) {
            *mHobListPtr = SystemTable->ConfigurationTable[Index].VendorTable;
            return EFI_SUCCESS;
        }
    }
    
    return EFI_NOT_FOUND;
}

void sub_28F47(void *GuidHob)
{
    GuidHob_24 = GuidHob;
}

EFI_STATUS sub_28C58(
                     IN EFI_HANDLE        ImageHandle,
                     IN EFI_SYSTEM_TABLE  *SystemTable
                     )
{
    EFI_STATUS  Status;
    
    // qword_B2090
    mSystemTable = SystemTable;
    // qword_B2098
    mBootServices = SystemTable->BootServices;
    // qword_B20A0
    mRuntimeServices = SystemTable->RuntimeServices;
    void* hobList = NULL;
    Status = HobLibConstructorPtr (SystemTable,&hobList);
    
    void *GuidHob = GetNextGuidHob (&unknown_hob_guid, hobList);
    if(GuidHob){
        sub_28F47(GuidHob + 3);
    }
    return Status;
}

void* AllocatePool_malloc(UINTN bufferSize)
{
    void* bufferPtr = NULL; // [rsp+20h] [rbp-10h] BYREF
    EFI_ALLOCATE_POOL AllocatePool = mBootServices->AllocatePool;
    AllocatePool(4LL, bufferSize, &bufferPtr);
    return bufferPtr;
}



UINT64* sub_7CE38528(void){
    UINT64* result = qword_B01F8;
    UINT64** buffer = NULL;
    UINT64 v1 = 0;
    if(qword_B01F8){
        v1 = *(UINT64 *)(qword_B01F8 + 24);
    }else{
        buffer = AllocatePool_malloc(4096);
        if(buffer == NULL){
            DEBUG ((DEBUG_INFO,"AAPL: #[EB.M.GT|!] NULL <- EDK.ELAP\n"));
        }
        UINT64* v3 = qword_B01F8;
        for (UINT32 i = 0LL; i != 508; i += 4LL )
        {
            UINT64* v1 = v3;
            v3 = (UINT64*)&buffer[i];
            buffer[i + 3] = v1;
        }
        result = (UINT64*)buffer + 504;
        qword_B01F8 = result;
        buffer[508] = (UINT64*)buffer;
        buffer[510] = 0;
        buffer[509] = 0;
        buffer[511] = qword_B01F0;
        qword_B01F0 = (UINT64*)buffer + 508;
    }
    qword_B01F8 = (UINT64*)v1;
    result[3] = 0;
    return result;
}

void* sub_1D2B1(UINTN bufferSize)
{
    void* buffer = NULL;
    buffer = NULL;
    /*
     UINTN *v1; // rax
     */
    buffer = AllocatePool_malloc(bufferSize);
    UINT64* v2 = sub_7CE38528();
    
    v2[0] = (UINT64)buffer;
    v2[2] = 0LL;
    v2[1] = 0LL;
    v2[3] = (UINT64)qword_B01E8;
    qword_B01E8 = (UINT64**)v2;
    return buffer;
}

BOOLEAN _bittest64(UINT64 *a, UINT32 b)
{
    return (*a & (1LL << b)) != 0;
}

// strlen
UINT32 sub_2822A(const char* a1)
{
    return strlen(a1);
}


UINT64 sub_28556(char* a1, char* a2, UINT64 a3)
{
    UINT64 result; // rax
    UINT64 v4; // r9
    UINT32 v5; // r10d
    
    result = 0LL;
    v4 = 0LL;
    while ( a3 > 0 )
    {
        v5 = *(char *)(a1 + v4);
        if ( ((*(a1 + v4) ^ *(a2 + v4)) & 0xDF) != 0 )
            return (v5 & 0xFFFFFFDF) - (*(char *)(a2 + v4) & 0xFFFFFFDF);
        --a3;
        ++v4;
        if ( !v5 )
            return result;
    }
    return result;
}

UINT64 sub_1D5D4(char *a1, char* a2, UINT64 *a3, char **a4){
    
    EFI_STATUS Status = RETURN_NOT_FOUND;
    char* v4 = a1;
    char v5 = *a1;
    UINT64 v7;
    UINT64 v8;
    char* v9;
    char* v10;
    char v11;
    UINT64 v12;
    UINT64 v13;
    char* v14;
    UINT64 v15;
    UINT8 v16;
    char v17; // al
    UINT64 v19 = 0; // [rsp+28h] [rbp-58h]
    UINT64 v22 = 0; // [rsp+40h] [rbp-40h]
    if(v5){
        v7 = 0x2000000100002601LL;
        v8 = 0x100002600LL;
        do{
            v9 = v4 + 1;
            while ( v5 <= 0x22u )
            {
                if ( !_bittest64(&v8, v5) )
                {
                    if ( v5 == 34LL )
                    {
                        v10 = v9;
                        do
                            v11 = *v10++;
                        while ( v11 != 34 && v11 );
                        v12 = (UINT64)v10 + ~(UINT64)v9;
                        v5 = *v10;
                        goto LABEL_18;
                    }
                    break;
                }
                v5 = *v9++;
            }
            v10 = --v9;
            while ( v5 > 0x3Du || !_bittest64(&v7, v5) )
                v5 = *++v10;
            v12 = v10 - v9;
        LABEL_18:
            if ( v5 <= 0x20u && (v13 = 0x100002601LL && _bittest64(&v13, v5)) )
            {
                v14 = 0LL;
                v4 = (char *)v10;
            }
            else
            {
                v16 = *(v10 + 1);
                if ( v16 == 34 )
                {
                    v10 += 2LL;
                    v4 = (char *)v10;
                    do
                        v17 = *v4++;
                    while ( v17 != 34 && v17 );
                    v14 = v4 + ~(UINT64)v10;
                }
                else
                {
                    v4 = (char *)++v10;
                    while ( v16 > 0x3Du || !_bittest64(&v7, v16) )
                        v16 = *++v4;
                    v14 = v4 - (UINT64)v10;
                }
            }
            if ( v12 == sub_2822A(a2) )
            {
                v15 = sub_28556(a2, v9, v12);
                v7 = 0x2000000100002601LL;
                if ( !v15 )
                {
                    *a3 = (UINT64)v10;
                    *a4 = v14;
                    v22 = 0LL;
                }
            }
            else
            {
                v7 = 0x2000000100002601LL;
            }
            v5 = *v4;
            a2 = (char*)v19;
        }
        while ( *v4 );
    }
    return v22 | Status;
}

UINT64 sub_2834C(char *a1, char **a2, UINT64 a3)
{
    char *i; // r10
    char v5; // al
    char v6; // r11
    UINT32 v7; // esi
    bool v8; // dl
    UINT32 v9; // eax
    UINT64 v10; // rax
    UINT64 v11; // r15
    char *v12; // r10
    UINT32 v13; // r14d
    UINT32 v14; // esi
    UINT32 v15; // ebx
    UINT64 v16; // rdi
    UINT64 v17; // rcx
    UINT64 result; // rax
    
    for ( i = a1 + 1; ; ++i )
    {
        v5 = *(i - 1);
        if ( v5 > 31 )
            break;
        if ( (UINT8)(v5 - 9) >= 2u )
            goto LABEL_12;
    LABEL_6:
        ;
    }
    switch ( v5 )
    {
        case ' ':
            goto LABEL_6;
        case '+':
            v6 = 1;
            goto LABEL_11;
        case '-':
            v6 = 0;
        LABEL_11:
            v5 = *i++;
            goto LABEL_13;
    }
LABEL_12:
    v6 = 1;
LABEL_13:
    if ( (a3 & 0xFFFFFFEF) != 0 || v5 != 48 )
    {
        v8 = a3 == 0;
        if ( v5 == 48 )
            goto LABEL_20;
        v7 = v5;
        v9 = 10;
    }
    else
    {
        if ( ((UINT8)*i | 0x20) == 0x78 )
        {
            v7 = i[1];
            i += 2;
            a3 = 16;
            goto LABEL_23;
        }
        v8 = a3 == 0;
    LABEL_20:
        v9 = 8;
        v7 = 48;
    }
    if ( v8 )
        a3 = v9;
LABEL_23:
    v10 = 0xFFFFFFFFFFFFFFFFuLL / a3;
    v11 = 0LL;
    v12 = i - 1;
    v13 = 0;
    while ( 2 )
    {
        if ( (UINT8)(v7 - 48) <= 9u )
        {
            v14 = v7 - 48;
        }
        else
        {
            if ( (UINT8)((v7 & 0xDF) - 65) > 0x19u )
                break;
            v14 = 32 * ((UINT8)(v7 - 65) < 0x1Au) + v7 - 87;
        }
        if ( v14 < (int)a3 )
        {
            v15 = -1;
            if ( v11 > v10 || v13 < 0 )
            {
            LABEL_34:
                v16 = v11;
            }
            else if ( v11 != v10 || (v16 = 0xFFFFFFFFFFFFFFFFuLL / a3 && 0xFFFFFFFFFFFFFFFFuLL % a3 >= v14) )
            {
                v11 = v14 + a3 * v11;
                v15 = 1;
                goto LABEL_34;
            }
            v7 = *++v12;
            v11 = v16;
            v13 = v15;
            continue;
        }
        break;
    }
    if ( a2 )
    {
        if ( !v13 )
            v12 = a1;
        *a2 = v12;
    }
    v17 = -(UINT64)v11;
    if ( v6 )
        v17 = v11;
    result = -1LL;
    if ( v13 >= 0 )
        return v17;
    return result;
}

EFI_STATUS sub_E68E(void)
{
    EFI_STATUS  Status;
    
    UINT64 v8 = 0;
    
    UINT64 v9 = 0xAAAAAAAAAAAAAAAAuLL;
    
    UINT64 v10 = 0xAAAAAAAAAAAAAAAAuLL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    
    UINTN  DataSizeArray[5];
    
    
    
    
    BOOLEAN hadBootercfg = FALSE;
    
    Status = GetVariable (
                          L"b",
                          &gAppleBootVariableGuid,
                          NULL,
                          DataSizeArray,
                          NULL
                          );
    if(Status != RETURN_BUFFER_TOO_SMALL){
        Status = GetVariable (
                              L"bootercfg",
                              &gAppleBootVariableGuid,
                              NULL,
                              DataSizeArray,
                              NULL
                              );
        hadBootercfg = TRUE;
    }
    
    if(Status == RETURN_BUFFER_TOO_SMALL){
        if(DataSizeArray[0] <= 0x200){
            char *buffer = sub_1D2B1(DataSizeArray[0] + 1);
            if(buffer){
                Status = GetVariable (
                                      L"bootercfg",
                                      &gAppleBootVariableGuid,
                                      NULL,
                                      DataSizeArray,
                                      buffer
                                      );
                buffer[DataSizeArray[0]] = 0;
                qword_AEE30 = buffer;
                qword_AEE38 = buffer;
                qword_AEE40 = DataSizeArray[0];
            }
            if(Status >= 0){
            }
        }
    }
    
    if(!hadBootercfg){
        EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
        Status = SetVariable (
                              L"b",
                              &gAppleBootVariableGuid,
                              0,
                              0,
                              NULL
                              );
        
        
    }
    
    if(qword_AEE30){
        for (UINT64 i = 0; i < log_config_list_count; i++) {
            LOG_CONFIG_INFO log_config_info = log_config_list[i];
            if(sub_1D5D4(qword_AEE30,log_config_info.name,&v9,(char**)&v10) >= 0){
                log_config_info.config2 = 0;
                if(v10){
                    v8 = sub_2834C((char*)v9, NULL, 16LL);
                }else{
                    v8 = 0;
                }
                log_config_info.config1 = v8;
                UINT64 result = *log_config_info.sign;
                if ( result == 1 )
                {
                    result = (UINT64)log_config_info.name;
                    if ( result > log_config_info.config2 )
                        result = log_config_info.config2;
                }
                else
                {
                    if (result)
                        break;
                    result = log_config_info.config2 & (UINT64)log_config_info.name;
                }
                *log_config_info.config4 = result;
            }
        }
    }
    return Status;
}


BOOLEAN sub_170CB(void){
    EFI_STATUS  Status;
    UINTN DataSizeArray[4];
    DataSizeArray[0] = 0xAAAAAAAAAAAAAAAAuLL;
    UINT32 v0 = dword_AD820;
    if(dword_AD820 == 0xFFFFFFFF){
        dword_AD820 = 1;
        DataSizeArray[0] = 0;
        
        EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
        
        
        
        Status = GetVariable (
                              L"boot-signature",
                              &gAppleBootVariableGuid,
                              NULL,
                              DataSizeArray,
                              NULL
                              );
        
        if(Status != RETURN_BUFFER_TOO_SMALL){
            DEBUG ((DEBUG_INFO,"AAPL: #[EB.H.IS|!] %r <- RT.GV %S %g\n", Status, L"boot-signature", &gAppleBootVariableGuid));
            dword_AD820 = 0;
        }
        v0 = 0;
        DataSizeArray[0] = 0;
        
        Status = GetVariable (
                              L"boot-image-key",
                              &gAppleBootVariableGuid,
                              NULL,
                              DataSizeArray,
                              NULL
                              );
        
        if(Status == RETURN_BUFFER_TOO_SMALL){
            v0 = dword_AD820;
        }else{
            DEBUG ((DEBUG_INFO,"AAPL: #[EB.H.IS|!] %r <- RT.GV %S %g\n", Status, L"boot-image-key", &gAppleBootVariableGuid));
            dword_AD820 = 0;
        }
    }
    DEBUG ((DEBUG_INFO,"AAPL: #[EB|H:IS] %d\n", v0));
    return dword_AD820 == 1;
}

void* sub_27E8D(void)
{
    return NULL;
}

void* sub_27F03(void)
{
    return NULL;
}

void* sub_27F1C(void)
{
    return NULL;
}

void* sub_27F2F(void)
{
    return NULL;
}

void* sub_27F41(void)
{
    return NULL;
}

void* sub_27F5E(void)
{
    return NULL;
}

void *off_ADA40[6] =
{
    (void*)sub_27E8D,
    (void*)sub_27F03,
    (void*)sub_27F1C,
    (void*)sub_27F2F,
    (void*)sub_27F41,
    (void*)sub_27F5E
};

void* sub_28004(void)
{
    return NULL;
}

void* sub_28047(void)
{
    return NULL;
}

void* sub_2804D(void)
{
    return NULL;
}

void* sub_2805D(void)
{
    return NULL;
}

void* sub_2806A(void)
{
    return NULL;
}

void* sub_28088(void)
{
    return NULL;
}

void *off_ADA70[6] =
{
    (void*)sub_28004,
    (void*)sub_28047,
    (void*)sub_2804D,
    (void*)sub_2805D,
    (void*)sub_2806A,
    (void*)sub_28088
};


void* sub_28096(void)
{
    return NULL;
}

void* sub_2809C(void)
{
    return NULL;
}

void* sub_280A2(void)
{
    return NULL;
}

void* sub_280C3(void)
{
    return NULL;
}

void* sub_280DD(void)
{
    return NULL;
}

void* sub_28116(void)
{
    return NULL;
}

void *off_ADAA0[6] =
{
    (void*)sub_28096,
    (void*)sub_2809C,
    (void*)sub_280A2,
    (void*)sub_280C3,
    (void*)sub_280DD,
    (void*)sub_28116
};


#define out8(port, v) ({ \
asm volatile ( \
"movb %1, %%al\n" \
"movw %0, %%dx\n" \
"out %%al, %%dx\n" \
: \
: "g" (port), "g" (v) \
:"%al","%dx" \
); \
})

#define in8(port, v) ({ \
asm volatile ( \
"movw %1, %%dx\n" \
"in  %%dx, %%al\n" \
"movb %%al, %0\n" \
: "=g"(v)  \
: "g" (port) \
:"%al","%dx" \
); \
})

#define write_read_addree(address, value) ({ \
unsigned char read_value; \
asm volatile ( \
"movb %[v], %%al\n" \
"movl %[addr], %%edi\n" \
"movb %%al, (%%edi)\n" \
"movb (%%edi), %%al\n" \
: "=a" (read_value) \
: [addr]"r" (address), [v]"g" (value) \
: "%rdi","memory" \
); \
read_value; \
})


// 定义字符串数组，每个元素对应一个字符串
const UINT16 *debug_tag_strings[] = {
    L"INIT",
    L"VERBOSE",
    L"EXIT",
    L"RESET:OK",
    L"RESET:FAIL",
    L"RESET:RECOVERY",
    L"REAN:START",
    L"REAN:END",
    L"DT",
    L"EXITBS:START",
    L"EXITBS:END",
    L"HANDOFF TO XNU",
    L"UNKNOWN"
};

EFI_STATUS sub_27D1F(void){
    
    
    EFI_STATUS Status = 0;
    UINT32 ptr2 = addr_FE03401C + 0x1FE4;
    UINT64 baseZeroAddress = 0x0;
    qword_B0360 = (UINT32*)(ptr2 + baseZeroAddress);
    UINT32 ptr3 = addr_FE03401C + 0x2000;
    UINT32 V_5A = 0x5A;
    UINT32 V_A5 = 0xA5;
    
    UINT32 V_5A_newValue = write_read_addree(ptr3, V_5A);
    
    if(V_5A_newValue == V_5A){
        UINT32 V_A5_newValue = write_read_addree(ptr3, V_A5);
        if(V_A5_newValue == V_A5){
            qword_B0358 = (UINT64*)off_ADA40;
            Status = 1;
            return Status;
        }
    }
    
    UINT32 ptr4 = (UINT32)addr_FE03401C - 0x1C;
    qword_B0360 = (UINT32*)(ptr4 + baseZeroAddress);
    V_5A_newValue = write_read_addree(ptr4, V_5A);
    if(V_5A_newValue == V_5A){
        UINT32 V_A5_newValue = write_read_addree(ptr4, V_A5);
        if(V_A5_newValue == V_A5){
            qword_B0358 = (UINT64*)off_ADA40;
            Status = 1;
            return Status;
        }
    }
    
    
    
    UINT16 port = 0x3FF;
    out8(port,V_5A);
    UINT32 in_v = 0;
    in8(port,in_v);
    
    
    if(in_v == V_5A){
        out8(port,V_A5);
        UINT32 in_v = 0;
        in8(port,in_v);
        if(in_v == V_A5){
            qword_B0358 = (UINT64*)off_ADA70;
            Status = 1;
            return Status;
        }
    }else{
        
        
        unsigned char read_value = 0;
        asm volatile ( \
                      "movb $1, %[addr1]\n" \
                      "movq $0xFE03401C, %%rcx\n" \
                      "addq $0x3DC014, %%rcx\n" \
                      "movb $0x5A, (%%rcx)\n" \
                      "cmpb $0, %[addr1]\n" \
                      "movl $0x30,(%%edx)\n" \
                      "cmovnz %%rcx, %%rdx\n" \
                      "movl $0x30,%%r8d\n" \
                      "movb (%%rdx),%%al\n" \
                      : "=a" (read_value) \
                      : [addr1]"m" (byte_B0370) \
                      : "%rcx","%rdx","%r8d","memory" \
                      );
        
        if(read_value == V_5A){
            unsigned char read_value = 0;
            asm volatile ( \
                          "movb $0xA5, (%%rdx)\n" \
                          "cmpb $0, %[addr1]\n" \
                          "cmovnz %%rcx, %%r8\n" \
                          "movb (%%r8),%%cl\n" \
                          "movb %%cl,%%al\n" \
                          : "=a" (read_value) \
                          : [addr1]"m" (byte_B0370) \
                          : "%rcx","%rdx","r8","memory" \
                          );
            if(read_value == V_A5){
                qword_B0358 = (UINT64*)off_ADAA0;
                Status = 1;
                return Status;
            }
        }
        
    }
    
    return Status;
}

void sub_22D1D(UINT32 a1){
    if(a1 >= 0xC){
        DEBUG ((DEBUG_INFO,"AAPL: #[EB|LOG:UNKNOWN] %d\n", a1));
        return;
    }
    EFI_TIME                LogTime;
    
    EFI_GET_TIME GetTime = mRuntimeServices->GetTime;
    if(a1 > 9 ||  GetTime (&LogTime, NULL) < 0) {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB|LOG:%s] _\n", debug_tag_strings[a1]));
        return;
    }
    const UINT16 * logtag = debug_tag_strings[a1];
    DEBUG ((DEBUG_INFO,"AAPL: #[EB|LOG:%S] %04d-%02d-%02d %02d:%02d:%02d\n", logtag, LogTime.Year, LogTime.Month, LogTime.Day, LogTime.Hour, LogTime.Minute, LogTime.Second));
}

EFI_STATUS sub_27E18(void)
{
    EFI_STATUS  Status = 0;
    if ( !qword_B0358 )
        return Status;
    (*(void (**)(void))qword_B0358)();
    Status = 1;
    return Status;
}

void* sub_27E3D(void)
{
    void *result = qword_B0358;
    if(qword_B0358){
        return (*(void* (**)(void))(qword_B0358 + 8))();
    }
    return result;
}

EFI_STATUS sub_1D327(void* buffer)
{
    EFI_STATUS  Status = 0;
    EFI_FREE_POOL FreePool = mBootServices->FreePool;
    UINT64* v3 = NULL;
    Status = FreePool(buffer);
    if(Status >= 0){
        v3 = (UINT64*)qword_B01E8;
        if(v3 == NULL){
            DEBUG ((DEBUG_INFO,"AAPL: #[EB.M.BMF|UK!]\n"));
            return Status;
        }
        if(*qword_B01E8 != buffer){
            while (TRUE) {
                v3 = (UINT64*)*(v3 + 24);
                if(v3 == NULL){
                    DEBUG ((DEBUG_INFO,"AAPL: #[EB.M.BMF|UK!]\n"));
                    return Status;
                }
                if(*v3 == (UINT64)buffer){
                    break;
                }
            }
        }
        v3 = NULL;
        UINT64* v6 = (UINT64*)&qword_B01E8;
        if(v3){
            v6 = v3 + 24;
        }
        *v6 = v3[3];
        v3[3] = (UINT64)qword_B01F8;
        qword_B01F8 = v3;
    }
    return Status;
}

const char * sub_12034(StringStruct2* a1)
{
    StringStruct1*fist_StringStruct1; // rbx
    const char *v2; // rsi
    
    fist_StringStruct1 = a1->next0;
    
    v2 = "(null)";
    if (fist_StringStruct1 )
    {
        //  while ( (UINT32)sub_28246(*v1, "name") ) strcmp
        while (strcmp(fist_StringStruct1->string1, "name") != 0)
        {
            fist_StringStruct1 = fist_StringStruct1->next3;
            if ( fist_StringStruct1 == NULL)
                return v2;
        }
        return fist_StringStruct1->next1;
    }
    return v2;
}
// memset
UINT64 sub_E5B3(void* buffer,UINT64 byteSize){
    UINT64 result = 0;
    memset(buffer, result, (UINT32)byteSize);
    return result;
}

UINT64 sub_E5B0(void* buffer,UINT64 size){
    return sub_E5B3(buffer,size);
}



void sub_240B0(
               void        *DestinationBuffer,
               const void  *SourceBuffer,
               size_t      Length
               ){
    memcpy(DestinationBuffer, SourceBuffer, Length);
}

StringStruct1* sub_11BA4(StringStruct2 *a1, char *a2, UINT64 a3, char* a4, char a5){
    StringStruct1* next0 = a1->next0;
    StringStruct1* buffer = NULL;
    StringStruct1* v11 = NULL;
    StringStruct1* v14 = NULL;
    StringStruct1* v16 = NULL;
    StringStruct1* v17 = NULL;
    StringStruct2* v18 = NULL;
    
    if(next0 == NULL){
    LABEL_5:
        if(qword_AEFA0){
            v11 = qword_AEFA0->next3;
        }
        else{
            buffer = sub_1D2B1(4096);
            if(buffer == NULL){
                return NULL;
            }
            sub_E5B0(buffer, 4096LL);
            buffer->next3 = qword_AEFA8;
            qword_AEFA8 = buffer;
            buffer->next1 = buffer;
            v14 = buffer;
            v14++;
            char count = 101;
            v16 = qword_AEFA0;
            while (count-- > 0) {
                v11 = v16;
                v14->next3 = v16;
                v16 = v14;
                v14++;
            }
            qword_AEFA0 = v16;
        }
        qword_AEFA0 = v11;
        v16->string1 = a2;
        v16->size = a3;
        if(a5 == 0){
            v16->next1 = a4;
            v16->v1 = 0;
            goto LABEL_15;
        }
        v17 = sub_1D2B1(a3);
        v16->next1 = v17;
        if ( v17 ){
            v16->v1 = 1;
            sub_240B0(v17,a4,a3);
        LABEL_15:
            if(next0){
                v18 = a1->next1;
                v18++;
            }else{
                v18 = a1;
            }
            v18->next0 = v16;
            v18->next1 = v16;
            v16->next3 = NULL;
            ++dword_AEFCC;
            dword_AEFD0 += (a3 + 3) & 0xFFFFFFFC;
            return v16;
        }
        return NULL;
    }
    v16 = NULL;
    
    while (strcmp(next0->string1, a2) != 0)
    {
        next0 = next0->next3;
        if ( next0 == NULL)
            goto LABEL_5;
    }
    return v16;
}

StringStruct2* sub_11D29(StringStruct2 *a1, char* a2)
{
    StringStruct2* buffer = NULL;
    void** v5 = NULL;
    StringStruct2* v6 = NULL;
    StringStruct2 *v9 = NULL;
    if(qword_AEFB0){
        v5 = &qword_AEFB0->next3;
        v6 = qword_AEFB0->next3;
    }else{
        buffer = sub_1D2B1(4096);
        if(buffer == NULL){
            return NULL;
        }
        sub_E5B0(buffer, 4096LL);
        buffer->next3 = qword_AEFB8;
        qword_AEFB8 = buffer;
        buffer->next1 = buffer;
        v9 = ++buffer;
        char count = 127;
        StringStruct2* v11 = qword_AEFB0;
        while (count-- > 0) {
            v6 = v11;
            v9->next3 = v11;
            v11 = v9;
            v9++;
        }
        v5 = &v9->next3;
        qword_AEFB0 = v9--;
    }
    qword_AEFB0 = v6;
    if(a1){
        *v5 = a1->next1;
        a1->next1 = v9;
    }else{
        qword_AEFC0 = v9;
        v9->next3 = NULL;
    }
    dword_AEFC8++;
    UINT64 strlen = sub_2822A(a2);
    sub_11BA4(v9, "name", strlen + 1, a2, 1);
    return v9;
}

StringStruct2* sub_1207F(char *a1, char a2)
{
    StringStruct2* v2 = 0;
    char *v5; // rax
    char *v6; // rdx
    char v7; // cl
    StringStruct2* i;
    const char* v11;
    char v13[32];
    memset(v13,0xAA,32);
    v2 = qword_AEFC0;
    if(qword_AEFC0 == 0){
        
    }
    while (TRUE) {
        v5 = a1 - 1;
        v6 = a1 + 1;
        do{
            a1 = v6;
            v7 = *++v5;
            ++v6;
        }while(v7 == '/');
        char v8 = 61;
        char* v13_ptr = v13;
        while (*a1 && *a1 != '/') {
            *v13_ptr++ = *a1;
            if(v8 == 0){
                break;
            }
            v7 = *a1;
            v8--;
            ++a1;
        }
        --a1;
        *v13_ptr = 0;
        if(v13[0] == 0){
            return v2;
        }
        
        for ( i = v2->next2; i; i = v2->next3 )
        {
            v11 = sub_12034(i);
            if(strcmp(v11, v13) == 0){
                goto LABEL_17;
            }
        }
        if(a2 == 0){
            return NULL;
        }
        i = sub_11D29(v2, v13);
    LABEL_17:
        v2 = i;
        if ( !i )
            return 0LL;
    }
    return i;
}

EFI_STATUS sub_22DC7(UINT32 a1){
    EFI_STATUS  Status = 0;
    
    UINT64 v11[10];
    
    APPLE_DEBUG_LOG_PROTOCOL  *Protocol = NULL;
    
    switch (a1) {
        case 0:
        {
            Status = mBootServices->LocateProtocol (
                                                    &gAppleDebugLogProtocolGuid,
                                                    NULL,
                                                    (VOID *)&Protocol
                                                    );
            
            if (Status < 0) {
                Protocol = NULL;
            }
            BOOLEAN v2 = sub_170CB();
            UINT64* v3 = &unk_AD378;
            if(!v2){
                v3 = &unk_AD370;
            }
            byte_B0342 = v2;
            qword_B0348 = *v3;
            if(qword_B0348 >= 4){
                if(Protocol){
                    byte_B0340 = 1;
                    if(v2){
                        APPLE_DEBUG_LOG_SETUP_FILES SetupFiles = Protocol->SetupFiles;
                        SetupFiles();
                    }
                }
                
            }
            if(qword_AD380 > 1 || (qword_AD380 == 1 && Protocol == NULL)){
                byte_B0333 = 1;
            }
            if(qword_AD388){
                sub_27D1F();
                if(qword_AD388 > 3 || (qword_AD388 == 1 && Protocol == NULL)){
                    byte_B0331 = 1;
                }
            }
            if(qword_AD390){
                byte_B0332 = 1;
            }
            goto LABEL_40;
        }
            break;
        case 1:{
            byte_B0333 = 1;
            goto LABEL_40;
        }
            break;
        case 7:{
            byte_B0341 = 1;
            goto LABEL_40;
        }
            break;
        case 9:{
            if(qword_AD388 >= 2){
                byte_B0331 = 1;
            }
            sub_22D1D(9);
        LABEL_29:
            if(qword_B0348 >=3 ){
            LABEL_30:
                if(Protocol){
                    if(byte_B0342){
                        APPLE_DEBUG_LOG_SETUP_FILES SetupFiles = Protocol->SetupFiles;
                        SetupFiles();
                    }
                    APPLE_DEBUG_LOG_WRITE_FILES WriteFiles = Protocol->WriteFiles;
                    WriteFiles();
                }
            }
        LABEL_34:
            Status = a1 - 2;
            if(Status >= 4){
                if(a1 == 6){
                LABEL_46:
                    byte_B0341 = 1;
                }else if(a1 == 9){
                    if(qword_B2078){
                        sub_1D327(qword_B2078);
                        qword_B2078 = NULL;
                    }
                    Status = 0LL;
                    byte_B0332 = 0;
                    byte_B0333 = 0;
                    Protocol = NULL;
                    qword_AD398 = 0LL;
                }
            }else{
            LABEL_35:
                if ( qword_B2078 )
                {
                    Status = sub_1D327(qword_B2078);
                    qword_B2078 = 0LL;
                }
                byte_B0332 = 0;
            }
        }
            break;
        case 0xAu:{
            if ( qword_AD388 )
            {
                sub_27E18();
                byte_B0331 = 1;
            }
        LABEL_40:
            sub_22D1D(a1);
        }
            break;
        default:
            sub_22D1D(a1);
            UINT32 newA1 = a1 - 2;
            switch (newA1) {
                case 2:
                case 4:
                    if(qword_B0348 == 0){
                        goto LABEL_34;
                    }
                    goto LABEL_30;
                    break;
                case 3:
                    goto LABEL_29;
                case 5:
                    if(qword_B0348 >= 2){
                        if(Protocol){
                            if(byte_B0342){
                                APPLE_DEBUG_LOG_SETUP_FILES SetupFiles = Protocol->SetupFiles;
                                SetupFiles();
                            }
                            APPLE_DEBUG_LOG_WRITE_FILES WriteFiles = Protocol->WriteFiles;
                            WriteFiles();
                        }
                    }
                    goto LABEL_35;
                    break;
                case 6:
                    goto LABEL_46;
                    break;
                case 8:{
                    if(qword_AD390 == 1){
                        v11[6] = 0xAAAAAAAA;
                        v11[5] = 0xAAAAAAAAAAAAAAAA;
                        UINT32 v4 = 1;
                        StringStruct2* result = sub_1207F("/efi/debug-log", v4);
                        if(result){
                            if(Protocol){
                                v11[6] = 0;
                                v11[5] = 0;
                                APPLE_DEBUG_LOG_EXTRACT_BUFFER ExtractBuffer = Protocol->ExtractBuffer;
                                Status = ExtractBuffer((UINT32*)&v11[6] + 4,&v11[5],NULL,NULL);
                                if(Status >= 0){
                                    void *buffer = sub_1D2B1(v11[5]);
                                    if(buffer){
                                        v11[6] = 0;
                                        Status = ExtractBuffer((UINT32*)&v11[6] + 4,&v11[5],buffer,NULL);
                                        sub_11BA4(result, "efiboot", v11[5], buffer, 1);
                                        Status = sub_1D327(buffer);
                                    }
                                }
                            }else if (qword_B2078 < qword_B2080){
                                sub_11BA4(result, "efiboot", qword_B2080 - qword_B2078,
                                          qword_B2078,
                                          1);
                            }
                        }
                    }
                }
                    break;
                case 0xBu:
                {
                    if ( (qword_AD3A0 & 1) != 0 )
                    {
                        DEBUG ((DEBUG_INFO,"AAPL: ======== End of efiboot serial output. ========\r\n"));
                        
                    }
                    byte_B0331 = 0;
                    Status = (EFI_STATUS)sub_27E3D();
                }
                    break;
                default:
                    break;
            }
            break;
            
    }
    return Status;
}


EFI_STATUS sub_E9AB(void)
{
    EFI_STATUS Status;
    UINT64 v4; // rbx
    UINT64 v6; // rax
    UINT64 v8; // [rsp+28h] [rbp-28h] BYREF
    UINT8 v9[25]; // [rsp+37h] [rbp-19h] BYREF
    
    v9[0] = -86;
    v4 = qword_AD3B8;
    v8 = 1LL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    
    
    Status = GetVariable (
                          L"booter-strict-xmlparser",
                          &gAppleBootVariableGuid,
                          NULL,
                          &v8,
                          v9
                          );
    
    
    if ( Status < 0 )
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB.CFG.DEV|!] %r <- RT.GV %S %g\n", Status, L"booter-strict-xmlparser", &gAppleBootVariableGuid));
        v6 = qword_AD3B8;
    }
    else
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB.CFG.DEV|VAR] %S %g %d\n", L"booter-strict-xmlparser", &gAppleBootVariableGuid, v9[0]));
        if ( v9[0] )
            v6 = qword_AD3B8 | 2;
        else
            v6 = qword_AD3B8 & 0xFFFFFFFFFFFFFFFDuLL;
        qword_AD3B8 = v6;
    }
    DEBUG ((DEBUG_INFO,"AAPL: #[EB|CFG:DEV] r%d 0x%X 0x%X\n", 5LL, v4, v6));
    return Status;
}


EFI_STATUS sub_5D8E(void)
{
    EFI_STATUS Status;
    
    if ( sub_170CB() )
    {
        byte_AE978 = 1;
        Status = 2;
    }
    else
    {
        Status = 2 * byte_AE978;
    }
    DEBUG ((DEBUG_INFO,"AAPL: #[EB|WL:MODE] %d\n", Status));
    return Status;
}


EFI_STATUS
ConvertToUnicodeText (
                      OUT EFI_STRING  StringDest,
                      IN  CHAR8       *StringSrc,
                      IN  OUT UINTN   *BufferSize
                      )
{
    UINTN  StringSize;
    UINTN  Index;
    
    ASSERT (StringSrc != NULL && BufferSize != NULL);
    
    StringSize = AsciiStrSize (StringSrc) * 2;
    if ((*BufferSize < StringSize) || (StringDest == NULL)) {
        *BufferSize = StringSize;
        return EFI_BUFFER_TOO_SMALL;
    }
    
    for (Index = 0; Index < AsciiStrLen (StringSrc); Index++) {
        StringDest[Index] = (CHAR16)StringSrc[Index];
    }
    
    StringDest[Index] = 0;
    return EFI_SUCCESS;
}

void sub_E869(void)
{
    if ( qword_AEE38 )
        DEBUG ((DEBUG_INFO,"AAPL: #[EB|CFG:VAR] %S <\"%.*e\">\n", qword_AEE38));
    if ( qword_AEE30 )
    {
        sub_1D327(qword_AEE30);
        qword_AEE30 = 0LL;
        qword_AEE40 = 0LL;
    }
    
    UINTN StringBufferSize = sizeof (CHAR16) * 20;
    EFI_STRING  StringDest = AllocateZeroPool (StringBufferSize);
    for(UINT32 i = 0; i < log_config_list_count;i++){
        LOG_CONFIG_INFO log_config =  log_config_list[i];
        if(log_config.config3 != 2 || log_config.config2 == 0){
            char sign = '&';
            if(log_config.config3 == 1){
                sign = '<';
            }
            CHAR16* type = L"set";
            if(log_config.config3 == 2){
                type = L"obsolete";
            }
            if(log_config.config2 != 0){
                type = L"default";
            }
            ConvertToUnicodeText(StringDest,log_config.name,&StringBufferSize);
            
            DEBUG ((DEBUG_INFO,"AAPL: #[EB|CFG:ARG] %S 0x%016X (0x%016X %c 0x%016X) %S\n", StringDest,log_config.config1,log_config.config1,sign,log_config.config4,type));
        }
    }
    FreePool (StringDest);
}

UINT64 sub_46911(UINT64 a1, char a2)
{
    return a1 >> a2;
}

EFI_STATUS sub_1D3BB(EFI_ALLOCATE_TYPE a1, EFI_MEMORY_TYPE a2, UINT64 a3, UINT64 *a4)
{
    EFI_STATUS Status; // rdi
    UINT64 v7; // rbx
    UINT64 *v8; // rax
    EFI_ALLOCATE_PAGES AllocatePages = mBootServices->AllocatePages;
    Status = AllocatePages(a1,a2,a3,a4);
    if ( Status >= 0 )
    {
        v7 = *a4;
        v8 = sub_7CE38528();
        *v8 = 0LL;
        v8[2] = v7;
        v8[1] = a3;
        v8[3] = (UINT64)qword_B01E8;
        qword_B01E8 = (UINT64**)v8;
    }
    return Status;
}

UINT64 sub_4691F(UINT64 a1,UINT64 a2,UINT64 *a3)
{
    if ( a3 )
        *a3 = a1 % a2;
    return a1 / a2;
}

UINT64 sub_1D413(EFI_PHYSICAL_ADDRESS a1, UINT64 a2)
{
    EFI_STATUS Status; // rdi
    UINT64 v4 = 0; // rsi
    UINT64* v5; // rax
    UINT64 v6; // rdx
    UINT64 v7; // rcx
    UINT64 *v8; // rbx
    
    EFI_FREE_PAGES FreePages = mBootServices->FreePages;
    Status = FreePages(a1,a2);
    if ( Status < 0 )
        return Status;
    v5 = (UINT64*)qword_B01E8;
    if ( !qword_B01E8 )
    {
    LABEL_11:
        DEBUG ((DEBUG_INFO,"AAPL: #[EB.M.BFP|UK!] %d\n", a2));
        return v4;
    }
    if ( *(UINT64 *)(qword_B01E8 + 16) != a1 )
    {
        v7 = (UINT64)qword_B01E8;
        while ( 1 )
        {
            v5 = (UINT64*)(*(UINT64**)(v7 + 24));
            if ( !v5 )
                goto LABEL_11;
            v6 = v7;
            v7 = *(UINT64 *)(v7 + 24);
            if ( *(UINT64 *)(v5 + 16) == a1 )
                goto LABEL_8;
        }
    }
    v6 = 0LL;
LABEL_8:
    v8 = (UINT64*)&qword_B01E8;
    if ( v6 )
        v8 = (UINT64 *)(v6 + 24);
    *v8 = *(UINT64 *)(v5 + 24);
    *(UINT64 *)(v5 + 24) = (UINT64)qword_B01F8;
    qword_B01F8 = v5;
    return v4;
}

UINT64 sub_123FA(void)
{
    EFI_PHYSICAL_ADDRESS v13; // rsi
    UINT64 v14; // rcx
    UINT64 result; // rax
    UINT64 v16; // eax
    
    v13 = qword_AEFE8;
    v14 = qword_AEFF0;
    result = 0LL;
    qword_AEFE8 = 0LL;
    qword_AEFF0 = 0LL;
    qword_AEFF8 = 0LL;
    if ( v13 )
    {
        if ( v14 )
        {
            v16 = sub_46911(v14, 12);
            return sub_1D413(v13, v16);
        }
    }
    return result;
}

UINT64 sub_1217B(UINT64 a1, UINT64 a2)
{
    char v2; // r15
    UINT64 v4; // rdi
    EFI_ALLOCATE_TYPE v5; // r14d
    char v6 = 0; // si
    EFI_STATUS Status;
    UINT64 v8; // rdi
    UINT64 v9; // rax
    UINT64 v10 = 0; // r8
    EFI_CPU_ARCH_PROTOCOL *v11; // rcx
    EFI_PHYSICAL_ADDRESS v12; // rax
    UINT64 v13; // rdx
    EFI_PHYSICAL_ADDRESS v14; // rcx
    UINT64 v15[2]; // [rsp+30h] [rbp-50h] BYREF
    UINT64 v16; // [rsp+40h] [rbp-40h] BYREF
    UINT64 v17[7]; // [rsp+48h] [rbp-38h] BYREF
    
    v15[1] = 0x829F5C9941FE80A8uLL;
    v15[0] = 0x4BBBAB2A7C436110LL;
    v16 = 0LL;
    v17[0] = 0LL;
    v2 = byte_AEFE0;
    if ( a1 )
    {
        v4 = a1;
        v5 = 2;
        v6 = 1;
    }
    else
    {
        v4 = 0xFFFFFFFFLL;
        a2 = 4096LL;
        v5 = 1;
        if ( sub_170CB() )
        {
            v6 = 0;
        }
        else
        {
            EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
            
            
            Status = GetVariable (
                                  L"efiboot-perf-record",
                                  &gAppleBootVariableGuid,
                                  NULL,
                                  &v16,
                                  NULL
                                  );
            
            if(Status != RETURN_BUFFER_TOO_SMALL){
                return Status;
            }
            
            EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
            
            
            Status = SetVariable (
                                  L"efiboot-perf-record",
                                  &gAppleBootVariableGuid,
                                  7,
                                  0,
                                  NULL
                                  );
            
        }
    }
    v17[0] = v4;
    byte_AEFE0 = v6;
    v8 = (UINT32)sub_46911(a2, 12LL);
    v9 = sub_1D3BB(v5, 9LL, v8, v17);
    if ( v9 < 0 )
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB.DBG.IDTP|!] %r <- EB.M.BAP %qd\n", v9, v8));
        Status = 0LL;
        qword_AEFE8 = 0LL;
        qword_AEFF0 = 0LL;
        return Status;
    }
    
    if ( !qword_AEFE8 || !qword_AEFF8 || (v10 = *(UINT32 *)(qword_AEFF8 + 12) && a2 < v10))
    {
        qword_AEFE8 = (EFI_PHYSICAL_ADDRESS)v17;
        qword_AEFF0 = a2;
        v11 = mCpu;
        if ( mCpu )
        {
            EFI_CPU_GET_TIMER_VALUE GetTimerValue = v11->GetTimerValue;
            
        LABEL_12:
            Status = GetTimerValue(v11,0LL,&qword_AF000,&qword_AF008);
            if ( Status < 0 )
                return Status;
            qword_AF008 = sub_4691F(1000000000000LL, qword_AF008, 0LL);
            goto LABEL_22;
        }
        
        EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
        
        Status = LocateProtocol (&gEfiCpuArchProtocolGuid,
                                 NULL,
                                 (VOID *)&mCpu
                                 );
        
        if ( Status < 0 )
        {
            mCpu = 0LL;
        }
        else
        {
            v11 = mCpu;
            if ( mCpu )
                goto LABEL_12;
        }
    LABEL_22:
        v12 = qword_AEFE8;
        qword_AEFF8 = qword_AEFE8;
        *(UINT64 *)qword_AEFE8 = 0x6F6F746C65666962LL;
        *(UINT64 *)(v12 + 8) = 0LL;
        v13 = qword_AF008;
        *(UINT64 *)(v12 + 16) = qword_AF008;
        Status = sub_4691F(qword_AF000, v13, 0LL);
        v14 = qword_AEFF8;
        *(UINT64 *)(qword_AEFF8 + 24) = Status;
        *(UINT32 *)(v14 + 12) += 64;
        return Status;
    }
    sub_240B0(&v17, (const void *)qword_AEFE8, v10);
    if ( (v2 & 1) == 0 )
        sub_123FA();
    Status = v17[0];
    qword_AEFE8 = (EFI_PHYSICAL_ADDRESS)v17;
    qword_AEFF0 = a2;
    qword_AEFF8 = (EFI_PHYSICAL_ADDRESS)v17;
    return Status;
}



UINT64 sub_281BA(UINT64 a1, UINT32 a2, char *a3, UINT8 ***a4)
{
    UINT64 v4; // esi
    UINT64 v6[4]; // [rsp+20h] [rbp-20h] BYREF
    
    v4 = a1;
    v6[0] = a1;
    v6[1] = a1 + a2 - 1;
    
#if 0
    sub_271BC(a3, (void ( *)(UINT64, UINT64))sub_7CE0E194, (UINT64)v6, a4);
    *(char *)v6[0] = 0;
#endif
    return (UINT32)(v6[0] - v4);
}

UINT64 sub_124A8(UINT32 a1, UINT64 a2, UINT64 a3)
{
    UINT64 v6 = 0; // rsi
    UINT64 v7; // r15
    UINT32 v8; // eax
    UINT32 v9; // edx
    UINT32 v10; // edx
    UINT64 v11; // rax
    UINT64 v12; // rcx
    UINT64 v13; // rdi
    UINT64 v14; // rbx
    UINT64 result = 0; // rax
    UINT64 v16; // r12
    UINT64 v17; // rax
    UINT64 v18; // r12
    UINT32 v19; // eax
    UINT64 v20; // rdx
    UINT64 v21; // rcx
    UINT64 v22[8]; // [rsp+20h] [rbp-40h] BYREF
    
    asm volatile ( \
                  "lfence\n" \
                  "rdtsc\n" \
                  "lfence\n" \
                  : \
                  : \
                  : \
                  ); \
    if ( qword_AF008 )
        v7 = sub_4691F(v6, qword_AF008, 0LL);
    else
        v7 = -1LL;
#if 0
    v8 = sub_281BA(byte_7CE95010, 256LL, a2, a3);
    if ( byte_7CE95010[v8 - 1] == 10 )
        byte_7CE95010[--v8] = 0;
    v9 = v8 + 15;
    if ( v8 + 8 >= 0 )
        v9 = v8 + 8;
    v10 = v8 + (v9 & 0xFFFFFFF8);
    v11 = qword_7CE94FF8;
    v12 = *(UINT32 *)(qword_7CE94FF8 + 12);
    v13 = v10;
    v14 = qword_7CE94FF0;
    if ( v12 + v10 + 24 <= (UINT64)qword_7CE94FF0 )
    {
        v20 = qword_7CE94FE8;
    LABEL_14:
        *(UINT64 *)(v20 + v12) = a1;
        *(UINT64 *)(v20 + v12 + 4) = v13;
        *(UINT64 *)(v20 + v12 + 8) = v7;
        *(UINT64 *)(v20 + v12 + 16) = v6;
        v21 = (UINT32)(*(UINT64 *)(v11 + 12) + 24);
        *(UINT64 *)(v11 + 12) = v21;
        sub_7CE0A0B0(v20 + v21, byte_7CE95010, v13);
        result = qword_7CE94FF8;
        *(UINT64 *)(qword_7CE94FF8 + 12) += v13;
        ++*(UINT64 *)(result + 8);
        return result;
    }
    result = 0xAAAAAAAAAAAAAAAAuLL;
    v22[0] = 0xAAAAAAAAAAAAAAAAuLL;
    if ( !byte_7CE94FE0 && qword_7CE98098 )
    {
        v22[0] = 0xFFFFFFFFLL;
        qword_7CE94FF0 = sub_7CE2C8F6(qword_7CE94FF0, 1LL);
        v16 = (UINT32)sub_7CE2C911(qword_7CE94FF0, 12);
        v17 = sub_7CE033BB(1LL, 9LL, v16, v22);
        if ( v17 < 0 )
            return sub_7CE08C97(1LL, "#[EB.DBG.TTPI|!] %r <- EB.M.BAP %qd\n", v17, v16);
        sub_7CE0A0B0(v22[0], qword_7CE94FE8, *(UINT32 *)(qword_7CE94FF8 + 12));
        v18 = qword_7CE94FE8;
        v19 = sub_7CE2C911(v14, 12);
        sub_1D413(v18, v19);
        v11 = v22[0];
        qword_7CE94FE8 = v22[0];
        qword_7CE94FF8 = v22[0];
        v12 = *(UINT32 *)(v22[0] + 12);
        v20 = v22[0];
        goto LABEL_14;
    }
#endif
    return result;
}

EFI_STATUS sub_12453(char* fmt, ...)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    UINT64 v2[3]; // [rsp+20h] [rbp-20h] BYREF
    va_list va; // [rsp+58h] [rbp+18h] BYREF
    
    va_start(va, fmt);
    memset(v2, 170, sizeof(v2));
    if ( qword_AEFE8 )
    {
#if 0
        va_copy((va_list)v2, va);
        return sub_124A8(1LL, fmt,v2);
#endif
    }
    return Status;
}



EFI_STATUS sub_16E01(void)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    UINT64 v1; // [rsp+28h] [rbp-28h] BYREF
    UINT64 v2; // [rsp+32h] [rbp-1Eh] BYREF
    UINT64 v3; // [rsp+34h] [rbp-1Ch] BYREF
    char v4[25]; // [rsp+37h] [rbp-19h] BYREF
    
    v2 = 0;
    v3 = 0;
    v4[0] = 0;
    v1 = 1LL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    
    STATIC UINT8 mUIScale;
    
    STATIC UINT16 mActualDensity;
    STATIC UINT16 mDensityThreshold;
    
    
    UINTN UiScaleSize = sizeof (mUIScale);
    
    Status = GetVariable (
                          APPLE_UI_SCALE_VARIABLE_NAME,
                          &gAppleVendorVariableGuid,
                          NULL,
                          &UiScaleSize,
                          (VOID *)&mUIScale
                          );
    
    if ( Status >= 0 && mUIScale )
    {
        UINTN mActualDensitySize = sizeof (mActualDensity);
        
        
        Status = GetVariable (
                              APPLE_UI_SCALE_VARIABLE_NAME,
                              &gAppleVendorVariableGuid,
                              NULL,
                              &UiScaleSize,
                              (VOID *)&mUIScale
                              );
        
        Status         = GetVariable(
                                     L"ActualDensity",
                                     &gAppleBootVariableGuid,
                                     NULL,
                                     &mActualDensitySize,
                                     (VOID *)&mActualDensity
                                     );
        UINTN mDensityThresholdSize = sizeof (mDensityThreshold);
        Status         = GetVariable(
                                     L"DensityThreshold",
                                     &gAppleBootVariableGuid,
                                     NULL,
                                     &mDensityThresholdSize,
                                     (VOID *)&mActualDensity
                                     );
        
        if ( mUIScale >= 2u )
            byte_B1F30 = 1;
    }
    return Status;
}

#if 0
void sub_7CE1EDE6()
{
    EFI_STATUS Status;
    
    UINT64 v0; // rax
    UINT64 v1; // rax
    
    if ( !qword_7CEAE210 && (byte_7CEAE320 & 1) == 0 )
    {
        EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
        Status = LocateProtocol(&gAppleRtcRamProtocolGuid,0,qword_7CEAE210);
        v0 = (*(UINT64 ( **)(void *, UINT64, UINT64 *))(qword_7CEB0098 + 320))(
                                                                               &unk_7CEABBA0,
                                                                               0LL,
                                                                               &qword_7CEAE210);
        if ( v0 < 0 )
        {
            DEBUG ((DEBUG_INFO,"#[EB.RTC.CP|!] %r <- BS.LocP %g\n", v0, &unk_7CEABBA0));
            byte_7CEAE320 = 1;
        }
        else
        {
            v1 = (*(UINT64 ( **)(UINT64, UINT64, UINT64 ( *)(), UINT64, UINT64 *))(qword_7CEB0098 + 80))(
                                                                                                         513LL,
                                                                                                         8LL,
                                                                                                         sub_7CE1F067,
                                                                                                         0LL,
                                                                                                         &qword_7CEB0060);
            if ( v1 < 0 )
                DEBUG ((DEBUG_INFO,"#[EB.RTC.CP|!] %r <- BS.CrE 0x%08X\n", v1, 513LL));
        }
    }
}

#endif

#if 0
UINT64 sub_20EDF(UINT64 a1, UINT8 a2, UINT8 a3)
{
    UINT64 result; // rax
    UINT64 v4; // rdi
    UINT64 v5; // r14
    UINT32 v6; // ebx
    UINT64 v7; // rsi
    char v8; // al
    UINT8 v9; // cl
    UINT32 v10; // esi
    UINT32 v11; // edi
    UINT32 v12; // ecx
    
    result = 0x8000000000000002uLL;
    if ( a1 )
    {
        if ( a3 >= 0xEu )
        {
            v4 = a3;
            v5 = a2;
            v6 = a3 + a2;
            if ( v6 <= 0xFF )
            {
                if ( a2 )
                {
                    v7 = a1;
                    sub_7CE1EDE6();
                    if ( qword_7CEAE210 )
                        return (*(UINT64 ( **)(UINT64, UINT64, UINT64, UINT64))(qword_7CEAE210 + 16))(
                                                                                                      qword_7CEAE210,
                                                                                                      v7,
                                                                                                      v5,
                                                                                                      v4);
                    sub_7CE1EE9E();
                    v8 = 1;
                    while ( 2 )
                    {
                        ++v7;
                        while ( 1 )
                        {
                            v9 = *(char *)(v7 - 1);
                            if ( v9 != byte_7CEAE220[v4] )
                                break;
                            ++v4;
                            ++v7;
                            if ( v4 >= v6 )
                            {
                                if ( (v8 & 1) != 0 )
                                    return 0LL;
                                goto LABEL_18;
                            }
                        }
                        if ( (v4 & 0x80u) != 0LL )
                        {
                            __outbyte(0x72u, v4);
                            __outbyte(0x73u, v9);
                        }
                        else
                        {
                            __outbyte(0x70u, v4);
                            __outbyte(0x71u, v9);
                        }
                        byte_7CEAE220[v4++] = *(char *)(v7 - 1);
                        v8 = 0;
                        if ( v4 < v6 )
                            continue;
                        break;
                    }
                LABEL_18:
                    if ( (UINT16)word_7CEAE25E != (UINT16)sub_7CE0C53F(0LL, &unk_7CEAE22E, 48LL) )
                    {
                        __outbyte(0x70u, 0x3Eu);
                        __outbyte(0x71u, word_7CEAE25E);
                        __outbyte(0x70u, 0x3Fu);
                        __outbyte(0x71u, HIBYTE(word_7CEAE25E));
                    }
                    v10 = (UINT8)word_7CEAE278;
                    v11 = HIBYTE(word_7CEAE278);
                    word_7CEAE278 = 0;
                    v12 = sub_7CE0C53F(0LL, &unk_7CEAE22E, 242LL);
                    if ( (v11 | (v10 << 8)) != (UINT16)v12 )
                    {
                        v10 = v12 >> 8;
                        __outbyte(0x70u, 0x58u);
                        __outbyte(0x71u, BYTE1(v12));
                        __outbyte(0x70u, 0x59u);
                        __outbyte(0x71u, v12);
                        LOBYTE(v11) = v12;
                    }
                    LOBYTE(word_7CEAE278) = v10;
                    HIBYTE(word_7CEAE278) = v11;
                }
                return 0LL;
            }
        }
    }
    return result;
}
#endif

UINT64 sub_5E29(char a1, UINT64 a2, UINT32 a3)
{
    UINT8 v3; // cl
    UINT64 v4; // rsi
    EFI_STATUS Status = 0; // rax
    UINT32 v6; // edx
    UINT32 v7; // r8d
    UINT32 v8; // r9d
    UINT32 v9; // [rsp+10Ch] [rbp-30h]
    UINT32 v10; // [rsp+114h] [rbp-28h]
    UINT32 v11; // [rsp+11Ch] [rbp-20h]
    UINT32 v12; // [rsp+124h] [rbp-18h]
    
    v3 = byte_AD1D9 | a1;
    byte_AD1D9 = v3;
    if ( a2 )
        byte_AD1DB = a2;
    if ( a3 )
        byte_AD1DA = a3;
    else
        a3 = byte_AD1DA;
    v4 = (UINT8)byte_AE978;
    DEBUG ((DEBUG_INFO,"AAPL: #[EB|WL] %d %d 0x%02X 0x%02X % 3d 0x%02X\n",
            (UINT8)byte_AE978,
            2 * (UINT32)(UINT8)byte_AE978,
            (UINT8)byte_AD1D8,
            v3,
            a3,
            byte_AD1DB));
    if ( v4 == 1 )
    {
#if 0
        v6 |= 4;
        v7 |= -80;
        EFI_STATUS = sub_20EDF((UINT32)&byte_AD1D8, v6, v7, v8, v9, v10, v11, v12);
        if ( result < 0 )
            return DEBUG ((DEBUG_INFO,"#[EB.WL.WL|!] %r <- EB.WL.WLF\n", result));
#endif
    }
    return Status;
}


EFI_STATUS sub_16C17(void)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    EFI_CONSOLE_CONTROL_SCREEN_MODE v1; // [rsp+2Ch] [rbp-4h] BYREF
    
    v1 = -1431655766;
    /*
     * frame #0: 0x000000007d69c5e0 OpenCore.dll`VirtualFsLocateProtocol(Protocol=0xaaaaaaaa00000000, Registration=0xaaaaaaaa00000000, Interface=0xaaaaaaaaaaaaaaaa) at VirtualFs.c:84
     
     */
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    Status = LocateProtocol(&gEfiConsoleControlProtocolGuid, 0LL, (void **)&qword_B1F40);
    if ( Status >= 0 )
    {
        /*
         
         * frame #0: 0x000000007d66da10 OpenCore.dll`ConsoleControlGetMode(This=0xaaaaaaaa00000000, Mode=0xaaaaaaaa00000000, GopUgaExists="", StdInLocked="") at TextOutputBuiltin.c:999
         
         */
        
        EFI_CONSOLE_CONTROL_PROTOCOL_GET_MODE GetMode = qword_B1F40->GetMode;
        Status = GetMode(qword_B1F40, &v1, 0LL, 0LL);
        if ( Status >= 0 )
        {
            dword_AF158 = v1;
            dword_AF15C = v1;
            return RETURN_SUCCESS;
        }
    }
    return Status;
}

EFI_STATUS sub_E580(CHAR16 *a1, UINT64 a2, UINT64 a3)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    sub_E5B3(a1, a3);
    return Status;
}

EFI_STATUS sub_228A3(char *a1, UINT64 a2)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    UINT8 ***v2; // r14
    CHAR16 v5[149]; // [rsp+20h] [rbp-D0h] BYREF
    char v6; // [rsp+B5h] [rbp-3Bh] BYREF
    char* v7[7]; // [rsp+B8h] [rbp-38h] BYREF
    
    v2 = (UINT8 ***)a2;
    a2 |= -86;
    sub_E580(v5, a2, 149LL);
    v7[0] = (char*)v5;
    v7[1] = (char*)v5;
    v7[2] = &v6;
#if 0
    sub_271BC(a1, (void ( *)(UINT64, UINT64))sub_7CE209F1, (UINT64)v7, v2);
    return sub_22958(v7);
#endif
    return Status;
}

EFI_STATUS sub_E617(char *a1, ...)
{
    EFI_STATUS Status = RETURN_SUCCESS;
#if 0
    UINT64 v2[4]; // [rsp+20h] [rbp-20h] BYREF
    va_list va; // [rsp+58h] [rbp+18h] BYREF
    
    va_start(va, a1);
    memset(v2, 170, 24);
    if ( (qword_AD3A0 & 1) != 0 )
    {
        va_copy(v2, va);
        sub_228A3(a1, (UINT64)v2);
    }
    sub_5E29(2, 0LL, 0);
    DEBUG ((DEBUG_INFO,"AAPL: #[EB|STOP]\n"));
    
    return sub_9707(0x8000000000000015uLL);
#endif
    return Status;
}


EFI_STATUS sub_15501(UINT32 a1)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    UINT32 v1; // esi
    
    v1 = a1;
    if ( a1 != 1 )
    {
        if ( a1 != 2 )
            return 0x8000000000000002uLL;
        v1 = 0;
    }
    if ( v1 != dword_AF158 )
    {
        EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE SetMode = qword_B1F40->SetMode;
        Status = SetMode(qword_B1F40,a1);
        if ( Status < 0 )
            return Status;
        dword_AF158 = v1;
    }
    return 0LL;
}

void sub_8FAD(void)
{
    UINT32 var_18, var_14, var_10, var_C;
    
    var_18 = 0xAAAAAAAA;
    var_14 = 0xAAAAAAAA;
    var_10 = 0xAAAAAAAA;
    var_C = 0xAAAAAAAA;
    
    UINT32 eax = 1;
    UINT32 ebx = 0, ecx = 0, edx = 0;
    
    asm volatile (
                  "cpuid\n\t"
                  : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                  : "a" (eax)
                  );
    
    var_18 = eax;
    var_14 = ebx;
    var_10 = ecx;
    var_C = edx;
    
    if (ecx & (1 << 9)) {
        // Set a specific bit in memory location cs:byte_B1DC9
        byte_B1DC9 |= 1;
    }
    
    if (ecx & (1 << 25)) {
        // Set another specific bit in memory location cs:byte_B1DC9
        byte_B1DC9 |= 0x10;
    }
}

EFI_STATUS sub_BA0F(void)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    
    UINT64 v2; // rdi
    UINT64 v3; // rsi
    UINT64 v4; // rdi
    char *v5; // rcx
    UINT64 v6; // rax
    UINT64 v7; // rbx
    UINT64 v8; // rdx
    char *v9; // rcx
    UINT64 v10; // rax
    UINT64 v11[5]; // [rsp+28h] [rbp-28h] BYREF
    
    v11[0] = 64LL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    Status = GetVariable(
                         L"H",
                         &gAppleVendorVariableGuid,
                         0LL,
                         v11,
                         &unk_AE980);
    if ( Status >= 0 )
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB|BRD:NV] %e\n", &unk_AE980));
        return Status;
    }
#if 0
    Status = RETURN_NOT_FOUND;
    if ( *(UINT64 *)(qword_B2090 + 104) )
    {
        v2 = *(UINT64 *)(qword_B2090 + 112);
        v3 = 0LL;
        while ( !sub_46304(v2, (UINT64)&unk_ADCC0) )
        {
            ++v3;
            v2 += 24LL;
            if ( *(UINT64 *)(qword_B2090 + 104) <= v3 )
                return v0;
        }
        v4 = *(UINT64 *)(v2 + 16);
        if ( !sub_4632A(v4, "_SM_", 4LL) )
        {
            v5 = (char *)*(UINT32 *)(v4 + 24);
            v6 = (UINT32)v5 + *(UINT16 *)(v4 + 22);
            if ( (UINT64)v5 < v6 && (UINT64)(v5 + 4) <= v6 )
            {
                v7 = *(UINT32 *)(v4 + 24);
                while ( 1 )
                {
                    if ( *v5 == 2 )
                    {
                        if ( (UINT64)(v5 + 17) > v6 )
                            return v0;
                        if ( *(char *)(v7 + 5) )
                            break;
                    }
                    v8 = *(UINT8 *)(v7 + 1);
                    v7 = (UINT64)&v5[v8];
                    if ( (UINT64)&v5[v8] < v6 )
                    {
                        v9 = &v5[v8 + 1];
                        while ( 1 )
                        {
                            v7 = (UINT64)v9;
                            if ( !*(v9 - 1) && (UINT64)v9 < v6 && !*v9 )
                                break;
                            ++v9;
                            if ( v7 >= v6 )
                                goto LABEL_23;
                        }
                        v7 = (UINT64)(v9 + 1);
                    }
                LABEL_23:
                    if ( v7 < v6 )
                    {
                        v5 = (char *)v7;
                        if ( v7 + 4 <= v6 )
                            continue;
                    }
                    return v0;
                }
                v10 = sub_BBB2(&v5[*(UINT8 *)(v7 + 1)]);
                sub_282BC(&unk_AE980, v10, 64LL);
                byte_AE9BF = 0;
                DEBUG ((DEBUG_INFO,"#[EB|BRD:SMBIOS] %e\n", &unk_AE980));
                return 0LL;
            }
        }
    }
#endif
    return Status;
}

const char *sub_E237(void)
{
    const char *result; // rax
    
    long rax = 1;
    UINT32 rcx;
    
    asm volatile(
                 "movq %1, %%rax\n\t"   // Move the value 1 to RAX register
                 "cpuid\n\t"            // CPUID instruction
                 : "=c" (rcx)           // Output constraUINT32 for RCX
                 : "r" (rax)            // Input constraUINT32 for RAX
                 : "rax", "rbx", "rdx"  // List of clobbered registers
                 );
    
    result = "VMM";
    if (rcx >= 0) {
        return (const char *)&unk_AE980;
    } else {
        return result;
    }
}

UINT64 sub_12759(void)
{
    UINT64 result; // rax
    UINT32 v9; // esi
    UINT64 v10; // rax
    UINT64 v12; // rax
    UINT64 v13; // rax
    
    
    UINT32 eax = 0;
    UINT32 ebx = 0, ecx = 0, edx = 0;
    
    result = 0xAAAAAAAA;
    if ( qword_AEFE8 )
    {
        asm volatile (
                      "cpuid\n\t"
                      : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                      : "a" (eax)
                      );
        result = eax;
        if ( (UINT64)result )
        {
            eax = 1LL;
            ebx = 0;
            ecx = 0;
            edx = 0;
            asm volatile (
                          "cpuid\n\t"
                          : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                          : "a" (eax)
                          );
            
            v9 = edx;
            if ( (ecx & 0x80u) != 0LL )
            {
                v10 = AsmReadMsr64(0x198u);
                v12 = AsmReadMsr64(0x199u);
                result = sub_12453("PERF_STS 0x%x, PERF_CTL 0x%x", v10, v12);
            }
            if ( (v9 & 0x20000000) != 0 )
            {
                v13 = AsmReadMsr64(0x19Cu);
                return sub_12453("THERM_STATUS 0x%x", v13);
            }
        }
    }
    return result;
}

UINT64 sub_2826F(char* a1, char* a2, UINT32 a3)
{
    for(UINT32 i = 0;i < a3;i++){
        if(a1[i] == 0){
            return 0;
        }
        if(a1[i] != a2[i] ){
            return a1[i] - a2[i];
        }
    }
    return 0;
}

StringStruct2* sub_11E1C(void)
{
    StringStruct2* result; // rax
    
    qword_AEFB0 = 0LL;
    qword_AEFB8 = 0LL;
    qword_AEFA0 = 0LL;
    qword_AEFA8 = 0LL;
    dword_AEFC8 = 0;
    dword_AEFCC = 0;
    dword_AEFD0 = 0;
    result = sub_11D29(NULL, "/");
    qword_AEFC0 = result;
    return result;
}

bool sub_46304(
               IN CONST GUID  *Guid1,
               IN CONST GUID  *Guid2
               )
{
    return CompareGuid(Guid1,Guid2);
}

char sub_B393(CHAR16* a1, UINTN a2, char *a3, UINTN a4, UINT32 a5)
{
    UINT32 v5 = 0; // eax
    UINT64 v6; // r11
    CHAR16* v7; // rsi
    UINTN v8; // edi
    UINTN v9; // edx
    UINT32 v10; // esi
    
    v5 |= a4;
    v6 = (UINT64)&a3[a4 - 1];
LABEL_2:
    v7 = a1 + 2;
    v8 = a2;
    while ( v8 > 0 )
    {
        v9 = v8;
        a1 = v7;
        v10 = *(UINT16 *)(v7 - 2);
        
        SETHIWORD(v5, HIWORD(v10));
        HIWORD(v5) = HIWORD(v10);
        SETHIWORD(v5, HIWORD(v10));
        LOWORD(v5) = __ROL2__(v10, 8);
        if ( a5 != 2 )
            v5 = v10;
        if ( (UINT16)v5 > 0x7Fu )
        {
            if ( (UINT16)v5 > 0x7FFu )
            {
                if ( (UINT64)(a3 + 2) >= v6 )
                    break;
                *a3 = ((UINT16)v5 >> 12) | 0xE0;
                a3[1] = (((UINT16)v5 >> 6) & 0x3F) | 0x80;
                LOBYTE(v5) = (v5 & 0x3F) | 0x80;
                a3[2] = v5;
                a3 += 3;
            }
            else
            {
                if ( (UINT64)(a3 + 1) >= v6 )
                    break;
                *a3 = (v5 >> 6) | 0xC0;
                LOBYTE(v5) = (v5 & 0x3F) | 0x80;
                a3[1] = v5;
                a3 += 2;
            }
            goto LABEL_15;
        }
        if ( (UINT64)a3 >= v6 )
            break;
        --v8;
        v7 = a1 + 2;
        if ( (CHAR16)v5 )
        {
            *a3++ = v5;
        LABEL_15:
            a2 = v9 - 1;
            goto LABEL_2;
        }
    }
    if ( a4 )
        *a3 = 0;
    return v5;
}


UINT64 sub_B5EA(void)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    
    StringStruct2 *v0; // rsi
    UINT32 v1; // eax
    StringStruct2* v2; // rax
    StringStruct2* v3; // r13
    EFI_DATA_HUB_PROTOCOL* v4; // rdi
    EFI_DATA_RECORD_HEADER* v5; // r14
    UINT32 v6; // ebx
    void* v7; // rsi
    StringStruct2* v8; // rax
    StringStruct2* v9; // rsi
    UINT32 v10; // eax
    GUID v12 = NULL_GUID;
    
    
    UINT64 v13; // [rsp+38h] [rbp-D8h]
    char v14[48]; // [rsp+40h] [rbp-D0h] BYREF
    char v15[9]; // [rsp+70h] [rbp-A0h] BYREF
    EFI_DATA_HUB_PROTOCOL  *DataHub;
    UINT64 v17; // [rsp+C0h] [rbp-50h] BYREF
    UINT64 v18; // [rsp+C8h] [rbp-48h] BYREF
    EFI_DATA_RECORD_HEADER* v19; // [rsp+D0h] [rbp-40h] BYREF
    
    memset(v15, 170, 64);
    v17 = 0xAAAAAAAAAAAAAAAAuLL;
    DEBUG ((DEBUG_INFO,"AAPL: #[EB.BST.IDT|+]\n"));
    sub_11E1C();
    v0 = sub_1207F("/", 1);
    if ( !v0 )
        sub_E617("#[EB.BST.IDT|!] NULL <- EB.DT.FN + /\n");
    memset(v14, 170, sizeof(v14));
    
    v18 = 63LL;
    
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    Status = GetVariable(
                         L"B",
                         &gAppleVendorVariableGuid,
                         0LL,
                         &v18,
                         &v12);
    if ( Status >= 0  && v18 )
    {
        *((char *)&v12 + v18) = 0;
        DEBUG ((DEBUG_INFO,"AAPL: #[EB|BM] %s\n", (const char *)&v12));
        v1 = (UINT32)sub_2822A((const char *)&v12);
        sub_11BA4(v0, "bridge-model", v1 + 1LL, (char *)&v12, 1);
    }
    v2 = sub_1207F("/efi/platform", 1);
    if ( v2 )
    {
        v3 = v2;
        DataHub = (EFI_DATA_HUB_PROTOCOL *)0xAAAAAAAAAAAAAAAAuLL;
        
        Status = LocateProtocol (&gEfiDataHubProtocolGuid,
                                 NULL,
                                 (VOID *)&DataHub
                                 );
        
        if ( Status >= 0 )
        {
            v4 = DataHub;
            
            EFI_DATA_HUB_GET_NEXT_RECORD GetNextRecord = v4->GetNextRecord;
            /*
             v13 = 0x7A4C0FB664593CB0LL;
             v12 = (char*)0x4051656164517CC8LL;
             */
            v12.Data1 = 0x40516561;
            v12.Data2 = 0x6451;
            v12.Data3 = 0x7CC8;
            v12.Data4[0] = 0x7A;
            v12.Data4[1] = 0x4C;
            v12.Data4[2] = 0x0F;
            v12.Data4[3] = 0xB6;
            v12.Data4[4] = 0x64;
            v12.Data4[5] = 0x59;
            v12.Data4[6] = 0x3C;
            v12.Data4[7] = 0xB0;
            //= { 0x7A, 0x4C, 0x0F, 0xB6, 0x64, 0x59, 0x3C, 0xB0 };
            //{ 0x40516561, 0x6451, 0x7CC8, { 0x7A, 0x4C, 0x0F, 0xB6, 0x64, 0x59, 0x3C, 0xB0 }};
            v18 = 0LL;
            do
            {
                if ( GetNextRecord(v4,
                                   &v18,
                                   0LL,
                                   &v19) < 0 )
                    break;
                if ( (bool)sub_46304(&v19->ProducerName, &v12) )
                {
                    v5 = v19;
                    v6 = 3 * *(UINT32 *)(v19 + 88) + 1;
                    v7 = sub_1D2B1(v6);
                    EFI_DATA_RECORD_HEADER *next = ++v19;
                    EFI_GUID ProducerName = next->ProducerName;
                    sub_B393((CHAR16*)v5 + 96, *(UINT32 *)(v5 + 88) >> 1, v7, v6, 1);
                    sub_11BA4(v3, v7, *(UINT32 *)(v5 + 92), (char* )(v5 + *(UINT32 *)(v5 + 88) + 96), 1);
                }
            }
            while ( v18 );
        }
    }
    v8 = sub_1207F("/chosen", 1);
    if ( v8 )
    {
        v9 = v8;
        v17 = 63LL;
        
        
        Status = GetVariable(
                             L"B",
                             &gAppleVendorVariableGuid,
                             0LL,
                             &v17,
                             v15);
        
        if ( Status >= 0 )
        {
            *((char *)v15 + v17) = 0;
            DEBUG ((DEBUG_INFO,"AAPL: #[EB|BBSID] %s\n", v15));
            v10 = sub_2822A(v15);
            sub_11BA4(v9, "bridge-boot-session-uuid", v10 + 1, v15, 1);
        }
    }
    else
    {
        sub_E617("#[EB.BST.IDT|!] NULL <- EB.DT.FN + /chosen\n");
    }
    DEBUG ((DEBUG_INFO,"AAPL: #[EB.BST.IDT|-]\n"));
    return 0LL;
}

void sub_E430(void) {
    UINT64 *rax;
    
    // Load the address of byte_E467 into rax
    rax = (UINT64 *)&byte_E467;
    
    // Move the content of rax to dword_E48A
    dword_E48A.dwordValue = ((UINT64)rax & 0xffffffff);
    
    // Load the address of word_E49A into rax
    rax = (UINT64 *)&word_E49A;
    
    // Load the address of byte_E490+2 into rax
    rax = (UINT64 *)&byte_E490[2];
    
    // Load the global descriptor table using lgdt
    asm volatile("lgdt %0" : : "m"(byte_E490));
    
    // Move the value 0x10 to ax register
    UINT16 ax = 0x10;
    
    // Move the value in ax register to ds register
    asm("mov %0, %%ds" : : "r"(ax));
    
    // Set other segment registers like es, gs, and fs to the value in ax
    asm("mov %0, %%es" : : "r"(ax));
    asm("mov %0, %%gs" : : "r"(ax));
    asm("mov %0, %%fs" : : "r"(ax));
    rax = (UINT64*)&dword_E48A;
    // Jump to the address stored in rax
    asm("jmp *%0" : : "r"(rax));
}

UINT64 sub_E4B2(UINT32 a1, UINT64 a2, UINT32 a3)
{
    UINT32 v3; // eax
    UINT64 v4; // rcx
    
    v3 = ~a1;
    if ( a3 )
    {
        v4 = 0LL;
        do
            v3 = *((UINT32 *)qword_A9D50 + (*(UINT8 *)(a2 + v4++) ^ (UINT32)(UINT8)v3)) ^ (v3 >> 8);
        while ( a3 != (UINT32)v4 );
    }
    return ~v3;
}

UINT64 sub_4632A(
                 IN CONST VOID  *DestinationBuffer,
                 IN CONST VOID  *SourceBuffer,
                 IN UINTN       Length
                 )
{
    return CompareMem(DestinationBuffer, SourceBuffer, Length);
}

const char * sub_BBB2(char *a1, UINT8 a2)
{
    UINT8 v3; // bl
    bool v4; // cf
    const char *result; // rax
    
    if ( a2 >= 2u )
    {
        v3 = a2;
        do
        {
            if ( !*a1 )
                break;
            a1 += (int)sub_2822A(a1) + 1;
            v4 = v3-- == 1;
        }
        while ( !v4 && v3 != 1 );
    }
    result = "BadIndex";
    if ( *a1 )
        return a1;
    return result;
}

void sub_BEBA(UINT64* a1)
{
    DEBUG ((DEBUG_INFO,"%08lX-%04lX-%04lX-%02lX%02lX-%02lX%02lX%02lX%02lX%02lX%02lX",
            *a1,
            *(UINT16 *)(a1 + 4),
            *(UINT16 *)(a1 + 6),
            *(UINT8 *)(a1 + 8),
            *(UINT8 *)(a1 + 9),
            *(UINT8 *)(a1 + 10),
            *(UINT8 *)(a1 + 11),
            *(UINT8 *)(a1 + 12),
            *(UINT8 *)(a1 + 13),
            *(UINT8 *)(a1 + 14),
            *(UINT8 *)(a1 + 15)));
}

UINT64 sub_BBF8(double a3)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    
    char v3; // r14
    UINT64 v4; // rsi
    UINT64 v5; // rbx
    char* v6; // rsi
    UINT8 *v8; // r12
    UINT64 v9; // rsi
    UINT8 *v10; // r15
    UINTN v11; // al
    UINT64 v12; // rax
    UINT64 v13; // rax
    UINT64 v14; // rax
    UINT64 v15; // rax
    UINT64 v16; // rax
    UINT64 v17; // rax
    UINT8 *v18; // rax
    UINT8 v19[128]; // [rsp+30h] [rbp-C0h] BYREF
    UINT64 v20[8]; // [rsp+B0h] [rbp-40h] BYREF
    
    v20[0] = 64LL;
    v3 = 0;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    Status = GetVariable(
                         L"H",
                         &gAppleVendorVariableGuid,
                         0LL,
                         &v11,
                         &unk_AE980);
    
    if ( Status == RETURN_SUCCESS )
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB|BRD:NV] %e\n", a3));
        v3 = 1;
    }
    UINTN NumberOfTableEntries = mSystemTable->NumberOfTableEntries;
    if ( NumberOfTableEntries )
        ;
    return 0LL;
}

char * sub_B526(CHAR16* a1)
{
    UINTN v2; // edi
    char *v3; // rax
    char *v4; // rsi
    
    v2 = StrLen(a1);
    v3 = sub_1D2B1((UINT32)(3 * v2 + 1));
    v4 = v3;
    if ( v3 )
    {
        *v3 = 0;
        sub_B393(a1, v2, v3, 3 * v2 + 1, 1);
    }
    return v4;
}

char sub_1F241(char *a1, char *a2)
{
    char v2; // r8
    UINT8 v3; // al
    UINT8 v4; // r8
    char result; // al
    char v6; // r8
    char v7; // r8
    
    v2 = *a1;
    v3 = *a1 - 48;
    if ( v3 >= 0xAu )
    {
        if ( (UINT8)(v2 - 97) <= 5u )
        {
            v4 = v2 - 87;
        LABEL_6:
            v3 = v4;
            goto LABEL_8;
        }
        if ( (UINT8)(v2 - 65) <= 5u )
        {
            v4 = v2 - 55;
            goto LABEL_6;
        }
        *a2 = 1;
        v3 = 0;
    }
LABEL_8:
    result = 16 * v3;
    v6 = a1[1];
    if ( (UINT8)(v6 - 48) <= 9u )
        return result | (v6 - 48);
    if ( (UINT8)(v6 - 97) <= 5u )
    {
        v7 = v6 - 87;
        return result | v7;
    }
    if ( (UINT8)(v6 - 65) <= 5u )
    {
        v7 = v6 - 55;
        return result | v7;
    }
    *a2 = 1;
    return result;
}

bool sub_1F0E2(char *a1, char *a2)
{
    char v5[25]; // [rsp+27h] [rbp-19h] BYREF
    
    v5[0] = 0;
    if ( (UINT32)sub_2822A(a1) != 36 )
        return 0;
    *a2 = sub_1F241(a1, v5);
    a2[1] = sub_1F241(a1 + 2, v5);
    a2[2] = sub_1F241(a1 + 4, v5);
    a2[3] = sub_1F241(a1 + 6, v5);
    if ( a1[8] != 45 )
        return 0;
    a2[4] = sub_1F241(a1 + 9, v5);
    a2[5] = sub_1F241(a1 + 11, v5);
    if ( a1[13] != 45 )
        return 0;
    a2[6] = sub_1F241(a1 + 14, v5);
    a2[7] = sub_1F241(a1 + 16, v5);
    if ( a1[18] != 45 )
        return 0;
    a2[8] = sub_1F241(a1 + 19, v5);
    a2[9] = sub_1F241(a1 + 21, v5);
    if ( a1[23] != 45 )
        return 0;
    a2[10] = sub_1F241(a1 + 24, v5);
    a2[11] = sub_1F241(a1 + 26, v5);
    a2[12] = sub_1F241(a1 + 28, v5);
    a2[13] = sub_1F241(a1 + 30, v5);
    a2[14] = sub_1F241(a1 + 32, v5);
    a2[15] = sub_1F241(a1 + 34, v5);
    return v5[0] == 0;
}


EFI_STATUS sub_15D44(char a1, char a2)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    
    void* v4; // rax
    UINT64 v5; // rdi
    UINT64 v6; // rax
    UINT64 v8; // [rsp+48h] [rbp-28h] BYREF
    UINT64 v9[4]; // [rsp+50h] [rbp-20h] BYREF
    
    v8 = 0xAAAAAAAAAAAAAAAAuLL;
    v9[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v4 = qword_B1F28;
    
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    
    if ( qword_B1F28 )
    {
        v5 = 0LL;
    }
    else
    {
        v6 = LocateProtocol(&qword_ADC40, 0LL, &qword_B1F28);
        if ( v6 < 0 )
        {
            qword_B1F28 = (void*)1LL;
            v5 = 0LL;
            goto LABEL_15;
        }
        v5 = v6;
        v4 = qword_B1F28;
    }
    if ( v4 >= (void*)2 )
    {
        if ( a1 )
        {
            if ( !byte_B1F31 )
            {
                byte_B1F31 = 1;
                DEBUG ((DEBUG_INFO,"#[EB|G:CA]\n"));
                sub_12453("Start ConnectAll");
                v5 = (*(UINT64 (**)(void))(qword_B1F28 + 24))();
                sub_12453("End ConnectAll");
                if ( v5 < 0 )
                    DEBUG ((DEBUG_INFO,"#[EB.G.CS|!] %r <- %g.CA\n", v5));
            }
        }
        if ( a2 )
        {
            if ( !byte_AF140 )
            {
                byte_AF140 = 1;
                DEBUG ((DEBUG_INFO,"#[EB|G:CD]\n"));
                sub_12453("Start ConnectDisplay");
                v5 = (*(UINT64 (**)(void))(qword_B1F28 + 8))();
                sub_12453("End ConnectDisplay");
                if ( v5 < 0 )
                    DEBUG ((DEBUG_INFO,"#[EB.G.CS|!] %r <- %g.CD\n", v5));
            }
        }
    }
LABEL_15:
    if ( !qword_AF148 )
    {
        v5 = LocateProtocol(&unk_AE8DC, 0LL, &qword_AF148);
        if ( v5 < 0 )
        {
            qword_AF148 = (void*)1LL;
            v5 = 0LL;
        }
    }
    if ( a1 && a2 && (UINT64)qword_AF148 >= 2 )
        (*(void ( **)(UINT64, UINT64, UINT64, UINT64, UINT64, UINT64))(qword_AF148 + 8))(
                                                                                         (UINT64)qword_AF148,
                                                                                         2LL,
                                                                                         0LL,
                                                                                         0LL,
                                                                                         0LL,
                                                                                         0LL);
    v8 = 8LL;
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    
    if ( GetVariable(
                     L"gfx-saved-config-restore-status",
                     &gAppleVendorVariableGuid,
                     0LL,
                     &v8,
                     v9) >= 0 )
    {
        dword_AD81C = (UINT32)v9[0];
        if ( v9[0] < 0 )
            dword_AD81C = LODWORD(v9[0]) | 0x80000000;
    }
    DEBUG ((DEBUG_INFO,"#[EB.G.CS|-?] %r\n", v5));
    return Status;
}

UINT64 sub_2830D(UINT8 *a1)
{
    UINT8 v1; // dl
    UINT64 result; // rax
    
    do
    {
        do
            v1 = *a1++;
        while ( v1 == 9 );
    }
    while ( v1 == 32 );
    result = 0LL;
    if ( (UINT8)(v1 - 48) <= 9u )
    {
        LODWORD(result) = 0;
        do
        {
            result = (UINT32)v1 + 10 * (UINT32)result - 48;
            v1 = *a1++;
        }
        while ( (UINT8)(v1 - 48) < 0xAu );
    }
    return result;
}


unsigned long charswap_ulong(unsigned long val) {
    return (
            ((val & 0x000000FF) << 24) |
            ((val & 0x0000FF00) << 8) |
            ((val & 0x00FF0000) >> 8) |
            ((val & 0xFF000000) >> 24)
            );
}

UINT64 sub_12166(void)
{
    UINT64 result;
    
    __asm volatile (
                    "lfence\n\t"
                    "rdtsc\n\t"
                    "lfence\n\t"
                    : "=a" (result)
                    :
                    :
                    );
    return result;
}

char* sub_240D0(char *a1, char *a2, UINT64 a3)
{
    char *result; // rax
    UINT64 v5; // rcx
    char *v6; // rdi
    char *v7; // rsi
    
    result = a2;
    if ( a1 < a2 && &a1[a3 - 1] >= a2 )
    {
        v7 = &a1[a3 - 1];
        v6 = &a2[a3 - 1];
    }
    else
    {
        v5 = a3;
        a3 &= 7u;
        v5 >>= 3;
        memcpy(a2, a1, 8 * v5);
        v7 = &a1[8 * v5];
        v6 = &a2[8 * v5];
    }
    memcpy(v6, v7, a3);
    return result;
}

void sub_97BF(EFI_RESET_TYPE a1, UINT32 a2)
{
    EFI_RESET_TYPE v3; // edi
    
    v3 = a1;
    LOWORD(a1) = 64;
    sub_5E29(a1, 0LL, 0LL);
    DEBUG ((DEBUG_INFO,"#[EB|REBOOT] %d\n", v3));
    sub_22DC7(a2);
    EFI_RESET_SYSTEM ResetSystem = mRuntimeServices->ResetSystem;
    
    
    ResetSystem(v3, 0LL, 0LL, 0LL);
}

void* sub_171EC(char* a1, UINT64 a2, double a3)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    
    UINT64 v4; // r13
    UINT64 v5; // rax
    char v6; // bl
    UINT64 v7; // rsi
    UINT64 v8; // rax
    UINT64 v9; // rsi
    UINT64 v10; // rsi
    UINT64 v11; // rax
    void* v12; // rax
    void* v13; // rbx
    UINT64 v14; // rax
    UINT64 v15; // rax
    UINT64 v16; // rax
    UINT64 v17; // rax
    UINT64 v18; // rcx
    UINT64 v19; // rax
    UINT64 v20; // rdx
    void* v21; // rdi
    int v22; // ebx
    int v23; // eax
    int v24; // ebx
    int v25; // edx
    UINT64 *v26; // rcx
    VOID* i; // rcx
    char v28; // al
    char v29; // r14
    UINT64 v30; // rdx
    UINT64 v31; // r8
    char* v32; // rax
    char* v33; // rbx
    UINT64 v34; // rdx
    UINT64 v35; // rcx
    UINT64 v36; // rax
    UINT64 v37; // rax
    UINT64 v38; // rax
    EFI_GUID *v39; // rsi
    UINT64 v40; // rax
    UINT64 v41; // rax
    UINT64 v42; // rsi
    UINT64 v43; // rax
    UINT64 v44; // r8
    void* v45; // rcx
    UINT64 v46; // rax
    UINT64 v47; // rsi
    UINT64 v48; // r8
    UINT64 v49; // rax
    UINT64 v50; // rcx
    UINT64 v51; // r8
    UINT64 v52; // r8
    UINT64 v53; // rcx
    UINT64 v55; // ecx
    UINT64 v56; // r8
    UINT64 v57; // rcx
    UINT64 j; // rax
    EFI_DEVICE_PATH_PROTOCOL* v59; // [rsp+38h] [rbp-B8h] BYREF
    char *v61; // [rsp+50h] [rbp-A0h] BYREF
    EFI_HANDLE v62; // [rsp+58h] [rbp-98h] BYREF
    EFI_DEVICE_PATH_PROTOCOL* v63[5]; // [rsp+60h] [rbp-90h] BYREF
    UINT64 v64; // [rsp+88h] [rbp-68h] BYREF
    UINTN v65[12]; // [rsp+90h] [rbp-60h] BYREF
    
    v64 = 0LL;
    /*
     v60[1] = 0x829F5C9941FE80A8uLL;
     v60[0] = 0x4BBBAB2A7C436110LL;
     */
    EFI_GUID v60 = {0x4BBBAB2A,0x7C43,0x6110,{0x82,0x9F,0x5C,0x99,0x41,0xFE,0x80,0xA8}};
    
    v62 = 0LL;
    v59 = (EFI_DEVICE_PATH_PROTOCOL*)0xAAAAAAAAAAAAAAAAuLL;
    v4 = 1LL;
    DEBUG ((DEBUG_INFO,"#[EB.H.CHK|BM] 0x%016qx\n", qword_B1DE8));
    v65[0] = 20LL;
    
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    EFI_GET_TIME GetTime = mRuntimeServices->GetTime;
    
    
    EFI_LOCATE_DEVICE_PATH LocateDevicePath = mBootServices->LocateDevicePath;
    
    EFI_HANDLE_PROTOCOL HandleProtocol = mBootServices->HandleProtocol;
    
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    Status = GetVariable (
                          L"boot-signature",
                          &gAppleBootVariableGuid,
                          NULL,
                          v65,
                          &unk_B1FE8);
    
    v6 = 1;
    if ( v5 < 0 )
    {
        v7 = v5;
        DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.GV %S %g\n", v5, L"boot-signature", &gAppleBootVariableGuid));
        v6 = 0;
        sub_5E29(0LL, v7, 23LL);
        v4 = 0LL;
    }
    v65[0] = 16LL;
    
    Status = GetVariable (
                          L"b",
                          &gAppleBootVariableGuid,
                          NULL,
                          v65,
                          &unk_B1FFC);
    if ( Status < 0 )
    {
        v9 = v8;
        DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.GV %S %g\n", v8, "b", &gAppleBootVariableGuid));
        sub_5E29(0LL, v9, 24LL);
    }
    else
    {
        v4 = 1LL;
        if ( v6 )
        {
            qword_B1FE0 = 0x14C504141LL;
            v4 = 2LL;
        }
    }
    v10 = 0x800000000000000EuLL;
    v65[0] = 0LL;
    
    Status = GetVariable (
                          L"b",
                          &gAppleBootVariableGuid,
                          NULL,
                          v65,
                          NULL);
    
    if ( Status == RETURN_BUFFER_TOO_SMALL && v65[0] )
    {
        v12 = sub_1D2B1(v65[0]);
        if ( v12 )
        {
            v13 = v12;
            
            Status = GetVariable (
                                  L"b",
                                  &gAppleBootVariableGuid,
                                  NULL,
                                  v65,
                                  v12);
            
            if ( Status < 0 )
            {
                DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.GV %S %g\n", v14, "b", &gAppleBootVariableGuid));
                sub_1D327(v13);
            }
            else
            {
                ++v4;
                DEBUG ((DEBUG_INFO,"#[EB|H:BI] <%.*b>\n", v65[0], v13));
                qword_B1FD8 = v65[0];
                qword_B1F78 = v13;
            }
        }
        else
        {
            DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] NULL <- EB.M.BMA %qd\n", v65[0]));
        }
    }
    else
    {
        DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.GV %S %g\n", v11, "b", &gAppleBootVariableGuid));
    }
    
    
    Status = SetVariable (
                          L"boot-signature",
                          &gAppleBootVariableGuid,
                          0LL,
                          0LL,
                          0LL);
    
    if ( Status < 0 )
        DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.SV- %S %g\n", v15, L"boot-signature", &gAppleBootVariableGuid));
    Status = SetVariable (
                          L"b",
                          &gAppleBootVariableGuid,
                          0LL,
                          0LL,
                          0LL);
    if ( Status < 0 )
        DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.SV- %S %g\n", v16, "b", &gAppleBootVariableGuid));
    
    
    Status = SetVariable (
                          L"b",
                          &gAppleBootVariableGuid,
                          0LL,
                          0LL,
                          0LL);
    if ( Status < 0 )
        DEBUG ((DEBUG_INFO,"#[EB.H.LV|!] %r <- RT.SV- %S %g\n", v17, "b", &gAppleBootVariableGuid));
    v19 = RETURN_ABORTED;
    if ( !v4 )
        v19 = RETURN_NOT_FOUND;
    Status = RETURN_SUCCESS;
    if ( v4 != 3 )
        Status = v19;
    if ( v20 != RETURN_NOT_FOUND )
    {
        if ( v20 >= 0 )
        {
            if ( (qword_B1DE8 & 1) != 0 )
            {
                DEBUG ((DEBUG_INFO,"#[EB.H.CHK|BM:SAFE]\n"));
                v10 = 0LL;
                sub_5E29(0LL, 0LL, 32LL);
                goto LABEL_120;
            }
            LODWORD(v21) = 0;
            LOWORD(v18) = 16;
            sub_5E29(v18, v20, 0LL);
            
            EFI_TIME                LogTime;
            
            EFI_GET_TIME GetTime = mRuntimeServices->GetTime;
            
            if ( GetTime (&LogTime, NULL) >= 0 )
            {
                v22 = -1;
                if ( LOWORD(v65[0]) >= 0x7B3u )
                {
                    v23 = 1971;
                    if ( LOWORD(v65[0]) > 0x7B3u )
                        v23 = LOWORD(v65[0]);
                    v24 = 0;
                    v25 = 1970;
                    do
                        v24 += ((v25++ & 3) == 0) + 365;
                    while ( v23 != v25 );
                    v22 = v24 - 1;
                }
                v26 = qword_AA9D0;
                if ( (v65[0] & 3) == 0 )
                    v26 = qword_AAA10;
                LODWORD(v21) = BYTE6(v65[0])
                + 60 * BYTE5(v65[0])
                + 3600 * BYTE4(v65[0])
                + 86400 * (*((UINT32 *)v26 + BYTE2(v65[0]) - 1) + v22 + BYTE3(v65[0]));
            }
            dword_B1F68 = (UINT32)((UINT64)v21 & 0xffffffff);
            v65[0] = 0LL;
            v63[0] = (EFI_DEVICE_PATH_PROTOCOL *)0xAAAAAAAAAAAAAAAAuLL;
            v61 = (char *)0xAAAAAAAAAAAAAAAALL;
            for ( i = qword_B1F78; ; i += *(UINT16 *)(i + 2) )
            {
                v28 = *(char *)i & 0x7F;
                if ( v28 == 4 )
                {
                    if ( *(char *)(i + 1) == 4 )
                    {
                        v32 = sub_B526((CHAR16*)(i + 4));
                        if ( v32 )
                        {
                            v33 = v32;
                            qword_B1F70 = sub_2834C(v32, &v61, 16LL);
                            LOBYTE(v21) = 1;
                            if ( a1 && v61 && *v61 == 58 && (UINT8)sub_1F0E2(v61 + 1, a1) )
                            {
                                DEBUG ((DEBUG_INFO,"#[EB|H:VKU] %g\n", a3));
                                v29 = 1;
                                LODWORD(v21) = 0;
                            }
                            else
                            {
                                v29 = 0;
                            }
                            sub_1D327(v33);
                            if ( qword_B1F70 )
                            {
                                v63[0] = qword_B1F78;
                                v21 = (void*)LocateDevicePath(
                                                              &gEfiBlockIoCryptoProtocolGuid,
                                                              v63,
                                                              (EFI_HANDLE *)v65);
                                DEBUG ((1LL, "#[EB.H.OID|!] %r <- BS.LocDP %g\n", v21, &gEfiBlockIoCryptoProtocolGuid));
                                if ( v21
                                    
                                    ||
                                    v21 < 0
                                    || HandleProtocol(
                                                      (EFI_HANDLE)v65[0],
                                                      &gEfiBlockIoCryptoProtocolGuid,
                                                      (void**)&qword_B1F80) < 0 )
                                {
                                    v63[0] = qword_B1F78;
                                    if ( LocateDevicePath(&gEfiBlockIoProtocolGuid, v63, (EFI_HANDLE*)v65) < 0 )
                                    {
                                        v63[0] = qword_B1F78;
                                        LOBYTE(v35) = 1;
                                        LOBYTE(v34) = 1;
                                        v38 = sub_15D44(v35, v34);
                                        if ( v38 < 0
                                            || (v38 = LocateDevicePath(
                                                                       &gEfiBlockIoProtocolGuid,
                                                                       v63,
                                                                       (EFI_HANDLE*)v65) &&
                                                v38 < 0) )
                                        {
                                            v10 = v38;
                                            DEBUG ((DEBUG_INFO,"#[EB.H.OID|!] %r <- BS.LocDP %g\n", v38, &gEfiBlockIoProtocolGuid));
                                            v30 = v10;
                                            v31 = 38LL;
                                            goto LABEL_71;
                                        }
                                    }
                                    EFI_DISK_IO_PROTOCOL         *DiskIo;
                                    
                                    v36 = HandleProtocol(
                                                         (EFI_HANDLE)v65[0],
                                                         &gEfiDiskIoProtocolGuid,
                                                         (void**)&qword_B1F50);
                                    if ( v36 < 0 )
                                    {
                                        v10 = v36;
                                        DEBUG ((DEBUG_INFO,"#[EB.H.OID|!] %r <- BS.HdlP %g\n", v36, &gEfiBlockIoProtocolGuid));
                                        v30 = v10;
                                        v31 = 39LL;
                                        goto LABEL_71;
                                    }
                                    v37 = HandleProtocol(
                                                         (EFI_HANDLE)v65[0],
                                                         &gEfiBlockIoProtocolGuid,
                                                         (void**)&qword_B1F80);
                                    if ( v37 < 0 )
                                    {
                                        v10 = v37;
                                        v30 = v37;
                                        v31 = 40LL;
                                        goto LABEL_71;
                                    }
                                }
                                else
                                {
                                    BYTE2(qword_B1DE8) |= 0x10u;
                                    DEBUG ((DEBUG_INFO,"#[EB|H:BIOC]\n"));
                                }
                                v10 = 0LL;
                                goto LABEL_72;
                            }
                            v30 = 0LL;
                            v31 = 37LL;
                        }
                        else
                        {
                            DEBUG ((DEBUG_INFO,"#[EB.H.OID|!] NULL <- EB.BST.U8U16\n"));
                            v29 = 0;
                            v30 = 0LL;
                            v31 = 36LL;
                        }
                    LABEL_71:
                        sub_5E29(0LL, v30, (UINT32)v31);
                    LABEL_72:
                        if ( v10 < 0 )
                        {
                            DEBUG ((DEBUG_INFO,"#[EB.H.CHK|!] %r <- EB.H.OID\n", v10));
                            goto LABEL_120;
                        }
                        v39 = &unk_ADB90;
                        if ( !_bittest64(&qword_B1DE8, 0x14u) )
                            v39 = &unk_ADB80;
                        EFI_GUID  gAppleDiskIoProtocolGuid = unk_ADB80;
                        v59 = qword_B1F78;
                        v40 = LocateDevicePath(
                                               &gAppleDiskIoProtocolGuid,
                                               &v59,
                                               &v62);
                        if ( v40 < 0 )
                        {
                            DEBUG ((DEBUG_INFO,"#[EB.H.CHK|!] %r <- BS.LocDP %g\n", v40, &gAppleDiskIoProtocolGuid));
                        LABEL_80:
                            qword_B1F58 = 0LL;
                        }
                        else
                        {
                            v41 = HandleProtocol(v62, &gAppleDiskIoProtocolGuid, &qword_B1F58);
                            if ( v41 < 0 )
                            {
                                DEBUG ((DEBUG_INFO,"#[EB.H.CHK|!] %r <- BS.HdlP %g\n", v41, &gAppleDiskIoProtocolGuid));
                                goto LABEL_80;
                            }
                        }
                        v64 = 0LL;
                        v42 = 3500LL;
                        if ( GetVariable(
                                         L"boot-gfx-delay",
                                         &v60,
                                         0LL,
                                         &v64,
                                         0LL) == RETURN_BUFFER_TOO_SMALL )
                        {
                            if ( v64 )
                            {
                                v21 = (UINT64 *)sub_1D2B1(v64);
                                v43 = GetVariable(
                                                  L"boot-gfx-delay",
                                                  &v60,
                                                  0LL,
                                                  &v64,
                                                  v21);
                                if ( v21 )
                                {
                                    if ( v43 >= 0 )
                                        v42 = (int)sub_2830D(v21);
                                }
                            }
                        }
                        if ( LocateProtocol(
                                            &gAppleSmcIoProtocolGuid,
                                            0LL,
                                            &qword_B1F60) < 0 )
                        {
                            qword_B1F60 = 0LL;
                            v64 = 0LL;
                        }
                        else
                        {
                            v64 = 0LL;
                            if ( qword_B1F60 )
                            {
                                if ( GetVariable(
                                                 L"b",
                                                 &v60,
                                                 0LL,
                                                 &v64,
                                                 0LL) == RETURN_BUFFER_TOO_SMALL )
                                {
                                    LOWORD(v65[0]) = -21846;
                                    LOBYTE(v44) = 2;
                                    if ( (*(UINT64 ( **)(void*, UINT64, UINT64, UINT64 *))(qword_B1F60 + 8))(
                                                                                                             qword_B1F60,
                                                                                                             1129076555LL,
                                                                                                             v44,
                                                                                                             v65) >= 0 )
                                        dword_AF170 = 10 * (UINT32)charswap_ulong(LOWORD(v65[0]) << 16);
                                }
                            }
                        }
                        v45 = qword_B1E38;
                        if ( v45 )
                        {
                        LABEL_93:
                            v65[0] = 0xAAAAAAAAAAAAAAAAuLL;
                            v63[0] = (EFI_DEVICE_PATH_PROTOCOL *)0xAAAAAAAAAAAAAAAAuLL;
                            v46 = (*(UINT64 ( **)(void*, UINT64, UINTN *, EFI_DEVICE_PATH_PROTOCOL* *))(v45 + 48))(v45, 0LL, v65, v63);
                            if ( v42 && v46 >= 0 )
                                qword_AF178 = sub_4691F(1000000000000LL * v42, (UINT64)v63[0], 0LL);
                        }
                        else if ( LocateProtocol(
                                                 &gEfiCpuArchProtocolGuid,
                                                 0LL,
                                                 &qword_B1E38) < 0 )
                        {
                            qword_B1E38 = 0LL;
                        }
                        else
                        {
                            v45 = qword_B1E38;
                            if ( qword_B1E38 )
                                goto LABEL_93;
                        }
                        qword_AF180 = sub_12166();
                        memset(v65, 170, 32);
                        memset(v63, 170, 32);
                        v47 = 64LL;
                        if ( (qword_B1DE8 & 0x100000) == 0 )
                            v47 = 16LL;
                        qword_B1FD0 = v47;
                        if ( (qword_B1DE8 & 0x100000) == 0 )
                            sub_240D0((char*)&unk_B1FFC, (char*)byte_B1F90, v47);
                        if ( v29 )
                        {
                            sub_5E29(0LL, 0LL, 7LL);
                            DEBUG ((DEBUG_INFO,"#[EB|H:FND.1]\n"));
                            if ( !qword_B1F60 )
                            {
                                DEBUG ((DEBUG_INFO,"#[EB.H.LK|!SIO]\n"));
                                goto LABEL_116;
                            }
                            v21 = v65;
                            sub_E5B0(v65, 32LL);
                            LOBYTE(v48) = 32;
                            v49 = (*(UINT64 (**)(void *, UINT64, UINT64, UINT64 *))(qword_B1F60 + 8))(
                                                                                                      qword_B1F60,
                                                                                                      1212304208LL,
                                                                                                      v48,
                                                                                                      v65);
                            if ( v49 < 0 )
                            {
                                DEBUG ((DEBUG_INFO,"#[EB.H.LK|!] %r <- %g.R HBKP\n", v49, &gAppleSmcIoProtocolGuid));
                                goto LABEL_116;
                            }
                            v50 = 0LL;
                            while ( !*((char *)v65 + v50) )
                            {
                                if ( v50 <= 0x1E && ++v50 < v47 )
                                    continue;
                                DEBUG ((DEBUG_INFO,"#[EB.H.LK|!HBKP]\n"));
                                goto LABEL_116;
                            }
                            v55 = qword_B1DE8;
                            if ( (qword_B1DE8 & 0x100000) != 0 )
                            {
                                v21 = v63;
                                sub_E5B0(v63, 32LL);
                                LOBYTE(v56) = 32;
                                v49 = (*(UINT64 ( **)(void*, UINT64, UINT64, EFI_DEVICE_PATH_PROTOCOL* *))(qword_B1F60 + 8))(
                                                                                                                             qword_B1F60,
                                                                                                                             1212304213LL,
                                                                                                                             v56,
                                                                                                                             v63);
                                if ( v49 >= 0 )
                                {
                                    v57 = 0LL;
                                    while ( !*((char *)v63 + v57) )
                                    {
                                        if ( ++v57 == 32 )
                                        {
                                            DEBUG ((DEBUG_INFO,"#[EB.H.LK|!HBKU]\n"));
                                            goto LABEL_116;
                                        }
                                    }
                                    v55 = qword_B1DE8;
                                    goto LABEL_130;
                                }
                                DEBUG ((DEBUG_INFO,"#[EB.H.LK|!] %r <- %g.R HBKU\n", v49, &gAppleSmcIoProtocolGuid));
                            LABEL_116:
                                LOBYTE(v21) = 1;
                            LABEL_117:
                                sub_E5B0(v65, 32LL);
                                LOBYTE(v51) = 32;
                                (*(void (**)(void*, UINT64, UINT64, UINT64 *))(qword_B1F60 + 16))(
                                                                                                  qword_B1F60,
                                                                                                  1212304208LL,
                                                                                                  v51,
                                                                                                  v65);
                                LOBYTE(v52) = 32;
                                (*(void (**)(void*, UINT64, UINT64, UINT64 *))(qword_B1F60 + 16))(
                                                                                                  qword_B1F60,
                                                                                                  1212304213LL,
                                                                                                  v52,
                                                                                                  v65);
                                if ( v21 )
                                {
                                    DEBUG ((DEBUG_INFO,"#[EB|H:FND.2]\n"));
                                    LOBYTE(v21) = 1;
                                    return v21;
                                }
                                DEBUG ((DEBUG_INFO,"#[EB|H:BOOT.1]\n"));
                                // v10 = sub_17F0B(qword_B1F80, qword_B1F50, qword_B1F58, qword_B1F70, (UINT64)byte_B1F90, qword_B1FD0, 0);
                                DEBUG ((DEBUG_INFO,"#[EB.H.CHK|!] %r <- EB.H.HB\n", v10));
                            LABEL_120:
                                DEBUG ((DEBUG_INFO,"#[EB|H:FAIL]\n"));
                                LODWORD(v21) = 0;
                                LOWORD(v53) = 32;
                                sub_5E29(v53, v10, 0LL);
                                sub_97BF(0LL, 4LL);
                                return v21;
                            }
                        LABEL_130:
                            if ( (v55 & 0x100000) != 0 )
                            {
                                DEBUG ((DEBUG_INFO,"#[EB|H:HBKPU]\n"));
                                sub_240D0((char*)v65, (char*)byte_B1F90, 32LL);
                                sub_240D0((char*)v63, (char*)&unk_B1FB0, 32LL);
                            }
                            else
                            {
                                DEBUG ((DEBUG_INFO,"#[EB|H:HBKP]\n", v49));
                                for ( j = 0LL; j != v47; ++j )
                                    byte_B1F90[j] ^= *((char *)v65 + j);
                            }
                        }
                        else
                        {
                            DEBUG ((DEBUG_INFO,"#[EB|H:WCK]\n"));
                        }
                        LODWORD(v21) = 0;
                        goto LABEL_117;
                    }
                }
                else if ( v28 == 127 && *(unsigned char *)(i + 1) == 0xFF )
                {
                    DEBUG ((DEBUG_INFO,"#[EB.H.OID|!] NULL <- EB.H.FDPT\n"));
                    v29 = 0;
                    v30 = 0LL;
                    v31 = 35LL;
                    goto LABEL_71;
                }
            }
        }
        v10 = v20;
        goto LABEL_120;
    }
    DEBUG ((DEBUG_INFO,"#[EB|H:NOT]\n"));
    LODWORD(v21) = 0;
    return v21;
}

/*
 typedef
 VOID
 (EFIAPI *EFI_EVENT_NOTIFY)(
 IN  EFI_EVENT                Event,
 IN  VOID                     *Context
 );
 
 */
void sub_21067(
               IN  EFI_EVENT                Event,
               IN  VOID                     *Context
               )
{
    byte_B0320 = 1;
    qword_B0210 = 0LL;
}

void sub_20E9E(void)
{
    UINT64 i; // rcx
    UINT8 v1; // al
    
    if ( !byte_B0321 )
    {
        for ( i = 14LL; i != 256; ++i )
        {
            if ( (i & 0x80u) != 0LL )
            {
                out8(0x72u, i);
                
                in8(0x73u,v1);
            }
            else
            {
                out8(0x70u, i);
                in8(0x71u,v1);
            }
            byte_B0220[i] = v1;
        }
        byte_B0321 = 1;
    }
}

void sub_20DE6(void)
{
    UINT64 v0; // rax
    UINT64 v1; // rax
    
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    EFI_CREATE_EVENT CreateEvent = mBootServices->CreateEvent;
    if ( !qword_B0210 && (byte_B0320 & 1) == 0 )
    {
        v0 = LocateProtocol(&gAppleRtcRamProtocolGuid, 0LL, &qword_B0210);
        if ( v0 < 0 )
        {
            DEBUG ((DEBUG_INFO,"#[EB.RTC.CP|!] %r <- BS.LocP %g\n", v0, &gAppleRtcRamProtocolGuid));
            byte_B0320 = 1;
        }
        else
        {
            v1 = CreateEvent(
                             513LL,
                             8LL,
                             sub_21067,
                             0LL,
                             &qword_B2060);
            if ( v1 < 0 )
                DEBUG ((DEBUG_INFO,"#[EB.RTC.CP|!] %r <- BS.CrE 0x%08X\n", v1, 513LL));
        }
    }
}

UINT16 sub_E53F(UINT16 a1, UINT64 a2, int a3)
{
    UINT16 result; // ax
    UINT64 v4; // rcx
    
    result = a1;
    if ( a3 )
    {
        v4 = 0LL;
        do
            result = *((CHAR16 *)qword_AA550 + (*(UINT8 *)(a2 + v4++) ^ (UINT64)(UINT8)result)) ^ HIBYTE(result);
        while ( a3 != (UINT32)v4 );
    }
    return result;
}

UINT64 sub_20EDF(UINT64 a1, UINT8 a2, UINT8 a3)
{
    UINT64 result; // rax
    UINT64 v4; // rdi
    UINT64 v5; // r14
    UINT32 v6; // ebx
    UINT64 v7; // rsi
    char v8; // al
    UINT8 v9; // cl
    UINT32 v10; // esi
    int v11; // edi
    UINT32 v12; // ecx
    
    result = 0x8000000000000002uLL;
    if ( a1 )
    {
        if ( a3 >= 0xEu )
        {
            v4 = a3;
            v5 = a2;
            v6 = a3 + a2;
            if ( v6 <= 0xFF )
            {
                if ( a2 )
                {
                    v7 = a1;
                    sub_20DE6();
                    if ( qword_B0210 )
                        return (*(UINT64 ( **)(void*, UINT64, UINT64, UINT64))(qword_B0210 + 16))(
                                                                                                  qword_B0210,
                                                                                                  v7,
                                                                                                  v5,
                                                                                                  v4);
                    sub_20E9E();
                    v8 = 1;
                    while ( 2 )
                    {
                        ++v7;
                        while ( 1 )
                        {
                            v9 = *(char *)(v7 - 1);
                            if ( v9 != byte_B0220[v4] )
                                break;
                            ++v4;
                            ++v7;
                            if ( v4 >= v6 )
                            {
                                if ( (v8 & 1) != 0 )
                                    return 0LL;
                                goto LABEL_18;
                            }
                        }
                        if ( (v4 & 0x80u) != 0LL )
                        {
                            out8(0x72u, v4);
                            out8(0x73u, v9);
                        }
                        else
                        {
                            out8(0x70u, v4);
                            out8(0x71u, v9);
                        }
                        byte_B0220[v4++] = *(char *)(v7 - 1);
                        v8 = 0;
                        if ( v4 < v6 )
                            continue;
                        break;
                    }
                LABEL_18:
                    if ( (UINT16)word_B025E != (UINT16)sub_E53F(0LL, (UINT64)&unk_B022E, 48LL) )
                    {
                        out8(0x70u, 0x3Eu);
                        out8(0x71u, word_B025E);
                        out8(0x70u, 0x3Fu);
                        out8(0x71u, HIBYTE(word_B025E));
                    }
                    v10 = (UINT8)word_B0278;
                    v11 = HIBYTE(word_B0278);
                    word_B0278 = 0;
                    v12 = sub_E53F(0LL, (UINT64)&unk_B022E, 242LL);
                    if ( (v11 | (v10 << 8)) != (UINT16)v12 )
                    {
                        v10 = v12 >> 8;
                        out8(0x70u, 0x58u);
                        out8(0x71u, BYTE1(v12));
                        out8(0x70u, 0x59u);
                        out8(0x71u, v12);
                        LOBYTE(v11) = v12;
                    }
                    LOBYTE(word_B0278) = v10;
                    HIBYTE(word_B0278) = v11;
                }
                return 0LL;
            }
        }
    }
    return result;
}

void sub_5DD0(UINT64 a1, UINT64 a2, UINT64 a3)
{
    UINT64 v3; // rax
    
    if ( byte_AE978 == 1 )
    {
        LOBYTE(a2) = 4;
        LOBYTE(a3) = -80;
        v3 = sub_20EDF((UINT64)&byte_AD1D8, a2, a3);
        if ( v3 < 0 )
            DEBUG ((DEBUG_INFO,"#[EB.WL.FIN|!] %r <- EB.WL.WLF\n", v3));
        DEBUG ((DEBUG_INFO,"#[EB|WL:FIN]\n"));
        byte_AE978 = 0;
    }
}

UINT64 sub_1535C(void)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    UINT64 v1; // [rsp+30h] [rbp-10h] BYREF
    char v2; // [rsp+3Fh] [rbp-1h] BYREF
    
    v2 = 0;
    v1 = 1LL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    
    
    Status = GetVariable(
                         L"BlackMode",
                         &gAppleBootVariableGuid,
                         0LL,
                         &v1,
                         &v2);
    if ( !Status )
    {
        if ( v2 )
        {
            BYTE2(qword_B1DE8) |= 1u;
            return SetVariable(
                               L"BlackMode",
                               &gAppleBootVariableGuid,
                               0LL,
                               0LL,
                               0LL);
        }
    }
    return Status;
}

bool sub_4833(void)
{
    void* v0; // rcx
    EFI_STATUS v1; // rax
    UINT64 v2; // rax
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    v0 = qword_AE970;
    if ( !qword_AE970 )
    {
        v1 = LocateProtocol(&qword_ADBB0, NULL, &qword_AE970);
        if ( v1 < 0 )
        {
            DEBUG ((DEBUG_INFO,"#[EB.SB.EN|!] %r <- BS.LocP %g\n", v1, &qword_ADBB0));
            return 0;
        }
        v0 = qword_AE970;
    }
    v2 = (*(UINT64 ( **)(void*, char *))(v0 + 32))(v0, &byte_AD160);
    if ( v2 < 0 )
        DEBUG ((DEBUG_INFO,"#[EB.SB.EN|!] %r <- %g.GP\n", v2, &qword_ADBB0));
    DEBUG ((DEBUG_INFO,"#[EB|SB:P] 0x%X\n", (UINT8)byte_AD160));
    return byte_AD160 != 0;
}

UINT64 sub_11AC8(void)
{
    UINT32 v0; // esi
    void* v1; // rcx
    
    v1 = qword_AEF90;
    if ( !qword_AEF90 )
    {
        v0 = 0;
        EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
        
        if ( LocateProtocol(&gAppleSecureBootProtocolGuid, NULL, &qword_AEF90) < 0 )
            return v0;
        v1 = qword_AEF90;
    }
    (*(void (**)(void*, char *))(v1 + 16))(v1, &byte_AEF98);
    LOBYTE(v0) = byte_AEF98;
    if ( byte_AEF98 )
    {
        if ( byte_AEF98 == -126 )
            byte_AEF88 = 1;
        LOBYTE(v0) = 1;
    }
    return v0;
}

UINT64 sub_11A43(void)
{
    return 0LL;
}

UINT64 sub_4000(void)
{
    UINT64 v0; // rbx
    UINT64 v1; // rax
    UINT64 v2; // rax
    UINT64 v4[5]; // [rsp+30h] [rbp-80h] BYREF
    UINT64 v5; // [rsp+58h] [rbp-58h] BYREF
    UINT64 v6; // [rsp+60h] [rbp-50h] BYREF
    UINT64 v7[2]; // [rsp+68h] [rbp-48h] BYREF
    UINT32 v8; // [rsp+7Ch] [rbp-34h] BYREF
    int v9; // [rsp+80h] [rbp-30h] BYREF
    int v10; // [rsp+84h] [rbp-2Ch] BYREF
    int v11; // [rsp+88h] [rbp-28h] BYREF
    char v12; // [rsp+8Dh] [rbp-23h] BYREF
    char v13; // [rsp+8Eh] [rbp-22h] BYREF
    char v14[33]; // [rsp+8Fh] [rbp-21h] BYREF
    
    v8 = 0;
    v0 = 0LL;
    v5 = 0LL;
    v6 = 0LL;
    v9 = 240;
    v10 = 32786;
    v11 = 1;
    v12 = 1;
    v13 = 0;
    v14[0] = 2;
    memset(v4, 170, 32);
    v7[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v7[1] = 0xAAAAAAAAAAAAAAAAuLL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    if ( GetVariable(
                     L"ApECID",
                     &gAppleSecureBootVariableGuid,
                     &v8,
                     &v5,
                     0LL) != 0x8000000000000005uLL )
    {
        v5 = 16LL;
        if ( GetVariable(
                         L"s",
                         &gAppleVendorVariableGuid,
                         &v8,
                         &v5,
                         v7) >= 0 )
        {
            sub_240B0(&v6, v7, 8LL);
            v1 = SetVariable(
                             L"ApECID",
                             &gAppleSecureBootVariableGuid,
                             6LL,
                             8LL,
                             &v6);
            if ( v1 < 0 )
                return v1;
        }
        sub_E5B0(&v4[2], 16LL);
        sub_240B0(&v4[2], "sha2-384", 9LL);
        sub_E5B0(v4, 16LL);
        sub_240B0(v4, "x86legacyap", 12LL);
        v1 = SetVariable(
                         L"A",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         4LL,
                         &v10);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"ApBoardID",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         4LL,
                         &v9);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"ApSecurityDomain",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         4LL,
                         &v11);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"A",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         1LL,
                         &v12);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"ApSecurityMode",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         1LL,
                         &v12);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"E",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         1LL,
                         &v12);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"E",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         1LL,
                         &v12);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"C",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         1LL,
                         v14);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"MixNMatchPreventionStatus",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         1LL,
                         &v13);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"CryptoDigestMethod",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         16LL,
                         &v4[2]);
        if ( v1 < 0 )
            return v1;
        v1 = SetVariable(
                         L"H",
                         &gAppleSecureBootVariableGuid,
                         6LL,
                         16LL,
                         v4);
        if ( v1 < 0 )
        {
            return v1;
        }
        else
        {
            v2 = SetVariable(
                             L"I",
                             &gAppleSecureBootVariableGuid,
                             6LL,
                             1LL,
                             &v12);
            return v2 & (v2 >> 63);
        }
    }
    return v0;
}

UINT64 sub_2410C(void)
{
    UINT64 v0; // rax
    UINT64 v2; // [rsp+30h] [rbp-10h] BYREF
    
    v2 = 0LL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    if ( !byte_B0350 )
    {
        v2 = 4LL;
        v0 = GetVariable(
                         L"csr-active-config",
                         &gAppleBootVariableGuid,
                         0LL,
                         &v2,
                         &dword_B0354);
        if ( v0 < 0 )
        {
            DEBUG ((DEBUG_INFO,"#[EB.CSR.S|!] %r <- RT.GV %S %g\n", v0, L"csr-active-config", &gAppleBootVariableGuid));
        }
        else
        {
            DEBUG ((DEBUG_INFO,"#[EB.CSR.S|VAR] 0x%08X\n", dword_B0354));
            byte_B0350 = 1;
        }
    }
    return dword_B0354 & 0x40;
}


UINT64 sub_1E045(void)
{
    UINT64 v0; // rax
    UINT64 v1; // rax
    UINT64 v2; // rdx
    UINT64 v3; // rsi
    char v4; // r10
    UINT64 v5; // r9
    char v6; // r12
    UINT64 v7; // r8
    char v8; // r13
    char v9; // r14
    char v10; // r15
    UINT64 i; // rbx
    int v12; // edi
    UINT64 v13; // edi
    char v14; // bl
    UINT64 v15; // rcx
    char v17; // bl
    char v18; // di
    UINT64 v19[9]; // [rsp+20h] [rbp-A0h] BYREF
    void* v20; // [rsp+68h] [rbp-58h] BYREF
    UINT64 v21; // [rsp+70h] [rbp-50h] BYREF
    void* v22; // [rsp+78h] [rbp-48h] BYREF
    UINT16 v23[29]; // [rsp+86h] [rbp-3Ah] BYREF
    
    v20 = (void*)0xAAAAAAAAAAAAAAAAuLL;
    v22 = (void*)0xAAAAAAAAAAAAAAAAuLL;
    
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    if ( LocateProtocol(&unk_ADBC0, 0LL, &v20) >= 0 )
    {
        v19[0] = 0LL;
        (*(void ( **)(void*, UINT64 *))(v20 + 32))(v20, v19);
        if ( v19[0] )
        {
            DEBUG ((DEBUG_INFO,"#[EB|SM]\n"));
            LOBYTE(qword_B1DE8) = qword_B1DE8 | 0x40;
        }
    }
    if ( (qword_B1DE8 & 0x2000) != 0 )
        return 0LL;
    v0 = LocateProtocol(&gAppleKeyMapAggregatorProtocolGuid, 0LL, &v22);
    if ( v0 < 0 )
    {
        v3 = v0;
        DEBUG ((DEBUG_INFO,"#[EB.OPT.PK|!] %r <- BS.LocP %g\n", v0, &gAppleKeyMapAggregatorProtocolGuid));
        v22 = 0LL;
    }
    else
    {
        v23[0] = -21846;
        memset(v19, 170, 64);
        v21 = 32LL;
        v1 = (*(UINT64 ( **)(void*, UINT16 *, UINT64 *, UINT64 *))(v22 + 8))(
                                                                             v22,
                                                                             v23,
                                                                             &v21,
                                                                             v19);
        v3 = v1;
        if ( v1 < 0 )
        {
            DEBUG ((DEBUG_INFO,"#[EB.OPT.PK|!] %r <- %g.GKM\n", v1, &gAppleKeyMapAggregatorProtocolGuid));
        }
        else
        {
            if ( v21 )
            {
                v4 = 0;
                v5 = 0LL;
                v6 = 0;
                v7 = 0LL;
                v8 = 0;
                v9 = 0;
                v10 = 0;
                for ( i = 0LL; i != v21; ++i )
                {
                    v12 = *((UINT16 *)v19 + i);
                    if ( v12 <= 28692 )
                    {
                        if ( (UINT16)v12 == 28678 )
                        {
                            v6 = 1;
                        }
                        else if ( (UINT16)v12 == 28686 )
                        {
                            LOBYTE(v5) = 1;
                        }
                    }
                    else
                    {
                        switch ( (UINT16)v12 )
                        {
                            case 28693:
                                LOBYTE(v7) = 1;
                                break;
                            case 28694:
                                v9 = 1;
                                break;
                            case 28695:
                            case 28696:
                            case 28698:
                                continue;
                            case 28697:
                                v10 = 1;
                                break;
                            case 28699:
                                v8 = 1;
                                break;
                            default:
                                if ( (UINT16)v12 == 28717 )
                                    v4 = 1;
                                break;
                        }
                    }
                }
            }
            else
            {
                v10 = 0;
                v9 = 0;
                v8 = 0;
                v7 = 0LL;
                v6 = 0;
                v5 = 0LL;
                v4 = 0;
            }
            if ( (v23[0] & 0x88) != 0 && (v23[0] & 0xFF77) == 0 && (char)v7 && v21 == 1 )
            {
                v13 = v5;
                v14 = v4;
                DEBUG ((DEBUG_INFO,"#[EB|KP:CmdR]\n"));
                v4 = v14;
                v5 = v13;
                BYTE1(qword_B1DE8) |= 8u;
            }
            if ( (qword_B1DE8 & 0x40) != 0 )
            {
                DEBUG ((DEBUG_INFO,"#[EB|PK:SKIP]\n", v7, v5));
            }
            else
            {
                v15 = v23[0];
                if ( (v23[0] & 0x22) == 0 || ((void)(v2 = v23[0] & 0xFFDD), (v23[0] & 0xFFDD) != 0) )
                {
                    if ( (v23[0] & 0x88) != 0 )
                    {
                        v17 = v4;
                        v18 = v5;
                        if ( v10 )
                        {
                            DEBUG ((DEBUG_INFO,"#[EB|KP:CmdV]\n", v7));
                            LOBYTE(qword_B1DE8) = qword_B1DE8 | 2;
                            sub_22DC7(1LL);
                        }
                        if ( v9 )
                        {
                            if ( v17 )
                            {
                                if ( (UINT32)sub_2410C() )
                                {
                                    DEBUG ((DEBUG_INFO,"#[EB|KP:CmdS_]\n"));
                                    dword_AD244 = 0;
                                }
                            }
                            else
                            {
                                DEBUG ((DEBUG_INFO,"#[EB|KP:CmdS]\n", v7));
                                LOBYTE(qword_B1DE8) = qword_B1DE8 | 0x10;
                            }
                        }
                        if ( v8 )
                        {
                            DEBUG ((DEBUG_INFO,"#[EB|KP:CmdX]\n", v7));
                            BYTE1(qword_B1DE8) |= 4u;
                        }
                        if ( v6 && v17 )
                        {
                            DEBUG ((DEBUG_INFO,"#[EB|KP:CmdC_]\n", v7));
                            byte_AD220 = 0;
                        }
                        if ( v18 )
                        {
                            DEBUG ((DEBUG_INFO,"#[EB|KP:CmdK]\n", v7));
                            BYTE2(qword_B1DE8) |= 2u;
                        }
                    }
                }
                else
                {
                    qword_B1DE8 |= 1uLL;
                    DEBUG ((DEBUG_INFO,"#[EB|KP:Shift]\n", v7, v5));
                    dword_AD244 = 0;
                }
            }
        }
    }
    return v3;
}

void* sub_28E94(UINT64 a1)
{
    VOID* v2; // rdi
    VOID* v4[3]; // [rsp+28h] [rbp-18h] BYREF
    
    EFI_ALLOCATE_POOL AllocatePool = mBootServices->AllocatePool;
    
    EFI_SET_MEM SetMem = mBootServices->SetMem;
    
    
    v4[0] = 0LL;
    AllocatePool(4LL, a1, v4);
    v2 = v4[0];
    if ( v4[0] )
        SetMem(v4[0], a1, 0LL);
    return v2;
}

void* sub_13EE1(char *a1)
{
    UINT64 v2; // rcx
    char *i; // rax
    UINT64 v4; // rdx
    UINT64 v5; // rdi
    UINT64 v6; // rbx
    UINT64 v7; // rcx
    void* v8; // rax
    void* v9; // r14
    void* v10; // rbx
    char v11; // si
    UINT64 v12; // rdi
    UINT64 v13; // rsi
    UINT64 v14; // rax
    UINT64 v15; // rax
    
    v2 = 0LL;
    for ( i = a1; ; i += v4 )
    {
        v4 = *((UINT16 *)i + 1);
        v5 = v4 + v2;
        v6 = ((char)v4 + (char)v2) & 3;
        v7 = 4 - v6;
        if ( !v6 )
            v7 = 0LL;
        v2 = v5 + v7;
        if ( (*i & 0x7F) == 0x7F && (unsigned char)i[1] == 0xFF )
            break;
    }
    v8 = sub_28E94(v2);
    v9 = v8;
    if ( v8 )
    {
        v10 = v8;
        while ( 1 )
        {
            v11 = a1[2];
            v12 = *((UINT16 *)a1 + 1);
            (*(void ( **)(void*, char *, UINT64))(qword_B2098 + 352))(v10, a1, v12);
            v13 = v11 & 3;
            v14 = 4 - v13;
            if ( !v13 )
                v14 = 0LL;
            v15 = v12 + v14;
            *(CHAR16 *)(v10 + 2) = v15;
            *(char *)v10 |= 0x80u;
            if ( (unsigned char)(*a1 & 0x7F) == 0x7F && (unsigned char)a1[1] == 0xFF )
                break;
            v10 += v15;
            a1 += *((UINT16 *)a1 + 1);
        }
    }
    return v9;
}



// 类型 sprintf 功能
CHAR16 * sub_13DF4( CHAR16 *StartOfBuffer, CHAR16* FormatString, ...)
{
#if 0
    UINT64 v4; // rax
    UINT64 v5; // r15
    UINT64 v6; // rdi
    UINT64 v7; // rax
    UINT64 v8; // rbx
    UINT64 v9; // r14
    UINT64 v10; // rax
    UINT64 v11; // rax
    UINT64 v13[3]; // [rsp+20h] [rbp-40h] BYREF
    va_list va; // [rsp+80h] [rbp+20h] BYREF
    
    va_start(va, a2);
    memset(v13, 170, sizeof(v13));
    v4 = sub_28E94(4096LL);
    if ( v4 )
    {
        v5 = v4;
        va_copy((va_list)v13, va);
        sub_28F75(v4, 4096LL, a2);
        v6 = *a1;
        v7 = sub_463D6(v5);
        v8 = v7;
        if ( v6 )
        {
            v8 = sub_463D6(*a1) + v7 - 2;
            v9 = *a1;
            v10 = sub_463D6(*a1);
            v11 = sub_13D7F(v9, v10, v8);
        }
        else
        {
            v11 = sub_28E94(v7);
        }
        *a1 = v11;
        a1[2] = 960LL;
        if ( v8 <= 0x3BF )
        {
            sub_46431(v11, v5);
            a1[1] = v8 - 2;
        }
        (*(void (**)(UINT64))(qword_B2098 + 72))(v5);
    }
    return *a1;
#else
    
    VA_LIST  Marker;
    UINTN    NumberOfPrinted;
    
    VA_START (Marker, FormatString);
    VA_END (Marker);
    
    UINT64 StartOfBuffer_len = StrLen(StartOfBuffer);
    
    UnicodeSPrint(StartOfBuffer + StartOfBuffer_len,0x100,FormatString,Marker);
    return StartOfBuffer;
#endif
}

CHAR16* sub_14517(CHAR16* a1, CHAR16* a2)
{
    UINT64 v4; // rbx
    UINT64 v5; // rsi
    
    sub_13DF4(a1, L"?[ ");
    if ( a2 )
    {
        v4 = *(UINT16 *)(a2 + 2);
        if ( *(CHAR16 *)(a2 + 2) )
        {
            v5 = 0LL;
            do
                sub_13DF4(a1, L"0x%x ", *(UINT8 *)(a2 + v5++));
            while ( v4 != v5 );
        }
    }
    return sub_13DF4(a1, L"]");
}

CHAR16* sub_144DB(CHAR16 *a1)
{
    return sub_13DF4(a1, L",");
}

CHAR16* sub_14583(char* a1)
{
    void* v2; // rsi
    char v3; // al
    CHAR16* ( *v4)(CHAR16*,CHAR16*); // r14
    char *v5; // rcx
    CHAR16* v7; // [rsp+28h] [rbp-58h] BYREF
    
    void* v8; // [rsp+40h] [rbp-40h]
    
    memset(v7, 170, 6);
    
    EFI_SET_MEM SetMem = mBootServices->SetMem;
    
    SetMem(v7, 24LL, 0LL);
    if ( a1 )
    {
        v2 = sub_13EE1(a1);
        v8 = v2;
        while ( 1 )
        {
            v3 = *(char *)v2 & 0x7F;
            if ( v3 == 127 && *(unsigned char *)(v2 + 1) == 0xFF )
                break;
            v4 = (CHAR16* (*)(CHAR16*,CHAR16*))off_AD628;
            if ( off_AD628 )
            {
                v5 = (char *)&unk_AD620;
                while ( v3 != *v5 || *(char *)(v2 + 1) != v5[1] )
                {
                    v4 = (CHAR16* ( *)(CHAR16*,CHAR16*))*((UINT64 *)v5 + 3);
                    v5 += 16;
                    if ( !v4 )
                        goto LABEL_10;
                }
            }
            else
            {
            LABEL_10:
                v4 = 0LL;
            }
            if ( !v4 )
                v4 = sub_14517;
            if ( v4 != (CHAR16* ( *)(CHAR16*,CHAR16*))sub_144DB )
            {
                if ( v7[1] )
                    sub_13DF4(v7, L"/");
            }
            v4(v7, v2);
            v2 += *(UINT16 *)(v2 + 2);
        }
        EFI_FREE_POOL FreePool = mBootServices->FreePool;
        
        FreePool(v8);
    }
    return v7;
}

CHAR16 * sub_4656B(CHAR16 *a1, CHAR16 *a2)
{
    UINT16 v2; // r11
    CHAR16 *v3; // r8
    CHAR16 *v4; // r9
    CHAR16 *v5; // r10
    CHAR16 *result; // rax
    
    v2 = *a1;
    if ( *a1 )
    {
        v3 = a2;
        v4 = a1;
        do
        {
            if ( !*v3 )
                break;
            if ( v2 == *v3 )
            {
                v5 = v4 + 1;
                ++v3;
            }
            else
            {
                v5 = a1 + 1;
                v4 = a1;
                v3 = a2;
                ++a1;
            }
            v2 = v4[1];
            v4 = v5;
        }
        while ( v2 );
    }
    else
    {
        v3 = a2;
    }
    result = 0LL;
    if ( !*v3 )
        return a1;
    return result;
}

void* sub_28B39(EFI_HANDLE a1)
{
    void* v2; // [rsp+20h] [rbp-10h] BYREF
    
    v2 = 0LL;
    
    EFI_HANDLE_PROTOCOL HandleProtocol = mBootServices->HandleProtocol;
    
    
    
    HandleProtocol(a1, &gEfiLoadedImageProtocolGuid, &v2);
    return v2;
}

UINT64 sub_281FE(UINT32* a1, char* a2, UINT64 a3)
{
    UINT64 v3; // r9
    int v4; // eax
    int v5; // r10d
    
    if ( !a3 )
        return 0LL;
    v3 = 0LL;
    while ( 1 )
    {
        v4 = *(UINT8 *)(a1 + v3);
        v5 = *(UINT8 *)(a2 + v3);
        if ( (char)v4 != (char)v5 )
            break;
        if ( a3 == ++v3 )
            return 0LL;
    }
    return (UINT32)(v4 - v5);
}

UINT64 sub_14901(EFI_HANDLE a1, EFI_HANDLE a2, UINT64 a3)
{
    UINT64 v4; // r15
    void* v5; // rax
    UINT64* i; // rbx
    char v7; // al
    UINT64 v9[8]; // [rsp+20h] [rbp-40h] BYREF
    
    v4 = 0x800000000000000EuLL;
    v5 = sub_28B39(a1);
    if ( !v5 )
        return v4;
    for ( i = v5; ; i += *(UINT16 *)(i + 2) )
    {
        v7 = *(char *)i & 0x7F;
        if ( v7 == 4 )
            break;
        if ( v7 == 127 && *(unsigned char *)(i + 1) == 0xFF )
            return v4;
    LABEL_10:
        ;
    }
    if ( *(char *)(i + 1) != 3 )
        goto LABEL_10;
    if ( *(CHAR16 *)(i + 2) != 36 )
        goto LABEL_10;
    v9[0] = 0x49F30B7CBE74FCF7LL;
    v9[1] = 0x42682E04F4014791LL;
    if ( (UINT32)sub_281FE((UINT32*)(i + 4), (char*)v9, 16LL) )
        goto LABEL_10;
    if ( a3 )
        sub_BEBA(i + 20);
    return 0LL;
}
//       StrnCatS (NewString, NewSize/sizeof (CHAR16), SourceString, 1);

void sub_28F54(CHAR16*string,int string_len,char* flagString,CHAR16* append_string){
    char flag = flagString[0];
    switch (flag) {
        case '%':
            memcpy(string, append_string, string_len);
            break;
            
        default:
            break;
    }
}

UINT64 sub_9815(EFI_FILE_PROTOCOL *Root, EFI_FILE_PROTOCOL* *a2)
{
    UINT64 v3; // r12
    UINT64 v4; // rax
    UINT64 v5; // rdi
    bool v6; // r13
    BOOL v7; // esi
    bool v8; // bl
    CHAR16 v10[2056]; // [rsp+30h] [rbp-860h] BYREF
    EFI_FILE_PROTOCOL* v11[3]; // [rsp+838h] [rbp-58h] BYREF
    EFI_FILE_PROTOCOL* *v12; // [rsp+850h] [rbp-40h]
    
    v12 = a2;
    memset(v11, 170, sizeof(v11));
    LOBYTE(a2) = -86;
    sub_E580(v10, (UINT64)a2, 2048LL);
    sub_28F54(v10, 1024LL, "%", L"com.apple.boot.R");
    EFI_FILE_OPEN Open = Root->Open;
    
    v3 = Open(Root, &v11[1], v10, 1LL, 0LL);
    sub_28F54(v10, 1024LL, "%", L"com.apple.boot.P");
    v4 = Open(Root, v11, v10, 1LL, 0LL);
    v5 = (UINT64)v4 >> 63;
    v6 = v4 >= 0;
    sub_28F54(v10, 1024LL, "%", L"com.apple.boot.S");
    v7 = Open(Root, &v11[2], v10, 1LL, 0LL) >= 0;
    DEBUG ((DEBUG_INFO,"#[EB|RPS] %d %d %d\n", v3 >= 0, v6, v7));
    v8 = v3 < 0 || (char)v5 != 0;
    if ( !v8 && v7 )
    {
        *v12 = v11[1];
        LOBYTE(v7) = 1;
    LABEL_11:
        (*(void (**)(void))(v11[0] + 16LL))();
        goto LABEL_12;
    }
    if ( !v8 )
    {
        *v12 = v11[0];
        v6 = 0;
    LABEL_9:
        (*(void (**)(void))(v11[1] + 16LL))();
        goto LABEL_10;
    }
    if ( v3 >= 0 && v7 )
    {
        *v12 = v11[1];
        LOBYTE(v7) = 1;
        goto LABEL_10;
    }
    if ( v6 && v7 )
    {
        LOBYTE(v7) = 0;
        *v12 = v11[2];
        v6 = 1;
        if ( v3 < 0 )
            goto LABEL_11;
        goto LABEL_9;
    }
    if ( v3 >= 0 )
    {
        *v12 = v11[1];
    LABEL_10:
        if ( !v6 )
            goto LABEL_12;
        goto LABEL_11;
    }
    if ( v6 )
    {
        *v12 = v11[0];
    LABEL_12:
        if ( v7 )
            (*(void (**)(void))(v11[2] + 16LL))();
        return 0LL;
    }
    if ( v7 )
    {
        *v12 = v11[2];
        return 0LL;
    }
    return 0x800000000000000EuLL;
}

UINT64  sub_282E1(UINT64 a1, UINT64 a2, UINT64 a3)
{
    UINT64 result; // rax
    UINT64 v4; // rcx
    char v5; // r9
    
    result = a1;
    if ( a3 )
    {
        v4 = 0LL;
        while ( 1 )
        {
            v5 = *(char *)(a2 + v4);
            *(char *)(result + v4) = v5;
            if ( !v5 )
                break;
            if ( a3 == ++v4 )
            {
                a1 = result + v4;
                goto LABEL_6;
            }
        }
    }
    else
    {
    LABEL_6:
        *(char *)(a1 - 1) = 0;
    }
    return result;
}

UINT64 sub_14AF9(
                 EFI_HANDLE a1,
                 EFI_HANDLE a2,
                 EFI_FILE_PROTOCOL *Root,
                 EFI_FILE_PROTOCOL **a4,
                 UINT64 a5,
                 UINT64 a6,
                 char* *a7)
{
    UINT64 v10; // r15
    CHAR16* v11; // rax
    CHAR16 *v12; // rbx
    UINT16 v13; // ax
    CHAR16 *i; // rdx
    CHAR16* v15; // ecx
    char *v16; // rax
    char *v17; // r13
    char* v18; // rax
    UINT32 v20[10]; // [rsp+28h] [rbp-78h] BYREF
    UINTN v21; // [rsp+50h] [rbp-50h] BYREF
    char* v22; // [rsp+58h] [rbp-48h] BYREF
    char* v23; // [rsp+60h] [rbp-40h]
    
    v10 = 0x800000000000000EuLL;
    v22 = (char*)0xAAAAAAAAAAAAAAAAuLL;
    v23 = (char*)0xAAAAAAAAAAAAAAAAuLL;
    if ( sub_14901(a1, a2, 0LL) >= 0 )
    {
        if ( Root )
        {
            memset(v20, 170, sizeof(v20));
            v21 = 40LL;
            EFI_FILE_GET_INFO GetInfo = Root->GetInfo;
            
            if ( GetInfo(Root, &gAppleApfsVolumeInfoGuid, &v21, v20) < 0
                || !v20[0]
                || (v20[5] & 0x10) == 0 )
            {
                return 0x8000000000000003uLL;
            }
        }
        v11 = sub_14583((char*)a2);
        if ( v11 )
        {
            v12 = v11;
            v13 = *v11;
            if ( v13 == 92 || v13 == 47 )
            {
                for ( i = v12; ; v13 = *i )
                {
                    ++i;
                    if ( v13 != 47 && v13 != 92 )
                        break;
                }
                v15 = i - 2;
                while ( v13 && v13 != 47 && v13 != 92 )
                    v13 = *i++;
                *(i - 1) = 0;
                v16 = sub_B526(v15);
                if ( v16 )
                {
                    v17 = v16;
                    if ( sub_1F0E2(v16, (char*)&v22) )
                    {
                        if ( a6 )
                        {
                            sub_282E1(a6, (UINT64)v17, a5);
                            sub_1D327(v17);
                            if ( a7 )
                            {
                                v18 = v22;
                                a7[1] = v23;
                                *a7 = v18;
                            }
                            return 0LL;
                        }
                        
                        EFI_FILE_OPEN Open = Root->Open;
                        
                        v10 = Open(
                                   Root,
                                   a4,
                                   v12,
                                   1LL,
                                   0LL);
                    }
                    sub_1D327(v17);
                }
            }
        }
    }
    return v10;
}

UINT64 sub_14C87(EFI_HANDLE a1, EFI_HANDLE a2, EFI_FILE_PROTOCOL *Root,  EFI_FILE_PROTOCOL **a4)
{
    return sub_14AF9(a1, a2, Root, a4, 0LL, 0LL, 0LL);
}

UINT64 sub_14A6E(EFI_FILE_PROTOCOL *Root)
{
    UINT32 v1; // esi
    UINT64 v3[5]; // [rsp+28h] [rbp-38h] BYREF
    UINTN v4; // [rsp+50h] [rbp-10h] BYREF
    
    memset(v3, 170, sizeof(v3));
    v4 = 40LL;
    v1 = 0;
    EFI_FILE_GET_INFO GetInfo = Root->GetInfo;
    
    if ( Root
        && GetInfo(Root, &gAppleApfsVolumeInfoGuid, &v4, v3) >= 0
        && LODWORD(v3[0]) )
    {
        return HIDWORD(v3[2]);
    }
    return v1;
}

UINT64 sub_463FF(UINT16 *a1, CHAR16 *a2)
{
    UINT16 v2; // ax
    UINT16 *v3; // rcx
    
    v2 = *a1;
    if ( *a1 )
    {
        v3 = a1 + 1;
        while ( v2 == *a2 )
        {
            ++a2;
            v2 = *v3++;
            if ( !v2 )
                goto LABEL_5;
        }
    }
    else
    {
    LABEL_5:
        v2 = 0;
    }
    return v2 - (UINT64)(UINT16)*a2;
}

EFI_FILE_PROTOCOL* sub_9A24(UINT64 a1, UINT64 a2, UINT64 a3, double a4)
{
    UINT64 v4; // rax
    UINT64 v5; // rax
    UINT64 v6; // r14
    char *v7; // rbx
    UINT64 v8; // rdi
    char *v9; // rax
    UINT64 v10; // rsi
    UINT64 v11; // rax
    CHAR16* v12; // rax
    void* i; // rax
    char v14; // cl
    UINT64 v15; // rax
    UINT64 v16; // rdx
    StringStruct2* v17; // rax
    EFI_FILE_PROTOCOL* result; // rax
    EFI_FILE_PROTOCOL* v19; // rsi
    CHAR16 *v20; // rax
    CHAR16 *v21; // rdi
    UINT16 v22; // ax
    CHAR16 *v23; // rax
    CHAR16 *v24; // rcx
    UINT16 v25; // dx
    void* v26; // [rsp+30h] [rbp-60h] BYREF
    void* v27; // [rsp+38h] [rbp-58h] BYREF
    EFI_FILE_PROTOCOL* v28; // [rsp+40h] [rbp-50h] BYREF
    EFI_FILE_PROTOCOL* v29; // [rsp+48h] [rbp-48h] BYREF
    UINT64 v30; // [rsp+50h] [rbp-40h] BYREF
    void* v31; // [rsp+58h] [rbp-38h] BYREF
    EFI_FILE_PROTOCOL* v32[6]; // [rsp+60h] [rbp-30h] BYREF
    v26 = (void*)0xAAAAAAAAAAAAAAAAuLL;
    v31 = (void*)0xAAAAAAAAAAAAAAAAuLL;
    v27 = (void*)0xAAAAAAAAAAAAAAAAuLL;
    v32[0] = (EFI_FILE_PROTOCOL*)0xAAAAAAAAAAAAAAAAuLL;
    EFI_HANDLE_PROTOCOL HandleProtocol = mBootServices->HandleProtocol;
    
    
    v4 = HandleProtocol(
                        qword_B1DD0,
                        &gEfiLoadedImageProtocolGuid,
                        &v31);
    if ( v4 < 0 )
        sub_E617("#[EB.B.OBV|!] %r <- BS.HdlP %g\n", v4, &gEfiLoadedImageProtocolGuid);
    qword_B1DD8 = v31;
    v5 = HandleProtocol(
                        *(EFI_HANDLE *)(v31 + 24),
                        &gEfiDevicePathProtocolGuid,
                        &qword_B1E18);
    v6 = v5;
    if ( v5 < 0 || !qword_B1E18 )
        sub_E617("#[EB.B.OBV|!] %r <- BS.HdlP %g\n", v5, &gEfiDevicePathProtocolGuid);
    DEBUG ((DEBUG_INFO,"#[EB|LIMG:DP] %D\n"));
    DEBUG ((DEBUG_INFO,"#[EB|LIMG:FP] %D\n", *(UINT64 *)(v31 + 32)));
    DEBUG ((DEBUG_INFO,"#[EB|LIMG:OPT] %.*E\n", *(UINT32 *)(v31 + 48), a4));
    v7 = *(char **)(v31 + 56);
    v8 = *(UINT32 *)(v31 + 48);
    if ( *(UINT32 *)(v31 + 48) )
    {
        while ( *(CHAR16 *)v7 == 32 || *(CHAR16 *)v7 == 9 )
        {
            v7 += 2;
            v8 -= 2LL;
            if ( !v8 )
                goto LABEL_10;
        }
    }
    else
    {
    LABEL_10:
        v8 = 0LL;
    }
    v9 = (char *)sub_1D2B1(v8 + 2);
    if ( v9 )
    {
        v10 = (UINT64)v9;
        sub_240D0(v7, v9, v8);
        *(CHAR16 *)(v10 + (v8 & 0xFFFFFFFFFFFFFFFEuLL)) = 0;
        qword_B1DF0 = v10;
    }
    else
    {
        sub_E617("#[EB.B.OBV|!] %r <- EB.B.GA\n", v6);
    }
    sub_12453("Start OpenVolume");
    if ( HandleProtocol(
                        *(EFI_HANDLE *)(v31 + 24),
                        &gEfiBlockIoProtocolGuid,
                        &v26) < 0 )
    {
        v32[0] = 0LL;
        v15 = HandleProtocol(
                             *(EFI_HANDLE *)(v31 + 24),
                             &gEfiLoadFileProtocolGuid,
                             &v27);
        if ( v15 < 0 )
            sub_E617("#[EB.B.OBV|FS/LF!] %r\n", v15);
        DEBUG ((DEBUG_INFO,"#[EB|B:LF]\n"));
        goto LABEL_41;
    }
    v30 = 0xAAAAAAAAAAAAAAAAuLL;
    v11 = (*(UINT64 ( **)(void*, EFI_FILE_PROTOCOL* *))(v26 + 8))(v26, v32);
    if ( v11 < 0 )
        sub_E617("#[EB.B.OBV|!] %r <- %g.OV\n", v11, &gEfiBlockIoProtocolGuid);
    v12 = (CHAR16*)sub_14583(*(char* *)(qword_B1DD8 + 32));
    if ( v12 && sub_4656B(v12, L"com.apple.recovery.boot") )
    {
        DEBUG ((DEBUG_INFO,"#[EB.B.OBV|BM:+ROS]\n"));
        BYTE1(qword_B1DE8) |= 0x10u;
    }
    for ( i = qword_B1E18; ; i += *(UINT16 *)(i + 2) )
    {
        v14 = *(char *)i & 0x7F;
        if ( v14 == 1 )
            break;
        if ( v14 == 127 && *(unsigned char *)(i + 1) == 0xFF )
            goto LABEL_31;
    LABEL_26:
        ;
    }
    if ( *(char *)(i + 1) != 3 )
        goto LABEL_26;
    DEBUG ((DEBUG_INFO,"#[EB.B.OBV|BM:+DMG]\n"));
    DEBUG ((DEBUG_INFO,"#[EB.B.OBV|BM:+ROS]\n"));
    qword_B1DE8 |= 0x801000uLL;
    EFI_FILE_PROTOCOL *Root = v32[0];
    EFI_FILE_OPEN Open = Root->Open;
    
LABEL_31:
    
    if ( Open(
              Root,
              (EFI_FILE_PROTOCOL **)&v30,
              L"System\\Library\\Kernels\\kernel",
              1LL,
              0LL) >= 0 )
        goto LABEL_32;
    v28 = NULL;
    if ( sub_14901(*(EFI_HANDLE *)(v31 + 24), 0LL, 0LL) < 0 )
    {
        if ( sub_9815(Root, &v28) >= 0 )
            goto LABEL_37;
    }
    else
    {
        if ( sub_14C87(*(EFI_HANDLE *)(v31 + 24), *(EFI_HANDLE *)(v31 + 32), v32[0], &v28) >= 0 )
        {
            BYTE2(qword_B1DE8) |= 8u;
        LABEL_37:
            v32[0] = v28;
            LOBYTE(v16) = 1;
            v17 = sub_1207F("/chosen", v16);
            if ( v17 )
                sub_11BA4(v17, "bootroot-active", 0, 0, 0);
            BYTE1(qword_B1DE8) |= 1u;
            goto LABEL_40;
        }
        if ( (sub_14A6E(v32[0]) & 4) != 0 )
            BYTE1(qword_B1DE8) |= 0x10u;
    }
    v29 = 0LL;
    v19 = v32[0];
    v20 = (CHAR16 *)sub_14583(*(char* *)(qword_B1DD8 + 32));
    if ( !v20 )
        goto LABEL_40;
    v21 = v20;
    v22 = *v20;
    if ( v22 != 92 && v22 != 47 )
        goto LABEL_40;
    if ( !sub_463FF(v21, L"\\") || !sub_463FF(v21, L"/") )
    {
    LABEL_57:
        *v21 = 46;
        v23 = v21 + 1;
        goto LABEL_58;
    }
    v23 = 0LL;
    v24 = v21;
    while ( 2 )
    {
        v25 = *v24;
        if ( *v24 == 47 || v25 == 92 )
        {
            v23 = v24;
            goto LABEL_55;
        }
        if ( v25 )
        {
        LABEL_55:
            ++v24;
            continue;
        }
        break;
    }
    if ( !v23 )
        goto LABEL_57;
LABEL_58:
    *v23 = 0;
    if ( (*(UINT64 ( **)(EFI_FILE_PROTOCOL*, EFI_FILE_PROTOCOL* *, CHAR16 *, UINT64, UINT64))(v19 + 8))(
                                                                                                        v19,
                                                                                                        &v29,
                                                                                                        v21,
                                                                                                        1LL,
                                                                                                        0LL) >= 0
        && (*(UINT64 ( **)(EFI_FILE_PROTOCOL*, UINT64 *, const UINT16 *, UINT64, UINT64))(v32[0] + 8))(
                                                                                                       v29,
                                                                                                       &v30,
                                                                                                       L"EncryptedRoot.plist.wipekey",
                                                                                                       1LL,
                                                                                                       0LL) >= 0 )
    {
        DEBUG ((DEBUG_INFO, "#[EB|B:ERPLWK]\n"));
        BYTE1(qword_B1DE8) |= 1u;
        v32[0] = v29;
    LABEL_32:
        (*(void (**)(void))(v30 + 16))();
    }
LABEL_40:
    v27 = 0LL;
LABEL_41:
    sub_12453("End OpenVolume");
    qword_B1E20 = v27;
    result = v32[0];
    qword_B1E28 = v32[0];
    return result;
}

UINT64 sub_9014(void)
{
    UINT64 v5; // r8
    
    UINT64 _RAX = 0x80000000LL;
    UINT64 _RBX;
    UINT64 _RCX;
    asm volatile ("cpuid");
    v5 = 0x8000000000000003uLL;
    if ( (UINT32)_RAX >= 0x80000001 )
    {
        _RAX = 1LL;
        asm volatile ("cpuid");
        
        if ( (_RCX & 0x10D01000) == 0x10D01000 )
        {
            _RAX = 7LL;
            asm volatile ("cpuid");
            if ( (_RBX & 0x128) == 0x128 )
            {
                _RAX = 2147483649LL;
                asm volatile ("cpuid");
                if ( (_RCX & 0x20) != 0 )
                    return 0LL;
            }
        }
    }
    return v5;
}

UINT64 sub_90D9(UINT32 a1)
{
    UINT64 result; // rax
    
    if ( a1 == 3 )
    {
        result = 0LL;
        goto LABEL_6;
    }
    if ( a1 != 8 )
        return 0x8000000000000003uLL;
    result = sub_9014();
    if ( result >= 0 )
        LABEL_6:
        dword_AD218 = a1;
    return result;
}

UINT64 sub_463B0(CHAR16 *a1)
{
    UINT64 v1; // rdx
    UINT64 result; // rax
    
    if ( !a1 || !*a1 )
        return 0LL;
    v1 = 0LL;
    do
        result = v1 + 1;
    while ( a1[++v1] != 0 );
    return result;
}

char sub_1EEDF(UINT64 a1)
{
    char v1; // al
    
    while ( 1 )
    {
        v1 = *(char *)a1 & 0x7F;
        if ( v1 == 3 )
            break;
        if ( v1 == 127 && *(unsigned char *)(a1 + 1) == 0xFF )
            return 0;
    LABEL_6:
        a1 += *(UINT16 *)(a1 + 2);
    }
    if ( *(char *)(a1 + 1) != 11 )
        goto LABEL_6;
    return 1;
}

UINT64 sub_28B07(UINT64 a1)
{
    UINT64 i; // rax
    
    if ( !a1 )
        return 0LL;
    for ( i = a1; (*(unsigned char *)i & 0x7F) != 0x7F || *(unsigned char *)(i + 1) != 0xFF; i += *(UINT16 *)(i + 2) )
        ;
    return i - a1 + 4;
}

void* sub_1DD73(UINT64 a1, UINT16 *a2, char a3)
{
    UINT64 v6; // rsi
    UINT64 v7; // rax
    void* v8; // rax
    void* v9; // r15
    UINT64 v10; // rdi
    CHAR16 *v11; // rsi
    CHAR16 *v12; // r13
    char v13; // al
    UINT16 v14; // cx
    UINT16 *v15; // rdi
    CHAR16 *v16; // rbx
    CHAR16 *v17; // rdx
    UINT16 v18; // cx
    CHAR16 *v19; // rcx
    UINT16 v20; // dx
    UINT16 *v21; // r12
    UINT16 v22; // cx
    
    v6 = sub_28B07(a1);
    v7 = sub_463B0(a2);
    v8 = sub_1D2B1(v6 + 2 * v7 + 2);
    if ( !v8 )
        return 0LL;
    v9 = v8;
    v10 = 0LL;
    v11 = (CHAR16 *)v8;
    v12 = 0LL;
    while ( 1 )
    {
        v13 = *(char *)a1 & 0x7F;
        if ( v13 == 3 )
        {
            if ( *(char *)(a1 + 1) != 12 )
                goto LABEL_10;
        LABEL_9:
            v10 = a1;
            v12 = v11;
            goto LABEL_10;
        }
        if ( v13 != 4 )
            break;
        if ( *(char *)(a1 + 1) == 4 )
            goto LABEL_9;
    LABEL_10:
        sub_240B0(v11, (void*)a1, *(UINT16 *)(a1 + 2));
        a1 += *(UINT16 *)(a1 + 2);
        v11 = (CHAR16 *)((char *)v11 + (UINT16)v11[1]);
    }
    if ( v13 != 127 || *(unsigned char *)(a1 + 1) != 0xFF )
        goto LABEL_10;
    if ( !v10 || !v12 )
    {
        sub_1D327(v9);
        return 0LL;
    }
    *v12 = 1028;
    v14 = *(CHAR16 *)(v10 + 4);
    if ( v14 )
    {
        v15 = (UINT16 *)(v10 + 6);
        v16 = 0LL;
        v17 = v12 + 2;
        do
        {
            *v17 = v14;
            v18 = *(v15 - 1);
            if ( v18 == 92 || v18 == 47 )
                v16 = v17;
            ++v17;
            v14 = *v15++;
        }
        while ( v14 );
    }
    else
    {
        v16 = 0LL;
        v17 = v12 + 2;
    }
    v19 = v16 + 1;
    if ( !v16 )
        v19 = v12 + 2;
    if ( !a3 )
        v19 = v17;
    v20 = *a2;
    if ( *a2 )
    {
        v21 = a2 + 1;
        do
        {
            *v19++ = v20;
            v20 = *v21++;
        }
        while ( v20 );
    }
    *v19 = 0;
    v22 = v19 - (v12 + 4) + 6;
    v12[1] = v22;
    //  *((char *)v12 + v22) = "Row";
    return v9;
}

CHAR16 * sub_1DED3(UINT64 a1, char a2)
{
    UINT16 *v4; // rbx
    UINT16 *v5; // rsi
    UINT64 v6; // r12
    char v7; // al
    void* v8; // rax
    UINT64 v9; // r12
    UINT16 *v10; // rcx
    UINT16 v11; // dx
    UINT16 *v12; // rdi
    CHAR16 *v13; // rbx
    CHAR16 *v14; // rax
    CHAR16 *v15; // rsi
    UINT16 v16; // cx
    char *v17; // rdi
    UINT64 v18; // rsi
    void* v19; // rax
    CHAR16 *v20; // r15
    CHAR16 *i; // rax
    
    v4 = 0LL;
    v5 = 0LL;
    v6 = 0LL;
    while ( 1 )
    {
        v7 = *(char *)a1 & 0x7F;
        if ( v7 != 4 )
            break;
        if ( *(char *)(a1 + 1) == 4 )
        {
            if ( !v4 )
                v4 = (UINT16 *)a1;
            v6 += sub_463B0((CHAR16 *)(a1 + 4));
            v5 = (UINT16 *)a1;
            goto LABEL_12;
        }
    LABEL_10:
        if ( v4 )
            goto LABEL_14;
        v4 = 0LL;
    LABEL_12:
        a1 += *(UINT16 *)(a1 + 2);
    }
    if ( v7 != 127 || *(unsigned char *)(a1 + 1) != 0xFF )
        goto LABEL_10;
    if ( !v4 )
        return 0LL;
LABEL_14:
    if ( v4 == v5 )
    {
        v13 = v4 + 2;
        v9 = 0LL;
        goto LABEL_24;
    }
    v8 = sub_1D2B1(2 * v6 + 2);
    if ( !v8 )
        return 0LL;
    v9 = v6 + 1;
    v10 = (UINT16 *)v8;
    while ( 1 )
    {
        v11 = v4[2];
        if ( v11 )
        {
            v12 = v4 + 3;
            do
            {
                *v10++ = v11;
                v11 = *v12++;
            }
            while ( v11 );
        }
        if ( v4 == v5 )
            break;
        v4 = (UINT16 *)((char *)v4 + v4[1]);
    }
    *v10 = 0;
    v13 = (CHAR16 *)v8;
LABEL_24:
    v14 = 0LL;
    v15 = v13;
    while ( 2 )
    {
        v16 = *v15;
        if ( *v15 == 47 || v16 == 92 )
        {
            v14 = v15;
            goto LABEL_30;
        }
        if ( v16 )
        {
        LABEL_30:
            ++v15;
            continue;
        }
        break;
    }
    v17 = (char *)(v14 + 1);
    if ( !v14 )
        v17 = (char *)v13;
    if ( a2 )
        v17 = (char *)v13;
    v18 = ((char *)v15 - v17 + 2) & 0xFFFFFFFFFFFFFFFEuLL;
    v19 = sub_1D2B1(v18);
    v20 = (CHAR16 *)v19;
    if ( v19 )
    {
        sub_240B0(v19, v17, v18);
        if ( a2 )
        {
            for ( i = v20; ; ++i )
            {
                if ( *i == 47 )
                {
                    *i = 92;
                }
                else if ( !*i )
                {
                    break;
                }
            }
        }
    }
    if ( v9 )
        sub_1D327(v13);
    return v20;
}

UINT64 sub_1A182(UINT64 a1, UINT64 a2, CHAR16 *a3, UINT64 *a4)
{
    double* v4; // xmm2_8
    UINT64 v8; // rax
    UINT64 v9; // r14
    UINT16 v10; // r12
    UINT16 v11; // rsi
    void* v12; // rbx
    UINT64 v13; // rax
    UINT64 v14; // rsi
    UINT64 v15; // rbx
    UINT64 v17[7]; // [rsp+28h] [rbp-38h] BYREF
    
    v17[0] = 0xAAAAAAAAAAAAAAAAuLL;
    if ( a1 )
    {
        DEBUG ((DEBUG_INFO, "#[EB|LD:OLFS] <\"%E\">\n", v4));
        sub_E5B0(a4, 48LL);
        *a4 = a1;
        if ( a3 )
        {
            v8 = sub_463B0(a3);
            v9 = 2 * v8 + 2;
            v10 = 2 * v8 + 6;
            v11 = 2 * v8 + 10;
            v12 = sub_1D2B1(v11);
            sub_E5B0(v12, v11);
            *(CHAR16 *)v12 = 1028;
            *(CHAR16 *)(v12 + 2) = v10;
            sub_240D0((char*)a3, (char *)(v12 + 4), v9);
            //     *(UINT32 *)(v12 + *(UINT16 *)(v12 + 2)) = (UINT32)"Row";
        }
        else
        {
            v12 = 0LL;
        }
        a4[2] = (UINT64)v12;
    }
    else
    {
        v13 = (*(UINT64 (**)(UINT64, UINT64 *, CHAR16 *, UINT64, UINT64))(a2 + 8))(
                                                                                   a2,
                                                                                   v17,
                                                                                   a3,
                                                                                   1LL,
                                                                                   0LL);
        if ( v13 < 0 )
        {
            v15 = v13;
            DEBUG ((DEBUG_INFO, "#[EB.LD.OFS|OPEN!] %r <\"%E\">\n", v13, a3));
            return v15;
        }
        v14 = v17[0];
        sub_E5B0(a4, 48LL);
        a4[1] = v14;
    }
    return 0LL;
}

UINT64 sub_28EEA(UINT64 a1, UINT64 a2)
{
    UINT64 v5[4]; // [rsp+20h] [rbp-20h] BYREF
    
    v5[0] = 0LL;
    (*(void (**)(UINT64, UINT64, UINT64 *))(qword_B2098 + 64))(4LL, a1, v5);
    if ( !v5[0] )
        return 0LL;
    (*(void (**)(UINT64, UINT64, UINT64))(qword_B2098 + 352))(v5[0], a2, a1);
    return v5[0];
}

UINT64 sub_28AD1(UINT64 a1)
{
    UINT64 v1; // rdx
    
    if ( !a1 )
        return 0LL;
    v1 = a1;
    while ( (*(unsigned char *)a1 & 0x7F) != 0x7F || *(unsigned char *)(a1 + 1) != 0xFF )
        a1 += *(UINT16 *)(a1 + 2);
    if ( a1 - v1 == -4 )
        return 0LL;
    else
        return sub_28EEA(a1,v1);
}

UINT64 sub_28E61(UINT64 a1)
{
    UINT64 v2; // [rsp+20h] [rbp-10h] BYREF
    
    v2 = 0LL;
    (*(void (**)(UINT64, UINT64, UINT64 *))(qword_B2098 + 64))(4LL, a1, &v2);
    return v2;
}

UINT64* sub_1D528(void)
{
    UINT64* result; // rax
    UINT64* v1; // rcx
    UINT64* v2; // rdi
    UINT64* v3; // rax
    UINT64 i; // rdx
    
    result = qword_B01F8;
    if ( qword_B01F8 )
    {
        v1 = qword_B01F8 + 24;
    }
    else
    {
        v2 = (UINT64 *)sub_28E61(4096LL);
        if ( !v2 )
            sub_E617("#[EB.M.GT|!] NULL <- EDK.ELAP\n");
        v3 = qword_B01F8;
        for ( i = 0LL; i != 508; i += 4LL )
        {
            v1 = v3;
            v3 = &v2[i];
            v2[i + 3] = (UINT64)v1;
        }
        result = (v2 + 504);
        qword_B01F8 = (v2 + 504);
        v2[508] = (UINT64)v2;
        v2[510] = 0LL;
        v2[509] = 0LL;
        v2[511] = (UINT64)qword_B01F0;
        qword_B01F0 = (UINT64*)(v2 + 508);
    }
    qword_B01F8 = v1;
    *(UINT64 *)(result + 24) = 0LL;
    return result;
}

UINT64 *sub_1D2F0(UINT64 a1)
{
    UINT64* result; // rax
    
    result = (UINT64 *)sub_1D528();
    *result = a1;
    result[2] = 0LL;
    result[1] = 0LL;
    result[3] = (UINT64)qword_B01E8;
    qword_B01E8 = (UINT64**)result;
    return result;
}

UINT64* sub_19E5F(UINT64 a1, UINT64 a2, UINT64 *a3)
{
    UINT64 v6; // rax
    
    DEBUG ((DEBUG_INFO, "#[EB|LD:ODPS]\n"));
    sub_E5B0(a3, 48LL);
    *a3 = a1;
    v6 = sub_28AD1(a2);
    a3[2] = v6;
    return sub_1D2F0(v6);
}

UINT64 sub_19EB0(EFI_FILE_PROTOCOL *File, UINT64 *a2)
{
    UINT64 v4; // rdi
    void* v5; // rax
    void* v6; // rbx
    UINT64 v7; // rax
    void* v8; // rax
    UINT64 v10[5]; // [rsp+28h] [rbp-28h] BYREF
    
    v4 = 0x8000000000000009uLL;
    v10[0] = 208LL;
    v5 = sub_1D2B1(208LL);
    
    EFI_FILE_GET_INFO GetInfo = File->GetInfo;     // 72
    
    if ( v5 )
    {
        v6 = v5;
        v7 = GetInfo(File, &gEfiFileInfoGuid, v10, v5);
        if ( v7 == 0x8000000000000005uLL )
        {
            sub_1D327(v6);
            v8 = sub_1D2B1(v10[0]);
            if ( !v8 )
                return v4;
            v6 = v8;
            v7 = GetInfo(File, &gEfiFileInfoGuid, v10, v8);
        }
        v4 = v7;
        if ( v7 < 0 )
        {
            sub_1D327(v6);
        }
        else
        {
            *a2 = (UINT64)v6;
            return 0LL;
        }
    }
    return v4;
}

UINT64 sub_1A2B1(UINT64 a1, UINT64 *a2)
{
    UINT64 (**v3)(UINT64, UINT64, UINT64, UINT64, UINT64); // rax
    UINT64 v4; // rdx
    UINT64 v5; // rax
    UINT64 *v6; // rbx
    UINT64 v7; // rdi
    EFI_FILE_PROTOCOL* v9; // rcx
    UINT64 v10; // rax
    UINT64 v11; // rcx
    UINT64 v12[4]; // [rsp+30h] [rbp-20h] BYREF
    
    v12[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v3 = *(UINT64 (***)(UINT64, UINT64, UINT64, UINT64, UINT64))a1;
    if ( *(UINT64 *)a1 )
    {
        v4 = *(UINT64 *)(a1 + 40);
        if ( v4 )
        {
            if ( a2 )
                *a2 = v4;
            return 0LL;
        }
        v6 = (UINT64 *)(a1 + 40);
        *(UINT64 *)(a1 + 40) = 0LL;
        // v7 = (*v3)(v3, *(UINT64 *)(a1 + 16), 0LL, a1 + 40, 0LL);
        if ( v7 != 0x8000000000000005uLL )
        {
            DEBUG ((DEBUG_INFO, "#[EB.LD.GSS|LF!] %r\n", v7));
            return v7;
        }
        if ( !a2 )
            return 0LL;
        v5 = *v6;
        goto LABEL_11;
    }
    if ( *(UINT64 *)(a1 + 24) )
    {
        if ( a2 )
        {
            v5 = *(UINT64 *)(a1 + 40);
        LABEL_11:
            *a2 = v5;
        }
    }
    else
    {
        v7 = 0x8000000000000002uLL;
        v9 = *(EFI_FILE_PROTOCOL* *)(a1 + 8);
        if ( !v9 )
            return v7;
        v10 = sub_19EB0(v9, v12);
        if ( v10 < 0 )
            return v10;
        v11 = v12[0];
        if ( !v12[0] )
            return 0x8000000000000009uLL;
        if ( a2 )
            *a2 = *(UINT64 *)(v12[0] + 8);
        sub_1D327((void*)v11);
    }
    return 0LL;
}

UINT64 sub_1A5FA(UINT64 *a1)
{
    UINT64 result; // rax
    UINT64 v3; // rcx
    UINT64 v4; // rcx
    
    result = 0LL;
    a1[5] = 0LL;
    a1[4] = 0LL;
    v3 = a1[2];
    if ( v3 )
    {
        result = sub_1D327((void*)v3);
        v4 = a1[3];
        if ( v4 )
            return sub_1D327((void*)v4);
    }
    else if ( !a1[3] )
    {
        return (*(UINT64 (**)(void))(a1[1] + 16LL))();
    }
    return result;
}

UINT64 sub_1A3A5(UINT64 *a1, UINT64 *a2, char *a3)
{
    char *v3; // r15
    UINT64 v6; // rdi
    UINT64 v7; // rax
    UINT64 v8; // rcx
    UINT64 v9; // rbx
    UINT64 v10; // rcx
    UINT64 v11; // rax
    UINT64 v12; // rsi
    UINT64 v13; // rax
    UINT64 v14; // rsi
    UINT64 v15; // rcx
    UINT64 v16; // rax
    UINT64 v17; // rax
    UINT64 v18; // rbx
    UINT64 v19; // rcx
    UINT64 v20; // rax
    void* v21; // rax
    UINT64 v23[8]; // [rsp+30h] [rbp-40h] BYREF
    
    v3 = a3;
    v6 = *a2;
    v23[0] = 0xAAAAAAAAAAAAAAAAuLL;
    if ( !*a1 )
    {
        v10 = a1[3];
        if ( v10 )
        {
            v11 = a1[4];
            v12 = a1[5] - v11;
            v23[0] = v12;
            if ( v6 <= v12 )
                v12 = v6;
            if ( v12 )
            {
                sub_240D0((char *)(v11 + v10), a3, v12);
                a1[4] += v12;
            }
            *a2 = v12;
            return 0LL;
        }
        v15 = a1[1];
        if ( !v15 )
            return 0x8000000000000002uLL;
        if ( v6 )
        {
            v16 = qword_AD3B0;
            if ( v6 <= qword_AD3B0 )
                v16 = v6;
            v23[0] = v16;
            v14 = (*(UINT64 (**)(UINT64, UINT64 *, char *))(v15 + 32))(v15, v23, a3);
            v17 = v23[0];
            if ( v23[0] )
            {
                v18 = 0LL;
                do
                {
                    v18 += v17;
                    if ( v14 < 0 )
                        break;
                    v6 -= v17;
                    if ( !v6 )
                        break;
                    v3 += v17;
                    v19 = a1[1];
                    v20 = qword_AD3B0;
                    if ( v6 <= qword_AD3B0 )
                        v20 = v6;
                    v23[0] = v20;
                    v14 = (*(UINT64 (**)(UINT64, UINT64 *, char *))(v19 + 32))(v19, v23, v3);
                    v17 = v23[0];
                }
                while ( v23[0] );
                goto LABEL_44;
            }
        }
        else
        {
            v14 = 0LL;
        }
        v18 = 0LL;
    LABEL_44:
        *a2 = v18;
        return v14;
    }
    v7 = sub_1A2B1((UINT64)a1, v23);
    if ( v7 < 0 )
        return v7;
    v8 = a1[3];
    if ( v8 )
    {
        v9 = v23[0] - a1[4];
        v23[0] = v9;
        if ( v6 <= v9 )
            v9 = v6;
        if ( v9 )
        {
        LABEL_7:
            sub_240D0((char *)(a1[4] + v8), v3, v9);
            a1[4] += v9;
            *a2 = v9;
            return 0LL;
        }
    LABEL_38:
        *a2 = 0LL;
        return 0LL;
    }
    v13 = a1[4];
    if ( !v13 && v6 == v23[0] )
        return (*(UINT64 (**)(UINT64, UINT64, UINT64, UINT64 *, char *))*a1)(*a1, a1[2], 0LL, a2, v3);
    v9 = v23[0] - v13;
    v23[0] = v9;
    if ( v6 <= v9 )
        v9 = v6;
    if ( !v9 )
        goto LABEL_38;
    v23[0] = a1[5];
    v21 = sub_1D2B1(v23[0] + 4096LL);
    a1[3] = (UINT64)v21;
    if ( !v3 )
        return 0x8000000000000009uLL;
    v14 = (*(UINT64 (**)(UINT64, UINT64, UINT64, UINT64 *, UINT64*))*a1)(*a1, a1[2], 0LL, v23, v21);
    if ( v23[0] != a1[5] )
        sub_E617("#[EB.LD.RS|SZ!] %qd %qd\n");
    v8 = a1[3];
    if ( v14 >= 0 )
        goto LABEL_7;
    sub_1D327((void*)v8);
    a1[3] = 0LL;
    return v14;
}

UINT64 sub_19F56(UINT64 a1, UINT64 a2, CHAR16* a3, UINT64 a4, UINT64 *a5, CHAR16* *a6)
{
    UINT64 v6; // rbx
    UINT64 v10; // rsi
    char v11; // al
    char v12; // al
    CHAR16 *v13; // rdi
    CHAR16 *v14; // rax
    UINT16 v15; // cx
    UINT16 *v16; // rdx
    UINT64 v17; // rcx
    char v18; // dl
    UINT64 v19; // rax
    UINT64 v21[11]; // [rsp+38h] [rbp-58h] BYREF
    
    v6 = a4;
    memset(v21, 170, 56);
    DEBUG ((DEBUG_INFO, "#[EB.LD.LF|IN] %d %d <\"%E\"> <\"%d\">\n", a1 != 0, a2 != 0, *(double *)&a3, a4 != 0));
    if ( a1 || !v6 )
    {
        if ( !v6 )
        {
            v10 = sub_1A182(a1, a2, a3, v21);
            v13 = 0LL;
            goto LABEL_22;
        }
        sub_19E5F(a1, v6, v21);
        v13 = 0LL;
    LABEL_23:
        v10 = sub_1A2B1((UINT64)v21, &v21[6]);
        if ( v10 >= 0 && v21[6] )
        {
            v19 = (UINT64)sub_1D2B1(v21[6]);
            if ( !v19 )
            {
                sub_1A5FA(v21);
                return 0x8000000000000009uLL;
            }
            v13 = (CHAR16 *)v19;
            v10 = sub_1A3A5(v21, &v21[6], (char*)v19);
        }
        sub_1A5FA(v21);
        if ( v10 < 0 )
        {
            if ( v13 )
                sub_1D327(v13);
        }
        else
        {
            *a5 = v21[6];
            *a6 = v13;
        }
        return v10;
    }
    v10 = 0x8000000000000003uLL;
    if ( !a2 )
        return v10;
    while ( 1 )
    {
        v11 = *(char *)v6 & 0x7F;
        if ( v11 == 127 )
            break;
        if ( v11 == 4 || *(char *)(v6 + 1) == 4 )
            goto LABEL_13;
    LABEL_10:
        v6 += *(UINT16 *)(v6 + 2);
    }
    v12 = *(char *)(v6 + 1);
    if ( v12 == -1 )
    {
        DEBUG ((DEBUG_INFO, "#[EB.LD.LF|!FP]\n"));
        return v10;
    }
    if ( v12 != 4 )
        goto LABEL_10;
LABEL_13:
    v21[6] = sub_28B07(v6);
    v14 = (CHAR16 *)sub_1D2B1(v21[6]);
    if ( !v14 )
        return 0x8000000000000009uLL;
    v13 = v14;
    if ( (*(char *)v6 & 0x7F) == 4 )
    {
        do
        {
            if ( *(char *)(v6 + 1) != 4 )
                break;
            v15 = *(CHAR16 *)(v6 + 4);
            if ( v15 )
            {
                v16 = (UINT16 *)(v6 + 6);
                do
                {
                    *v14++ = v15;
                    v15 = *v16++;
                }
                while ( v15 );
            }
            v17 = *(UINT16 *)(v6 + 2);
            v18 = *(char *)(v6 + v17);
            v6 += v17;
        }
        while ( (v18 & 0x7F) == 4 );
    }
    *v14 = 0;
    v10 = sub_1A182(0LL, a2, v13, v21);
    sub_1D327(v13);
LABEL_22:
    if ( v10 >= 0 )
        goto LABEL_23;
    return v10;
}

UINT64 sub_1E3E8(UINT64 a1, UINT64 a2, UINT16 *a3, UINT64 *a4, CHAR16** a5)
{
    double v5; // xmm2_8
    void* v10; // rax
    void* v11; // rbx
    UINT64 v12; // rsi
    UINT64 v13; // rax
    
    DEBUG ((DEBUG_INFO, "#[EB.OPT.LXF|F] <\"%E\">\n", v5));
    if ( a1 )
    {
        v10 = sub_1DD73(*(UINT64 *)(qword_B1DD8 + 32), a3, 1);
        if ( v10 )
        {
            v11 = v10;
            v12 = sub_19F56(a1, 0, 0, (UINT64)v10, a4, a5);
            sub_1D327(v11);
            goto LABEL_7;
        }
        v13 = sub_19F56(a1, a2, a3, 0, a4, a5);
    }
    else
    {
        v13 = sub_19F56(0, a2, a3, 0, a4, a5);
    }
    v12 = v13;
LABEL_7:
    if ( v12 < 0 )
    {
        DEBUG ((DEBUG_INFO, "#[EB.OPT.LXF|LF!] %r\n", v12));
    }
    else if ( *a4 )
    {
        return 0LL;
    }
    else
    {
        DEBUG ((DEBUG_INFO, "#[EB.OPT.LXF|LF0]\n"));
        return 0x800000000000000EuLL;
    }
    return v12;
}

char *sub_8940(char *a1, char a2, UINT64 a3)
{
    char *result; // rax
    
    result = 0LL;
    if ( a1 && a3 )
    {
        result = 0LL;
        while ( *a1 != a2 )
        {
            ++a1;
            if ( !--a3 )
                return result;
        }
        return a1;
    }
    return result;
}

UINT64 sub_88BB(UINT64 a1, char *a2, UINT64 a3)
{
    UINT64 result; // rax
    UINT64 v4; // rdi
    UINT64 v5; // rbx
    UINT64 v7; // r8
    UINT64 v8; // r10
    UINT64 v9; // r11
    UINT64 v10; // rdi
    char v11; // r14
    char v12; // bl
    
    if ( !a2 || !*a2 )
        return a1;
    result = 0LL;
    if ( a1 && a3 )
    {
        result = 0LL;
        v4 = 0LL;
        while ( 1 )
        {
            v5 = v4 + 1;
            if ( v4 + 1 > a3 )
                break;
            if ( a2[++v4] == 0 )
            {
                v7 = a3 - v5;
                result = 0LL;
                v8 = a1;
                v9 = 0LL;
                do
                {
                    v10 = 0LL;
                    v11 = *a2;
                    while ( 1 )
                    {
                        v12 = *(char *)(v8 + v10);
                        if ( !v12 || v12 != v11 )
                            break;
                        v11 = a2[++v10];
                        if ( !v11 )
                            return a1 + v9;
                    }
                    ++v9;
                    ++v8;
                }
                while ( v9 <= v7 );
                return result;
            }
        }
    }
    return result;
}

UINT64 sub_13A6A(UINT64 *a1, UINT64* a2)
{
    UINT64 v4; // rax
    UINT64 v5; // rcx
    UINT64 v6; // rdx
    UINT64 v7; // rbx
    bool v8; // cf
    UINT64 v9; // rax
    UINT64 v10; // r8
    UINT64 v11; // rax
    char *v12; // rcx
    
    v4 = a1[2];
    v5 = a1[4];
    v6 = v5 - a1[1];
    LODWORD(v7) = 0;
    v8 = v4 < v6;
    v9 = v4 - v6;
    v10 = 0LL;
    if ( !v8 )
        v10 = v9;
    v11 = sub_88BB(v5, ">", v10);
    if ( v11 )
    {
        v7 = a1[3];
        v12 = (char *)a1[4];
        *(UINT64 *)(a2 + 8) = v7;
        *(UINT32 *)a2 = 14;
        *(UINT64 *)(a2 + 16) = (UINT64)v12;
        *(UINT64 *)(a2 + 24) = v11 - (UINT64)v12;
        if ( *v12 == 47 )
        {
            *(UINT32 *)a2 = 15;
            *(UINT64 *)(a2 + 16) = (UINT64)(v12 + 1);
        }
        else
        {
            if ( *(char *)(v11 - 1) != 47 )
            {
            LABEL_9:
                a1[4] = v11 + 1;
                LOBYTE(v7) = 1;
                return (UINT32)v7;
            }
            *(UINT32 *)a2 = 16;
        }
        *(UINT64 *)(a2 + 24) = v11 - (UINT64)v12 - 1;
        goto LABEL_9;
    }
    return (UINT32)v7;
}


UINT64 sub_13B0B(UINT32 *a1)
{
    UINT64 result; // rax
    
    result = *a1;
    switch ( (UINT32)result )
    {
        case 0x10:
            if ( *((UINT64 *)a1 + 3) < 4uLL )
                return result;
            result = sub_281FE((a1 + 2), "dict", 4LL);
            if ( !(UINT32)result )
            {
                *a1 = 8;
                return result;
            }
            result = *((UINT64 *)a1 + 3);
            if ( result >= 5 )
            {
                result = sub_281FE((a1 + 2), "array", 5LL);
                if ( !(UINT32)result )
                {
                    *a1 = 11;
                    return result;
                }
                result = *((UINT64 *)a1 + 3);
            }
            if ( result >= 4 )
            {
                result = sub_281FE((a1 + 2), "true", 4LL);
                if ( (UINT32)result )
                {
                    if ( *((UINT64 *)a1 + 3) >= 5uLL )
                    {
                        result = sub_281FE((a1 + 2), "false", 5LL);
                        if ( !(UINT32)result )
                            *a1 = 18;
                    }
                }
                else
                {
                    *a1 = 17;
                }
            }
            break;
        case 0xF:
            result = *((UINT64 *)a1 + 3);
            if ( result >= 5 )
            {
                result = sub_281FE((a1 + 2), "plist", 5LL);
                if ( !(UINT32)result )
                {
                    *a1 = 5;
                    return result;
                }
                result = *((UINT64 *)a1 + 3);
            }
            if ( result >= 4 )
            {
                result = sub_281FE((a1 + 2), "dict", 4LL);
                if ( !(UINT32)result )
                {
                    *a1 = 7;
                    return result;
                }
                result = *((UINT64 *)a1 + 3);
            }
            if ( result >= 3 )
            {
                result = sub_281FE((a1 + 2), "key", 3LL);
                if ( (UINT32)result )
                {
                    if ( *((UINT64 *)a1 + 3) >= 5uLL )
                    {
                        result = sub_281FE((a1 + 2), "array", 5LL);
                        if ( !(UINT32)result )
                            *a1 = 10;
                    }
                }
                else
                {
                    *a1 = 13;
                }
            }
            break;
        case 0xE:
            result = *((UINT64 *)a1 + 3);
            if ( result >= 5 )
            {
                result = sub_281FE((a1 + 2), "plist", 5LL);
                if ( !(UINT32)result )
                {
                    *a1 = 4;
                    return result;
                }
                result = *((UINT64 *)a1 + 3);
            }
            if ( result >= 4 )
            {
                result = sub_281FE((a1 + 2), "dict", 4LL);
                if ( !(UINT32)result )
                {
                    *a1 = 6;
                    return result;
                }
                result = *((UINT64 *)a1 + 3);
            }
            if ( result >= 3 )
            {
                result = sub_281FE((a1 + 2), "key", 3LL);
                if ( (UINT32)result )
                {
                    if ( *((UINT64 *)a1 + 3) >= 5uLL )
                    {
                        result = sub_281FE((a1 + 2), "array", 5LL);
                        if ( !(UINT32)result )
                            *a1 = 9;
                    }
                }
                else
                {
                    *a1 = 12;
                }
            }
            break;
        default:
            return result;
    }
    return result;
}

UINT64 sub_8D39(UINT64 a1, const char *a2, ...)
{
    return 0LL;
}

UINT64 sub_131DC(UINT32 *a1, UINT64 a2)
{
    char *v4; // rax
    char *v5; // rcx
    UINT64 v6; // rbx
    UINT64 v7; // rdx
    UINT64 v8; // rdi
    int v9; // eax
    int v10; // esi
    char *v11; // rax
    int v12; // edi
    char *v13; // rcx
    bool v14; // cf
    UINT64 v15; // rdx
    UINT64 v16; // rcx
    UINT64 v17; // rax
    UINT64 v18; // rdx
    UINT64 v19; // rax
    UINT64 v20; // r8
    UINT64 v21; // rax
    UINT64 v22; // rcx
    UINT64 v23; // rax
    UINT64 v24; // rdx
    char *v25; // rcx
    char *v26; // rax
    UINT64 v27; // rdx
    UINT64 v28; // r8
    UINT64 v29; // rax
    UINT64 v30; // rdx
    char *v31; // rcx
    char *v32; // rax
    UINT64 v33; // rdx
    UINT64 v34; // r8
    char* v35; // rax
    UINT64 v36; // rdx
    char *v37; // rcx
    UINT64 v38; // rdx
    UINT64 v39; // rcx
    UINT64 v40; // rax
    UINT64 v41; // rcx
    UINT64 v42; // rdx
    UINT64 v43; // rax
    UINT64 v44; // r8
    UINT64 v45; // rax
    UINT64 v46; // rcx
    UINT64 v47; // rax
    UINT64 v48; // rdx
    UINT64 v49; // rax
    UINT64 v50; // rdx
    UINT64 v52; // rax
    UINT64 v53; // rcx
    UINT64 v54; // rdx
    UINT64 v55; // rax
    UINT64 v56; // r8
    char* v57; // rax
    
    *(UINT32 *)a2 = 0;
    v4 = (char *)*((UINT64 *)a1 + 4);
    v5 = v4 + 1;
    v6 = *((UINT64 *)a1 + 1);
    v7 = *((UINT64 *)a1 + 2);
    v8 = v6 + v7;
    if ( (UINT64)(v4 + 1) >= v6 + v7 )
    {
    LABEL_60:
        LODWORD(v6) = 0;
        return (UINT32)v6;
    }
    while ( 1 )
    {
        v9 = *v4;
        *((char *)a1 + 40) = v9;
        *((UINT64 *)a1 + 4) = (UINT64)v5;
        v10 = *a1;
        if ( !*a1 )
            break;
        if ( v10 == 1 )
        {
            if ( (char)v9 != 60 )
            {
                v31 = v5 - 1;
                *((UINT64 *)a1 + 4) = (UINT64)v31;
                v32 = &v31[-v6];
                LODWORD(v6) = 0;
                v14 = v7 < (UINT64)v32;
                v33 = v7 - (UINT64)v32;
                v34 = 0LL;
                if ( !v14 )
                    v34 = v33;
                v35 = sub_8940(v31, 60LL, v34);
                if ( v35 )
                {
                    v36 = *((UINT64 *)a1 + 4);
                    LODWORD(v6) = (UINT64)(v35 - v36) & 0xffffffff;
                    *(UINT64 *)(a2 + 8) = *((UINT64 *)a1 + 3);
                    *(UINT32 *)a2 = 19;
                    *(UINT64 *)(a2 + 16) = v36;
                    *(UINT64 *)(a2 + 24) = (UINT64)(v35 - v36);
                    goto LABEL_44;
                }
                return (UINT32)v6;
            }
            v14 = v7 < (UINT64)&v5[-v6];
            v24 = v7 - (UINT64)&v5[-v6];
            if ( v14 )
                v24 = 0LL;
            if ( v24 < 9 || (UINT32)sub_281FE((UINT32*)v5, "![CDATA[", 8LL) )
            {
                if ( (UINT8)sub_13A6A((UINT64 *)a1, (UINT64*)a2) )
                    sub_13B0B((UINT32*)a2);
                LOBYTE(v6) = 1;
                if ( (*(UINT32 *)a2 | 2) == 0xF )
                    *a1 = 0;
                return (UINT32)v6;
            }
            *a1 = 2;
            v5 = (char *)(*((UINT64 *)a1 + 4) + 8LL);
            goto LABEL_26;
        }
        if ( v10 == 2 )
        {
            v25 = v5 - 1;
            *((UINT64 *)a1 + 4) = (UINT64)v25;
            v26 = &v25[-v6];
            LODWORD(v6) = 0;
            v14 = v7 < (UINT64)v26;
            v27 = v7 - (UINT64)v26;
            v28 = 0LL;
            if ( !v14 )
                v28 = v27;
            v29 = sub_88BB((UINT64)v25, "]]>", v28);
            if ( v29 )
            {
                v30 = *((UINT64 *)a1 + 4);
                LODWORD(v6) = (v29 - v30) & 0xffffffff;
                *(UINT64 *)(a2 + 8) = *((UINT64 *)a1 + 3);
                *(UINT32 *)a2 = 20;
                *(UINT64 *)(a2 + 16) = v30;
                *(UINT64 *)(a2 + 24) = v29 - v30;
                *((UINT64 *)a1 + 4) = v29 + 3;
                *a1 = 1;
                goto LABEL_45;
            }
            return (UINT32)v6;
        }
    LABEL_27:
        v4 = v5++;
        v6 = *((UINT64 *)a1 + 1);
        v7 = *((UINT64 *)a1 + 2);
        v8 = v6 + v7;
        if ( (UINT64)v5 >= v6 + v7 )
            goto LABEL_60;
    }
    if ( v9 <= 12 )
    {
        if ( v9 != 9 )
        {
            if ( v9 != 10 )
            {
            LABEL_46:
                LODWORD(v6) = 0;
                sub_8D39(0LL, "invalid syntax on line %lld", *((UINT64 *)a1 + 3));
                return (UINT32)v6;
            }
            ++*((UINT64 *)a1 + 3);
        }
        goto LABEL_27;
    }
    if ( v9 == 13 || v9 == 32 )
        goto LABEL_27;
    if ( v9 != 60 )
        goto LABEL_46;
    v11 = v5 + 1;
    if ( (UINT64)(v5 + 1) >= v8 )
        goto LABEL_27;
    v12 = *v5;
    *((char *)a1 + 40) = *v5;
    *((char* *)a1 + 4) = v11;
    if ( v12 != 33 )
    {
        if ( v12 == 63 )
        {
            v37 = &v11[-v6];
            LODWORD(v6) = 0;
            v14 = v7 < (UINT64)v37;
            v38 = v7 - (UINT64)v37;
            v39 = 0LL;
            if ( !v14 )
                v39 = v38;
            if ( v39 >= 6 )
            {
                if ( (UINT32)sub_281FE((UINT32*)v11, "xml ", 4LL) )
                    goto LABEL_60;
                v40 = *((UINT64 *)a1 + 2);
                v41 = *((UINT64 *)a1 + 4) + 4LL;
                *((UINT64 *)a1 + 4) = v41;
                v42 = v41 - *((UINT64 *)a1 + 1);
                LODWORD(v6) = 0;
                v14 = v40 < v42;
                v43 = v40 - v42;
                v44 = 0LL;
                if ( !v14 )
                    v44 = v43;
                v45 = sub_88BB(v41, "?>", v44);
                if ( v45 )
                {
                    v46 = *((UINT64 *)a1 + 4);
                    *(UINT32 *)a2 = 1;
                    v6 = *((UINT64 *)a1 + 3);
                    *(UINT64 *)(a2 + 8) = v6;
                    *(UINT64 *)(a2 + 16) = v46;
                    *(UINT64 *)(a2 + 24) = v45 - v46;
                    v35 = (char*)(v45 + 2);
                    goto LABEL_44;
                }
            }
        }
        else
        {
            *((char* *)a1 + 4) = v5;
            if ( (UINT8)sub_13A6A((UINT64*)a1, (UINT64*)a2) )
                sub_13B0B((UINT32*)a2);
            LOBYTE(v6) = 1;
            if ( (*(UINT32 *)a2 | 2) == 0xE )
                *a1 = 1;
        }
        return (UINT32)v6;
    }
    v13 = &v11[-v6];
    LODWORD(v6) = 0;
    v14 = v7 < (UINT64)v13;
    v15 = v7 - (UINT64)v13;
    v16 = 0LL;
    if ( !v14 )
        v16 = v15;
    if ( v16 < 5 )
        return (UINT32)v6;
    if ( !(UINT32)sub_281FE((UINT32*)v11, "--", 2LL) )
    {
        v17 = *((UINT64 *)a1 + 2);
        v18 = *((UINT64 *)a1 + 4) - *((UINT64 *)a1 + 1);
        LODWORD(v6) = 0;
        v14 = v17 < v18;
        v19 = v17 - v18;
        v20 = 0LL;
        if ( !v14 )
            v20 = v19;
        v21 = sub_88BB(*((UINT64 *)a1 + 4), "-->", v20);
        if ( !v21 )
            return (UINT32)v6;
        v22 = v21;
        *(UINT32 *)a2 = 3;
        *(UINT64 *)(a2 + 8) = *((UINT64 *)a1 + 3);
        v23 = *((UINT64 *)a1 + 4) + 2LL;
        *(UINT64 *)(a2 + 16) = v23;
        *(UINT64 *)(a2 + 24) = v22 - v23;
        v5 = (char *)(v22 + 3);
    LABEL_26:
        *((char* *)a1 + 4) = v5;
        goto LABEL_27;
    }
    v47 = *((UINT64 *)a1 + 2);
    v48 = *((UINT64 *)a1 + 4) - *((UINT64 *)a1 + 1);
    LODWORD(v6) = 0;
    v14 = v47 < v48;
    v49 = v47 - v48;
    v50 = 0LL;
    if ( !v14 )
        v50 = v49;
    if ( v50 >= 0xE )
    {
        if ( (UINT32)sub_281FE((a1 + 4), "DOCTYPE plist", 13LL) )
            goto LABEL_60;
        v52 = *((UINT64 *)a1 + 2);
        v53 = *((UINT64 *)a1 + 4) + 13LL;
        *((UINT64 *)a1 + 4) = v53;
        v54 = v53 - *((UINT64 *)a1 + 1);
        LODWORD(v6) = 0;
        v14 = v52 < v54;
        v55 = v52 - v54;
        v56 = 0LL;
        if ( !v14 )
            v56 = v55;
        v57 = (char*)sub_88BB(v53, ">", v56);
        if ( v57 )
        {
            *(UINT32 *)a2 = 2;
            *(UINT64 *)(a2 + 8) = *((UINT64 *)a1 + 3);
            *(UINT64 *)(a2 + 16) = 0LL;
            v35 = v57 + 1;
        LABEL_44:
            *((char* *)a1 + 4) = v35;
        LABEL_45:
            LOBYTE(v6) = 1;
        }
    }
    return (UINT32)v6;
}

void sub_2118C(UINT64 a1)
{
    if ( a1 )
        ++*(UINT64 *)(a1 + 8);
}

UINT64 sub_8D41(UINT64 a1)
{
    sub_2118C(a1);
    return a1;
}

void sub_21081(UINT64 a1)
{
    UINT64 v2; // rax
    UINT64 v3; // rcx
    UINT64 v4; // rcx
    UINT64 v5; // rcx
    
    if ( a1 )
    {
        v2 = *(UINT64 *)(a1 + 8);
        if ( v2 < 2 )
        {
            switch ( *(UINT32 *)a1 )
            {
                case 2:
                    while ( 1 )
                    {
                        v3 = *(UINT64 *)(a1 + 16);
                        if ( !v3 || *(UINT32 *)v3 != 3 )
                            break;
                        *(UINT64 *)(a1 + 16) = *(UINT64 *)(v3 + 32);
                        sub_21081(v3);
                    }
                    break;
                case 3:
                    sub_1D327((void*)(a1 + 16));
                    v5 = *(UINT64 *)(a1 + 24);
                    goto LABEL_13;
                case 4:
                    goto LABEL_15;
                case 6:
                    if ( *(char *)(a1 + 32) )
                        LABEL_15:
                        sub_1D327((void*)(a1 + 16));
                    break;
                case 0xA:
                    while ( 1 )
                    {
                        v4 = *(UINT64 *)(a1 + 16);
                        if ( !v4 || *(UINT32 *)v4 != 13 )
                            break;
                        *(UINT64 *)(a1 + 16) = *(UINT64 *)(v4 + 24);
                        sub_21081(v4);
                    }
                    break;
                case 0xD:
                    v5 = *(UINT64 *)(a1 + 16);
                LABEL_13:
                    sub_21081(v5);
                    break;
                default:
                    break;
            }
            sub_E580((CHAR16*)a1, 0LL, 40LL);
            *(UINT64 *)(a1 + 24) = qword_B0328;
            --qword_B2070;
            qword_B0328 = a1;
        }
        else
        {
            *(UINT64 *)(a1 + 8) = v2 - 1;
        }
    }
}

UINT64 sub_8D5C(UINT64 a1)
{
    sub_21081(a1);
}

UINT64 sub_13683(UINT64 *a1)
{
    UINT64 *v1; // rsi
    UINT64 result; // rax
    
    v1 = (UINT64 *)*a1;
    if ( *a1 )
    {
        if ( *v1 )
            sub_8D5C(*v1);
        *a1 = v1[2];
        return sub_1D327(v1);
    }
    return result;
}

UINT64 *sub_211B9(void)
{
    UINT64 *v0; // rsi
    void* v1; // rax
    UINT64 *v2; // rax
    UINT64 v3; // rcx
    
    v0 = (UINT64 *)qword_B0328;
    if ( !qword_B0328 )
    {
        UINT64 loc_27FFC = 0x27FFC;
        v1 = sub_1D2B1(loc_27FFC + 4);
        if ( !v1 )
            return 0LL;
        v0 = (UINT64 *)v1;
        sub_E580(v1, 0LL, loc_27FFC + 4);
        v2 = v0 + 5;
        v3 = 4096LL;
        do
        {
            *(v2 - 2) = (UINT64)v2;
            v2 += 5;
            --v3;
        }
        while ( v3 );
        v0[20478] = 0LL;
        qword_B0328 = (UINT64)v0;
        qword_B2068 += 4096LL;
    }
    qword_B0328 = v0[3];
    sub_E580((CHAR16*)v0, 0LL, 40LL);
    v0[1] = 1LL;
    ++qword_B2070;
    return v0;
}

UINT32 *sub_2119B(void)
{
    UINT32 *result; // rax
    
    result = (UINT32 *)sub_211B9();
    if ( result )
        *result = 2;
    return result;
}

UINT64 sub_4650B(char *a1)
{
    UINT64 v1; // rdx
    UINT64 result; // rax
    
    if ( !*a1 )
        return 1LL;
    v1 = 1LL;
    do
        result = v1 + 1;
    while ( a1[v1++] != 0 );
    return result;
}

char * sub_464A4(char *a1, char *a2)
{
    char v3; // cl
    char *v4; // rdx
    
    v3 = *a2;
    if ( *a2 )
    {
        v4 = a2 + 1;
        do
        {
            *a1++ = v3;
            v3 = *v4++;
        }
        while ( v3 );
    }
    *a1 = 0;
    return a1 + 1;
}

UINT64 sub_8CF8(char* a1)
{
    UINT64 v2; // rax
    void* v3; // rax
    UINT64 v4; // rsi
    
    if ( !a1 )
        return 0LL;
    v2 = sub_4650B(a1);
    v3 = sub_1D2B1(v2);
    if ( !v3 )
        return 0LL;
    v4 = (UINT64)v3;
    sub_464A4(v3, a1);
    return v4;
}

UINT64 *sub_2125E(UINT64 a1, UINT64 a2)
{
    UINT64 *result; // rax
    UINT64 v4; // rax
    UINT64 v5; // rdi
    
    result = 0LL;
    if ( a1 && a2 )
    {
        if ( (UINT32)(*(UINT32 *)a2 - 2) <= 9 )
        {
            v4 = sub_8CF8((char*)a1);
            if ( v4 )
            {
                v5 = v4;
                result = sub_211B9();
                if ( result )
                {
                    *(UINT32 *)result = 3;
                    result[2] = v5;
                    result[3] = a2;
                    ++*(UINT64 *)(a2 + 8);
                    return result;
                }
                sub_1D327((void*)v5);
            }
        }
        return 0LL;
    }
    return result;
}

UINT64 sub_21360(UINT64 a1, UINT64 a2)
{
    UINT64 result; // rax
    UINT64 v3; // r9
    UINT64 *v4; // rax
    
    result = 0x8000000000000002uLL;
    if ( a1 && a2 && *(UINT32 *)a1 == 2 && *(UINT32 *)a2 == 3 )
    {
        v3 = *(UINT64 *)(a1 + 24);
        v4 = (UINT64 *)(v3 + 32);
        if ( !v3 )
            v4 = (UINT64 *)(a1 + 16);
        *v4 = a2;
        *(UINT64 *)(a1 + 24) = a2;
        ++*(UINT64 *)(a2 + 8);
        return 0LL;
    }
    return result;
}

void sub_8E2D(UINT64 a1, UINT64 a2, UINT64 a3)
{
    UINT64* v4; // rax
    UINT64* v5; // rdi
    
    v4 = sub_2125E(a2, a3);
    if ( v4 )
    {
        v5 = v4;
        sub_21360(a1, (UINT64)v4);
        sub_21081((UINT64)v5);
    }
}

UINT64 sub_8EB9(UINT64 a1, UINT64 a2, UINT64 a3)
{
    UINT32* v6; // rax
    UINT64 v7; // rsi
    UINT64 v8; // rdi
    
    v6 = sub_2119B();
    v7 = (UINT64)v6;
    if ( a3 && a2 && a1 && v6 )
    {
        v8 = 0LL;
        do
        {
            sub_8E2D(v7, *(UINT64 *)(a1 + 8 * v8), *(UINT64 *)(a2 + 8 * v8));
            ++v8;
        }
        while ( a3 != v8 );
    }
    return v7;
}

UINT64 *sub_212D9(void)
{
    UINT64 *result; // rax
    
    result = sub_211B9();
    if ( result )
        *(UINT32 *)result = 10;
    return result;
}

UINT64 sub_212F7(UINT64 *a1, UINT64 a2)
{
    UINT64 v2; // rdi
    UINT64 *v5; // rax
    UINT64 v6; // rdx
    UINT64 *v7; // rbx
    
    v2 = 0x8000000000000002uLL;
    if ( a1 )
    {
        if ( a2 )
        {
            v5 = sub_211B9();
            if ( v5 )
            {
                *(UINT32 *)v5 = 13;
                v5[2] = a2;
                ++*(UINT64 *)(a2 + 8);
                v6 = a1[3];
                v7 = (UINT64 *)(v6 + 24);
                if ( !v6 )
                    v7 = a1 + 2;
                *v7 = (UINT64)v5;
                a1[3] = (UINT64)v5;
                ++a1[4];
                return 0LL;
            }
        }
    }
    return v2;
}

UINT64 sub_8F3E(UINT64 a1, UINT64 a2)
{
    UINT64 v4; // rax
    UINT64 v5; // r14
    UINT64 v6; // rsi
    
    v4 = (UINT64)sub_212D9();
    v5 = v4;
    if ( a2 && a1 && v4 )
    {
        v6 = 0LL;
        while ( sub_212F7((UINT64*)v5, *(UINT64 *)(a1 + 8 * v6)) >= 0 )
        {
            if ( a2 == ++v6 )
                return v5;
        }
        sub_21081(v5);
        return 0LL;
    }
    return v5;
}

void* sub_13780(UINT64 a1, UINT64 a2)
{
    void* v4; // rax
    void* v5; // rdi
    
    v4 = sub_1D2B1((UINTN)(a2 + 1));
    v5 = v4;
    if ( v4 )
    {
        sub_240B0(v4, (void*)a1, a2);
        *(char *)(v5 + a2) = 0;
    }
    return v5;
}

char * sub_8965(char *a1, char a2)
{
    char *result; // rax
    int i; // ecx
    
    if ( !a1 )
        return 0LL;
    result = a1;
    for ( i = *a1; a2 != i; i = *++result )
    {
        if ( !(char)i )
            return 0LL;
    }
    return result;
}

UINT64 sub_8D85(char* a1)
{
    if ( a1 )
        return sub_8CF8(a1);
    else
        return 0LL;
}

UINT64 sub_137C0(char *a1)
{
    char *v1; // rax
    char *v2; // r12
    char *v3; // rax
    char *v4; // r14
    UINT64 v5; // rdi
    char v6; // al
    char v7; // dl
    UINT64 i; // rsi
    UINT32 v9; // ecx
    char* v10; // r14d
    int v11; // r14d
    int v12; // ebx
    char *v13; // rsi
    UINT64 v14; // r13
    UINT64 v17; // [rsp+30h] [rbp-40h]
    
    v1 = sub_8965(a1, 38);
    if ( v1 )
    {
        v2 = v1;
        v17 = 0LL;
        while ( 1 )
        {
            v3 = sub_8965(v2, 59);
            if ( !v3 )
                return v17;
            v4 = v3;
            LODWORD(v5) = (v3 - v2) & 0xffffffff;
            if ( v3 - v2 > 2 )
            {
                if ( v2[1] != 35 )
                {
                    for ( i = 0LL; i != 15; i += 3LL )
                    {
                        if ( (char *)(v5 - 1) == (&off_AC720)[i + 1]
                            && !(UINT32)sub_2826F(v2 + 1, (&off_AC720)[i], (UINT32)(v5 - 1)) )
                        {
                            v10 = (&off_AC720)[i + 2];
                            goto LABEL_31;
                        }
                    }
                    if ( (UINT32)v5 != 1 )
                    {
                        sub_8D39(0LL, "literal entity value not recognised, entity replacement skipped");
                        goto LABEL_43;
                    }
                    v10 = 0;
                LABEL_31:
                    sub_8D39(0LL, "literal entity found for codepoint 0x%x", v10);
                    goto LABEL_32;
                }
                v6 = v2[2];
                v5 = (v6 == 120) | 2u;
                v7 = v2[v5];
                if ( v7 != 59 )
                {
                    v9 = 10;
                    if ( v6 == 120 )
                        v9 = 16;
                    v10 = 0;
                    do
                    {
                        v11 = v9 * ((UINT32)((UINT64)v10 & 0xffffffff));
                        if ( (UINT8)(v7 - 48) > 9u )
                        {
                            if ( v6 == 120 && (UINT8)(v7 - 97) <= 5u )
                            {
                                v12 = v7 - 97;
                            }
                            else
                            {
                                v12 = v7 - 65;
                                if ( (UINT8)(v7 - 65) >= 6u )
                                    v12 = 0;
                                if ( v6 != 120 )
                                    v12 = 0;
                            }
                        }
                        else
                        {
                            v12 = v7 - 48;
                        }
                        v10 = (char*)(UINT64)(v12 + v11);
                        v5 = (UINT32)(v5 + 1);
                        v7 = v2[v5];
                    }
                    while ( v7 != 59 );
                LABEL_32:
                    if ( !v17 )
                    {
                        v17 = sub_8D85(a1);
                        v2 = (char *)(v17 + v2 - a1);
                    }
                    v13 = &v2[(UINT32)(v5 + 1)];
                    v14 = (UINT32)sub_2822A(v2) - (UINT32)v5;
                    sub_8D39(0LL, "inserting codepoint 0x%x", v10);
                    if ( (UINT64)v10 > 0x7F )
                    {
                        if ( (UINT64)v10 > 0x7FF )
                        {
                            if ( (UINT64)v10 > 0xFFFF )
                            {
                                if ( (UINT64)v10 <= 0x1FFFFF )
                                {
                                    *v2 = ((UINT64)v10 >> 18) | 0xF0;
                                    v2[1] = (((UINT64)v10 >> 12) & 0x3F) | 0x80;
                                    v2[2] = (((UINT64)v10 >> 6) & 0x3F) | 0x80;
                                    v2[3] = ((UINT64)v10 & 0x3F) | 0x80;
                                    v2 += 4;
                                }
                            }
                            else
                            {
                                *v2 = ((UINT64)v10 >> 12) | 0xE0;
                                v2[1] = (((UINT64)v10 >> 6) & 0x3F) | 0x80;
                                v2[2] = ((UINT64)v10 & 0x3F) | 0x80;
                                v2 += 3;
                            }
                        }
                        else
                        {
                            *v2 = ((UINT64)v10 >> 6) | 0xC0;
                            v2[1] = ((UINT64)v10 & 0x3F) | 0x80;
                            v2 += 2;
                        }
                    }
                    else
                    {
                        *(char**)v2++ = v10;
                    }
                    EFI_COPY_MEM CopyMem = mBootServices->CopyMem;
                    
                    CopyMem(v2, v13, v14);
                    v4 = v2;
                    goto LABEL_43;
                }
                sub_8D39(0LL, "invalid numeric entity specified but no codepoint given, skipping");
            }
            else
            {
                sub_8D39(0LL, "entity found but less than minimum of 4 bytes, skipping");
            }
        LABEL_43:
            v2 = sub_8965(v4, 38);
            if ( !v2 )
                return v17;
        }
    }
    return 0LL;
}

UINT64 * sub_213A4(UINT64 a1)
{
    UINT64 *result; // rax
    
    if ( !a1 )
        return 0LL;
    result = sub_211B9();
    if ( !result )
        return 0LL;
    *(UINT32 *)result = 4;
    result[2] = a1;
    return result;
}

UINT64 sub_8E79(UINT64 a1)
{
    return (UINT64)sub_213A4(a1);
}

UINT64 sub_13630(UINT64 *a1, UINT64 a2, int a3)
{
    UINT64 v6; // rbx
    UINT64 result; // rax
    
    v6 = (UINT64)sub_1D2B1(24LL);
    *(UINT64 *)v6 = 0LL;
    if ( a2 )
        *(UINT64 *)v6 = sub_8D41(a2);
    *(UINT32 *)(v6 + 8) = a3;
    result = *a1;
    *(UINT64 *)(v6 + 16) = *a1;
    *a1 = v6;
    return result;
}

UINT64 * sub_213FB(UINT64 a1, UINT64 a2, char a3)
{
    UINT64 *result; // rax
    
    if ( !a1 && a2 )
        return 0LL;
    result = sub_211B9();
    if ( !result )
        return 0LL;
    *(UINT32 *)result = 6;
    result[2] = a1;
    result[3] = a2;
    *((char *)result + 32) = a3 & (a1 != 0);
    return result;
}

UINT64 * sub_21D44(void* a1, UINT64 a2)
{
    UINT64 *v2; // rsi
    void* v5; // rax
    void* v6; // r14
    
    v2 = qword_AAA98;
    if ( a1 )
    {
        if ( a2 )
        {
            v5 = sub_1D2B1(a2);
            if ( v5 )
            {
                v6 = v5;
                sub_240B0(v5, a1, a2);
                v2 = (UINT64 *)sub_213FB((UINT64)v6, a2, 0LL);
                if ( !v2 )
                    v2 = qword_AAA98;
                if ( v2 == qword_AAA98 )
                {
                    sub_1D327(v6);
                    return qword_AAA98;
                }
            }
        }
    }
    return v2;
}

UINT64 sub_212BE(UINT32 *a1)
{
    UINT32 v1; // ecx
    UINT64 result; // rax
    
    if ( !a1 )
        return 0LL;
    v1 = *a1;
    result = 0LL;
    if ( v1 - 2 < 0xA )
        return v1;
    return result;
}

UINT64 sub_21A25(UINT64 a1, UINT64 *a2)
{
    UINT64 result; // rax
    UINT64 v3; // rax
    
    result = 0LL;
    if ( a1 && a2 )
    {
        if ( *(UINT32 *)a1 == 10 )
        {
            *a2 = a1;
            v3 = *(UINT64 *)(a1 + 16);
            a2[1] = v3;
            if ( v3 && *(UINT32 *)v3 == 13 )
                return *(UINT64 *)(v3 + 16);
        }
        else if ( *(UINT32 *)a1 == 2 )
        {
            *a2 = a1;
            result = *(UINT64 *)(a1 + 16);
            a2[1] = result;
            return result;
        }
        return 0LL;
    }
    return result;
}

int * sub_21A6D(int *a1, int **a2)
{
    int *result; // rax
    int *v3; // r8
    int v4; // ecx
    UINT64 v5; // rax
    
    result = 0LL;
    if ( a1 && a2 )
    {
        if ( *a2 != a1 )
            return 0LL;
        v3 = a2[1];
        v4 = *a1;
        if ( v3 && v4 == 2 )
        {
            if ( *v3 == 3 )
            {
                result = (int *)*((UINT64 *)v3 + 4);
                a2[1] = result;
                return result;
            }
            return 0LL;
        }
        result = 0LL;
        if ( v3 && v4 == 10 )
        {
            if ( *v3 == 13 )
            {
                v5 = *((UINT64 *)v3 + 3);
                a2[1] = (int *)v5;
                if ( v5 )
                    return *(int **)(v5 + 16);
            }
            return 0LL;
        }
    }
    return result;
}

UINT64 sub_220F6(UINT64 *a1, UINT64 *a2)
{
    UINT64 result; // rax
    UINT64 v5; // rdx
    UINT64 v6; // rdx
    UINT64 v7; // rbx
    UINT64 v8[6]; // [rsp+20h] [rbp-30h] BYREF
    
    v8[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v8[1] = 0xAAAAAAAAAAAAAAAAuLL;
    result = sub_212BE((UINT32*)a1);
    if ( a2 != qword_AAA98 && (UINT32)result == 10 )
    {
        if ( (UINT32)sub_212BE((UINT32*)a2) == 6 )
        {
            return sub_212F7(a1, (UINT64)a2);
        }
        else
        {
            result = sub_212BE((UINT32*)a2);
            if ( (UINT32)result == 10 )
            {
                result = sub_21A25((UINT64)a2, v8);
                if ( result )
                {
                    do
                    {
                        v7 = sub_212F7(a1, result);
                        result = (UINT64)sub_21A6D((int *)a2, (int **)v8);
                    }
                    while ( v7 >= 0 && result );
                }
            }
        }
    }
    return result;
}

UINT64 * sub_2206E(UINT64 *a1, UINT64 *a2)
{
    UINT64 *v4; // rdi
    bool v5; // al
    UINT64 *v6; // rax
    UINT64 *v7; // r14
    
    v4 = qword_AAA98;
    v5 = a2 == 0LL || a2 == qword_AAA98;
    if ( !a1 || a1 == qword_AAA98 )
    {
        if ( !v5 )
        {
            sub_2118C((UINT64)a2);
            return a2;
        }
    }
    else if ( v5 )
    {
        sub_2118C((UINT64)a1);
        return a1;
    }
    else
    {
        v6 = sub_212D9();
        if ( v6 )
        {
            v7 = v6;
            sub_220F6(v6, a1);
            sub_220F6(v7, a2);
            return v7;
        }
    }
    return v4;
}

void sub_21D27(UINT64 a1)
{
    if ( a1 )
    {
        if ( (UINT64 *)a1 != qword_AAA98 )
            sub_21081(a1);
    }
}

UINT64  sub_21968(UINT64 a1, UINT64 *a2)
{
    UINT64 result; // rax
    
    result = 0x8000000000000002uLL;
    if ( a1 && a2 && *(UINT32 *)a1 == 10 )
    {
        *a2 = *(UINT64 *)(a1 + 32);
        return 0LL;
    }
    return result;
}

UINT64  sub_21708(UINT64 a1, UINT64 *a2, UINT64 *a3)
{
    UINT64 result; // rax
    
    result = 0x8000000000000002uLL;
    if ( a1 && a2 && *(UINT32 *)a1 == 6 )
    {
        *a2 = *(UINT64 *)(a1 + 16);
        if ( a3 )
            *a3 = *(UINT64 *)(a1 + 24);
        return 0LL;
    }
    return result;
}

UINT64  sub_2219B(int *a1)
{
    UINT64 v2; // rax
    UINT64 v3; // rdi
    UINT64 v4; // rax
    UINT64 v5; // rcx
    int *v7[2]; // [rsp+20h] [rbp-50h] BYREF
    UINT64 v8; // [rsp+30h] [rbp-40h] BYREF
    UINT64 v9[7]; // [rsp+38h] [rbp-38h] BYREF
    
    v7[0] = (int *)0xAAAAAAAAAAAAAAAALL;
    v7[1] = (int *)0xAAAAAAAAAAAAAAAALL;
    v8 = 0LL;
    v9[0] = 0LL;
    if ( sub_21968((UINT64)a1, v9) < 0 )
    {
        if ( sub_21708((UINT64)a1, &v8, v9) >= 0 )
            return v9[0];
        return 0LL;
    }
    if ( !v9[0] )
        return 0LL;
    v2 = sub_21A25((UINT64)a1, (UINT64*)v7);
    if ( !v2 )
        return 0LL;
    v3 = 0LL;
    do
    {
        v9[0] = 0LL;
        v4 = sub_21708(v2, &v8, v9);
        v5 = 0LL;
        if ( v4 >= 0 )
            v5 = v9[0];
        v3 += v5;
        v2 = (UINT64)sub_21A6D(a1, v7);
    }
    while ( v2 );
    return v3;
}

UINT64 * sub_21EA6(int *a1, UINT64 *a2, UINT64 *a3)
{
    UINT64 *v5; // rbx
    UINT64 v7; // rax
    UINT64 v8; // rdi
    void* v9; // rax
    void* v10; // r12
    UINT64 *v11; // rax
    UINT64 i; // rax
    UINT64 v13; // rax
    UINT64 v14; // r8
    UINT64 *v15; // rsi
    int *v17[3]; // [rsp+28h] [rbp-68h] BYREF
    UINT64 v18; // [rsp+40h] [rbp-50h] BYREF
    UINT64 v19; // [rsp+48h] [rbp-48h] BYREF
    UINT64 v20[8]; // [rsp+50h] [rbp-40h] BYREF
    
    v18 = 0LL;
    v19 = 0LL;
    memset(v17, 170, sizeof(v17));
    v20[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v5 = qword_AAA98;
    if ( a1 && a1 != (int *)qword_AAA98 )
    {
        if ( (unsigned int)sub_212BE((unsigned int *)a1) == 6 )
        {
            sub_2118C((UINT64)a1);
            v5 = (UINT64 *)a1;
        }
        else
        {
            if ( (unsigned int)sub_212BE((unsigned int *)a1) == 10 )
            {
                v7 = sub_2219B(a1);
                if ( v7 )
                {
                    v8 = v7;
                    v9 = sub_1D2B1(v7);
                    if ( v9 )
                    {
                        v10 = v9;
                        v11 = sub_213FB((UINT64)v9, v8, 1);
                        if ( v11 )
                        {
                            v5 = v11;
                            for ( i = sub_21A25((UINT64)a1, (UINT64*)v17); ; i = (UINT64)sub_21A6D(a1, v17) )
                            {
                                if ( !i )
                                    goto LABEL_19;
                                v17[2] = 0LL;
                                v20[0] = 0LL;
                                v13 = sub_21708(i, (UINT64*)&v17[2], v20);
                                if ( v13 < 0 )
                                    break;
                                if ( v20[0] > v8 )
                                {
                                    v14 = 0x8000000000000005uLL;
                                    goto LABEL_17;
                                }
                                sub_240B0(v10, v17[2], v20[0]);
                                v10 += v20[0];
                                v8 -= v20[0];
                            }
                            v14 = v13;
                        LABEL_17:
                            DEBUG ((DEBUG_INFO, "#[EB.DD.DDF|!] %r\n", v14));
                            sub_21081((UINT64)v5);
                            goto LABEL_18;
                        }
                        sub_1D327(v10);
                    }
                }
            }
        LABEL_18:
            v5 = qword_AAA98;
        }
    }
LABEL_19:
    if ( v5 && ((void)(v15 = qword_AAA98), v5 != qword_AAA98) && sub_21708((UINT64)v5, &v18, &v19) < 0 )
    {
        v18 = 0LL;
        v19 = 0LL;
        sub_21081((UINT64)v5);
    }
    else
    {
        v15 = v5;
    }
    if ( a2 )
        *a2 = v18;
    if ( a3 )
        *a3 = v19;
    return v15;
}

UINT64 sub_8D66(UINT64 a1, UINT64 a2, UINT64 a3, UINT64 a4)
{
    if ( a2 == a4 )
        return sub_281FE((UINT32*)a1, (char*)a3, a2);
    else
        return 0xFFFFFFFFLL;
}

UINT64 sub_21B2F(UINT64 *a1, int a2, unsigned int a3)
{
    UINT64 result; // rax
    UINT64 v4; // rdi
    
    result = 0x8000000000000002uLL;
    if ( a1 && a3 - 1 <= 3 )
    {
        v4 = *a1;
        if ( *a1 + (UINT64)a3 > a1[2] )
            return 0x8000000000000009uLL;
        if ( a3 != 1 )
        {
            if ( a3 != 2 )
            {
                if ( a3 != 3 )
                    return 0LL;
                *(char *)(v4 + a1[1] + 2) = a2;
                v4 = *a1;
            }
            *(char *)(v4 + a1[1] + 1) = BYTE1(a2);
            v4 = *a1;
        }
        *(char *)(a1[1] + v4) = BYTE2(a2);
        *a1 += a3;
        return 0LL;
    }
    return result;
}

UINT64  sub_21B9E(UINT64 a1, UINT64 a2, UINT64 *a3)
{
    UINT64 v5; // rcx
    UINT64 v7; // rdx
    int v8; // ebx
    UINT64 v9; // rdi
    UINT64 v10; // r8
    UINT64 v11; // rax
    UINT64 v12; // r9
    int v13; // ecx
    UINT64 v14; // rax
    UINT64 v15; // rax
    int v17; // eax
    UINT64 v18; // [rsp+20h] [rbp-50h] BYREF
    UINT64 v19; // [rsp+28h] [rbp-48h]
    UINT64 v20; // [rsp+30h] [rbp-40h]
    
    v19 = 0xAAAAAAAAAAAAAAAAuLL;
    *a3 = 0LL;
    v18 = 0LL;
    v5 = (3 * a2 + 3) >> 2;
    v20 = v5;
    if ( !v5 )
    {
        v19 = 0LL;
        goto LABEL_16;
    }
    v19 = (UINT64)sub_1D2B1(v5);
    if ( !v19 )
    {
    LABEL_16:
        v20 = 0LL;
        goto LABEL_17;
    }
    v7 = 0LL;
    v8 = 0;
    v9 = 0LL;
    while ( 1 )
    {
        v10 = (unsigned int)v8;
        v11 = a2;
        if ( v9 > a2 )
            v11 = v9;
        do
        {
            if ( v11 == v9 )
            {
                LODWORD(v9) = (UINT32)(v11 & 0xffffffff);
                goto LABEL_25;
            }
            v12 = *(UINT8 *)(a1 + v9);
            if ( (v12 & 0x80u) != 0LL || ((void)(v13 = *((UINT8 *)qword_AAAF0 + v12)), (v13 & 0x80u) != 0) )
            {
                DEBUG ((DEBUG_INFO, "#[EB.XA.DB64|IC!] %ld %d\n", v9 + 1, v12));
            LABEL_20:
                v18 = 0LL;
                *a3 = 0LL;
                goto LABEL_21;
            }
            ++v9;
        }
        while ( (char)v13 == 64 );
        if ( !(char)v12 || (char)v12 == 61 )
            break;
        ++v8;
        v7 = (unsigned int)(v13 + ((UINT32)v7 << 6));
        if ( (UINT32)v10 == 3 )
        {
            v14 = sub_21B2F(&v18, (int)v7, (unsigned int)v10);
            v7 = 0LL;
            v8 = 0;
            if ( v14 )
            {
            LABEL_30:
                DEBUG ((DEBUG_INFO, "#[EB.XA.DB64|M!] %ld\n", v9));
                goto LABEL_20;
            }
        }
    }
LABEL_25:
    if ( v8 > 0 )
    {
        if ( v8 <= 3 )
        {
            v17 = v8 - 4;
            do
            {
                v7 = (unsigned int)((UINT32)v7 << 6);
                ++v17;
            }
            while ( v17 );
        }
        if ( sub_21B2F(&v18, v7 & 0xffffffff, (unsigned int)(v8 - 1)) )
            goto LABEL_30;
    }
LABEL_17:
    v15 = v18;
    *a3 = v18;
    if ( v15 )
        return v19;
LABEL_21:
    if ( v19 )
    {
        sub_1D327((void*)v19);
        v19 = 0LL;
    }
    return 0LL;
}

UINT64 * sub_21DC3(int *a1, int a2, int a3)
{
    UINT64 *v3; // rsi
    UINT64 *v4; // rdi
    UINT64 *v5; // rax
    UINT64 v6; // rsi
    UINT64 v7; // rax
    UINT64 *v8; // rax
    UINT64 v10; // [rsp+28h] [rbp-28h] BYREF
    UINT64 v11; // [rsp+30h] [rbp-20h] BYREF
    UINT64 v12[3]; // [rsp+38h] [rbp-18h] BYREF
    
    v12[0] = 0LL;
    v10 = 0xAAAAAAAAAAAAAAAAuLL;
    v11 = 0xAAAAAAAAAAAAAAAAuLL;
    v3 = qword_AAA98;
    if ( a1 )
    {
        v4 = qword_AAA98;
        if ( a1 != (int *)qword_AAA98 && a2 == 1 && !a3 )
        {
            v10 = 0LL;
            v5 = sub_21EA6(a1, v12, &v10);
            if ( !v5 || ((void)(v6 = (UINT64)v5), v5 == qword_AAA98) )
            {
                v3 = qword_AAA98;
                if ( v12[0] )
                    sub_1D327((void*)v12[0]);
            }
            else
            {
                v11 = 0LL;
                v7 = sub_21B9E(v12[0], v10, &v11);
                if ( v7 )
                {
                    if ( v11 )
                    {
                        v8 = sub_213FB(v7, v11, 0);
                        v4 = qword_AAA98;
                        if ( v8 )
                            v4 = v8;
                    }
                }
                sub_21081(v6);
                return v4;
            }
        }
    }
    return v3;
}

UINT64 * sub_8D98(int *a1)
{
    UINT64 *v1; // rdi
    UINT64 *v2; // rsi
    UINT64 v3; // rcx
    UINT64 v5; // [rsp+20h] [rbp-20h] BYREF
    UINT64 v6[3]; // [rsp+28h] [rbp-18h] BYREF
    
    v1 = 0LL;
    v5 = 0LL;
    v6[0] = 0LL;
    v2 = sub_21EA6(a1, &v5, v6);
    if ( v2 )
    {
        v3 = v5;
        if ( !v6[0] || v5 )
        {
            if ( !v6[0] )
                v3 = 0LL;
            v1 = sub_213FB(v3, v6[0], 1);
        }
        else
        {
            v1 = 0LL;
        }
    }
    sub_21D27((UINT64)v2);
    return v1;
}

UINT64 sub_8B03(char *a1, char **a2, int a3)
{
    UINT64 v4; // r10
    char v5; // al
    char v6; // r11
    char v7; // bl
    char v8; // al
    UINT8 v9; // al
    UINT64 v10; // rdx
    UINT64 result; // rax
    UINT64 v12; // rsi
    UINT64 v13; // rax
    int v14; // edx
    int v15; // edx
    int v16; // edi
    char *v17; // rbx
    int v18; // edx
    UINT8 v19; // bl
    int v20; // esi
    
    if ( a2 )
        *a2 = a1;
    if ( !a1 )
        return 0LL;
    v4 = 0x7FFFFFFFFFFFFFFFLL;
    while ( 1 )
    {
        v5 = *a1;
        if ( (UINT8)(*a1 - 9) >= 5u && v5 != 32 )
            break;
        ++a1;
    }
    if ( v5 == 43 )
    {
        ++a1;
    }
    else if ( v5 == 45 )
    {
        ++a1;
        v4 = 0x8000000000000000uLL;
        v6 = 0;
        goto LABEL_13;
    }
    v6 = 1;
LABEL_13:
    if ( a3 )
    {
        if ( a3 == 16 )
        {
            v7 = *a1;
            a3 = 16;
            if ( *a1 != 48 )
                goto LABEL_27;
            if ( ((UINT8)a1[1] | 0x20) == 0x78 )
            {
                v8 = a1[2];
                if ( (UINT8)(v8 - 48) >= 0xAu )
                {
                    a3 = 16;
                    v9 = v8 - 65;
                    if ( v9 > 0x25u )
                        goto LABEL_26;
                    v10 = 0x3F0000003FLL;
                    if ( !_bittest64(&v10, v9) )
                        goto LABEL_26;
                }
                goto LABEL_23;
            }
            a3 = 16;
        LABEL_35:
            v13 = 0xFFFFFFFFFFFFFFFFuLL / (unsigned int)(2 * a3);
            v14 = 48;
            goto LABEL_36;
        }
        if ( a3 >= 2 )
            goto LABEL_26;
        return 0LL;
    }
    v7 = *a1;
    a3 = 10;
    if ( *a1 != 48 )
        goto LABEL_27;
    if ( ((UINT8)a1[1] | 0x20) != 0x78 )
    {
        a3 = 8;
        goto LABEL_35;
    }
LABEL_23:
    a1 += 2;
    a3 = 16;
LABEL_26:
    v7 = *a1;
LABEL_27:
    v12 = 0LL;
    v13 = 0xFFFFFFFFFFFFFFFFuLL / (unsigned int)(2 * a3);
    if ( v7 < 48 )
        goto LABEL_51;
    v14 = v7;
    if ( v7 > 57 )
    {
        if ( v7 < 65 )
        {
            v12 = 0LL;
            goto LABEL_51;
        }
        if ( v7 > 90 )
        {
            v20 = -1;
            if ( (UINT8)(v7 - 97) < 0x1Au )
                v20 = v7 - 87;
            v15 = v20;
        }
        else
        {
            v15 = v7 - 55;
        }
        goto LABEL_37;
    }
LABEL_36:
    v15 = v14 - 48;
LABEL_37:
    v12 = 0LL;
    if ( v15 >= 0 && v15 < a3 )
    {
        v16 = 0;
        v17 = a1 + 1;
        v12 = 0LL;
        do
        {
            a1 = v17;
            v16 |= v12 >= v13;
            v12 = (unsigned int)a3 * v12 + (unsigned int)v15;
            v18 = *v17;
            if ( v18 < 48 )
                break;
            if ( (char)v18 > 57 )
            {
                if ( (char)v18 < 65 )
                    goto LABEL_52;
                if ( (char)v18 > 90 )
                {
                    v19 = v18 - 97;
                    v15 = v18 - 87;
                    if ( v19 >= 0x1Au )
                        v15 = -1;
                }
                else
                {
                    v15 = v18 - 55;
                }
            }
            else
            {
                v15 = v18 - 48;
            }
            if ( v15 < 0 )
                break;
            v17 = a1 + 1;
        }
        while ( v15 < a3 );
        goto LABEL_52;
    }
LABEL_51:
    v16 = 0;
LABEL_52:
    if ( a2 )
        *a2 = a1;
    if ( !v16 )
        v4 = v12;
    result = -(UINT64)v4;
    if ( v6 )
        return v4;
    return result;
}

UINT64 * sub_213D4(UINT64 a1)
{
    UINT64 *result; // rax
    
    result = sub_211B9();
    if ( result )
    {
        *(UINT32 *)result = 5;
        result[2] = a1;
    }
    return result;
}

UINT64 sub_8E83(UINT64 a1)
{
    return (UINT64)sub_213D4(a1);
}

typedef struct __m128 {
    
}* __m128;

__m128 sub_898C(char *a1, UINT64 *a2)
{
    UINT64 v4; // rax
    UINT64 v5; // rdi
    char *v6; // rcx
    UINT64 v7; // rax
    __m128 v8; // xmm6
    char v9; // al
    char *v10; // rcx
    double v11; // xmm0_8
    char v12; // al
    UINT64 v13; // rax
    UINT64 v14; // rcx
    double v15; // xmm0_8
    char *v17; // [rsp+28h] [rbp-38h] BYREF
    
    v17 = (char *)0xAAAAAAAAAAAAAAAALL;
    if ( !a1 )
        return (__m128)0LL;
    v17 = a1;
    v4 = sub_8B03(a1, &v17, 10);
    v5 = v4;
    v6 = v17;
    if ( v17 == a1 )
    {
        v8 = 0LL;
    }
    else
    {
        v7 = -(UINT64)v4;
        if ( -v5 < 1 )
            LODWORD(v7) = (UINT32)(v5 & 0xffffffff);
        v8 = 0LL;
        *(double *)&v8 = (double)(int)v7;
        v9 = *v17;
        if ( *v17 != 46 )
            goto LABEL_14;
        if ( (UINT8)(v17[1] - 48) <= 9u )
        {
            ++v17;
            v9 = v6[1];
            if ( (UINT8)(v9 - 48) > 9u )
            {
                ++v6;
            }
            else
            {
                v10 = v6 + 2;
                v11 = 1.0;
                do
                {
                    v11 = v11 / 10.0;
                    v17 = v10;
                    *(double *)&v8 = *(double *)&v8 + (double)(*(v10 - 1) - 48) * v11;
                    v9 = *v10++;
                }
                while ( (UINT8)(v9 - 48) < 0xAu );
                v6 = v10 - 1;
            }
        LABEL_14:
            if ( v9 == 69 || (v9 == 101 && (qword_AD3B8 & 2) == 0) )
            {
                v12 = v6[1];
                if ( v12 == 45 || v12 == 43 )
                    v12 = v6[2];
                if ( (UINT8)(v12 - 48) <= 9u )
                {
                    v17 = v6 + 1;
                    v13 = sub_8B03(v6 + 1, &v17, 10);
                    v14 = -(UINT64)v13;
                    if ( -(UINT64)v13 < 1 )
                        v14 = v13;
                    if ( v14 )
                    {
                        v15 = *(double *)&qword_A9CD0[v13 >> 63];
                        do
                        {
                            if ( (v14 & 1) != 0 )
                                *(double *)&v8 = *(double *)&v8 * v15;
                            v14 >>= 1;
                            v15 = v15 * v15;
                        }
                        while ( v14 );
                    }
                }
            }
        }
    }
    if ( a2 )
        *a2 = (UINT64)v17;
    if ( v5 < 0 )
    {
        *(double *)&v8 = -*(double *)&v8;
        *((double *)&v8 + 1) = -*((double *)&v8 + 1);
    }
    return v8;
}

UINT64 * sub_215FF(UINT64 a1)
{
    UINT64 *result; // rax
    
    result = sub_211B9();
    if ( result )
    {
        *(UINT32 *)result = 11;
        *((char *)result + 16) = 1;
        result[3] = a1;
    }
    return result;
}

UINT64 * sub_2162A(double a1)
{
    UINT64 *result; // rax
    
    result = sub_211B9();
    if ( result )
    {
        *(UINT32 *)result = 11;
        *((char *)result + 16) = 0;
        *((double *)result + 3) = a1;
    }
    return result;
}

UINT64  sub_8E8D(double a1)
{
    if ( (double)(int)a1 == a1 )
        return (UINT64)sub_215FF(a1);
    else
        return (UINT64)sub_2162A(a1);
}

UINT64 * sub_21449(UINT8 *a1)
{
    UINT64 *v2; // r14
    int v3; // eax
    int v4; // edx
    UINT64 v5; // r13
    UINT64 v6; // r15
    int v7; // ebx
    UINT8 *v8; // rdi
    int v9; // esi
    char v10; // al
    char *v11; // rax
    UINT64 *v12; // rax
    UINT64 v14; // [rsp+28h] [rbp-48h] BYREF
    UINT16 v15; // [rsp+30h] [rbp-40h]
    
    v15 = -21846;
    v14 = 0xAAAAAAAAAAAAAAAAuLL;
    v2 = 0LL;
    sub_E580((CHAR16*)&v14, 0LL, 10LL);
    if ( a1 )
    {
        if ( *a1 )
        {
            v2 = 0LL;
            v3 = 0;
            LOWORD(v4) = 0;
            while ( 1 )
            {
                v5 = 28LL * v3;
                v6 = *(int *)&byte_AD960[v5];
                if ( v6 > 0 )
                    break;
                v10 = v14;
            LABEL_12:
                switch ( v10 )
                {
                    case 1:
                        WORD1(v14) = v4;
                        break;
                    case 2:
                        BYTE4(v14) = v4;
                        break;
                    case 3:
                        BYTE5(v14) = v4;
                        break;
                    case 4:
                        BYTE6(v14) = v4;
                        break;
                    case 5:
                        HIBYTE(v14) = v4;
                        break;
                    case 6:
                        LOBYTE(v15) = v4;
                        break;
                    default:
                        break;
                }
                if ( *a1 == byte_AD960[v5 + 12] )
                {
                    v11 = &byte_AD960[v5 + 16];
                }
                else
                {
                    if ( *a1 != byte_AD960[v5 + 20] )
                        return v2;
                    v11 = &byte_AD960[v5 + 24];
                }
                ++a1;
                v3 = *(UINT32 *)v11;
                if ( v3 >= 7 )
                    goto LABEL_24;
            }
            v7 = (v6 + 1) & 0xffffffff;
            v4 = 0;
            v8 = a1;
            while ( 1 )
            {
                v9 = *v8;
                if ( (UINT8)(v9 - 48) > 9u )
                    break;
                ++v8;
                v4 = v9 + 10 * v4 - 48;
                if ( --v7 <= 1 )
                {
                    if ( v4 >= *(UINT32 *)&byte_AD960[v5 + 4] && v4 <= *(UINT32 *)&byte_AD960[v5 + 8] )
                    {
                        a1 += v6;
                        v10 = v3 + 1;
                        LOBYTE(v14) = v10;
                        goto LABEL_12;
                    }
                    return v2;
                }
            }
        }
        else
        {
        LABEL_24:
            v12 = sub_211B9();
            if ( v12 )
            {
                v2 = v12;
                *(UINT32 *)v12 = 7;
                sub_240B0(v12 + 2, &v14, 10LL);
            }
            else
            {
                return 0LL;
            }
        }
    }
    return v2;
}

UINT64 sub_8EAF(UINT8 *a1)
{
    return (UINT64)sub_21449(a1);
}

UINT64 sub_8E6F(UINT64 *a1, UINT64 a2)
{
    return sub_212F7(a1, a2);
}

UINT64  sub_216B8(UINT64 a1, UINT64 *a2)
{
    UINT64 result; // rax
    
    result = 0x8000000000000002uLL;
    if ( a1 && a2 && *(UINT32 *)a1 == 4 )
    {
        *a2 = *(UINT64 *)(a1 + 16);
        return 0LL;
    }
    return result;
}


UINT64  sub_8DFA(UINT64 a1)
{
    UINT64 v2; // [rsp+28h] [rbp-8h] BYREF
    
    v2 = 0LL;
    if ( !sub_216B8(a1, &v2) )
        return v2;
    v2 = 0LL;
    return 0LL;
}

UINT64 sub_136C3(UINT64 **a1, UINT64 a2, UINT64 a3)
{
    UINT64 *v4; // rax
    UINT64 v5; // rsi
    int v7; // ecx
    UINT64 v8; // r14
    UINT64 v9; // rax
    const char *v10; // rdx
    
    v4 = *a1;
    if ( !*a1 )
        goto LABEL_8;
    v5 = a2;
    v7 = *((UINT32 *)v4 + 2);
    switch ( v7 )
    {
        case 4:
            **a1 = sub_8D41(a2);
            goto LABEL_12;
        case 9:
            sub_8E6F((UINT64*)*v4, a2);
            goto LABEL_12;
        case 12:
            v8 = sub_8D41(*v4);
            sub_13683((UINT64*)a1);
            if ( *a1 && *((UINT32 *)*a1 + 2) == 6 )
            {
                v9 = sub_8DFA(v8);
                sub_8E2D(**a1, v9, v5);
                sub_8D5C((UINT64)a1);
            LABEL_12:
                LOBYTE(v5) = 1;
                return (unsigned int)v5;
            }
            v10 = "parse error on line %lld, key+leaf found without enclosing dictionary.";
            break;
        default:
        LABEL_8:
            v10 = "parse error on line %lld, unexpected token T_LEAF (stack head not T_KEY or T_ARRAY).";
            break;
    }
    LODWORD(v5) = 0;
    sub_8D39(0LL, v10, a3);
    return (unsigned int)v5;
}

UINT64 * sub_8F17(UINT64 a1)
{
    UINT64 v1; // rax
    
    v1 = sub_8CF8((char*)a1);
    if ( v1 )
        return sub_213A4(v1);
    else
        return 0LL;
}

UINT64 * sub_215D8(UINT8 a1)
{
    UINT64 *result; // rax
    
    result = sub_211B9();
    if ( result )
    {
        *(UINT32 *)result = a1 | 8;
        *((char *)result + 16) = a1;
    }
    return result;
}

UINT64 sub_8F9E(UINT64 a1)
{
    LOBYTE(a1) = (char)a1 != 0;
    return (UINT64)sub_215D8(a1);
}

UINT64 sub_127FC(UINT64 a1, UINT64 a2)
{
    UINT64 v2; // eax
    void* v3; // rax
    UINT64 v4; // r13
    UINT64 v5; // rcx
    UINT64 v6; // rsi
    UINT64 v7; // rdx
    UINT64 v8; // r8
    UINT64 v9; // r8
    UINT64 v10; // rdx
    void* v11; // rax
    void* v12; // rsi
    void* v13; // rax
    void* v14; // rbx
    UINT64 v15; // rsi
    UINT64 v16; // r14
    UINT64 (*v17)[5]; // rsi
    UINT64 v18; // rdi
    UINT64 v19; // rbx
    UINT64 v20; // rdx
    UINT64 v21; // rdx
    UINT64 v22; // rdx
    UINT64 v23; // rax
    UINT64 v24; // rsi
    UINT64 v25; // r12
    UINT64 v26; // rdx
    UINT64 v27; // r14
    UINT64 v28; // rbx
    UINT64 v29; // rax
    UINT64 v30; // rsi
    UINT64 v31; // rdi
    UINT64 v32; // r14
    UINT64 v33; // eax
    UINT64 v34; // rdi
    UINT64 v35; // rbx
    UINT64 v36; // rdi
    UINT64 v37; // rsi
    UINT64 v38; // rbx
    UINT64 v39; // rax
    UINT64 v40; // r14
    UINT64 v41; // rax
    UINT64 v42; // rax
    UINT64 v43; // rax
    UINT64 v44; // r8
    const char *v46; // rsi
    UINT64 v47[6]; // [rsp+28h] [rbp-B8h] BYREF
    UINT64 v48; // [rsp+58h] [rbp-88h]
    UINT64 v49; // [rsp+60h] [rbp-80h] BYREF
    UINT64 v50; // [rsp+68h] [rbp-78h] BYREF
    UINT64 v51; // [rsp+70h] [rbp-70h]
    UINT64 v52; // [rsp+78h] [rbp-68h]
    UINT64 v53[4]; // [rsp+80h] [rbp-60h] BYREF
    UINT64 v54[8]; // [rsp+A0h] [rbp-40h] BYREF
    
    v54[0] = 0LL;
    v47[0] = 0xAAAAAAAA00000000uLL;
    v47[5] = 0xAAAAAAAAAAAAAAAAuLL;
    v47[1] = a1;
    v47[2] = a2;
    v47[4] = a1;
    v47[3] = 1LL;
    memset(v53, 170, sizeof(v53));
    UINT64 off_AC868 = 0xAC868;
    
    do
    {
        if ( !(UINT8)sub_131DC((UINT32*)v47, (UINT64)v53) )
        {
            v4 = 0LL;
            sub_8D39(0LL, "parse error on line %lld, expecting T_HEADER, T_DOCTYPE or T_PLIST, got EOF.", v53[1]);
            return v4;
        }
        v2 = v53[0];
        if ( (UINT32)(LODWORD(v53[0]) - 1) >= 2 )
        {
            if ( LODWORD(v53[0]) != 4 )
            {
                sub_8D39(
                         0LL,
                         "parse error on line %lld, unexpected token %s.",
                         v53[1],
                         *((UINT64 *)&unk_AC480 + 4 * LODWORD(v53[0]) + 1));
                goto LABEL_104;
            }
            v3 = sub_1D2B1(24LL);
            *(UINT64 *)v3 = 0LL;
            *(UINT32 *)(v3 + 8) = 4;
            *(UINT64 *)(v3 + 16) = v54[0];
            v54[0] = (UINT64)v3;
            v2 = v53[0];
        }
    }
    while ( v2 != 4 );
    if ( !(UINT8)sub_131DC((UINT32*)v47, (UINT64)v53) )
        return 0LL;
    v4 = 0LL;
    while ( 2 )
    {
        v5 = (UINT32)(LODWORD(v53[0]) - 5);
        switch ( LODWORD(v53[0]) )
        {
            case 5:
                if ( v54[0] && *(UINT32 *)(v54[0] + 8LL) == 4 && *(UINT64 *)v54[0] )
                {
                    v4 = sub_8D41(*(UINT64 *)v54[0]);
                    sub_13683(v54);
                    goto LABEL_71;
                }
                sub_8D39(0LL, "parse error on line %lld, unexpected token T_PLIST_END.", v53[1]);
                goto LABEL_104;
            case 6:
                v6 = sub_8EB9(0LL, 0LL, 0LL);
                v7 = v6;
                v8 = 6LL;
                goto LABEL_69;
            case 7:
                if ( v54[0] && *(UINT32 *)(v54[0] + 8LL) == 6 )
                    goto LABEL_21;
                sub_8D39(0LL, "parse error on line %lld, unexpected token T_DICT_END.", v53[1]);
                goto LABEL_104;
            case 8:
                goto LABEL_61;
            case 9:
                v6 = sub_8F3E(0LL, 0LL);
                v7 = v6;
                v8 = 9LL;
                goto LABEL_69;
            case 0xA:
                if ( !v54[0] || *(UINT32 *)(v54[0] + 8LL) != 9 )
                {
                    sub_8D39(0LL, "parse error on line %lld, unexpected token T_ARRAY_END.", v53[1]);
                    goto LABEL_104;
                }
            LABEL_21:
                v6 = sub_8D41(*(UINT64 *)v54[0]);
                sub_13683(v54);
                v9 = v53[1];
                v10 = v6;
                goto LABEL_66;
            case 0xB:
                goto LABEL_60;
            case 0xC:
                if ( !v54[0] || *(UINT32 *)(v54[0] + 8LL) != 6 )
                {
                    sub_8D39(0LL, "parse error on line %lld, unexpected token T_KEY.", v53[1]);
                    goto LABEL_104;
                }
                if ( !(UINT8)sub_131DC((UINT32*)v47, (UINT64)v53) )
                {
                    sub_8D39(0LL, "parse error on line %lld, expected T_VALUE or T_KEY_END, got EOF.", v53[1]);
                    goto LABEL_104;
                }
                if ( LODWORD(v53[0]) == 13LL )
                    goto LABEL_68;
                if ( LODWORD(v53[0]) != 19 )
                {
                    sub_8D39(
                             0LL,
                             "parse error on line %lld, expected T_VALUE or T_KEY_END, got %s.",
                             v53[1],
                             *((UINT64 *)&unk_AC480 + 4 * LODWORD(v53[0]) + 1));
                    goto LABEL_104;
                }
                v11 = sub_13780(v53[2],a2);
                if ( !v11 )
                {
                    sub_8D39(0LL, "failed to null terminate string, malloc probably failed.");
                    goto LABEL_104;
                }
                v12 = v11;
                if ( sub_8965(v11, 38LL) )
                {
                    sub_8D39(0LL, "found entity prefix in value string (&)");
                    v13 = (void*)sub_137C0(v12);
                    if ( v13 )
                    {
                        v14 = v13;
                        sub_1D327(v12);
                        v12 = v14;
                    }
                }
                v15 = sub_8E79((UINT64)v12);
                sub_13630(v54, v15, 12LL);
                sub_8D5C(v15);
                if ( (UINT8)sub_131DC((UINT32*)v47, (UINT64)v53) && LODWORD(v53[0]) == 13 )
                    goto LABEL_71;
                sub_8D39(
                         0LL,
                         "parse error on line %lld, expected T_KEY_END, got %s.",
                         v53[1],
                         *((UINT64 *)&unk_AC480 + 4 * LODWORD(v53[0]) + 1));
                goto LABEL_104;
            case 0xE:
                v51 = v53[2];
                v16 = v53[3];
                v17 = (UINT64 (*)[5])off_AC868;
                while ( 2 )
                {
                    if ( !(UINT8)sub_131DC((UINT32*)v47, (UINT64)v53) )
                    {
                        sub_8D39(0LL, "parse error on line %lld, expected T_VALUE, T_CDATA or T_LEAF_END, got EOF.", v53[1]);
                        goto LABEL_104;
                    }
                    if ( (UINT32)(LODWORD(v53[0]) - 19) < 2 )
                    {
                        v18 = (UINT64)sub_21D44((void*)v53[2], (UINT64)v53[3]);
                        v19 = (UINT64)sub_2206E((UINT64*)v17, (UINT64*)v18);
                        sub_21D27((UINT64)v17);
                        sub_21D27(v18);
                        v17 = (UINT64 (*)[5])v19;
                        if ( (UINT32)(LODWORD(v53[0]) - 19) >= 2 )
                            goto LABEL_74;
                        continue;
                    }
                    break;
                }
                v19 = (UINT64)v17;
                if ( LODWORD(v53[0]) != 15 )
                {
                    sub_8D39(
                             0LL,
                             "parse error on line %lld, expected T_VALUE, T_CDATA or T_LEAF_END, got %s.",
                             v53[1],
                             *((UINT64 *)&unk_AC480 + 4 * LODWORD(v53[0]) + 1));
                    goto LABEL_104;
                }
            LABEL_74:
                v49 = 0xAAAAAAAAAAAAAAAAuLL;
                v50 = 0xAAAAAAAAAAAAAAAAuLL;
                v24 = (UINT64)sub_21EA6((int*)v19, &v49, &v50);
                sub_21D27(v19);
                if ( !v24 )
                {
                    sub_8D39(0LL, "parse error on line %lld, failed to create dispatch_data_t map from T_LEAF data.", v53[1]);
                    goto LABEL_104;
                }
                v52 = v24;
                v25 = v49;
                v26 = v16;
                v27 = v50;
                v28 = v51;
                v48 = v26;
                if ( !(UINT32)sub_8D66(v51, v26, (UINT64)"data", 4LL) )
                {
                    v36 = (UINT64)sub_21D44((void*)v25, v27);
                    v37 = (UINT64)sub_21DC3((int*)v36, 1LL, 0LL);
                    sub_21D27(v36);
                    if ( v37 )
                    {
                        v35 = (UINT64)sub_8D98((int*)v37);
                        sub_21D27(v37);
                        v34 = v52;
                        goto LABEL_97;
                    }
                }
                v29 = (UINT64)sub_13780((UINT64)v25,0);
                if ( v29 )
                {
                    v30 = v29;
                    v31 = v48;
                    if ( !(UINT32)sub_8D66(v28, v48, (UINT64)"string", 6LL) )
                    {
                        v38 = (UINT64)sub_13780(v25,0);
                        if ( sub_8965((char*)v38, 38LL) )
                        {
                            sub_8D39(0LL, "found entity prefix in value string (&)");
                            v39 = sub_137C0((char*)v38);
                            if ( v39 )
                            {
                                v40 = v39;
                                sub_1D327((void*)v38);
                                v38 = v40;
                            }
                        }
                        v41 = sub_8E79(v38);
                        goto LABEL_92;
                    }
                    if ( !(UINT32)sub_8D66(v28, v31, (UINT64)"integer", 7LL) )
                    {
                        v42 = sub_8B03((char*)v30, 0LL, 10LL);
                        v41 = sub_8E83(v42);
                    LABEL_92:
                        v35 = v41;
                        v34 = v52;
                        goto LABEL_96;
                    }
                    v32 = v31;
                    v33 = sub_8D66(v28, v31, (UINT64)"real", 4LL);
                    v34 = v52;
                    if ( !v33 )
                    {
                        sub_898C((char*)v30, 0LL);
                        v43 = sub_8E8D(0);
                    LABEL_95:
                        v35 = v43;
                        goto LABEL_96;
                    }
                    if ( !(UINT32)sub_8D66(v28, v32, (UINT64)"date", 4LL) )
                    {
                        v43 = sub_8EAF((UINT8*)v30);
                        goto LABEL_95;
                    }
                    v35 = 0LL;
                LABEL_96:
                    sub_1D327((void*)v30);
                LABEL_97:
                    sub_21D27(v34);
                    v44 = v53[1];
                    if ( !v35 )
                        goto LABEL_123;
                    if ( !(UINT8)sub_136C3((UINT64**)v54, v35, v53[1]) )
                        goto LABEL_104;
                    sub_8D5C(v35);
                    if ( (UINT32)sub_281FE((UINT32*)v53[2], (char*)v51, v53[3]) )
                    {
                        sub_8D39(0LL, "parse error on line %lld, expected matching tags.", v53[1]);
                        goto LABEL_104;
                    }
                LABEL_71:
                    if ( !(UINT8)sub_131DC((UINT32*)v47, (UINT64)v53) )
                        return v4;
                    continue;
                }
                sub_21D27(v52);
                v44 = v53[1];
            LABEL_123:
                sub_8D39(0LL, "parse error on line %lld, failed to parse xpc object from tag.", v44);
            LABEL_104:
                if ( !v54[0] )
                    return 0LL;
                v4 = 0LL;
                do
                    sub_13683(v54);
                while ( v54[0] );
                return v4;
            case 0x10:
                v20 = v53[3];
                if ( v53[3] < 6uLL )
                    goto LABEL_43;
                if ( !(UINT32)sub_281FE((UINT32*)v53[2], (char*)"string", 6LL) )
                {
                    v23 = (UINT64)sub_8F17((UINT64)&qword_4C5F8);
                    goto LABEL_65;
                }
                v20 = v53[3];
            LABEL_43:
                if ( v20 < 3 )
                {
                LABEL_124:
                    v46 = (const char *)sub_13780(v53[2],0);
                    sub_8D39(0LL, "parse error on line %lld, found T_LEAF_EMPTY for unexpected type %s.", v53[1], v46);
                    if ( v46 )
                        sub_1D327((void*)v46);
                    goto LABEL_104;
                }
                if ( !(UINT32)sub_281FE((UINT32*)v53[2], "key", 3LL) )
                {
                LABEL_68:
                    v6 = (UINT64)sub_8F17((UINT64)&qword_4C5F8);
                    v7 = v6;
                    v8 = 12LL;
                LABEL_69:
                    sub_13630((UINT64*)v54, v7, (int)v8);
                    goto LABEL_70;
                }
                v21 = v53[3];
                if ( v53[3] >= 5uLL )
                {
                    if ( !(UINT32)sub_281FE((UINT32*)v53[2], "array", 5LL) )
                    {
                    LABEL_60:
                        v23 = sub_8F3E(0LL, 0LL);
                        goto LABEL_65;
                    }
                    v21 = v53[3];
                }
                if ( v21 < 4 )
                    goto LABEL_124;
                if ( !(UINT32)sub_281FE((UINT32*)v53[2], "data", 4LL) )
                {
                    v23 = (UINT64)sub_8D98((int*)off_AC868);
                    goto LABEL_65;
                }
                if ( v53[3] < 4uLL )
                    goto LABEL_124;
                if ( !(UINT32)sub_281FE((UINT32*)v53[2], "date", 4LL) )
                {
                    v23 = sub_8EAF(0LL);
                    goto LABEL_65;
                }
                if ( v53[3] < 4uLL )
                    goto LABEL_124;
                if ( !(UINT32)sub_281FE((UINT32*)v53[2], "dict", 4LL) )
                {
                LABEL_61:
                    v23 = sub_8EB9(0LL, 0LL, 0LL);
                    goto LABEL_65;
                }
                v22 = v53[3];
                if ( v53[3] < 7uLL )
                    goto LABEL_57;
                if ( (UINT32)sub_281FE((UINT32*)v53[2], "integer", 7LL) )
                {
                    v22 = v53[3];
                LABEL_57:
                    if ( v22 < 4 || (UINT32)sub_281FE((UINT32*)v53[2], "real", 4LL) )
                        goto LABEL_124;
                    v23 = sub_8E8D(a1);
                }
                else
                {
                    v23 = sub_8E83(0LL);
                }
            LABEL_65:
                v6 = v23;
                v9 = v53[1];
                v10 = v23;
            LABEL_66:
                if ( !(UINT8)sub_136C3((UINT64**)v54, v10, v9) )
                    goto LABEL_104;
            LABEL_70:
                sub_8D5C(v6);
                goto LABEL_71;
            case 0x11:
                LOBYTE(v5) = 1;
                goto LABEL_64;
            case 0x12:
                v5 = 0LL;
            LABEL_64:
                v23 = sub_8F9E(v5);
                goto LABEL_65;
            default:
                sub_8D39(
                         0LL,
                         "parse error on line %lld, unexpected token: %s",
                         v53[1],
                         *((const char **)&unk_AC480 + 4 * LODWORD(v53[0]) + 1));
                goto LABEL_71;
        }
    }
}

UINT64 sub_1F414(UINT64 a1, UINT64 a2, UINT64 *a3)
{
    UINT64 v4; // rax
    
    v4 = sub_127FC(a1,a2);
    if ( !v4 )
        return 0x800000000000000EuLL;
    *a3 = v4;
    return 0LL;
}

UINT64 sub_28246(char *a1, char *a2)
{
    char v2; // al
    char *v3; // rcx
    
    v2 = *a1;
    if ( *a1 )
    {
        v3 = a1 + 1;
        while ( v2 == *a2 )
        {
            ++a2;
            v2 = *v3++;
            if ( !v2 )
                goto LABEL_5;
        }
    }
    else
    {
    LABEL_5:
        v2 = 0;
    }
    return (unsigned int)(v2 - (char)*a2);
}

UINT64 sub_2165C(UINT64 a1, UINT64 a2)
{
    UINT64 v2; // rbx
    UINT64 v4; // rsi
    UINT64 v5; // rcx
    
    if ( !a1 )
        return 0LL;
    if ( *(UINT32 *)a1 != 2 )
        return 0LL;
    v2 = *(UINT64 *)(a1 + 16);
    if ( !v2 )
        return 0LL;
    v4 = 0LL;
    while ( *(UINT32 *)v2 == 3 )
    {
        v5 = *(UINT64 *)(v2 + 16);
        if ( v5 && !(unsigned int)sub_28246((char*)v5, (char*)a2) )
            return *(UINT64 *)(v2 + 24);
        v2 = *(UINT64 *)(v2 + 32);
        if ( !v2 )
            return v4;
    }
    return v4;
}

UINT64 sub_1EF0E(int a1, int a2, UINT16* a3, UINT64 a4)
{
    double v4; // xmm2_8
    UINT64 v6; // rax
    UINT64 v7; // rax
    UINT64 v8; // rdi
    UINT64 v9; // rax
    UINT64 v11; // [rsp+30h] [rbp-20h] BYREF
    UINT64 v12[3]; // [rsp+38h] [rbp-18h] BYREF
    
    v11 = 0LL;
    v12[0] = 0LL;
    v6 = sub_1E3E8(a1, a2, a3, &v11, (CHAR16**)v12);
    if ( v6 < 0 )
        return v6;
    v7 = sub_1F414(v12[0], v11, &qword_B0200);
    if ( v7 )
    {
        v8 = v7;
        DEBUG ((DEBUG_INFO, "#[EB.OPT.LCF|!] %r <- EB.X.PF\n", v7));
        qword_B0200 = 0LL;
    }
    else
    {
        if ( (qword_B1DE8 & 1) == 0 )
        {
            v9 = sub_2165C(qword_B0200, (UINT64)"Kernel Flags");
            if ( sub_216B8(v9, (UINT64*)a4) >= 0 )
                DEBUG ((DEBUG_INFO, "#[EB|KF] <\"%e\">\n", v4));
        }
        return 0LL;
    }
    return v8;
}

void sub_4637C(CHAR16 *a1, CHAR16 *a2)
{
    CHAR16 v2; // ax
    CHAR16 *v3; // rdx
    
    if ( a1 && a2 )
    {
        v2 = *a2;
        if ( *a2 )
        {
            v3 = a2 + 1;
            do
            {
                *a1++ = v2;
                v2 = *v3++;
            }
            while ( v2 );
        }
        *a1 = 0;
    }
}

int sub_1F3BB(UINT64 a1, UINT64 a2, UINT64 *a3)
{
    UINT64 v4; // rcx
    UINT64 *i; // rsi
    UINT64 v7; // r8
    
    v4 = *a3;
    if ( !*a3 )
        return 0;
    for ( i = a3 + 1; ; ++i )
    {
        v7 = (int)sub_2822A((const char*)v4);
        if ( v7 <= a2 && !(unsigned int)sub_2826F((char*)a1, (char*)*(i - 1), (UINT32)v7) )
            break;
        v4 = *i;
        if ( !v4 )
            return 0;
    }
    return 1;
}

UINT8 * sub_1DBD5(
                  int a1,
                  UINT8 *a2,
                  UINT8 *a3,
                  char *a4,
                  UINT64 a5,
                  UINT64 a6)
{
    UINT64 v10; // rdx
    UINT8 v11; // al
    UINT64 v12; // r8
    UINT64 v13; // r9
    UINT8 *v14; // rcx
    char *v15; // rdi
    UINT64 v16; // rsi
    UINT64 v17; // rax
    char v18; // al
    UINT64 v19; // rsi
    int v20; // al
    UINT64 v21; // rsi
    char v22; // al
    UINT64 i; // rcx
    
    v10 = a5;
    v11 = *a2;
    if ( *a2 )
    {
        v12 = 0x2000000100002601LL;
        v13 = 0x100002600LL;
        v14 = a2;
    LABEL_3:
        while ( v11 <= 0x20u && _bittest64(&v13, v11) )
            v11 = *++v14;
        v15 = a4;
        v16 = a5;
        while ( 1 )
        {
            if ( v11 <= 0x20u )
            {
                if ( _bittest64(&v13, v11) )
                    goto LABEL_3;
                if ( !v11 )
                    break;
            }
            if ( v15 && *v15 == v11 )
            {
                ++v15;
                ++v14;
                if ( !--v16 )
                {
                    v17 = *v14;
                    if ( v17 > 0x3D )
                    {
                        v16 = 0LL;
                    }
                    else
                    {
                        v16 = 0LL;
                        if ( _bittest64((UINT64*)&v12, (UINT32)v17) )
                            return a3;
                    }
                }
            }
            else
            {
                ++v14;
                v15 = 0LL;
            }
            v11 = *v14;
        }
    }
    
    if ( (a1 & 1) != 0 && ((void)(v18 = sub_1F3BB((UINT64)a4, a5, (UINT64*)off_AD8F0)), (void)(v10 = a5), !v18) )
    {
        DEBUG ((DEBUG_INFO, "#[EB|O:NB] SM %s\n", a4));
    }
    else if ( (a1 & 0x1000) != 0 && ((void)(v19 = v10), (void)(v20 = sub_1F3BB((UINT64)a4, (UINT64)v10, (UINT64*)&off_AD920)), (void)(v10 = v19), v20) )
    {
        DEBUG ((DEBUG_INFO, "#[EB|O:NB] R %s\n", a4));
    }
    else if ( (a1 & 0x200000) != 0 && ((void)(v21 = v10), (void)(v22 = sub_1F3BB((UINT64)a4, v10, (UINT64*)off_AD930)), (void)(v10 = v21), v22) )
    {
        DEBUG ((DEBUG_INFO, "#[EB|O:NB] SB %s\n", a4));
    }
    else if ( (a1 & 0x1000000) != 0 && (UINT8)sub_1F3BB((UINT64)a4, v10, (UINT64*)off_AD930) )
    {
        DEBUG ((DEBUG_INFO, "#[EB|O:NB] TB %s\n", a4));
    }
    else
    {
        if ( a3 > a2 )
            *a3++ = 32;
        if ( a6 )
        {
            for ( i = 0LL; i != a6; ++i )
                a3[i] = a4[i];
            a3 += i;
        }
        *a3 = 0;
    }
    return a3;
}


UINT64 sub_11A4B(void)
{
    unsigned int v0; // esi
    UINT64 v1; // rax
    UINT64 v3; // [rsp+28h] [rbp-28h] BYREF
    UINT32 v4; // [rsp+30h] [rbp-20h] BYREF
    UINT8 v5[25]; // [rsp+37h] [rbp-19h] BYREF
    
    v4 = -1431655766;
    v3 = 0xAAAAAAAAAAAAAAAAuLL;
    v5[0] = -86;
    v0 = 0;
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    if ( byte_AEF88 == 1 )
    {
        v3 = 1LL;
        v1 = GetVariable(
                         L"SecurityEpoch",
                         &gAppleSecureBootVariableGuid,
                         &v4,
                         &v3,
                         v5);
        if ( !(v1 | (v3 ^ 1)) )
            return v5[0];
    }
    return v0;
}


char* sub_1D82E(UINT64 a1, unsigned char *a2, unsigned char *a3, unsigned char *a4)
{
    double v4; // xmm2_8
    UINT64 v9; // rbx
    UINT64 v10; // rax
    char *v11; // rbx
    UINT64 v12; // r14
    char *v13; // r8
    UINT64 v14; // rax
    UINT64 v15; // r8
    UINT8 v16; // al
    UINT64 v17; // r12
    char v18; // cl
    UINT64 v19; // rdx
    UINT64 v20; // r15
    UINT8 v21; // al
    UINT64 v22; // r13d
    UINT64 v23; // r15
    char v24; // cl
    UINT64 v25; // rdx
    UINT64 v26; // rbx
    char *v27; // rdi
    UINT8 v28; // al
    int v29; // eax
    UINT64 v30; // rcx
    UINT64 v31; // rax
    UINT64 i; // rax
    char v34[40]; // [rsp+30h] [rbp-70h] BYREF
    UINT64 v35; // [rsp+58h] [rbp-48h]
    char *v36; // [rsp+60h] [rbp-40h]
    
    DEBUG ((DEBUG_INFO, "#[EB|MBA:CL] <\"%e\">\n", v4));
    DEBUG ((DEBUG_INFO, "#[EB|MBA:NV] <\"%e\">\n", v4));
    DEBUG ((DEBUG_INFO, "#[EB|MBA:KF] <\"%e\">\n", v4));
    char* aChunklistNoRev = "-chunklist-no-rev2-dev";
    if ( a2 )
        v9 = (int)sub_2822A((const char*)a2);
    else
        v9 = 0LL;
    if ( a3 )
        v9 += (int)sub_2822A((const char*)a3);
    if ( a4 )
        v9 += (int)sub_2822A((const char*)a4);
    v10 = (UINT64)sub_1D2B1(v9 + 64);
    v11 = (char *)v10;
    if ( v10 )
    {
        v12 = 0x100002600LL;
        v13 = (char *)v10;
        if ( a2 )
        {
            while ( 1 )
            {
                v14 = *a2++;
                if ( v14 > 0x20 )
                    break;
                if ( !_bittest64(&v12, v14 & 0xffffffff) )
                {
                    if ( !*(a2 - 1) )
                    {
                        v13 = v11;
                        goto LABEL_17;
                    }
                    break;
                }
            }
            v15 = 0LL;
            do
            {
                v11[v15] = v14;
                LOBYTE(v14) = a2[v15++];
            }
            while ( (char)v14 );
            v13 = &v11[v15];
        }
    LABEL_17:
        *v13 = 0;
        v36 = v11;
        v35 = a1;
        if ( a3 )
        {
            v16 = *a3;
            if ( *a3 )
            {
                v17 = 0x100002601LL;
                do
                {
                    while ( v16 <= 0x20u && _bittest64(&v12, v16) )
                        v16 = *++a3;
                    v18 = 0;
                    v19 = 0LL;
                    v20 = 0LL;
                    while ( v16 > 0x20u || !_bittest64(&v17, v16) )
                    {
                        if ( v16 == 61 )
                            v18 = 1;
                        v19 += v18 == 0;
                        v16 = a3[++v20];
                    }
                    v13 = (char *)sub_1DBD5(v35 & 0xffffffff, (UINT8*)v36, (UINT8*)v13, (char*)a3, v19, v20);
                    v16 = a3[v20];
                    a3 += v20;
                }
                while ( v16 );
            }
        }
        if ( a4 )
        {
            v21 = *a4;
            v22 = v35;
            if ( *a4 )
            {
                v23 = 0x100002601LL;
                do
                {
                    while ( v21 <= 0x20u && _bittest64(&v12, v21) )
                        v21 = *++a4;
                    v24 = 0;
                    v25 = 0LL;
                    v26 = 0LL;
                    while ( v21 > 0x20u || !_bittest64(&v23, v21) )
                    {
                        if ( v21 == 61 )
                            v24 = 1;
                        v25 += v24 == 0;
                        v21 = a4[++v26];
                    }
                    v13 = (char *)sub_1DBD5((int)v35, (UINT8*)v36, (UINT8*)v13, (char*)a4, v25, v26);
                    v21 = a4[v26];
                    a4 += v26;
                }
                while ( v21 );
            }
        }
        else
        {
            v22 = v35;
        }
        v11 = v36;
        if ( (v22 & 1) != 0 )
            v13 = (char *)sub_1DBD5((int)v22, (UINT8*)v36, (UINT8*)v13, "-x", 2LL, 2LL);
        if ( (v22 & 0x10) != 0 )
            v13 = (char *)sub_1DBD5((int)v22, (UINT8*)v36, (UINT8*)v13, "-s", 2LL, 2LL);
        if ( (v22 & 2) != 0 )
            v13 = (char *)sub_1DBD5((int)v22, (UINT8*)v36, (UINT8*)v13, "-v", 2LL, 2LL);
        if ( (v22 & 0x400) != 0 )
            v13 = (char *)sub_1DBD5((int)v22, (UINT8*)v36, (UINT8*)v13, "srv=1", 5LL, 5LL);
        if ( (v22 & 0x1200000) != 0 )
        {
            v27 = v13;
            v28 = sub_11A4B();
            memset(v34, 170, 29);
            DEBUG ((DEBUG_INFO, "%s=%d", "chunklist-security-epoch", v28));
            v29 = sub_2822A(v34);
            if ( v27 > v11 )
                *v27++ = 32;
            if ( v29 )
            {
                v30 = v29;
                v31 = 0LL;
                do
                {
                    v27[v31] = v34[v31];
                    ++v31;
                }
                while ( v30 != v31 );
                v27 += v31;
            }
            *v27 = 0;
            if ( (v22 & 0x2000000) == 0 )
            {
                if ( v27 > v11 )
                    *v27++ = 32;
                for ( i = 0LL; i != 22; ++i )
                    v27[i] = aChunklistNoRev[i];
                v27[22] = 0;
            }
        }
        DEBUG ((DEBUG_INFO,"#[EB|MBA:OUT] <\"%e\">\n", v4));
    }
    return v11;
}

char sub_B45F(char *a1, UINT64 a2, CHAR16 *a3, UINT64 *a4, UINT64 a5)
{
    UINT16 v5; // ax
    CHAR16 *v6; // r11
    bool v7; // cf
    char *v8; // rsi
    char v9; // bl
    UINT16 v10; // di
    
    LOBYTE(v5) = *a1;
    v6 = a3;
    if ( *a1 )
    {
        v6 = a3;
        while ( 1 )
        {
            v7 = a2-- != 0;
            if ( v6 >= (CHAR16 *)((char *)a3 + a5) || !v7 )
                goto LABEL_16;
            v8 = a1 + 1;
            if ( (v5 & 0x80u) != 0 )
                break;
            v5 = (UINT8)v5;
        LABEL_15:
            *v6++ = v5;
            LOBYTE(v5) = *v8;
            a1 = v8;
            if ( !*v8 )
                goto LABEL_16;
        }
        v9 = v5 & 0xF0;
        if ( (v5 & 0xF0) != 0xC0 )
        {
            if ( v9 == -32 )
            {
                if ( (*v8 & 0xC0) != 0x80 )
                    goto LABEL_17;
                v10 = ((UINT8)v5 << 12) | ((*v8 & 0x3F) << 6);
                v8 = a1 + 2;
                goto LABEL_13;
            }
            if ( v9 != -48 )
                goto LABEL_17;
        }
        v10 = (v5 & 0x1F) << 6;
    LABEL_13:
        LOBYTE(v5) = *v8;
        if ( (*v8 & 0xC0) != 0x80 )
            goto LABEL_17;
        ++v8;
        v5 = v10 + (v5 & 0x3F);
        goto LABEL_15;
    }
LABEL_16:
    *v6++ = 0;
LABEL_17:
    *a4 = v6 - a3;
    return v5;
}

UINT64 sub_1D76C(UINT64 a1, UINT64 a2, UINT64 *a3, UINT64 *a4)
{
    UINT64 v7; // rdi
    UINT64 v8; // rax
    
    if ( !sub_1D5D4((char*)a1, (char*)a2, a3, (char**)a4) )
        return 0LL;
    v7 = 0x800000000000000EuLL;
    v8 = sub_2165C(qword_B0200, a2);
    if ( (sub_216B8(v8, a3) & 0x8000000000000000uLL) == 0LL && *a3 )
    {
        *a4 = (int)sub_2822A((const char*)*a3);
        return 0LL;
    }
    return v7;
}

CHAR16 * sub_B57C(UINT64 a1, int a2)
{
    UINT64 v3; // esi
    UINT64 v4; // rbx
    CHAR16 *v5; // rax
    CHAR16 *v6; // rdi
    UINT64 v8; // [rsp+28h] [rbp-28h] BYREF
    
    v3 = a1;
    v8 = 0xAAAAAAAAAAAAAAAAuLL;
    v4 = (int)(4 * sub_2822A((char*)a1) + 2);
    v5 = (CHAR16 *)sub_1D2B1(v4);
    if ( !v5 )
        return 0LL;
    v6 = v5;
    *v5 = 0;
    sub_B45F((char*)v3, a2, (CHAR16*)v5, (UINT64*)&v8, v4);
    return v6;
}

UINT64 sub_43E3(UINT8 a1)
{
    UINT64 v1; // rdi
    UINT64 v3; // rax
    UINT64 v5[3]; // [rsp+28h] [rbp-38h] BYREF
    _DWORD v6[7]; // [rsp+44h] [rbp-1Ch] BYREF
    
    v1 = 0x8000000000000002uLL;
    v6[0] = -1431655766;
    memset(v5, 170, sizeof(v5));
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    if ( a1 <= 0x3Fu )
    {
        v5[2] = 0LL;
        if ( GetVariable(
                         L"b",
                         &unk_AD140,
                         v6,
                         &v5[2],
                         0LL) != 0x800000000000000EuLL
            && v5[2] != 8LL )
        {
            SetVariable(
                        L"b",
                        &unk_AD140,
                        0LL,
                        0LL,
                        0LL);
        }
        v5[2] = 8LL;
        v1 = GetVariable(
                         L"b",
                         &unk_AD140,
                         v6,
                         &v5[2],
                         &v5[1]);
        if ( v1 == 0x800000000000000EuLL )
        {
            v3 = 0LL;
            goto LABEL_9;
        }
        if ( !v1 )
        {
            v3 = v5[1];
        LABEL_9:
            v5[0] = v3 | (1LL << a1);
            v5[2] = 8LL;
            v1 = SetVariable(
                             L"b",
                             &unk_AD140,
                             7LL,
                             8LL,
                             v5);
            DEBUG ((DEBUG_INFO,  "#[EB|RBFU] %d 0x%016qx\n", a1, v5[0]));
        }
    }
    return v1;
}

UINT64 sub_1EFC9(UINT8 a1)
{
    int v1; // r8d
    UINT64 result; // rax
    int v8; // ecx
    unsigned int v9; // edx
    int v10; // r8d
    
    v1 = a1;
    if ( (a1 & 0x80u) != 0 )
    {
        UINT64 _RAX = 1LL;
        __asm { cpuid }
        v8 = (UINT8)_RAX >> 4;
        v9 = (UINT8)((unsigned int)_RAX >> 20) + 15;
        if ( (((unsigned int)_RAX >> 8) & 0xF) != 0xF )
            v9 = ((unsigned int)_RAX >> 8) & 0xF;
        if ( v9 == 15 || v9 == 6 )
            v8 |= ((unsigned int)_RAX >> 12) & 0xF0;
        v10 = v1 << 21;
        LODWORD(result) = v10 + 270532608;
        if ( (v9 ^ 6) | ((v8 & 0xFFFFFFEF) ^ 0x2A) )
            LODWORD(result) = v10;
    }
    else
    {
        LODWORD(result) = a1 << 21;
    }
    return (unsigned int)result;
}



UINT64 sub_1E706(UINT64 a1, UINT64 a2, CHAR16 *a3, UINT64 *a4)
{
    CHAR16 *v5; // rdi
    CHAR16 *v7; // rax
    UINT64 v8; // rsi
    UINT64 v9; // rdx
    UINT64 v10; // r12
    UINT64 v11; // rbx
    void* v12; // rax
    void* v13; // r14
    char v14; // al
    UINT16 v15; // cx
    UINT64 v16; // rax
    UINT64 v17; // r13
    UINT64 v18; // rax
    UINT64 v19; // rcx
    UINT64 v20; // rax
    void* v21; // rdi
    void* v22; // rax
    void* v23; // rsi
    UINT64 v24; // rax
    CHAR16 *v25; // rbx
    CHAR16 *v26; // rdi
    UINT64 v27; // r8
    UINT64 v28; // rax
    UINT64 v29; // rdx
    UINT64 v30; // r12
    UINT64 v31; // rsi
    CHAR16* v32; // rax
    CHAR16 *v33; // r12
    UINT64 v34; // rbx
    bool v35; // sf
    UINT8 *v36; // rax
    CHAR16 *v37; // rcx
    CHAR16 *v38; // rsi
    UINT64 v39; // rbx
    UINT64 v41; // rbx
    UINT16 v42; // dx
    UINT64 v43; // rax
    UINT64 v44; // rax
    UINT64 v45; // r8
    UINT64 v46; // rax
    UINT64 v47; // rcx
    UINT32 v48; // eax
    UINT8 *v49; // rdi
    UINT64 v50; // rsi
    int v51; // ecx
    CHAR16 *v52; // rax
    UINT64 v53; // rcx
    UINT16 *v54; // rax
    CHAR16 *v56; // rdi
    UINT16 v57; // bx
    const UINT16 *v58; // rdx
    UINT64 v60; // [rsp+38h] [rbp-88h]
    UINT64 v61; // [rsp+48h] [rbp-78h]
    UINT64 v62; // [rsp+50h] [rbp-70h] BYREF
    UINT16 v63; // [rsp+58h] [rbp-68h]
    UINT32 v64; // [rsp+5Ch] [rbp-64h] BYREF
    UINT64 v65; // [rsp+60h] [rbp-60h] BYREF
    UINT64 v66; // [rsp+68h] [rbp-58h] BYREF
    UINT32 v67; // [rsp+74h] [rbp-4Ch] BYREF
    UINT8 *v68; // [rsp+78h] [rbp-48h] BYREF
    UINT32 v69[16]; // [rsp+80h] [rbp-40h] BYREF
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    v5 = a3;
    v68 = 0LL;
    v66 = 0LL;
    *(UINT64 *)v69 = 0LL;
    v67 = -1431655766;
    v65 = 0LL;
    v7 = a3 + 1;
    v8 = 0x100002600LL;
    while ( 1 )
    {
        v9 = (UINT16)*(v7 - 1);
        if ( v9 > 0x20 || !_bittest64(&v8, v9 & 0xffffffff) )
            break;
        ++v7;
    }
    if ( (UINT16)((v9 & 0xFFDF) - 65) >= 0x1Au && (CHAR16)v9 != 47 && (CHAR16)v9 != 92 )
        goto LABEL_17;
    while ( 1 )
    {
        if ( (UINT16)v9 > 0x3Du )
            goto LABEL_11;
        if ( _bittest64(&v8, (UINT16)v9) )
            goto LABEL_16;
        if ( !(CHAR16)v9 )
            break;
        if ( (UINT16)v9 == 61LL )
            goto LABEL_17;
    LABEL_11:
        LOWORD(v9) = *v7++;
    }
    --v7;
LABEL_16:
    v5 = v7;
LABEL_17:
    v10 = 0x8000000000000009uLL;
    v11 = sub_463B0(v5);
    *(UINT64 *)v69 = 3 * v11 + 1;
    v12 = sub_1D2B1(*(UINTN *)v69);
    if ( !v12 )
        return v10;
    v13 = v12;
    sub_B393(v5, v11, v12, 3 * v11 + 1, 1);
    if ( !sub_1D5D4(v13, "-x", (UINT64*)&v68, (char **)v69) )
        LOBYTE(qword_B1DE8) = qword_B1DE8 | 1;
    v14 = sub_1EEDF((UINT64)qword_B1E18);
    v15 = qword_B1DE8;
    if ( v14 )
    {
        v15 = qword_B1DE8 | 4;
        qword_B1DE8 |= 4uLL;
    }
    if ( (v15 & 0x2004) == 0 )
    {
        v62 = 0xAAAAAAAAAAAAAAAAuLL;
        v63 = -21846;
        v66 = 10LL;
        v16 = GetVariable(L"r",
                          &gAppleBootVariableGuid,
                          &v67,
                          &v66,
                          &v62);
        if ( !v16 || v16 == 0x8000000000000005uLL )
            qword_B1DE8 = (qword_B1DE8 & 0xFFFFFFFFFFFFF7EEuLL) | 0x800;
    }
    v61 = a2;
    v17 = 0LL;
    v66 = 0LL;
    v18 = GetVariable(L"boot-args",
                      &gAppleBootVariableGuid,
                      &v67,
                      &v66,
                      0LL);
    v19 = 0LL;
    if ( v18 != 0x8000000000000005uLL )
    {
    LABEL_31:
        v60 = v19;
        v21 = 0LL;
        v62 = 0LL;
        v64 = -1431655766;
        if ( GetVariable(L"efi-boot-kernelcache-data",
                         &gAppleBootVariableGuid,
                         &v64,
                         &v62,
                         0LL) == 0x8000000000000005uLL )
        {
            v22 = sub_1D2B1(v62);
            if ( v22 )
            {
                v23 = v22;
                v21 = 0LL;
                if ( GetVariable(L"efi-boot-kernelcache-data",
                                 &gAppleBootVariableGuid,
                                 &v64,
                                 &v62,
                                 v22) >= 0 )
                    v21 = v23;
            }
            else
            {
                v21 = 0LL;
            }
        }
        qword_B2058 = v21;
        v24 = (UINT64)sub_1D2B1(2048LL);
        if ( !v24 )
            return v10;
        v25 = (CHAR16 *)v24;
        sub_E5B0((void*)v24, 2048LL);
        v26 = L"com.apple.Boot.plist";
        LOBYTE(v27) = 1;
        v28 = (UINT64)sub_1DD73(*(UINT64 *)(qword_B1DD8 + 32), L"com.apple.Boot.plist", v27);
        v30 = a1;
        if ( v28 )
        {
            v31 = v28;
            LOBYTE(v29) = 1;
            v32 = sub_1DED3(v28, v29);
            if ( v32 )
            {
                v33 = v25;
                v34 = sub_1EF0E((int)a1, (int)v61, v32, (UINT64)&v65);
                sub_1D327((void*)v31);
                v35 = v34 < 0;
                v25 = v33;
                v30 = a1;
                if ( !v35 )
                    goto LABEL_53;
            }
            else
            {
                sub_1D327((void*)v31);
            }
        }
        if ( sub_1D5D4(v13, "config", (UINT64*)&v68, (char**)v69) )
        {
            if ( !v30 )
                v26 = L"Library\\Preferences\\SystemConfiguration\\com.apple.Boot.plist";
            sub_4637C(v25, v26);
        }
        else
        {
            v36 = v68;
            v37 = v25;
            if ( !v30 )
            {
                v37 = v25;
                if ( *v68 != 47 && *v68 != 92 )
                {
                    v56 = v25;
                    v37 = v25 + 1;
                    v57 = 76;
                    v58 = L"ibrary\\Preferences\\SystemConfiguration";
                    do
                    {
                        *(v37 - 1) = v57;
                        v57 = *v58++;
                        ++v37;
                    }
                    while ( v57 );
                    *(v37 - 1) = 92;
                    v25 = v56;
                }
            }
            v38 = v25;
            v39 = *(UINT64 *)v69;
            if ( (*(UINT64 *)v69)-- != 0LL )
            {
                v41 = v39 - 2;
                do
                {
                    v42 = (char)*v36++;
                    *v37++ = v42;
                    *(UINT64 *)v69 = v41--;
                }
                while ( v41 != -2 );
            }
            sub_4637C(v37, L".");
            v25 = v38;
        }
        sub_1EF0E((int)v30, (int)v61, v25, (UINT64)&v65);
    LABEL_53:
        v43 = (UINT64)sub_1D82E((UINT64)qword_B1DE8, v13, (unsigned char*)v60,(unsigned char*)v65);
        *a4 = v43;
        if ( !sub_1D76C(v43, (UINT64)"Kernel Cache", (UINT64*)&v68, (UINT64*)v69) )
        {
            v52 = (CHAR16 *)sub_B57C((UINT64)v68, v69[0]);
            qword_B2028 = (UINT64)v52;
            while ( 1 )
            {
                if ( *v52 == 47 )
                {
                    *v52 = 92;
                }
                else if ( !*v52 )
                {
                    break;
                }
                ++v52;
            }
        }
        if ( !sub_1D76C(*a4, (UINT64)"RAM Disk", (UINT64*)&v68, (UINT64*)v69) )
        {
            v44 = (UINT64)sub_B57C((UINT64)v68, v69[0]);
            qword_B2038 = v44;
            if ( v30 )
            {
                LOBYTE(v45) = 1;
                qword_B2040 = (UINT64)sub_1DD73(*(UINT64 *)(qword_B1DD8 + 32), (UINT16*)v44, v45);
            }
        }
        sub_1D327(v13);
        if ( !sub_1D5D4((char*)*a4, "-v", (UINT64*)&v68, (char**)v69) )
        {
            LOBYTE(qword_B1DE8) = qword_B1DE8 | 2;
            sub_22DC7(1LL);
        }
        if ( !sub_1D5D4((char*)*a4, "-x", (UINT64*)&v68, (char**)v69) )
            LOBYTE(qword_B1DE8) = qword_B1DE8 | 1;
        if ( !sub_1D5D4((char*)*a4, "-s", (UINT64*)&v68, (char**)v69) )
            LOBYTE(qword_B1DE8) = qword_B1DE8 | 0x10;
        if ( !sub_1D5D4((char*)*a4, "-no_compat_check", (UINT64*)&v68, (char**)v69) )
            byte_AD220 = 0;
        if ( !sub_1D5D4((char*)*a4, "-no_panic_dialog", (UINT64*)&v68, (char**)v69) )
            byte_AD221 = 0;
        v46 = sub_1D5D4((char*)*a4, "debug", (UINT64*)&v68, (char**)v69);
        v47 = qword_B1DE8;
        if ( !v46 )
        {
            v47 = qword_B1DE8 | 0x80;
            qword_B1DE8 |= 0x80uLL;
        }
        if ( (v47 & 1) != 0 )
        {
            LOBYTE(v47) = 15;
            sub_43E3(v47);
        }
        else
        {
            if ( !dword_AD244 || sub_1D76C(*a4, (UINT64)"slide", (UINT64*)&v68, (UINT64*)v69) || !(UINT32)sub_2410C() || !*(UINT64 *)v69 )
                goto LABEL_79;
            v48 = (UINT32)sub_2830D(v68);
            if ( v48 )
            {
                if ( v48 - 1 <= 0xFE )
                {
                    qword_B1DF8 = sub_1EFC9(v48);
                    dword_AD244 = 2;
                }
            LABEL_79:
                if ( !sub_1D76C(*a4, (UINT64)"cpu_subtype", (UINT64*)&v68, (UINT64*)v69) )
                {
                    v49 = v68;
                    v50 = v69[0];
                    if ( !(UINT32)sub_2826F((char*)v68, "x86_64", v69[0]) )
                    {
                        v51 = 3;
                        goto LABEL_90;
                    }
                    if ( !(UINT32)sub_2826F((char*)v49, "x86_64h", (UINT32)v50) )
                    {
                        v51 = 8;
                    LABEL_90:
                        sub_90D9(v51);
                    }
                }
                if ( (qword_B1DE8 & 0x1005) != 0 )
                    qword_B1DE8 |= 0x20000uLL;
                if ( !sub_1D76C(*a4, (UINT64)"kcsuffix", (UINT64*)&v68, (UINT64*)v69) )
                {
                    if ( !*(UINT64 *)v69
                        || ((void)(v54 = (UINT16 *)sub_B57C((UINT64)v68, *(int *)v69)), (qword_B1E00 = (UINT64)v54) == 0)
                        || !*(UINT64 *)v69
                        || !sub_463FF(v54, L"r") )
                    {
                        BYTE2(qword_B1DE8) |= 2u;
                    }
                }
                if ( (qword_B1DE8 & 0x10) != 0 )
                {
                    LOBYTE(v53) = 19;
                    sub_43E3(v53);
                    LOBYTE(qword_B1DE8) = qword_B1DE8 | 2;
                    sub_22DC7(1LL);
                }
                DEBUG ((DEBUG_INFO, "#[EB|OPT:BM] 0x%qx\n"));
                if ( v17 )
                    sub_1D327((void*)v17);
                sub_1D327(v25);
                return 0LL;
            }
        }
        dword_AD244 = 0;
        goto LABEL_79;
    }
    v20 = (UINT64)sub_1D2B1((UINTN)++v66);
    if ( v20 )
    {
        v17 = v20;
        if ( GetVariable(
                         L"boot-args",
                         &gAppleBootVariableGuid,
                         &v67,
                         &v66,
                         (void*)v20) < 0 )
        {
            v19 = 0LL;
        }
        else
        {
            *(char *)(v17 + v66) = 0;
            v19 = v17;
        }
        goto LABEL_31;
    }
    return v10;
}

UINT64 sub_216E0(UINT64 a1, UINT64 *a2)
{
    UINT64 result; // rax
    
    result = 0x8000000000000002uLL;
    if ( a1 && a2 && *(_DWORD *)a1 == 5 )
    {
        *a2 = *(UINT64 *)(a1 + 16);
        return 0LL;
    }
    return result;
}

UINT64 sub_1D7D7(UINT64 a1, UINT64 a2, UINT64 *a3)
{
    UINT64 v4; // rax
    UINT64 v6[3]; // [rsp+28h] [rbp-18h] BYREF
    
    v6[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v4 = sub_2165C(qword_B0200, a2);
    if ( sub_216E0(v4, v6) < 0 )
        return 0x800000000000000EuLL;
    *a3 = v6[0];
    return 0LL;
}

UINT64 sub_A038(void)
{
    UINT64 v0; // rax
    UINT64 result; // rax
    void* v2; // [rsp+28h] [rbp-18h] BYREF
    UINT64 v3; // [rsp+30h] [rbp-10h] BYREF
    unsigned int v4; // [rsp+3Ch] [rbp-4h] BYREF
    
    v2 = 0LL;
    v4 = 0;
    v3 = 0xAAAAAAAAAAAAAAAAuLL;
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    /*
     ((*(UINT64 (**)(unsigned int *))(v2 + 8))(&v4) < 0
     ? (qword_B1DE0 && !sub_1D7D7(qword_B1DE0, (UINT64)"Background Color", &v3)
     ? (LOWORD(v4) = v3, v0 = v3 >> 16, HIWORD(v4) = BYTE2(v3))
     : (v4 = dword_AD818, LODWORD(v0) = HIWORD(dword_AD818)))
     : (LOBYTE(v0) = BYTE2(v4)),
     !(char)v0);
     */
    if ( LocateProtocol(&gEfiUserInterfaceThemeProtocolGuid, 0LL, &v2) < 0
        )
    {
        if ( !(CHAR16)v4 )
            BYTE2(qword_B1DE8) |= 4u;
    }
    result = v4;
    dword_AD818 = v4;
    return result;
}

UINT64 sub_21990(UINT64 a1, UINT64 a2)
{
    UINT64 v2; // rcx
    UINT64 v3; // rdx
    UINT64 result; // rax
    
    if ( !a1 )
        return 0LL;
    if ( *(_DWORD *)a1 != 10 )
        return 0LL;
    if ( *(UINT64 *)(a1 + 32) <= a2 )
        return 0LL;
    v2 = *(UINT64 *)(a1 + 16);
    if ( !v2 )
        return 0LL;
    v3 = a2 + 1;
    result = 0LL;
    while ( *(_DWORD *)v2 == 13 )
    {
        if ( !--v3 )
            return *(UINT64 *)(v2 + 16);
        v2 = *(UINT64 *)(v2 + 24);
        if ( !v2 )
            return result;
    }
    return result;
}

UINT64 sub_1E4E2(char *a1)
{
    UINT64 v1; // rsi
    void* v3; // rax
    void* v4; // rbx
    UINT16 *v5; // rax
    UINT64 v6; // rdi
    UINT64 v7; // rax
    UINT64 v8; // rax
    UINT64 v9; // rdi
    unsigned int *v10; // r14
    UINT64 v11; // rbx
    UINT64 v12; // rax
    char *v14; // [rsp+28h] [rbp-58h] BYREF
    UINT64 v15; // [rsp+30h] [rbp-50h] BYREF
    UINT64 v16; // [rsp+38h] [rbp-48h] BYREF
    UINT64 v17; // [rsp+40h] [rbp-40h] BYREF
    UINT64 v18[7]; // [rsp+48h] [rbp-38h] BYREF
    
    v1 = 0x8000000000000003uLL;
    v17 = 0LL;
    v18[0] = 0LL;
    v15 = 0LL;
    v16 = 0xAAAAAAAAAAAAAAAAuLL;
    if ( !a1 )
        return 0x8000000000000002uLL;
    if ( !(unsigned int)sub_2826F(a1, "VMM", 3LL) )
        return 0LL;
    v3 = sub_1DD73(*(UINT64 *)(qword_B1DD8 + 32), (UINT16 *)"P", 1);
    if ( v3 )
    {
        v4 = v3;
        v5 = sub_1DED3((UINT64)v3, 1);
        if ( v5 )
            v6 = sub_1E3E8((UINT64)qword_B1E20, (UINT64)qword_B1E28, v5, &v17, (CHAR16**)v18);
        else
            v6 = 0x800000000000000EuLL;
        sub_1D327(v4);
    }
    else
    {
        v6 = sub_1E3E8((UINT64)qword_B1E20, (UINT64)qword_B1E28, (UINT16 *)"P", &v17, (CHAR16**)v18);
    }
    if ( v6 < 0 )
    {
        v7 = sub_1E3E8((UINT64)qword_B1E20, (UINT64)qword_B1E28, (UINT16 *)"S", &v17, (CHAR16**)v18);
        if ( v7 < 0 )
        {
            DEBUG ((DEBUG_INFO, "#[EB.OPT.PCC|!] %r <- EB.X.LXF\n", v7));
            return 0LL;
        }
    }
    v8 = sub_1F414(v18[0], v17, &v15);
    if ( v8 )
    {
        v9 = v8;
        DEBUG ((DEBUG_INFO, "#[EB.OPT.PCC|!] %r <- EB.X.PF\n", v8));
        v15 = 0LL;
        return v9;
    }
    v10 = (unsigned int *)sub_2165C(v15, (UINT64)"SupportedBoardIds");
    if ( (unsigned int)sub_212BE(v10) != 10 )
    {
        DEBUG ((DEBUG_INFO, "#[EB.OPT.PCC|BID!]\n"));
        return 0x800000000000000EuLL;
    }
    if ( (sub_21968((UINT64)v10, &v16) & 0x8000000000000000uLL) == 0LL && v16 )
    {
        v11 = 0LL;
        while ( 1 )
        {
            v14 = 0LL;
            v12 = sub_21990((UINT64)v10, v11);
            if ( (sub_216B8(v12, (UINT64*)&v14) & 0x8000000000000000uLL) == 0LL && v14 && !(unsigned int)sub_28246(v14, a1) )
                break;
            if ( ++v11 >= v16 )
                return v1;
        }
        return 0LL;
    }
    return v1;
}

UINT64 sub_1554C(_DWORD *a1)
{
    int v1; // edx
    UINT64 result; // rax
    unsigned int v3; // ecx
    int v4; // eax
    
    v1 = a1[6] | a1[4] | a1[5];
    result = 0LL;
    if ( a1[3] < 2u )
        return 32LL;
    if ( a1[3] == 2 )
    {
        v3 = 0;
        v4 = 0;
        do
            v4 += _bittest64((UINT64*)&v1, v3++);
        while ( v3 != 32 );
        return (v4 + 7) & 0xFFFFFFF8;
    }
    return result;
}

UINT64 sub_153D5(void)
{
    UINT64 v0; // rax
    UINT64 v1; // rsi
    UINT64 v2; // rcx
    EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE SetMode = qword_B1F40->SetMode;
    if ( dword_AF158 != 1 )
    {
        v0 = SetMode(qword_B1F40, 1LL);
        if ( v0 < 0 )
            return v0;
        dword_AF158 = 1;
    }
    EFI_HANDLE_PROTOCOL HandleProtocol = mBootServices->HandleProtocol;
    EFI_HANDLE ConsoleOutHandle = mSystemTable->ConsoleOutHandle;
    v1 = HandleProtocol(
                        ConsoleOutHandle,
                        &gEfiGraphicsOutputProtocolGuid,
                        (void**)&qword_B1F20);
    qword_B1F20 = 0LL;
    v1 = HandleProtocol(
                        ConsoleOutHandle,
                        &gEfiUgaDrawProtocolGuid,
                        (void**)&qword_B1F18);
    if ( v1 >= 0
        ||v1 >= 0 )
    {
        LOBYTE(qword_B1DE8) = qword_B1DE8 | 0x20;
        if ( !qword_B1F20 )
            return (*(UINT64 (**)(UINT64, int *, int *, int *, int *))qword_B1F18)(
                                                                                   qword_B1F18,
                                                                                   (int*)&dword_AF110,
                                                                                   (int*)&dword_AF114,
                                                                                   (int*)&dword_AF130,
                                                                                   (int*)&dword_AF134);
        v2 = *(UINT64 *)(*(UINT64 *)(qword_B1F20 + 24) + 8LL);
        dword_AF110 = *(_DWORD *)(v2 + 4);
        dword_AF114 = *(_DWORD *)(v2 + 8);
        dword_AF130 = (UINT32)sub_1554C(0);
        if ( dword_AF130 )
            dword_AF134 = 1;
        else
            return 0x8000000000000003uLL;
    }
    return v1;
}

char  sub_16556(UINT64 a1, _DWORD *a2)
{
    UINT64 v3; // rax
    UINT64 v4; // rax
    UINT64 v5; // rcx
    _DWORD *v6; // rdi
    UINT64 v8[2]; // [rsp+40h] [rbp-40h] BYREF
    UINT64 (**v9)(UINT64, UINT64 *, UINT32 *, UINT32 *, UINT32 *, UINT32 *, UINT32 *); // [rsp+50h] [rbp-30h] BYREF
    UINT64 v10; // [rsp+58h] [rbp-28h] BYREF
    UINT64 v11; // [rsp+60h] [rbp-20h] BYREF
    int v12[5]; // [rsp+6Ch] [rbp-14h] BYREF
    
    v9 = 0LL;
    v10 = 0LL;
    v11 = 0LL;
    v12[0] = -1431655766;
    v8[1] = 0x3CE3895ECE110C8ALL;
    v8[0] = 0x4CB9E7EE63FAECF2LL;
    EFI_HANDLE_PROTOCOL HandleProtocol = mBootServices->HandleProtocol;
    EFI_HANDLE ConsoleOutHandle = mSystemTable->ConsoleOutHandle;
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    if ( a2 )
        *a2 = dword_AD81C;
    if ( !qword_AF138 )
    {
        v4 = *(UINT64 *)(v10 + 24);
        v5 = *(UINT64 *)(v4 + 24);
        
        if ( HandleProtocol(
                            ConsoleOutHandle,
                            &gEfiGraphicsOutputProtocolGuid,
                            (void**)&v10) >= 0
            && v5 != 0 )
        {
            v6 = *(_DWORD **)(v4 + 8);
            dword_AF110 = v6[1];
            dword_AF114 = v6[2];
            qword_AF138 = v5;
            dword_AF154 = *(_DWORD *)(v4 + 32);
            LODWORD(v3) = (UINT32)sub_1554C(v6);
            dword_AF130 = (UINT32)v3;
            if ( !(_DWORD)v3 )
            {
                qword_AF138 = 0LL;
                return v3;
            }
            dword_AF150 = v6[8] * ((unsigned int)v3 >> 3);
        }
        else
        {
            v3 = LocateProtocol(
                                &gAppleFramebufferInfoProtocolGuid,
                                0LL,
                                (void**)&v9);
            if ( v3 < 0 || !v9 )
                return v3;
            if ( (*v9)((UINT64)v9, &qword_AF138, &dword_AF154, &dword_AF150, &dword_AF110, &dword_AF114, &dword_AF130) < 0 )
            {
                qword_AF138 = 0LL;
                dword_AF150 = 0;
                dword_AF110 = 0;
                dword_AF114 = 0;
                dword_AF130 = 0;
            }
        }
    }
    v12[0] = 0;
    v3 = (*(UINT64 (**)(UINT64 *, UINT64, UINT64 *))(qword_B2098 + 320))(v8, 0LL, &v11);
    if ( v3 >= 0 && *(_DWORD *)v11 >= 0x10002u )
    {
        v3 = (*(UINT64 (**)(UINT64, int *))(v11 + 48))(v11, v12);
        if ( v3 < 0 )
            v12[0] = 0;
    }
    if ( a1 )
    {
        *(UINT64 *)(a1 + 48) = qword_AF138;
        *(_DWORD *)(a1 + 4) = dword_AF150;
        *(_DWORD *)(a1 + 8) = dword_AF110;
        *(_DWORD *)(a1 + 12) = dword_AF114;
        *(_DWORD *)(a1 + 16) = dword_AF130;
        LOBYTE(v3) = v12[0];
        *(char *)(a1 + 20) = v12[0];
    }
    return v3;
}

UINT64 _byteswap_uint64 (UINT64 Operand);

UINT64 sub_28800(UINT64 a1, UINT64 a2, UINT64 *a3, UINT64 a4)
{
    UINT64 *v6; // rdx
    UINT64 result; // rax
    UINT64 v8; // r12
    bool v9; // cf
    UINT64 v10; // rsi
    UINT64 v11; // rcx
    UINT64 v12; // r9
    UINT64 v13; // r8
    UINT64 v14; // r9
    UINT64 v15; // r10
    UINT64 v16; // r8
    UINT64 v17; // r10
    UINT64 v18; // r8
    UINT64 v19; // r9
    bool v20; // cc
    char v21; // r9
    UINT64 v22; // r10
    UINT64 v23; // r10
    UINT64 v24; // r8
    UINT64 v25; // r10
    UINT64 v26; // r8
    UINT64 v27; // r8
    UINT64 v28; // r11
    UINT64 v29; // r8
    UINT64 v30; // r11
    
    v6 = a3;
    result = 0LL;
    v8 = 0LL;
    v9 = a2 < 8;
    v10 = a2 - 8;
    if ( v9 )
        return 0LL;
    v11 = (UINT64)a3 + a4 - 8;
    if ( (UINT64)a3 > v11 )
        return 0LL;
    v12 = *(UINT8 *)a3;
    v13 = *a3;
    switch ( *(_BYTE *)v6 )
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 8:
        case 9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x58:
        case 0x59:
        case 0x5A:
        case 0x5B:
        case 0x5C:
        case 0x5D:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x88:
        case 0x89:
        case 0x8A:
        case 0x8B:
        case 0x8C:
        case 0x8D:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        LABEL_6:
            v14 = v12 >> 6;
            v6 = (UINT64 *)((char *)v6 + v14 + 2);
            if ( (UINT64)v6 > v11 )
                return 0LL;
            v15 = _byteswap_uint64(v13);
            v8 = (32 * v15) >> 53;
            v16 = v13 >> 16;
            v17 = ((4 * v15) >> 61) + 3;
            goto LABEL_8;
        case 6:
            return result;
        case 7:
        case 0xF:
        case 0x17:
        case 0x1F:
        case 0x27:
        case 0x2F:
        case 0x37:
        case 0x3F:
        case 0x47:
        case 0x4F:
        case 0x57:
        case 0x5F:
        case 0x67:
        case 0x6F:
        case 0x87:
        case 0x8F:
        case 0x97:
        case 0x9F:
        case 0xC7:
        case 0xCF:
        LABEL_23:
            v14 = v12 >> 6;
            v6 = (UINT64 *)((char *)v6 + v14 + 3);
            if ( (UINT64)v6 > v11 )
                return 0LL;
            v23 = v13 & 0x38;
            v24 = v13 >> 8;
            v8 = (UINT16)v24;
            v16 = v24 >> 16;
            v17 = (v23 >> 3) + 3;
            goto LABEL_8;
        case 0xE:
        case 0x16:
        LABEL_4:
            while ( 2 )
            {
                v6 = (UINT64 *)((char *)v6 + 1);
                if ( (UINT64)v6 > v11 )
                    return 0LL;
                v12 = *(UINT8 *)v6;
                v13 = *v6;
                switch ( *(_BYTE *)v6 )
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 8:
                    case 9:
                    case 0xA:
                    case 0xB:
                    case 0xC:
                    case 0xD:
                    case 0x10:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 0x14:
                    case 0x15:
                    case 0x18:
                    case 0x19:
                    case 0x1A:
                    case 0x1B:
                    case 0x1C:
                    case 0x1D:
                    case 0x20:
                    case 0x21:
                    case 0x22:
                    case 0x23:
                    case 0x24:
                    case 0x25:
                    case 0x28:
                    case 0x29:
                    case 0x2A:
                    case 0x2B:
                    case 0x2C:
                    case 0x2D:
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x38:
                    case 0x39:
                    case 0x3A:
                    case 0x3B:
                    case 0x3C:
                    case 0x3D:
                    case 0x40:
                    case 0x41:
                    case 0x42:
                    case 0x43:
                    case 0x44:
                    case 0x45:
                    case 0x48:
                    case 0x49:
                    case 0x4A:
                    case 0x4B:
                    case 0x4C:
                    case 0x4D:
                    case 0x50:
                    case 0x51:
                    case 0x52:
                    case 0x53:
                    case 0x54:
                    case 0x55:
                    case 0x58:
                    case 0x59:
                    case 0x5A:
                    case 0x5B:
                    case 0x5C:
                    case 0x5D:
                    case 0x60:
                    case 0x61:
                    case 0x62:
                    case 0x63:
                    case 0x64:
                    case 0x65:
                    case 0x68:
                    case 0x69:
                    case 0x6A:
                    case 0x6B:
                    case 0x6C:
                    case 0x6D:
                    case 0x80:
                    case 0x81:
                    case 0x82:
                    case 0x83:
                    case 0x84:
                    case 0x85:
                    case 0x88:
                    case 0x89:
                    case 0x8A:
                    case 0x8B:
                    case 0x8C:
                    case 0x8D:
                    case 0x90:
                    case 0x91:
                    case 0x92:
                    case 0x93:
                    case 0x94:
                    case 0x95:
                    case 0x98:
                    case 0x99:
                    case 0x9A:
                    case 0x9B:
                    case 0x9C:
                    case 0x9D:
                    case 0xC0:
                    case 0xC1:
                    case 0xC2:
                    case 0xC3:
                    case 0xC4:
                    case 0xC5:
                    case 0xC8:
                    case 0xC9:
                    case 0xCA:
                    case 0xCB:
                    case 0xCC:
                    case 0xCD:
                        goto LABEL_6;
                    case 6:
                        return result;
                    case 7:
                    case 0xF:
                    case 0x17:
                    case 0x1F:
                    case 0x27:
                    case 0x2F:
                    case 0x37:
                    case 0x3F:
                    case 0x47:
                    case 0x4F:
                    case 0x57:
                    case 0x5F:
                    case 0x67:
                    case 0x6F:
                    case 0x87:
                    case 0x8F:
                    case 0x97:
                    case 0x9F:
                    case 0xC7:
                    case 0xCF:
                        goto LABEL_23;
                    case 0xE:
                    case 0x16:
                        continue;
                    case 0x1E:
                    case 0x26:
                    case 0x2E:
                    case 0x36:
                    case 0x3E:
                    case 0x70:
                    case 0x71:
                    case 0x72:
                    case 0x73:
                    case 0x74:
                    case 0x75:
                    case 0x76:
                    case 0x77:
                    case 0x78:
                    case 0x79:
                    case 0x7A:
                    case 0x7B:
                    case 0x7C:
                    case 0x7D:
                    case 0x7E:
                    case 0x7F:
                    case 0xD0:
                    case 0xD1:
                    case 0xD2:
                    case 0xD3:
                    case 0xD4:
                    case 0xD5:
                    case 0xD6:
                    case 0xD7:
                    case 0xD8:
                    case 0xD9:
                    case 0xDA:
                    case 0xDB:
                    case 0xDC:
                    case 0xDD:
                    case 0xDE:
                    case 0xDF:
                        return 0LL;
                    case 0x46:
                    case 0x4E:
                    case 0x56:
                    case 0x5E:
                    case 0x66:
                    case 0x6E:
                    case 0x86:
                    case 0x8E:
                    case 0x96:
                    case 0x9E:
                    case 0xC6:
                    case 0xCE:
                        goto LABEL_21;
                    case 0xA0:
                    case 0xA1:
                    case 0xA2:
                    case 0xA3:
                    case 0xA4:
                    case 0xA5:
                    case 0xA6:
                    case 0xA7:
                    case 0xA8:
                    case 0xA9:
                    case 0xAA:
                    case 0xAB:
                    case 0xAC:
                    case 0xAD:
                    case 0xAE:
                    case 0xAF:
                    case 0xB0:
                    case 0xB1:
                    case 0xB2:
                    case 0xB3:
                    case 0xB4:
                    case 0xB5:
                    case 0xB6:
                    case 0xB7:
                    case 0xB8:
                    case 0xB9:
                    case 0xBA:
                    case 0xBB:
                    case 0xBC:
                    case 0xBD:
                    case 0xBE:
                    case 0xBF:
                        goto LABEL_25;
                    case 0xE0:
                        goto LABEL_35;
                    case 0xE1:
                    case 0xE2:
                    case 0xE3:
                    case 0xE4:
                    case 0xE5:
                    case 0xE6:
                    case 0xE7:
                    case 0xE8:
                    case 0xE9:
                    case 0xEA:
                    case 0xEB:
                    case 0xEC:
                    case 0xED:
                    case 0xEE:
                    case 0xEF:
                        goto LABEL_34;
                    case 0xF0:
                        goto LABEL_29;
                    case 0xF1:
                    case 0xF2:
                    case 0xF3:
                    case 0xF4:
                    case 0xF5:
                    case 0xF6:
                    case 0xF7:
                    case 0xF8:
                    case 0xF9:
                    case 0xFA:
                    case 0xFB:
                    case 0xFC:
                    case 0xFD:
                    case 0xFE:
                    case 0xFF:
                        goto LABEL_27;
                }
            }
            return result;
        case 0x1E:
        case 0x26:
        case 0x2E:
        case 0x36:
        case 0x3E:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
            return 0LL;
        case 0x46:
        case 0x4E:
        case 0x56:
        case 0x5E:
        case 0x66:
        case 0x6E:
        case 0x86:
        case 0x8E:
        case 0x96:
        case 0x9E:
        case 0xC6:
        case 0xCE:
        LABEL_21:
            v14 = v12 >> 6;
            v6 = (UINT64 *)((char *)v6 + v14 + 1);
            if ( (UINT64)v6 > v11 )
                return 0LL;
            v22 = v13 & 0x38;
            v16 = v13 >> 8;
            v17 = (v22 >> 3) + 3;
            goto LABEL_8;
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAC:
        case 0xAD:
        case 0xAE:
        case 0xAF:
        case 0xB0:
        case 0xB1:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xB7:
        case 0xB8:
        case 0xB9:
        case 0xBA:
        case 0xBB:
        case 0xBC:
        case 0xBD:
        case 0xBE:
        case 0xBF:
        LABEL_25:
            while ( 2 )
            {
                while ( 2 )
                {
                    v14 = (v12 >> 3) & 3;
                    v6 = (UINT64 *)((char *)v6 + v14 + 3);
                    if ( (UINT64)v6 > v11 )
                        return 0LL;
                    v25 = v13 & 0x307;
                    v26 = v13 >> 10;
                    v17 = ((4LL * (UINT8)v25) | (v25 >> 8)) + 3;
                    v8 = v26 & 0x3FFF;
                    v16 = v26 >> 14;
                LABEL_8:
                    if ( v17 + result + v14 >= v10 )
                    {
                        for ( ; v14; --v14 )
                        {
                            *(_BYTE *)(a1 + result++) = v16;
                            if ( v10 + 8 == result )
                                return result;
                            v16 >>= 8;
                        }
                        v18 = result - v8;
                        if ( result < v8 )
                            return 0LL;
                    }
                    else
                    {
                        *(_QWORD *)(a1 + result) = v16;
                        result += v14;
                        v18 = result - v8;
                        if ( result < v8 )
                            return 0LL;
                        if ( v8 >= 8 )
                        {
                            do
                            {
                            LABEL_11:
                                v19 = *(_QWORD *)(a1 + v18);
                                v18 += 8LL;
                                *(_QWORD *)(a1 + result) = v19;
                                result += 8LL;
                                v20 = v17 <= 8;
                                v17 -= 8LL;
                            }
                            while ( !v20 );
                            result += v17;
                            v12 = *(UINT8 *)v6;
                            v13 = *v6;
                            switch ( *(_BYTE *)v6 )
                            {
                                case 0:
                                case 1:
                                case 2:
                                case 3:
                                case 4:
                                case 5:
                                case 8:
                                case 9:
                                case 0xA:
                                case 0xB:
                                case 0xC:
                                case 0xD:
                                case 0x10:
                                case 0x11:
                                case 0x12:
                                case 0x13:
                                case 0x14:
                                case 0x15:
                                case 0x18:
                                case 0x19:
                                case 0x1A:
                                case 0x1B:
                                case 0x1C:
                                case 0x1D:
                                case 0x20:
                                case 0x21:
                                case 0x22:
                                case 0x23:
                                case 0x24:
                                case 0x25:
                                case 0x28:
                                case 0x29:
                                case 0x2A:
                                case 0x2B:
                                case 0x2C:
                                case 0x2D:
                                case 0x30:
                                case 0x31:
                                case 0x32:
                                case 0x33:
                                case 0x34:
                                case 0x35:
                                case 0x38:
                                case 0x39:
                                case 0x3A:
                                case 0x3B:
                                case 0x3C:
                                case 0x3D:
                                case 0x40:
                                case 0x41:
                                case 0x42:
                                case 0x43:
                                case 0x44:
                                case 0x45:
                                case 0x48:
                                case 0x49:
                                case 0x4A:
                                case 0x4B:
                                case 0x4C:
                                case 0x4D:
                                case 0x50:
                                case 0x51:
                                case 0x52:
                                case 0x53:
                                case 0x54:
                                case 0x55:
                                case 0x58:
                                case 0x59:
                                case 0x5A:
                                case 0x5B:
                                case 0x5C:
                                case 0x5D:
                                case 0x60:
                                case 0x61:
                                case 0x62:
                                case 0x63:
                                case 0x64:
                                case 0x65:
                                case 0x68:
                                case 0x69:
                                case 0x6A:
                                case 0x6B:
                                case 0x6C:
                                case 0x6D:
                                case 0x80:
                                case 0x81:
                                case 0x82:
                                case 0x83:
                                case 0x84:
                                case 0x85:
                                case 0x88:
                                case 0x89:
                                case 0x8A:
                                case 0x8B:
                                case 0x8C:
                                case 0x8D:
                                case 0x90:
                                case 0x91:
                                case 0x92:
                                case 0x93:
                                case 0x94:
                                case 0x95:
                                case 0x98:
                                case 0x99:
                                case 0x9A:
                                case 0x9B:
                                case 0x9C:
                                case 0x9D:
                                case 0xC0:
                                case 0xC1:
                                case 0xC2:
                                case 0xC3:
                                case 0xC4:
                                case 0xC5:
                                case 0xC8:
                                case 0xC9:
                                case 0xCA:
                                case 0xCB:
                                case 0xCC:
                                case 0xCD:
                                    goto LABEL_6;
                                case 6:
                                    return result;
                                case 7:
                                case 0xF:
                                case 0x17:
                                case 0x1F:
                                case 0x27:
                                case 0x2F:
                                case 0x37:
                                case 0x3F:
                                case 0x47:
                                case 0x4F:
                                case 0x57:
                                case 0x5F:
                                case 0x67:
                                case 0x6F:
                                case 0x87:
                                case 0x8F:
                                case 0x97:
                                case 0x9F:
                                case 0xC7:
                                case 0xCF:
                                    goto LABEL_23;
                                case 0xE:
                                case 0x16:
                                    goto LABEL_4;
                                case 0x1E:
                                case 0x26:
                                case 0x2E:
                                case 0x36:
                                case 0x3E:
                                case 0x70:
                                case 0x71:
                                case 0x72:
                                case 0x73:
                                case 0x74:
                                case 0x75:
                                case 0x76:
                                case 0x77:
                                case 0x78:
                                case 0x79:
                                case 0x7A:
                                case 0x7B:
                                case 0x7C:
                                case 0x7D:
                                case 0x7E:
                                case 0x7F:
                                case 0xD0:
                                case 0xD1:
                                case 0xD2:
                                case 0xD3:
                                case 0xD4:
                                case 0xD5:
                                case 0xD6:
                                case 0xD7:
                                case 0xD8:
                                case 0xD9:
                                case 0xDA:
                                case 0xDB:
                                case 0xDC:
                                case 0xDD:
                                case 0xDE:
                                case 0xDF:
                                    return 0LL;
                                case 0x46:
                                case 0x4E:
                                case 0x56:
                                case 0x5E:
                                case 0x66:
                                case 0x6E:
                                case 0x86:
                                case 0x8E:
                                case 0x96:
                                case 0x9E:
                                case 0xC6:
                                case 0xCE:
                                    goto LABEL_21;
                                case 0xA0:
                                case 0xA1:
                                case 0xA2:
                                case 0xA3:
                                case 0xA4:
                                case 0xA5:
                                case 0xA6:
                                case 0xA7:
                                case 0xA8:
                                case 0xA9:
                                case 0xAA:
                                case 0xAB:
                                case 0xAC:
                                case 0xAD:
                                case 0xAE:
                                case 0xAF:
                                case 0xB0:
                                case 0xB1:
                                case 0xB2:
                                case 0xB3:
                                case 0xB4:
                                case 0xB5:
                                case 0xB6:
                                case 0xB7:
                                case 0xB8:
                                case 0xB9:
                                case 0xBA:
                                case 0xBB:
                                case 0xBC:
                                case 0xBD:
                                case 0xBE:
                                case 0xBF:
                                    continue;
                                case 0xE0:
                                    goto LABEL_35;
                                case 0xE1:
                                case 0xE2:
                                case 0xE3:
                                case 0xE4:
                                case 0xE5:
                                case 0xE6:
                                case 0xE7:
                                case 0xE8:
                                case 0xE9:
                                case 0xEA:
                                case 0xEB:
                                case 0xEC:
                                case 0xED:
                                case 0xEE:
                                case 0xEF:
                                    goto LABEL_34;
                                case 0xF0:
                                    goto LABEL_29;
                                case 0xF1:
                                case 0xF2:
                                case 0xF3:
                                case 0xF4:
                                case 0xF5:
                                case 0xF6:
                                case 0xF7:
                                case 0xF8:
                                case 0xF9:
                                case 0xFA:
                                case 0xFB:
                                case 0xFC:
                                case 0xFD:
                                case 0xFE:
                                case 0xFF:
                                    goto LABEL_27;
                            }
                            return result;
                        }
                    }
                    break;
                }
            LABEL_17:
                if ( !v8 )
                    return 0LL;
                do
                {
                    v21 = *(_BYTE *)(a1 + v18++);
                    *(_BYTE *)(a1 + result++) = v21;
                    if ( v10 + 8 == result )
                        return result;
                    --v17;
                }
                while ( v17 );
                v12 = *(UINT8 *)v6;
                v13 = *v6;
                switch ( *(_BYTE *)v6 )
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 8:
                    case 9:
                    case 0xA:
                    case 0xB:
                    case 0xC:
                    case 0xD:
                    case 0x10:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 0x14:
                    case 0x15:
                    case 0x18:
                    case 0x19:
                    case 0x1A:
                    case 0x1B:
                    case 0x1C:
                    case 0x1D:
                    case 0x20:
                    case 0x21:
                    case 0x22:
                    case 0x23:
                    case 0x24:
                    case 0x25:
                    case 0x28:
                    case 0x29:
                    case 0x2A:
                    case 0x2B:
                    case 0x2C:
                    case 0x2D:
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x38:
                    case 0x39:
                    case 0x3A:
                    case 0x3B:
                    case 0x3C:
                    case 0x3D:
                    case 0x40:
                    case 0x41:
                    case 0x42:
                    case 0x43:
                    case 0x44:
                    case 0x45:
                    case 0x48:
                    case 0x49:
                    case 0x4A:
                    case 0x4B:
                    case 0x4C:
                    case 0x4D:
                    case 0x50:
                    case 0x51:
                    case 0x52:
                    case 0x53:
                    case 0x54:
                    case 0x55:
                    case 0x58:
                    case 0x59:
                    case 0x5A:
                    case 0x5B:
                    case 0x5C:
                    case 0x5D:
                    case 0x60:
                    case 0x61:
                    case 0x62:
                    case 0x63:
                    case 0x64:
                    case 0x65:
                    case 0x68:
                    case 0x69:
                    case 0x6A:
                    case 0x6B:
                    case 0x6C:
                    case 0x6D:
                    case 0x80:
                    case 0x81:
                    case 0x82:
                    case 0x83:
                    case 0x84:
                    case 0x85:
                    case 0x88:
                    case 0x89:
                    case 0x8A:
                    case 0x8B:
                    case 0x8C:
                    case 0x8D:
                    case 0x90:
                    case 0x91:
                    case 0x92:
                    case 0x93:
                    case 0x94:
                    case 0x95:
                    case 0x98:
                    case 0x99:
                    case 0x9A:
                    case 0x9B:
                    case 0x9C:
                    case 0x9D:
                    case 0xC0:
                    case 0xC1:
                    case 0xC2:
                    case 0xC3:
                    case 0xC4:
                    case 0xC5:
                    case 0xC8:
                    case 0xC9:
                    case 0xCA:
                    case 0xCB:
                    case 0xCC:
                    case 0xCD:
                        goto LABEL_6;
                    case 6:
                        return result;
                    case 7:
                    case 0xF:
                    case 0x17:
                    case 0x1F:
                    case 0x27:
                    case 0x2F:
                    case 0x37:
                    case 0x3F:
                    case 0x47:
                    case 0x4F:
                    case 0x57:
                    case 0x5F:
                    case 0x67:
                    case 0x6F:
                    case 0x87:
                    case 0x8F:
                    case 0x97:
                    case 0x9F:
                    case 0xC7:
                    case 0xCF:
                        goto LABEL_23;
                    case 0xE:
                    case 0x16:
                        goto LABEL_4;
                    case 0x1E:
                    case 0x26:
                    case 0x2E:
                    case 0x36:
                    case 0x3E:
                    case 0x70:
                    case 0x71:
                    case 0x72:
                    case 0x73:
                    case 0x74:
                    case 0x75:
                    case 0x76:
                    case 0x77:
                    case 0x78:
                    case 0x79:
                    case 0x7A:
                    case 0x7B:
                    case 0x7C:
                    case 0x7D:
                    case 0x7E:
                    case 0x7F:
                    case 0xD0:
                    case 0xD1:
                    case 0xD2:
                    case 0xD3:
                    case 0xD4:
                    case 0xD5:
                    case 0xD6:
                    case 0xD7:
                    case 0xD8:
                    case 0xD9:
                    case 0xDA:
                    case 0xDB:
                    case 0xDC:
                    case 0xDD:
                    case 0xDE:
                    case 0xDF:
                        return 0LL;
                    case 0x46:
                    case 0x4E:
                    case 0x56:
                    case 0x5E:
                    case 0x66:
                    case 0x6E:
                    case 0x86:
                    case 0x8E:
                    case 0x96:
                    case 0x9E:
                    case 0xC6:
                    case 0xCE:
                        goto LABEL_21;
                    case 0xA0:
                    case 0xA1:
                    case 0xA2:
                    case 0xA3:
                    case 0xA4:
                    case 0xA5:
                    case 0xA6:
                    case 0xA7:
                    case 0xA8:
                    case 0xA9:
                    case 0xAA:
                    case 0xAB:
                    case 0xAC:
                    case 0xAD:
                    case 0xAE:
                    case 0xAF:
                    case 0xB0:
                    case 0xB1:
                    case 0xB2:
                    case 0xB3:
                    case 0xB4:
                    case 0xB5:
                    case 0xB6:
                    case 0xB7:
                    case 0xB8:
                    case 0xB9:
                    case 0xBA:
                    case 0xBB:
                    case 0xBC:
                    case 0xBD:
                    case 0xBE:
                    case 0xBF:
                        continue;
                    case 0xE0:
                        goto LABEL_35;
                    case 0xE1:
                    case 0xE2:
                    case 0xE3:
                    case 0xE4:
                    case 0xE5:
                    case 0xE6:
                    case 0xE7:
                    case 0xE8:
                    case 0xE9:
                    case 0xEA:
                    case 0xEB:
                    case 0xEC:
                    case 0xED:
                    case 0xEE:
                    case 0xEF:
                        goto LABEL_34;
                    case 0xF0:
                        goto LABEL_29;
                    case 0xF1:
                    case 0xF2:
                    case 0xF3:
                    case 0xF4:
                    case 0xF5:
                    case 0xF6:
                    case 0xF7:
                    case 0xF8:
                    case 0xF9:
                    case 0xFA:
                    case 0xFB:
                    case 0xFC:
                    case 0xFD:
                    case 0xFE:
                    case 0xFF:
                        goto LABEL_27;
                }
            }
            return result;
        case 0xE0:
        LABEL_35:
            v27 = BYTE1(v13) + 16LL;
            v6 = (UINT64 *)((char *)v6 + v27 + 2);
            goto LABEL_36;
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
        LABEL_34:
            while ( 2 )
            {
                v27 = v13 & 0xF;
                v6 = (UINT64 *)((char *)v6 + v27 + 1);
            LABEL_36:
                if ( (UINT64)v6 > v11 )
                    return 0LL;
                v28 = result + v27;
                v29 = -v27;
                if ( v28 > v10 )
                {
                    do
                    {
                        *(_BYTE *)(a1 + result++) = *((_BYTE *)v6 + v29);
                        if ( v10 + 8 == result )
                            return result;
                        ++v29;
                    }
                    while ( v29 );
                    v12 = *(UINT8 *)v6;
                    v13 = *v6;
                    switch ( *(_BYTE *)v6 )
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 8:
                        case 9:
                        case 0xA:
                        case 0xB:
                        case 0xC:
                        case 0xD:
                        case 0x10:
                        case 0x11:
                        case 0x12:
                        case 0x13:
                        case 0x14:
                        case 0x15:
                        case 0x18:
                        case 0x19:
                        case 0x1A:
                        case 0x1B:
                        case 0x1C:
                        case 0x1D:
                        case 0x20:
                        case 0x21:
                        case 0x22:
                        case 0x23:
                        case 0x24:
                        case 0x25:
                        case 0x28:
                        case 0x29:
                        case 0x2A:
                        case 0x2B:
                        case 0x2C:
                        case 0x2D:
                        case 0x30:
                        case 0x31:
                        case 0x32:
                        case 0x33:
                        case 0x34:
                        case 0x35:
                        case 0x38:
                        case 0x39:
                        case 0x3A:
                        case 0x3B:
                        case 0x3C:
                        case 0x3D:
                        case 0x40:
                        case 0x41:
                        case 0x42:
                        case 0x43:
                        case 0x44:
                        case 0x45:
                        case 0x48:
                        case 0x49:
                        case 0x4A:
                        case 0x4B:
                        case 0x4C:
                        case 0x4D:
                        case 0x50:
                        case 0x51:
                        case 0x52:
                        case 0x53:
                        case 0x54:
                        case 0x55:
                        case 0x58:
                        case 0x59:
                        case 0x5A:
                        case 0x5B:
                        case 0x5C:
                        case 0x5D:
                        case 0x60:
                        case 0x61:
                        case 0x62:
                        case 0x63:
                        case 0x64:
                        case 0x65:
                        case 0x68:
                        case 0x69:
                        case 0x6A:
                        case 0x6B:
                        case 0x6C:
                        case 0x6D:
                        case 0x80:
                        case 0x81:
                        case 0x82:
                        case 0x83:
                        case 0x84:
                        case 0x85:
                        case 0x88:
                        case 0x89:
                        case 0x8A:
                        case 0x8B:
                        case 0x8C:
                        case 0x8D:
                        case 0x90:
                        case 0x91:
                        case 0x92:
                        case 0x93:
                        case 0x94:
                        case 0x95:
                        case 0x98:
                        case 0x99:
                        case 0x9A:
                        case 0x9B:
                        case 0x9C:
                        case 0x9D:
                        case 0xC0:
                        case 0xC1:
                        case 0xC2:
                        case 0xC3:
                        case 0xC4:
                        case 0xC5:
                        case 0xC8:
                        case 0xC9:
                        case 0xCA:
                        case 0xCB:
                        case 0xCC:
                        case 0xCD:
                            goto LABEL_6;
                        case 6:
                            return result;
                        case 7:
                        case 0xF:
                        case 0x17:
                        case 0x1F:
                        case 0x27:
                        case 0x2F:
                        case 0x37:
                        case 0x3F:
                        case 0x47:
                        case 0x4F:
                        case 0x57:
                        case 0x5F:
                        case 0x67:
                        case 0x6F:
                        case 0x87:
                        case 0x8F:
                        case 0x97:
                        case 0x9F:
                        case 0xC7:
                        case 0xCF:
                            goto LABEL_23;
                        case 0xE:
                        case 0x16:
                            goto LABEL_4;
                        case 0x1E:
                        case 0x26:
                        case 0x2E:
                        case 0x36:
                        case 0x3E:
                        case 0x70:
                        case 0x71:
                        case 0x72:
                        case 0x73:
                        case 0x74:
                        case 0x75:
                        case 0x76:
                        case 0x77:
                        case 0x78:
                        case 0x79:
                        case 0x7A:
                        case 0x7B:
                        case 0x7C:
                        case 0x7D:
                        case 0x7E:
                        case 0x7F:
                        case 0xD0:
                        case 0xD1:
                        case 0xD2:
                        case 0xD3:
                        case 0xD4:
                        case 0xD5:
                        case 0xD6:
                        case 0xD7:
                        case 0xD8:
                        case 0xD9:
                        case 0xDA:
                        case 0xDB:
                        case 0xDC:
                        case 0xDD:
                        case 0xDE:
                        case 0xDF:
                            return 0LL;
                        case 0x46:
                        case 0x4E:
                        case 0x56:
                        case 0x5E:
                        case 0x66:
                        case 0x6E:
                        case 0x86:
                        case 0x8E:
                        case 0x96:
                        case 0x9E:
                        case 0xC6:
                        case 0xCE:
                            goto LABEL_21;
                        case 0xA0:
                        case 0xA1:
                        case 0xA2:
                        case 0xA3:
                        case 0xA4:
                        case 0xA5:
                        case 0xA6:
                        case 0xA7:
                        case 0xA8:
                        case 0xA9:
                        case 0xAA:
                        case 0xAB:
                        case 0xAC:
                        case 0xAD:
                        case 0xAE:
                        case 0xAF:
                        case 0xB0:
                        case 0xB1:
                        case 0xB2:
                        case 0xB3:
                        case 0xB4:
                        case 0xB5:
                        case 0xB6:
                        case 0xB7:
                        case 0xB8:
                        case 0xB9:
                        case 0xBA:
                        case 0xBB:
                        case 0xBC:
                        case 0xBD:
                        case 0xBE:
                        case 0xBF:
                            goto LABEL_25;
                        case 0xE0:
                            goto LABEL_35;
                        case 0xE1:
                        case 0xE2:
                        case 0xE3:
                        case 0xE4:
                        case 0xE5:
                        case 0xE6:
                        case 0xE7:
                        case 0xE8:
                        case 0xE9:
                        case 0xEA:
                        case 0xEB:
                        case 0xEC:
                        case 0xED:
                        case 0xEE:
                        case 0xEF:
                            continue;
                        case 0xF0:
                            goto LABEL_29;
                        case 0xF1:
                        case 0xF2:
                        case 0xF3:
                        case 0xF4:
                        case 0xF5:
                        case 0xF6:
                        case 0xF7:
                        case 0xF8:
                        case 0xF9:
                        case 0xFA:
                        case 0xFB:
                        case 0xFC:
                        case 0xFD:
                        case 0xFE:
                        case 0xFF:
                            goto LABEL_27;
                    }
                }
                else
                {
                    v30 = a1 + v28;
                    do
                    {
                        *(_QWORD *)(v30 + v29) = *(UINT64 *)((char *)v6 + v29);
                        v9 = __CFADD__(v29, 8LL);
                        v29 += 8LL;
                    }
                    while ( !v9 );
                    result = v30 - a1;
                    v12 = *(UINT8 *)v6;
                    v13 = *v6;
                    switch ( *(_BYTE *)v6 )
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 8:
                        case 9:
                        case 0xA:
                        case 0xB:
                        case 0xC:
                        case 0xD:
                        case 0x10:
                        case 0x11:
                        case 0x12:
                        case 0x13:
                        case 0x14:
                        case 0x15:
                        case 0x18:
                        case 0x19:
                        case 0x1A:
                        case 0x1B:
                        case 0x1C:
                        case 0x1D:
                        case 0x20:
                        case 0x21:
                        case 0x22:
                        case 0x23:
                        case 0x24:
                        case 0x25:
                        case 0x28:
                        case 0x29:
                        case 0x2A:
                        case 0x2B:
                        case 0x2C:
                        case 0x2D:
                        case 0x30:
                        case 0x31:
                        case 0x32:
                        case 0x33:
                        case 0x34:
                        case 0x35:
                        case 0x38:
                        case 0x39:
                        case 0x3A:
                        case 0x3B:
                        case 0x3C:
                        case 0x3D:
                        case 0x40:
                        case 0x41:
                        case 0x42:
                        case 0x43:
                        case 0x44:
                        case 0x45:
                        case 0x48:
                        case 0x49:
                        case 0x4A:
                        case 0x4B:
                        case 0x4C:
                        case 0x4D:
                        case 0x50:
                        case 0x51:
                        case 0x52:
                        case 0x53:
                        case 0x54:
                        case 0x55:
                        case 0x58:
                        case 0x59:
                        case 0x5A:
                        case 0x5B:
                        case 0x5C:
                        case 0x5D:
                        case 0x60:
                        case 0x61:
                        case 0x62:
                        case 0x63:
                        case 0x64:
                        case 0x65:
                        case 0x68:
                        case 0x69:
                        case 0x6A:
                        case 0x6B:
                        case 0x6C:
                        case 0x6D:
                        case 0x80:
                        case 0x81:
                        case 0x82:
                        case 0x83:
                        case 0x84:
                        case 0x85:
                        case 0x88:
                        case 0x89:
                        case 0x8A:
                        case 0x8B:
                        case 0x8C:
                        case 0x8D:
                        case 0x90:
                        case 0x91:
                        case 0x92:
                        case 0x93:
                        case 0x94:
                        case 0x95:
                        case 0x98:
                        case 0x99:
                        case 0x9A:
                        case 0x9B:
                        case 0x9C:
                        case 0x9D:
                        case 0xC0:
                        case 0xC1:
                        case 0xC2:
                        case 0xC3:
                        case 0xC4:
                        case 0xC5:
                        case 0xC8:
                        case 0xC9:
                        case 0xCA:
                        case 0xCB:
                        case 0xCC:
                        case 0xCD:
                            goto LABEL_6;
                        case 6:
                            return result;
                        case 7:
                        case 0xF:
                        case 0x17:
                        case 0x1F:
                        case 0x27:
                        case 0x2F:
                        case 0x37:
                        case 0x3F:
                        case 0x47:
                        case 0x4F:
                        case 0x57:
                        case 0x5F:
                        case 0x67:
                        case 0x6F:
                        case 0x87:
                        case 0x8F:
                        case 0x97:
                        case 0x9F:
                        case 0xC7:
                        case 0xCF:
                            goto LABEL_23;
                        case 0xE:
                        case 0x16:
                            goto LABEL_4;
                        case 0x1E:
                        case 0x26:
                        case 0x2E:
                        case 0x36:
                        case 0x3E:
                        case 0x70:
                        case 0x71:
                        case 0x72:
                        case 0x73:
                        case 0x74:
                        case 0x75:
                        case 0x76:
                        case 0x77:
                        case 0x78:
                        case 0x79:
                        case 0x7A:
                        case 0x7B:
                        case 0x7C:
                        case 0x7D:
                        case 0x7E:
                        case 0x7F:
                        case 0xD0:
                        case 0xD1:
                        case 0xD2:
                        case 0xD3:
                        case 0xD4:
                        case 0xD5:
                        case 0xD6:
                        case 0xD7:
                        case 0xD8:
                        case 0xD9:
                        case 0xDA:
                        case 0xDB:
                        case 0xDC:
                        case 0xDD:
                        case 0xDE:
                        case 0xDF:
                            return 0LL;
                        case 0x46:
                        case 0x4E:
                        case 0x56:
                        case 0x5E:
                        case 0x66:
                        case 0x6E:
                        case 0x86:
                        case 0x8E:
                        case 0x96:
                        case 0x9E:
                        case 0xC6:
                        case 0xCE:
                            goto LABEL_21;
                        case 0xA0:
                        case 0xA1:
                        case 0xA2:
                        case 0xA3:
                        case 0xA4:
                        case 0xA5:
                        case 0xA6:
                        case 0xA7:
                        case 0xA8:
                        case 0xA9:
                        case 0xAA:
                        case 0xAB:
                        case 0xAC:
                        case 0xAD:
                        case 0xAE:
                        case 0xAF:
                        case 0xB0:
                        case 0xB1:
                        case 0xB2:
                        case 0xB3:
                        case 0xB4:
                        case 0xB5:
                        case 0xB6:
                        case 0xB7:
                        case 0xB8:
                        case 0xB9:
                        case 0xBA:
                        case 0xBB:
                        case 0xBC:
                        case 0xBD:
                        case 0xBE:
                        case 0xBF:
                            goto LABEL_25;
                        case 0xE0:
                            goto LABEL_35;
                        case 0xE1:
                        case 0xE2:
                        case 0xE3:
                        case 0xE4:
                        case 0xE5:
                        case 0xE6:
                        case 0xE7:
                        case 0xE8:
                        case 0xE9:
                        case 0xEA:
                        case 0xEB:
                        case 0xEC:
                        case 0xED:
                        case 0xEE:
                        case 0xEF:
                            continue;
                        case 0xF0:
                            goto LABEL_29;
                        case 0xF1:
                        case 0xF2:
                        case 0xF3:
                        case 0xF4:
                        case 0xF5:
                        case 0xF6:
                        case 0xF7:
                        case 0xF8:
                        case 0xF9:
                        case 0xFA:
                        case 0xFB:
                        case 0xFC:
                        case 0xFD:
                        case 0xFE:
                        case 0xFF:
                            goto LABEL_27;
                    }
                }
            }
        case 0xF0:
        LABEL_29:
            v6 = (UINT64 *)((char *)v6 + 2);
            if ( (UINT64)v6 > v11 )
                return 0LL;
            v17 = BYTE1(v13) + 16LL;
            goto LABEL_31;
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
        LABEL_27:
            v6 = (UINT64 *)((char *)v6 + 1);
            if ( (UINT64)v6 > v11 )
                return 0LL;
            v17 = v13 & 0xF;
        LABEL_31:
            v18 = result - v8;
            if ( result + v17 >= v10 || v8 < 8 )
                goto LABEL_17;
            goto LABEL_11;
    }
    return result;
}

_BYTE* sub_1BA91(_BYTE *a1, char *a2, int a3)
{
    UINT64 i; // rax
    UINT64 v4; // r8
    UINT64 v5; // r15
    UINT64 v6; // r11
    _BYTE *v7; // rax
    UINT64 v8; // r11
    UINT64 v9; // rdi
    UINT64 v10; // rsi
    char v11; // bl
    char v12; // bl
    
    for ( i = 0LL; i != 4078; ++i )
        byte_AF1B0[i] = 32;
    v4 = (UINT64)&a2[a3];
    v5 = 4078LL;
    v6 = 0LL;
    v7 = a1;
    while ( 1 )
    {
        while ( 1 )
        {
            if ( (v6 & 0x200) != 0 )
            {
                v6 >>= 1;
            }
            else
            {
                if ( (UINT64)a2 >= v4 )
                    return (_BYTE *)(v7 - a1);
                v8 = (UINT8)*a2++;
                v6 = v8 | 0xFF00;
            }
            if ( (v6 & 1) == 0 )
                break;
            if ( (UINT64)a2 >= v4 )
                return (_BYTE *)(v7 - a1);
            v12 = *a2++;
            *v7++ = v12;
            byte_AF1B0[v5] = v12;
            v5 = ((_WORD)v5 + 1) & 0xFFF;
        }
        if ( (UINT64)a2 >= v4 || (UINT64)(a2 + 1) >= v4 )
            break;
        v9 = (UINT8)*a2 | (UINT64)((16 * (UINT8)a2[1]) & 0xF00);
        v10 = (a2[1] & 0xF) + 3LL;
        do
        {
            v11 = byte_AF1B0[v9 & 0xFFF];
            *v7++ = v11;
            byte_AF1B0[v5] = v11;
            v5 = ((int)v5 + 1) & 0xFFFLL;
            ++v9;
            --v10;
        }
        while ( v10 );
        a2 += 2;
    }
    return (_BYTE *)(v7 - a1);
}

UINT64 sub_6955(UINT64 a1, UINT64 *a2, _QWORD *a3, _QWORD *a4)
{
    UINT64 v4; // rsi
    UINT64 v8; // r14
    void* v9; // r12
    char v10; // al
    UINT64 v11; // rax
    UINT64 v12; // r13
    char v13; // al
    UINT64 v14; // rax
    unsigned int v15; // r8d
    UINT64 v16; // rcx
    UINT64 v17; // rdx
    UINT64 v18; // rbx
    UINT64 v19; // rsi
    char v20; // bl
    _QWORD *v23; // [rsp+30h] [rbp-40h]
    
    v4 = 0x8000000000000002uLL;
    if ( !a1 || !a2 )
        return v4;
    v8 = *(UINT16 *)(a1 + 16) * (UINT64)*(UINT16 *)(a1 + 18);
    v9 = sub_1D2B1((UINTN)v8);
    v10 = *(_BYTE *)(a1 + 20);
    if ( v10 )
    {
        if ( v10 != 1 )
            goto LABEL_13;
        v11 = sub_28800(v9, v8, *(_QWORD *)(a1 + 24), *(unsigned int *)(a1 + 32));
    }
    else
    {
        v11 = sub_1BA91(v9, *(_QWORD *)(a1 + 24), *(unsigned int *)(a1 + 32));
    }
    if ( v11 && v11 == v8 )
    {
        v23 = a4;
        if ( *(_QWORD *)(a1 + 40) )
        {
            v12 = sub_1D2B1(v8);
            v13 = *(_BYTE *)(a1 + 20);
            if ( v13 )
            {
                if ( v13 != 1 )
                    goto LABEL_26;
                v14 = sub_28800(v12, v8, *(_QWORD *)(a1 + 40), *(unsigned int *)(a1 + 48));
            }
            else
            {
                v14 = sub_1BA91(v12, *(_QWORD *)(a1 + 40), *(unsigned int *)(a1 + 48));
            }
            if ( !v14 || v14 != v8 )
                goto LABEL_26;
        }
        else
        {
            v12 = 0LL;
        }
        *a2 = sub_1D2B1(4 * v8);
        if ( !v8 )
        {
        LABEL_25:
            *a3 = *(UINT16 *)(a1 + 16);
            *v23 = *(UINT16 *)(a1 + 18);
            v4 = 0LL;
            goto LABEL_27;
        }
        v15 = *(_DWORD *)(a1 + 64);
        v16 = 0LL;
        while ( 1 )
        {
            v17 = *(UINT8 *)(v9 + v16);
            if ( v15 <= (unsigned int)v17 )
                break;
            v18 = *(_QWORD *)(a1 + 56);
            v19 = *a2;
            *(_BYTE *)(v19 + 4 * v16 + 2) = *(_BYTE *)(v18 + 3 * v17);
            *(_BYTE *)(v19 + 4 * v16 + 1) = *(_BYTE *)(v18 + 3 * v17 + 1);
            *(_BYTE *)(v19 + 4 * v16) = *(_BYTE *)(v18 + 3 * v17 + 2);
            if ( v12 )
                v20 = *(_BYTE *)(v12 + v16);
            else
                v20 = -1;
            *(_BYTE *)(v19 + 4 * v16++ + 3) = v20;
            if ( v8 == v16 )
                goto LABEL_25;
        }
    LABEL_26:
        v4 = 0x8000000000000001uLL;
    LABEL_27:
        if ( v12 )
            sub_1D327(v12);
        goto LABEL_29;
    }
LABEL_13:
    v4 = 0x8000000000000001uLL;
LABEL_29:
    if ( v9 )
        sub_1D327(v9);
    return v4;
}

UINT64 sub_6B00(char *a1, UINT64 a2, UINT64 a3, UINT64 a4)
{
    
    UINT64* v4 = (UINT64*)off_AD210;
    if ( !off_AD210 )
        return 0x800000000000000EuLL;
    while ( (unsigned int)sub_28246(a1, (char*)v4[1]) )
    {
        v4 = (UINT64*)*v4;
        if ( !v4 )
            return 0x800000000000000EuLL;
    }
    return sub_6955(v4, a2, a3, a4);
}

void sub_16D7E(UINT64 a1, UINT64 a2)
{
    UINT64 i; // rax
    UINT16 v3; // r8
    
    if ( a1 && a2 )
    {
        for ( i = 0LL; i != a2; ++i )
        {
            v3 = *(UINT8 *)(a1 + 4 * i + 3);
            *(_BYTE *)(a1 + 4 * i + 2) = (UINT16)(v3 * *(UINT8 *)(a1 + 4 * i + 2)
                                                  + (v3 ^ 0xFF) * BYTE2(dword_AD818)) >> 8;
            *(_BYTE *)(a1 + 4 * i + 1) = (UINT16)(v3 * *(UINT8 *)(a1 + 4 * i + 1)
                                                  + (v3 ^ 0xFF) * BYTE1(dword_AD818)) >> 8;
            *(_BYTE *)(a1 + 4 * i) = (UINT16)(v3 * *(UINT8 *)(a1 + 4 * i)
                                              + (v3 ^ 0xFF) * (UINT8)dword_AD818) >> 8;
            *(_BYTE *)(a1 + 4 * i + 3) = -1;
        }
    }
}

UINT64 sub_15CE7(UINT64 a1, UINT64 a2, UINT64 a3, UINT64 a4, UINT64 a5)
{
    UINT64 result; // rax
    
    result = qword_B1F20;
    if ( qword_B1F20 )
        return (*(UINT64 (**)(UINT64, UINT64, UINT64, _QWORD, _QWORD, UINT64, UINT64, UINT64, UINT64, _QWORD))(result + 16))(
                                                                                                                             result,
                                                                                                                             a5,
                                                                                                                             2LL,
                                                                                                                             0LL,
                                                                                                                             0LL,
                                                                                                                             a1,
                                                                                                                             a2,
                                                                                                                             a3,
                                                                                                                             a4,
                                                                                                                             0LL);
    result = qword_B1F18;
    if ( qword_B1F18 )
        return (*(UINT64 (**)(UINT64, UINT64, UINT64, _QWORD, _QWORD, UINT64, UINT64, UINT64, UINT64, _QWORD))(result + 16))(
                                                                                                                             result,
                                                                                                                             a5,
                                                                                                                             2LL,
                                                                                                                             0LL,
                                                                                                                             0LL,
                                                                                                                             a1,
                                                                                                                             a2,
                                                                                                                             a3,
                                                                                                                             a4,
                                                                                                                             0LL);
    return result;
}

UINT64 sub_1702B(void)
{
    UINT64 v0; // r10
    
    v0 = 0LL;
    if ( (UINT64)(qword_AF168 + 1) <= 0x11 )
        v0 = qword_AF168 + 1;
    qword_AF168 = v0;
    return sub_15CE7(
                     ((UINT64)(unsigned int)dword_AF110 - qword_AF118) >> 1,
                     qword_AF128 + (((UINT64)(unsigned int)dword_AF114 - qword_AF120) >> 1),
                     qword_AF118,
                     qword_AF120,
                     qword_AF160 + 4 * v0 * qword_AF118 * qword_AF120);
}

UINT64 sub_16EC5(void)
{
    UINT8 v0; // cf
    const char *v1; // rdx
    const char *v2; // rcx
    UINT64 v3; // rax
    UINT64 result; // rax
    
    v0 = _bittest64((UINT64 *)&qword_B1DE8, 0x12u);
    v1 = "NetBootBlack";
    if ( !v0 )
        v1 = "NetBoot";
    v2 = "NetBootBlack2X";
    if ( !v0 )
        v2 = "NetBoot2X";
    v3 = 400LL;
    if ( !byte_B1F30 )
    {
        v3 = 200LL;
        v2 = v1;
    }
    qword_AF128 = v3;
    EFI_CREATE_EVENT CreateEvent = mBootServices->CreateEvent;
    EFI_SET_TIMER SetTimer = mBootServices->SetTimer;
    EFI_CLOSE_EVENT CloseEvent = mBootServices->CloseEvent;
    result = sub_6B00((char*)v2, (UINT64)&qword_AF160, (UINT64)&qword_AF118, (UINT64)&qword_AF120);
    if ( result >= 0 )
    {
        sub_16D7E(qword_AF160, qword_AF118 * qword_AF120);
        qword_AF120 /= 0x12uLL;
        qword_AF168 = 0LL;
        sub_15CE7(
                  ((UINT64)(unsigned int)dword_AF110 - qword_AF118) >> 1,
                  qword_AF128 + (((UINT64)(unsigned int)dword_AF114 - qword_AF120) >> 1),
                  qword_AF118,
                  qword_AF120,
                  qword_AF160);
        result = CreateEvent(
                             2147484160LL,
                             16LL,
                             sub_1702B,
                             0LL,
                             &qword_B1F38);
        if ( result >= 0 )
        {
            result = SetTimer(qword_B1F38, 1LL, 1000000LL);
            if ( result < 0 )
            {
                result = CloseEvent(qword_B1F38);
                qword_B1F38 = 0LL;
            }
        }
    }
    return result;
}

UINT64 sub_468F6(UINT64 a1, char a2)
{
    return a1 << a2;
}

UINT64 sub_46904(UINT64 a1, UINT64 a2)
{
    return a2 * a1;
}

UINT64 sub_322F7(signed int a1, int a2)
{
    UINT64 v2; // rax
    int v3; // edi
    UINT64 v4; // rsi
    UINT64 v5; // rax
    UINT64 v6; // rax
    
    if ( !a2 )
        return 0xFFFFFFFFLL;
    v2 = (unsigned int)-a1;
    if ( -a1 < 1 )
        v2 = (unsigned int)a1;
    v3 = 1 - 2 * (a1 >= 0);
    v4 = (unsigned int)-a2;
    if ( -a2 < 1 )
        v4 = (unsigned int)a2;
    if ( a2 >= 0 )
        v3 = 2 * (a1 >= 0) - 1;
    v5 = sub_468F6(v2, 16LL);
    v6 = sub_4691F(v5, v4, 0LL);
    return sub_46904(v6, v3);
}

UINT64 sub_32363(int a1, int a2)
{
    return sub_322F7((unsigned int)(a1 << 16), (unsigned int)(a2 << 16));
}

UINT64 sub_32253(int a1)
{
    return (unsigned int)(a1 << 16);
}

__int64 sub_3228C(signed int a1, int a2);

__int64 sub_3227F(signed int a1, int a2)
{
    return sub_3228C(a1, a2 << 16);
}

__int64 sub_3228C(signed int a1, int a2)
{
    __int64 v2; // r8
    int v3; // esi
    __int64 v4; // rax
    __int64 v5; // rax
    __int64 v6; // rax
    
    v2 = (unsigned int)-a1;
    if ( -a1 < 1 )
        v2 = (unsigned int)a1;
    v3 = 1 - 2 * (a1 >= 0);
    v4 = (unsigned int)-a2;
    if ( -a2 < 1 )
        v4 = (unsigned int)a2;
    if ( a2 >= 0 )
        v3 = 2 * (a1 >= 0) - 1;
    v5 = sub_46904(v2, v4);
    v6 = sub_46911(v5, 16LL);
    return sub_46904(v6, v3);
}

__int64 sub_322EA(signed int a1, int a2)
{
    return sub_322F7(a1, a2 << 16);
}

__int64 sub_3225E(int a1)
{
    if ( a1 < 0 )
        return (unsigned int)-sub_3225E((unsigned int)-a1);
    else
        return HIWORD(a1);
}

__int64 sub_152A7(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
{
    __int64 result; // rax
    
    result = qword_B1F20;
    if ( qword_B1F20 )
        return (*(__int64 (**)(__int64, int *, _QWORD, _QWORD, _QWORD, __int64, __int64, __int64, __int64, _QWORD))(result + 16))(
                                                                                                                                  result,
                                                                                                                                  &dword_AD818,
                                                                                                                                  0LL,
                                                                                                                                  0LL,
                                                                                                                                  0LL,
                                                                                                                                  a1,
                                                                                                                                  a2,
                                                                                                                                  a3,
                                                                                                                                  a4,
                                                                                                                                  0LL);
    result = qword_B1F18;
    if ( qword_B1F18 )
        return (*(__int64 (**)(__int64, int *, _QWORD, _QWORD, _QWORD, __int64, __int64, __int64, __int64, _QWORD))(result + 16))(
                                                                                                                                  result,
                                                                                                                                  &dword_AD818,
                                                                                                                                  0LL,
                                                                                                                                  0LL,
                                                                                                                                  0LL,
                                                                                                                                  a1,
                                                                                                                                  a2,
                                                                                                                                  a3,
                                                                                                                                  a4,
                                                                                                                                  0LL);
    return result;
}

__int64 sub_389A1(int a1)
{
    int v1; // esi
    int v2; // eax
    
    v1 = a1 >> 31;
    v2 = sub_3225E(a1);
    return sub_32253(v2 + v1);
}

__int64 sub_44B8E(int *a1)
{
    __int64 v2; // rcx
    void (*v3)(__int64); // rdx
    __int64 v4; // rdi
    __int64 v5; // rbx
    __int64 result; // rax
    
    if ( a1 )
    {
        v2 = *((_QWORD *)a1 + 1);
        if ( v2 )
        {
            v3 = *(void (**)(__int64))(qword_B2098 + 72);
            if ( *a1 > 0 )
            {
                v4 = 8LL;
                v5 = 0LL;
                do
                {
                    v3(*(_QWORD *)(v2 + v4));
                    ++v5;
                    v3 = *(void (**)(__int64))(qword_B2098 + 72);
                    v2 = *((_QWORD *)a1 + 1);
                    v4 += 16LL;
                }
                while ( v5 < *a1 );
            }
            v3(v2);
        }
        return (*(__int64 (**)(int *))(qword_B2098 + 72))(a1);
    }
    return result;
}

unsigned __int64 sub_44C06(int a1, int a2, __int64 a3, int **a4)
{
    int v6; // r12d
    unsigned __int64 v7; // r15
    int v8; // r14d
    int *v9; // rax
    int *v10; // rbx
    __int64 v11; // rax
    __int64 v12; // rcx
    __int64 v13; // rbx
    int v14; // eax
    unsigned int v15; // eax
    __int64 v16; // rdi
    int v17; // eax
    int v18; // ecx
    int v19; // eax
    __int64 v20; // rdx
    unsigned int v21; // r14d
    int v22; // eax
    int v23; // r13d
    __int64 v24; // rdx
    int v25; // eax
    int v26; // esi
    int v27; // eax
    __int64 v28; // rdi
    __int64 v29; // rax
    bool v30; // zf
    unsigned __int64 v31; // rax
    int *v32; // rbx
    int v33; // r12d
    int v34; // r15d
    int *v35; // r14
    int v36; // edi
    int v37; // eax
    int v38; // ecx
    int v39; // esi
    int v40; // eax
    __int64 (*v41)(_QWORD); // rbx
    unsigned int v42; // eax
    signed int v43; // eax
    int v44; // eax
    __int64 v45; // rcx
    __int64 v46; // rdx
    __int64 v47; // rcx
    __int64 v48; // rsi
    int v49; // eax
    int v51; // [rsp+28h] [rbp-98h]
    int **v52; // [rsp+38h] [rbp-88h]
    __int64 v53; // [rsp+40h] [rbp-80h]
    __int64 v55; // [rsp+58h] [rbp-68h]
    __int64 v56; // [rsp+60h] [rbp-60h]
    int v57; // [rsp+68h] [rbp-58h]
    int v58; // [rsp+6Ch] [rbp-54h]
    int v59; // [rsp+70h] [rbp-50h]
    int v60; // [rsp+74h] [rbp-4Ch]
    int *v61; // [rsp+78h] [rbp-48h]
    int v62; // [rsp+84h] [rbp-3Ch]
    
    v6 = a1;
    v7 = 0x8000000000000009uLL;
    v8 = sub_32363(a2, a1);
    v9 = (int *)sub_28E94(16LL);
    if ( v9 )
    {
        v10 = v9;
        *v9 = a2;
        v11 = sub_28E94(16LL * a2);
        *((_QWORD *)v10 + 1) = v11;
        if ( v11 )
        {
            v52 = a4;
            if ( a2 <= 0 )
            {
            LABEL_33:
                *v52 = v10;
                return 0LL;
            }
            v61 = v10;
            v53 = (unsigned int)a2;
            v12 = 0LL;
            v13 = v11;
            v59 = v6;
            v60 = v8;
            while ( 2 )
            {
                v56 = v12;
                v14 = sub_32253(v12);
                v15 = sub_322F7(v14 + 0x8000, v8);
                v16 = v15;
                v17 = v15 - *(_DWORD *)a3;
                v18 = v17 - 8;
                if ( v17 < 8 )
                    v18 = 0;
                v62 = v18;
                v19 = sub_32253(v6);
                v20 = v16;
                v51 = v16;
                v21 = v16 + *(_DWORD *)a3 + 8;
                if ( v19 - 8 < (int)v21 )
                    v21 = sub_32253(v6) - 8;
                v22 = sub_389A1((unsigned int)v62);
                v23 = sub_3225E(v22);
                v25 = sub_389A1(v21);
                v26 = sub_3225E(v25);
                v27 = v26 - v23 + 1;
                if ( v26 - v23 == -1 )
                {
                    v31 = 0x8000000000000002uLL;
                }
                else
                {
                    v28 = 16 * v56;
                    *(_DWORD *)(v13 + 16 * v56 + 4) = v27;
                    v29 = sub_28E94(8LL * v27);
                    *(_QWORD *)(v13 + 16 * v56 + 8) = v29;
                    v30 = v29 == 0;
                    v31 = 0LL;
                    if ( v30 )
                        v31 = 0x8000000000000009uLL;
                    if ( !(_DWORD)v31 )
                    {
                        v31 = 0x8000000000000015uLL;
                        if ( v26 >= v23 )
                        {
                            v57 = v21;
                            v55 = v13 + v28 + 4;
                            v32 = (int *)(v28 + v13);
                            v33 = 0;
                            v58 = v26 + 1;
                            do
                            {
                                v34 = v62;
                                if ( v62 <= (int)sub_32253(v23) )
                                    v34 = sub_32253(v23);
                                v35 = v32;
                                v36 = v23 + 1;
                                v37 = sub_32253(v23 + 1);
                                v38 = v57;
                                if ( v57 >= v37 )
                                    v38 = sub_32253(v36);
                                v39 = v38 - v34;
                                v40 = sub_322EA(v34 + v38, 2);
                                v41 = *(__int64 (**)(_QWORD))(a3 + 8);
                                v42 = sub_3228C(*(_DWORD *)(a3 + 4), v40 - v51);
                                v43 = v41(v42);
                                v44 = sub_3228C(v43, v39);
                                v32 = v35;
                                if ( v44 )
                                {
                                    v45 = *v35;
                                    if ( (int)v45 >= *(_DWORD *)v55 )
                                    {
                                        v31 = 0x8000000000000009uLL;
                                        goto LABEL_32;
                                    }
                                    v46 = *(_QWORD *)(v55 + 4);
                                    *(_DWORD *)(v46 + 8 * v45) = v23;
                                    *(_DWORD *)(v46 + 8 * v45 + 4) = v44;
                                    *v35 = v45 + 1;
                                }
                                v33 += v44;
                                ++v23;
                            }
                            while ( v58 != v36 );
                            v31 = 0x8000000000000015uLL;
                            if ( !v33 )
                                break;
                            if ( v33 != 0x10000 && *v35 > 0 )
                            {
                                v47 = *(_QWORD *)(v55 + 4);
                                v48 = 0LL;
                                do
                                {
                                    v49 = sub_322F7(*(_DWORD *)(v47 + 8 * v48 + 4), v33);
                                    v47 = *(_QWORD *)(v55 + 4);
                                    *(_DWORD *)(v47 + 8 * v48++ + 4) = v49;
                                }
                                while ( v48 < *v35 );
                            }
                            v12 = v56 + 1;
                            v10 = v61;
                            v6 = v59;
                            v8 = v60;
                            if ( v56 + 1 != v53 )
                            {
                                v13 = *((_QWORD *)v61 + 1);
                                continue;
                            }
                            goto LABEL_33;
                        }
                    }
                }
                break;
            }
        }
        else
        {
            v61 = v10;
            v31 = 0x8000000000000009uLL;
        }
    LABEL_32:
        v7 = v31;
        sub_44B8E(v61);
    }
    return v7;
}

__int64 sub_44386(__int64 a1, __int64 a2, unsigned __int64 a3, __int64 a4, __int64 a5, signed int a6)
{
    unsigned int v7; // r13d
    unsigned __int64 v8; // rsi
    unsigned __int64 v9; // rbx
    int v10; // edi
    int v11; // edi
    __int64 v12; // rsi
    __int64 v13; // rax
    __int64 v14; // rsi
    __int64 v15; // r13
    __int64 v16; // rax
    __int64 v17; // r13
    __int64 v18; // rdi
    int v19; // eax
    __int64 v20; // rcx
    __int64 v21; // rbx
    __int64 v22; // rax
    __int64 v23; // r15
    __int64 v24; // rdi
    int v25; // ebx
    __int64 i; // r13
    int v27; // r14d
    signed int v28; // ecx
    signed int v29; // eax
    __int64 j; // rbx
    signed int v31; // edi
    int v32; // ecx
    int v33; // eax
    _DWORD v35[2]; // [rsp+28h] [rbp-F8h] BYREF
    __int64 v36; // [rsp+30h] [rbp-F0h]
    _DWORD v37[2]; // [rsp+38h] [rbp-E8h] BYREF
    __int64 v38; // [rsp+40h] [rbp-E0h]
    __int64 v39; // [rsp+48h] [rbp-D8h]
    int *v40; // [rsp+50h] [rbp-D0h]
    _QWORD *v41; // [rsp+58h] [rbp-C8h]
    __int64 v42; // [rsp+60h] [rbp-C0h]
    __int64 v43; // [rsp+68h] [rbp-B8h]
    __int64 *v44; // [rsp+70h] [rbp-B0h]
    unsigned __int64 v45; // [rsp+78h] [rbp-A8h]
    __int64 v46; // [rsp+80h] [rbp-A0h]
    __int64 v47; // [rsp+88h] [rbp-98h]
    __int64 v48; // [rsp+90h] [rbp-90h]
    _DWORD *v49; // [rsp+98h] [rbp-88h]
    __int64 v50; // [rsp+A0h] [rbp-80h]
    unsigned __int64 v51; // [rsp+A8h] [rbp-78h]
    int v52; // [rsp+B4h] [rbp-6Ch]
    __int64 v53; // [rsp+B8h] [rbp-68h] BYREF
    __int64 v54; // [rsp+C0h] [rbp-60h] BYREF
    __int64 v55; // [rsp+C8h] [rbp-58h]
    __int64 v56; // [rsp+D0h] [rbp-50h]
    __int64 v57; // [rsp+D8h] [rbp-48h]
    unsigned __int64 v58; // [rsp+E0h] [rbp-40h]
    
    v7 = a3;
    v57 = a2;
    v50 = a1;
    v53 = 0LL;
    v54 = 0LL;
    v8 = HIDWORD(a3);
    v9 = HIDWORD(a4);
    v10 = sub_32363(SHIDWORD(a4), SHIDWORD(a3));
    if ( v10 > 0x10000 )
        v10 = 0x10000;
    v35[0] = sub_322F7(a6, v10);
    v35[1] = v10;
    v36 = a5;
    v11 = sub_32363(a4, v7);
    if ( v11 > 0x10000 )
        v11 = 0x10000;
    v37[0] = sub_322F7(a6, v11);
    v37[1] = v11;
    v38 = a5;
    v45 = v8;
    v51 = v9;
    v12 = sub_44C06((unsigned int)v8, (unsigned int)v9, v35, &v53);
    if ( !(_DWORD)v12 )
        v12 = sub_44C06(v7, (unsigned int)a4, v37, &v54);
    if ( !(_DWORD)v12 )
    {
        v58 = v12;
        v13 = sub_28E94(16LL);
        if ( v13 )
        {
            v14 = v13;
            if ( (int)a4 > 0 )
            {
                v48 = (unsigned int)a4;
                v47 = 4 * (a4 >> 32);
                v58 = 0LL;
                v56 = 0LL;
                v15 = v57;
                while ( 1 )
                {
                    v16 = *(_QWORD *)(v54 + 8);
                    if ( !v16 )
                        break;
                    if ( !v50 )
                        break;
                    if ( !v15 )
                        break;
                    v46 = v53;
                    if ( !v53 )
                        break;
                    v57 = v15;
                    if ( (int)v51 > 0 )
                    {
                        v49 = (_DWORD *)(v16 + 16 * v56);
                        v44 = (__int64 *)(v49 + 2);
                        v17 = v57;
                        v55 = 0LL;
                        do
                        {
                            v18 = *(_QWORD *)(v46 + 8);
                            (*(void (**)(__int64, __int64, _QWORD))(qword_B2098 + 360))(v14, 16LL, 0LL);
                            v19 = *v49;
                            if ( (int)*v49 > 0 )
                            {
                                v40 = (int *)(v18 + 16 * v55);
                                v41 = v40 + 2;
                                LODWORD(v20) = *v40;
                                v21 = 0LL;
                                v43 = v17;
                                do
                                {
                                    if ( (int)v20 > 0 )
                                    {
                                        v22 = *v44;
                                        v52 = *(_DWORD *)(*v44 + 8 * v21 + 4);
                                        v42 = v21;
                                        v39 = v50 + 4 * *(int *)(v22 + 8 * v21) * (__int64)(int)v45;
                                        v23 = 0LL;
                                        do
                                        {
                                            v24 = v39 + 4LL * *(int *)(*v41 + 8 * v23);
                                            v25 = sub_3228C(*(_DWORD *)(*v41 + 8 * v23 + 4), v52);
                                            for ( i = 0LL; i != 4; ++i )
                                            {
                                                v27 = sub_32363(*(unsigned __int8 *)(v24 + i), 255);
                                                v28 = 0x10000;
                                                if ( i != 3 )
                                                    v28 = sub_32363(*(unsigned __int8 *)(v24 + 3) ^ 0xFFu, 255);
                                                v29 = sub_3228C(v28, v27);
                                                *(_DWORD *)(v14 + 4 * i) += sub_3228C(v29, v25);
                                            }
                                            ++v23;
                                            v20 = *v40;
                                        }
                                        while ( v23 < v20 );
                                        v19 = *v49;
                                        v17 = v43;
                                        v21 = v42;
                                    }
                                    ++v21;
                                }
                                while ( v21 < v19 );
                            }
                            for ( j = 0LL; j != 4; ++j )
                            {
                                v31 = *(_DWORD *)(v14 + 4 * j);
                                if ( (int)sub_3227F(v31, 255) < 0 )
                                    v32 = 0;
                                else
                                    v32 = sub_3227F(v31, 255);
                                v33 = sub_3225E(v32 + 0x8000);
                                if ( (unsigned __int64)v33 >= 0xFF )
                                    LOBYTE(v33) = -1;
                                *(_BYTE *)(v17 + j) = v33;
                            }
                            v17 += 4LL;
                            ++v55;
                        }
                        while ( v55 != v51 );
                    }
                    v15 = v47 + v57;
                    if ( ++v56 == v48 )
                        goto LABEL_40;
                }
                v58 = 0x8000000000000002uLL;
            }
        LABEL_40:
            (*(void (**)(__int64))(qword_B2098 + 72))(v14);
            v12 = v58;
        }
        else
        {
            v12 = 0x8000000000000009uLL;
        }
    }
    if ( v53 )
        sub_44B8E(a1);
    if ( v54 )
        sub_44B8E(a1);
    return v12;
}

__int64 sub_4479F(int a1)
{
    int v1; // edx
    __int64 result; // rax
    
    v1 = -a1;
    if ( -a1 < 1 )
        v1 = a1;
    result = 0LL;
    if ( v1 <= 0x8000 )
        return 0x10000LL;
    return result;
}

__int64 sub_4424F(int a1, int a2, int a3, int a4)
{
    return sub_44386(a2, a1, a3, a4, (__int64)sub_4479F, 0x8000);
}

__int64 sub_441BD(__int64 a1, __int64 a2, __int64 a3, __int64 a4)
{
    __int64 (*v4)(void); // rax
    
    if ( (unsigned int)dword_B1D7C < 8 )
    {
        if ( dword_B1D7C )
        {
            v4 = funcs_4421A[dword_B1D7C - 1];
            return v4();
        }
    }
    else
    {
        dword_B1D7C = 0;
    }
    v4 = sub_4424F;
    if ( SHIDWORD(a4) > SHIDWORD(a3) )
        v4 = sub_4421D;
    if ( (int)a4 > (int)a3 )
        v4 = sub_4421D;
    return v4();
}

void sub_15784(UINT64 a1, UINT64 *a2, _QWORD *a3, _QWORD *a4)
{
    const char *v8; // rdx
    UINT64 v9; // rcx
    bool v10; // cf
    const char *v11; // rax
    char *v12; // rcx
    _WORD *v13; // rax
    UINT64 v14; // rax
    UINT64 v15; // rdi
    UINT64 v16; // rdi
    UINT64 v17; // rax
    UINT64 v18; // rcx
    UINT64 v19; // rdx
    int v20; // esi
    UINT64 v21; // rdi
    UINT64 v22; // rdi
    UINT64 v23; // rdi
    UINT64 v24; // rcx
    UINT64 v25; // esi
    UINT64 v26; // r12d
    unsigned int v27; // edi
    UINT64 v28; // eax
    int v29; // esi
    unsigned int v30; // eax
    unsigned int v31; // esi
    unsigned int v32; // ebx
    unsigned int v33; // edi
    UINT64 v34; // rsi
    unsigned int v35; // eax
    UINT64 v36; // r12
    unsigned int v37; // edi
    UINT64 v38; // rax
    UINT64 v39; // rbx
    UINT64 v40; // rcx
    UINT64 v41; // [rsp+38h] [rbp-98h]
    UINT64 v42; // [rsp+40h] [rbp-90h] BYREF
    UINT64 v43; // [rsp+48h] [rbp-88h] BYREF
    UINT64 v44; // [rsp+50h] [rbp-80h] BYREF
    UINT64 v45; // [rsp+58h] [rbp-78h] BYREF
    UINT64 v46; // [rsp+60h] [rbp-70h] BYREF
    UINT64 v47; // [rsp+68h] [rbp-68h] BYREF
    UINT64 v48; // [rsp+70h] [rbp-60h] BYREF
    UINT64 v49; // [rsp+78h] [rbp-58h] BYREF
    UINT64 v50; // [rsp+80h] [rbp-50h] BYREF
    UINT64 v51; // [rsp+88h] [rbp-48h] BYREF
    unsigned int v52; // [rsp+90h] [rbp-40h] BYREF
    _DWORD v53[15]; // [rsp+94h] [rbp-3Ch] BYREF
    
    v47 = 0LL;
    v51 = 0LL;
    v50 = 0LL;
    v48 = 0LL;
    v49 = 0LL;
    v45 = 0LL;
    v52 = 0;
    v53[0] = 0;
    v46 = 0LL;
    v42 = 0LL;
    v43 = 0LL;
    if ( a1 == 1 )
    {
        v8 = "Boot Logo";
    }
    else
    {
        if ( a1 != 2 )
            goto LABEL_26;
        v8 = "Boot Fail Logo";
    }
    if ( sub_1D76C(qword_B1DE0, (UINT64)v8, &v42, &v43) )
        goto LABEL_6;
    if ( !v43 )
    {
    LABEL_26:
        *a2 = 0LL;
        *a3 = 0LL;
        *a4 = 0LL;
        return;
    }
    v13 = sub_B57C(v42, v43);
    v14 = sub_19F56(qword_B1E20, qword_B1E28, (UINT64)v13, 0LL, &v48, &v50);
    v9 = v50;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer = mBootServices->LocateHandleBuffer;
    EFI_HANDLE_PROTOCOL HandleProtocol = mBootServices->HandleProtocol;
    if ( v14 < 0 || !v50 )
        goto LABEL_7;
    if ( LocateHandleBuffer(
                            2LL,
                            &gAppleImageConversionProtocolGuid,
                            0LL,
                            &v46,
                            &v49) >= 0 )
    {
        sub_1D2F0(v49);
        if ( v46 )
        {
            v15 = 0LL;
            while ( 1 )
            {
                if ( HandleProtocol(
                                    *(_QWORD *)(v49 + 8 * v15),
                                    &gAppleImageConversionProtocolGuid,
                                    &v47) < 0 )
                    goto LABEL_6;
                if ( (*(UINT64 (**)(UINT64, UINT64))(v47 + 16))(v50, v48) >= 0 )
                    break;
                if ( ++v15 >= v46 )
                    goto LABEL_6;
            }
            sub_1D327(v49);
            v49 = 0LL;
            if ( (*(UINT64 (**)(UINT64, UINT64, unsigned int *, _DWORD *))(v47 + 24))(v50, v48, &v52, v53) < 0 )
                goto LABEL_6;
            v16 = (*(UINT64 (**)(UINT64, UINT64, UINT64 *, UINT64 *))(v47 + 32))(
                                                                                 v50,
                                                                                 v48,
                                                                                 &v51,
                                                                                 &v45);
            sub_1D327(v50);
            v50 = 0LL;
            if ( v16 < 0 )
            {
                v9 = 0LL;
                goto LABEL_7;
            }
            if ( v51 )
            {
                sub_1D2F0(v51);
                v17 = v51;
                if ( v51 )
                {
                    v18 = v45 >> 2;
                    if ( v45 >> 2 )
                    {
                        v19 = 0LL;
                        do
                        {
                            if ( *(_BYTE *)(v17 + 4 * v19 + 3) )
                            {
                                v20 = *(UINT8 *)(v17 + 4 * v19 + 3) ^ 0xFF;
                                v21 = ((32897 * v20 * (unsigned int)*(UINT8 *)(v17 + 4 * v19 + 2)) >> 23)
                                + (UINT64)BYTE2(dword_AD818);
                                if ( v21 >= 0xFF )
                                    LOBYTE(v21) = -1;
                                *(_BYTE *)(v17 + 4 * v19 + 2) = v21;
                                v22 = ((32897 * v20 * (unsigned int)*(UINT8 *)(v17 + 4 * v19 + 1)) >> 23)
                                + (UINT64)BYTE1(dword_AD818);
                                if ( v22 >= 0xFF )
                                    LOBYTE(v22) = -1;
                                *(_BYTE *)(v17 + 4 * v19 + 1) = v22;
                                v23 = ((32897 * v20 * (unsigned int)*(UINT8 *)(v17 + 4 * v19)) >> 23)
                                + (UINT64)(UINT8)dword_AD818;
                                if ( v23 >= 0xFF )
                                    LOBYTE(v23) = -1;
                                *(_BYTE *)(v17 + 4 * v19) = v23;
                            }
                            ++v19;
                        }
                        while ( v18 != v19 );
                    }
                }
            }
            v44 = 0LL;
            if ( sub_1D7D7(qword_B1DE0, (UINT64)"Boot Logo Scale", &v44) )
            {
                v44 = 125LL;
                v24 = 125LL;
            }
            else
            {
                v24 = v44;
            }
            if ( (unsigned int)dword_AF114 <= 0x437 && v24 )
            {
                v25 = sub_32363((int)v24, 1000LL);
                v26 = sub_32253(1LL);
                sub_32253(1LL);
                v27 = 16 * dword_AF114 / 9u;
                if ( v27 != dword_AF110 )
                    v26 = dword_AF110 * v26 / v27;
                v28 = sub_32253((int)dword_AF110);
                v29 = sub_3228C(v28, v25);
                if ( v29 > (int)sub_32253(v52) )
                {
                    *a3 = v52;
                LABEL_65:
                    *a4 = v53[0];
                    *a2 = v51;
                    return;
                }
                v30 = sub_3227F(v26, v52);
                v31 = sub_322EA((unsigned int)v29, v30);
                v32 = sub_3228C(v31, v26);
                v33 = sub_3227F(v31, v53[0]);
                v34 = v52;
                v41 = v53[0];
                v35 = sub_3227F(v32, v52);
                v36 = (unsigned int)sub_3225E(v35);
                v37 = sub_3225E(v33);
                v38 = sub_1D2B1(v45);
                if ( v38 )
                {
                    v39 = v38;
                    if ( sub_441BD(v38, v51, v41 + (v34 << 32), (v36 << 32) | v37) )
                    {
                        *a3 = v52;
                        *a4 = v53[0];
                        *a2 = v51;
                        v40 = v39;
                    }
                    else
                    {
                        *a3 = (int)v36;
                        *a4 = (int)v37;
                        *a2 = v39;
                        v40 = v51;
                    }
                    sub_1D327(v40);
                    return;
                }
            }
            *a3 = v52;
            goto LABEL_65;
        }
    }
LABEL_6:
    v9 = v50;
LABEL_7:
    if ( v9 )
        sub_1D327(v9);
    if ( v49 )
        sub_1D327(v49);
    if ( v51 )
        sub_1D327(v51);
    if ( a1 == 2 )
    {
        v10 = (qword_B1DE8 & 0x40000) != 0;
        if ( byte_B1F30 )
        {
            v11 = "CircleSlash2X";
            v12 = "CircleSlashBlack2X";
        }
        else
        {
            v11 = "CircleSlash";
            v12 = "CircleSlashBlack";
        }
    }
    else
    {
        v10 = (qword_B1DE8 & 0x40000) != 0;
        if ( byte_B1F30 )
        {
            v11 = "AppleLogo2X";
            v12 = "AppleLogoBlack2X";
        }
        else
        {
            v11 = "AppleLogo";
            v12 = "AppleLogoBlack";
        }
    }
    if ( !v10 )
        v12 = (char *)v11;
    if ( sub_6B00(v12, (UINT64)a2, (UINT64)a3, (UINT64)a4) >= 0 )
        sub_16D7E(*a2, *a3 * *a4);
}



UINT64 sub_15583(int a1)
{
    UINT64 result; // rax
    UINT64 v2; // rsi
    UINT64 v3; // rdi
    UINT64 v4; // [rsp+30h] [rbp-50h] BYREF
    UINT64 v5; // [rsp+38h] [rbp-48h] BYREF
    UINT64 v6; // [rsp+40h] [rbp-40h] BYREF
    UINT64 v7; // [rsp+48h] [rbp-38h] BYREF
    UINT64 v8; // [rsp+50h] [rbp-30h] BYREF
    UINT64 v9; // [rsp+58h] [rbp-28h] BYREF
    UINT64 v10; // [rsp+60h] [rbp-20h] BYREF
    _BYTE v11[17]; // [rsp+6Fh] [rbp-11h] BYREF
    
    result = 0xAAAAAAAAAAAAAAAAuLL;
    v5 = 0xAAAAAAAAAAAAAAAAuLL;
    v6 = 0xAAAAAAAAAAAAAAAAuLL;
    v9 = 0LL;
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    if ( dword_AF158 == 1 && qword_AF138 )
    {
        if ( a1 == 1 )
        {
            v10 = 0LL;
            v7 = 0xAAAAAAAAAAAAAAAAuLL;
            v8 = 0xAAAAAAAAAAAAAAAAuLL;
            v11[0] = 0;
            v4 = 1LL;
            if ( !GetVariable(
                              L"S",
                              &gAppleVendorVariableGuid,
                              0LL,
                              (UINTN*)&v4,
                              v11)
                && v11[0] )
            {
                UINT64 loc_10004 = 0x10004;
                result = qword_B1DE8 & (UINT64)&loc_10004;
                if ( (_DWORD)result != 4 )
                    return result;
                return sub_16EC5();
            }
            sub_15784(1LL, &v10, &v7, &v8);
            sub_12453("Start DrawColorRectangle");
            sub_152A7(0LL, 0LL, (unsigned int)dword_AF110, (unsigned int)dword_AF114);
            result = sub_12453("End DrawColorRectangle");
            if ( (qword_B1DE8 & 0x10000) == 0 )
            {
                if ( v10 )
                {
                    v2 = ((unsigned int)dword_AF110 - v7) >> 1;
                    v3 = ((unsigned int)dword_AF114 - v8) >> 1;
                    sub_12453("Start DrawDataRectangle");
                    sub_15CE7(v2, v3, v7, v8, v10);
                    sub_12453("End DrawDataRectangle");
                    result = sub_1D327(v10);
                    if ( (qword_B1DE8 & 4) != 0 )
                        return sub_16EC5();
                }
            }
        }
        else
        {
            sub_15784(a1, &v9, &v5, &v6);
            if ( v9 )
            {
                sub_152A7(0LL, 0LL, (unsigned int)dword_AF110, (unsigned int)dword_AF114);
                sub_15CE7(((unsigned int)dword_AF110 - v5) >> 1, ((unsigned int)dword_AF114 - v6) >> 1, v5, v6, v9);
                return sub_1D327(v9);
            }
        }
    }
    return result;
}

__int64 sub_20A3F(unsigned __int16 *a1)
{
    unsigned int v1; // r8d
    unsigned __int16 v2; // r11
    __int16 v3; // r9
    unsigned __int16 v4; // cx
    __int64 v5; // rdx
    bool v6; // r10
    __int64 v7; // rdx
    __int64 v8; // rcx
    __int64 result; // rax
    
    v1 = *((unsigned __int8 *)a1 + 2);
    if ( *((_BYTE *)a1 + 2) )
    {
        v2 = *a1;
        v3 = *a1 % 0x64u;
        v4 = __ROR2__(23593 * *a1, 4);
        v5 = 1LL;
        if ( v1 > 1 )
            v5 = v1;
        v6 = v4 < 0xA4u && (_BYTE)v1 == 2;
        v7 = 4 * v5;
        v8 = 0LL;
        LODWORD(result) = 0;
        while ( 1 )
        {
            if ( (v2 & 3) == 0 )
            {
                if ( v3 )
                {
                    if ( (_BYTE)v1 == 2 )
                        goto LABEL_13;
                }
                else if ( v6 )
                {
                LABEL_13:
                    result = (unsigned int)(result + 29);
                    goto LABEL_7;
                }
            }
            result = (unsigned int)(*(_DWORD *)((char *)qword_AAA60 + v8) + result);
        LABEL_7:
            v8 += 4LL;
            if ( v7 == v8 )
                return result;
        }
    }
    return 0LL;
}

void __fastcall sub_207F6(int a1, int a2)
{
    __int64 v4; // rax
    __int64 v5; // rax
    __int64 v6; // rax
    __int64 (__fastcall *v7)(const __int16 *, void *, __int64, __int64, char *); // rax
    __int64 v8; // rax
    __int64 v9; // [rsp+30h] [rbp-40h] BYREF
    char v10; // [rsp+3Eh] [rbp-32h] BYREF
    char v11[49]; // [rsp+3Fh] [rbp-31h] BYREF
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    EFI_GET_TIME GetTime = mRuntimeServices->GetTime;
    
    v11[0] = -86;
    v9 = 1LL;
    v4 = GetVariable(
                     L"panicmedic",
                     &gAppleBootVariableGuid,
                     0LL,
                     &v9,
                     v11);
    if ( v4 < 0 )
    {
        DEBUG ((DEBUG_INFO, "#[EB.P.TPM|!] %r <- RT.GV %S %g\n", v4, L"panicmedic", &gAppleBootVariableGuid));
        v11[0] = 0;
    }
    else if ( v11[0] )
    {
        DEBUG ((DEBUG_INFO, "#[EB|P:PM] Shutdown\n"));
        v5 = SetVariable(
                         L"AAPL,PanicInfoLog",
                         &gAppleBootVariableGuid,
                         3LL,
                         0LL,
                         0LL);
        DEBUG ((DEBUG_INFO, "#[EB.P.TPM|!] %r <- RT.SV- %S %g\n", v5, L"AAPL,PanicInfoLog", &gAppleBootVariableGuid));
        v6 = SetVariable(
                         L"panicmedic",
                         &gAppleBootVariableGuid,
                         7LL,
                         0LL,
                         0LL);
        DEBUG ((DEBUG_INFO, "#[EB.P.TPM|!] %r <- RT.SV- %S %g\n", v6, L"AAPL,PanicInfoLog", &gAppleBootVariableGuid));
        sub_15D44(0, 0);
        if ( (qword_B1DE8 & 2) != 0 )
        {
            sub_15501(2LL);
        }
        else if ( sub_153D5() >= 0 )
        {
            sub_16556(0LL, 0LL);
            sub_15583(2);
        }
        DEBUG ((DEBUG_INFO, "***************************************************************\n"));
        DEBUG ((DEBUG_INFO, "Panic loop detected (%d in %3d seconds).  System will shut down.\n", a1, a2));
        DEBUG ((DEBUG_INFO, "Try booting into the Recovery HD volume by holding Command + R.\n"));
        DEBUG ((DEBUG_INFO, "***************************************************************\n"));
        EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount = mBootServices->GetNextMonotonicCount;
        GetNextMonotonicCount(30000000LL);
        sub_97BF(2LL, 4u);
        return;
    }
    DEBUG ((DEBUG_INFO, "#[EB|P:PM] Enable\n"));
    v10 = 1;
    v8 = SetVariable(L"panicmedic", &gAppleBootVariableGuid, 7LL, 1LL, &v10);
    if ( v8 < 0 )
        DEBUG ((DEBUG_INFO, "#[EB.P.TPM|!] %r <- RT.SV %S %g\n", v8, L"panicmedic", &gAppleBootVariableGuid));
    byte_B1E10 = 1;
}

__int64 sub_16CB0(void)
{
    unsigned __int8 v0; // cf
    const char *v1; // rax
    char *v2; // rcx
    __int64 result; // rax
    __int64 v4[3]; // [rsp+28h] [rbp-18h] BYREF
    
    memset(v4, 170, sizeof(v4));
    v0 = _bittest64((const signed __int32 *)&qword_B1DE8, 0x12u);
    v1 = "PanicDialogBlack2X";
    if ( !v0 )
        v1 = "PanicDialog2X";
    v2 = "PanicDialogBlack";
    if ( !v0 )
        v2 = "PanicDialog";
    if ( byte_B1F30 )
        v2 = (char *)v1;
    result = sub_6B00(v2, (__int64)&v4[2], (__int64)v4, (__int64)&v4[1]);
    if ( result >= 0 )
    {
        sub_16D7E(v4[2], v4[0] * v4[1]);
        sub_152A7(0LL, 0LL, (unsigned int)dword_AF110, (unsigned int)dword_AF114);
        sub_15CE7(
                  ((unsigned __int64)(unsigned int)dword_AF110 - v4[0]) >> 1,
                  ((unsigned __int64)(unsigned int)dword_AF114 - v4[1]) >> 1,
                  v4[0],
                  v4[1],
                  v4[2]);
        return sub_1D327(v4[2]);
    }
    return result;
}

__int64 sub_1528B()
{
    return sub_152A7(0LL, 0LL, (unsigned int)dword_AF110, (unsigned int)dword_AF114);
}

void sub_1FBBA(void)
{
    int v0; // r13d
    __int64 v1; // rax
    __int64 v2; // rax
    __int64 v3; // rdi
    __int64 *v4; // rdi
    __int64 i; // rcx
    __int64 v6; // rsi
    unsigned __int64 v7; // rax
    unsigned __int64 v8; // r15
    bool v9; // cc
    __int16 v10; // ax
    __int64 v11; // rbx
    __int64 v12; // rax
    unsigned __int64 v13; // rsi
    __int64 v14; // r12
    __int64 v15; // r13
    __int16 v16; // ax
    __int64 v17; // rdi
    int v18; // esi
    char v19; // bl
    __int64 v20; // rax
    __int64 v21; // rax
    __int64 v22; // rdi
    __int64 v23; // rax
    __int64 v24; // rax
    __int64 v25; // rcx
    _BYTE *v26; // rsi
    __int64 v27; // rax
    __int16 v28; // r12
    unsigned int v29; // r13d
    __int64 v30; // r15
    unsigned int v31; // esi
    unsigned int v32; // eax
    unsigned int v33; // edx
    unsigned int v34; // eax
    int v35; // r8d
    int v36; // r8d
    int v37; // esi
    int v38; // edi
    __int64 v39; // rdi
    __int64 v40; // rax
    __int64 v41; // rax
    int v42; // edi
    __int64 v43; // rcx
    int v44; // ebx
    __int64 v45; // rax
    __int64 v46; // [rsp+20h] [rbp-130h]
    __int64 v47; // [rsp+20h] [rbp-130h]
    __int64 v48; // [rsp+20h] [rbp-130h]
    __int64 v49; // [rsp+28h] [rbp-128h] BYREF
    __int64 v50; // [rsp+30h] [rbp-120h] BYREF
    __int64 v51; // [rsp+38h] [rbp-118h] BYREF
    _QWORD v52[10]; // [rsp+40h] [rbp-110h] BYREF
    int v53; // [rsp+94h] [rbp-BCh]
    unsigned int v54; // [rsp+98h] [rbp-B8h]
    int v55; // [rsp+9Ch] [rbp-B4h]
    __int64 v56; // [rsp+A0h] [rbp-B0h] BYREF
    unsigned __int64 v57; // [rsp+A8h] [rbp-A8h] BYREF
    unsigned __int64 v58; // [rsp+B0h] [rbp-A0h] BYREF
    unsigned __int64 v59; // [rsp+B8h] [rbp-98h]
    __int64 v60; // [rsp+C0h] [rbp-90h]
    __int64 v61; // [rsp+C8h] [rbp-88h]
    int v62; // [rsp+D0h] [rbp-80h]
    __int16 v63; // [rsp+D4h] [rbp-7Ch]
    __int16 v64; // [rsp+D6h] [rbp-7Ah]
    unsigned __int64 v65; // [rsp+E0h] [rbp-70h] BYREF
    __int64 v66; // [rsp+E8h] [rbp-68h] BYREF
    _QWORD v67[2]; // [rsp+F0h] [rbp-60h] BYREF
    __int64 v68; // [rsp+100h] [rbp-50h]
    __int64 v69; // [rsp+108h] [rbp-48h]
    int v70; // [rsp+114h] [rbp-3Ch]
    
    v57 = 0xAAAAAAAAAAAAAAAAuLL;
    v65 = 0xAAAAAAAAAAAAAAAAuLL;
    v49 = 4LL;
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    EFI_GET_TIME GetTime = mRuntimeServices->GetTime;
    v1 = GetVariable(
                     L"p",
                     &gAppleBootVariableGuid,
                     0LL,
                     &v49,
                     &dword_AD958);
    if ( v1 < 0 )
    {
        if ( v1 != 0x800000000000000EuLL )
            DEBUG ((DEBUG_INFO, "#[EB.P.DC|!] %r <- RT.GV %S %g\n", v1, "p", &gAppleBootVariableGuid));
    }
    else
    {
        if ( v49 == 4 )
        {
            DEBUG ((DEBUG_INFO, "#[EB.P.DC|PMT] %d\n", dword_AD958));
            goto LABEL_8;
        }
        DEBUG ((DEBUG_INFO, "#[EB.P.DC|SZ!] %qd %d\n", v49, 4LL, v46));
    }
    dword_AD958 = 240;
LABEL_8:
    LOBYTE(v58) = 0;
    LOBYTE(v67[0]) = 0;
    v50 = 1LL;
    v2 = GetVariable(
                     L"AAPL,CoProcessorReboot",
                     &gAppleBootVariableGuid,
                     0LL,
                     &v50,
                     &v58);
    if ( v2 < 0 )
    {
        v3 = v2;
        DEBUG ((DEBUG_INFO, "#[EB|P:CPR] N\n"));
        LOBYTE(v0) = 1;
        if ( v3 != 0x800000000000000EuLL )
            DEBUG ((DEBUG_INFO, "#[EB.P.DCCPR|!] %r <- RT.GV %S %g\n", v3, L"AAPL,CoProcessorReboot", &gAppleBootVariableGuid));
    }
    else
    {
        DEBUG ((DEBUG_INFO, "#[EB|P:CPR] %d\n", (unsigned __int8)v58));
        if ( (_BYTE)v58 )
        {
            SetVariable(
                        L"AAPL,CoProcessorReboot",
                        &gAppleBootVariableGuid,
                        7LL,
                        v50,
                        v67);
            v0 = 0;
        }
        else
        {
            LOBYTE(v0) = 1;
        }
    }
    v4 = &v50;
    for ( i = 23LL; i; --i )
    {
        *(_DWORD *)v4 = -1431655766;
        v4 = (__int64 *)((char *)v4 + 4);
    }
    v67[0] = 0xAAAAAAAAAAAAAAAAuLL;
    v67[1] = 0xAAAAAAAAAAAAAAAAuLL;
    v56 = 0LL;
    v6 = GetVariable(
                     L"A",
                     &gAppleBootVariableGuid,
                     0LL,
                     &v56,
                     0LL);
    if ( v6 != 0x8000000000000005uLL || !v56 )
    {
        DEBUG ((DEBUG_INFO, "#[EB|P:MPI] N\n"));
        v19 = 1;
        if ( v6 != 0x800000000000000EuLL || v56 )
            DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|!PIVAR] %r %qd\n", v6, v56, v47));
        goto LABEL_45;
    }
    v70 = v0;
    DEBUG ((DEBUG_INFO, "#[EB|P:MPI] Y\n"));
    GetTime(v67, 0LL);
    v64 = -21846;
    v66 = 0LL;
    v58 = 0x4C005000410041LL;
    v59 = 0x6E00610050002CLL;
    v60 = 0x6E004900630069LL;
    v61 = 0x300030006F0066LL;
    v62 = 3145776;
    v63 = 0;
    v69 = 0LL;
    v7 = 0LL;
    do
    {
        v8 = v7;
        v9 = v7 <= 9;
        v10 = 48;
        if ( !v9 )
            v10 = 55;
        HIWORD(v62) = v8 + v10;
        v66 = 0LL;
        v11 = GetVariable(
                          &v58,
                          &gAppleBootVariableGuid,
                          0LL,
                          &v66,
                          0LL);
        if ( v11 < 0 )
        {
            if ( v11 == 0x8000000000000005uLL )
            {
                v69 += v66;
            }
            else if ( v11 == 0x800000000000000EuLL )
            {
                break;
            }
        }
        v7 = v8 + 1;
    }
    while ( v8 != 15 );
    if ( !v69 )
    {
        v11 = 0x8000000000000004uLL;
        LOBYTE(v0) = v70;
    LABEL_43:
        DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|!] %r <- EB.P.PIC\n", v11));
        goto LABEL_44;
    }
    v12 = sub_1D2B1(v69);
    LOBYTE(v0) = v70;
    if ( !v12 )
    {
        v11 = 0x8000000000000009uLL;
        goto LABEL_43;
    }
    v68 = v12;
    if ( v8 )
    {
        v13 = 0LL;
        v14 = v69;
        v15 = v68;
        while ( 1 )
        {
            v16 = 48;
            if ( v13 > 9 )
                v16 = 55;
            HIWORD(v62) = v13 + v16;
            v66 = v14;
            v11 = GetVariable(
                              &v58,
                              &gAppleBootVariableGuid,
                              0LL,
                              &v66,
                              v15);
            if ( v11 < 0 )
                break;
            v14 -= v66;
            if ( v14 )
            {
                v15 += v66;
                if ( v8 != ++v13 )
                    continue;
            }
            goto LABEL_37;
        }
        v18 = -1431655766;
        v17 = v68;
    }
    else
    {
    LABEL_37:
        v17 = v68;
        v18 = sub_E4B2(0LL, v68, v69);
    }
    sub_1D327(v17);
    LOBYTE(v0) = v70;
    if ( v11 < 0 )
        goto LABEL_43;
    LODWORD(v68) = v18;
    v56 = 92LL;
    v27 = GetVariable(
                      L"AAPL,PanicInfoLog",
                      &gAppleBootVariableGuid,
                      0LL,
                      &v56,
                      &v50);
    if ( v27 < 0 )
    {
        v39 = v27;
        DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|!] %r <- RT.GV %S %g\n", v27, L"AAPL,PanicInfoLog", &gAppleBootVariableGuid));
        if ( v39 != 0x800000000000000EuLL )
        {
        LABEL_96:
            v19 = 0;
            v41 = SetVariable(
                              L"AAPL,PanicInfoLog",
                              &gAppleBootVariableGuid,
                              3LL,
                              0LL,
                              0LL);
            DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|!] %r <- RT.SV- %S %g\n", v41, L"AAPL,PanicInfoLog", &gAppleBootVariableGuid));
            goto LABEL_45;
        }
        sub_E5B0(&v50, 92LL);
        v50 = 4660LL;
        LODWORD(v51) = v68;
        sub_240D0((char *)v67, (char *)&v51 + 4, 0x10uLL);
        v40 = SetVariable(
                          L"AAPL,PanicInfoLog",
                          &gAppleBootVariableGuid,
                          3LL,
                          92LL,
                          &v50);
        if ( v40 < 0 )
            DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|SV1!] %r <- RT.SV %S %g\n", v40, L"AAPL,PanicInfoLog", &gAppleBootVariableGuid));
        else
            DEBUG ((DEBUG_INFO, "#[EB|P:LOG1]\n"));
    LABEL_44:
        v19 = 0;
        goto LABEL_45;
    }
    if ( (_DWORD)v50 != 4660 )
    {
        DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|PLV!] %ld %d\n", v50, 4660));
        goto LABEL_96;
    }
    v28 = v67[0];
    LOBYTE(v69) = BYTE2(v67[0]);
    v54 = 3600 * BYTE4(v67[0]) + 60 * BYTE5(v67[0]) + BYTE6(v67[0]);
    v53 = BYTE3(v67[0]);
    v55 = -BYTE3(v67[0]);
    v29 = 0;
    v30 = 0LL;
    while ( *((_WORD *)&v51 + 8 * v30 + 2) == v28 )
    {
        if ( *((_BYTE *)&v51 + 16 * v30 + 6) != (_BYTE)v69 )
            break;
        v31 = sub_20A3F((char *)&v51 + 16 * v30 + 4);
        v32 = sub_20A3F(v67);
        if ( v31 > v32 )
            break;
        if ( v32 == v31 )
        {
            v33 = 3600 * LOBYTE(v52[2 * v30]) + 60 * BYTE1(v52[2 * v30]) + BYTE2(v52[2 * v30]);
            v34 = v54 - v33;
            if ( v54 < v33 )
                v34 = 0;
        }
        else
        {
            v35 = *((unsigned __int8 *)&v51 + 16 * v30 + 7);
            if ( (unsigned __int8)v35 >= (unsigned __int8)v53 )
                break;
            v36 = v55 + v35;
            v37 = 0;
            v34 = 0;
            do
            {
                v38 = 86400;
                if ( !v37 )
                    v38 = -3600 * LOBYTE(v52[2 * v30]) - 60 * BYTE1(v52[2 * v30]) - BYTE2(v52[2 * v30]) + 86400;
                v34 += v38;
                --v37;
            }
            while ( v36 != v37 );
        }
        if ( !v34 )
            break;
        if ( v34 > v29 )
            v29 = v34;
        if ( ++v30 == 5 )
            goto LABEL_98;
    }
    v29 = 0;
LABEL_98:
    v42 = v51;
    v43 = 0LL;
    if ( (unsigned int)(HIDWORD(v50) + 1) <= 4 )
        v43 = (unsigned int)(HIDWORD(v50) + 1);
    v44 = v68;
    LODWORD(v51) = v68;
    HIDWORD(v50) = v43;
    sub_240D0((char *)v67, (char *)&v51 + 16 * v43 + 4, 0x10uLL);
    v45 = SetVariable(
                      L"AAPL,PanicInfoLog",
                      &gAppleBootVariableGuid,
                      3LL,
                      92LL,
                      &v50);
    if ( v45 < 0 )
    {
        DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSP|!] %r <- RT.SV %S %g\n", v45, L"AAPL,PanicInfoLog", &gAppleBootVariableGuid));
        v19 = 0;
    }
    else
    {
        DEBUG ((DEBUG_INFO, "#[EB|P:SLOT] %d\n", HIDWORD(v50)));
        if ( v42 == v44 )
        {
            DEBUG ((DEBUG_INFO, "#[EB|P:SKIP.3]\n"));
            v19 = 1;
        }
        else
        {
            if ( v29 && v29 < dword_AD958 )
            {
                DEBUG ((DEBUG_INFO, "#[EB|P:STAT] %d in %d s\n", 5, v29));
                sub_207F6(5LL, v29);
            }
            else
            {
                DEBUG ((DEBUG_INFO, "#[EB|P:SKIP.4] %d s\n", v29));
            }
            v19 = 0;
        }
    }
    LOBYTE(v0) = v70;
LABEL_45:
    LOBYTE(v58) = 0;
    v50 = 1LL;
    v20 = GetVariable(
                      L"p",
                      &gAppleBootVariableGuid,
                      0LL,
                      &v50,
                      &v58);
    if ( v20 < 0 )
    {
        v22 = v20;
        DEBUG ((DEBUG_INFO, "#[EB|P:BPI] N\n"));
        if ( v22 != 0x800000000000000EuLL )
            DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSPG|!] %r <- RT.GV %S %g\n", v22, "p", &gAppleBootVariableGuid));
    }
    else
    {
        if ( v50 == 1 )
        {
            DEBUG ((DEBUG_INFO, "#[EB|P:BPI] Y (%d)\n", (unsigned __int8)v58));
            v21 = SetVariable(
                              L"p",
                              &gAppleBootVariableGuid,
                              3LL,
                              0LL,
                              0LL);
            DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSPG|!] %r <- RT.SV- %S %g\n", v21, "p", &gAppleBootVariableGuid));
            if ( (unsigned __int8)v58 >= 5u )
                sub_207F6((unsigned __int8)v58, (unsigned int)dword_AD958);
            goto LABEL_53;
        }
        DEBUG ((DEBUG_INFO, "#[EB.P.DCMOSPG|SZ!] %qd %d\n", v50, 1LL, v48));
    }
    if ( ((unsigned __int8)v19 & (unsigned __int8)v0) != 0 )
        return;
LABEL_53:
    DEBUG ((DEBUG_INFO, "\n"));
    DEBUG ((DEBUG_INFO, "**************************************************\n"));
    DEBUG ((DEBUG_INFO, "This system was automatically rebooted after panic\n"));
    DEBUG ((DEBUG_INFO, "**************************************************\n"));
    if ( (qword_B1DE8 & 2) != 0 )
    {
        sub_15501(2LL);
        v24 = qword_B2098;
        v25 = 5000000LL;
    }
    else
    {
        sub_15D44(0, 0);
        if ( sub_153D5() < 0 )
            return;
        sub_16556(0LL, 0LL);
        sub_16CB0();
        if ( (*(__int64 (**)(__int64, _QWORD, _QWORD, _QWORD, unsigned __int64 *))(qword_B2098 + 80))(
                                                                                                      0x80000000LL,
                                                                                                      0LL,
                                                                                                      0LL,
                                                                                                      0LL,
                                                                                                      &v65) >= 0 )
        {
            v67[0] = 0xAAAAAAAAAAAAAAAAuLL;
            v59 = 0xAAAAAAAAAAAAAAAAuLL;
            v58 = 0xAAAAAAAAAAAAAAAAuLL;
            v52[0] = 0xAAAAAAAAAAAAAAAAuLL;
            (*(void (**)(unsigned __int64, __int64, __int64))(qword_B2098 + 88))(v65, 2LL, 100000000LL);
            v50 = v65;
            EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn = mSystemTable->ConIn;
            EFI_EVENT WaitForKey = ConIn->WaitForKey;
            EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
            //  v51 = *(_QWORD *)(*(_QWORD *)(qword_B2090 + 48) + 16LL);
            if ( LocateProtocol(&gAppleBootVariableGuid, 0LL, v67) < 0 )
            {
                (*(void (**)(__int64, __int64 *, unsigned __int64 *))(qword_B2098 + 96))(2LL, &v50, &v57);
            }
            else
            {
                v52[0] = *(_QWORD *)(v67[0] + 16LL);
                do
                {
                    if ( (*(__int64 (**)(__int64, __int64 *, unsigned __int64 *))(qword_B2098 + 96))(3LL, &v50, &v57) < 0 )
                        break;
                    if ( v57 != 2 )
                        break;
                    v23 = (*(__int64 (**)(_QWORD, unsigned __int64 *))(v67[0] + 8LL))(v67[0], &v58);
                    if ( WORD2(v59) )
                        break;
                }
                while ( v23 >= 0 );
            }
            (*(void (**)(unsigned __int64))(qword_B2098 + 112))(v65);
        }
        sub_1528B();
        v24 = qword_B2098;
        v25 = 1000000LL;
    }
    (*(void (**)(__int64))(v24 + 248))(v25);
    sub_1E045();
    if ( qword_B1DE0 )
    {
        v26 = sub_1D82E(qword_B1DE8, (unsigned __int8 *)qword_B1DE0, 0LL, 0LL);
        sub_1D327(qword_B1DE0);
        qword_B1DE0 = (__int64)v26;
    }
}

__int64 __fastcall sub_BF7A(__int64 *a1)
{
    __int64 v2; // rax
    __int64 v3; // rdi
    __int64 v4; // rax
    __int64 v5; // rbx
    __int64 v6; // rax
    __int64 v7; // rax
    __int64 v9; // [rsp+28h] [rbp-38h] BYREF
    __int64 v10; // [rsp+30h] [rbp-30h] BYREF
    __int64 v11; // [rsp+38h] [rbp-28h] BYREF
    unsigned int v12; // [rsp+40h] [rbp-20h] BYREF
    _DWORD v13[7]; // [rsp+44h] [rbp-1Ch] BYREF
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    v2 = qword_AEA20;
    if ( !qword_AEA20 )
    {
        v9 = 0LL;
        v10 = 0LL;
        v11 = 8LL;
        if ( GetVariable(
                         L"E",
                         &gAppleVendorVariableGuid,
                         0LL,
                         &v11,
                         &v9) >= 0 )
        {
            v11 = 8LL;
            v4 = GetVariable(
                             L"ExtendedFirmwareFeaturesMask",
                             &gAppleVendorVariableGuid,
                             0LL,
                             &v11,
                             &v10);
            if ( v4 >= 0 )
            {
                v3 = v4;
                v5 = v10;
                v6 = v9;
            LABEL_13:
                qword_AEA20 = v5 & v6;
                DEBUG ((DEBUG_INFO, "#[EB|FWFM] 0x%016qX\n", v5));
                DEBUG ((DEBUG_INFO, "#[EB|FWFT] 0x%016qX\n", v9));
                v2 = qword_AEA20;
                goto LABEL_14;
            }
        }
        v12 = 0;
        v13[0] = 0;
        v11 = 4LL;
        v7 = GetVariable(
                         L"F",
                         &gAppleVendorVariableGuid,
                         0LL,
                         &v11,
                         &v12);
        if ( v7 < 0 )
        {
            v3 = v7;
            v12 = 0;
            v5 = 0LL;
        }
        else
        {
            v11 = 4LL;
            v5 = 0LL;
            v3 = GetVariable(
                             L"FirmwareFeaturesMask",
                             &gAppleVendorVariableGuid,
                             0LL,
                             &v11,
                             v13);
            if ( v3 >= 0 )
            {
                v6 = v12;
                v5 = v13[0];
            LABEL_12:
                v9 = v6;
                v10 = v5;
                goto LABEL_13;
            }
            v12 = 0;
            v13[0] = 0;
        }
        v6 = 0LL;
        goto LABEL_12;
    }
    v3 = 0LL;
LABEL_14:
    *a1 = v2;
    return v3;
}

bool sub_BF44(__int64 a1)
{
    __int64 v2; // rax
    _QWORD v4[3]; // [rsp+28h] [rbp-18h] BYREF
    
    v4[0] = 0LL;
    v2 = sub_BF7A(v4);
    return v2 >= 0 && (v4[0] & a1) != 0;
}

double sub_100CB(const char *a1, __int64 a2)
{
    __int64 v4; // rax
    double result; // xmm0_8
    char v6; // bl
    __int64 v7; // rax
    __int64 v8; // rax
    __int64 v9; // rax
    __int64 v10; // rax
    __int64 v11; // rax
    __int64 v12; // rsi
    __int64 (__fastcall *v13)(const char *, void *, __int64, __int64, __int64); // rbx
    __int64 v14; // rax
    __int64 v15; // rax
    _BYTE v16[136]; // [rsp+30h] [rbp-B0h] BYREF
    __int64 v17; // [rsp+B8h] [rbp-28h] BYREF
    _DWORD v18[7]; // [rsp+C4h] [rbp-1Ch] BYREF
    
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    EFI_FREE_POOL FreePool = mBootServices->FreePool;
    memset(v16, 0, 0x80uLL);
    v17 = 128LL;
    v18[0] = 0;
    if ( GetVariable(
                     L"r",
                     &gAppleBootVariableGuid,
                     v18,
                     &v17,
                     v16)
        || (unsigned int)sub_2826F(v16, "locked", 6LL) )
    {
        if ( a1 )
        {
            if ( (unsigned int)sub_2826F(a1, "__efiboot_recovery_reason_cmd_r__", a2) )
            {
                v4 = SetVariable(
                                 L"r",
                                 &gAppleBootVariableGuid,
                                 7LL,
                                 a2,
                                 a1);
                if ( v4 < 0 )
                    DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] %r <- RT.SV %S %g\n", v4, "r", &gAppleBootVariableGuid));
                else
                    DEBUG ((DEBUG_INFO, "#[EB|CS:SRBM] %s\n", a1));
            LABEL_23:
                if ( (unsigned __int8)sub_BF44(0x200000000LL) || (unsigned __int8)sub_BF44(0x800000LL) )
                {
                LABEL_28:
                    v11 = sub_28B6E(qword_B1E18, *(_QWORD *)(qword_B1DD8 + 32));
                    if ( !v11 )
                        DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] NULL <- EDK.EADP\n"));
                    v12 = v11;
                    v13 = *(__int64 (__fastcall **)(const char *, void *, __int64, __int64, __int64))(qword_B20A0 + 88);
                    v14 = sub_28B07(v11);
                    v15 = v13("R", &gAppleVendorVariableGuid, 7LL, v14, v12);
                    if ( v15 < 0 )
                        DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] %r <- RT.SV %S %g\n", v15, "R", &gAppleVendorVariableGuid));
                    (*(void (__fastcall **)(__int64))(qword_B2098 + 72))(v12);
                    return result;
                }
            LABEL_25:
                v10 = (*(__int64 (__fastcall **)(const __int16 *, void *, __int64, __int64, const char *))(qword_B20A0 + 88))(
                                                                                                                              L"internet-recovery-mode",
                                                                                                                              &unk_AE920,
                                                                                                                              7LL,
                                                                                                                              17LL,
                                                                                                                              "RecoveryModeDisk");
                if ( v10 < 0 )
                    DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] %r <- RT.SV %S %g\n", v10, L"internet-recovery-mode", &unk_AE920));
                else
                    DEBUG ((DEBUG_INFO, "#[EB|IRM:RMD]\n"));
                goto LABEL_28;
            }
            v6 = 0;
        }
        else
        {
            v6 = 1;
        }
        v7 = (*(__int64 (__fastcall **)(const char *, void *, _QWORD, _QWORD, _QWORD))(qword_B20A0 + 88))(
                                                                                                          "r",
                                                                                                          &gAppleBootVariableGuid,
                                                                                                          0LL,
                                                                                                          0LL,
                                                                                                          0LL);
        if ( v7 < 0 && v7 != 0x800000000000000EuLL )
            DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] %r <- RT.SV- %S %g\n", v7, "r", &gAppleBootVariableGuid));
        if ( a1 )
        {
            if ( !v6 )
                goto LABEL_25;
            goto LABEL_23;
        }
    }
    else
    {
        DEBUG ((DEBUG_INFO, "#[EB|CS:RBML]\n"));
        if ( a1 )
            goto LABEL_23;
    }
    v8 = (*(__int64 (__fastcall **)(const __int16 *, void *, _QWORD, _QWORD, _QWORD))(qword_B20A0 + 88))(
                                                                                                         L"internet-recovery-mode",
                                                                                                         &unk_AE920,
                                                                                                         0LL,
                                                                                                         0LL,
                                                                                                         0LL);
    if ( v8 < 0 && v8 != 0x800000000000000EuLL )
        DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] %r <- RT.SV- %S %g\n", v8, L"internet-recovery-mode", &unk_AE920));
    v9 = (*(__int64 (__fastcall **)(const char *, void *, _QWORD, _QWORD, _QWORD))(qword_B20A0 + 88))(
                                                                                                      "R",
                                                                                                      &gAppleVendorVariableGuid,
                                                                                                      0LL,
                                                                                                      0LL,
                                                                                                      0LL);
    if ( v9 < 0 && v9 != 0x800000000000000EuLL )
        DEBUG ((DEBUG_INFO, "#[EB.CS.SRBM|!] %r <- RT.SV- %S %g\n", v9, "R", &gAppleVendorVariableGuid));
    return result;
}

void sub_9FE7(__int64 a1, __int64 a2)
{
    if ( a1 && a2 )
        sub_100CB();
    sub_22C97(1LL, "#[EB|B:RB]\n");
    sub_97BF(1LL, 5u);
    while ( 1 )
        ;
}

#pragma mark ========================================= functions end ==================================

EFI_STATUS
EFIAPI
UefiMain (
          IN EFI_HANDLE        ImageHandle,
          IN EFI_SYSTEM_TABLE  *SystemTable
          )
{
    
    EFI_STATUS Status = RETURN_SUCCESS;
    
    /*
     UINT64 a1 = 0xAAAAAAAAAAAAAAAA;
     UINT64 a2 = 0xAAAAAAAAAAAAAAAA;
     UINT64 a3 = 0xAAAAAAAAAAAAAAAA;
     
     
     UINT32 a4 = 0;
     */
    double v2 = 0;
    UINT64 v3 = 0;
    UINT64 v6 = 0;
    const char* v7 = 0;
    
    UINT64 v8; // rcx
    UINT64 v9; // r8
    UINT64 v11; // r8
    UINT32* v12;
    UINT64 v13;
    char* v14;
    UINT64 v15;
    const char *v17; // rdi
    UINT64 v18; // rax
    UINT64 v19; // rax
    UINT64 v23;
    UINT64 v25; // rsi
    UINT64 v26; // rax
    UINT64 v29;
    const char *v30;
    UINT64 v31;
    UINT64 v44[16]; // [rsp+30h] [rbp-190h] BYREF
    UINT32 v45[18]; // [rsp+B0h] [rbp-110h] BYREF
    UINT64 v48[16];
    UINT32 v49[18];
    UINT64 v56[3];
    memset(v56, 170, sizeof(v56));
    UINT64 v57 = 0;
    UINT64 v58 = 0;
    UINT64 v62 = 0;
    UINT32 *v63;
    UINT64 savedregs;
    EFI_OS_INFO_PROTOCOL* v59 = NULL;
    mImageHandle  = ImageHandle;
    mSystemTable = SystemTable;
    sub_28C58(ImageHandle,SystemTable);
    EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
    EFI_SET_VARIABLE SetVariable = mRuntimeServices->SetVariable;
    
    sub_E68E();
    sub_22DC7(0);
    DEBUG ((DEBUG_INFO,"#[EB|VERSION] <\"%s\">\n",L"bootbase.efi 495.140.2~17 (Official), built 2021-08-30T07:02:12-0"));
    DEBUG ((DEBUG_INFO,"#[EB|BUILD] <\"%S\">\n",L"BUILD-INFO[308]:{\"DisplayName\":\"bootbase.efi\",\"DisplayVersion\":\"495.140.2~17\",\"RecordUuid\":\"6B6B10D3-E712-4F5D-9988-B4A9386C79CF\",\"BuildTime\":\"2021-08-30T07:02:12-0700\",\"ProjectName\":\"efiboot\",\"ProductName\":\"bootbase.efi\",\"SourceVersion\":\"495.140.2\",\"BuildVersion\":\"17\",\"BuildConfiguration\":\"Release\",\"BuildType\":\"Official\"}"));
    
    
    
    sub_E9AB();
    sub_5D8E();
    if ( (qword_AD3A0 & 1) != 0 ){
        sub_E869();
    }
    sub_1217B(0,0);
    sub_12453("Start");
    v3 |= 0x1;
    sub_5E29(v3,0,0);
    Status = sub_16C17();
    if(Status < 0){
        sub_E617("#[EB.B.MN|!] %r <- EB.G.ICM\n", Status);
    }
    
    if ( (qword_B1DE8 & 2) != 0 || qword_AD380 >= 3 )
        sub_15501(2LL);
    sub_8FAD();
    Status = sub_BA0F();
    
    EFI_LOCATE_PROTOCOL LocateProtocol = mBootServices->LocateProtocol;
    
    if ( Status < 0 )
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB.B.MN|!] %r <- EB.BST.IPI\n", Status));
    }
    else
    {
        v7 = sub_E237();
        
        for (UINT32 i = 0; i < off_AD250_count; i++) {
            if(strcmp(v7, off_AD250[i]) == 0){
                break;
            }
            if(i == off_AD250_count){
                goto LABEL_22;
            }
        }
        
        
        v45[0] = 0;
        v44[0] = 4LL;
        
        Status = GetVariable (
                              L"e",
                              &gEfiGlobalVariableGuid,
                              NULL,
                              v44,
                              v45);
        
        
        if ( Status < 0 )
        {
            v8 = (UINT64)qword_B1F28;
            if ( qword_B1F28 )
            {
                v9 = 0LL;
            }
            else
            {
                
                
                Status = LocateProtocol ((EFI_GUID *)&qword_ADC40,
                                         0LL,
                                         (void**)&qword_B1F28);
                
                
                v8 = (UINT64)qword_B1F28;
            }
            if ( v8 && v9 >= 0 )
                (*(void (**)(void))(v8 + 24))();
            else
                DEBUG ((DEBUG_INFO,"AAPL: #[EB.B.IEP|!] %r <- BS.LocP %g\n", v9, &qword_ADC40));
        }
    }
LABEL_22:
    
    
    
    Status = LocateProtocol (&gAppleOSLoadedNamedEventGuid,0LL, (void**)&v59);
    
    if ( Status >= 0 )
    {
        if ( v59->Revision )
        {
            sub_12453("Start OSName");
            (*(void (**)(const char *))(v59->OSName))("macOS 11.0");
            sub_12453("End OSName");
            if ( v59->Revision >= 2u )
            {
                sub_12453("Start OSVendor");
                (*(void ( **)(const char *))(v59->OSVendor))("Apple Inc.");
                sub_12453("End OSVendor");
            }
        }
    }
    
    sub_5E29(0LL, 0LL, 2LL);
    sub_12453("Start InitDeviceTree");
    sub_12759();
    memset(v44, 0xAAu, sizeof(v44));
    v58 = 0xAAAAAAAAAAAAAAAAuLL;
    void *addr_FFFFFF01 = (void*)0xFFFFFF01;
    if ( (UINT32)sub_2826F((char*)0xFFFFFF00, "__AAPL__", 8LL) )
    {
        
        
        v58 = 6LL;
        
        Status = GetVariable(
                             L"ROM",
                             &gAppleVendorVariableGuid,
                             0LL,
                             &v58,
                             &v57);
        
        if ( Status < 0 )
        {
            Status = SetVariable (
                                  L"ROM",
                                  &gAppleBootVariableGuid,
                                  6,
                                  6,
                                  addr_FFFFFF01
                                  );
            
            if ( Status < 0 )
                DEBUG ((DEBUG_INFO,"AAPL: #[EB.B.SRNO|!] %r <- RT.SV %S %g\n", Status, L"ROM", &gAppleBootVariableGuid));
            else
                DEBUG ((DEBUG_INFO,"AAPL: #[EB|B:RRVAR] <%.*b>\n", 6LL, addr_FFFFFF01));
        }
        v58 = 128LL;
        sub_E5B0(v44, 128LL);
        
        
        Status = GetVariable(
                             L"MLB",
                             &gAppleVendorVariableGuid,
                             0LL,
                             &v58,
                             v44);
        
        if ( Status < 0 )
        {
            v12 = v45;
            memset(v45, 0xAAu, sizeof(v45));
            
            v11 |= 0xff;
            EFI_COPY_MEM CopyMem = mBootServices->CopyMem;
            EFI_SET_MEM SetMem = mBootServices->SetMem;
            
            SetMem(v45, 72LL, v11);
            CopyMem(v45, addr_FFFFFF01 + 7, 72LL);
            v13 = 0LL;
            while ( *(char *)v12 != 0xFF )
            {
                ++v13;
                v12 = (UINT32 *)((char *)v12 + 18);
                if ( v13 == 4 )
                {
                    v13 = 4LL;
                    goto LABEL_37;
                }
            }
            if ( !v13 )
                goto LABEL_42;
        LABEL_37:
            v14 = (char *)&savedregs + 18 * v13 - 272;
            v15 = -1LL;
            while ( v14[v15++ - 17] != 32 );
            v17 = (char *)&savedregs + 18 * v13 - 290;
            
            Status = SetVariable (
                                  L"MLB",
                                  &gAppleBootVariableGuid,
                                  6,
                                  v15,
                                  (void*)v17
                                  );
            
            if ( Status < 0 )
            {
                DEBUG ((DEBUG_INFO,"AAPL: #[EB.B.SRNO|!] %r <- RT.SV %S %g\n", v18, L"MLB", &gAppleBootVariableGuid));
            }
            else
            {
                v14[v15 - 18] = 0;
                DEBUG ((DEBUG_INFO,"AAPL: #[EB|B:MLBVAR] %s\n", v17));
            }
        }
    }
    
LABEL_42:
    sub_12453("Start InitDeviceTree");
    sub_B5EA();
    sub_12453("End InitDeviceTree");
    sub_5E29(0LL, 0LL, 3LL);
    qword_B1E08 = 0xFFFFFFFFLL;
    v19 = sub_1D3BB(1LL, 1LL, (UINT64)((char *)sub_E4B2 - (char *)sub_E430 + 4095) >> 12, &qword_B1E08);
    if ( v19 < 0 )
        sub_E617(
                 "#[EB.B.MN|KCG!] %r <- EB.M.BAP %qd\n",
                 v19,
                 (UINT64)((char *)sub_E4B2 - (char *)sub_E430 + 4095) >> 12);
    sub_240B0(qword_B1E08, sub_E430, (char *)sub_E4B2 - (char *)sub_E430);
    sub_5E29(0LL, 0LL, 4LL);
    sub_12453("Start InitMemoryConfig");
    sub_BBF8(130);
    sub_12453("End InitMemoryConfig");
    sub_5E29(0LL, 0LL, 5LL);
    sub_12453("Start CheckHibernate");
    UINT64 v20 = sub_171EC(v56, v23, v2);
    if ( (char)v20 )
        BYTE1(qword_B1DE8) |= 0x20u;
    else
        sub_5DD0(v56, v23, v2);
    sub_12453("End CheckHibernate");
    sub_16E01();
    sub_1535C();
    if ( (UINT8)sub_4833() )
    {
        v25 = 0x200000LL;
        DEBUG ((DEBUG_INFO,"#[EB.B.MN|BM:+SB]\n"));
    }
    else
    {
        if ( !(UINT8)sub_11AC8() )
            goto LABEL_52;
        v25 = 0x1000000LL;
        DEBUG ((DEBUG_INFO,"#[EB.B.MN|BM:+TB]\n"));
    }
    qword_B1DE8 |= v25;
    DEBUG ((DEBUG_INFO,"AAPL: This is a test boot.efi!!!\n"));
LABEL_52:
    if ( (UINT8)sub_11A43() )
    {
        DEBUG ((DEBUG_INFO,"#[EB.B.MN|BM:+FD]\n"));
        
        BYTE3(qword_B1DE8) |= 2u;
    }
    v26 = sub_4000();
    if ( v26 < 0 )
        sub_E617("#[EB.B.MN|!] %r <- EB.G.SIM4E\n", v26);
    sub_1E045();
    sub_9A24(0,0,0,0);
    sub_90D9(3LL);
    UINT64 v27 = sub_9014();
    if ( !v27 )
        sub_90D9(8LL);
    sub_12453("Start ProcessOptions");
    sub_1E706((UINT64)qword_B1E20, (UINT64)qword_B1E28, (CHAR16*)qword_B1DF0, (UINT64*)&qword_B1DE0);
    sub_12453("End ProcessOptions");
    sub_A038();
    if ( byte_AD220 )
    {
        const char * v28 = sub_E237();
        v27 = sub_1E4E2((char*)v28);
        if ( v27 < 0 )
        {
            sub_15D44(0, 0);
            if ( (qword_B1DE8 & 2) != 0 )
            {
                sub_15501(2LL);
            }
            else if ( sub_153D5() >= 0 )
            {
                sub_16556(0LL, 0LL);
                sub_15583(2LL);
            }
            DEBUG ((DEBUG_INFO,"\n***********************************************************\n"));
            DEBUG ((DEBUG_INFO,"This version of Mac OS X is not supported on this platform!\n"));
            DEBUG ((DEBUG_INFO,"***********************************************************\n"));
            DEBUG ((DEBUG_INFO,"Reason: %s\n", v28));
            DEBUG ((DEBUG_INFO,"Sleeping for 30 seconds before exiting...\n"));
            
            EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount = mBootServices->GetNextMonotonicCount;
            GetNextMonotonicCount((UINT64*)30000000LL);
            sub_97BF(2LL, 4LL);
        }
    }
    if ( v63 && *(UINT32 *)v63 >= 3u )
    {
        v48[0] = 1LL;
        DEBUG ((DEBUG_INFO,"#[EB|B:VAw]\n"));
        (*(void ( **)(UINT64 *))(v63 + 24))(v48);
    }
    if ( (qword_B1DE8 & 0x1000) != 0 )
    {
        qword_B1DE8 &= 0xFFFFFFFFFFFFF6FFuLL;
        DEBUG ((DEBUG_INFO,"#[EB.B.MN|BM:ROS]\n"));
        sub_5E29(0LL, v27, 19LL);
    }
    if ( byte_AD221 && (qword_B1DE8 & 0x3001) == 0 )
        sub_1FBBA();
    if ( (qword_B1DE8 & 0x800) != 0 )
    {
        memset(v48, 0, sizeof(v48));
        *(UINT64 *)v49 = 128LL;
        LODWORD(v62) = 0;
        sub_5E29(64LL, 0LL, 25LL);
        v29 = GetVariable(
                          L"recovery-boot-mode",
                          &gAppleBootVariableGuid,
                          &v62,
                          v49,
                          v48);
        if ( v29 )
        {
            DEBUG ((DEBUG_INFO, "#[EB.B.MN|!] %r <- RT.GV %S %g\n", v29, L"recovery-boot-mode", &gAppleBootVariableGuid));
            v30 = "__efiboot_recovery_reason_cmd_r__";
            v31 = 33LL;
        }
        else
        {
            v31 = *(UINT64 *)v49;
            v30 = (const char *)v48;
        }
        sub_9FE7(v30, v31);
    }
    sub_1F32C(v24);
    if ( !(char)v24 && (qword_B1DE8 & 0x5000) == 0x4000 )
    {
        v32 = sub_1C7E8();
        if ( v32 < 0 )
            sub_E617("#[EB.B.MN|!] %r <- EB.MM.AKMR\n", v32);
    }
    sub_118F7(ImageHandle);
    sub_ECA5();
    v64[0] = 1;
    if ( (qword_B1DE8 & 0x100) != 0 )
    {
        sub_5E29(0LL, 0LL, 6LL);
        sub_12453("Start LoadCoreStorageConfiguration");
        v33 = sub_F4B4(qword_B1E20, qword_B1E28, v24);
        sub_12453("End LoadCoreStorageConfiguration");
        v34 = 0x800000000000000EuLL;
        if ( (v33 & 0xFFFFFFFFFFFFFFEFuLL) == 0x800000000000000EuLL && (qword_B1DE8 & 0x80000) != 0 )
        {
            v37 = sub_10451();
            if ( v37 < 0 )
                sub_22C97(1LL, "#[EB.B.MN|!] %r <- EB.CS.UVNO\n", v37);
        }
        else if ( v33 >= 0 )
        {
            LOBYTE(v34) = 20;
            sub_43E3(v34);
            BYTE1(qword_B1DE8) |= 2u;
            sub_12453("Start UnlockCoreStorageVolumeKey");
            v35 = sub_10608(ImageHandle, v57, v64, v24);
            sub_12453("End UnlockCoreStorageVolumeKey");
            LOWORD(v36) = 8;
            sub_5E29(v36, v35, 20LL);
            if ( v35 < 0 )
                sub_E617("#[EB.B.MN|!] %r <- EB.CS.UFVK\n", v35);
        }
    }
    v38 = qword_B1DE8;
    if ( (qword_B1DE8 & 0x200) == 0 )
    {
        sub_15D44(0, 0);
        v38 = qword_B1DE8;
    }
    if ( (v38 & 2) != 0 )
    {
        qword_B1DE8 = v38 & 0xFFFFFFFFFFFFFFDFuLL;
        sub_12453("Start SetConsoleMode");
        sub_15501(2LL);
        v39 = "End SetConsoleMode";
    }
    else
    {
        sub_12453("Start DrawBootGraphics");
        v39 = "End DrawBootGraphics";
        if ( sub_153D5() >= 0 && v64[0] )
        {
            sub_16556(0LL, 0LL);
            sub_15583(1LL);
        }
    }
    sub_12453(v39);
    if ( (qword_B1DE8 & 0x801000) == 0x1000 )
    {
        v40 = sub_60CD();
        sub_22C97(1LL, "#[EB.B.MN|!] %r <- EB.R.RVBI\n", v40);
        if ( v40 < 0 )
        {
            if ( (qword_B1DE8 & 0x1200000) != 0 )
            {
            LABEL_100:
                sub_E617("#[EB.B.MN|RDV!] %r\n", v40);
                goto LABEL_105;
            }
            if ( (UINT32)sub_241B1() )
            {
                DEBUG ((DEBUG_INFO,"#[EB|B:RCSR]\n"));
            }
            else
            {
                if ( v40 != 0x8000000000000018uLL )
                    goto LABEL_100;
                DEBUG ((DEBUG_INFO,"#[EB|B:RWKA]\n"));
            }
        }
    }
LABEL_105:
    if ( (char)v24 )
    {
        memset(v48, 170, 64);
        v49[0] = 64;
        sub_12453("Start LookupCoreStorageVolumeKey");
        v41 = sub_116A8(v56, v48, v49);
        if ( v41 < 0 )
            sub_22C97(1LL, "#[EB.B.MN|!] %r <- EB.CS.LFVK\n", v41);
        sub_12453("Start FinishFDEHibernate");
        sub_19D22(v48, v49[0]);
        sub_12453("End FinishFDEHibernate");
    }
    *(UINT64 *)v49 = 0LL;
    v62 = 0LL;
    v61 = 0LL;
    v58 = 0LL;
    v59 = 0LL;
    memset(v48, 170, 20);
    v60 = (UINT64 *)0xAAAAAAAAAAAAAAAALL;
    if ( (*(UINT64 ( **)(void *, UINT64, UINT64 ( ***)(UINT64, UINT64 *)))(qword_B2098 + 320))(
                                                                                               &unk_ADD70,
                                                                                               0LL,
                                                                                               &v61) >= 0
        && (*v61)(v61, &v59) >= 0
        && (*(UINT64 ( **)(UINT64, void *, UINT64 *, UINT64, UINT64, int))(qword_B2098 + 280))(
                                                                                               v59,
                                                                                               &unk_ADD80,
                                                                                               &v58,
                                                                                               qword_B1DD0,
                                                                                               0LL,
                                                                                               2) >= 0 )
    {
        v60 = v48;
        LOBYTE(v42) = 1;
        v43 = sub_1207F("/chosen", v42);
        v44 = sub_19F56(qword_B1E20, qword_B1E28, 0, *(UINT64 *)(qword_B1DD8 + 32), (UINT64)&v62, (UINT64)v49);
        if ( v44 < 0 )
        {
            sub_22C97(1LL, "#[EB.B.SBS|!] %r <- EB.LD.LF\n", v44);
        }
        else
        {
            sub_22C97(1LL, "#[EB.B.SBS|SZ] %qd\n", v62);
            if ( (*(UINT64 ( **)(UINT64, void *, UINT64, UINT64, UINT64, UINT64 **))(v58 + 8))(
                                                                                               v58,
                                                                                               &unk_ADD90,
                                                                                               0LL,
                                                                                               *(UINT64 *)v49,
                                                                                               v62,
                                                                                               &v60) >= 0 )
                sub_22C97(1LL, "#[EB|B:SHA] <%.*b>\n", 20LL, v48);
            sub_11BA4(v43, (UINT32)"boot-signature", 20, (UINT32)v48, 1);
        }
    }
    sub_6018();
    sub_9F09();
    v45 = sub_1A64E(qword_B1E20, qword_B1E30, v50);
    if ( v45 < 0 )
    {
        v46 = v45;
        sub_5E29(0LL, v45, 22LL);
        sub_E617("#[EB.B.MN|!] %r <- EB.LD.LKC\n", v46);
    }
    (*(void ( **)(const UINT16 *, void *, UINT64, UINT64, UINT64))(qword_B20A0 + 88))(
                                                                                      L"DefaultBackgroundColor",
                                                                                      &gAppleVendorVariableGuid,
                                                                                      0LL,
                                                                                      0LL,
                                                                                      0LL);
    sub_12453("Start InitBootStruct");
    sub_C0FD(qword_B1DD8, qword_B1DE0);
    sub_12453("End InitBootStruct");
    sub_12453("Start LoadRAMDisk");
    sub_1ABE0(qword_B1E20, qword_B1E28);
    sub_12453("End LoadRAMDisk");
    if ( qword_B1F38 )
    {
        sub_12453("Start StopAnimation");
        sub_15301();
        sub_12453("End StopAnimation");
    }
    sub_12453("Start FinalizeBootStruct");
    sub_C81C(&v56[2], qword_B1DE0, v50);
    sub_12453("End FinalizeBootStruct");
    DEBUG ((DEBUG_INFO,"#[EB|B:BOOT]\n"));
    sub_12453("End");
    sub_22DC7(11LL);
    _disable();
    qword_B1E08(qword_B1E40, v51);
    sub_E617("#[EB|B:BOOT!] Kernel entry failed\n");
    sub_9707(0x8000000000000015uLL);
    return Status;
}
