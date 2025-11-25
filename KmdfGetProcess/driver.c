#include <ntifs.h>
#include <windef.h>

extern PVOID PsGetProcessPeb(_In_ PEPROCESS Process); // 函数返回给定进程的进程环境块（Process Environment Block，PEB）的指针
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS* Process); // 此函数通过给定的进程 ID 查找对应的进程，并将进程的 EPROCESS 结构指针存储在提供的指针变量中。
extern NTKERNELAPI PVOID PsGetProcessWow64Process(_In_ PEPROCESS Process); // 此函数返回给定进程的 Wow64 进程环境的指针。Wow64 进程环境是用于在 64 位 Windows 上运行 32 位应用程序的兼容性层。
extern NTKERNELAPI UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process); // 此函数返回给定进程的映像文件名，即执行该进程的可执行文件的名称。
extern NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process); //  此函数返回给定进程所继承的唯一进程 ID,即父进程ID。

typedef struct
{
	DWORD Pid;
	UCHAR ProcessName[2048];
	DWORD Handle;
	LIST_ENTRY ListEntry;
} ProcessList;

// 根据进程ID返回进程EPROCESS结构体失败返回NULL
PEPROCESS LookupProcess(HANDLE Pid)
{
	PEPROCESS eprocess = NULL;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = PsLookupProcessByProcessId(Pid, &eprocess);
	if (NT_SUCCESS(Status))
	{
		return eprocess;
	}
	return NULL;
}

// 内核链表操作
BOOLEAN GetAllProcess()
{
	PEPROCESS eproc = NULL;
	LIST_ENTRY linkListHead;

	// 初始化链表头部
	InitializeListHead(&linkListHead);
	ProcessList* pData = NULL;

	for (int temp = 0; temp < 100000; temp += 4) // pid 一定是4的整数倍
	{
		eproc = LookupProcess((HANDLE)temp);
		if (eproc != NULL)
		{
			STRING nowProcessnameString = { 0 };
			RtlInitString(&nowProcessnameString, PsGetProcessImageFileName(eproc));

			// 分配内核堆空间
			pData = (ProcessList*)ExAllocatePool(PagedPool, sizeof(ProcessList));
			RtlZeroMemory(pData, sizeof(ProcessList));

			// 设置变量
			pData->Pid = (DWORD)PsGetProcessId(eproc);
			RtlCopyMemory(pData->ProcessName, PsGetProcessImageFileName(eproc), strlen(PsGetProcessImageFileName(eproc)));
			pData->Handle = (DWORD)PsGetProcessInheritedFromUniqueProcessId(eproc);

			// 插入元素
			InsertTailList(&linkListHead, &pData->ListEntry);
			ObDereferenceObject(eproc); // 内核对象引用计数+1
		}
	}

	// 输出链表内的数据
	while (!IsListEmpty(&linkListHead))
	{
		LIST_ENTRY* pEntry = RemoveHeadList(&linkListHead);
		pData = CONTAINING_RECORD(pEntry, ProcessList, ListEntry);

		DbgPrint("Pid[%d], ProcessName[%s], Handle[0x%x] \n", pData->Pid, pData->ProcessName, pData->Handle);
		ExFreePool(pData);
	}
	return TRUE;
}

// 绕过签名检查
BOOLEAN BypassCheckSign(PDRIVER_OBJECT pDriverObject)
{
#ifdef _WIN64
	typedef struct _KLDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY listEntry;
		ULONG64 __Undefined1;
		ULONG64 __Undefined2;
		ULONG64 __Undefined3;
		ULONG64 NonPagedDebugInfo;
		ULONG64 DllBase;
		ULONG64 EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING path;
		UNICODE_STRING name;
		ULONG   Flags;
		USHORT  LoadCount;
		USHORT  __Undefined5;
		ULONG64 __Undefined6;
		ULONG   CheckSum;
		ULONG   __padding1;
		ULONG   TimeDateStamp;
		ULONG   __padding2;
	} KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;
#else
	typedef struct _KLDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY listEntry;
		ULONG unknown1;
		ULONG unknown2;
		ULONG unknown3;
		ULONG unknown4;
		ULONG unknown5;
		ULONG unknown6;
		ULONG unknown7;
		UNICODE_STRING path;
		UNICODE_STRING name;
		ULONG   Flags;
	} KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;
#endif

	PKLDR_DATA_TABLE_ENTRY pLdrData = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	pLdrData->Flags = pLdrData->Flags | 0x20;

	return TRUE;
}



// 卸载驱动
void UnloadDriver(PDRIVER_OBJECT driver)
{
	DbgPrint("Uninstall Driver Is Ok \n");
}

// 驱动入口地址
NTSTATUS DriverEntry(IN PDRIVER_OBJECT Driver, PUNICODE_STRING RegistryPath)
{

	if (BypassCheckSign(Driver))
		DbgPrint("Bypass Sign Success.");


	DbgPrint("Driver loaded. \n");

	GetAllProcess();

	Driver->DriverUnload = UnloadDriver;
	return STATUS_SUCCESS;

}