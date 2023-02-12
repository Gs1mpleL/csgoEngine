#include "memory.h"
//Converting a WChar string to a Ansi string
std::string WChar2Ansi(LPCWSTR pwszSrc) {
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

void memory_tool::memory_manager::bind_process(std::string_view name)
{
	if (name.empty()) {
		throw std::exception("进程名为空");
	}
	HANDLE _handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);  // 获取所有进程
	PROCESSENTRY32 pro;
	pro.dwSize = sizeof pro;
	for (Process32First(_handle, &pro); Process32Next(_handle, &pro);) {
		if (name.compare(pro.szExeFile)==0) {
			pid = pro.th32ProcessID;
			mem_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
			break;
		}
	}
	if (pid==0) {
		throw std::exception("pid为空");
	}
}

void memory_tool::memory_manager::bind_modules(std::string_view name)
{
	HANDLE _handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	MODULEENTRY32 mod;
	mod.dwSize = sizeof mod;
	for (Module32First(_handle, &mod); Module32Next(_handle, &mod);) {
		if (name.compare(mod.szModule) == 0)
		{
			client_dll = (uintptr_t)mod.modBaseAddr;
			break;
		}
	}
	if (client_dll == 0x0) {
		throw std::exception("client_dll基址为空");
	}
}


