// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "http.hpp"
#include "bakaneko-version.h"

#include <sstream>
#include <google/protobuf/message.h>
#include <ljh/string_utils.hpp>

static std::map<int, std::string> http_codes_strings {
    { 200, "OK" }, { 404, "Not Found" }, { 403, "Forbidden" }, { 418, "I'm a teapot" }, { 400, "Bad Request" },
    { 500, "Internal Server Error" }, { 501, "Not Implemented" }, { 415, "Unsupported Media Type" },
};

Http::Response::Response(int http_code)
    : http_code(http_code)
{}

Http::Response& Http::Response::set_content(const std::string& content_type, const std::string& data)
{
    this->content_type = content_type;
    this->data = data;
    return *this;
}

Http::Response& Http::Response::add_header(const std::string& key, const std::string& value)
{
    headers.push_back(key + ": " + value);
    return *this;
}

void Http::Response::get(asio::streambuf& buffer) const
{
    std::ostream output(&buffer);
    output << "HTTP/1.1 " << http_code << " " << http_codes_strings[http_code] << std::endl;
    output << "Server: " << "Bakaneko/" << BAKANEKO_VERSION_STRING << std::endl;
    output << "Content-Type: " << content_type << std::endl;
    output << "Content-Length: " << data.length() << std::endl;
    for (auto& header : headers) output << header << std::endl;
    output << std::endl;
    output << data;
}

bool Http::Response::has_content() const
{
    return !data.empty();
}

Http::Request::Request(std::string data)
{
    if (data.empty())
        return;

    auto data_split = ljh::split(data, "\r\n\r\n");

    if (data_split.size() > 1)
        _i_body = data_split[1];

    auto list = ljh::split(data_split[0], "\r\n");

    auto request = ljh::split(list[0], ' ');
    _i_method = request[0];
    _i_url = request[1];

    for (int a = 1; a < list.size(); a++)
    {
        auto items = ljh::split(list[a], ": ");
        for (int a = 2; a < items.size(); a++)
            items[1] += ": " + items[a];
        _i_fields[items[0]] = items[1];
    }
}

void Http::Request::get(asio::streambuf& buffer) const
{
    std::ostream output(&buffer);
    output << _i_method << " " << _i_url << " HTTP/1.1" << std::endl;
    for (auto& [key, value] : _i_fields)
        output << key << ": " << value << std::endl;
    output << std::endl;
    output << _i_body;
}

std::string                       & Http::Request::method() { return _i_method; };
std::string                       & Http::Request::url   () { return _i_url   ; };
std::map<std::string, std::string>& Http::Request::fields() { return _i_fields; };
std::string                       & Http::Request::body  () { return _i_body  ; };

const std::string                       & Http::Request::method() const { return _i_method; };
const std::string                       & Http::Request::url   () const { return _i_url   ; };
const std::map<std::string, std::string>& Http::Request::fields() const { return _i_fields; };
const std::string                       & Http::Request::body  () const { return _i_body  ; };

Http::Response Http::output_message(google::protobuf::Message& message, const Http::Request& request)
{
    Http::Response response{200};

    response.add_header("Protobuf-Type", message.GetTypeName());

    if (request.fields().find("Content-Type") != request.fields().end())
    {
        auto content_type = request.fields().at("Content-Type");
        if (content_type.find("application/x-protobuf") != std::string::npos)
        {
            response.set_content("application/x-protobuf", message.SerializeAsString());
        }
        else if (content_type.find("text/plain") != std::string::npos)
        {
            response.set_content("text/plain", message.Utf8DebugString());
        }
        else
        {
            response = Http::Response{415};
        }
    }
    else
    {
        response.set_content("application/x-protobuf", message.SerializeAsString());
    }

    return response;
}