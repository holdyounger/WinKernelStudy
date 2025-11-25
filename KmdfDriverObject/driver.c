// #include <ntddk.h>

#include <ntifs.h>

//
// Loader Data Table Entry
//
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
        PVOID SectionPointer;
    };
    ULONG CheckSum;
    union
    {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriver)
{
	DbgPrint("Bye, Driver\n");

	return STATUS_SUCCESS;
}

void PrintCurDirverInfo(PDRIVER_OBJECT pDriver)
{
	DbgPrint("[*] 驱动名：%wZ", pDriver->DriverName);

	DbgPrint("[*] 驱动起始地址：%p, 大小：%x, 结束地址: %p", pDriver->DriverStart, pDriver->DriverSize, (ULONG64)pDriver->DriverStart + pDriver->DriverSize);

	DbgPrint("[*] 卸载地址: %p", pDriver->DriverUnload);
	DbgPrint("[*] IRP_MJ_READ: %p", pDriver->MajorFunction[IRP_MJ_READ]);
	DbgPrint("[*] IRP_MJ_WRITE: %p", pDriver->MajorFunction[IRP_MJ_WRITE]);
	DbgPrint("[*] IRP_MJ_CREATE: %p", pDriver->MajorFunction[IRP_MJ_CREATE]);
	DbgPrint("[*] IRP_MJ_CLOSE: %p", pDriver->MajorFunction[IRP_MJ_CLOSE]);
	DbgPrint("[*] IRP_MJ_DEVICE_CONTROL: %p", pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL]);

	// 输出完整的调用号
	for (auto i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DbgPrint("IRP_MJ 调用号：%d, 函数地址：%p", i, pDriver->MajorFunction[i]);
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrint("Hello World, Driver\n");
	DbgPrint("PDRIVER_OBJECT-> [0x%08x]\n", pDriver);
	DbgPrint("PUNICODE_STRING->[%ws]\n", pReg->Buffer);

	pDriver->DriverUnload = UnloadDriver;

	// PrintCurDirverInfo(pDriver);

    PLDR_DATA_TABLE_ENTRY pldr = NULL;
    PLIST_ENTRY pListEntry = NULL;
    PLIST_ENTRY pCurListEntry = NULL;

    PLDR_DATA_TABLE_ENTRY pCurDriver = NULL;
    pldr = (PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
    pListEntry = pldr->InLoadOrderLinks.Flink;
    pCurListEntry = pListEntry->Flink;

    // 遍历
    while (pCurListEntry != pListEntry)
    {
        // 获取 LDR_DATA_TABLE_ENTRY 结构
        pCurDriver = CONTAINING_RECORD(pCurListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

        if (pCurDriver->BaseDllName.Buffer != 0)
        {
            DbgPrint("[*] 模块名: %wZ, 模块基址: %p, 模块入口: %p, 模块时间戳: %d",
                pCurDriver->BaseDllName,
                pCurDriver->DllBase,
                pCurDriver->EntryPoint,
                pCurDriver->TimeDateStamp);
        }

        pCurListEntry = pCurListEntry->Flink;
    }


	return STATUS_SUCCESS;
}
