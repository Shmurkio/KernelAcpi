#pragma warning(disable : 4201)
#pragma warning(disable : 4083)
#pragma warning(disable : 4005)
#pragma warning(disable : 4200)

#pragma once

#include <ntddk.h>
#include <cstdint>

#include "Util.hpp"

typedef
void*(
    __fastcall* HalpAcpiGetCachedTable_t
)
(
    _In_ uint32_t Signature,
    _In_opt_ const char* OemId,
    _In_opt_ const char* OemTableId
);

#pragma pack(push, 1)
typedef struct _ACPI_DESCRIPTION_HEADER
{
    uint32_t Signature;
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    uint8_t OemId[6];
    uint64_t OemTableId;
    uint32_t OemRevision;
    uint32_t CreatorId;
    uint32_t CreatorRevision;
} ACPI_DESCRIPTION_HEADER, *PACPI_DESCRIPTION_HEADER;

typedef struct _ACPI_XSDT
{
    ACPI_DESCRIPTION_HEADER Header;
    PACPI_DESCRIPTION_HEADER TablePointer[];
} ACPI_XSDT, *PACPI_XSDT;
#pragma pack(pop)

#define ACPI_SIG(a, b, c, d) ((d << 24) | (c << 16) | (b << 8) | a)

#define RSDT_SIG ACPI_SIG('R', 'S', 'D', 'T')  // Root System Description Table
#define XSDT_SIG ACPI_SIG('X', 'S', 'D', 'T')  // Extended System Description Table
#define FADT_SIG ACPI_SIG('F', 'A', 'C', 'P')  // Fixed ACPI Description Table
#define FACS_SIG ACPI_SIG('F', 'A', 'C', 'S')  // Firmware ACPI Control Structure
#define DSDT_SIG ACPI_SIG('D', 'S', 'D', 'T')  // Differentiated System Description Table
#define SSDT_SIG ACPI_SIG('S', 'S', 'D', 'T')  // Secondary System Description Table
#define APIC_SIG ACPI_SIG('A', 'P', 'I', 'C')  // Multiple APIC Description Table
#define HPET_SIG ACPI_SIG('H', 'P', 'E', 'T')  // High Precision Event Timer Table
#define MCFG_SIG ACPI_SIG('M', 'C', 'F', 'G')  // Memory Mapped Configuration Space Base Address Description Table
#define SRAT_SIG ACPI_SIG('S', 'R', 'A', 'T')  // System Resource Affinity Table
#define SLIT_SIG ACPI_SIG('S', 'L', 'I', 'T')  // System Locality Information Table
#define ECDT_SIG ACPI_SIG('E', 'C', 'D', 'T')  // Embedded Controller Boot Resources Table
#define BERT_SIG ACPI_SIG('B', 'E', 'R', 'T')  // Boot Error Record Table
#define EINJ_SIG ACPI_SIG('E', 'I', 'N', 'J')  // Error Injection Table
#define ERST_SIG ACPI_SIG('E', 'R', 'S', 'T')  // Error Record Serialization Table
#define HEST_SIG ACPI_SIG('H', 'E', 'S', 'T')  // Hardware Error Source Table
#define AML_SIG  ACPI_SIG('A', 'M', 'L', ' ')  // AML code block

#define TPM2_SIG ACPI_SIG('T', 'P', 'M', '2')  // Trusted Platform Module 2 Table
#define NFIT_SIG ACPI_SIG('N', 'F', 'I', 'T')  // NVDIMM Firmware Interface Table
#define PCCT_SIG ACPI_SIG('P', 'C', 'C', 'T')  // Platform Communications Channel Table
#define PMTT_SIG ACPI_SIG('P', 'M', 'T', 'T')  // Platform Memory Topology Table
#define SDEV_SIG ACPI_SIG('S', 'D', 'E', 'V')  // Secure Devices Table
#define IORT_SIG ACPI_SIG('I', 'O', 'R', 'T')  // I/O Remapping Table
#define DMAR_SIG ACPI_SIG('D', 'M', 'A', 'R')  // DMA Remapping Table
#define IVRS_SIG ACPI_SIG('I', 'V', 'R', 'S')  // I/O Virtualization Reporting Structure