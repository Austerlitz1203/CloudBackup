#ifndef __MY_HOT__
#define __MY_HOT__

#include "data.hpp"
#include <unistd.h>

extern cloud::DataManager *_data;
namespace cloud
{
    class HotManager
    {
    private:
        string _pack_dir;    // 压缩目录
        string _back_dir;    // 备份目录
        string _pack_suffix; // 压缩后缀
        time_t _hot_time;

    private:
        bool HotJudge(string filename)
        {
            FileUtil fu(filename);
            time_t atime = fu.LastATime();
            time_t curtime = time(NULL);
            if (curtime - atime > _hot_time) // 非热点文件
                return false;
            return true;
        }

    public:
        HotManager()
        {
            Config *con = Config::GetInstance();
            _pack_dir = con->GetPackDir();
            _back_dir = con->GetBackDir();
            _pack_suffix = con->GetPackFileSuffix();
            _hot_time = con->GetHotTime();
            FileUtil fu1(_pack_dir);
            fu1.CreateDirectory();
            FileUtil fu2(_back_dir);
            fu2.CreateDirectory();
        }

        bool RunModle()
        {

            while (true)
            {
                // 1.遍历备份目录，得到所有文件名
                FileUtil fu(_back_dir);
                std::vector<string> array;
                fu.ScanDirectory(&array);
                // 2.判断得到的文件是否是非热点文件
                for (auto &a : array)
                {
                    if (HotJudge(a))
                    {
                        continue;
                    }
                    // 3.获取非热点文件的备份信息
                    BackupInfo info;
                    if (_data->GetOneByRealpath(a, &info) == false)
                    {
                        // 文件被读取，存在，但是没有备份信息
                        info.NewBackupInfo(a); // 设置备份信息
                    }
                    // 4.非热点文件压缩
                    FileUtil tmp(a);
                    tmp.Compress(info.pack_path);
                    // 5.删除源文件，修改压缩标志位
                    tmp.remove();
                    info.pack_flag = true;
                    _data->Update(info);
                }
                usleep(1000);
            }
        }
    };
}

#endif