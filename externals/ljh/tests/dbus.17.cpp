
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_translate_exception.hpp>

#include <ljh/unix/dbus.hpp>

#include <iostream>

CATCH_TRANSLATE_EXCEPTION(const ljh::unix::dbus::error& ex) {
	return "DBus Error (" + ex.name() + "): " + ex.message();
}

TEST_CASE("dbus - call (store)", "[test_17][dbus]" ) {
	using namespace ljh::unix;
	
	dbus::connection connection(dbus::bus::SYSTEM);
	auto dbus_obj = connection.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	std::string storage;
	dbus_obj.call("GetId").run(storage);
	REQUIRE(storage != "");
}

TEST_CASE("dbus - call (return)", "[test_17][dbus]" ) {
	using namespace ljh::unix;
	
	dbus::connection connection(dbus::bus::SYSTEM);
	auto dbus_obj = connection.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	REQUIRE(dbus_obj.call("GetId").run<std::string>() != "");
}

TEST_CASE("dbus - call (args, return)", "[test_17][dbus]" ) {
	using namespace ljh::unix;
	
	dbus::connection connection(dbus::bus::SYSTEM);
	auto dbus_obj = connection.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	REQUIRE(dbus_obj.call("GetNameOwner").args("org.freedesktop.DBus").run<std::string>() == "org.freedesktop.DBus");
}

TEST_CASE("dbus - call (args, store)", "[test_17][dbus]" ) {
	using namespace ljh::unix;
	
	dbus::connection connection(dbus::bus::SYSTEM);
	auto dbus_obj = connection.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	std::string storage;
	dbus_obj.call("GetNameOwner").args("org.freedesktop.DBus").run(storage);
	REQUIRE(storage != "");
}

TEST_CASE("dbus - get property", "[test_17][dbus]" ) {
	using namespace ljh::unix;
	
	dbus::connection connection(dbus::bus::SYSTEM);
	auto dbus_obj = connection.get(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

	CHECK(dbus_obj.get<std::vector<std::string>>("Features") != std::vector<std::string>{"SystemdActivation"});
}