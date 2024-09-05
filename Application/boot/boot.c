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
#include <string.h>
#include <Library/OcMainLib.h>
#include <Uefi.h>



#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>

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

STATIC EFI_HANDLE  mImageHandle;

STATIC EFI_SYSTEM_TABLE  *mSystemTable;

STATIC EFI_BOOT_SERVICES * mBootServices;

STATIC EFI_RUNTIME_SERVICES *mRuntimeServices;


typedef struct StringStruct1 {
    void* string1;
    char* string2;
    char* string3;
    void* next2;
    void* next3;
}StringStruct1;

typedef struct StringStruct2 {
    StringStruct1* next0;
    char* string2;
    void* next1;
    void* next2;
}StringStruct2;


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

void *GuidHob_24 = NULL;

/*
 #define gEfiHobListGuid { 0x7739F24C, 0x93D7, 0x11D4, { 0x9A, 0x3A, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }}
 
 */
EFI_GUID  unknown_hob_guid = {
    0xef4ae2dd, 0xb736, 0x40e3, {0x80, 0x61, 0xa7, 0x46, 0x33, 0x34, 0x7f, 0x23 }
};

void sub_7CE43F47(void *GuidHob)
{
    GuidHob_24 = GuidHob;
}

EFI_STATUS sub_7CE43C58(
                        IN EFI_HANDLE        ImageHandle,
                        IN EFI_SYSTEM_TABLE  *SystemTable
                        )
{
    EFI_STATUS  Status;
    
    mSystemTable = SystemTable;
    mBootServices = SystemTable->BootServices;
    mRuntimeServices = SystemTable->RuntimeServices;
    void* hobList = NULL;
    Status = HobLibConstructorPtr (SystemTable,&hobList);
    
    void *GuidHob = GetNextGuidHob (&unknown_hob_guid, hobList);
    sub_7CE43F47(GuidHob + 3);
    
    
    
    return Status;
    
}

void* AllocatePool_malloc(UINTN bufferSize)
{
    void* bufferPtr = NULL; // [rsp+20h] [rbp-10h] BYREF
    EFI_ALLOCATE_POOL AllocatePool = mBootServices->AllocatePool;
    AllocatePool(4LL, bufferSize, &bufferPtr);
    return bufferPtr;
}

UINT64** qword_7CECB1E8 = 0;

UINT64* qword_7CECB1F8 = 0;

UINT64* qword_7CECB1F0 = 0;

UINT64* sub_7CE38528(){
    UINT64* result = qword_7CECB1F8;
    UINT64** buffer = NULL;
    UINT64 v1 = 0;
    if(qword_7CECB1F8){
        v1 = *(UINT64 *)(qword_7CECB1F8 + 24);
    }else{
        buffer = AllocatePool_malloc(4096);
        if(buffer == NULL){
            DEBUG ((DEBUG_INFO,"#[EB.M.GT|!] NULL <- EDK.ELAP\n"));
        }
        UINT64* v3 = qword_7CECB1F8;
        for (int i = 0LL; i != 508; i += 4LL )
        {
            UINT64* v1 = v3;
            v3 = (UINT64*)&buffer[i];
            buffer[i + 3] = v1;
        }
        result = (UINT64*)buffer + 504;
        qword_7CECB1F8 = result;
        buffer[508] = (UINT64*)buffer;
        buffer[510] = 0;
        buffer[509] = 0;
        buffer[511] = qword_7CECB1F0;
        qword_7CECB1F0 = (UINT64*)buffer + 508;
    }
    qword_7CECB1F8 = (UINT64*)v1;
    result[3] = 0;
    return result;
}

void* sub_7CE382B1(UINTN bufferSize)
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
    v2[3] = (UINT64)qword_7CECB1E8;
    qword_7CECB1E8 = (UINT64**)v2;
    return buffer;
}

void *qword_7CEA0E30 = NULL;

EFI_STATUS sub_7CE0F68E()
{
    EFI_STATUS  Status;
    /*
     UINT64 v1 = 0xAAAAAAAAAAAAAAAAuLL;
     */
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
            void *buffer = sub_7CE382B1(DataSizeArray[0] + 1);
            DEBUG ((DEBUG_INFO,"buffer--::0x%x\n", buffer));
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
    
    if(qword_7CEA0E30){
        
    }
    
    return Status;
}

UINT32 dword_7CE9F820 = 0xFFFFFFFF;

BOOLEAN sub_7CE090CB(){
    EFI_STATUS  Status;
    UINTN DataSizeArray[4];
    DataSizeArray[0] = 0xAAAAAAAAAAAAAAAAuLL;
    UINT32 v0 = dword_7CE9F820;
    if(dword_7CE9F820 == 0xFFFFFFFF){
        dword_7CE9F820 = 1;
        DataSizeArray[0] = 0;
        
        EFI_GET_VARIABLE GetVariable = mRuntimeServices->GetVariable;
        
        
        
        Status = GetVariable (
                              L"boot-signature",
                              &gAppleDebugLogProtocolGuid,
                              NULL,
                              DataSizeArray,
                              NULL
                              );
        
        if(Status != RETURN_BUFFER_TOO_SMALL){
            DEBUG ((DEBUG_INFO,"#[EB.H.IS|!] %r <- RT.GV %S %g\n", Status, L"boot-signature", &gAppleDebugLogProtocolGuid));
            dword_7CE9F820 = 0;
        }
        v0 = 0;
        DataSizeArray[0] = 0;
        
        Status = GetVariable (
                              L"b",
                              &gAppleDebugLogProtocolGuid,
                              NULL,
                              DataSizeArray,
                              NULL
                              );
        
        if(Status == RETURN_BUFFER_TOO_SMALL){
            v0 = dword_7CE9F820;
        }else{
            DEBUG ((DEBUG_INFO,"#[EB.H.IS|!] %r <- RT.GV %S %g\n", Status, L"b", &gAppleDebugLogProtocolGuid));
            dword_7CE9F820 = 0;
        }
    }
    DEBUG ((DEBUG_INFO,"#[EB|H:IS] %d\n", v0));
    return dword_7CE9F820 == 1;
}

UINT64 unk_7CE9F370 = 0x00000002;

UINT64 unk_7CE9F378 = 0x00000002;

UINT8 byte_7CECB340 = 0;

UINT8 byte_7CECB342 = 0;

UINT8 byte_7CECB331 = 0;

UINT8 byte_7CECB332 = 0;

UINT8 byte_7CECB333 = 0;

UINT8 byte_7CECB341 = 0;

UINT64 qword_7CECB348 = 0;

UINT64 qword_7CE9F380 = 0;

UINT64 qword_7CE9F388 = 1;

UINT64 qword_7CEC8390 = 1;

StringStruct2* qword_7CEC9FB0 = NULL;

StringStruct2* qword_7CEC9FB8 = NULL;

StringStruct2* qword_7CEC9FC0 = NULL;

UINT32 dword_7CEC9FC8 = 0;

UINT32* qword_7CECB360 = (UINT32*)4261634048LL;

UINT64* qword_7CECB358 = 0;

UINT32* qword_7CECD078 = 0;

UINT32* qword_7CEC8398 = 0;

UINT8* byte_7CECB370 = 0;

UINT32 addr_FE03401C = 0xFE03401C;


void sub_7CE42E8D()
{
    
}

void sub_7CE42F03()
{
    
}

void sub_7CE42F1C()
{
    
}

void sub_7CE42F2F()
{
    
}

void sub_7CE42F41()
{
    
}

void sub_7CE42F5E()
{
    
}

void *off_7CEC8A40[6] =
{
    (void*)sub_7CE42E8D,
    (void*)sub_7CE42F03,
    (void*)sub_7CE42F1C,
    (void*)sub_7CE42F2F,
    (void*)sub_7CE42F41,
    (void*)sub_7CE42F5E
};

void sub_7CE43004()
{
    
}

void sub_7CE43047()
{
    
}

void sub_7CE4304D()
{
    
}

void sub_7CE4305D()
{
    
}

void sub_7CE4306A()
{
    
}

void sub_7CE43088()
{
    
}

void *off_7CEC8A70[6] =
{
    (void*)sub_7CE43004,
    (void*)sub_7CE43047,
    (void*)sub_7CE4304D,
    (void*)sub_7CE4305D,
    (void*)sub_7CE4306A,
    (void*)sub_7CE43088
};


void sub_7CE43096()
{
    
}

void sub_7CE4309C()
{
    
}

void sub_7CE430A2()
{
    
}

void sub_7CE430C3()
{
    
}

void sub_7CE430DD()
{
    
}

void sub_7CE43116()
{
    
}

void *off_7CEC8AA0[6] =
{
    (void*)sub_7CE43096,
    (void*)sub_7CE4309C,
    (void*)sub_7CE430A2,
    (void*)sub_7CE430C3,
    (void*)sub_7CE430DD,
    (void*)sub_7CE43116
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
const char *debug_tag_strings[] = {
    "INIT",
    "VERBOSE",
    "EXIT",
    "RESET:OK",
    "RESET:FAIL",
    "RESET:RECOVERY",
    "REAN:START",
    "REAN:END",
    "DT",
    "EXITBS:START",
    "EXITBS:END",
    "HANDOFF TO XNU",
    "UNKNOWN"
};

EFI_STATUS sub_7CE42D1F(){
    
    
    EFI_STATUS Status = 0;
    UINT32 ptr2 = addr_FE03401C + 0x1FE4;
    UINT64 baseZeroAddress = 0x0;
    qword_7CECB360 = (UINT32*)(ptr2 + baseZeroAddress);
    UINT32 ptr3 = addr_FE03401C + 0x2000;
    UINT32 V_5A = 0x5A;
    UINT32 V_A5 = 0xA5;
    
    UINT32 V_5A_newValue = write_read_addree(ptr3, V_5A);
    
    if(V_5A_newValue == V_5A){
        UINT32 V_A5_newValue = write_read_addree(ptr3, V_A5);
        if(V_A5_newValue == V_A5){
            qword_7CECB358 = (UINT64*)off_7CEC8A40;
            Status = 1;
            return Status;
        }
    }
    
    UINT32 ptr4 = (UINT32)addr_FE03401C - 0x1C;
    qword_7CECB360 = (UINT32*)(ptr4 + baseZeroAddress);
    V_5A_newValue = write_read_addree(ptr4, V_5A);
    if(V_5A_newValue == V_5A){
        UINT32 V_A5_newValue = write_read_addree(ptr4, V_A5);
        if(V_A5_newValue == V_A5){
            qword_7CECB358 = (UINT64*)off_7CEC8A40;
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
            qword_7CECB358 = (UINT64*)off_7CEC8A70;
            Status = 1;
            return Status;
        }
    }
    
    
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
                  : [addr1]"m" (byte_7CECB370) \
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
                      : [addr1]"m" (byte_7CECB370) \
                      : "%rcx","%rdx","r8","memory" \
                      );
        if(read_value == V_A5){
            qword_7CECB358 = (UINT64*)off_7CEC8AA0;
            Status = 1;
            return Status;
        }
    }
    
    
    return Status;
}

void sub_7CE3DD1D(int a1){
    if(a1 >= 0xC){
        DEBUG ((DEBUG_INFO, "#[EB|LOG:UNKNOWN] %d\n", a1));
        return;
    }
    EFI_TIME                LogTime;
    
    EFI_GET_TIME GetTime = mRuntimeServices->GetTime;
    if(a1 > 9 ||  GetTime (&LogTime, NULL) < 0) {
        DEBUG ((DEBUG_INFO, "#[EB|LOG:%s] _\n", debug_tag_strings[a1]));
        return;
    }
    DEBUG ((DEBUG_INFO, "#[EB|LOG:%s] %t\n", debug_tag_strings[a1], &LogTime));
}

EFI_STATUS sub_7CE42E18()
{
    EFI_STATUS  Status = 0;
    if ( !qword_7CECB358 )
        return Status;
    (*(void (**)(void))qword_7CECB358)();
    Status = 1;
    return Status;
}
EFI_STATUS sub_7CE38327(void* buffer)
{
    EFI_STATUS  Status = 0;
    EFI_FREE_POOL FreePool = mBootServices->FreePool;
    UINT64* v3 = NULL;
    Status = FreePool(buffer);
    if(Status >= 0){
        v3 = (UINT64*)qword_7CECB1E8;
        if(v3 == NULL){
            DEBUG ((DEBUG_INFO, "#[EB.M.BMF|UK!]\n"));
            return Status;
        }
        if(*qword_7CECB1E8 != buffer){
            while (TRUE) {
                v3 = (UINT64*)*(v3 + 24);
                if(v3 == NULL){
                    DEBUG ((DEBUG_INFO, "#[EB.M.BMF|UK!]\n"));
                    return Status;
                }
                if(*v3 == (UINT64)buffer){
                    break;
                }
            }
        }
        v3 = NULL;
        UINT64* v6 = (UINT64*)&qword_7CECB1E8;
        if(v3){
            v6 = v3 + 24;
        }
        *v6 = v3[3];
        v3[3] = (UINT64)qword_7CECB1F8;
        qword_7CECB1F8 = v3;
    }
    return Status;
}

const char * sub_7CE2D034(StringStruct2* a1)
{
    StringStruct1*fist_StringStruct1; // rbx
    const char *v2; // rsi
    
    fist_StringStruct1 = a1->next0;
    
    v2 = "(null)";
    if (fist_StringStruct1 )
    {
      //  while ( (unsigned int)sub_7CE43246(*v1, "name") ) strcmp
        while (strcmp(fist_StringStruct1->string1, "name") != 0)
        {
            fist_StringStruct1 = fist_StringStruct1->next3;
            if ( fist_StringStruct1 == NULL)
                return v2;
        }
        return fist_StringStruct1->string3;
    }
    return v2;
}
// memset
UINT64 sub_7CE295B3(void* buffer,UINT64 byteSize){
    UINT64 result = 0;
    memset(buffer, result, byteSize);
    return result;
}

UINT64 sub_7CE295B0(void* buffer,UINT64 size){
    return sub_7CE295B3(buffer,size);
}

// strlen
UINT64 sub_7CE4322A(const char* a1)
{
    return strlen(a1);
}

void sub_7CE2CBA4(StringStruct2 *a1, char *a2, UINT64 a3, char a4, char a5){
    
}

StringStruct2* sub_7CE2CD29(StringStruct2 *a1, char* a2)
{
    StringStruct2* buffer = NULL;
    void** v5 = NULL;
    StringStruct2* v6 = NULL;
    StringStruct2 *v9 = NULL;
    if(qword_7CEC9FB0){
        v5 = &qword_7CEC9FB0->next2;
        v6 = qword_7CEC9FB0->next2;
    }else{
        buffer = sub_7CE382B1(4096);
        if(buffer == NULL){
            return NULL;
        }
        sub_7CE295B0(buffer, 4096LL);
        buffer->next2 = qword_7CEC9FB8;
        qword_7CEC9FB8 = buffer;
        buffer->next1 = buffer;
        v9 = ++buffer;
        char count = 127;
        StringStruct2* v11 = qword_7CEC9FB0;
        StringStruct2* lastStringStruct2 = NULL;
        while (count-- == 0) {
            v6 = v11;
            v9->next2 = v11;
            v11 = v9;
            v9++;
        }
        v5 = &v9->next2;
        qword_7CEC9FB0 = v9--;
    }
    qword_7CEC9FB0 = v6;
    if(a1){
        *v5 = a1->next1;
        a1->next1 = v9;
    }else{
        qword_7CEC9FC0 = v9;
        v9->next2 = NULL;
    }
    dword_7CEC9FC8++;
    UINT64 strlen = sub_7CE4322A(a2);
    
    return v9;
}

StringStruct2* sub_7CE2D07F(char *a1, char a2)
{
    EFI_STATUS  Status = 0;
    StringStruct2* v2 = 0;
    char *v5; // rax
    char *v6; // rdx
    char v7; // cl
    StringStruct2* i;
    const char* v11;
    char v13[32];
    memset(v13,0xAA,32);
    v2 = qword_7CEC9FC0;
    if(qword_7CEC9FC0 == 0){
        
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
        
        for ( i = v2->next1; i; i = v2->next2 )
        {
            v11 = sub_7CE2D034(i);
            if(strcmp(v11, v13) == 0){
                goto LABEL_17;
            }
        }
        if(a2 == 0){
            return NULL;
        }
        i = sub_7CE2CD29(v2, v13);
    LABEL_17:
        v2 = i;
        if ( !i )
          return 0LL;
    }
    return i;
}

EFI_STATUS sub_7CE14DC7(unsigned int a1){
    EFI_STATUS  Status = 0;
    
    UINT64 v11[10];
    
    APPLE_DEBUG_LOG_PROTOCOL  *Protocol = NULL;
    
    sub_7CE42D1F();
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
            BOOLEAN v2 = sub_7CE090CB();
            UINT64* v3 = &unk_7CE9F378;
            if(!v2){
                v3 = &unk_7CE9F370;
            }
            byte_7CECB342 = v2;
            qword_7CECB348 = *v3;
            if(qword_7CECB348 >= 4){
                if(Protocol){
                    byte_7CECB340 = 1;
                    if(v2){
                        APPLE_DEBUG_LOG_SETUP_FILES SetupFiles = Protocol->SetupFiles;
                        SetupFiles();
                    }
                }
                
            }
            if(qword_7CE9F380 > 1 || (qword_7CE9F380 == 1 && Protocol == NULL)){
                byte_7CECB333 = 1;
            }
            if(qword_7CE9F388){
                sub_7CE42D1F();
                if(qword_7CE9F388 > 3 || (qword_7CE9F388 == 1 && Protocol == NULL)){
                    byte_7CECB331 = 1;
                }
            }
            if(qword_7CEC8390){
                byte_7CECB332 = 1;
            }
        }
            break;
        case 1:{
            byte_7CECB333 = 1;
        }
            break;
        case 7:{
            byte_7CECB341 = 1;
        }
            break;
        case 9:{
            if(qword_7CE9F388 >= 2){
                byte_7CECB331 = 1;
            }
            sub_7CE3DD1D(9);
        LABEL_29:
            if(qword_7CECB348 >=3 ){
            LABEL_30:
                if(Protocol){
                    if(byte_7CECB342){
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
                    byte_7CECB341 = 1;
                }else if(a1 == 9){
                    if(qword_7CECD078){
                        sub_7CE38327(qword_7CECD078);
                        qword_7CECD078 = NULL;
                    }
                    Status = 0LL;
                    byte_7CECB332 = 0;
                    byte_7CECB333 = 0;
                    Protocol = NULL;
                    qword_7CEC8398 = 0LL;
                }
            }else{
            LABEL_35:
                if ( qword_7CECD078 )
                {
                    Status = sub_7CE38327(qword_7CECD078);
                    qword_7CECD078 = 0LL;
                }
                byte_7CECB332 = 0;
            }
        }
            break;
        case 0xAu:{
            if ( qword_7CE9F388 )
            {
                sub_7CE42E18();
                byte_7CECB331 = 1;
            }
            sub_7CE3DD1D(a1);
        }
            break;
        default:
            sub_7CE3DD1D(a1);
            unsigned int newA1 = a1 - 2;
            switch (newA1) {
                case 2:
                case 4:
                    if(qword_7CECB348 == 0){
                        goto LABEL_34;
                    }
                    goto LABEL_30;
                    break;
                case 3:
                    goto LABEL_29;
                case 5:
                    if(qword_7CECB348 >= 2){
                        if(Protocol){
                            if(byte_7CECB342){
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
                case 8:
                    if(qword_7CEC8390 == 1){
                        v11[6] = 0xAAAAAAAA;
                        v11[5] = 0xAAAAAAAAAAAAAAAA;
                        int v4 = 1;
                        sub_7CE2D07F("/efi/debug-log", v4);
                    }
                default:
                    break;
            }
            break;
            
    }
    return Status;
}

EFI_STATUS
EFIAPI
UefiMain (
          IN EFI_HANDLE        ImageHandle,
          IN EFI_SYSTEM_TABLE  *SystemTable
          )
{
    
    EFI_STATUS                       Status = 0;
    
    /*
     UINT64 a1 = 0xAAAAAAAAAAAAAAAA;
     UINT64 a2 = 0xAAAAAAAAAAAAAAAA;
     UINT64 a3 = 0xAAAAAAAAAAAAAAAA;
     
     
     int a4 = 0;
     */
    
    mImageHandle  = ImageHandle;
    mSystemTable = SystemTable;
    sub_7CE43C58(ImageHandle,SystemTable);
    sub_7CE0F68E();
    sub_7CE14DC7(0);
    
    
    
    DEBUG ((DEBUG_INFO, "This is a test boot.efi!!!\n"));
    
    return Status;
}
