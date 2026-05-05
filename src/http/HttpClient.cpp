#include "HttpClient.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

HttpClient::HttpClient(std::string host) : host_(std::move(host)) {}

HttpResponse HttpClient::send(HttpRequest request) {
    namespace asio = boost::asio;
    namespace beast = boost::beast;
    namespace ssl = asio::ssl;
    namespace http = beast::http;
    using tcp = asio::ip::tcp;

    asio::io_context ioContext;
    ssl::context sslContext(ssl::context::tls_client);
    sslContext.set_default_verify_paths();

    tcp::resolver resolver(ioContext);
    beast::ssl_stream<beast::tcp_stream> stream(ioContext, sslContext);

    if (!SSL_set_tlsext_host_name(stream.native_handle(), host_.c_str())) {
        throw beast::system_error(static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category());
    }

    auto const results = resolver.resolve(host_, "443");
    beast::get_lowest_layer(stream).connect(results);

    stream.set_verify_mode(ssl::verify_peer);
    stream.handshake(ssl::stream_base::client);

    if (!request.base().count(http::field::host)) {
        request.set(http::field::host, host_);
    }
    if (!request.base().count(http::field::user_agent)) {
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    }

    http::write(stream, request);

    beast::flat_buffer buffer;
    HttpResponse response;
    http::read(stream, buffer, response);

    beast::error_code ec;
    stream.shutdown(ec);
    if (ec == asio::error::eof) {
        ec = {};
    }
    if (ec) {
        throw beast::system_error(ec);
    }

    return response;
}
