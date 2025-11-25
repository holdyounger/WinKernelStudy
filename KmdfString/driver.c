// #include <ntddk.h>

#include <ntifs.h>

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriver)
{
	DbgPrint("Bye, Driver\n");

	return STATUS_SUCCESS;
}

KIRQL WPOFFx64()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	UINT64 cr0 = __readcr0();
	cr0 &= 0xfffffffffffeffff;
	__writecr0(cr0);
	_disable();
	return irql;
}

void WPONx64(KIRQL irql)
{
	UINT64 cr0 = __readcr0();
	cr0 |= 0x10000;
	_enable();
	__writecr0(cr0);
	KeLowerIrql(irql);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrint("Hello World, Driver\n");
	DbgPrint("PDRIVER_OBJECT-> [0x%08x]\n", pDriver);
	DbgPrint("PUNICODE_STRING->[%ws]\n", pReg->Buffer);


	// 定义内核字符串
	ANSI_STRING ansi;
	ANSI_STRING ansiDst = {0}; // 最好赋个初值
	UNICODE_STRING unicode;
	UNICODE_STRING str;

	// 定义普通字符串
	char* chr = "Hello Kernel Test";
	wchar_t* wchr = (WCHAR*)L"Hello Kernel Test";

	// 初始化字符串，有多种方式可以选择
	RtlInitAnsiString(&ansi, chr);
	RtlInitUnicodeString(&unicode, wchr);
	RtlInitUnicodeString(&str, L"Hello Kernel Test");

#if 0
	// 创建 MDL
	PMDL mdl = IoAllocateMdl(chr, strlen(chr) + 1, FALSE, FALSE, NULL);
	if (!mdl) {
		DbgPrint("Failed to allocate MDL.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// 锁定内存页
	MmBuildMdlForNonPagedPool(mdl);

	// 映射 MDL 到系统地址空间
	PVOID baseAddress = MmGetSystemAddressForMdlSafe(mdl, MdlMappingNoExecute);
	if (!baseAddress) {
		DbgPrint("Failed to map MDL to system address space.\n");
		IoFreeMdl(mdl);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// 修改内存保护属性为可读写
	NTSTATUS status = MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);
	if (!NT_SUCCESS(status)) {
		DbgPrint("Failed to change memory protection: %x\n", status);
		IoFreeMdl(mdl);
		return status;
	}
#endif

	// 改变原始字符串，看下效果，这里应该是不能修改的，编译后位于常量区，要修改必须先修改内存属性才可以。否则会报错
	KIRQL irql = WPOFFx64();
	// 修改字符串内容
	chr[0] = 'h'; // 将 'H' 改为 'h'
	chr[6] = 'k'; // 将 'K' 改为 'k'

	WPONx64(irql);

	// 打印
	DbgPrint("[*] 输出ANSI: %Z \n", &ansi);
	DbgPrint("[*] 输出WCHAR: %Z \n", &unicode);
	DbgPrint("[*] 输出字符串: %wZ \n", &str);

	NTSTATUS flag = RtlUnicodeStringToAnsiString(&ansiDst, &unicode, TRUE);
	if (NT_SUCCESS(flag))
	{
		DbgPrint("[*] RtlUnicodeStringToAnsiString Called Print: %Z", &ansiDst);
	}

	pDriver->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
