#pragma once
#include "const.h"

class CServer:public std::enable_shared_from_this<CServer>//看
{
    public:
        CServer(boost::asio::io_context& ioc, unsigned short& port);//io_context用于和底层线程通信的IO对象
        void Start();
    private:
        tcp::acceptor _acceptor;//接收器
        net::io_context& _ioc;
        //tcp::socket _socket;//用于接收对端的链接信息，可以复用//多线程删除
};

