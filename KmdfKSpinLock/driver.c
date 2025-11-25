// #include <ntddk.h>

#include <ntifs.h>

typedef struct _MyStruct
{
	ULONG x;
	ULONG y;
	LIST_ENTRY lpListEntry;
}MyStruct, * PMyStruct;

// 定义全局链表和全局锁
LIST_ENTRY g_ListHeader;
KSPIN_LOCK g_kSpinLock;

void Init()
{
	InitializeListHead(&g_ListHeader);
	KeInitializeSpinLock(&g_kSpinLock);
}

void function_ins()
{
	KIRQL Irql;

	// 加锁
	KeAcquireSpinLock(&g_kSpinLock, &Irql);

	DbgPrint("[*] 锁内部执行 \n");

	KeReleaseSpinLock(&g_kSpinLock, Irql);
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriver)
{
	DbgPrint("Bye, Driver\n");

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrint("Hello World, Driver\n");
	DbgPrint("PDRIVER_OBJECT-> [0x%08x]\n", pDriver);
	DbgPrint("PUNICODE_STRING->[%ws]\n", pReg->Buffer);

	// 初始化链表
	Init();

	// 分配链表空间
	PMyStruct pStuA = (PMyStruct)ExAllocatePool(NonPagedPoolExecute, sizeof(PMyStruct));
	PMyStruct pStuB = (PMyStruct)ExAllocatePool(NonPagedPoolExecute, sizeof(PMyStruct));

	// 赋值
	if (pStuA)
	{
		pStuA->x = 10;
		pStuA->y = 20;
	}

	if (pStuB)
	{
		pStuB->x = 100;
		pStuB->y = 200;
	}

	if (pStuA && pStuB)
	{
		ExInterlockedInsertHeadList(&g_ListHeader, (PLIST_ENTRY)&pStuA->lpListEntry, &g_kSpinLock);
		ExInterlockedInsertTailList(&g_ListHeader, (PLIST_ENTRY)&pStuB->lpListEntry, &g_kSpinLock);
	}

	function_ins();

	int i = 0;

	// 移除节点A并放入到 remove_entry 中
	PLIST_ENTRY pRemoveEntry = ExInterlockedRemoveHeadList(&pStuA->lpListEntry, &g_kSpinLock);

	while (pRemoveEntry != &g_ListHeader)
	{
		// 计算出成员距离结构体顶部内存距离
		PMyStruct ptr = CONTAINING_RECORD(pRemoveEntry, MyStruct, lpListEntry);
		DbgPrint("[*] 节点元素x = %d, 节点元素y = %d\n", ptr->x, ptr->y);

		if (i++ > 10)
		{
			break;
		}
		// 获取下一个链表地址
		pRemoveEntry = pRemoveEntry->Flink;
	}

	pDriver->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
