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
#include <Guid/HobList.h>
#include <Guid/OcVariable.h>

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


#pragma mark ==================================== data struct define end ================================





#pragma mark ========================================variable data begin ================================


STATIC EFI_HANDLE  mImageHandle;

STATIC EFI_SYSTEM_TABLE  *mSystemTable;

STATIC EFI_BOOT_SERVICES * mBootServices;

STATIC EFI_RUNTIME_SERVICES *mRuntimeServices;

void *GuidHob_24 = NULL;

EFI_CPU_ARCH_PROTOCOL *mCpu = NULL;

/*
 #define gEfiHobListGuid { 0x7739F24C, 0x93D7, 0x11D4, { 0x9A, 0x3A, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }}
 
 */
EFI_GUID  unknown_hob_guid = {
    0xef4ae2dd, 0xb736, 0x40e3, {0x80, 0x61, 0xa7, 0x46, 0x33, 0x34, 0x7f, 0x23 }
};


UINT8 byte_AD1D8 = 0;

UINT8 byte_AD1D9 = 0;

UINT8 byte_AD1DA = 0;

UINT8 byte_AD1DB = 0;

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

UINT32 dword_AD820 = 0xFFFFFFFF;

UINT64 qword_ADFE8 = 0;

char unk_AE980[0x3e] = {0};

UINT8 byte_AE978 = 0;

char *qword_AEE30 = NULL;

char* qword_AEE38 = NULL;

UINT64 qword_AEE40 = 0;

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

EFI_CONSOLE_CONTROL_SCREEN_MODE dword_AF158 = 0;

EFI_CONSOLE_CONTROL_SCREEN_MODE dword_AF15C = 0;

UINT8 byte_B0331 = 0;

UINT8 byte_B0332 = 0;

UINT8 byte_B0333 = 0;

UINT8 byte_B0340 = 0;

UINT8 byte_B0341 = 0;

UINT8 byte_B0342 = 0;

UINT8 byte_B0370 = 0;

UINT8 byte_B1DC9 = 0;

UINT64** qword_B01E8 = 0;

UINT64* qword_B01F0 = 0;

UINT64* qword_B01F8 = 0;

UINT64 qword_B0348 = 0;

UINT64* qword_B0358 = 0;

UINT32* qword_B0360 = (UINT32*)4261634048LL;

UINT64 qword_B1DE8 = 0;

EFI_CONSOLE_CONTROL_PROTOCOL* qword_B1F40 = NULL;

char* qword_B2078 = 0;

char* qword_B2080 = 0;

UINT32 addr_FE03401C = 0xFE03401C;


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
        for (int i = 0LL; i != 508; i += 4LL )
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

typedef struct LOG_CONFIG_INFO{
    char* name;
    UINT64 config1;
    UINT32 config2;
    UINT32 config3;
    UINT64* config4;
    UINT64* sign;
}LOG_CONFIG_INFO;

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

BOOLEAN _bittest64(UINT64 *a, int b)
{
    return (*a & (1LL << b)) != 0;
}

// strlen
UINT64 sub_2822A(const char* a1)
{
    return strlen(a1);
}


UINT64 sub_28556(char* a1, char* a2, UINT64 a3)
{
    UINT64 result; // rax
    UINT64 v4; // r9
    int v5; // r10d
    
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
    int v7; // esi
    bool v8; // dl
    int v9; // eax
    UINT64 v10; // rax
    UINT64 v11; // r15
    char *v12; // r10
    int v13; // r14d
    int v14; // esi
    int v15; // ebx
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
    int in_v = 0;
    in8(port,in_v);
    
    
    if(in_v == V_5A){
        out8(port,V_A5);
        int in_v = 0;
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

void sub_22D1D(int a1){
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
        //  while ( (unsigned int)sub_28246(*v1, "name") ) strcmp
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
    memset(buffer, result, (unsigned int)byteSize);
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

EFI_STATUS sub_22DC7(unsigned int a1){
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
            unsigned int newA1 = a1 - 2;
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
                        int v4 = 1;
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
    for(int i = 0; i < log_config_list_count;i++){
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
    v8 = (unsigned int)sub_46911(a2, 12LL);
    v9 = sub_1D3BB(v5, 9LL, v8, v17);
    if ( v9 < 0 )
    {
        DEBUG ((DEBUG_INFO,"AAPL: #[EB.DBG.IDTP|!] %r <- EB.M.BAP %qd\n", v9, v8));
        Status = 0LL;
        qword_AEFE8 = 0LL;
        qword_AEFF0 = 0LL;
        return Status;
    }
    
    if ( !qword_AEFE8 || !qword_AEFF8 || (v10 = *(unsigned int *)(qword_AEFF8 + 12) && a2 < v10))
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



UINT64 sub_281BA(UINT64 a1, int a2, char *a3, UINT8 ***a4)
{
    UINT64 v4; // esi
    UINT64 v6[4]; // [rsp+20h] [rbp-20h] BYREF
    
    v4 = a1;
    v6[0] = a1;
    v6[1] = a1 + a2 - 1;
    
#if 0
    sub_271BC(a3, (void (__fastcall *)(UINT64, UINT64))sub_7CE0E194, (UINT64)v6, a4);
    *(char *)v6[0] = 0;
#endif
    return (unsigned int)(v6[0] - v4);
}

UINT64 sub_124A8(int a1, UINT64 a2, UINT64 a3)
{
    UINT64 v6 = 0; // rsi
    UINT64 v7; // r15
    int v8; // eax
    int v9; // edx
    int v10; // edx
    UINT64 v11; // rax
    UINT64 v12; // rcx
    UINT64 v13; // rdi
    UINT64 v14; // rbx
    UINT64 result = 0; // rax
    UINT64 v16; // r12
    UINT64 v17; // rax
    UINT64 v18; // r12
    unsigned int v19; // eax
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
    v12 = *(unsigned int *)(qword_7CE94FF8 + 12);
    v13 = v10;
    v14 = qword_7CE94FF0;
    if ( v12 + v10 + 24 <= (UINT64)qword_7CE94FF0 )
    {
        v20 = qword_7CE94FE8;
    LABEL_14:
        *(_DWORD *)(v20 + v12) = a1;
        *(_DWORD *)(v20 + v12 + 4) = v13;
        *(UINT64 *)(v20 + v12 + 8) = v7;
        *(UINT64 *)(v20 + v12 + 16) = v6;
        v21 = (unsigned int)(*(_DWORD *)(v11 + 12) + 24);
        *(_DWORD *)(v11 + 12) = v21;
        sub_7CE0A0B0(v20 + v21, byte_7CE95010, v13);
        result = qword_7CE94FF8;
        *(_DWORD *)(qword_7CE94FF8 + 12) += v13;
        ++*(_DWORD *)(result + 8);
        return result;
    }
    result = 0xAAAAAAAAAAAAAAAAuLL;
    v22[0] = 0xAAAAAAAAAAAAAAAAuLL;
    if ( !byte_7CE94FE0 && qword_7CE98098 )
    {
        v22[0] = 0xFFFFFFFFLL;
        qword_7CE94FF0 = sub_7CE2C8F6(qword_7CE94FF0, 1LL);
        v16 = (unsigned int)sub_7CE2C911(qword_7CE94FF0, 12);
        v17 = sub_7CE033BB(1LL, 9LL, v16, v22);
        if ( v17 < 0 )
            return sub_7CE08C97(1LL, "#[EB.DBG.TTPI|!] %r <- EB.M.BAP %qd\n", v17, v16);
        sub_7CE0A0B0(v22[0], qword_7CE94FE8, *(unsigned int *)(qword_7CE94FF8 + 12));
        v18 = qword_7CE94FE8;
        v19 = sub_7CE2C911(v14, 12);
        sub_1D413(v18, v19);
        v11 = v22[0];
        qword_7CE94FE8 = v22[0];
        qword_7CE94FF8 = v22[0];
        v12 = *(unsigned int *)(v22[0] + 12);
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
        v0 = (*(UINT64 (__fastcall **)(void *, UINT64, UINT64 *))(qword_7CEB0098 + 320))(
                                                                                         &unk_7CEABBA0,
                                                                                         0LL,
                                                                                         &qword_7CEAE210);
        if ( v0 < 0 )
        {
            sub_22C97(1LL, "#[EB.RTC.CP|!] %r <- BS.LocP %g\n", v0, &unk_7CEABBA0);
            byte_7CEAE320 = 1;
        }
        else
        {
            v1 = (*(UINT64 (__fastcall **)(UINT64, UINT64, UINT64 (__fastcall *)(), UINT64, UINT64 *))(qword_7CEB0098 + 80))(
                                                                                                                             513LL,
                                                                                                                             8LL,
                                                                                                                             sub_7CE1F067,
                                                                                                                             0LL,
                                                                                                                             &qword_7CEB0060);
            if ( v1 < 0 )
                sub_22C97(1LL, "#[EB.RTC.CP|!] %r <- BS.CrE 0x%08X\n", v1, 513LL);
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
    unsigned int v6; // ebx
    UINT64 v7; // rsi
    char v8; // al
    UINT8 v9; // cl
    unsigned int v10; // esi
    int v11; // edi
    unsigned int v12; // ecx
    
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
                        return (*(UINT64 (__fastcall **)(UINT64, UINT64, UINT64, UINT64))(qword_7CEAE210 + 16))(
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
                    if ( (unsigned __int16)word_7CEAE25E != (unsigned __int16)sub_7CE0C53F(0LL, &unk_7CEAE22E, 48LL) )
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
                    if ( (v11 | (v10 << 8)) != (unsigned __int16)v12 )
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

UINT64 sub_5E29(char a1, UINT64 a2, int a3)
{
    UINT8 v3; // cl
    UINT64 v4; // rsi
    EFI_STATUS Status = 0; // rax
    int v6; // edx
    int v7; // r8d
    int v8; // r9d
    int v9; // [rsp+10Ch] [rbp-30h]
    int v10; // [rsp+114h] [rbp-28h]
    int v11; // [rsp+11Ch] [rbp-20h]
    int v12; // [rsp+124h] [rbp-18h]
    
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
            2 * (unsigned int)(UINT8)byte_AE978,
            (UINT8)byte_AD1D8,
            v3,
            a3,
            byte_AD1DB));
    if ( v4 == 1 )
    {
#if 0
        v6 |= 4;
        v7 |= -80;
        EFI_STATUS = sub_20EDF((unsigned int)&byte_AD1D8, v6, v7, v8, v9, v10, v11, v12);
        if ( result < 0 )
            return sub_22C97(1LL, "#[EB.WL.WL|!] %r <- EB.WL.WLF\n", result);
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

EFI_STATUS sub_E580(char *a1, UINT64 a2, UINT64 a3)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    sub_E5B3(a1, a3);
    return Status;
}

EFI_STATUS sub_228A3(char *a1, UINT64 a2)
{
    EFI_STATUS Status = RETURN_SUCCESS;
    UINT8 ***v2; // r14
    char v5[149]; // [rsp+20h] [rbp-D0h] BYREF
    char v6; // [rsp+B5h] [rbp-3Bh] BYREF
    char* v7[7]; // [rsp+B8h] [rbp-38h] BYREF
    
    v2 = (UINT8 ***)a2;
    a2 |= -86;
    sub_E580(v5, a2, 149LL);
    v7[0] = v5;
    v7[1] = v5;
    v7[2] = &v6;
#if 0
    sub_271BC(a1, (void (__fastcall *)(UINT64, UINT64))sub_7CE209F1, (UINT64)v7, v2);
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


EFI_STATUS sub_15501(unsigned int a1)
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
  if ( *(_QWORD *)(qword_B2090 + 104) )
  {
    v2 = *(_QWORD *)(qword_B2090 + 112);
    v3 = 0LL;
    while ( !sub_46304(v2, (UINT64)&unk_ADCC0) )
    {
      ++v3;
      v2 += 24LL;
      if ( *(_QWORD *)(qword_B2090 + 104) <= v3 )
        return v0;
    }
    v4 = *(_QWORD *)(v2 + 16);
    if ( !sub_4632A(v4, "_SM_", 4LL) )
    {
      v5 = (char *)*(unsigned int *)(v4 + 24);
      v6 = (unsigned int)v5 + *(unsigned __int16 *)(v4 + 22);
      if ( (UINT64)v5 < v6 && (UINT64)(v5 + 4) <= v6 )
      {
        v7 = *(unsigned int *)(v4 + 24);
        while ( 1 )
        {
          if ( *v5 == 2 )
          {
            if ( (UINT64)(v5 + 17) > v6 )
              return v0;
            if ( *(char *)(v7 + 5) )
              break;
          }
          v8 = *(unsigned __int8 *)(v7 + 1);
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
        v10 = sub_BBB2(&v5[*(unsigned __int8 *)(v7 + 1)]);
        sub_282BC(&unk_AE980, v10, 64LL);
        byte_AE9BF = 0;
        sub_22C97(1LL, "#[EB|BRD:SMBIOS] %e\n", &unk_AE980);
        return 0LL;
      }
    }
  }
#endif
  return Status;
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
     
     
     int a4 = 0;
     */
    
    UINT64 v3 = 0;
    mImageHandle  = ImageHandle;
    mSystemTable = SystemTable;
    sub_28C58(ImageHandle,SystemTable);
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
    sub_BA0F();
    DEBUG ((DEBUG_INFO,"AAPL: This is a test boot.efi!!!\n"));
    
    return Status;
}
