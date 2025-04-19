#include "Util.hpp"

NTSTATUS
Util::GetModuleByName(
    _In_ const char* ModuleName,
    _Out_ PLDR_DATA_TABLE_ENTRY& Module
)
{
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;

    RtlInitAnsiString(&AnsiString, ModuleName);

    NTSTATUS Status = RtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, TRUE);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    PLIST_ENTRY List = PsLoadedModuleList;
    PLIST_ENTRY Current = List->Flink;

    while (Current != List)
    {
        PLDR_DATA_TABLE_ENTRY Entry = CONTAINING_RECORD(Current, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        if (RtlCompareUnicodeString(&UnicodeString, &Entry->BaseDllName, TRUE) == 0)
        {
            RtlFreeUnicodeString(&UnicodeString);
            Module = Entry;
            return STATUS_SUCCESS;
        }

        Current = Current->Flink;
    }

    RtlFreeUnicodeString(&UnicodeString);
    Module = nullptr;
    return STATUS_NOT_FOUND;
}

uint32_t
Util::SwitchEndianness32(
    uint32_t Value
)
{
    return ((Value >> 24) & 0x000000FF) |
        ((Value >> 8) & 0x0000FF00) |
        ((Value << 8) & 0x00FF0000) |
        ((Value << 24) & 0xFF000000);
}

uint16_t
Util::SwitchEndianness16(
    uint16_t Value
)
{
    return (Value >> 8) | (Value << 8);
}

uint64_t
Util::SwitchEndianness64(
    uint64_t Value
)
{
    return ((Value >> 56) & 0x00000000000000FF) |
        ((Value >> 40) & 0x000000000000FF00) |
        ((Value >> 24) & 0x0000000000FF0000) |
        ((Value >> 8) & 0x00000000FF000000) |
        ((Value << 8) & 0x000000FF00000000) |
        ((Value << 24) & 0x0000FF0000000000) |
        ((Value << 40) & 0x00FF000000000000) |
        ((Value << 56) & 0xFF00000000000000);
}