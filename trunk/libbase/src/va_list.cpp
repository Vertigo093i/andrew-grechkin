﻿#include <libbase/va_list.hpp>

#include <stdarg.h>
#include <stdio.h>

namespace {
	const size_t default_buffer_size = 4 * 1024;
}

namespace Base {

	ustring as_str(PCWSTR format, ...) {
		va_list args;
		va_start(args, format);
		auto tmp = as_str(format, args);
		va_end(args);
		return tmp;
	}

	ustring as_str(PCWSTR format, va_list args) {
		WCHAR buf[default_buffer_size];
		size_t size = lengthof(buf) - 1;
		buf[size] = L'\0';
		::_vsnwprintf(buf, size, format, args);
		return ustring(buf);
	}

}