// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <string>
#include <map>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;

namespace google::protobuf
{
    class Message;
}

namespace Http
{
    class Response
    {
        int http_code;
        std::string data;
        std::string content_type;
        std::vector<std::string> headers;

    public:
        Response(int http_code);
        Response& set_content(const std::string& content_type, const std::string& data);
        Response& add_header(const std::string& key, const std::string& value);
        void get(asio::streambuf& buffer) const;

        bool has_content() const;
    };

    class Request
    {
        std::string _i_method;
        std::string _i_url;
        std::map<std::string, std::string> _i_fields;
        std::string _i_body;

    public:
        Request(std::string data);

        void get(asio::streambuf& buffer) const;

        std::string                       & method();
        std::string                       & url   ();
        std::map<std::string, std::string>& fields();
        std::string                       & body  ();

        const std::string                       & method() const;
        const std::string                       & url   () const;
        const std::map<std::string, std::string>& fields() const;
        const std::string                       & body  () const;
    };
    
    Response output_message(google::protobuf::Message&, const Request&);
}
