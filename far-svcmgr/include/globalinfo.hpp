﻿/**
	svcmgr: Manage services
	Allow to manage windows services
	FAR3 plugin

	© 2012 Andrew Grechkin

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef _FAR_GLOBALINFO_HPP_
#define _FAR_GLOBALINFO_HPP_

#include <libfar3/globalinfo_i.hpp>
#include <libfar3/settings.hpp>
#include <libbase/observer_p.hpp>
#include <libbase/uncopyable.hpp>


///====================================================================================== GlobalInfo
struct FarGlobalInfo: public Far::GlobalInfo_i, public Base::Observable_p, private Base::Uncopyable {
	static FarGlobalInfo & inst();

	~FarGlobalInfo();

	PCWSTR get_author() const override;

	PCWSTR get_description() const override;

	const GUID * get_guid() const override;

	PCWSTR get_title() const override;

	VersionInfo get_version() const override;

	int Configure(const ConfigureInfo * Info) override;

	Far::Plugin_i * CreatePlugin(const PluginStartupInfo * Info) override;

	void load_settings();

	ssize_t addToPluginsMenu;
	ssize_t addToDisksMenu;
	ssize_t waitForState;
	ssize_t waitTimeout;
	WCHAR Prefix[32];

private:
	FarGlobalInfo();

	void save_settings() const;

	Far::Settings_t * m_settings;
};


#endif