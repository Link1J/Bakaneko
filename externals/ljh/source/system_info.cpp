
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ljh/system_info.hpp"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#if defined(LJH_TARGET_Windows)
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <windows.h>
#    include <winternl.h>
#    include "ljh/windows/registry.hpp"
#    include "ljh/char_convertions.hpp"
#    include "ljh/function_pointer.hpp"
#    undef MessageBox
static ljh::function_pointer<NTSTATUS WINAPI(POSVERSIONINFOW            )> RtlGetVersion        ;
static ljh::function_pointer<const char*    (                           )> wine_get_version     ;
static ljh::function_pointer<const char*    (                           )> wine_get_build_id    ;
static ljh::function_pointer<void           (const char**, const char **)> wine_get_host_version;
static ljh::function_pointer<int      WINAPI(HWND, LPCSTR, LPCSTR, UINT )> MessageBox           ;
#elif defined(LJH_TARGET_MacOS)
#    include <sys/utsname.h>
ljh::version __os_info_get_version_obj_c();
std::string  __os_info_get_string_obj_c ();
#elif defined(LJH_TARGET_Linux)
#    include <sys/utsname.h>
#    include <fstream>
#elif defined(LJH_TARGET_Android)
#    include <sys/system_properties.h>
#elif defined(LJH_TARGET_iOS)
#    include <sys/utsname.h>
ljh::version __os_info_get_version_obj_c();
std::string  __os_info_get_string_obj_c ();
std::string  __os_info_get_model_obj_c  ();
#elif defined(LJH_TARGET_Unix)
#    include <sys/utsname.h>
#endif

using namespace ljh::int_types;

static void init_static()
{
	static bool setup = false;
	
	if (setup) { return; }
	setup = true;

#if defined(LJH_TARGET_Windows)
	HMODULE ntdll  = LoadLibrary(TEXT("ntdll.dll" ));
	HMODULE user32 = LoadLibrary(TEXT("user32.dll"));

	RtlGetVersion         = GetProcAddress(ntdll , "RtlGetVersion"        );
	wine_get_version      = GetProcAddress(ntdll , "wine_get_version"     );
	wine_get_build_id     = GetProcAddress(ntdll , "wine_get_build_id"    );
	wine_get_host_version = GetProcAddress(ntdll , "wine_get_host_version");
	MessageBox            = GetProcAddress(user32, "MessageBoxA"          );

	// Windows 10 Threshold
	auto version = *ljh::system_info::get_version();
	if (version.major() < 10 && version.build() > 9600)
	{
		std::string message_text = 
			"Windows " + std::string{version} + 
			" is not a version of Windows that should be used."
			" Please install a real version of Windows 10.";
		MessageBox(NULL, message_text.c_str(), "Not Windows 10", MB_OK | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY | MB_ICONERROR);
	}

	// Windows Longhorn
	if (version.major() >= 6 && version.major() == 0 && version.build() > 5000)
	{
		std::string message_text = 
			"Windows Vista's pre-reset builds are buggy and broken.\n"
			"Please install install version of Windows that works correctly.";
		MessageBox(NULL, message_text.c_str(), "Not Windows Vista", MB_OK | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY | MB_ICONERROR);
	}
	try
	{
		if ((std::wstring)ljh::windows::registry::key::LOCAL_MACHINE[L"SOFTWARE"][L"Microsoft"][L"Windows NT"][L"CurrentVersion"](L"BuildLab") == L"5.2.3790.1232.winmain.040819-1629")
		{
			MessageBox(NULL, "Why are you using this build?", "Really?", MB_OK | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY | MB_ICONERROR);
			ExitProcess(1);
		}
	}
	catch (LSTATUS)
	{
	}

	// Windows Neptune
	if (version.major() < 6 && version.build() >= 5000)
	{
		std::string message_text = 
			"Windows Vista's pre-reset builds are buggy and broken.\n"
			"Please install install version of Windows that works correctly.";
		MessageBox(NULL, message_text.c_str(), "Not Windows Vista", MB_OK | MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY | MB_ICONERROR);
	}

#elif defined(LJH_TARGET_MacOS)
#elif defined(LJH_TARGET_Linux)
#elif defined(LJH_TARGET_Android)
#elif defined(LJH_TARGET_iOS)
#elif defined(LJH_TARGET_Unix)
#endif
}

#if defined(LJH_TARGET_Windows)
static std::string to_utf8(std::wstring_view string)
{
	std::string output;
	int size = WideCharToMultiByte(CP_UTF8, 0, string.data(), -1, NULL, 0, NULL, NULL);
	output.resize(size);
	WideCharToMultiByte(CP_UTF8, 0, string.data(), -1, (char*)output.data(), size, NULL, NULL);
	output.resize(size - 1);
	return output;
}
#elif defined(LJH_TARGET_Android)
static std::string get_system_prop(std::string_view string)
{
	char osVersion[PROP_VALUE_MAX+1];
	unsigned int osVersionLength = __system_property_get(string.data(), osVersion);
	return std::string{osVersion, osVersionLength};
}
#endif

ljh::expected<ljh::system_info::platform, ljh::system_info::error> ljh::system_info::get_platform()
{
	init_static();
#if defined(LJH_TARGET_Windows)
	return platform::Windows;
#elif defined(LJH_TARGET_MacOS)
	return platform::macOS;
#elif defined(LJH_TARGET_Linux)
	return platform::Linux;
#elif defined(LJH_TARGET_Android)
	return platform::Android;
#elif defined(LJH_TARGET_iOS)
	return platform::iOS;
#elif defined(LJH_TARGET_Unix)
	return platform::Unix;
#else
	return unexpected{error::unknown_os};
#endif
}

ljh::expected<ljh::version, ljh::system_info::error> ljh::system_info::get_version()
{
	init_static();
#if defined(LJH_TARGET_Windows)

	version::value_type v_patch = 0;
	try
	{
		v_patch = (version::value_type)windows::registry::key::LOCAL_MACHINE[L"SOFTWARE"][L"Microsoft"][L"Windows NT"][L"CurrentVersion"](L"UBR");
	}
	catch (LSTATUS)
	{
	}

	OSVERSIONINFOW osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	if (RtlGetVersion) { RtlGetVersion(&osinfo); }
	else               { GetVersionExW(&osinfo); }

	return version{osinfo.dwMajorVersion, osinfo.dwMinorVersion, osinfo.dwBuildNumber, v_patch};
#elif defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	struct utsname buffer;
	uname(&buffer);
	return buffer.release;
#elif defined(LJH_TARGET_Android)
	return get_system_prop("ro.build.version.release");
#elif defined(LJH_TARGET_MacOS)
	auto version = __os_info_get_version_obj_c();
	if (version > macOS_10_15 && version < macOS_11)
		version = macOS_11.version;
	return version;
#elif defined(LJH_TARGET_iOS)
	return get_version_obj_c();
#else
	return unexpected{error::unknown_os};
#endif
}

ljh::expected<std::string, ljh::system_info::error> ljh::system_info::get_string()
{
	init_static();
#if defined(LJH_TARGET_Windows)
	std::string string = "";

	if (wine_get_version)
	{
		const char* sysname,* release;
		wine_get_host_version(&sysname, &release);
		string = std::string{wine_get_build_id()} + " on " + sysname + " " + release;
	}

	if (auto SYSTEM = windows::registry::key::LOCAL_MACHINE.get_key(L"SYSTEM"); SYSTEM.has_value())
		if (auto CurrentControlSet = SYSTEM->get_key(L"CurrentControlSet"); CurrentControlSet.has_value())
			if (auto Control = CurrentControlSet->get_key(L"Control"); Control.has_value())
				if (auto ReactOS = Control->get_key(L"ReactOS"); ReactOS.has_value())
				{
					// This works in some versions
					OSVERSIONINFOA osinfo;
					osinfo.dwOSVersionInfoSize = sizeof(osinfo);
					GetVersionExA(&osinfo);
					if (osinfo.szCSDVersion[strlen(osinfo.szCSDVersion) + 1] == L'R')
						string = &osinfo.szCSDVersion[strlen(osinfo.szCSDVersion) + 1 + 8];
					else
						string = "Unknown ReactOS Version ";
				}

	if (!string.empty()) { string += " mimicking "; }

	auto key = windows::registry::key::LOCAL_MACHINE[L"SOFTWARE"][L"Microsoft"][L"Windows NT"][L"CurrentVersion"];
	auto os_name = to_utf8((std::wstring)key(L"ProductName"));
	if (os_name.find("Microsoft ") == 0)
		os_name = os_name.substr(10);
	if (auto line = os_name.find("Windows 10"); line != std::string::npos)
	{
		line += 10;
		if (auto version_display = key.get_value(L"DisplayVersion"); version_display.has_value())
			os_name = os_name.substr(0, line + 1) + to_utf8(version_display->get<std::wstring>()) + os_name.substr(line);
		else if (version_display = key.get_value(L"ReleaseId"); version_display.has_value())
			os_name = os_name.substr(0, line + 1) + to_utf8(version_display->get<std::wstring>()) + os_name.substr(line);
	}
	else
	{
		line += os_name.find_first_of(' ', line + 11);
		if (auto version_display = key.get_value(L"CSDVersion"); version_display.has_value())
			os_name = os_name.substr(0, line + 1) + to_utf8(version_display->get<std::wstring>()) + os_name.substr(line);
	}
	string += os_name;

	return string;
#elif defined(LJH_TARGET_MacOS)
	auto string = __os_info_get_string_obj_c();
	auto ver = __os_info_get_version_obj_c();

	if (!string.empty())
		string = string.substr(8);
	else
		string = ver;

	if (ver >= version{10, 12})
		string = "macOS " + string;
	else if (ver >= version{10, 8})
		string = "OS X " + string;
	else if (ver >= version{10})
		string = "Mac OS X " + string;
	else
		string = "Mac OS " + string;

	return string;
#elif defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	std::ifstream file;
	std::string name;

	file.open("/etc/os-release");
	if (file.is_open())
	{
		std::string version;
		std::string temp;

		while(file)
		{
			std::getline(file, temp);

			if (temp.starts_with("NAME="   ))
				name    = temp.substr(6, temp.find_last_of('"') - 6);
			if (temp.starts_with("VERSION="))
				version = temp.substr(9, temp.find_last_of('"') - 9);
		}

		if (version != "")
			name += " " + version;
		name += " on ";
	}

	struct utsname buffer;
	uname(&buffer);
	name += buffer.sysname + std::string{" "} + buffer.release;

	return name;
#elif defined(LJH_TARGET_Android)
	std::string string = std::string{"Android "} + (std::string)*get_version();
	if (auto sdk = *get_sdk(); sdk >= 29) {}
	else if (sdk >= 28) { string += " Pie"               ; }
	else if (sdk >= 26) { string += " Oreo"              ; }
	else if (sdk >= 24) { string += " Nougat"            ; }
	else if (sdk >= 23) { string += " Marshmallow"       ; }
	else if (sdk >= 21) { string += " Lollipop"          ; }
	else if (sdk >= 19) { string += " KitKat"            ; }
	else if (sdk >= 16) { string += " Jelly_Bean"        ; }
	else if (sdk >= 14) { string += " Ice_Cream_Sandwich"; }
	else if (sdk >= 11) { string += " Honeycomb"         ; }
	else if (sdk >= 9 ) { string += " Gingerbread"       ; }
	else if (sdk >= 8 ) { string += " Froyo"             ; }
	else if (sdk >= 5 ) { string += " Eclair"            ; }
	else if (sdk >= 4 ) { string += " Donut"             ; }
	else if (sdk >= 3 ) { string += " Cupcake"           ; }
	return string;
#elif defined(LJH_TARGET_iOS)
	auto string = __os_info_get_string_obj_c();
	auto ver = __os_info_get_version_obj_c();
	
	if (!string.empty())
		string = string.substr(8);
	else
		string = ver;

	if (ver >= version{13} && __os_info_get_model_obj_c().starts_with("iPad"))
		string = "iPadOS " + string;
	else if (ver >= version{3})
		string = "iOS " + string;
	else
		string = "iPhone OS " + string;

	return string;
#else
	return unexpected{error::unknown_os};
#endif
}

ljh::expected<ljh::u32, ljh::system_info::error> ljh::system_info::get_sdk()
{
	init_static();
#if defined(LJH_TARGET_Windows)
	auto info_version = get_version();
	if (!info_version)
	{
		return unexpected{info_version.error()};
	}
	return info_version->build();
#elif defined(LJH_TARGET_MacOS) || defined(LJH_TARGET_iOS)
	auto version = __os_info_get_version_obj_c();
	return version.major() << 16 | version.minor() << 8  | version.build();
#elif defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	return unexpected{error::no_info};
#elif defined(LJH_TARGET_Android)
	u32 value;
	from_string(value, get_system_prop("ro.build.version.sdk"));
	return value;
#else
	return unexpected{error::unknown_os};
#endif
}

ljh::expected<std::string, ljh::system_info::error> ljh::system_info::get_model()
{
	init_static();
#if defined(LJH_TARGET_Windows)
	if (*get_sdk() >= Windows_7)
	{
		auto key = windows::registry::key::LOCAL_MACHINE[L"HARDWARE"][L"DESCRIPTION"][L"System"][L"BIOS"];
		return to_utf8((std::wstring)key(L"SystemFamily") + L" " + (std::wstring)key(L"SystemProductName"));
	}
	else
	{
		auto key = windows::registry::key::LOCAL_MACHINE[L"SYSTEM"][L"CurrentControlSet"][L"Control"][L"SystemInformation"];
		return to_utf8((std::wstring)key(L"SystemProductName"));
	}
#elif defined(LJH_TARGET_MacOS) || defined(LJH_TARGET_iOS)
	struct utsname buffer;
	uname(&buffer);
	return buffer.machine;
#elif defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	return unexpected{error::no_info};
#elif defined(LJH_TARGET_Android)
	return get_system_prop("ro.product.model");
#else
	return unexpected{error::unknown_os};
#endif
}

ljh::expected<std::string, ljh::system_info::error> ljh::system_info::get_manufacturer()
{
	init_static();
#if defined(LJH_TARGET_Windows)
	windows::registry::key key;
	if (*get_sdk() >= Windows_7)
		key = windows::registry::key::LOCAL_MACHINE[L"HARDWARE"][L"DESCRIPTION"][L"System"][L"BIOS"];
	else
		key = windows::registry::key::LOCAL_MACHINE[L"SYSTEM"][L"CurrentControlSet"][L"Control"][L"SystemInformation"];
	return to_utf8((std::wstring)key(L"SystemManufacturer"));
#elif defined(LJH_TARGET_MacOS) || defined(LJH_TARGET_iOS)
	return "Apple";
#elif defined(LJH_TARGET_Linux) || defined(LJH_TARGET_Unix)
	return unexpected{error::no_info};
#elif defined(LJH_TARGET_Android)
	return get_system_prop("ro.product.manufacturer");
#else
	return unexpected{error::unknown_os};
#endif
}


bool ljh::system_info::info_data::operator==(const ljh::system_info::platform& other) const { return platform == other; }
bool ljh::system_info::info_data::operator==(const ljh::version              & other) const { return version  == other; }
bool ljh::system_info::info_data::operator!=(const ljh::version              & other) const { return version  != other; }
bool ljh::system_info::info_data::operator< (const ljh::version              & other) const { return version  <  other; }
bool ljh::system_info::info_data::operator> (const ljh::version              & other) const { return version  >  other; }
bool ljh::system_info::info_data::operator>=(const ljh::version              & other) const { return version  >= other; }
bool ljh::system_info::info_data::operator<=(const ljh::version              & other) const { return version  <= other; }
bool ljh::system_info::info_data::operator==(const ljh::u32                  & other) const { return sdk      == other; }
bool ljh::system_info::info_data::operator!=(const ljh::u32                  & other) const { return sdk      != other; }
bool ljh::system_info::info_data::operator< (const ljh::u32                  & other) const { return sdk      <  other; }
bool ljh::system_info::info_data::operator> (const ljh::u32                  & other) const { return sdk      >  other; }
bool ljh::system_info::info_data::operator>=(const ljh::u32                  & other) const { return sdk      >= other; }
bool ljh::system_info::info_data::operator<=(const ljh::u32                  & other) const { return sdk      <= other; }
bool operator==(const ljh::system_info::platform& lhs, const ljh::system_info::info_data& rhs) { return rhs == lhs; }
bool operator==(const ljh::version              & lhs, const ljh::system_info::info_data& rhs) { return rhs == lhs; }
bool operator!=(const ljh::version              & lhs, const ljh::system_info::info_data& rhs) { return rhs != lhs; }
bool operator< (const ljh::version              & lhs, const ljh::system_info::info_data& rhs) { return rhs >  lhs; }
bool operator> (const ljh::version              & lhs, const ljh::system_info::info_data& rhs) { return rhs <  lhs; }
bool operator>=(const ljh::version              & lhs, const ljh::system_info::info_data& rhs) { return rhs <= lhs; }
bool operator<=(const ljh::version              & lhs, const ljh::system_info::info_data& rhs) { return rhs >= lhs; }
bool operator==(const ljh::u32                  & lhs, const ljh::system_info::info_data& rhs) { return rhs == lhs; }
bool operator!=(const ljh::u32                  & lhs, const ljh::system_info::info_data& rhs) { return rhs != lhs; }
bool operator< (const ljh::u32                  & lhs, const ljh::system_info::info_data& rhs) { return rhs >  lhs; }
bool operator> (const ljh::u32                  & lhs, const ljh::system_info::info_data& rhs) { return rhs <  lhs; }
bool operator>=(const ljh::u32                  & lhs, const ljh::system_info::info_data& rhs) { return rhs <= lhs; }
bool operator<=(const ljh::u32                  & lhs, const ljh::system_info::info_data& rhs) { return rhs >= lhs; }

namespace ljh::system_info::inline versions
{
	const info_data Windows_1_01                { platform::WindowsDOS, { 1,  1,     0            },      0};
	const info_data Windows_1_02                { platform::WindowsDOS, { 1,  2,     0            },      0};
	const info_data Windows_1_03                { platform::WindowsDOS, { 1,  3,     0            },      0};
	const info_data Windows_1_04                { platform::WindowsDOS, { 1,  4,     0            },      0};
	const info_data Windows_2_03                { platform::WindowsDOS, { 2,  3,     0            },      0};
	const info_data Windows_2_10                { platform::WindowsDOS, { 2, 10,     0            },      0};
	const info_data Windows_2_11                { platform::WindowsDOS, { 2, 11,     0            },      0};
	const info_data Windows_3_0                 { platform::WindowsDOS, { 3,  0,     0            },      0};
	const info_data Windows_3_1                 { platform::WindowsDOS, { 3,  1,   103            },    103};
	const info_data Windows_3_11                { platform::WindowsDOS, { 3, 11,   300            },    300};
	const info_data Windows_95                  { platform::WindowsDOS, { 4,  0,   950            },    950};
	const info_data Windows_98                  { platform::WindowsDOS, { 4, 10,  1998            },   1998};
	const info_data Windows_98SE                { platform::WindowsDOS, { 4, 10,  2222            },   2222};
	const info_data Windows_ME                  { platform::WindowsDOS, { 4, 90,  3000            },   3000};
	const info_data Windows_NT_3_1              { platform::WindowsNT , { 3,  1,   511 /*,     1*/},    511};
	const info_data Windows_NT_3_1_SP_1         { platform::WindowsNT , { 3,  1,   511 /*,     1*/},    511};
	const info_data Windows_NT_3_1_SP_2         { platform::WindowsNT , { 3,  1,   511 /*,     1*/},    511};
	const info_data Windows_NT_3_1_SP_3         { platform::WindowsNT , { 3,  1,   528 /*,     1*/},    528};
	const info_data Windows_NT_3_5              { platform::WindowsNT , { 3,  5,   807 /*,     1*/},    807};
	const info_data Windows_NT_3_5_SP_1         { platform::WindowsNT , { 3,  5,   807 /*,     1*/},    807};
	const info_data Windows_NT_3_5_SP_2         { platform::WindowsNT , { 3,  5,   807 /*,     1*/},    807};
	const info_data Windows_NT_3_5_SP_3         { platform::WindowsNT , { 3,  5,   807 /*,     1*/},    807};
	const info_data Windows_NT_3_51             { platform::WindowsNT , { 3, 51,  1057 /*,     1*/},   1057};
	const info_data Windows_NT_3_51_SP_1        { platform::WindowsNT , { 3, 51,  1057 /*,     2*/},   1057};
	const info_data Windows_NT_3_51_SP_2        { platform::WindowsNT , { 3, 51,  1057 /*,     3*/},   1057};
	const info_data Windows_NT_3_51_SP_3        { platform::WindowsNT , { 3, 51,  1057 /*,     4*/},   1057};
	const info_data Windows_NT_3_51_SP_4        { platform::WindowsNT , { 3, 51,  1057 /*,     5*/},   1057};
	const info_data Windows_NT_3_51_SP_5        { platform::WindowsNT , { 3, 51,  1057 /*,     6*/},   1057};
	const info_data Windows_NT_4_0              { platform::WindowsNT , { 4,  0,  1381 /*,     1*/},   1381};
	const info_data Windows_NT_4_0_SP_1         { platform::WindowsNT , { 4,  0,  1381 /*,     2*/},   1381};
	const info_data Windows_NT_4_0_SP_2         { platform::WindowsNT , { 4,  0,  1381 /*,     3*/},   1381};
	const info_data Windows_NT_4_0_SP_3         { platform::WindowsNT , { 4,  0,  1381 /*,     4*/},   1381};
	const info_data Windows_NT_4_0_SP_4         { platform::WindowsNT , { 4,  0,  1381 /*,   133*/},   1381};
	const info_data Windows_NT_4_0_SP_5         { platform::WindowsNT , { 4,  0,  1381 /*,   204*/},   1381};
	const info_data Windows_NT_4_0_SP_6         { platform::WindowsNT , { 4,  0,  1381 /*,   335*/},   1381};
	const info_data Windows_NT_4_0_SP_6a        { platform::WindowsNT , { 4,  0,  1381 /*,   335*/},   1381};
	const info_data Windows_NT_5_0              { platform::WindowsNT , { 5,  0,  2195 /*,     1*/},   2195};
	const info_data Windows_2000_SP_1           { platform::WindowsNT , { 5,  0,  2195 /*,  1620*/},   2195};
	const info_data Windows_2000_SP_2           { platform::WindowsNT , { 5,  0,  2195 /*,  2951*/},   2195};
	const info_data Windows_2000_SP_3           { platform::WindowsNT , { 5,  0,  2195 /*,  5438*/},   2195};
	const info_data Windows_2000_SP_4           { platform::WindowsNT , { 5,  0,  2195 /*,  6717*/},   2195};
	const info_data Windows_NT_5_1              { platform::WindowsNT , { 5,  1,  2600 /*,     0*/},   2600};
	const info_data Windows_XP_SP_1             { platform::WindowsNT , { 5,  1,  2600 /*,  1106*/},   2600};
	const info_data Windows_XP_SP_2             { platform::WindowsNT , { 5,  1,  2600 /*,  2180*/},   2600};
	const info_data Windows_XP_SP_3             { platform::WindowsNT , { 5,  1,  2600 /*,  5512*/},   2600};
	const info_data Windows_NT_5_2              { platform::WindowsNT , { 5,  2,  3790 /*,  1830*/},   3790};
	const info_data Windows_XP_x64_SP_3         { platform::WindowsNT , { 5,  2,  3790 /*,  3959*/},   3790};
	const info_data Windows_NT_6_0              { platform::WindowsNT , { 6,  0,  6000 /*, 16386*/},   6000};
	const info_data Windows_Vista_SP_1          { platform::WindowsNT , { 6,  0,  6001 /*, 18000*/},   6001};
	const info_data Windows_Vista_SP_2          { platform::WindowsNT , { 6,  0,  6002 /*, 18005*/},   6002};
	const info_data Windows_Vista_SP_2_Update   { platform::WindowsNT , { 6,  0,  6003 /*, 20491*/},   6003};
	const info_data Windows_NT_6_1              { platform::WindowsNT , { 6,  1,  7600 /*, 16385*/},   7600};
	const info_data Windows_7_SP_1              { platform::WindowsNT , { 6,  1,  7601 /*, 17514*/},   7601};
	const info_data Windows_NT_6_2              { platform::WindowsNT , { 6,  2,  9200 /*, 16384*/},   9200};
	const info_data Windows_NT_6_3              { platform::WindowsNT , { 6,  3,  9600 /*, 16384*/},   9600};
	const info_data Windows_NT_10_0             { platform::WindowsNT , {10,  0, 10240 /*, 16384*/},  10240};
	const info_data Windows_10_1511             { platform::WindowsNT , {10,  0, 10586 /*,     0*/},  10586};
	const info_data Windows_10_1607             { platform::WindowsNT , {10,  0, 14393 /*,     0*/},  14393};
	const info_data Windows_10_1703             { platform::WindowsNT , {10,  0, 15063 /*,     0*/},  15063};
	const info_data Windows_10_1709             { platform::WindowsNT , {10,  0, 16299 /*,    15*/},  16299};
	const info_data Windows_10_1803             { platform::WindowsNT , {10,  0, 17134 /*,     1*/},  17134};
	const info_data Windows_10_1809             { platform::WindowsNT , {10,  0, 17763 /*,     1*/},  17763};
	const info_data Windows_10_1903             { platform::WindowsNT , {10,  0, 18362 /*,     1*/},  18362};
	const info_data Windows_10_1909             { platform::WindowsNT , {10,  0, 18363 /*,   418*/},  18363};
	const info_data Windows_10_2004             { platform::WindowsNT , {10,  0, 19041 /*,     1*/},  19041};
	const info_data Windows_10_20H2             { platform::WindowsNT , {10,  0, 19042 /*,   508*/},  19042};
	const info_data Android_API_1               { platform::Android   , { 1                       },      1};
	const info_data Android_API_2               { platform::Android   , { 1,  1                   },      2};
	const info_data Android_API_3               { platform::Android   , { 1,  5                   },      3};
	const info_data Android_API_4               { platform::Android   , { 1,  6                   },      4};
	const info_data Android_API_5               { platform::Android   , { 2                       },      5};
	const info_data Android_API_6               { platform::Android   , { 2,  0,     1            },      6};
	const info_data Android_API_7               { platform::Android   , { 2,  1                   },      7};
	const info_data Android_API_8               { platform::Android   , { 2,  2                   },      8};
	const info_data Android_API_9               { platform::Android   , { 2,  3                   },      9};
	const info_data Android_API_10              { platform::Android   , { 2,  3,     3            },     10};
	const info_data Android_API_11              { platform::Android   , { 3                       },     11};
	const info_data Android_API_12              { platform::Android   , { 3,  1                   },     12};
	const info_data Android_API_13              { platform::Android   , { 3,  2                   },     13};
	const info_data Android_API_14              { platform::Android   , { 4                       },     14};
	const info_data Android_API_15              { platform::Android   , { 4,  0,     3            },     15};
	const info_data Android_API_16              { platform::Android   , { 4,  1                   },     16};
	const info_data Android_API_17              { platform::Android   , { 4,  2                   },     17};
	const info_data Android_API_18              { platform::Android   , { 4,  3                   },     18};
	const info_data Android_API_19              { platform::Android   , { 4,  4                   },     19};
	const info_data Android_API_20              { platform::Android   , { 4,  4                   },     20};
	const info_data Android_API_21              { platform::Android   , { 5                       },     21};
	const info_data Android_API_22              { platform::Android   , { 5,  1                   },     22};
	const info_data Android_API_23              { platform::Android   , { 6                       },     23};
	const info_data Android_API_24              { platform::Android   , { 7                       },     24};
	const info_data Android_API_25              { platform::Android   , { 7,  1                   },     25};
	const info_data Android_API_26              { platform::Android   , { 8                       },     26};
	const info_data Android_API_27              { platform::Android   , { 8,  1                   },     27};
	const info_data Android_API_28              { platform::Android   , { 9                       },     28};
	const info_data Android_API_29              { platform::Android   , {10                       },     29};
	const info_data Android_API_30              { platform::Android   , {11                       },     30};
	const info_data Mac_OS_X_10_0               { platform::macOS     , {10,  0                   }, 655360};
	const info_data Mac_OS_X_10_1               { platform::macOS     , {10,  1                   }, 655616};
	const info_data Mac_OS_X_10_2               { platform::macOS     , {10,  2                   }, 655872};
	const info_data Mac_OS_X_10_3               { platform::macOS     , {10,  3                   }, 656128};
	const info_data Mac_OS_X_10_4               { platform::macOS     , {10,  4                   }, 656384};
	const info_data Mac_OS_X_10_5               { platform::macOS     , {10,  5                   }, 656640};
	const info_data Mac_OS_X_10_6               { platform::macOS     , {10,  6                   }, 656896};
	const info_data Mac_OS_X_10_7               { platform::macOS     , {10,  7                   }, 657152};
	const info_data     OS_X_10_8               { platform::macOS     , {10,  8                   }, 657408};
	const info_data     OS_X_10_9               { platform::macOS     , {10,  9                   }, 657664};
	const info_data     OS_X_10_10              { platform::macOS     , {10, 10                   }, 657920};
	const info_data     OS_X_10_11              { platform::macOS     , {10, 11                   }, 658176};
	const info_data    macOS_10_12              { platform::macOS     , {10, 12                   }, 658432};
	const info_data    macOS_10_13              { platform::macOS     , {10, 13                   }, 658688};
	const info_data    macOS_10_14              { platform::macOS     , {10, 14                   }, 658944};
	const info_data    macOS_10_15              { platform::macOS     , {10, 15                   }, 659200};
	const info_data    macOS_11                 { platform::macOS     , {11                       }, 720896};
	const info_data iPhone_OS_1_0               { platform::iOS       , { 1,  0                   },  65536};
	const info_data iPhone_OS_1_1               { platform::iOS       , { 1,  1                   },  65792};
	const info_data iPhone_OS_2_0               { platform::iOS       , { 2,  0                   }, 131072};
	const info_data iPhone_OS_2_1               { platform::iOS       , { 2,  1                   }, 131328};
	const info_data iPhone_OS_2_2               { platform::iOS       , { 2,  2                   }, 131584};
	const info_data iPhone_OS_3_0               { platform::iOS       , { 3,  0                   }, 196608};
	const info_data iPhone_OS_3_1               { platform::iOS       , { 3,  1                   }, 196864};
	const info_data iPhone_OS_3_2               { platform::iOS       , { 3,  2                   }, 197120};
	const info_data       iOS_4_0               { platform::iOS       , { 4,  0                   }, 262144};
	const info_data       iOS_4_1               { platform::iOS       , { 4,  1                   }, 262400};
	const info_data       iOS_5_0               { platform::iOS       , { 5,  0                   }, 327680};
	const info_data       iOS_5_1               { platform::iOS       , { 5,  1                   }, 327936};
	const info_data       iOS_6_0               { platform::iOS       , { 6,  0                   }, 393216};
	const info_data       iOS_6_1               { platform::iOS       , { 6,  1                   }, 393472};
	const info_data       iOS_7_0               { platform::iOS       , { 7,  0                   }, 458752};
	const info_data       iOS_7_1               { platform::iOS       , { 7,  1                   }, 459008};
	const info_data       iOS_8_0               { platform::iOS       , { 8,  0                   }, 524288};
	const info_data       iOS_8_1               { platform::iOS       , { 8,  1                   }, 524544};
	const info_data       iOS_9_0               { platform::iOS       , { 9,  0                   }, 589824};
	const info_data       iOS_9_1               { platform::iOS       , { 9,  1                   }, 590080};
	const info_data       iOS_9_2               { platform::iOS       , { 9,  2                   }, 590336};
	const info_data       iOS_9_3               { platform::iOS       , { 9,  3                   }, 590592};
	const info_data       iOS_9_4               { platform::iOS       , { 9,  4                   }, 590848};
	const info_data       iOS_10_0              { platform::iOS       , {10,  0                   }, 655360};
	const info_data       iOS_10_1              { platform::iOS       , {10,  1                   }, 655616};
	const info_data       iOS_10_2              { platform::iOS       , {10,  2                   }, 655872};
	const info_data       iOS_10_3              { platform::iOS       , {10,  3                   }, 656128};
	const info_data       iOS_11_0              { platform::iOS       , {11,  0                   }, 720896};
	const info_data       iOS_11_1              { platform::iOS       , {11,  1                   }, 721152};
	const info_data       iOS_11_2              { platform::iOS       , {11,  2                   }, 721408};
	const info_data       iOS_11_3              { platform::iOS       , {11,  3                   }, 721664};
	const info_data       iOS_12_0              { platform::iOS       , {12,  0                   }, 786432};
	const info_data       iOS_12_1              { platform::iOS       , {12,  1                   }, 786688};
	const info_data       iOS_12_2              { platform::iOS       , {12,  2                   }, 786944};
	const info_data       iOS_12_3              { platform::iOS       , {12,  3                   }, 787200};
	const info_data       iOS_12_4              { platform::iOS       , {12,  4                   }, 787456};
	const info_data       iOS_13_0              { platform::iOS       , {13,  0                   }, 851968};
	const info_data       iOS_13_1              { platform::iOS       , {13,  1                   }, 852224};
	const info_data       iOS_13_2              { platform::iOS       , {13,  2                   }, 852480};
	const info_data       iOS_13_3              { platform::iOS       , {13,  3                   }, 852736};
	const info_data       iOS_13_4              { platform::iOS       , {13,  4                   }, 852992};
	const info_data       iOS_13_5              { platform::iOS       , {13,  5                   }, 853248};
	const info_data       iOS_13_6              { platform::iOS       , {13,  6                   }, 853504};
	const info_data       iOS_13_7              { platform::iOS       , {13,  7                   }, 853760};
	const info_data       iOS_14_0              { platform::iOS       , {14,  0                   }, 917504};
	const info_data       iOS_14_1              { platform::iOS       , {14,  1                   }, 917760};
	const info_data       iOS_14_2              { platform::iOS       , {14,  2                   }, 918016};
	const info_data       iOS_14_3              { platform::iOS       , {14,  3                   }, 918272};
	const info_data Windows_1_0                 = Windows_1_01   ;
	const info_data Windows_2_0                 = Windows_2_03   ;
	const info_data Windows_2000                = Windows_NT_5_0 ;
	const info_data Windows_XP                  = Windows_NT_5_1 ;
	const info_data Windows_XP_x64              = Windows_NT_5_2 ;
	const info_data Windows_Vista               = Windows_NT_6_0 ;
	const info_data Windows_7                   = Windows_NT_6_1 ;
	const info_data Windows_8                   = Windows_NT_6_2 ;
	const info_data Windows_8_1                 = Windows_NT_6_3 ;
	const info_data Windows_10                  = Windows_NT_10_0;
	const info_data Windows_10_1507             = Windows_NT_10_0;
	const info_data Android_1                   = Android_API_1  ;
	const info_data Android_1_1                 = Android_API_2  ;
	const info_data Android_1_5                 = Android_API_3  ;
	const info_data Android_1_6                 = Android_API_4  ;
	const info_data Android_2                   = Android_API_5  ;
	const info_data Android_2_0_1               = Android_API_6  ;
	const info_data Android_2_1                 = Android_API_7  ;
	const info_data Android_2_2                 = Android_API_8  ;
	const info_data Android_2_3                 = Android_API_9  ;
	const info_data Android_2_3_3               = Android_API_10 ;
	const info_data Android_3                   = Android_API_11 ;
	const info_data Android_3_1                 = Android_API_12 ;
	const info_data Android_3_2                 = Android_API_13 ;
	const info_data Android_4                   = Android_API_14 ;
	const info_data Android_4_0_3               = Android_API_15 ;
	const info_data Android_4_1                 = Android_API_16 ;
	const info_data Android_4_2                 = Android_API_17 ;
	const info_data Android_4_3                 = Android_API_18 ;
	const info_data Android_4_4                 = Android_API_19 ;
	const info_data Android_4_4W                = Android_API_20 ;
	const info_data Android_5                   = Android_API_21 ;
	const info_data Android_5_1                 = Android_API_22 ;
	const info_data Android_6                   = Android_API_23 ;
	const info_data Android_7                   = Android_API_24 ;
	const info_data Android_7_1                 = Android_API_25 ;
	const info_data Android_8                   = Android_API_26 ;
	const info_data Android_8_1                 = Android_API_27 ;
	const info_data Android_9                   = Android_API_28 ;
	const info_data Android_10                  = Android_API_29 ;
	const info_data Android_11                  = Android_API_30 ;
	const info_data Android_Cupcake             = Android_API_3  ;
	const info_data Android_Donut               = Android_API_4  ;
	const info_data Android_Eclair              = Android_API_5  ;
	const info_data Android_Froyo               = Android_API_8  ;
	const info_data Android_Gingerbread         = Android_API_9  ;
	const info_data Android_Honeycomb           = Android_API_11 ;
	const info_data Android_Ice_Cream_Sandwich  = Android_API_14 ;
	const info_data Android_Jelly_Bean          = Android_API_16 ;
	const info_data Android_KitKat              = Android_API_19 ;
	const info_data Android_Lollipop            = Android_API_21 ;
	const info_data Android_Marshmallow         = Android_API_23 ;
	const info_data Android_Nougat              = Android_API_24 ;
	const info_data Android_Oreo                = Android_API_26 ;
	const info_data Android_Pie                 = Android_API_28 ;
	const info_data Mac_OS_X_Jaguar             = Mac_OS_X_10_2  ;
	const info_data Mac_OS_X_Panther            = Mac_OS_X_10_3  ;
	const info_data Mac_OS_X_Tiger              = Mac_OS_X_10_4  ;
	const info_data Mac_OS_X_Leopard            = Mac_OS_X_10_5  ;
	const info_data Mac_OS_X_Snow_Leopard       = Mac_OS_X_10_6  ;
	const info_data Mac_OS_X_Lion               = Mac_OS_X_10_7  ;
	const info_data     OS_X_Mountain_Lion      =     OS_X_10_8  ;
	const info_data     OS_X_Mavericks          =     OS_X_10_9  ;
	const info_data     OS_X_Yosemite           =     OS_X_10_10 ;
	const info_data     OS_X_El_Capitan         =     OS_X_10_11 ;
	const info_data    macOS_Sierra             =    macOS_10_12 ;
	const info_data    macOS_High_Sierra        =    macOS_10_13 ;
	const info_data    macOS_Mojave             =    macOS_10_14 ;
	const info_data    macOS_Catalina           =    macOS_10_15 ;
	const info_data    macOS_Big_Sur            =    macOS_11    ;
	const info_data iPadOS_13_1                 = iOS_13_1       ;
	const info_data iPadOS_13_2                 = iOS_13_2       ;
	const info_data iPadOS_13_3                 = iOS_13_3       ;
	const info_data iPadOS_13_4                 = iOS_13_4       ;
	const info_data iPadOS_13_5                 = iOS_13_5       ;
	const info_data iPadOS_13_6                 = iOS_13_6       ;
	const info_data iPadOS_13_7                 = iOS_13_7       ;
	const info_data iPadOS_14_0                 = iOS_14_0       ;
	const info_data iPadOS_14_1                 = iOS_14_1       ;
	const info_data iPadOS_14_2                 = iOS_14_2       ;
	const info_data iPadOS_14_3                 = iOS_14_3       ;
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif