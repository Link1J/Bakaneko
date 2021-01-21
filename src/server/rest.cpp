// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "rest.hpp"

Rest::Server::Connection::Connection(asio::io_service& io_service)
    : _i_socket(io_service)
{}

void Rest::Server::Connection::handle(std::shared_ptr<Rest::Server::Connection> _this, const Rest::Server* server)
{
    std::shared_ptr<asio::streambuf> buff = std::make_shared<asio::streambuf>();
    asio::async_read_until(_i_socket, *buff, "\r\n\r\n", [this, _this, server, buff](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        auto internal_data = buff->data();
        std::string data((char*)internal_data.data(), internal_data.size());
        buff->consume(internal_data.size());
        Http::Request request(data);
        
        Http::Response response(404);
        response.set_content("text/html", "<html><head><title>404</title></head><body><h1>404 Not Found</h1><pre>" + data + "</pre></body></html>");

        if (server->handlers.find(request.url()) != server->handlers.end())
            response = server->handlers.at(request.url())(request);

        response.get(*buff);
        asio::async_write(_i_socket, *buff, [this, _this, server, buff](const boost::system::error_code& ec, std::size_t bytes_transferred){});
    });
    
}

auto& Rest::Server::Connection::socket()
{
    return _i_socket;
}

void Rest::Server::accept()
{
    std::shared_ptr<Connection> client = std::make_shared<Connection>(io_service);
    acceptor.async_accept(client->socket(), [this, client](const boost::system::error_code& error)
    {
        accept();
        if(!error)
            client->handle(client, this);
    });
}

Rest::Server::Server(asio::io_service& io_service, asio::ip::tcp::endpoint& endpoint)
    : io_service{io_service}
    , acceptor{io_service, endpoint}
{}

void Rest::Server::start_listening()
{
    acceptor.listen();
    accept();
}

void Rest::Server::add_handler(std::string path, Rest::Server::handler new_handler)
{
    handlers[path] = new_handler;
}