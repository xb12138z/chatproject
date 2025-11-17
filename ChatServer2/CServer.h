#pragma once
#include <boost/asio.hpp>
#include "CSession.h"
#include <memory.h>
#include <map>
#include <mutex>
using namespace std;
using boost::asio::ip::tcp;
class CServer
{
public:
	CServer(boost::asio::io_context& io_context, short port);
	~CServer();
	void ClearSession(std::string);
private:
	void HandleAccept(shared_ptr<CSession>, const boost::system::error_code& error);
	void StartAccept();
	boost::asio::io_context& _io_context;//主ioc
	short _port;
	tcp::acceptor _acceptor;// 是用来监听和接受传入的连接的。
	//它创建一个监听套接字，绑定到指定的地址和端口，等待客户端的连接。当客户端发起连接时，acceptor 会接受连接并为每个连接创建一个新的 socket，然后你可以用这个新的 socket 来与客户端进行数据交换。
	std::map<std::string, shared_ptr<CSession>> _sessions;
	std::mutex _mutex;
};