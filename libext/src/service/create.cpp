#include <libext/service.hpp>

#include <libbase/pcstr.hpp>


namespace Ext {

	void Service::Create_t::set_type(Type_t n) {
		serviceType = (DWORD)n;
	}

	void Service::Create_t::set_start(Start_t n) {
		startType = (DWORD)n;
	}

	void Service::Create_t::set_error_control(Error_t n) {
		errorControl = (DWORD)n;
	}

	void Service::Create_t::set_group(PCWSTR n) {
		if (!Base::Str::is_empty(n))
			loadOrderGroup = n;
	}

	void Service::Create_t::set_tag(DWORD & n) {
		tagId = &n;
	}

	void Service::Create_t::set_dependencies(PCWSTR n) {
		if (!Base::Str::is_empty(n))
			dependencies = n;
	}

	void Service::Create_t::set_display_name(PCWSTR n) {
		if (!Base::Str::is_empty(n))
			displayName = n;
	}

	PCWSTR Service::Create_t::get_name() const {
		return name.c_str();
	}

}
