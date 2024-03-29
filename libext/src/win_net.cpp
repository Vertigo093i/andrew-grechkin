﻿#include <libbase/std.hpp>
#include <libbase/str.hpp>
#include <libext/timer.hpp>
#include <libext/exception.hpp>

namespace Ext {

///========================================================================================== WinNet
namespace WinNet {
	ustring		GetCompName(COMPUTER_NAME_FORMAT cnf) {
		DWORD	size = 0;
		::GetComputerNameExW(cnf, nullptr, &size);
		wchar_t	buf[size];
		::GetComputerNameExW(cnf, buf, &size);
		return buf;
	}
}

///==================================================================================== WinSysTimers
WinSysTimers::WinSysTimers() {
	Base::Memory::zero(this, sizeof(*this));
	typedef LONG(WINAPI * PROCNTQSI)(UINT, PVOID, ULONG, PULONG);

	PROCNTQSI NtQuerySystemInformation;

	NtQuerySystemInformation = (PROCNTQSI)::GetProcAddress(::GetModuleHandleW(L"ntdll"), "NtQuerySystemInformation");

	if (!NtQuerySystemInformation)
		return;

	NtQuerySystemInformation(3, this, sizeof(*this), 0);
}

size_t	WinSysTimers::Uptime(size_t del) {
	ULONGLONG	start = liKeBootTime.QuadPart;
	ULONGLONG	now = liKeSystemTime.QuadPart;
	ULONGLONG	Result = (now - start) / 10000000LL;
	return Result / del;
}

ustring	WinSysTimers::UptimeAsText() {
	size_t	uptime = Uptime();
	size_t	ud = uptime / (60 * 60 * 24);
	uptime %= (60 * 60 * 24);
	size_t	uh = uptime / (60 * 60);
	uptime %= (60 * 60);
	size_t	um = uptime  / 60;
	size_t	us = uptime % 60;
	ustring	Result;
	if (ud) {
		Result += Base::as_str(ud);
		Result += L"d ";
	}

	Result += Base::as_str(uh);
	Result += L":";
	Result += Base::as_str(um);
	Result += L":";
	Result += Base::as_str(us);
	return Result;
}

}
