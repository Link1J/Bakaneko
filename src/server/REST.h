// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <memory>
#include <functional>
#include <string>
#include <sstream>
#include <map>

#include <ljh/string_utils.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "bakaneko-version.h"

namespace asio = boost::asio;

//application/x-protobuf

namespace Http
{
    class Response
    {
        static inline std::map<int, std::string> http_codes_strings {
            { 200, "OK" }, { 404, "Not Found" }, { 403, "Forbidden" }, { 418, "I'm a teapot" }, { 400, "Bad Request" },
            { 500, "Internal Server Error" },
        };

        int http_code;
        std::string data;
        std::string content_type;

    public:
        Response(int http_code, const std::string& content_type, const std::string& data)
            : http_code(http_code)
            , data(data)
            , content_type(content_type)
        {}

        void get(asio::streambuf& buffer) const
        {
            std::ostream output(&buffer);
            output << "HTTP/1.1 " << http_code << " " << http_codes_strings[http_code] << std::endl;
            output << "Server: " << "Bakaneko/" << BAKANEKO_VERSION_STRING << std::endl;
            output << "Content-Type: " << content_type << std::endl;
            output << "Content-Length: " << data.length() << std::endl;
            output << std::endl;
            output << data;
        }
    };

    class Request
    {
        std::string _i_method;
        std::string _i_url;
        std::map<std::string, std::string> _i_fields;
        std::string _i_body;

    public:
        Request(std::string data)
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

        void get(asio::streambuf& buffer) const
        {
            std::ostream output(&buffer);
            output << _i_method << " " << _i_url << " HTTP/1.1" << std::endl;
            for (auto& [key, value] : _i_fields)
                output << key << ": " << value << std::endl;
            output << std::endl;
            output << _i_body;
        }

        std::string                       & method() { return _i_method; };
        std::string                       & url   () { return _i_url   ; };
        std::map<std::string, std::string>& fields() { return _i_fields; };
        std::string                       & body  () { return _i_body  ; };
        const std::string                       & method() const { return _i_method; };
        const std::string                       & url   () const { return _i_url   ; };
        const std::map<std::string, std::string>& fields() const { return _i_fields; };
        const std::string                       & body  () const { return _i_body  ; };
    };
}



class RestServer
{
    friend class connection;
    using handler = std::function<Http::Response(const Http::Request&)>;

    asio::io_service& io_service;
    asio::ip::tcp::acceptor acceptor;
    std::map<std::string, handler> handlers;

public:
    class connection
    {
        asio::ip::tcp::socket _i_socket;

    public:
        connection(asio::io_service& io_service)
            : _i_socket(io_service)
        {}

        void handle(std::shared_ptr<connection> _this, const RestServer* server)
        {
            std::shared_ptr<asio::streambuf> buff = std::make_shared<asio::streambuf>();
            asio::async_read_until(_i_socket, *buff, "\r\n\r\n", [this, _this, server, buff](const boost::system::error_code& ec, std::size_t bytes_transferred)
            {
                auto internal_data = buff->data();
                std::string data((char*)internal_data.data(), internal_data.size());
                buff->consume(internal_data.size());
                Http::Request request(data);
                
                Http::Response response(200, "text/html", "<html><head><title>404</title></head><body><h1>404 Not Found</h1><pre>" + data + "</pre></body></html>");
                if (server->handlers.find(request.url()) != server->handlers.end())
                    response = server->handlers.at(request.url())(request);

                response.get(*buff);
                asio::async_write(_i_socket, *buff, [this, _this, server, buff](const boost::system::error_code& ec, std::size_t bytes_transferred){});
            });
            
        }

        auto& socket()
        {
            return _i_socket;
        }
    };

private:
    void accept()
    {
        std::shared_ptr<connection> client = std::make_shared<connection>(io_service);
        acceptor.async_accept(client->socket(), [this, client](const boost::system::error_code& error)
        {
            accept();
            if(!error)
                client->handle(client, this);
        });
    }

public:
    RestServer(asio::io_service& io_service, asio::ip::tcp::endpoint& endpoint)
        : io_service{io_service}
        , acceptor{io_service, endpoint}
    {}

    void start_listening()
    {
        acceptor.listen();
        accept();
    }

    void add_handler(std::string path, handler new_handler)
    {
        handlers[path] = new_handler;
    }
};