﻿#include <libwin_net/win_net.h>

#include "wmi_nic.h"

///=============================================================================== WmiNetworkAdapter
WmiEnum WmiNetworkAdapter::Enum(const WmiConnection &conn) {
	return WmiEnum(conn.Enum(L"Win32_NetworkAdapter"));
}

void WmiNetworkAdapter::Disable() const {
	exec_method(L"Disable");
}

void WmiNetworkAdapter::Enable() const {
	exec_method(L"Enable");
}

BStr WmiNetworkAdapter::Path(DWORD id) const {
	WCHAR	path[MAX_PATH];
	::_snwprintf(path, sizeofa(path), L"Win32_NetworkAdapter.DeviceID=\"%d\"", id);
	return BStr(path);
}

///=========================================================================== WmiNetworkAdapterConf
WmiEnum WmiNetworkAdapterConf::Enum(const WmiConnection &conn) {
	return WmiEnum(conn.Enum(L"Win32_NetworkAdapterConfiguration"));
}

size_t WmiNetworkAdapterConf::InterfaceIndex() const {
	return get_param(L"InterfaceIndex").as_uint();
}

size_t WmiNetworkAdapterConf::EnableDHCP() const {
	return exec_method_get_param(L"EnableDHCP", L"ReturnValue").as_uint();
}

size_t WmiNetworkAdapterConf::EnableStatic(const std::vector<AutoUTF> &ip, const std::vector<AutoUTF> &mask) const {
	WmiObject in_params = WmiObject::get_in_params(conn().get_object_class(m_obj), L"EnableStatic");

	in_params.Put(L"IPAddress", Variant(&ip[0], ip.size()));
	in_params.Put(L"SubnetMask", Variant(&mask[0], mask.size()));

	return exec_method_get_param(L"EnableStatic", in_params).as_uint();
}

size_t WmiNetworkAdapterConf::SetGateways(const std::vector<AutoUTF> &ip) const {
	WmiObject in_params(WmiObject::get_in_params(conn().get_object_class(m_obj), L"SetGateways"));

	in_params.Put(L"DefaultIPGateway", Variant(&ip[0], ip.size()));
//	in_params.Put(L"GatewayCostMetric", Variant(&metric[0], metric.size()));

	return exec_method_get_param(L"SetGateways", in_params).as_uint();
}

size_t WmiNetworkAdapterConf::SetDNSServerSearchOrder(const std::vector<AutoUTF> &ip) const {
	WmiObject in_params(WmiObject::get_in_params(conn().get_object_class(m_obj), L"SetDNSServerSearchOrder"));

	in_params.Put(L"DNSServerSearchOrder", Variant(&ip[0], ip.size()));

	return exec_method_get_param(L"SetDNSServerSearchOrder", in_params).as_uint();
}

size_t WmiNetworkAdapterConf::ReleaseDHCPLease() const {
	return exec_method_get_param(L"ReleaseDHCPLease", L"ReturnValue").as_uint();
}

size_t WmiNetworkAdapterConf::RenewDHCPLease() const {
	return exec_method_get_param(L"RenewDHCPLease", L"ReturnValue").as_uint();
}

IpAddresses WmiNetworkAdapterConf::GetIP() const {
	Variant vip(get_param(L"IPAddress"));
	Variant vms(get_param(L"IPSubnet"));
	IpAddresses ret;
	if (!vip.is_null() && !vms.is_null()) {
		SafeArray<BSTR> ip(vip);
		SafeArray<BSTR> ms(vms);
		if (ip.size() == ms.size()) {
			for (size_t i = 0; i < ip.size(); ++i) {
				ret.insert(IpAddresses::value_type(ip.at(i), ms.at(i)));
			}
		}
	}
	return ret;
}

int WmiNetworkAdapterConf::IPv4Add(const AutoUTF & ip) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 add address %Id %s", InterfaceIndex(), ip.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv4AddGateway(const AutoUTF & gw, size_t met) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 add address %Id gateway=%s gwmetric=%Id", InterfaceIndex(), gw.c_str(), met);
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv4AddDns(const AutoUTF & dns) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 add dnsservers %Id %s validate=no", InterfaceIndex(), dns.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6Add(const AutoUTF & ip) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 add address %Id %s", InterfaceIndex(), ip.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6AddGateway(const AutoUTF & gw) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 add route ::/0 %Id %s", InterfaceIndex(), gw.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6AddDns(const AutoUTF & dns) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 add dnsservers %Id %s validate=no", InterfaceIndex(), dns.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv4Del(const AutoUTF & ip) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 delete address %Id %s", InterfaceIndex(), ip.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv4DelGateway(const AutoUTF & gw) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 delete address %Id 0.0.0.0 gateway=%s", InterfaceIndex(), gw.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv4DelDns(const AutoUTF & dns) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 delete dnsservers %Id %s validate=no", InterfaceIndex(), dns.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6Del(const AutoUTF & ip) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 delete address %Id %s", InterfaceIndex(), ip.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6DelGateway(const AutoUTF & gw) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 delete route ::/0 %Id %s", InterfaceIndex(), gw.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6DelDns(const AutoUTF & dns) {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 delete dnsservers %Id %s validate=no", InterfaceIndex(), dns.c_str());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv4DhcpEnable() {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 set address %Id source=dhcp", InterfaceIndex());
	Exec::RunWait(buf, 10000);
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv4 set dnsservers %Id source=dhcp", InterfaceIndex());
	return Exec::RunWait(buf, 10000);
}

int WmiNetworkAdapterConf::IPv6DhcpEnable() {
	WCHAR buf[MAX_PATH];
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 set interface %Id advertise=disabled managedaddress=disabled", InterfaceIndex());
	Exec::RunWait(buf, 10000);
	::_snwprintf(buf, sizeofa(buf), L"netsh.exe interface ipv6 set dnsservers %Id source=dhcp", InterfaceIndex());
	return Exec::RunWait(buf, 10000);
}

BStr WmiNetworkAdapterConf::Path(DWORD index) const {
	WCHAR path[MAX_PATH];
	::_snwprintf(path, sizeofa(path), L"Win32_NetworkAdapterConfiguration.index=%d", index);
	return BStr(path);
}
