// #include <ntddk.h>

#include <ntifs.h>

typedef struct _MyStruct
{
	ULONG x;
	ULONG y;
}MyStruct, *PMyStruct;

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

	// 1 用于分配与释放非标签内存
	PVOID buffer = ExAllocatePool(NonPagedPool, 1024);

	DbgPrint("[*] 分配内存地址=%p \n", buffer);

	if (buffer)
	{
		ExFreePool(buffer);
	}

	// 用于分配带有标签的内存
	PMyStruct pStu = (PMyStruct)ExAllocatePoolWithTag(NonPagedPoolExecute, sizeof(PMyStruct), "KernelTest");

	if (pStu)
	{
		pStu->x = 100;
		pStu->y = 200;

		DbgPrint("[*] 分配内存 x = %d, y= %d", pStu->x, pStu->y);

		ExFreePoolWithTag(pStu, "KernelTest");
	}

	UNICODE_STRING dst = { 0 };
	UNICODE_STRING src = RTL_CONSTANT_STRING(L"Hello KernelTest");

	dst.Buffer = (PWCHAR)ExAllocatePool(NonPagedPool, src.Length);
	if (dst.Buffer == NULL)
	{
		DbgPrint("[-] 分配空间错误");
	}

	dst.Length = dst.MaximumLength = src.Length;
	RtlCopyUnicodeString(&dst, &src);

	DbgPrint("[*] 输出拷贝 = %wZ", dst);

	pDriver->DriverUnload = UnloadDriver;
	return STATUS_SUCCESS;
}
