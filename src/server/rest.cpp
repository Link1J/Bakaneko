// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "rest.hpp"

#include <spdlog/spdlog.h>

#include <ljh/function_pointer.hpp>

Rest::Server::Connection::Connection(asio::ip::tcp::socket socket_)
#if BOOST_VERSION < 107000
    : socket(std::move(socket_))
    , strand(socket.get_executor())
    , endpoint(socket.remote_endpoint())
#else
    : stream(std::move(socket_))
    , endpoint(stream.socket().remote_endpoint())
#endif
{
    // spdlog::get("networking")->info("Got connection from {}:{}", endpoint.address().to_string(), endpoint.port());
}

Rest::Server::Connection::~Connection()
{
    // spdlog::get("networking")->info("{}:{} disconnected", endpoint.address().to_string(), endpoint.port());
}

void Rest::Server::Connection::run()
{
#if BOOST_VERSION < 107000
    do_read();
#else
    asio::dispatch(stream.get_executor(), beast::bind_front_handler(&Connection::do_read, shared_from_this()));
#endif
}

void Rest::Server::Connection::do_read()
{
#if BOOST_VERSION < 107000
    beast::http::async_read(socket, buffer, req, asio::bind_executor(strand, std::bind(&Connection::on_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
#else
    req = {};
    stream.expires_after(std::chrono::seconds(30));
    beast::http::async_read(stream, buffer, req, beast::bind_front_handler(&Connection::on_read, shared_from_this()));
#endif
}

void Rest::Server::Connection::on_read(boost::system::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec == beast::http::error::end_of_stream)
        return do_close();

    if(ec)
        return; // fail(ec, "read");

    handler(std::move(req), [this](auto&& msg){
        auto sp = std::make_shared<beast::http::message<false, beast::http::string_body>>(std::move(msg));
        res = sp;
#if BOOST_VERSION < 107000
        beast::http::async_write(socket, *sp, boost::asio::bind_executor(strand,
            std::bind(&Connection::on_write, shared_from_this(), sp->need_eof(), std::placeholders::_1, std::placeholders::_2)
        ));
#else
        beast::http::async_write(stream, *sp, beast::bind_front_handler(
            &Connection::on_write, shared_from_this(), sp->need_eof()
        ));
#endif
    });
}

void Rest::Server::Connection::on_write(bool close, boost::system::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return; //fail(ec, "write");

    if(close)
        return do_close();

    res = nullptr;
    do_read();
}

void Rest::Server::Connection::do_close()
{
    boost::system::error_code ec;
#if BOOST_VERSION < 107000
    socket.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
#else
    stream.socket().shutdown(asio::ip::tcp::socket::shutdown_send, ec);
#endif
}

Rest::Server::Server(asio::io_context& io_service, asio::ip::tcp::endpoint endpoint)
    : io_service{io_service}
    , acceptor{io_service}
    , socket{io_service}
{
    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::socket_base::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(asio::socket_base::max_listen_connections);
    spdlog::get("networking")->info("Listening on {}:{}", endpoint.address().to_string(), endpoint.port());
}

void Rest::Server::run()
{
    if(!acceptor.is_open())
        return;
    do_accept();
}

void Rest::Server::do_accept()
{
    acceptor.async_accept(socket, std::bind(&Server::on_accept, shared_from_this(), std::placeholders::_1));
}

void Rest::Server::on_accept(boost::system::error_code ec)
{
    if(!ec)
        std::make_shared<Connection>(std::move(socket))->run();
    do_accept();
}

template<class MessageReply, class Body, class Allocator, class Send>
void Rest::Server::Connection::Run(ljh::expected<MessageReply,Errors>(*function)(), beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req, Send&& send)
{
    try
    {
        auto message_res = function();

        if (message_res.has_value())
        {
            if constexpr (!std::is_same_v<MessageReply,void>)
            {
                if (auto content_type = req.find(beast::http::field::content_type); content_type != req.end())
                {
                    if (content_type->value() == "application/x-protobuf")
                    {
                        beast::http::response<beast::http::string_body> res{beast::http::status::ok, req.version()};
                        res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
                        res.set(beast::http::field::content_type, "application/x-protobuf");
                        res.set("Protobuf-Type", message_res->GetTypeName());
                        res.body() = message_res->SerializeAsString();
                        res.prepare_payload();
                        return send(std::move(res));
                    }
                    else if (content_type->value() == "text/plain")
                    {
                        beast::http::response<beast::http::string_body> res{beast::http::status::ok, req.version()};
                        res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
                        res.set(beast::http::field::content_type, "text/plain");
                        res.body() = message_res->Utf8DebugString();
                        res.prepare_payload();
                        return send(std::move(res));
                    }
                    else
                    {
                        std::string_view lpath(content_type->value().data(), content_type->value().size());
                        std::string_view lclient(req[beast::http::field::user_agent].data(), req[beast::http::field::user_agent].size());
                        spdlog::get("networking")->warn("Unknown Content-Type '{}' requested from {}:{} ({})", lpath, endpoint.address().to_string(), endpoint.port(), lclient);
                    }
                }
            }
            else
            {
                beast::http::response<beast::http::string_body> res{beast::http::status::ok, req.version()};
                res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
                res.prepare_payload();
                return send(std::move(res));
            }

            beast::http::response<beast::http::empty_body> res{beast::http::status::unsupported_media_type, req.version()};
            res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
            res.keep_alive(req.keep_alive());
            res.prepare_payload();
            return send(std::move(res));
        }
        else if (message_res.error() == Errors::NotImplemented)
        {
            beast::http::response<beast::http::empty_body> res{beast::http::status::not_implemented, req.version()};
            res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
            res.keep_alive(req.keep_alive());
            res.prepare_payload();
            return send(std::move(res));
        }
        else if (message_res.error() == Errors::Failed)
        {
            beast::http::response<beast::http::empty_body> res{beast::http::status::internal_server_error, req.version()};
            res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
            res.keep_alive(req.keep_alive());
            res.prepare_payload();
            return send(std::move(res));
        }
    }
    catch (const std::exception& e)
    {
        spdlog::error(e.what());

        beast::http::response<beast::http::empty_body> res{beast::http::status::internal_server_error, req.version()};
        res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
        res.keep_alive(req.keep_alive());
        res.prepare_payload();
        return send(std::move(res));
    }
}

template<class Body, class Allocator, class Send>
void Rest::Server::Connection::handler(beast::http::request<Body, beast::http::basic_fields<Allocator>>&& req, Send&& send)
{
    auto path = req.target();
    
    if (path == "/")
    {
        beast::http::response<beast::http::empty_body> res{beast::http::status::found, req.version()};
        res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
        res.keep_alive(req.keep_alive());
        res.prepare_payload();
        return send(std::move(res));
    }

    if (req.method() == beast::http::verb::get)
    {
        if (path == "/drives")
            return Run(&Info::Drives  , std::move(req), std::move(send));
        if (path == "/system")
            return Run(&Info::System  , std::move(req), std::move(send));
        if (path == "/updates")
            return Run(&Info::Updates , std::move(req), std::move(send));
        if (path == "/network/adapters")
            return Run(&Info::Adapters, std::move(req), std::move(send));
    }
    if (req.method() == beast::http::verb::post)
    {
        if (path == "/power/shutdown")
            return Run(&Control::Shutdown, std::move(req), std::move(send));
        if (path == "/power/reboot")
            return Run(&Control::Reboot  , std::move(req), std::move(send));
    }

    std::string_view lpath(path.data(), path.size());
    std::string_view lclient(req[beast::http::field::user_agent].data(), req[beast::http::field::user_agent].size());

    spdlog::get("networking")->warn("Unknown Path '{}' requested from {}:{} ({})", lpath, endpoint.address().to_string(), endpoint.port(), lclient);

    beast::http::response<beast::http::empty_body> res{beast::http::status::not_found, req.version()};
    res.set(beast::http::field::server, "Bakaneko/" BAKANEKO_VERSION_STRING);
    res.keep_alive(req.keep_alive());
    res.prepare_payload();
    return send(std::move(res));
};