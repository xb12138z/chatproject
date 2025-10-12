#include "CServerClass.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

CServer::CServer(net::io_context& ioc, unsigned short& port):_ioc(ioc),//主ioc用于监听端口
_acceptor (ioc, tcp::endpoint(tcp::v4(), port)){//accpetor用于绑定  ioc → 事件循环    tcp::endpoint(tcp::v4(), port) → 绑定到 IPv4 的 port 端口

}

void CServer::Start(){
	auto self = shared_from_this();
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();//获取io_context线程池中的ioc，负责与soceket通信
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);//创立了一个ioc对应的HttpConnection类，用于与socket通信
	_acceptor.async_accept(new_con ->Getsocket(), [self , new_con](beast::error_code ec) {//传入self指针避免提前删除，对HttpConnection中的socket进行分配，实现主ioc到woker ioc的转发
		try {
			//出错放弃socket链接，继续监听其他链接
			if (ec) {
				self ->Start();
				return;
			}
			//创建新链接，并且创建HttpConnection类管理这个链接
			new_con -> Start();//不能直接传入socted的引用，因为后续可能socket值发生改变

			//继续监听
			self ->Start();
		}
		catch (std::exception& exp) {

			}
		});
}