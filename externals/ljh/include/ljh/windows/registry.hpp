
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// UTF-8.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires Windows SDK
// 
// ABOUT
//     Sets the Windows Console codepage to UTF-8
//
// USAGE
//
// Version History
//     1.0 Inital Version

#include "../expected.hpp"
#include "../int_types.hpp"
#include <windows.h>
#include <winternl.h>
#include <string>
#include <vector>
#include <cstddef>

namespace ljh::windows::registry
{
	class key;
	class value;
	template<typename T> class iterator;
}

namespace ljh::windows::_registry
{
	class _values_container
	{
		const registry::key& hkey;
		DWORD max;

	public:
		using iterator               = registry::iterator<registry::value>;
		using const_iterator         = registry::iterator<const registry::value>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		_values_container(const registry::key& hkey);
		
		iterator               begin  ()      ;
		const_iterator         begin  () const;
		const_iterator         cbegin () const;
		iterator               end    ()      ;
		const_iterator         end    () const;
		const_iterator         cend   () const;
		reverse_iterator       rbegin ()      ;
		const_reverse_iterator rbegin () const;
		const_reverse_iterator crbegin() const;
		reverse_iterator       rend   ()      ;
		const_reverse_iterator rend   () const;
		const_reverse_iterator crend  () const;
	};

	template<typename T> struct _value_convert     : std::false_type {};
	template<> struct _value_convert<DWORD       > : std::true_type  { const DWORD type = REG_DWORD; };
	template<> struct _value_convert<ljh::u32    > : std::true_type  { const DWORD type = REG_DWORD; };
	template<> struct _value_convert<ljh::u64    > : std::true_type  { const DWORD type = REG_QWORD; };
	template<> struct _value_convert<std::wstring> : std::true_type  { const DWORD type = REG_SZ   ; };
}

namespace ljh::windows::registry
{
	class value
	{
	public:
		enum class format : DWORD
		{
			NONE                       = REG_NONE                      ,
			SZ                         = REG_SZ                        ,
			EXPAND_SZ                  = REG_EXPAND_SZ                 ,
			BINARY                     = REG_BINARY                    ,
			DWORD                      = REG_DWORD                     ,
			DWORD_LITTLE_ENDIAN        = REG_DWORD_LITTLE_ENDIAN       ,
			DWORD_BIG_ENDIAN           = REG_DWORD_BIG_ENDIAN          ,
			LINK                       = REG_LINK                      ,
			MULTI_SZ                   = REG_MULTI_SZ                  ,
			RESOURCE_LIST              = REG_RESOURCE_LIST             ,
			FULL_RESOURCE_DESCRIPTOR   = REG_FULL_RESOURCE_DESCRIPTOR  ,
			RESOURCE_REQUIREMENTS_LIST = REG_RESOURCE_REQUIREMENTS_LIST,
			QWORD                      = REG_QWORD                     ,
			QWORD_LITTLE_ENDIAN        = REG_QWORD_LITTLE_ENDIAN       ,
		};

	private:
		std::reference_wrapper<const key> hkey;
		std::wstring _name;
		format _type;

	public:
		value();
		value(const key& hkey, std::wstring name);
		~value() = default;
		value(const value& other);
		value(value&& other);
		value& operator=(const value& other);
		value& operator=(value&& other);

		template<typename T> std::enable_if_t<_registry::_value_convert<std::decay_t<T>>::value, T> get() const;
		template<typename T> std::enable_if_t<_registry::_value_convert<std::decay_t<T>>::value> set(T);
		template<typename T, bool = _registry::_value_convert<std::decay_t<T>>::value> explicit operator T() const { return get<T>(); }
		template<typename T, bool = _registry::_value_convert<std::decay_t<T>>::value> value& operator=(T other) { set<T>(other); return *this; }

		template<> DWORD        get<DWORD       >() const;
		template<> ljh::u32     get<ljh::u32    >() const;
		template<> ljh::u64     get<ljh::u64    >() const;
		template<> std::wstring get<std::wstring>() const;

		template<> void set<DWORD       >(DWORD       );
		template<> void set<ljh::u32    >(ljh::u32    );
		template<> void set<ljh::u64    >(ljh::u64    );
		template<> void set<std::wstring>(std::wstring);

		std::wstring name() const;
		std::wstring full_name() const;
		format type() const;
	};

	class key
	{
		HKEY hkey = (HKEY)-1;
		DWORD max = 0;

		std::wstring _i_name() const;

	public:
		using iterator               = windows::registry::iterator<key>;
		using const_iterator         = windows::registry::iterator<const key>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		
		key();
		key(HKEY hkey);
		~key();
		key(const key& other);
		key(key&& other);
		key& operator=(const key& other);
		key& operator=(key&& other);

		operator HKEY() const;

		iterator               begin  ()      ;
		const_iterator         begin  () const;
		const_iterator         cbegin () const;
		iterator               end    ()      ;
		const_iterator         end    () const;
		const_iterator         cend   () const;
		reverse_iterator       rbegin ()      ;
		const_reverse_iterator rbegin () const;
		const_reverse_iterator crbegin() const;
		reverse_iterator       rend   ()      ;
		const_reverse_iterator rend   () const;
		const_reverse_iterator crend  () const;
		
		std::wstring name() const;
		std::wstring full_name() const;
		_registry::_values_container values();
		const _registry::_values_container values() const;

		ljh::expected<key        , LSTATUS> get_key  (std::wstring subkey_name)      ;
		ljh::expected<const key  , LSTATUS> get_key  (std::wstring subkey_name) const;
		ljh::expected<value      , LSTATUS> get_value(std::wstring value_name )      ;
		ljh::expected<const value, LSTATUS> get_value(std::wstring value_name ) const;

		key         operator[](std::wstring subkey_name)      ;
		const key   operator[](std::wstring subkey_name) const;
		value       operator()(std::wstring value_name )      ;
		const value operator()(std::wstring value_name ) const;

		static       key CLASSES_ROOT               ;
		static       key CURRENT_USER               ;
		static const key LOCAL_MACHINE              ;
		static       key USERS                      ;
		static       key PERFORMANCE_DATA           ;
		static       key PERFORMANCE_TEXT           ;
		static       key PERFORMANCE_NLSTEXT        ;
		static       key CURRENT_CONFIG             ;
		static       key DYN_DATA                   ;
		static       key CURRENT_USER_LOCAL_SETTINGS;
	};

	template<typename T>
	class iterator
	{
		const key& from;
		DWORD index;
		typename std::remove_cv<T>::type thing;

		void update();

	public:
		using difference_type   = DWORD;
		using value_type        = T;
		using pointer           = value_type*;
		using reference         = value_type&;
		using iterator_category = std::bidirectional_iterator_tag;

		iterator(const key& from, DWORD index)
			: from(from)
			, index(index)
		{
			update();
		}

		iterator& operator++()
		{
			++index;
			update();
			return *this;
		}

		iterator operator++(int)
		{
			iterator retval{*this};
			++(*this);
			return retval;
		}

		iterator& operator--()
		{
			--index;
			update();
			return *this;
		}

		iterator operator--(int)
		{
			iterator retval{*this};
			--(*this);
			return retval;
		}

		bool operator==(iterator other) const
		{
			return index == other.index;
		}

		bool operator!=(iterator other) const
		{
			return !(*this == other);
		}

		reference operator*()
		{
			return thing;
		}
		
		pointer operator->()
		{
			return &thing;
		}
	};
}