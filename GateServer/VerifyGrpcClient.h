#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyRsp;
using message::GetVarifyReq;
using message::VarifyService;

class RPConPool {
public:
	RPConPool(size_t poolsize, std::string host, std::string port) :poolSize_(poolsize), host_(host), port_(port), b_stop_(false) {
		for (size_t i = 0; i < poolSize_; i++){
			std::shared_ptr<Channel> channel = grpc::CreateChannel(
				host+":"+port, grpc::InsecureChannelCredentials());//创建一个grpc通道
			connections_.push(VarifyService::NewStub(channel));//stub为客户端（Client）用于调用远程服务（Server）的方法接口。记录grpc的通道
		}
	}
	~RPConPool() {
		std::lock_guard<std::mutex> lock(mutex_);
		Close();
		while (!connections_.empty()) {
            connections_.pop();
        }
	}

	void Close() {
		b_stop_ = true;
		cond_.notify_all();//唤醒全部等待的线程
	}

	std::unique_ptr<VarifyService::Stub> getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] { //当前线程等待，直到 predicate 返回 true
			if (b_stop_) {
				return true;
			}
			return !connections_.empty();
			});
		if (b_stop_) {
            return nullptr;
		}

		auto context = std::move(connections_.front());
		connections_.pop();
		return context;
	}

	void returnConnection(std::unique_ptr<VarifyService::Stub> context) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}

        connections_.push(std::move(context));
		cond_.notify_one();//唤醒一个等待的线程
	}
private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	std::string host_;
    std::string port_;
	std::queue<std::unique_ptr<VarifyService::Stub>> connections_;//用于存储stub的队列
	std::mutex mutex_;
	std::condition_variable cond_;//用于线程通信时使用
};

class VerifyGrpcClient:public Singleton<VerifyGrpcClient>
{
	friend class Singleton<VerifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string email) {
        ClientContext context;//控制 RPC 调用的上下文（元数据、超时等
		GetVarifyRsp reply;
		GetVarifyReq request;//定义好的记录email信息的成员类
		request.set_email(email);
		auto stub = pool_->getConnection();
		Status status = stub->GetVarifyCode(&context, request, &reply);
		if (status.ok()) {
			pool_->returnConnection(std::move(stub));
			return reply;
		}
		else {
			pool_->returnConnection(std::move(stub));
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	std::unique_ptr<RPConPool> pool_;
	VerifyGrpcClient();
		//std::shared_ptr<Channel> channel = grpc::CreateChannel(
		//	"127.0.0.1:50051", grpc::InsecureChannelCredentials());//创建一个grpc通道
  //      stub_ = VarifyService::NewStub(channel);
	//std::unique_ptr<VarifyService::Stub> stub_;//独占的指针,Stub相当于一个媒介，只有通过它才能与别人通信
};

