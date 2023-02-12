#pragma once
#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#include <vector>
using namespace std;
namespace memory_tool {

	class memory_manager {
	public:
		HANDLE mem_handle = nullptr;
		int pid;
		std::uintptr_t client_dll = 0x0;

		memory_manager(std::string_view name) {
			bind_process(name);
			bind_modules("client.dll");
		}

		void bind_process(std::string_view name);
		void bind_modules(std::string_view name);
	

		template<typename T>
		T read(uintptr_t address) {
			T t;
			ReadProcessMemory(mem_handle, reinterpret_cast<LPVOID>(address), &t, sizeof(T), nullptr);
			return t;
		}

		template<typename T>
		void read_list(uintptr_t address, T* t, SIZE_T count) {
			ReadProcessMemory(mem_handle, reinterpret_cast<LPVOID>(address), t, count * (sizeof (T)), false);
		}

		template<typename T>
		void read_chain(uintptr_t address, vector<uintptr_t>vec, T& out) {
			for (int i = 0; i < vec.size(); i++) {
				if (static_cast<unsigned long long>(i) + 1 == vec.size()) {
					out = read<T>(address + vec[i]);
				}
				else {
					address = read<int>(address + vec[i]);
				}
			}
		}
	};

	


	
}
