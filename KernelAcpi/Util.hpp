#pragma warning(disable : 4201)
#pragma warning(disable : 4083)
#pragma warning(disable : 4005)

#pragma once

#include <ntddk.h>
#include <cstdint>

extern "C" PLIST_ENTRY PsLoadedModuleList;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY HashLinks;
        struct
        {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union
    {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

#define IN_RANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x) (IN_RANGE((x&(~0x20)),'A','F')?((x&(~0x20))-'A'+0xA):(IN_RANGE(x,'0','9')?x-'0':0))
#define GET_BYTE(a, b) (GET_BITS(a) << 4 | GET_BITS(b))

namespace Util
{
    template<typename T>
    NTSTATUS FindPattern(void* BaseAddress, uint64_t Size, const char* Pattern, T& Out)
    {
        uint8_t* FirstMatch = nullptr;
        const char* CurrentPattern = Pattern;
        uint8_t* Start = static_cast<uint8_t*>(BaseAddress);
        uint8_t* End = Start + Size;

        for (uint8_t* Current = Start; Current < End; ++Current)
        {
            uint8_t Byte = CurrentPattern[0];

            if (!Byte)
            {
                Out = reinterpret_cast<T>(FirstMatch);
                return STATUS_SUCCESS;
            }

            if (Byte == '\?' || *Current == GET_BYTE(Byte, CurrentPattern[1]))
            {
                if (!FirstMatch)
                    FirstMatch = Current;

                if (!CurrentPattern[2])
                {
                    Out = reinterpret_cast<T>(FirstMatch);
                    return STATUS_SUCCESS;
                }

                if (Byte == '\?')
                    CurrentPattern += 2;
                else
                    CurrentPattern += 3;
            }
            else
            {
                CurrentPattern = Pattern;
                FirstMatch = nullptr;
            }
        }

        Out = 0ULL;
        return STATUS_NOT_FOUND;
    }

    NTSTATUS
    GetModuleByName(
        _In_ const char* ModuleName,
        _Out_ PLDR_DATA_TABLE_ENTRY& Module
    );

    template<typename T>
    T
    ReadPhysicalMemory(
        _In_ void* Address
    )
    {
        PHYSICAL_ADDRESS PhysicalAddress;
        PhysicalAddress.QuadPart = reinterpret_cast<LONGLONG>(Address);

        T Value{};
        size_t Size = sizeof(T);

        void* Mapped = MmMapIoSpace(PhysicalAddress, Size, MmNonCached);

        if (Mapped)
        {
            Value = *reinterpret_cast<T*>(Mapped);
            MmUnmapIoSpace(Mapped, Size);
        }

        return Value;
    }

    uint16_t
    SwitchEndianness16(
        uint16_t Value
    );

    uint32_t
    SwitchEndianness32(
        uint32_t Value
    );

    uint64_t
    SwitchEndianness64(
        uint64_t Value
    );
}