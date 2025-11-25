// #include <ntddk.h>

#include <ntifs.h>

#if 0 
typedef struct _LIST_ENTRY
{
	struct _LIST_ENTRY* Flink;
	struct _LIST_ENTRY* Blink;
}LIST_ENTRY, * PLIST_ENTRY;
#endif

typedef struct _MyStruct
{
	ULONG x;
	ULONG y;
	LIST_ENTRY lpListEntry;
}MyStruct, * PMyStruct;

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriver)
{
	DbgPrint("Bye, Driver\n");

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrint("Hello World, Driver\n");
	DbgPrint("PDRIVER_OBJECT-> [0x%016x]\n", pDriver);
	DbgPrint("PUNICODE_STRING->[%ws]\n", pReg->Buffer);

	// 初始化头节点
	LIST_ENTRY ListHeader = { 0 };
	InitializeListHead(&ListHeader);

	// 定义链表元素
	MyStruct testA = { 0 };
	MyStruct testB = { 0 };
	MyStruct testC = { 0 };

	testA.x = 10;
	testA.y = 20;

	testB.x = 100;
	testB.y = 200;

	testC.x = 1000;
	testC.y = 2000;

	// 分别插入节点到头部和尾部
	InsertHeadList(&ListHeader, &testA.lpListEntry);
	InsertTailList(&ListHeader, &testB.lpListEntry);
	InsertTailList(&ListHeader, &testC.lpListEntry);

	// 输出链表数据
	PLIST_ENTRY pListEntry = NULL;
	pListEntry = ListHeader.Flink;

	int i = 0;

	while (pListEntry != &ListHeader)
	{
		// 计算出成员距离结构体顶部内存距离
		PMyStruct ptr = CONTAINING_RECORD(pListEntry, MyStruct, lpListEntry);
		DbgPrint("节点元素x = %d, 节点元素y = %d\n", ptr->x, ptr->y);

		if (i++ > 10)
		{
			break;
		}
		// 获取下一个链表地址
		pListEntry = pListEntry->Flink;
	}

	pDriver->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
