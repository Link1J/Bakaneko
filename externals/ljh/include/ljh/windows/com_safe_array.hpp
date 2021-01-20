
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// com_safe_array.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// Requires winrt
// 
// ABOUT
//     A wrapper for SAFEARRAY
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#define NOMINMAX
#include <OAIdl.h>
#include <winrt/base.h>

namespace ljh::windows
{
	template<typename T>
	struct com_safe_array
	{
		using value_type = T;
		using size_type = uint32_t;
		using reference = value_type&;
		using const_reference = value_type const&;
		using pointer = value_type*;
		using const_pointer = value_type const*;
		using iterator = value_type*;
		using const_iterator = value_type const*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		com_safe_array() {}

		~com_safe_array()
		{
			if (array)
				winrt::check_hresult(SafeArrayDestroy(array));
		}
		
		com_safe_array(const com_safe_array& other)
		{
			winrt::check_hresult(SafeArrayCopy(other.array, &array));
		}

		com_safe_array& operator=(const com_safe_array& other)
		{
			if (array)
				winrt::check_hresult(SafeArrayDestroy(array));
			
			winrt::check_hresult(SafeArrayCopy(other.array, &array));
		}

		com_safe_array(com_safe_array&& other)
		{
			array = other.array;
			other.array = nullptr;
		}

		com_safe_array& operator=(com_safe_array&& other)
		{
			if (array)
				winrt::check_hresult(SafeArrayDestroy(array));

			array = other.array;
			other.array = nullptr;
		}
		
		reference operator[](size_type const pos) noexcept
		{
			WINRT_ASSERT(pos < size());
			return ((value_type*)array->pvData)[pos];
		}

		const_reference operator[](size_type const pos) const noexcept
		{
			WINRT_ASSERT(pos < size());
			return ((value_type*)array->pvData)[pos];
		}

		reference at(size_type const pos)
		{
			if (size() <= pos)
			{
				throw std::out_of_range("Invalid array subscript");
			}

			return ((value_type*)array->pvData)[pos];
		}

		const_reference at(size_type const pos) const
		{
			if (size() <= pos)
			{
				throw std::out_of_range("Invalid array subscript");
			}

			return ((value_type*)array->pvData)[pos];
		}

		reference front() noexcept
		{
			WINRT_ASSERT(size() > 0);
			return *((value_type*)array->pvData);
		}

		const_reference front() const noexcept
		{
			WINRT_ASSERT(size() > 0);
			return *((value_type*)array->pvData);
		}

		reference back() noexcept
		{
			WINRT_ASSERT(size() > 0);
			return ((value_type*)array->pvData)[size() - 1];
		}

		const_reference back() const noexcept
		{
			WINRT_ASSERT(size() > 0);
			return ((value_type*)array->pvData)[size() - 1];
		}

		pointer data() noexcept
		{
			return ((value_type*)array->pvData);
		}

		const_pointer data() const noexcept
		{
			return ((value_type*)array->pvData);
		}

		iterator begin() noexcept
		{
			return ((value_type*)array->pvData);
		}

		const_iterator begin() const noexcept
		{
			return ((value_type*)array->pvData);
		}

		const_iterator cbegin() const noexcept
		{
			return ((value_type*)array->pvData);
		}

		iterator end() noexcept
		{
			return ((value_type*)array->pvData) + array->rgsabound->cElements;
		}

		const_iterator end() const noexcept
		{
			return ((value_type*)array->pvData) + array->rgsabound->cElements;
		}

		const_iterator cend() const noexcept
		{
			return ((value_type*)array->pvData) + array->rgsabound->cElements;
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
			return array->rgsabound->cElements == 0;
		}

		size_type size() const noexcept
		{
			return array->rgsabound->cElements;
		}

		SAFEARRAY* get() const noexcept
		{
			return array;
		}

		SAFEARRAY** put() noexcept
		{
			WINRT_ASSERT(array == nullptr);
			return &array;
		}

	private:
		SAFEARRAY* array = nullptr;
	};
}