#pragma once
#include "const.h"

struct SectionInfo{//用于管理配置文件的单个表，管理配置文件的一项
	SectionInfo(){}
	~SectionInfo(){_section_datas.clear();}

	SectionInfo(const SectionInfo& src)
	{
		_section_datas = src._section_datas;
	}

	SectionInfo& operator=(const SectionInfo& src)
	{
		if (this == &src) {// 判断是否是同一对象
			return *this;
		}

		this->_section_datas = src._section_datas;
		//return *this; //一般习惯返回自身，但是他没写
	}

	std::map<std::string, std::string> _section_datas;
	
	std::string operator[](const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}
		 return _section_datas[key];
	}
};

class ConfigMgr//用于管理SectionInfo类表，配置文件的总管理表
{
public:
	~ConfigMgr()
	{
		_config_map.clear();
	}

	SectionInfo operator[](const std::string& section) {//[]符号的重载函数operator[]，
		if (_config_map.find(section) == _config_map.end()) {
			return SectionInfo();
		}
        return _config_map[section];
	}

	static ConfigMgr& Inst() {//C++11 以后常用的 局部静态变量单例模式实现
		static ConfigMgr cfg_mgr;//生成一次局部的静态变量,保证线程安全，实现单例
		return cfg_mgr;//返回的是cfg_mgr的引用，防止复制。
	}

	ConfigMgr(const ConfigMgr& src) {//拷贝构造函数
        _config_map = src._config_map;
	}

	ConfigMgr& operator=(const ConfigMgr& src) {//拷贝赋值
		if (this == &src) {
			return *this;
		}
		_config_map = src._config_map;
		return *this;
	}
private:
	ConfigMgr();
	std::map<std::string, SectionInfo> _config_map;
};

