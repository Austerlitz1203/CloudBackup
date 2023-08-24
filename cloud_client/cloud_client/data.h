#ifndef __MY_DATA__
#define __MY_DATA__
#include"Util.h"
#include<unordered_map>
#include<sstream>
namespace cloud
{
	using std::string;
	class DataManager
	{
	private:
		string _back_file; // 备份信息的持久化存储文件
		std::unordered_map<string, string> _table;
	public:
		DataManager(const string& backfile)
			:_back_file(backfile)
		{
			InitLoad();
		}

		bool Storage()
		{
			// 1.获取所有的备份信息
			std::stringstream ss;
			auto it = _table.begin();
			for (; it != _table.end(); it++)
			{
				// 按照指定格式组织（序列化）
				ss << it->first << " " << it->second << endl;
			}
			// 2.持久化存储
			FileUtil fu(_back_file);
			fu.SetContent(ss.str());
			return true;
		}

		int Split(const string& str, const string& sep, std::vector<string>* array)
		{
			int count = 0;
			int idx = 0, pos = 0;
			while (true)
			{
				pos = str.find(sep,idx);
				if (pos == std::string::npos)
					break;
				if (pos == idx)
				{
					idx = pos + sep.size();
					continue;
				}
				string tmp = str.substr(idx, pos - idx);
				array->push_back(tmp);
				count++;
				idx = pos + sep.size();
			}
			if (idx < str.size())
			{
				array->push_back(str.substr(idx));
				count++;
			}
			return count;
		}

		bool InitLoad()
		{
			// 读取文件数据
			FileUtil fu(_back_file);
			string body;
			fu.GetContent(&body);
			// 按照格式分割
			std::vector<string> array;
			Split(body, "\n", &array);
			for (auto &a : array)
			{
				std::vector<string> tmp;
				Split(a, " ", &tmp);
				if (tmp.size() != 2)
					continue;
				_table[tmp[0]] = tmp[1];
			}
			return true;
		}

		bool Insert(const string& key,const string& val)
		{
			_table[key] = val;
			Storage();
			return true;
		}

		bool Upload(const string& key, const string& val)
		{
			_table[key] = val;
			Storage();
			return true;
		}

		bool GetOneByKey(const string& key,string* val)
		{
			auto it = _table.find(key);
			if (it == _table.end()) return false;
			*val = it->second;
			return true;
		}

	};
}




#endif // !__MY_DATA__
