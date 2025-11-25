// #include <ntddk.h>

#include <ntifs.h>

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


	UNICODE_STRING unicode_buffer_source = { 0 };
	UNICODE_STRING unicode_buffer_target = { 0 };

	// 字符串转为数字
	ULONG number;
	RtlInitUnicodeString(&unicode_buffer_source, L"100");
	NTSTATUS flag = RtlUnicodeStringToInteger(&unicode_buffer_source, 10, &number);

	if (NT_SUCCESS(flag))
	{
		DbgPrint("[*] 字符串 -> 数字：%d \n", number);
	}

	// 数字转字符串
	unicode_buffer_target.Buffer = (PWSTR)ExAllocatePool(PagedPool, 1024);
	unicode_buffer_target.MaximumLength = 1024;

	flag = RtlIntegerToUnicodeString(number, 10, &unicode_buffer_target);
	if (NT_SUCCESS(flag))
	{
		DbgPrint("[*] 数字 -> 字符串：%wZ \n", unicode_buffer_target);
	}

	
	{
		RtlFreeUnicodeString(&unicode_buffer_target);
	}

	pDriver->DriverUnload = UnloadDriver;
	return STATUS_SUCCESS;
}
