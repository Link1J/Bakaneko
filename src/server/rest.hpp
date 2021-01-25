// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <memory>
#include <functional>
#include <type_traits>

#include <ljh/expected.hpp>

#include <boost/version.hpp>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include "bakaneko-version.h"
#include "info.hpp"

namespace asio  = boost::asio ;
namespace beast = boost::beast;

namespace Rest
{
    class Server : public std::enable_shared_from_this<Server>
    {
        asio::io_service& io_service;
        asio::ip::tcp::acceptor acceptor;
        asio::ip::tcp::socket socket;

    public:
        class Connection : public std::enable_shared_from_this<Connection>
        {
#if BOOST_VERSION < 017000
            asio::ip::tcp::socket socket;
            asio::strand<asio::io_context::executor_type> strand;
#else
            beast::tcp_stream stream;
#endif
            beast::flat_buffer buffer;
            beast::http::request<beast::http::string_body> req;
            std::shared_ptr<void> res;

            template<class Body, class Allocator, class Send>
            void handler(beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req, Send&& send);
            
            template<class MessageReply, class Body, class Allocator, class Send>
            void Run(ljh::expected<MessageReply,Errors>(*function)(), beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req, Send&& send);

            asio::ip::tcp::endpoint endpoint();

        public:
            explicit Connection(asio::ip::tcp::socket socket_);
            ~Connection();

            void run();
            void do_read();
            void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
            void on_write(bool close, boost::system::error_code ec, std::size_t bytes_transferred);
            void do_close();
        };

    private:
        void do_accept();
        void on_accept(boost::system::error_code ec);

    public:
        explicit Server(asio::io_context& io_service, asio::ip::tcp::endpoint endpoint);

        void run();
    };
}
