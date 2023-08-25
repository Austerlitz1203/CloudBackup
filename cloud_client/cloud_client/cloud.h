#ifndef __MY_CLOUD__
#define __MY_CLOUD__
#include"data.h"
#include"httplib.h"
#include<vector>
#include<Windows.h>

namespace cloud
{
#define SERVER_ADDR "1.15.119.209"
#define SERVER_PORT 9090

	class BackUp
	{
	private:
		string _back_dir;
		DataManager* _data;
	public:
		BackUp(const string& back_dir, const string& back_file)
			: _back_dir(back_dir)
		{
			_data = new DataManager(back_file);
		}
		
		string GetFileIdentifier(const string& filename)
		{
			// filename-size-mtime
			FileUtil fu(filename);
			std::stringstream ss;
			ss << fu.FileName() << "-" << fu.FileSize() << "-" << fu.LastMTime();
			return ss.str();
		}

		bool Upload(const string& filename)
		{
			//1. 获取文件数据
			FileUtil fu(filename);
			string body;
			fu.GetContent(&body);
			//2. 搭建 http 客户端上传文件
			httplib::Client client(SERVER_ADDR, SERVER_PORT);
			httplib::MultipartFormData item;
			item.content = body;
			item.name = "file";
			item.filename = fu.FileName();
			item.content_type = "application/octet-stream";
			httplib::MultipartFormDataItems items;
			items.push_back(item);

			auto res = client.Post("/upload", items);
			if (!res || (res->status) != 200) {
				return false;
			}
			return true;
		}

		bool IsNeedUpload(const string &filename)
		{
			// 文件是新增的或者修改过的，就需要上传
			string id;
			if (_data->GetOneByKey(filename, &id)) {
				//存在历史信息
				string new_id = GetFileIdentifier(filename);
				if (new_id == id) { // 一致，不需要上传
					return false;
				}
			}

			// 需要上传
			// 如果每次遍历都判断标识不一致，那么一个几十G的文件，在每次检测标识都不一致
			// 要一直上传，所以要一段时间没有被修改才能上传
			FileUtil fu(filename);
			if (time(NULL) - fu.LastMTime() < 3)
				return false;
			cout << filename <<" need upload" << endl;
			return true;
		}

		bool RunModule()
		{
			while (true)
			{
				// 遍历指定文件夹中的文件
				FileUtil fu(_back_dir);
				std::vector<string> array;
				fu.ScanDirectory(&array);
				for (auto& a : array)
				{
					// 判断文件是否需要上传
					if (IsNeedUpload(a) == false) {
						continue;
					}
					// 如需上传则上传文件
					if (Upload(a) == true) {
						_data->Insert(a, GetFileIdentifier(a)); //新增文件备份信息
						cout << a << "upload success!" << endl;
					}
				}
				Sleep(1);
			}
		}
	};
}


#endif