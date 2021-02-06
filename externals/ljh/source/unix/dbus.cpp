
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ljh/unix/dbus.hpp"


ljh::unix::dbus::error::error()
{
	dbus_error_init(&data);
}

ljh::unix::dbus::error::~error()
{
	if (*this)
		dbus_error_free(&data);
}

std::string ljh::unix::dbus::error::name() const
{
	return data.name;
}

std::string ljh::unix::dbus::error::message() const
{
	return data.message;
}

ljh::unix::dbus::error::operator bool() const
{
	return dbus_error_is_set(&data);
}

ljh::unix::dbus::error::error(error&& other)
{
	dbus_error_init(&data);
	if (other)
		dbus_move_error((DBusError*)other, &data);
}

ljh::unix::dbus::error& ljh::unix::dbus::error::operator=(error&& other)
{
	if (other)
		dbus_move_error((DBusError*)other, &data);
	return *this;
}

ljh::unix::dbus::error::operator DBusError*()
{
	return &data;
}

ljh::unix::dbus::connection::connection(bus type)
{
	error _error;
	data = dbus_bus_get(static_cast<DBusBusType>(type), (DBusError*)_error);
	if (_error) throw _error;
}

ljh::unix::dbus::connection::~connection()
{
	dbus_connection_unref(data);
	data = nullptr;
}

ljh::unix::dbus::connection::connection(const connection& other)
{
	data = other.data;
	dbus_connection_ref(data);
}

ljh::unix::dbus::connection& ljh::unix::dbus::connection::operator=(const connection& other)
{
	data = other.data;
	dbus_connection_ref(data);
	return *this;
}

ljh::unix::dbus::connection::connection(connection&& other)
{
	data = other.data;
	other.data = nullptr;
}

ljh::unix::dbus::connection& ljh::unix::dbus::connection::operator=(connection&& other)
{
	data = other.data;
	other.data = nullptr;
	return *this;
}

ljh::unix::dbus::connection::operator bool() const
{
	return data != nullptr;
}

ljh::unix::dbus::connection::operator DBusConnection*() const
{
	return data;
}

ljh::unix::dbus::interface ljh::unix::dbus::connection::get(std::string server, std::string path, std::string interface)
{
	return ljh::unix::dbus::interface{*this, server, path, interface};
}

ljh::unix::dbus::interface::interface(const connection& _connection, std::string server, std::string path, std::string interface)
	: _connection(_connection)
	, server(server)
	, path(path)
	, _interface(interface)
{}

ljh::unix::dbus::message ljh::unix::dbus::interface::call(std::string method)
{
	return message{*this, _interface, method};
}

ljh::unix::dbus::message ljh::unix::dbus::interface::get(std::string property)
{
	message temp{*this, DBUS_INTERFACE_PROPERTIES, "Get"};
	temp.args(_interface, property);
	return temp;
}

ljh::unix::dbus::message::message(const interface& base, std::string interface, std::string method)
	: base(base)
{
	_message = dbus_message_new_method_call(base.server.c_str(), base.path.c_str(), interface.c_str(), method.c_str());
}



