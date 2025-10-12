#include "AsioIOServicePool.h"
#include <iostream>
using namespace std;
AsioIOServicePool::AsioIOServicePool(size_t size) : _ioServices(size), _works(size), _nextIOService(0) {
	//size_t,用于表明数组的大小 ， 在_ioServices，_works创立两个空间，并且调用保存参数的构造函数
	for (size_t i = 0; i < size; i++) {
		_works[i] = std::make_unique<Work>(boost::asio::make_work_guard(_ioServices[i]));//创建work来保存线程，保证线程不结束
	}
	for (std::size_t i = 0; i < _ioServices.size(); i++) {
		_threads.emplace_back([this, i] {//lamda函数 ， 传入this便于获得_ioServices数组，传入i便于获得ioService
            _ioServices[i].run();//运行线程，如果前面不保存则会导致线程运行完成后结束  ，   前面保存work后不会自动结束 
        });
	}
}

AsioIOServicePool::~AsioIOServicePool() {
	Stop();
	std::cout << "AsioIOServicePool destruct" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService() {
	auto& service = _ioServices[_nextIOService++];
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop() {
	for (std::size_t i = 0; i < _ioServices.size(); i++) {
		_ioServices[i].stop();
	}
	for (auto& t : _threads) {//等待线程完成后停止线程，防止线程崩溃
		t.join();
	}
}