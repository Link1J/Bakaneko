
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "ljh/windows/wmi.hpp"

static winrt::com_ptr<IWbemLocator> locator;

static ljh::windows::com_bstr query_string(const std::wstring& class_name, const std::wstring& member, const std::wstring& associated)
{
	std::wstring query;
	if (!member.empty() && class_name.empty())
	{
		query = L"Associators of {" + member + L"}";
		if (!associated.empty())
			query += L" where AssocClass=" + associated;
	}
	else
	{
		query = L"SELECT * FROM " + class_name;
		if (!member.empty() && !associated.empty())
			query += L" WHERE " + member + L" = '" + associated + L"'";
	}
	return ljh::windows::com_bstr{query};
}

ljh::windows::com_variant ljh::windows::wmi::clazz::_i_get(const std::wstring& member)
{
	com_variant data;
	data.clear();
	winrt::check_hresult(_i_object->Get(member.c_str(), 0, &data.data(), 0, 0));
	return data;
}

ljh::windows::wmi::clazz::clazz(const winrt::com_ptr<IWbemServices>& _i_service, const winrt::com_ptr<IWbemClassObject>& _i_object)
	: _i_service(_i_service)
	, _i_object(_i_object)
{}

std::vector<std::wstring> ljh::windows::wmi::clazz::member_names()
{
	com_safe_array<com_bstr> names;
	winrt::check_hresult(_i_object->GetNames(nullptr, WBEM_FLAG_ALWAYS, nullptr, names.put()));

	std::vector<std::wstring> output_names;
	output_names.reserve(names.size());

	for (auto& name : names)
		output_names.push_back(name);

	return output_names;
}

bool ljh::windows::wmi::clazz::has(const std::wstring& member)
{
	com_safe_array<com_bstr> names;
	winrt::check_hresult(_i_object->GetNames(nullptr, WBEM_FLAG_ALWAYS, nullptr, names.put()));
	for (auto& name : names)
		if (name == member)
			return true;
	return false;
}

std::vector<ljh::windows::wmi::clazz> ljh::windows::wmi::clazz::associators(const std::wstring& assoc_class)
{
	winrt::com_ptr<IEnumWbemClassObject> enumerator;
	winrt::check_hresult(_i_service->ExecQuery(L"WQL"_bstr, query_string(L"", get(L"__RELPATH"), assoc_class), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, enumerator.put()));
	
	std::vector<clazz> clazzes;
	while (enumerator.get())
	{
		winrt::com_ptr<IWbemClassObject> object;
		ULONG return_value;
		winrt::check_hresult(enumerator->Next(WBEM_INFINITE, 1, object.put(), &return_value));

		if(return_value == 0)
			break;

		clazzes.emplace_back(_i_service, object);
	}
	return clazzes;
}

std::vector<ljh::windows::wmi::clazz> ljh::windows::wmi::clazz::associators()
{
	return associators(L"");
}

ljh::windows::wmi::service::service(const ljh::windows::com_bstr& location)
{
	winrt::check_hresult(locator->ConnectServer(location, NULL, NULL, 0, NULL, 0, 0, _i_service.put()));
	winrt::check_hresult(CoSetProxyBlanket(_i_service.get(), RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE));
}

std::vector<ljh::windows::wmi::clazz> ljh::windows::wmi::service::get_class(const std::wstring& class_name, const std::wstring& member, const std::wstring& value)
{
	using namespace ljh::windows::com_bstr_literals;
	
	winrt::com_ptr<IEnumWbemClassObject> enumerator;
	winrt::check_hresult(_i_service->ExecQuery(L"WQL"_bstr, query_string(class_name, member, value), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, enumerator.put()));
	
	std::vector<clazz> clazzes;
	while (enumerator.get())
	{
		winrt::com_ptr<IWbemClassObject> object;
		ULONG return_value;
		winrt::check_hresult(enumerator->Next(WBEM_INFINITE, 1, object.put(), &return_value));

		if(return_value == 0)
			break;

		clazzes.emplace_back(_i_service, object);
	}
	return clazzes;
}

void ljh::windows::wmi::setup()
{
	winrt::check_hresult(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL));
	locator = winrt::create_instance<IWbemLocator>(CLSID_WbemLocator);
}

ljh::windows::wmi::service& ljh::windows::wmi::service::root()
{
	using namespace ljh::windows::com_bstr_literals;
	static ljh::windows::wmi::service service{L"ROOT\\CIMV2"_bstr};
	return service;
}