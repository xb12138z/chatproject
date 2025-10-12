#pragma once
#include "Singleton.h"
#include <boost/asio.hpp>
#include <vector>
class AsioIOServicePool:public Singleton <AsioIOServicePool>
{
	friend class Singleton <AsioIOServicePool>;//因为AsioIOServicePool构造函数是私有的，所以Singleton <AsioIOServicePool>无法直接访问，所以只能friend,让他可以访问
public:
	using IOService = boost::asio::io_context;//给上下文重命名,ioc
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
	//executor_work_guard,工作守护对象。只要 work_guard 存在，io_context.run() 就认为自己“还有工作要做”，不会立刻退出。  executor_typeC++11以后成立的调度器类型
	using Workptr = std::unique_ptr<Work>;
	~AsioIOServicePool();
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

	boost::asio::io_context& GetIOService();
	void Stop();

private:
    AsioIOServicePool(std::size_t size = 2);//std::thread::hardware_concurrency()计算计算机核心数//目前默认开启2个线程
	std::vector<IOService> _ioServices;
	std::vector<Workptr> _works;
	std::vector<std::thread> _threads;
	std::size_t              _nextIOService;
};

