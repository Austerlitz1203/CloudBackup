#ifndef __MY_CLOUD__
#define __MY_CLOUD__
#include"data.h"
#include<vector>
#include<Windows.h>

namespace cloud
{
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
			ss << fu.FileName() << " " << fu.FileSize() << " " << fu.LastMTime();
			return ss.str();
		}

		bool RunModule()
		{
			while (true)
			{
				FileUtil fu(_back_dir);
				std::vector<string> array;
				fu.ScanDirectory(&array);
				for (auto& a : array)
				{
					string id = GetFileIdentifier(a);
					_data->Insert(a, id);
				}
				Sleep(1000);
			}
		}
	};
}


#endif