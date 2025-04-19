# KernelAcpi

On system boot, the kernel caches ACPI tables and stores them in a linked list known as `HalpAcpiTableCacheList`. To retrieve a specific ACPI table, the function `HalpAcpiGetCachedTable` can be used. This function traverses the cache list and matches entries based on the table **Signature**, **OEM ID**, and **OEM Table ID**.

## `HalpAcpiGetCachedTable` Function

```cpp
__int64 __fastcall HalpAcpiGetCachedTable(int Signature, const char *OemId, const char *OemTableId)
{
  __int64 Current; // rdi
  __int64 Result; // rbx

  Current = HalpAcpiTableCacheList;
  Result = 0i64;
  while ( (__int64 *)Current != &HalpAcpiTableCacheList )
  {
    if ( *(_DWORD *)(Current + 24) == Signature
      && (!OemId || !strncmp(OemId, (const char *)(Current + 34), 6ui64))
      && (!OemTableId || !strncmp(OemTableId, (const char *)(Current + 40), 8ui64)) )
    {
      return Current + 24;
    }
    Current = *(_QWORD *)Current;
  }
  return Result;
}
```

Once the XSDT is obtained using this function, its entries can be traversed to locate specific tables of interest.

## Example

![Retrieving TPM2](https://i.imgur.com/lgByDsP.png)
