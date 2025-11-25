// #include <ntddk.h>

#include <ntifs.h>

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriver)
{
	DbgPrint("Bye, Driver\n");

	return STATUS_SUCCESS;
}

void StringCopyDemo()
{
	UNICODE_STRING dst;
	WCHAR dst_buf[256] = { 0 };
	NTSTATUS status;

	// 初始化字符串
	UNICODE_STRING src = RTL_CONSTANT_STRING(L"Hello World!");

	// 字符串初始化为空，长度为256
	RtlInitEmptyUnicodeString(&dst, dst_buf, 256 * sizeof(WCHAR));

	if (&dst)
	{
		RtlCopyUnicodeString(&dst, &src);
	}

	// 在 dst 之后追加
	status = RtlAppendUnicodeToString(&dst, L"KernelTest");
	if (status == STATUS_SUCCESS)
	{
		DbgPrint("[*] 输出操作后的字符串：%wZ", &dst);
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrint("Hello World, Driver\n");
	DbgPrint("PDRIVER_OBJECT-> [0x%08x]\n", pDriver);
	DbgPrint("PUNICODE_STRING->[%ws]\n", pReg->Buffer);

	UNICODE_STRING unicode_buffer = { 0 };
	UNICODE_STRING unicode_buffer_dst = { 0 };
	wchar_t* wchar_string = L"Hello World! Kernel Test";

	// 初始化并分配
	RtlInitUnicodeString(&unicode_buffer_dst, L"Hello World! Kernel Test");

	// 设置最大长度
	unicode_buffer.MaximumLength = 1024;

	// 分配内存空间
	unicode_buffer.Buffer = (PWSTR)ExAllocatePool(PagedPool, 1024);

	// 设置字符长度，由于是宽字符，所以要*2
	unicode_buffer.Length = wcslen(wchar_string) * 2;

	// 保证缓冲区足够大，否则程序终止
	ASSERT(unicode_buffer.MaximumLength >= unicode_buffer.Length);

	// 将 wchar_string 中的字符串拷贝到 uncode_buffer.Buffer
	RtlCopyMemory(unicode_buffer.Buffer, wchar_string, unicode_buffer.Length);

	// 设置字符串长度，并打印
	unicode_buffer.Length = wcslen(wchar_string) * 2;
	DbgPrint("[*] 输出字符串：%wZ \n", unicode_buffer);

	// 比较字符串是否相等
	if (RtlEqualUnicodeString(&unicode_buffer, &unicode_buffer_dst, TRUE))
	{
		DbgPrint("[*] 字符串相等");
	}
	else
	{
		DbgPrint("[*] 字符串不相等");
	}

	// 释放堆空间
	ExFreePool(unicode_buffer.Buffer);

	unicode_buffer.Buffer = NULL;
	unicode_buffer.MaximumLength = 0;
	unicode_buffer.Length = 0;

	pDriver->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
