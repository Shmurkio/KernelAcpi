#include "Shmurkio.hpp"

extern "C"
VOID
DriverUnload(
    [[maybe_unused]] _In_ PDRIVER_OBJECT DriverObject
)
{
    DbgPrintEx(0, 0, "Driver unloaded!\n");
}

template<typename T>
NTSTATUS
GetAcpiTablePhysical(
    _In_ PACPI_XSDT Xsdt,
    _In_ uint32_t Signature,
    _Out_ T& TablePhysical
)
{
    if (!Xsdt || !Signature)
    {
        TablePhysical = 0;
        return STATUS_INVALID_PARAMETER;
    }

    uint32_t Entries = (Xsdt->Header.Length - sizeof(ACPI_DESCRIPTION_HEADER)) / sizeof(uint64_t);

    for (uint32_t i = 0; i < Entries; i++)
    {
        void* Entry = Xsdt->TablePointer[i];
        ACPI_DESCRIPTION_HEADER Header = Util::ReadPhysicalMemory<ACPI_DESCRIPTION_HEADER>(Entry);

        if (Header.Signature == Signature)
        {
            TablePhysical = reinterpret_cast<T>(Entry);
            return STATUS_SUCCESS;
        }
    }

    TablePhysical = 0;
    return STATUS_NOT_FOUND;
}

extern "C"
NTSTATUS
DriverEntry(
    [[maybe_unused]] _In_ PDRIVER_OBJECT DriverObject,
    [[maybe_unused]] _In_ PUNICODE_STRING RegistryPath
)
{
    DriverObject->DriverUnload = DriverUnload;
    
    //
    // We need ntoskrnl base to find HalpAcpiGetCachedTable.
    //
    PLDR_DATA_TABLE_ENTRY Kernel = nullptr;
    NTSTATUS Status = Util::GetModuleByName("ntoskrnl.exe", Kernel);

    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "ntoskrnl.exe not found\n");
        return Status;
    }

    DbgPrintEx(0, 0, "ntoskrnl.exe: 0x%p\n", Kernel);

    //
    // Sig-scan for HalpAcpiGetCachedTable.
    //
    HalpAcpiGetCachedTable_t HalpAcpiGetCachedTable = nullptr;
    Status = Util::FindPattern(Kernel->DllBase, Kernel->SizeOfImage, "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 48 89 78 ? 41 54 41 56 41 57 48 83 EC 20 48 8B 3D", HalpAcpiGetCachedTable);

    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "HalpAcpiGetCachedTable not found\n");
        return Status;
    }

    DbgPrintEx(0, 0, "HalpAcpiGetCachedTable: 0x%p\n", HalpAcpiGetCachedTable);

    //
    // Now we can use HalpAcpiGetCachedTable to retrieve the cached XSDT table.
    //
    PACPI_XSDT Xsdt = reinterpret_cast<PACPI_XSDT>(HalpAcpiGetCachedTable(XSDT_SIG, nullptr, nullptr));

    if (!Xsdt)
    {
        DbgPrintEx(0, 0, "XSDT not not found\n");
        return STATUS_NOT_FOUND;
    }

    DbgPrintEx(0, 0, "XSDT: 0x%p\n", Xsdt);

    //
    // Use the XSDT table to retrieve the physical address of any ACPI table.
    //
    void* Tpm2 = nullptr;
    Status = GetAcpiTablePhysical(Xsdt, TPM2_SIG, Tpm2);

    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "TPM2 not not found\n");
        return Status;
    }

    DbgPrintEx(0, 0, "TPM2: 0x%p\n", Tpm2);

    return Status;
}