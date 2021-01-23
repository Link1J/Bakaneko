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
        int _http_code;
        std::string _data;
        std::string _content_type;
        std::map<std::string, std::string> _headers;

    public:
        explicit Response(int http_code);
        explicit Response(std::string data);

        Response& set_content(const std::string& content_type, const std::string& data);
        Response& add_header(const std::string& key, const std::string& value);
        void get(asio::streambuf& buffer) const;

        bool has_content() const;

        const std::string                       & data        () const;
        int                                       http_code   () const;
        const std::map<std::string, std::string>& headers     () const;
        const std::string                       & content_type() const;
    };

    class Request
    {
        std::string _i_method;
        std::string _i_url;
        std::map<std::string, std::string> _i_fields;
        std::string _i_body;

    public:
        explicit Request(std::string data);
        Request() = default;

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