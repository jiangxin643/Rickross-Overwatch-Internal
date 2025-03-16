#pragma once

typedef float matrix_t[4][4];

bool OW_Init();
void OW_InitX(float a, matrix_t& m);
void OW_InitY(float a, matrix_t& m);
void OW_Transform(Vector vec, matrix_t m, Vector* out);

extern bool s_ow_init;

typedef enum _MEMORY_INFORMATION_CLASS {
  MemoryBasicInformation,
  MemoryWorkingSetList,
  MemorySectionName,
  MemoryBasicVlmInformation,
  MemoryWorkingSetExList
} MEMORY_INFORMATION_CLASS;

typedef union _PSAPI_WORKING_SET_EX_BLOCK {
  ULONG_PTR Flags;
  union {
    struct {
      ULONG_PTR Valid : 1;
      ULONG_PTR ShareCount : 3;
      ULONG_PTR Win32Protection : 11;
      ULONG_PTR Shared : 1;
      ULONG_PTR Node : 6;
      ULONG_PTR Locked : 1;
      ULONG_PTR LargePage : 1;
      ULONG_PTR Reserved : 7;
      ULONG_PTR Bad : 1;
      ULONG_PTR ReservedUlong : 32;
    };
    struct {
      ULONG_PTR Valid : 1;
      ULONG_PTR Reserved0 : 14;
      ULONG_PTR Shared : 1;
      ULONG_PTR Reserved1 : 15;
      ULONG_PTR Bad : 1;
      ULONG_PTR ReservedUlong : 32;
    } Invalid;
  };
} PSAPI_WORKING_SET_EX_BLOCK, *PPSAPI_WORKING_SET_EX_BLOCK;

typedef struct _PSAPI_WORKING_SET_EX_INFORMATION {
  PVOID                      VirtualAddress;
  PSAPI_WORKING_SET_EX_BLOCK VirtualAttributes;
} PSAPI_WORKING_SET_EX_INFORMATION, *PPSAPI_WORKING_SET_EX_INFORMATION;

typedef struct _THREAD_BASIC_INFORMATION {
  NTSTATUS                ExitStatus;
  PVOID                   TebBaseAddress;
  CLIENT_ID               ClientId;
  KAFFINITY               AffinityMask;
  KPRIORITY               Priority;
  KPRIORITY               BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef NTSTATUS (*NTAPI NtOpenThread_fn)(PHANDLE ThreadHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
typedef NTSTATUS (*NTAPI NtClose_fn)(HANDLE Handle); // doesn't work
typedef NTSTATUS (*NTAPI NtQueryInformationThread_fn)(HANDLE ThreadHandle, LONG ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
typedef NTSTATUS (*NTAPI NtQuerySystemInformation_fn)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef NTSTATUS (*NTAPI NtReadVirtualMemory_fn)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
typedef NTSTATUS (*NTAPI NtWriteVirtualMemory_fn)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);
typedef NTSTATUS (*NTAPI NtProtectVirtualMemory_fn)(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
typedef NTSTATUS (*NTAPI NtQueryVirtualMemory_fn)(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
typedef NTSTATUS (*NTAPI NtContinue_fn)(PCONTEXT ThreadContext, BOOLEAN RaiseAlert);

extern NtOpenThread_fn sys_NtOpenThread;
extern NtQueryInformationThread_fn sys_NtQueryInformationThread;
extern NtQuerySystemInformation_fn sys_NtQuerySystemInformation;
extern NtReadVirtualMemory_fn sys_NtReadVirtualMemory;
extern NtWriteVirtualMemory_fn sys_NtWriteVirtualMemory;
extern NtProtectVirtualMemory_fn sys_NtProtectVirtualMemory;
extern NtQueryVirtualMemory_fn sys_NtQueryVirtualMemory;
extern NtContinue_fn sys_NtContinue;