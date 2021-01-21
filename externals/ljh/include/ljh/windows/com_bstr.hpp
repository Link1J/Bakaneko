
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// com_bstr.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// Requires winrt
// 
// ABOUT
//     A wrapper for BSTR
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#define NOMINMAX
#include <OAIdl.h>
#include <stdint.h>
#include <iterator>
#include <winrt/base.h>
#include <string>
#include <string_view>
#include <ostream>

namespace ljh::windows
{
	struct com_bstr
	{
		using value_type = OLECHAR;
		using size_type = uint32_t;
		using const_reference = value_type const&;
		using pointer = value_type*;
		using iterator = pointer;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_pointer = value_type const*;
		using const_iterator = const_pointer;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		using bstr = value_type*;

		com_bstr() noexcept = default;

		com_bstr(const wchar_t* string) noexcept
		{
			this->string = SysAllocString(string);
		}

		explicit com_bstr(const std::wstring_view& string) noexcept
		{
			this->string = SysAllocStringLen(string.data(), string.size());
		}

		~com_bstr() noexcept
		{
			SysFreeString(string);
		}

		void clear() noexcept
		{
			SysFreeString(string);
		}

		operator std::basic_string_view<value_type>() const noexcept
		{
			return std::basic_string_view<value_type>(string, size());
		}

		operator std::basic_string<value_type>() const noexcept
		{
			return std::basic_string<value_type>(string, size());
		}

		bool operator==(const std::basic_string_view<value_type>& rhs) const noexcept
		{
			return std::basic_string_view<value_type>(string, size()) == rhs;
		}

		const_reference operator[](size_type pos) const noexcept
		{
			WINRT_ASSERT(pos < size());
			return *(begin() + pos);
		}

		const_reference front() const noexcept
		{
			WINRT_ASSERT(!empty());
			return *begin();
		}

		const_reference back() const noexcept
		{
			assert(!empty());
			return *(end() - 1);
		}

		pointer data() noexcept
		{
			return begin();
		}

		const_pointer c_str() const noexcept
		{
			return begin();
		}

		iterator begin() noexcept
		{
			return string;
		}

		const_iterator begin() const noexcept
		{
			return string;
		}

		const_iterator cbegin() const noexcept
		{
			return begin();
		}

		iterator end() noexcept
		{
			return string + size();
		}

		const_iterator end() const noexcept
		{
			return string + size();
		}

		const_iterator cend() const noexcept
		{
			return end();
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(begin());
		}

		const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator(begin());
		}

		const_reverse_iterator crend() const noexcept
		{
			return rend();
		}

		bool empty() const noexcept
		{
			return size() == 0;
		}

		size_type size() const noexcept
		{
			return SysStringLen(string);
		}
		
		bstr* put() noexcept
		{
			WINRT_ASSERT(string != nullptr);
			return &string;
		}

		operator BSTR() noexcept
		{
			return string;
		}

		operator const BSTR() const noexcept
		{
			return string;
		}

	private:
		bstr string;
	};

	inline namespace com_bstr_literals
	{
		inline auto operator "" _bstr(const wchar_t* string, std::size_t size)
		{
			return com_bstr{std::wstring_view{string, size}};
		}
	}
}

inline std::wostream& operator<<(std::wostream& os, const ljh::windows::com_bstr& string)
{
	return os << string.c_str();
}