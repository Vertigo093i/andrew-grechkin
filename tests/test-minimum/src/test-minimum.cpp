﻿#include <cstdint>
#include <windows.h>

#include <stdio.h>
#include <string>
#include <stdint.h>
#include <functional>
#include <memory>
#include <vector>

inline void * operator new(size_t size) noexcept {
	return ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

inline void * operator new [](size_t size) noexcept {
	return ::operator new(size);
}

inline void operator delete(void * in) noexcept {
	::HeapFree(::GetProcessHeap(), 0, (PVOID)in);
}

inline void operator delete [](void * ptr) noexcept {
	::operator delete(ptr);
}

template<typename CharType>
struct CharTraits
{
};

template<>
struct CharTraits<char>
{
	typedef char char_type;
	typedef ssize_t int_type;

	static void assign(char_type & r, char_type a);

	static char_type * assign(char_type * p, std::size_t count, char_type a);

	static constexpr bool eq(char_type a, char_type b);

	static constexpr bool lt(char_type a, char_type b);

	static char_type * move(char_type * dest, const char_type * src, std::size_t count);

	static char_type * copy(char_type * dest, const char_type * src, std::size_t count);

	static int compare(const char_type * lhs, const char_type * rhs, std::size_t count);

	static std::size_t length(const char_type * s);

	static const char_type * find(const char_type * p, std::size_t count, char_type a);

	static constexpr char_type to_char_type(int_type a);

	static constexpr char_type to_int_type(char_type a);

	static constexpr bool eq_int_type(int_type a, int_type b);

	static constexpr int_type eof();

	static constexpr bool not_eof(int_type a);
};

template<typename CharTraitsType>
class String
{

};

int wWmain() {
//	std::string a;
//	a += "a";

	typedef std::pair<char, int> datapair;
	typedef std::vector<datapair> vec;

	std::unique_ptr<int[]> ptr1(new int[5]);
	std::auto_ptr<int> ptr2(new int);
	vec v;
	v.reserve(128);
	vec::iterator it = v.begin();

	v.push_back(vec::value_type(1, 2));
	v.emplace_back(2, 3);
	v.emplace_back(3, 4);
	v.size();
	WCHAR qqq[256];
	_snwprintf(qqq, 256, L"size: %Iu, capacity: %Iu\n", v.size(), v.capacity());
	::MessageBoxW(nullptr, qqq, L"test", MB_OK);
	return 0;
}

///=================================================================================================
namespace {

	typedef void (*FAtExit)(void);

	const int64_t MAX_ATEXITLIST_ENTRIES = 8;

	int64_t atexit_index = MAX_ATEXITLIST_ENTRIES - 1;
	FAtExit pf_atexitlist[MAX_ATEXITLIST_ENTRIES];

	void init_atexit()
	{
	}

	void invoke_atexit()
	{

		if (atexit_index < 0)
			atexit_index = 0;
		else
			++atexit_index;

		for (int64_t i = atexit_index; i < MAX_ATEXITLIST_ENTRIES; ++i)
		{
			(*pf_atexitlist[i])();
		}
	}

}

/// ========================================================================== Startup (entry point)
extern "C" {

	int	mainCRTStartup() {
//	int	WinMainCRTStartup() {
		init_atexit();
		int Result = 0;
//		STARTUPINFO StartupInfo = {sizeof(STARTUPINFO), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//		::GetStartupInfo(&StartupInfo);
//
//		Result = wWinMain(::GetModuleHandle(nullptr), nullptr, ::GetCommandLine(),
//						  StartupInfo.dwFlags & STARTF_USESHOWWINDOW ? StartupInfo.wShowWindow : SW_SHOWDEFAULT);
		Result = wWmain();
		invoke_atexit();
		::ExitProcess(Result);
		return	Result;
	}

//	BOOL WINAPI	DllMainCRTStartup(HANDLE, DWORD dwReason, PVOID) {
//		switch (dwReason) {
//			case DLL_PROCESS_ATTACH:
//				init_atexit();
//				break;
//
//			case DLL_PROCESS_DETACH:
//				invoke_atexit();
//				break;
//		}
//		return true;
//	}

	int atexit(FAtExit pf)
	{
		int64_t ind = ::InterlockedExchangeAdd64(&atexit_index, -1);
		if (ind >= 0)
		{
			pf_atexitlist[ind] = pf;
			return 0;
		}
		return -1;
	}

	void __cxa_pure_virtual(void)
	{
//		::abort_message("pure virtual method called");
	}

}
