﻿/** std.hpp
 *	@author		© 2010 Andrew Grechkin
 **/
#ifndef WIN_STD_HPP
#define WIN_STD_HPP

#if (_WIN32_WINNT < 0x0501)
#undef _WIN32_WINNT
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#if (WINVER < 0x0501)
#undef WINVER
#endif
#ifndef WINVER
#define WINVER 0x0501
#endif

#if (_WIN32_IE < 0x0600)
#undef _WIN32_IE
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include <stdint.h>
#include <windows.h>

#ifdef NoStdNew
inline void* operator new(size_t size) {
	return ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}
inline void* operator new[](size_t size) {
	return ::operator new(size);
}
inline void operator delete(void *in) {
	::HeapFree(::GetProcessHeap(), 0, (PVOID)in);
}
inline void operator delete[](void *ptr) {
	::operator delete(ptr);
}
#endif

#include <algorithm>

#ifdef	__x86_64__
#define nullptr 0ll
#else
#define nullptr 0
#endif

PCWSTR const EMPTY_STR = L"";
PCWSTR const PATH_SEPARATOR = L"\\"; // Path separator in the file system
PCWSTR const PATH_SEPARATORS = L"\\/";
PCWSTR const SPACE = L" ";
PCWSTR const PATH_PREFIX_NT = L"\\\\?\\";  // Prefix to put ahead of a long path for Windows API
PCWSTR const NET_PREFIX = L"\\\\";

const WCHAR PATH_SEPARATOR_C = L'\\';
const WCHAR STR_END = L'\0';
const WCHAR SPACE_C = L' ';

#define NORM_M_PREFIX(m)	(*(LPDWORD)m==0x5c005c)
#define REV_M_PREFIX(m)		(*(LPDWORD)m==0x2f002f)

#define BOM_UTF32le			0x0000FEFF
#define BOM_UTF32be			0xFFFE0000
#define BOM_UTF16le			0xFEFF
#define BOM_UTF16be			0xFFFE
#define BOM_UTF8			0xBFBBEF00

const size_t MAX_PATH_LEN = 32772;
const UINT CP_UTF16le = 1200;
const UINT CP_UTF16be = 1201;
const UINT CP_UTF32le = 1202;
const UINT CP_UTF32be = 1203;
const UINT CP_AUTODETECT = (UINT)-1;
const UINT DEFAULT_CP = CP_UTF8;

#define NTSIGNATURE(a) ((LPVOID)((BYTE *)(a) + ((PIMAGE_DOS_HEADER)(a))->e_lfanew))

#define DEFINE_FUNC(name) F##name name
#define GET_DLL_FUNC(name) name = (F##name)get_function(#name)
#define GET_DLL_FUNC_NT(name) name = (F##name)get_function_nt(#name)

//#define THIS_FILE ((strrchr(__FILE__, '\\') ?: __FILE__ - 1) + 1)
#define THIS_FILE only_file(__FILE__)

#define HighLow64(high, low) (((uint64_t)(high) << 32) | (low))
#define HighPart64(arg64) ((DWORD)((arg64) >> 32))
#define LowPart64(arg64) ((DWORD)((arg64) & 0xFFFFFFFF))

#ifndef sizeofa
#define sizeofa(array)		(sizeof(array)/sizeof(0[array]))
#endif

#ifndef sizeofe
#define sizeofe(array)		(sizeof(0[array]))
#endif

#ifndef S_IXUSR
#define S_IFDIR 0x4000
#define S_IRUSR 0x0100
#define S_IWUSR 0x0080
#define S_IXUSR 0x0040
#define S_IRGRP 0x0020
#define S_IWGRP 0x0010
#define S_IXGRP 0x0008
#define S_IROTH 0x0004
#define S_IWOTH 0x0002
#define S_IXOTH 0x0001
#endif

inline PCSTR only_file(PCSTR path) {
	return (strrchr((path), L'\\') ?: (path) - 1) + 1;
}

inline PCWSTR only_file(PCWSTR path) {
	return (wcsrchr((path), L'\\') ?: (path) - 1) + 1;
}

typedef const void *PCVOID;

extern "C" {
	long long __MINGW_NOTHROW	wcstoll(const wchar_t * __restrict__, wchar_t** __restrict__, int);
	unsigned long long __MINGW_NOTHROW wcstoull(const wchar_t * __restrict__, wchar_t ** __restrict__, int);
}

///====================================================================================== Uncopyable
/// Базовый класс для private наследования классами, объекты которых не должны копироваться
class Uncopyable {
protected:
	~Uncopyable() {
	}
	Uncopyable() {
	}
private:
	Uncopyable(const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);
};

///========================================================================================= Command
/// Паттерн Command
struct CommandPattern {
	virtual ~CommandPattern() {
	}
	virtual bool Execute() const = 0;
};

struct NullCommand: public CommandPattern {
	virtual bool Execute() const {
		return true;
	}
};

///========================================================================================== WinMem
/// Функции работы с кучей
namespace WinMem {
	template<typename Type>
	inline bool Alloc(Type &in, size_t size, DWORD flags = HEAP_ZERO_MEMORY) {
		in = static_cast<Type> (::HeapAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | flags, size));
		return in != nullptr;
	}
	inline PVOID Alloc(size_t size, DWORD flags = HEAP_ZERO_MEMORY) {
		return ::HeapAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | flags, size);
	}

	template<typename Type>
	inline bool Realloc(Type &in, size_t size, DWORD flags = HEAP_ZERO_MEMORY) {
		if (in)
			in = (Type)::HeapReAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | flags, (PVOID)in, size);
		else
			in = (Type)::HeapAlloc(::GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | flags, size);
		return in != nullptr;
	}

	template<typename Type>
	inline void Free(Type &in) {
		if (in) {
			::HeapFree(::GetProcessHeap(), 0, (PVOID)in);
			in = nullptr;
		}
	}

	inline size_t Size(PCVOID in) {
		return (in) ? ::HeapSize(::GetProcessHeap(), 0, in) : 0;
	}

	inline bool Cmp(PCVOID m1, PCVOID m2, size_t size) {
		return ::memcmp(m1, m2, size) == 0;
	}

	inline PVOID Copy(PVOID dest, PCVOID sour, size_t size) {
		return ::memcpy(dest, sour, size);
	}
	inline PVOID Fill(PVOID in, size_t size, char fill) {
		return ::memset(in, (int)fill, size);
	}
	inline void Zero(PVOID in, size_t size) {
		Fill(in, size, 0);
	}
	template<typename Type>
	inline void Fill(Type &in, char fill) {
		Fill(&in, sizeof(in), fill);
	}
	template<typename Type>
	inline void Zero(Type &in) {
		Fill(&in, sizeof(in), 0);
	}
}

template<typename Type>
inline Type ReverseBytes(Type &in) {
	std::reverse((char*)&in, ((char*)&in) + sizeof(Type));
}

inline void XchgByte(WORD &inout) {
	inout = inout >> 8 | inout << 8;
}

inline void XchgWord(DWORD &inout) {
	inout = inout >> 16 | inout << 16;
}

inline intmax_t Mega2Bytes(size_t in) {
	return (in != 0) ? (intmax_t)in << 20 : -1ll;
}

inline size_t Bytes2Mega(intmax_t in) {
	return (in > 0) ? in >> 20 : 0;
}

///▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒ constraints
template <typename Type>
struct must_be_pointer {
	static bool constraints(const Type &type_is_not_pointer) {
		return sizeof(0[type_is_not_pointer]);
	}
};

template <>
struct must_be_pointer<PVOID> {
	static bool constraints(const PVOID &) {
		return true;
	}
};

///======================================================================================== auto_buf
template<typename Type>
class auto_buf {
	typedef auto_buf<Type> this_type;
	typedef Type value_type;
	typedef size_t size_type;

public:
	~auto_buf() {
		must_be_pointer<Type>::constraints(m_ptr);
		WinMem::Free(m_ptr);
	}

	auto_buf():
		m_ptr(nullptr) {
	}

	explicit auto_buf(size_type size):
		m_ptr((value_type)WinMem::Alloc(size, 0)) {
	}

	auto_buf(const this_type & rhs):
		m_ptr(nullptr) {
		swap((this_type&)rhs);
	}

	this_type & operator =(const this_type & rhs) {
		if (this != &rhs)
			this_type(rhs).swap(*this);
		return *this;
	}

	void reserve(size_type nsize) {
		if (size() < nsize) {
			WinMem::Realloc(m_ptr, nsize);
		}
	}

	size_type size() const {
		return WinMem::Size(m_ptr);
	}

	value_type operator &() const {
		return m_ptr;
	}

	value_type operator ->() const {
		return m_ptr;
	}

	operator value_type() const {
		return m_ptr;
	}

	value_type data() const {
		return m_ptr;
	}

	bool operator !() const {
		return m_ptr;
	}

	void attach(value_type & ptr) {
		WinMem::Free(m_ptr);
		m_ptr = ptr;
	}

	void detach(value_type & ptr) {
		ptr = m_ptr;
		m_ptr = nullptr;
	}

	void swap(value_type & ptr) {
		using std::swap;
		swap(m_ptr, ptr);
	}

	void swap(this_type & rhs) {
		using std::swap;
		swap(m_ptr, rhs.m_ptr);
	}

private:
	value_type m_ptr;
};

template<typename Type>
void swap(auto_buf<Type> & b1, auto_buf<Type> & b2) {
	b1.swap(b2);
}

///======================================================================================== auto_buf
template<typename Type>
class auto_array {
	typedef auto_array<Type> this_type;
	typedef Type value_type;
	typedef Type * pointer_type;
	typedef size_t size_type;

public:
	~auto_array() {
		WinMem::Free(m_ptr);
	}

	explicit auto_array(size_type size):
		m_ptr((pointer_type)WinMem::Alloc(size * sizeof(Type), 0)),
		m_size(size) {
	}

	auto_array(const this_type & rhs):
		m_ptr(nullptr),
		m_size(0) {
		swap((this_type&)rhs);
	}

	this_type & operator =(const this_type & rhs) {
		if (this != &rhs)
			this_type(rhs).swap(*this);
		return *this;
	}

	void reserve(size_type nsize) {
		if (size() < nsize) {
			WinMem::Realloc(m_ptr, nsize * sizeof(Type));
			m_size = nsize;
		}
	}

	size_type size() const {
		return m_size;
	}

	operator pointer_type() const {
		return m_ptr;
	}

	pointer_type data() const {
		return m_ptr;
	}

	value_type & operator [](int ind) {
		return m_ptr[ind];
	}

	const value_type & operator [](int ind) const {
		return m_ptr[ind];
	}

	void swap(this_type & rhs) {
		using std::swap;
		swap(m_ptr, rhs.m_ptr);
		swap(m_size, rhs.m_size);
	}

private:
	pointer_type m_ptr;
	size_type m_size;
};


template<typename Type>
void swap(auto_array<Type> & b1, auto_array<Type> & b2) {
	b1.swap(b2);
}

///====================================================================================== auto_close
template<typename Type>
class auto_close: private Uncopyable {
	typedef auto_close<Type> this_type;
	typedef Type value_type;

public:
	~auto_close() {
		delete m_impl;
	}

	template<typename Deleter>
	explicit auto_close(value_type ptr, Deleter del) :
		m_impl(new auto_close_deleter_impl<Deleter>(ptr, del)) {
	}

	operator value_type() {
		return m_impl->m_ptr;
	}

	value_type * operator &() {
		return &(m_impl->m_ptr);
	}

private:
	struct auto_close_impl {
		auto_close_impl(value_type ptr):
			m_ptr(ptr) {
		}
		virtual ~auto_close_impl() {
		}
		value_type m_ptr;
	};

	template <typename Deleter>
	struct auto_close_deleter_impl: public auto_close_impl {
		auto_close_deleter_impl(value_type ptr, Deleter d):
			auto_close_impl(ptr),
			m_deleter(d) {
		}
		virtual ~auto_close_deleter_impl() {
			m_deleter(this->m_ptr);
		}
		Deleter m_deleter;
	};

	auto_close_impl * m_impl;
};

template<>
struct auto_close<HANDLE>: private Uncopyable {
	typedef HANDLE value_type;
	typedef auto_close<value_type> this_type;

	~auto_close() {
		close();
	}

	explicit auto_close(value_type ptr = nullptr) :
		m_ptr(ptr) {
	}

	value_type * operator &() {
		close();
		return &m_ptr;
	}

	operator value_type() const {
		return m_ptr;
	}

	operator bool() const {
		return m_ptr && m_ptr != INVALID_HANDLE_VALUE;
	}

	void close() {
		if (m_ptr && m_ptr != INVALID_HANDLE_VALUE) {
			::CloseHandle(m_ptr);
			m_ptr = nullptr;
		}
	}

	void swap(this_type & rhs) {
		using std::swap;
		swap(m_ptr, rhs.m_ptr);
	}

private:
	value_type m_ptr;
};

inline void swap(auto_close<HANDLE> &b1, auto_close<HANDLE> &b2) {
	b1.swap(b2);
}

///========================================================================================= WinFlag
/// Проверка и установка битовых флагов
namespace WinFlag {
	template<typename Type1, typename Type2>
	bool Check(Type1 in, Type2 flag) {
		return static_cast<Type1>(flag) == (in & static_cast<Type1>(flag));
	}

	template<typename Type1, typename Type2>
	bool CheckAny(Type1 in, Type2 flag) {
		return in & static_cast<Type1>(flag);
	}

	template<typename Type1, typename Type2>
	Type1 &Set(Type1 &in, Type2 flag) {
		return in |= static_cast<Type1>(flag);
	}

	template<typename Type1, typename Type2>
	Type1 &UnSet(Type1 &in, Type2 flag) {
		return in &= ~static_cast<Type1>(flag);
	}

	template<typename Type1, typename Type2>
	Type1 &Switch(Type1 &in, Type2 flag, bool sw) {
		if (sw)
			return Set(in, flag);
		else
			return UnSet(in, flag);
	}
}

///========================================================================================== WinBit
//template<typename Type>
//struct type_size_bits {
//	size_t value = sizeof(Type) * 8;
//};

/// Проверка и установка битов
namespace WinBit {
	template<typename Type>
	size_t BIT_LIMIT() {
		return sizeof(Type) * 8;
	}

	template<typename Type>
	bool BadBit(size_t in) {
		return !(in < BIT_LIMIT<Type> ());
	}

	template<typename Type>
	size_t Limit(size_t in) {
		return (in == 0) ? BIT_LIMIT<Type> () : std::min<int>(in, BIT_LIMIT<Type> ());
	}

	template<typename Type>
	bool Check(Type in, size_t bit) {
		if (BadBit<Type> (bit))
			return false;
		Type tmp = 1;
		tmp <<= bit;
		return (in & tmp);
	}

	template<typename Type>
	Type &Set(Type &in, size_t bit) {
		if (BadBit<Type> (bit))
			return in;
		Type tmp = 1;
		tmp <<= bit;
		return (in |= tmp);
	}

	template<typename Type>
	Type &UnSet(Type &in, size_t bit) {
		if (BadBit<Type> (bit))
			return in;
		Type tmp = 1;
		tmp <<= bit;
		return (in &= ~tmp);
	}

	template<typename Type>
	Type &Switch(Type &in, size_t bit, bool sw) {
		if (sw)
			return Set(in, bit);
		else
			return UnSet(in, bit);
	}
}

///====================================================================== Функции работы с символами
inline int GetType(WCHAR in) {
	WORD Result[2] = {0};
	::GetStringTypeW(CT_CTYPE1, &in, 1, Result);
	return Result[0];
}
inline bool IsEol(WCHAR in) {
	return in == L'\r' || in == L'\n';
}
inline bool IsSpace(WCHAR in) {
	//	return in == L' ' || in == L'\t';
	return WinFlag::Check(GetType(in), C1_SPACE);
}
inline bool IsPrint(WCHAR in) {
	return !WinFlag::Check(GetType(in), C1_CNTRL);
}
inline bool IsCntrl(WCHAR in) {
	//	return in == L' ' || in == L'\t';
	return WinFlag::Check(GetType(in), C1_CNTRL);
}
inline bool IsUpper(WCHAR in) {
	//	return ::IsCharUpperW(in);
	return WinFlag::Check(GetType(in), C1_UPPER);
}
inline bool IsLower(WCHAR in) {
	//	return ::IsCharLowerW(in);
	return WinFlag::Check(GetType(in), C1_LOWER);
}
inline bool IsAlpha(WCHAR in) {
	//	return ::IsCharAlphaW(in);
	return WinFlag::Check(GetType(in), C1_ALPHA);
}
inline bool IsAlNum(WCHAR in) {
	//	return ::IsCharAlphaW(in);
	int Result = GetType(in);
	return WinFlag::Check(Result, C1_ALPHA) || WinFlag::Check(Result, C1_DIGIT);
}
inline bool IsDigit(WCHAR in) {
	//	return ::IsCharAlphaNumeric(in);
	return WinFlag::Check(GetType(in), C1_DIGIT);
}
inline bool IsXDigit(WCHAR in) {
	//	return ::IsCharAlphaNumeric(in);
	return WinFlag::Check(GetType(in), C1_XDIGIT);
}
inline bool IsPunct(WCHAR in) {
	//	return ::IsCharAlphaNumeric(in);
	return WinFlag::Check(GetType(in), C1_PUNCT);
}

///====================================================================== Функции работы со строками
inline size_t Len(PCSTR in) {
	return ::strlen(in);
}
inline size_t Len(PCWSTR in) {
	return ::wcslen(in);
}

inline bool Empty(PCSTR in) {
	return in[0] == 0;
}
inline bool Empty(PCWSTR in) {
	return in[0] == 0;
}

#ifndef NORM_STOP_ON_NULL
#define NORM_STOP_ON_NULL 0x10000000
#endif
inline int CmpCode(PCSTR in1, PCSTR in2) {
	return ::strcmp(in1, in2);
}
inline int CmpCode(PCSTR in1, PCSTR in2, size_t n) {
	return ::strncmp(in1, in2, n);
}
inline int CmpCode(PCWSTR in1, PCWSTR in2) {
	return ::wcscmp(in1, in2);
	//	return ::wcscoll(in1, in2);
}
inline int CmpCode(PCWSTR in1, PCWSTR in2, size_t n) {
	return ::wcsncmp(in1, in2, n);
}

inline int Cmp(PCSTR in1, PCSTR in2) {
	return ::CompareStringA(0, SORT_STRINGSORT, in1, -1, in2, -1) - CSTR_EQUAL;
}
inline int Cmp(PCSTR in1, PCSTR in2, size_t n) {
	return ::CompareStringA(0, NORM_STOP_ON_NULL | SORT_STRINGSORT, in1, n, in2, n) - CSTR_EQUAL;
}
inline int Cmp(PCWSTR in1, PCWSTR in2) {
	return ::CompareStringW(0 , SORT_STRINGSORT, in1, -1, in2, -1) - CSTR_EQUAL;
}
inline int Cmp(PCWSTR in1, PCWSTR in2, size_t n) {
	return ::CompareStringW(0 , NORM_STOP_ON_NULL | SORT_STRINGSORT, in1, n, in2, n) - CSTR_EQUAL;
}

inline int Cmpi(PCSTR in1, PCSTR in2) {
	//	return ::_stricmp(in1, in2);
	return ::CompareStringA(0, NORM_IGNORECASE | SORT_STRINGSORT, in1, -1, in2, -1) - CSTR_EQUAL;
}
inline int Cmpi(PCSTR in1, PCSTR in2, size_t n) {
	return ::CompareStringA(0, NORM_IGNORECASE | NORM_STOP_ON_NULL | SORT_STRINGSORT, in1, n, in2,
	                        n) - CSTR_EQUAL;
}
inline int Cmpi(PCWSTR in1, PCWSTR in2) {
	//	return ::_wcsicmp(in1, in2);
	//	return ::_wcsicoll(lhs.first.c_str(), rhs.first.c_str()) < 0;
	//	return fsf.LStricmp(lhs.first.c_str(), rhs.first.c_str()) < 0;
	return ::CompareStringW(0, NORM_IGNORECASE | SORT_STRINGSORT, in1, -1, in2, -1) - CSTR_EQUAL;
}
inline int Cmpi(PCWSTR in1, PCWSTR in2, size_t n) {
	return ::CompareStringW(0, NORM_IGNORECASE | NORM_STOP_ON_NULL | SORT_STRINGSORT, in1, n, in2,
	                        n) - CSTR_EQUAL;
}

inline bool EqCode(PCSTR in1, PCSTR in2) {
	return CmpCode(in1, in2) == 0;
}
inline bool EqCode(PCWSTR in1, PCWSTR in2) {
	return CmpCode(in1, in2) == 0;
}

inline bool Eq(PCSTR in1, PCSTR in2) {
	return Cmp(in1, in2) == 0;
}
inline bool Eq(PCWSTR in1, PCWSTR in2) {
	return Cmp(in1, in2) == 0;
}

inline bool Eqi(PCSTR in1, PCSTR in2) {
	return Cmpi(in1, in2) == 0;
}
inline bool Eqi(PCWSTR in1, PCWSTR in2) {
	return Cmpi(in1, in2) == 0;
}

inline PSTR Copy(PSTR dest, PCSTR src) {
	return ::strcpy(dest, src);
}
inline PWSTR Copy(PWSTR dest, PCWSTR src) {
	return ::wcscpy(dest, src);
}
inline PSTR Copy(PSTR dest, PCSTR src, size_t size) {
	return ::strncpy(dest, src, size);
}
inline PWSTR Copy(PWSTR dest, PCWSTR src, size_t size) {
	return ::wcsncpy(dest, src, size);
}

inline PSTR Cat(PSTR dest, PCSTR src) {
	return ::strcat(dest, src);
}
inline PWSTR Cat(PWSTR dest, PCWSTR src) {
	return ::wcscat(dest, src);
}
inline PSTR Cat(PSTR dest, PCSTR src, size_t size) {
	return ::strncat(dest, src, size);
}
inline PWSTR Cat(PWSTR dest, PCWSTR src, size_t size) {
	return ::wcsncat(dest, src, size);
}

inline PSTR Find(PCSTR where, PCSTR what) {
	return ::strstr(where, what);
}
inline PCSTR Find(PCSTR where, CHAR what) {
	return ::strchr(where, what);
}
inline PWSTR Find(PCWSTR where, PCWSTR what) {
	return ::wcsstr(where, what);
}
inline PCWSTR Find(PCWSTR where, WCHAR what) {
	return ::wcschr(where, what);
}

inline PSTR RFind(PCSTR where, PCSTR what) {
	PCSTR last1 = where + Len(where);
	PCSTR last2 = what + Len(what);
	last2 = std::find_end(where, last1, what, last2);
	return (last1 == last2) ? nullptr : const_cast<PSTR>(last2);
}
inline PSTR RFind(PCSTR where, CHAR what) {
	return ::strrchr(where, what);
}
inline PWSTR RFind(PCWSTR where, PCWSTR what) {
	PCWSTR last1 = where + Len(where);
	PCWSTR last2 = what + Len(what);
	last2 = std::find_end(where, last1, what, last2);
	return (last1 == last2) ? nullptr : const_cast<PWSTR>(last2);
}
inline PWSTR RFind(PCWSTR where, WCHAR what) {
	return ::wcsrchr(where, what);
}

inline size_t Span(PCSTR str, PCSTR strCharSet) {
	return ::strcspn(str, strCharSet);
}
inline size_t Span(PCWSTR str, PCWSTR strCharSet) {
	return ::wcscspn(str, strCharSet);
}

inline int64_t AsInt64(PCSTR in) {
	return _atoi64(in);
}
inline unsigned int AsUInt(PCSTR in, int base = 10) {
	PSTR end_ptr;
	return ::strtoul(in, &end_ptr, base);
}
inline int AsInt(PCSTR in, int base = 10) {
	PSTR end_ptr;
	return ::strtol(in, &end_ptr, base);
}
inline double AsDouble(PCSTR in) {
	PSTR end_ptr;
	return ::strtod(in, &end_ptr);
}

inline uint64_t AsUInt64(PCWSTR in, int base = 10) {
	//	return _wtoi64(in);
	PWSTR end_ptr;
	return ::wcstoull(in, &end_ptr, base);
}
inline int64_t AsInt64(PCWSTR in, int base = 10) {
	//	return _wtoi64(in);
	PWSTR end_ptr;
	return ::wcstoll(in, &end_ptr, base);
}
inline unsigned int  AsUInt(PCWSTR in, int base = 10) {
	PWSTR end_ptr;
	return ::wcstoul(in, &end_ptr, base);
}
inline int AsInt(PCWSTR in, int base = 10) {
	PWSTR end_ptr;
	return ::wcstol(in, &end_ptr, base);
}
inline double AsDouble(PCWSTR in) {
	PWSTR end_ptr;
	return ::wcstod(in, &end_ptr);
}

inline PCSTR Num2Str(PSTR str, int64_t num, int base = 10) {
	return ::_i64toa(num, str, base); //lltoa
}

inline PCWSTR Num2Str(PWSTR str, int64_t num, int base = 10) {
	return ::_i64tow(num, str, base); //lltow
}

//inline string	d2a(double in) {
//	CHAR	buf[MAX_PATH];
//	::_gcvt(in, 12, buf);
//	return buf;
//}

inline WCHAR ToUpper(WCHAR in) {
	::CharUpperBuffW(&in, 1);
	return in;
}
inline WCHAR ToLower(WCHAR in) {
	::CharLowerBuffW(&in, 1);
	return in;
}

inline PWSTR ToUpper(PWSTR buf, size_t len) {
	::CharUpperBuffW(buf, len);
	return buf;
}
inline PWSTR ToLower(PWSTR buf, size_t len) {
	::CharLowerBuffW(buf, len);
	return buf;
}

inline PWSTR ToUpper(PWSTR s1) {
	return ToUpper(s1, Len(s1));
}
inline PWSTR ToLower(PWSTR s1) {
	return ToLower(s1, Len(s1));
}

inline PSTR Fill(PSTR in, CHAR ch) {
	return ::_strset(in, ch);
}
inline PWSTR Fill(PWSTR in, WCHAR ch) {
	return ::_wcsset(in, ch);
}

inline PSTR Reverse(PSTR in) {
	return ::_strrev(in);
}
inline PWSTR Reverse(PWSTR in) {
	return ::_wcsrev(in);
}

inline PWSTR AssignStr(PCWSTR src) {
	size_t len = Len(src) + 1;
	PWSTR dest;
	WinMem::Alloc(dest, len * sizeof(WCHAR));
	Copy(dest, src, len);
	return dest;
}

inline size_t Convert(PCSTR from, UINT cp, PWSTR to = nullptr, size_t size = 0) {
	return ::MultiByteToWideChar(cp, 0, from, -1, to, (int)size);
}

inline size_t Convert(PCWSTR from, UINT cp, PSTR to = nullptr, size_t size = 0) {
	return ::WideCharToMultiByte(cp, 0, from, -1, to, (int)size, nullptr, nullptr);
}

#ifdef NoStlString
#include "autostr.h"
#else
#include "autoutf.h"
#endif

inline size_t Len(const astring &in) {
	return in.size();
}
inline size_t Len(const ustring &in) {
	return in.size();
}

inline astring Num2StrA(int64_t num, int base = 10) {
	CHAR buf[64];
	Num2Str(buf, num, base);
	return astring(buf);
}

inline ustring Num2Str(int64_t num, int base = 10) {
	WCHAR buf[64];
	Num2Str(buf, num, base);
	return ustring(buf);
}

inline astring oem(PCWSTR in) {
	return w2cp(in, CP_OEMCP);
}
inline astring oem(const ustring &in) {
	return w2cp(in.c_str(), CP_OEMCP);
}

inline astring ansi(PCWSTR in) {
	return w2cp(in, CP_ACP);
}
inline astring ansi(const ustring &in) {
	return w2cp(in.c_str(), CP_ACP);
}

inline astring utf8(PCWSTR in) {
	return w2cp(in, CP_UTF8);
}
inline astring utf8(const ustring &in) {
	return w2cp(in.c_str(), CP_UTF8);
}

inline ustring utf16(PCSTR in, UINT cp = CP_UTF8) {
	return cp2w(in, cp);
}
inline ustring utf16(const astring &in, UINT cp = CP_UTF8) {
	return cp2w(in.c_str(), cp);
}

astring& Trim_l(astring &str, const astring &chrs = " \t\r\n");

astring& Trim_r(astring &str, const astring &chrs = " \t\r\n");

astring& Trim(astring &str, const astring &chrs = " \t\r\n");

astring TrimOut(const astring &str, const astring &chrs = " \t\r\n");

ustring& Trim_l(ustring &str, const ustring &chrs = L" \t\r\n");

ustring& Trim_r(ustring &str, const ustring &chrs = L" \t\r\n");

ustring& Trim(ustring &str, const ustring &chrs = L" \t\r\n");

ustring TrimOut(const ustring &str, const ustring &chrs = L" \t\r\n");

ustring GetWord(const ustring &str, WCHAR d = PATH_SEPARATOR_C);

astring& AddWord(astring &inout, const astring &add, const astring &delim = "");

ustring& AddWord(ustring &inout, const ustring &add, const ustring &delim = L"");

astring& AddWordEx(astring &inout, const astring &add, const astring &delim = "");

ustring& AddWordEx(ustring &inout, const ustring &add, const ustring &delim = L"");

astring CutWord(astring &inout, const astring &delim = "\t ", bool delDelim = true);

ustring CutWord(ustring &inout, const ustring &delim = L"\t ", bool delDelim = true);

astring CutWordEx(astring &inout, const astring &delim, bool delDelim = true);

ustring CutWordEx(ustring &inout, const ustring &delim, bool delDelim = true);

ustring& ReplaceAll(ustring& str, const ustring &from, const ustring &to);

ustring ReplaceAllOut(const ustring& str, const ustring &from, const ustring &to);

inline void mbox(PCSTR text, PCSTR capt = "") {
	::MessageBoxA(nullptr, text, capt, MB_OK);
}

inline void mbox(PCWSTR text, PCWSTR capt = L"") {
	::MessageBoxW(nullptr, text, capt, MB_OK);
}

template<typename Type>
void StrToCont(const ustring &src, Type dst, const ustring &delim = L" \t\n\r") {
	ustring::size_type start, end = 0;
	while ((start = src.find_first_not_of(delim, end)) != ustring::npos) {
		end = src.find_first_of(delim, start);
		dst = src.substr(start, end - start);
	}
}

///▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒ err
ustring ErrAsStr(DWORD err = ::GetLastError(), PCWSTR lib = nullptr);

inline ustring ErrWmiAsStr(HRESULT err) {
	return ErrAsStr(err, L"wmiutils.dll");
}

#endif //WIN_STD_HPP
