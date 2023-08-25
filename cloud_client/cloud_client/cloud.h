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
			//1. ��ȡ�ļ�����
			FileUtil fu(filename);
			string body;
			fu.GetContent(&body);
			//2. � http �ͻ����ϴ��ļ�
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
			// �ļ��������Ļ����޸Ĺ��ģ�����Ҫ�ϴ�
			string id;
			if (_data->GetOneByKey(filename, &id)) {
				//������ʷ��Ϣ
				string new_id = GetFileIdentifier(filename);
				if (new_id == id) { // һ�£�����Ҫ�ϴ�
					return false;
				}
			}

			// ��Ҫ�ϴ�
			// ���ÿ�α������жϱ�ʶ��һ�£���ôһ����ʮG���ļ�����ÿ�μ���ʶ����һ��
			// Ҫһֱ�ϴ�������Ҫһ��ʱ��û�б��޸Ĳ����ϴ�
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
				// ����ָ���ļ����е��ļ�
				FileUtil fu(_back_dir);
				std::vector<string> array;
				fu.ScanDirectory(&array);
				for (auto& a : array)
				{
					// �ж��ļ��Ƿ���Ҫ�ϴ�
					if (IsNeedUpload(a) == false) {
						continue;
					}
					// �����ϴ����ϴ��ļ�
					if (Upload(a) == true) {
						_data->Insert(a, GetFileIdentifier(a)); //�����ļ�������Ϣ
						cout << a << "upload success!" << endl;
					}
				}
				Sleep(1);
			}
		}
	};
}


#endif