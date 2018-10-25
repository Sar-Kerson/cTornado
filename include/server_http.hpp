#ifndef HTTPSERVER_HPP
#define	HTTPSERVER_HPP

#include <boost/asio.hpp>
#include <regex>
#include <unordered_map>
#include <thread>
#include <string>
using namespace std;
using namespace boost::asio;

struct Request {
    string method, path, http_version;

    shared_ptr<istream> content;

    unordered_map<string, string> header;
};

class HttpServer {
public:
    // usage of std::function https://en.cppreference.com/w/cpp/utility/functional/function
    // resources: {path1: {post: func1, get: func2, ...}, ...}
    unordered_map<string, unordered_map<string, function<void(ostream&, const Request&, const smatch&)>>> resources;

    HttpServer(unsigned short, size_t); // port, and ??

    void start();

private:
    io_service m_io_service; // main io service
    ip::tcp::endpoint endpoint;
    ip::tcp::acceptor acceptor;

    // thread pools
    size_t num_threads;
    vector<thread> threads;

    void accept();
    
    void process_request_and_respond(shared_ptr<ip::tcp::socket> socket);

    Request parse_request(istream & stream);

    void respond(shared_ptr<ip::tcp::socket> socket, shared_ptr<Request> request);
};

HttpServer::HttpServer 
(unsigned short port, size_t num_threads = 1) 
:endpoint(ip::tcp::v4(), port), 
acceptor(m_io_service, endpoint), 
num_threads(num_threads) { }


void
HttpServer::start()
{

}


void
HttpServer::accept()
{
    // Create new socket for this connections
    shared_ptr<ip::tcp::socket> socket(new ip::tcp::socket(m_io_service));

    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code & ec) {
        accept(); // ? 这样不会造成爆炸式增长吗

        if (!ec) { // ? error_code是全局变量吧，那么这时候会存在资源竞争问题
            process_request_and_respond(socket);
        }
    });
}


void
HTTPServer::process_request_and_respond(shared_ptr<ip::tcp::socket> socket) 
{
    shared_ptr<boost::asio::streambuf> read_buffer(new boost::asio::streambuf);

    async_read_until(*socket, *read_buffer, "\r\n\r\n",
        [this, socket, read_buffer]
        (const boost::system::error_code & ec,
        size_t bytes_tranferred) {
        if (!ec) {
            size_t total = read_buffer->size();

            
        }
    })
}


#endif	/* HTTPSERVER_HPP */
