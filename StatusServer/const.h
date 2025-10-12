#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <map>
#include <functional>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "hiredis.h"
#include <cassert>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>#pragma once

enum ErrorCodes {
    SUCCESS = 0,
    Error_Json = 1001,
    RPCFailed = 1002,
    VarifyExpired = 1003,
    VarifyCodeErr = 1004,
    UserExist = 1005,
    PasswdErr = 1006,
    EmailNotMatch = 1007,
    PasswdUpdateFailed = 1008,
    PasswdInvalid = 1009,
    UidInvalid = 1010,
    TokenInvalid = 1011,
};

#define CODEPREFIX "code_"

class Defer {
public:
    Defer(std::function<void()> func) :func_(func){}
    ~Defer() {
        func_();
    }
private:
    std::function<void()> func_;
};
//class ConfigMgr;//不需要扩展为单例后删除
//extern ConfigMgr gCfgMgr;