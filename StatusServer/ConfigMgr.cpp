#include "ConfigMgr.h"
ConfigMgr::ConfigMgr()
{
	boost::filesystem::path current_path = boost::filesystem::current_path();//返回当前的工作目录
	boost::filesystem::path config_path = current_path / "config.ini";//这个类重载了 / 符号为链接
	std::cout << "Config path :" << config_path << std::endl;

	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);//ini文件按照树状读取

	for (const auto& section_pair : pt) { //第一层
		const ::std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section_tree = section_pair.second;//第二层内容
		std::map<std::string, std::string> section_config;//创建同层配置查找表
		for (const auto& key_value_pair : section_tree) {
			const ::std::string& key = key_value_pair.first;
            const ::std::string& value = key_value_pair.second.get_value<std::string>();
            section_config[key] = value;
		}

		SectionInfo section_info;
        section_info._section_datas = section_config;
		_config_map[section_name] = section_info;
    }

	// 输出所有的section和key-value对  
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto& key_value_pair : section_config._section_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
}