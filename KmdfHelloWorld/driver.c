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

	pDriver->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
