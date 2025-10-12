#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "CServerClass.h"
#include "ConfigMgr.h"
#include "const.h"
#include "RedisMgr.h"

//void TestRedis() {
//	//连接redis 需要启动才可以进行连接
////redis默认监听端口为6387 可以再配置文件中修改
//	redisContext* c = redisConnect("127.0.0.1", 6380);
//	if (c->err)
//	{
//		printf("Connect to redisServer faile:%s\n", c->errstr);
//		redisFree(c);        return;
//	}
//	printf("Connect to redisServer Success\n");
//
//	std::string redis_password = "123456";
//	redisReply* r = (redisReply*)redisCommand(c, "AUTH %s", redis_password.c_str());
//	if (r->type == REDIS_REPLY_ERROR) {
//		printf("Redis认证失败！\n");
//	}
//	else {
//		printf("Redis认证成功！\n");
//	}
//
//	//为redis设置key
//	const char* command1 = "set stest1 value1";
//
//	//执行redis命令行
//	r = (redisReply*)redisCommand(c, command1);
//
//	//如果返回NULL则说明执行失败
//	if (NULL == r)
//	{
//		printf("Execut command1 failure\n");
//		redisFree(c);        return;
//	}
//
//	//如果执行失败则释放连接
//	if (!(r->type == REDIS_REPLY_STATUS && (strcmp(r->str, "OK") == 0 || strcmp(r->str, "ok") == 0)))
//	{
//		printf("Failed to execute command[%s]\n", command1);
//		freeReplyObject(r);
//		redisFree(c);        return;
//	}
//
//	//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
//	freeReplyObject(r);
//	printf("Succeed to execute command[%s]\n", command1);
//
//	const char* command2 = "strlen stest1";
//	r = (redisReply*)redisCommand(c, command2);
//
//	//如果返回类型不是整形 则释放连接
//	if (r->type != REDIS_REPLY_INTEGER)
//	{
//		printf("Failed to execute command[%s]\n", command2);
//		freeReplyObject(r);
//		redisFree(c);        return;
//	}
//
//	//获取字符串长度
//	int length = r->integer;
//	freeReplyObject(r);
//	printf("The length of 'stest1' is %d.\n", length);
//	printf("Succeed to execute command[%s]\n", command2);
//
//	//获取redis键值对信息
//	const char* command3 = "get stest1";
//	r = (redisReply*)redisCommand(c, command3);
//	if (r->type != REDIS_REPLY_STRING)
//	{
//		printf("Failed to execute command[%s]\n", command3);
//		freeReplyObject(r);
//		redisFree(c);        return;
//	}
//	printf("The value of 'stest1' is %s\n", r->str);
//	freeReplyObject(r);
//	printf("Succeed to execute command[%s]\n", command3);
//
//	const char* command4 = "get stest2";
//	r = (redisReply*)redisCommand(c, command4);
//	if (r->type != REDIS_REPLY_NIL)
//	{
//		printf("Failed to execute command[%s]\n", command4);
//		freeReplyObject(r);
//		redisFree(c);        return;
//	}
//	freeReplyObject(r);
//	printf("Succeed to execute command[%s]\n", command4);
//
//	//释放连接资源
//	redisFree(c);
//
//}

void TestRedisMgr() {
    assert(RedisMgr::GetInstance()->Set("blogwebsite", "llfc.club"));
    std::string value = "";
    assert(RedisMgr::GetInstance()->Get("blogwebsite", value));
    assert(RedisMgr::GetInstance()->Get("nonekey", value) == false);
    assert(RedisMgr::GetInstance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
    assert(RedisMgr::GetInstance()->HGet("bloginfo", "blogwebsite") != "");
    assert(RedisMgr::GetInstance()->ExistsKey("bloginfo"));
    assert(RedisMgr::GetInstance()->Del("bloginfo"));
    assert(RedisMgr::GetInstance()->Del("bloginfo"));
    assert(RedisMgr::GetInstance()->ExistsKey("bloginfo") == false);
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue1"));
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue2"));
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue3"));
    assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->LPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->LPop("lpushkey2", value) == false);
}

int main()
{
	//TestRedis();
    //TestRedisMgr();
    auto& gCfgMgr = ConfigMgr::Inst();//成立配置管理器单类
    std::string gate_port_str = gCfgMgr["GateServer"]["Port"];//利用重载函数来访问
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try {   //捕获异常
        unsigned short port = static_cast<unsigned short>(atoi("8080"));//static_cast<type>(expression)显示类型转换将表达式expression转换成type类型
        net::io_context ioc{ 1 };//事件循环核心，1代表支持单线程//一个ioc对象可以管理多个socket对象，ico内部通过队列存储事件，当有信号发生时，会从队列中取出事件进行处理。
        
        //利用signal_set进行等待信号的注册
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);//创建一个异步信号处理器signal，用于捕获系统信号并在事件循环中处理。其中SIGINT为系统中断信号, SIGTERM为系统终止信号
        //发生了signal_set注册的信号后，会调用此函数
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {//beats'aiso函数回调部分，不会阻塞主线程，如果触发信号则进行下方的lamda函数部分
            //回调参数：const boost::system::error_code& error, int signal_number    //当asio触发时，会自动填充此参数
            if (error) {
                return;
            }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->Start();//创建一个CServer实例，并启动->Start函数     //避免局部对象的生命不足，导致对象被销毁，从而导致内存泄漏所以创建一个shared_ptr对象，将CServer实例作为参数传入，并调用Start函数
        std::cout << "Gate Server listen on port:" << port << std::endl;
        ioc.run();//asio会执行所有异步操作，直到ioc.stop()被调用
    }
    catch(std::exception const&e){
        std::cerr << "ERROR:" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}