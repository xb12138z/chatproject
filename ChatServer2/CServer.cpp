#include "CServer.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"
#include <iostream>

CServer::CServer(boost::asio::io_context& io_context, short port) :_io_context(io_context), _port(port),
_acceptor(io_context, tcp::endpoint(tcp::v4(), port))//_acceptor监听port。说明_acceptor跑在主线程中
{
	cout << "Server start success, listen on port : " << _port << endl;
	StartAccept();
}

CServer::~CServer() {
	cout << "Server destruct listen on port : " << _port << endl;

}

void CServer::StartAccept() {//监听到了一个新的链接并且绑定到了new_session中的socket中
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();//获取一个 子io_context 实例
	shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);//新建的跑在子线程中
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));//在_acceptor中监听到后async_accept将链接绑定到返回的socket中
	//async_accept（socket,回调函数）
	//std::bind 让我们可以在调用 async_accept 时，将 HandleAccept 和它需要的参数（new_session 和 error_code）绑定在一起，以便在连接完成后异步调用该函数。
}

void CServer::ClearSession(std::string session_id) {
	if (_sessions.find(session_id) != _sessions.end()) {
		auto uid = _sessions[session_id]->GetUserId();

		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId(), session_id);
	}
	{
		lock_guard<mutex> lock(_mutex);
		_sessions.erase(session_id);
	}
}

void CServer::HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error) {
	if (!error) {
		new_session->Start();
		lock_guard<mutex> lock(_mutex);
		_sessions.insert(make_pair(new_session->GetSessionId(), new_session));
	}
	else {
		cout << "session accept failed, error is " << error.what() << endl;
	}

	StartAccept();
}