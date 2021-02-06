
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// dbus.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++20
// 
// ABOUT
//     A wrapper for dbus
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#include <dbus/dbus.h>

#include <string>
#include <vector>
#include <variant>
#include <tuple>
#include <map>
#include <stdexcept>

#include <cstring>

#include <ljh/type_traits.hpp>
#include <ljh/casting.hpp>
#include <ljh/compile_time_string.hpp>

#undef unix

namespace ljh::unix::dbus
{
	struct expand_type {
		template<typename... T>
		expand_type(T&&...) {}
	};

	template<template<int> class W, std::size_t... I, typename...A>
	void caller_impl(std::index_sequence<I...>, A&... args) {
		int t[] = { 0, ((void)W<I>()(args...), 1)... };
		(void)t;
	}
	
	template<template<int> class W, std::size_t N, typename Indices = std::make_index_sequence<N>, typename...A>
	void call_times(A&... args) {
		caller_impl<W>(Indices(), args...);
	}

	class object_path : public std::string {};

	template<typename T>
	constexpr auto _i_type_value_f()
	{
		if constexpr (ljh::is_instance_v<T, std::vector>)
			return DBUS_TYPE_ARRAY;
		else if constexpr (ljh::is_instance_v<T, std::variant>)
			return DBUS_TYPE_VARIANT;
		else if constexpr (std::is_same_v<std::string, T>)
			return DBUS_TYPE_STRING;
		else if constexpr (std::is_same_v<int32_t, T>)
			return DBUS_TYPE_INT32;
		else if constexpr (std::is_same_v<uint32_t, T>)
			return DBUS_TYPE_UINT32;
		else if constexpr (std::is_same_v<int16_t, T>)
			return DBUS_TYPE_INT16;
		else if constexpr (std::is_same_v<uint16_t, T>)
			return DBUS_TYPE_UINT16;
		else if constexpr (std::is_same_v<int64_t, T>)
			return DBUS_TYPE_INT64;
		else if constexpr (std::is_same_v<uint64_t, T>)
			return DBUS_TYPE_UINT64;
		else if constexpr (std::is_same_v<uint8_t, T>)
			return DBUS_TYPE_BYTE;
		else if constexpr (std::is_same_v<object_path, T>)
			return DBUS_TYPE_OBJECT_PATH;
		else if constexpr (std::is_same_v<double, T>)
			return DBUS_TYPE_DOUBLE;
		else if constexpr (std::is_same_v<const char*, T>)
			return DBUS_TYPE_STRING;
		else if constexpr (ljh::is_instance_v<T, std::tuple>)
			return DBUS_TYPE_STRUCT;
		else if constexpr (ljh::is_instance_v<T, std::map>)
			return DBUS_TYPE_ARRAY;
		else
			return 0.1;
	}

	template<typename T>
	constexpr bool _i_type_fixed_array_f()
	{
		switch (_i_type_value_f<T>())
		{
		case DBUS_TYPE_BYTE:
		case DBUS_TYPE_BOOLEAN:
		case DBUS_TYPE_INT16:
		case DBUS_TYPE_UINT16:
		case DBUS_TYPE_INT32:
		case DBUS_TYPE_UINT32:
		case DBUS_TYPE_INT64:
		case DBUS_TYPE_UINT64:
		case DBUS_TYPE_DOUBLE:
			return true;
		
		default:
			return false;
		}
	}
	
	template <typename, template <typename...> typename>
	struct _i_gen_sig_s {};

	template<typename T>
	constexpr auto _i_gen_sig_f()
	{
		if constexpr (ljh::is_instance_v<T, std::vector>)
			return compile_time_string{DBUS_TYPE_ARRAY_AS_STRING} + _i_gen_sig_f<T::value_type>();
		else if constexpr (ljh::is_instance_v<T, std::variant>)
			return compile_time_string{DBUS_TYPE_VARIANT_AS_STRING};
		else if constexpr (std::is_same_v<std::string, T>)
			return compile_time_string{DBUS_TYPE_STRING_AS_STRING};
		else if constexpr (std::is_same_v<int32_t, T>)
			return compile_time_string{DBUS_TYPE_INT32_AS_STRING};
		else if constexpr (std::is_same_v<uint32_t, T>)
			return compile_time_string{DBUS_TYPE_UINT32_AS_STRING};
		else if constexpr (std::is_same_v<int16_t, T>)
			return compile_time_string{DBUS_TYPE_INT16_AS_STRING};
		else if constexpr (std::is_same_v<uint16_t, T>)
			return compile_time_string{DBUS_TYPE_UINT16_AS_STRING};
		else if constexpr (std::is_same_v<int64_t, T>)
			return compile_time_string{DBUS_TYPE_INT64_AS_STRING};
		else if constexpr (std::is_same_v<uint64_t, T>)
			return compile_time_string{DBUS_TYPE_UINT64_AS_STRING};
		else if constexpr (std::is_same_v<uint8_t, T>)
			return compile_time_string{DBUS_TYPE_BYTE_AS_STRING};
		else if constexpr (std::is_same_v<object_path, T>)
			return compile_time_string{DBUS_TYPE_OBJECT_PATH_AS_STRING};
		else if constexpr (std::is_same_v<double, T>)
			return compile_time_string{DBUS_TYPE_DOUBLE_AS_STRING};
		else if constexpr (std::is_same_v<const char*, T>)
			return compile_time_string{DBUS_TYPE_STRING_AS_STRING};
		else if constexpr (ljh::is_instance_v<T, std::tuple>)
			return compile_time_string{DBUS_STRUCT_BEGIN_CHAR_AS_STRING} + _i_gen_sig_s<T, std::tuple>()() + compile_time_string{DBUS_STRUCT_END_CHAR_AS_STRING};
		else if constexpr (ljh::is_instance_v<T, std::map>)
			return compile_time_string{DBUS_TYPE_ARRAY_AS_STRING} + compile_time_string{DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING} + _i_gen_sig_f<typename T::key_type>() + _i_gen_sig_f<typename T::mapped_type>() + compile_time_string{DBUS_DICT_ENTRY_END_CHAR_AS_STRING};
	}

	template <template <typename...> typename U, typename...Ts>
	struct _i_gen_sig_s<U<Ts...>, U> {
		constexpr auto operator()() { return (_i_gen_sig_f<std::decay_t<Ts>>() + ...); }
	};

	template<typename T> constexpr auto _i_type_value       = _i_type_value_f      <std::decay_t<T>>();
	template<typename T> constexpr auto _i_type_fixed_array = _i_type_fixed_array_f<std::decay_t<T>>();
	template<typename T> constexpr auto gen_sig             = _i_gen_sig_f         <std::decay_t<T>>();

	class connection;
	class error;
	class interface;
	class message;

	enum class bus
	{
		SESSION = DBUS_BUS_SESSION,
		SYSTEM  = DBUS_BUS_SYSTEM
	};

	class connection
	{
		DBusConnection* data = nullptr;

	public:
		connection() = default;

		connection(bus type);
		~connection();

		connection(const connection& other);
		connection& operator=(const connection& other);

		connection(connection&& other);
		connection& operator=(connection&& other);

		operator bool() const;
		operator DBusConnection*() const;

		interface get(std::string server, std::string path, std::string interface);
	};

	class interface
	{
		friend class connection;
		friend class message;

		connection _connection;
		std::string path;
		std::string server;
		std::string _interface;

		interface(const connection& _connection, std::string server, std::string path, std::string interface);
		message get(std::string property);

	public:
		interface() = default;

		message call(std::string method);

		template<typename T>
		T get(std::string property);
	};

	class error
	{
		DBusError data;

	public:
		error();
		~error();

		error(error&& other);
		error& operator=(error&& other);

		std::string name() const;
		std::string message() const;

		operator bool() const;
		explicit operator DBusError*();
	};

	class message 
	{
		friend interface;
		const interface& base;
		DBusMessage* _message = nullptr;

		template<int i>
		struct wrapper_v {
			template<typename T>
			void operator()(message& _this, DBusMessageIter& item, T& data) const 
			{
				char cur = dbus_message_iter_get_arg_type(&item);
				char req = _i_type_value<std::variant_alternative_t<i, T>>;
				if (cur == req)
				{
					auto& temp = std::get<i>(data);
					_this.get(item, temp);
				}
			}
		};

		template<int i>
		struct wrapper_t {
			template<typename T>
			void operator()(message& _this, DBusMessageIter& item, T& data) const 
			{
				char cur = dbus_message_iter_get_arg_type(&item);
				char req = _i_type_value<std::tuple_element_t<i, T>>;
				if (cur == req)
				{
					auto& temp = std::get<i>(data);
					_this.get(item, temp);
				}
			}
		};

		message(const interface& base, std::string interface, std::string method);

		template<typename T>
		void add(DBusMessageIter& item, T&& data)
		{
			using type = std::decay_t<T>;
			static_assert(std::is_same_v<decltype(_i_type_value<T>), decltype(_i_type_value<int>)>, "Unknown type");
			if constexpr (std::is_fundamental_v<T>)
			{
				dbus_message_iter_append_basic(&item, _i_type_value<T>, &data);
			}
			else if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, object_path>)
			{
				auto temp = data.data();
				dbus_message_iter_append_basic(&item, _i_type_value<T>, &temp);
			}
			else if constexpr (std::is_same_v<std::decay_t<T>, const char*>)
			{
				std::decay_t<T> temp = data;
				dbus_message_iter_append_basic(&item, _i_type_value<T>, &temp);
			}
			else if constexpr (ljh::is_bounded_array_v<std::remove_reference_t<T>>)
			{
				std::decay_t<T> temp = data;
				dbus_message_iter_append_fixed_array(&item, _i_type_value<T>, &temp, std::size(data));
			}
			else if constexpr (ljh::is_instance_v<T, std::vector>)
			{
				DBusMessageIter sub;
				dbus_message_iter_open_container(&item, _i_type_value<T>, gen_sig<typename T::value_type>.data(), &sub);
				for (int a = 0; a < data.size(); add(sub, data[a++]));
				dbus_message_iter_close_container(&item, &sub);
			}
			else if constexpr (ljh::is_instance_v<T, std::variant>)
			{
				DBusMessageIter sub;
				dbus_message_iter_open_container(&item, _i_type_value<T>, std::visit([](auto&& a){ return gen_sig<decltype(a)>.data(); }, data), &sub);
				std::visit([this, &sub](auto&& a){ add(sub, std::forward<decltype(a)>(a)); }, data);
				dbus_message_iter_close_container(&item, &sub);
			}
			else if constexpr (ljh::is_instance_v<T, std::tuple>)
			{
				DBusMessageIter sub;
				dbus_message_iter_open_container(&item, _i_type_value<T>, nullptr, &sub);
				std::apply([this, &sub](auto&&... args){ expand_type{(add(sub, std::forward<decltype(args)>(args)), 0)...}; }, data);
				dbus_message_iter_close_container(&item, &sub);
			}
			else if constexpr (ljh::is_instance_v<T, std::map>)
			{
				DBusMessageIter sub;
				dbus_message_iter_open_container(&item, DBUS_TYPE_ARRAY, gen_sig<T>.data() + 1, &sub);
				for (auto& [key, value] : data)
				{ 
					DBusMessageIter member;
					dbus_message_iter_open_container(&sub, DBUS_TYPE_DICT_ENTRY, NULL, &member);
					add(member, key); add(member, value);
					dbus_message_iter_close_container(&sub, &member);
				}
				dbus_message_iter_close_container(&item, &sub);
			}
			else
			{
#if !defined(LJH_COMPILER_CLANG)
				static_assert(std::is_same_v<std::void_t<T>, int>, "Can't handle type");
#endif
			}
		}

		template<typename T>
		bool get(DBusMessageIter& item, T& data);

		error do_call()
		{
			error pending;
			auto temp = dbus_connection_send_with_reply_and_block(base._connection, _message, DBUS_TIMEOUT_INFINITE, (DBusError*)pending);
			dbus_message_unref(_message);
			_message = temp;
			return pending;
		}

	public:
		~message()
		{
			if (_message != nullptr)
				dbus_message_unref(_message);
		}

		std::string signature() const
		{
			return dbus_message_get_signature(_message);
		}
		
		template<typename... A>
		message& args(A&&... args)
		{
			DBusMessageIter iter;
			dbus_message_iter_init_append(_message, &iter);
			expand_type{(add(iter, std::forward<A>(args)), 0)...};
			return *this;
		}

		template<typename... A>
		void run(A&... args)
		{
			auto pending = do_call();
			if (pending) throw pending;

			DBusMessageIter iter;
			dbus_message_iter_init(_message, &iter);
			expand_type{(get(iter, args), 0)...};
		}

		template<typename A = void>
		A run()
		{
			if constexpr (std::is_void_v<A>)
			{
				do_call();
			}
			else
			{
				A data;
				run(data);
				return data;
			}
		}
	};


	template<typename T>
	T interface::get(std::string property)
	{
		return std::get<0>(get(property).run<std::variant<T>>());
	}

	template<typename T>
	bool message::get(DBusMessageIter& item, T& data)
	{
		static_assert(std::is_same_v<std::decay_t<decltype(_i_type_value<T>)>, int>, "Unknown type");
		if (dbus_message_iter_get_arg_type(&item) != _i_type_value<T>)
		{
			char cur = (char)dbus_message_iter_get_arg_type(&item);
			char req = (char)_i_type_value<T>;
			throw std::runtime_error(std::string{"Type ("} + cur + ") does not equal requested type (" + req + ")");
		}
		if constexpr (std::is_fundamental_v<T>)
		{
			dbus_message_iter_get_basic(&item, &data);
		}
		else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, object_path>)
		{
			const char* temp;
			dbus_message_iter_get_basic(&item, &temp);
			data = temp;
		}
		else if constexpr (ljh::is_instance_v<T, std::vector>)
		{
			DBusMessageIter sub;
			dbus_message_iter_recurse(&item, &sub);
			while (get(sub, data.emplace_back()));
		}
		else if constexpr (ljh::is_instance_v<T, std::variant>)
		{
			DBusMessageIter sub;
			dbus_message_iter_recurse(&item, &sub);
			call_times<wrapper_v, std::variant_size_v<T>>(*this, sub, data);
		}
		else if constexpr (ljh::is_instance_v<T, std::tuple>)
		{
			DBusMessageIter sub;
			dbus_message_iter_recurse(&item, &sub);
			call_times<wrapper_t, std::tuple_size_v<T>>(*this, sub, data);
		}
		else if constexpr (ljh::is_instance_v<T, std::map>)
		{
			DBusMessageIter sub;
			dbus_message_iter_recurse(&item, &sub);
			do
			{
				DBusMessageIter member;
				dbus_message_iter_recurse(&sub, &member);

				typename T::key_type key;
				typename T::mapped_type value;

				get(member, key);
				get(member, value);

				data[key] = value;
			}
			while (dbus_message_iter_next(&sub));
		}
		else
		{
#if !defined(LJH_COMPILER_CLANG)
			static_assert(std::is_same_v<std::void_t<T>, int>, "Can't handle type");
#endif
		}
		return dbus_message_iter_next(&item);
	}
}