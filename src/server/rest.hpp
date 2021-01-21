// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "http.hpp"

#include <memory>
#include <functional>

namespace Rest
{
    class Server
    {
        friend class Connection;
        using handler = std::function<Http::Response(const Http::Request&)>;

        asio::io_service& io_service;
        asio::ip::tcp::acceptor acceptor;
        std::map<std::string, handler> handlers;

    public:
        class Connection
        {
            asio::ip::tcp::socket _i_socket;

        public:
            Connection(asio::io_service& io_service);
            void handle(std::shared_ptr<Connection> _this, const Server* server);
            auto& socket();
        };

    private:
        void accept();

    public:
        Server(asio::io_service& io_service, asio::ip::tcp::endpoint& endpoint);

        void start_listening();
        void add_handler(std::string path, handler new_handler);
    };
}