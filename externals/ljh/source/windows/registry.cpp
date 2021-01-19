
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ljh/windows/registry.hpp"
#include "ljh/function_pointer.hpp"
#include "ljh/casting.hpp"
#include <winternl.h>
#include <winerror.h>
#pragma comment(lib, "Advapi32.lib")

namespace ljh::windows::registry
{
	enum KEY_INFORMATION_CLASS
	{
		KeyBasicInformation,
		KeyNodeInformation,
		KeyFullInformation,
		KeyNameInformation,
		KeyCachedInformation,
		KeyFlagsInformation,
		KeyVirtualizationInformation,
		KeyHandleTagsInformation,
		KeyTrustInformation,
		KeyLayerInformation,
		MaxKeyInfoClass
	};

	static ljh::function_pointer<NTSTATUS WINAPI(HKEY, KEY_INFORMATION_CLASS, PVOID, ULONG, PULONG)> NtQueryKey;

	key::key() {}

	key::key(HKEY hkey)
		: hkey(hkey)
	{
		RegQueryInfoKey(hkey, NULL, NULL, NULL, &max, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	key::~key()
	{
		RegCloseKey(hkey);
	}

	key::key(const key& other)
	{
		DuplicateHandle(GetCurrentProcess(), other.hkey, GetCurrentProcess(), (LPHANDLE)&hkey, 0, FALSE, DUPLICATE_SAME_ACCESS);
		max = other.max;
	}
	
	key::key(key&& other)
	{
		std::swap(hkey, other.hkey);
		std::swap(max , other.max );
	}

	key& key::operator=(const key& other)
	{
		DuplicateHandle(GetCurrentProcess(), other.hkey, GetCurrentProcess(), (LPHANDLE)&hkey, 0, FALSE, DUPLICATE_SAME_ACCESS);
		return *this;
	}
	
	key& key::operator=(key&& other)
	{
		std::swap(hkey, other.hkey);
		std::swap(max , other.max );
		return *this;
	}

	key::operator HKEY() const
	{
		return hkey;
	}

	key::iterator               key::begin  ()       { return                            {*this,   0}; }
	key::const_iterator         key::begin  () const { return                            {*this,   0}; }
	key::const_iterator         key::cbegin () const { return                            {*this,   0}; }
	key::iterator               key::end    ()       { return                            {*this, max}; }
	key::const_iterator         key::end    () const { return                            {*this, max}; }
	key::const_iterator         key::cend   () const { return                            {*this, max}; }
	key::reverse_iterator       key::rbegin ()       { return key::reverse_iterator      {end   ()  }; }
	key::const_reverse_iterator key::rbegin () const { return key::const_reverse_iterator{end   ()  }; }
	key::const_reverse_iterator key::crbegin() const { return key::const_reverse_iterator{cend  ()  }; }
	key::reverse_iterator       key::rend   ()       { return key::reverse_iterator      {begin ()  }; }
	key::const_reverse_iterator key::rend   () const { return key::const_reverse_iterator{begin ()  }; }
	key::const_reverse_iterator key::crend  () const { return key::const_reverse_iterator{cbegin()  }; }
	
	std::wstring key::_i_name() const
	{
		if (!NtQueryKey)
		{
			NtQueryKey = GetProcAddress(LoadLibraryA("ntdll.dll"), "NtQueryKey");
		}

		DWORD size = 0;
		std::wstring _name;
		NtQueryKey(hkey, KeyNameInformation, 0, 0, &size);
		size = size + 2;
		_name.resize(size);
		NtQueryKey(hkey, KeyNameInformation, &_name[0], size, &size);
		return _name;
	}

	std::wstring key::name() const
	{
		auto _name = _i_name();
		_name = _name.substr(_name.find_last_of(L'\\') + 1);
		return _name;
	}
	
	std::wstring key::full_name() const
	{
		auto _name = _i_name();
		_name = _name.substr(2);
		return _name;
	}

	_registry::_values_container key::values()
	{
		return {*this};
	}

	const _registry::_values_container key::values() const
	{
		return {*this};
	}

	ljh::expected<key, LSTATUS> key::get_key(std::wstring subkey_name)
	{
		HKEY subkey;
		if (auto status = RegOpenKeyExW(hkey, subkey_name.c_str(), 0, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, &subkey); status != ERROR_SUCCESS)
		{
			return ljh::unexpected{status};
		}
		return subkey;
	}
	
	key key::operator[](std::wstring subkey_name)
	{
		return *get_key(subkey_name);
	}

	ljh::expected<const key, LSTATUS> key::get_key(std::wstring subkey_name) const
	{
		HKEY subkey;
		if (auto status = RegOpenKeyExW(hkey, subkey_name.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &subkey); status != ERROR_SUCCESS)
		{
			return ljh::unexpected{status};
		}
		return subkey;
	}
	
	const key key::operator[](std::wstring subkey_name) const
	{
		return *get_key(subkey_name);
	}

	ljh::expected<value, LSTATUS> key::get_value(std::wstring value_name)
	{
		DWORD size = 0;
		if (auto status = RegGetValueW(hkey, NULL, value_name.c_str(), RRF_RT_ANY, NULL, NULL, &size); status != ERROR_SUCCESS && status != ERROR_MORE_DATA)
		{
			return ljh::unexpected{status};
		}
		return value{*this, value_name};
	}
	
	value key::operator()(std::wstring value_name)
	{
		return *get_value(value_name);
	}

	ljh::expected<const value, LSTATUS> key::get_value(std::wstring value_name) const
	{
		DWORD size = 0;
		if (auto status = RegGetValueW(hkey, NULL, value_name.c_str(), RRF_RT_ANY, NULL, NULL, &size); status != ERROR_SUCCESS && status != ERROR_MORE_DATA)
		{
			return ljh::unexpected{status};
		}
		return value{*this, value_name};
	}
	
	const value key::operator()(std::wstring value_name) const
	{
		return *get_value(value_name);
	}

	value::value()
		: hkey(key::LOCAL_MACHINE)
		, _name(L"")
	{
	}

	value::value(const key& hkey, std::wstring name)
		: hkey (hkey)
		, _name(name)
	{
		RegGetValueW(hkey, NULL, name.data(), RRF_RT_ANY, (LPDWORD)&_type, NULL, NULL);
	}

	value::value(const value& other)
		: hkey (other.hkey )
		, _name(other._name)
		, _type(other._type)
	{
	}

	value::value(value&& other)
		: hkey(key::LOCAL_MACHINE)
	{
		std::swap(hkey , other.hkey );
		std::swap(_name, other._name);
		std::swap(_type, other._type);
	}

	value& value::operator=(const value& other)
	{
		hkey  = other.hkey ;
		_name = other._name;
		_type = other._type;
		return *this;
	}

	value& value::operator=(value&& other)
	{
		std::swap(hkey , other.hkey );
		std::swap(_name, other._name);
		std::swap(_type, other._type);
		return *this;
	}

	template<>
	DWORD value::get<DWORD>() const
	{
		DWORD data = 0;
		DWORD size = sizeof(data);
		RegGetValueW(hkey.get(), NULL, _name.data(), RRF_RT_REG_DWORD, NULL, (void*)&data, &size);
		return data;
	}
	
	template<>
	ljh::u32 value::get<ljh::u32>() const
	{
		ljh::u32 data = 0;
		DWORD size = sizeof(data);
		RegGetValueW(hkey.get(), NULL, _name.data(), RRF_RT_REG_DWORD, NULL, (void*)&data, &size);
		return data;
	}

	template<>
	ljh::u64 value::get<ljh::u64>() const
	{
		ljh::u64 data= 0;
		DWORD size = sizeof(data);
		RegGetValueW(hkey.get(), NULL, _name.data(), RRF_RT_REG_QWORD, NULL, (void*)&data, &size);
		return data;
	}

	template<>
	std::wstring value::get<std::wstring>() const
	{
		std::wstring data;
		DWORD size = 0;
		RegGetValueW(hkey.get(), NULL, _name.data(), RRF_RT_REG_SZ, NULL, NULL, &size);
		data.resize(size / sizeof(std::wstring::value_type));
		RegGetValueW(hkey.get(), NULL, _name.data(), RRF_RT_REG_SZ, NULL, data.data(), &size);
		data.resize(size / sizeof(std::wstring::value_type) - 1);
		return data;
	}

	template<>
	void value::set<DWORD>(DWORD other)
	{
		RegSetValueExW(hkey.get(), _name.data(), 0, REG_DWORD, (const BYTE*)&other, sizeof(other));
	}

	template<>
	void value::set<ljh::u32>(ljh::u32 other)
	{
		RegSetValueExW(hkey.get(), _name.data(), 0, REG_DWORD, (const BYTE*)&other, sizeof(other));
	}

	template<>
	void value::set<ljh::u64>(ljh::u64 other)
	{
		RegSetValueExW(hkey.get(), _name.data(), 0, REG_QWORD, (const BYTE*)&other, sizeof(other));
	}

	template<>
	void value::set<std::wstring>(std::wstring other)
	{
		RegSetValueExW(hkey.get(), _name.data(), 0, REG_SZ, (const BYTE*)other.c_str(), (DWORD)other.size() * sizeof(std::wstring::value_type));
	}

	std::wstring value::name() const
	{
		return _name;
	}

	std::wstring value::full_name() const
	{
		return hkey.get().full_name() + L'\\' + _name;
	}
	
	constexpr int MAX_KEY_LENGTH =   255;
	constexpr int MAX_VALUE_NAME = 16383;

	template<>
	void iterator<key>::update()
	{
		HKEY subkey;
		wchar_t subkey_name[MAX_KEY_LENGTH];
		DWORD subkey_name_length = MAX_KEY_LENGTH;
		RegEnumKeyExW(from, index, subkey_name, &subkey_name_length, NULL, NULL, NULL, NULL);
		RegOpenKeyExW(from, subkey_name, 0, KEY_READ | KEY_WRITE, &subkey);
		thing = key{subkey};
	}

	template<>
	void iterator<const key>::update()
	{
		HKEY subkey;
		wchar_t subkey_name[MAX_KEY_LENGTH];
		DWORD subkey_name_length = MAX_KEY_LENGTH;
		RegEnumKeyExW(from, index, subkey_name, &subkey_name_length, NULL, NULL, NULL, NULL);
		RegOpenKeyExW(from, subkey_name, 0, KEY_READ | KEY_WRITE, &subkey);
		thing = key{subkey};
	}

	template<>
	void iterator<value>::update()
	{
		wchar_t subkey_name[MAX_VALUE_NAME];
		DWORD subkey_name_length = MAX_VALUE_NAME;
		RegEnumValueW(from, index, subkey_name, &subkey_name_length, NULL, NULL, NULL, NULL);
		thing = value{from, subkey_name};
	}

	template<>
	void iterator<const value>::update()
	{
		wchar_t subkey_name[MAX_VALUE_NAME];
		DWORD subkey_name_length = MAX_VALUE_NAME;
		RegEnumValueW(from, index, subkey_name, &subkey_name_length, NULL, NULL, NULL, NULL);
		thing = value{from, subkey_name};
	}

	      key key::CLASSES_ROOT                {HKEY_CLASSES_ROOT               };
	      key key::CURRENT_USER                {HKEY_CURRENT_USER               };
	const key key::LOCAL_MACHINE               {HKEY_LOCAL_MACHINE              };
	      key key::USERS                       {HKEY_USERS                      };
	      key key::PERFORMANCE_DATA            {HKEY_PERFORMANCE_DATA           };
	      key key::PERFORMANCE_TEXT            {HKEY_PERFORMANCE_TEXT           };
	      key key::PERFORMANCE_NLSTEXT         {HKEY_PERFORMANCE_NLSTEXT        };
	      key key::CURRENT_CONFIG              {HKEY_CURRENT_CONFIG             };
	      key key::DYN_DATA                    {HKEY_DYN_DATA                   };
	      key key::CURRENT_USER_LOCAL_SETTINGS {HKEY_CURRENT_USER_LOCAL_SETTINGS};
}

namespace ljh::windows::_registry
{
	_values_container::_values_container(const registry::key& hkey)
		: hkey(hkey)
	{
		RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL,  &max, NULL, NULL, NULL, NULL);
	}
		
	_values_container::iterator               _values_container::begin  ()       { return                                          {hkey,   0}; }
	_values_container::const_iterator         _values_container::begin  () const { return                                          {hkey,   0}; }
	_values_container::const_iterator         _values_container::cbegin () const { return                                          {hkey,   0}; }
	_values_container::iterator               _values_container::end    ()       { return                                          {hkey, max}; }
	_values_container::const_iterator         _values_container::end    () const { return                                          {hkey, max}; }
	_values_container::const_iterator         _values_container::cend   () const { return                                          {hkey, max}; }
	_values_container::reverse_iterator       _values_container::rbegin ()       { return _values_container::reverse_iterator      {end   () }; }
	_values_container::const_reverse_iterator _values_container::rbegin () const { return _values_container::const_reverse_iterator{end   () }; }
	_values_container::const_reverse_iterator _values_container::crbegin() const { return _values_container::const_reverse_iterator{cend  () }; }
	_values_container::reverse_iterator       _values_container::rend   ()       { return _values_container::reverse_iterator      {begin () }; }
	_values_container::const_reverse_iterator _values_container::rend   () const { return _values_container::const_reverse_iterator{begin () }; }
	_values_container::const_reverse_iterator _values_container::crend  () const { return _values_container::const_reverse_iterator{cbegin() }; }
}
