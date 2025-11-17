#pragma once
#include <fstream>  
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp>  
#include <boost/filesystem.hpp>    
#include <map>
#include <iostream>

struct SectionInfo {
	SectionInfo() {}
	~SectionInfo() {
		_section_datas.clear();
	}

	SectionInfo(const SectionInfo& src) {
		_section_datas = src._section_datas;
	}

	SectionInfo& operator = (const SectionInfo& src) {
		if (&src == this) {
			return *this;
		}

		this->_section_datas = src._section_datas;
		return *this;//赋值运算符的返回类型通常是 *this，即返回左操作数的引用，这样可以支持链式赋值（如 a = b = c;）
	}

	std::map<std::string, std::string> _section_datas;
	std::string  operator[](const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}
		// 这里可以添加一些边界检查  
		return _section_datas[key];
	}

	std::string GetValue(const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}
		// 这里可以添加一些边界检查  
		return _section_datas[key];
	}
};

class ConfigMgr
{
public:
	~ConfigMgr() {
		_config_map.clear();
	}
	SectionInfo operator[](const std::string& section) {
		if (_config_map.find(section) == _config_map.end()) {
			return SectionInfo();//返回一个空的默认构造
		}
		return _config_map[section];
	}


	ConfigMgr& operator=(const ConfigMgr& src) {
		if (&src == this) {
			return *this;
		}

		this->_config_map = src._config_map;
	};

	ConfigMgr(const ConfigMgr& src) {
		this->_config_map = src._config_map;
	}

	static ConfigMgr& Inst() {//static 关键字表示该变量的生命周期与整个程序的生命周期相同，且只会初始化一次。即它在第一次调用 Inst() 时被创建，之后每次调用 Inst() 时返回的都是同一个 cfg_mgr 实例。
		static ConfigMgr cfg_mgr;
		return cfg_mgr;
	}

	std::string GetValue(const std::string& section, const std::string& key);
private:
	ConfigMgr();
	// 存储section和key-value对的map  
	std::map<std::string, SectionInfo> _config_map;
};

