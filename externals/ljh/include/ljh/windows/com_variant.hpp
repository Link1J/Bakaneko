
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// com_variant.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// Requires winrt
// 
// ABOUT
//     A wrapper for VARIANT
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#define NOMINMAX
#include <OAIdl.h>
#include <stdint.h>
#include <winrt/base.h>
#include <ljh/type_traits.hpp>
#include <ljh/string_utils.hpp>
#include <ljh/windows/com_bstr.hpp>
#include <ljh/windows/com_safe_array.hpp>

namespace ljh::windows
{
	struct com_variant
	{
		com_variant() noexcept
		{
			VariantInit(&_i_data);
		}

		~com_variant() noexcept
		{
			VariantClear(&_i_data);
		}

		void init() noexcept
		{
			VariantInit(&_i_data);
		}

		void clear() noexcept
		{
			VariantClear(&_i_data);
		}

		com_variant(const com_variant& other) noexcept
		{
			init();
			VariantCopy(&_i_data, &other.data());
		}

		com_variant& operator=(const com_variant& other) noexcept
		{
			clear();
			VariantCopy(&_i_data, &other.data());
			return *this;
		}

		com_variant(com_variant&& other) noexcept
		{
			init();
			VariantCopy(&_i_data, &other.data());
			other.clear();
			other.init();
		}

		com_variant& operator=(com_variant&& other) noexcept
		{
			clear();
			VariantCopy(&_i_data, &other.data());
			other.clear();
			other.init();
			return *this;
		}

		VARIANT& data()
		{
			return _i_data;
		}

		const VARIANT& data() const
		{
			return _i_data;
		}

		template<typename T>
		bool contains()
		{
			return __contains<T>(V_VT(&_i_data));
		}

		template<typename T>
		auto get()
		{
			WINRT_ASSERT(contains<T>());
			if constexpr (std::is_same_v<T, bool>)
				return (T)(V_BOOL(&_i_data) == VARIANT_TRUE);
			else if constexpr (std::is_same_v<T, char>)
				return (T)(V_I1(&_i_data));
			else if constexpr (std::is_same_v<T, short>)
				return (T)(V_I2(&_i_data));
			else if constexpr (std::is_same_v<T, int>)
				return (T)(V_I4(&_i_data));
			else if constexpr (std::is_same_v<T, long long>)
				return (T)(V_I8(&_i_data));
			else if constexpr (std::is_same_v<T, unsigned char>)
				return (T)(V_UI1(&_i_data));
			else if constexpr (std::is_same_v<T, unsigned short>)
				return (T)(V_UI2(&_i_data));
			else if constexpr (std::is_same_v<T, unsigned int>)
				return (T)(V_UI4(&_i_data));
			else if constexpr (std::is_same_v<T, unsigned long long>)
				return (T)(V_UI8(&_i_data));
			else if constexpr (std::is_same_v<T, float>)
				return (T)(V_R4(&_i_data));
			else if constexpr (std::is_same_v<T, double>)
				return (T)(V_R8(&_i_data));
			else if constexpr (std::is_same_v<T, com_bstr> || std::is_same_v<T, BSTR>)
				return com_bstr(V_BSTR(&_i_data));
			else if constexpr (std::is_same_v<T, HRESULT>)
				return (T)(V_ERROR(&_i_data));
			else if constexpr (is_instance_v<T, com_safe_array>)
				return T(V_ARRAY(&_i_data));
			else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
				return T((char*)V_RECORD(&_i_data));
			else if constexpr (std::is_same_v<T, std::wstring> || std::is_same_v<T, std::wstring_view>)
				return T((wchar_t*)V_RECORD(&_i_data));
		}

		template<typename T>
		auto as()
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				     if (contains<char              >()) return std::to_string(get<char              >());
				else if (contains<short             >()) return std::to_string(get<short             >());
				else if (contains<int               >()) return std::to_string(get<int               >());
				else if (contains<long long         >()) return std::to_string(get<long long         >());
				else if (contains<unsigned char     >()) return std::to_string(get<unsigned char     >());
				else if (contains<unsigned short    >()) return std::to_string(get<unsigned short    >());
				else if (contains<unsigned int      >()) return std::to_string(get<unsigned int      >());
				else if (contains<unsigned long long>()) return std::to_string(get<unsigned long long>());
				else if (contains<float             >()) return std::to_string(get<float             >());
				else if (contains<double            >()) return std::to_string(get<double            >());
				else if (contains<bool              >()) return std::to_string(get<bool              >());
				else if (contains<std::string       >()) return                get<std::string       >() ;
				else if (contains<std::wstring      >()) return convert_string(get<std::wstring      >());
				else if (contains<com_bstr          >()) return convert_string(get<com_bstr          >());
			}
			else if constexpr (std::is_same_v<T, std::wstring>)
			{
				     if (contains<char              >()) return std::to_wstring(get<char              >());
				else if (contains<short             >()) return std::to_wstring(get<short             >());
				else if (contains<int               >()) return std::to_wstring(get<int               >());
				else if (contains<long long         >()) return std::to_wstring(get<long long         >());
				else if (contains<unsigned char     >()) return std::to_wstring(get<unsigned char     >());
				else if (contains<unsigned short    >()) return std::to_wstring(get<unsigned short    >());
				else if (contains<unsigned int      >()) return std::to_wstring(get<unsigned int      >());
				else if (contains<unsigned long long>()) return std::to_wstring(get<unsigned long long>());
				else if (contains<float             >()) return std::to_wstring(get<float             >());
				else if (contains<double            >()) return std::to_wstring(get<double            >());
				else if (contains<bool              >()) return std::to_wstring(get<bool              >());
				else if (contains<std::string       >()) return convert_string (get<std::string       >());
				else if (contains<std::wstring      >()) return                 get<std::wstring      >() ;
				else if (contains<com_bstr          >()) 
				{
					std::wstring temp = get<com_bstr>();
					return temp;
				}
			}
			else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>)
			{
				     if (contains<char              >()) return (T)(get<char              >());
				else if (contains<short             >()) return (T)(get<short             >());
				else if (contains<int               >()) return (T)(get<int               >());
				else if (contains<long long         >()) return (T)(get<long long         >());
				else if (contains<unsigned char     >()) return (T)(get<unsigned char     >());
				else if (contains<unsigned short    >()) return (T)(get<unsigned short    >());
				else if (contains<unsigned int      >()) return (T)(get<unsigned int      >());
				else if (contains<unsigned long long>()) return (T)(get<unsigned long long>());
				else if (contains<float             >()) return (T)(get<float             >());
				else if (contains<double            >()) return (T)(get<double            >());
				else if (contains<bool              >()) return (T)(get<bool              >());

				else if constexpr (std::is_same_v<T, long>)
				{
					     if (contains<std::string >()) return std::stol(get<std::string >());
					else if (contains<std::wstring>()) return std::stol(get<std::wstring>());
					else if (contains<com_bstr    >()) return std::stol(get<com_bstr    >());
				}
				else if constexpr (std::is_same_v<T, long long>)
				{
					     if (contains<std::string >()) return std::stoll(get<std::string >());
					else if (contains<std::wstring>()) return std::stoll(get<std::wstring>());
					else if (contains<com_bstr    >()) return std::stoll(get<com_bstr    >());
				}
				else if constexpr (std::is_same_v<T, unsigned long long>)
				{
					     if (contains<std::string >()) return std::stoull(get<std::string >());
					else if (contains<std::wstring>()) return std::stoull(get<std::wstring>());
					else if (contains<com_bstr    >()) return std::stoull(get<com_bstr    >());
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					     if (contains<std::string >()) return std::stof(get<std::string >());
					else if (contains<std::wstring>()) return std::stof(get<std::wstring>());
					else if (contains<com_bstr    >()) return std::stof(get<com_bstr    >());
				}
				else if constexpr (std::is_same_v<T, double>)
				{
					     if (contains<std::string >()) return std::stod(get<std::string >());
					else if (contains<std::wstring>()) return std::stod(get<std::wstring>());
					else if (contains<com_bstr    >()) return std::stod(get<com_bstr    >());
				}
				else if constexpr (std::is_same_v<T, long double>)
				{
					     if (contains<std::string >()) return std::stold(get<std::string >());
					else if (contains<std::wstring>()) return std::stold(get<std::wstring>());
					else if (contains<com_bstr    >()) return std::stold(get<com_bstr    >());
				}
				else if constexpr (std::is_unsigned_v<T>)
				{
					     if (contains<std::string >()) return (T)std::stoul(get<std::string >());
					else if (contains<std::wstring>()) return (T)std::stoul(get<std::wstring>());
					else if (contains<com_bstr    >()) return (T)std::stoul(get<com_bstr    >());
				}
				else if constexpr (std::is_signed_v<T>)
				{
					     if (contains<std::string >()) return (T)std::stoi(get<std::string >());
					else if (contains<std::wstring>()) return (T)std::stoi(get<std::wstring>());
					else if (contains<com_bstr    >()) return (T)std::stoi(get<com_bstr    >());
				}
			}
			else if constexpr (is_instance_v<T, com_safe_array>)
			{
				return get<T>();
			}
			else
			{
				static_assert(std::is_same_v<std::void_t<T>, int>, "Invalid Type");
			}
			WINRT_ASSERT(false);
		}

		bool empty()
		{
			return V_VT(&_i_data) == VT_EMPTY;
		}
		
		bool null()
		{
			return V_VT(&_i_data) == VT_NULL;
		}

		operator bool()
		{
			return !empty() && !null();
		}

	private:
		template<typename T>
		bool __contains(unsigned short vt)
		{
			if constexpr (std::is_same_v<T, bool>)
				return vt == VT_BOOL;
			else if constexpr (std::is_same_v<T, char>)
				return vt == VT_I1;
			else if constexpr (std::is_same_v<T, short>)
				return vt == VT_I2;
			else if constexpr (std::is_same_v<T, int>)
				return vt == VT_I4 || vt == VT_INT;
			else if constexpr (std::is_same_v<T, long long>)
				return vt == VT_I8;
			else if constexpr (std::is_same_v<T, unsigned char>)
				return vt == VT_UI1;
			else if constexpr (std::is_same_v<T, unsigned short>)
				return vt == VT_UI2;
			else if constexpr (std::is_same_v<T, unsigned int>)
				return vt == VT_UI4 || vt == VT_UINT;
			else if constexpr (std::is_same_v<T, unsigned long long>)
				return vt == VT_UI8;
			else if constexpr (std::is_same_v<T, float>)
				return vt == VT_R4;
			else if constexpr (std::is_same_v<T, double>)
				return vt == VT_R8;
			else if constexpr (std::is_same_v<T, com_bstr> || std::is_same_v<T, BSTR>)
				return vt == VT_BSTR;
			else if constexpr (std::is_same_v<T, HRESULT>)
				return vt == VT_ERROR;
			else if constexpr (is_instance_v<T, com_safe_array>)
				return (vt & ~VT_TYPEMASK) == VT_ARRAY && __contains<T::value_type>(vt & VT_TYPEMASK);
			else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
				return vt == VT_LPSTR;
			else if constexpr (std::is_same_v<T, std::wstring> || std::is_same_v<T, std::wstring_view>)
				return vt == VT_LPWSTR;
			else
				static_assert(std::is_same_v<std::void_t<T>, int>, "Unknown Type");
		}

		VARIANT _i_data;
	};
}

namespace std
{
	template<typename T>
	auto get(const ljh::windows::com_variant& variant)
	{
		return variant.get<T>();
	}
}