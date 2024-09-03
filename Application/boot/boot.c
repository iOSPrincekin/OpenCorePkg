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

void boot_func_7CE43F47(void *GuidHob)
{
    GuidHob_24 = GuidHob;
}

EFI_STATUS boot_func_7CE43C58(
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
    boot_func_7CE43F47(GuidHob + 3);
    
    
    
    return Status;
    
}

void* AllocatePool_malloc(UINTN bufferSize)
{
    void* bufferPtr = NULL; // [rsp+20h] [rbp-10h] BYREF
    EFI_ALLOCATE_POOL AllocatePool = mBootServices->AllocatePool;
    AllocatePool(4LL, bufferSize, &bufferPtr);
    return bufferPtr;
}

void boot_func_7CE1E528(){
    
}

void* boot_func_7CE1E2B1(UINTN bufferSize)
{
    void* buffer = NULL;
    buffer = NULL;
    /*
    UINTN *v1; // rax
     */
    buffer = AllocatePool_malloc(bufferSize);
    return buffer;
}

void *qword_7CEA0E30 = NULL;

EFI_STATUS boot_func_7CE0F68E()
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

BOOLEAN boot_func_7CE090CB(){
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


UINT8 byte_7CEA2340 = 0;

UINT8 byte_7CEA2342 = 0;

UINT8 byte_7CEA2333 = 0;


UINT64 qword_7CEA2348 = 0;

UINT64 qword_7CE9F380 = 0;

UINT64 qword_7CE9F388 = 1;

UINT64 qword_7CEA2360 = 4261634048LL;

UINT64* qword_7CECB358 = 0;

UINT32* qword_7CECB360 = 0;

UINT32* addr_FE03401C = (UINT32*)0xFE03401C;


void boot_func_7CE42E8D()
{
    
}

void boot_func_7CE42F03()
{
    
}

void boot_func_7CE42F1C()
{
    
}

void boot_func_7CE42F2F()
{
    
}

void boot_func_7CE42F41()
{
    
}

void boot_func_7CE42F5E()
{
    
}

void *off_7CEC8A40[6] =
{
    (void*)boot_func_7CE42E8D,
    (void*)boot_func_7CE42F03,
    (void*)boot_func_7CE42F1C,
    (void*)boot_func_7CE42F2F,
    (void*)boot_func_7CE42F41,
    (void*)boot_func_7CE42F5E
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



EFI_STATUS boot_func_7CE42D1F(){
    
    DEBUG ((DEBUG_INFO, "This is a test boot.efi!!!, boot_func_7CE42D1F \n"));
    
    UINT8 data = 0x5A;
    UINT16 port = 0x3FF;
    out8(port,data);
    int v = 0;
    in8(port,v);
    
    
    if(v == data){
        
    }
    
    UINT8 newV = write_read_addree(0xFE03401C, data);
    
    DEBUG ((DEBUG_INFO,"asm volatile , boot_func_7CE42D1F,%d,%d\n",v,newV));

    
    EFI_STATUS Status = 0;
    UINT32* ptr1 = addr_FE03401C;
    UINT32* ptr2 = addr_FE03401C + 0x1FE4;
    qword_7CECB360 = ptr2;
    UINT32* ptr3 = addr_FE03401C + 0x2000;
    UINT32 V_5A = 0x5A;
    UINT32 V_A5 = 0xA5;
    
    *ptr3 = V_5A;
    if(*ptr3 == V_5A){
        *ptr3 = V_A5;
        if(*ptr3 == V_A5){
            qword_7CECB358 = (UINT64*)off_7CEC8A40;
            Status = 1;
            return Status;
        }
    }else{
        UINT32* ptr4 = addr_FE03401C - 0x1C;
        qword_7CECB360 = ptr4;
        *ptr1 = V_5A;
        if(*ptr1 == V_5A){
            *ptr1 = V_A5;
            if(*ptr1 == V_A5){
                qword_7CECB358 = (UINT64*)off_7CEC8A40;
                Status = 1;
                return Status;
            }
        }

        
        
    }

    return Status;
}

EFI_STATUS boot_func_7CE14DC7(unsigned int a1){
    EFI_STATUS  Status = 0;
    
    
    APPLE_DEBUG_LOG_PROTOCOL  *Protocol = NULL;

    boot_func_7CE42D1F();
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
            BOOLEAN v2 = boot_func_7CE090CB();
            UINT64* v3 = &unk_7CE9F378;
            if(!v2){
                v3 = &unk_7CE9F370;
            }
            byte_7CEA2342 = v2;
            qword_7CEA2348 = *v3;
            if(qword_7CEA2348 >= 4){
                if(Protocol){
                    byte_7CEA2340 = 1;
                    if(v2){
                        APPLE_DEBUG_LOG_SETUP_FILES SetupFiles = Protocol->SetupFiles;
                        SetupFiles();
                    }
                }
                if(qword_7CE9F380 > 1 || (qword_7CE9F380 == 1 && Protocol == NULL)){
                    byte_7CEA2333 = 1;
                }
                if(qword_7CE9F388){
                    boot_func_7CE42D1F();
                }
            }
        }
            break;
            
        default:
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
    boot_func_7CE43C58(ImageHandle,SystemTable);
    boot_func_7CE0F68E();
    boot_func_7CE14DC7(0);
    
    
    
    DEBUG ((DEBUG_INFO, "This is a test boot.efi!!!\n"));
    DEBUG ((DEBUG_INFO, "This is a test boot.efi2!!!\n"));

    return Status;
}
