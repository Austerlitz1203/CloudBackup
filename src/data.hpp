#ifndef __MY_DATA__
#define __MY_DATA__

#include <pthread.h>
#include <unordered_map>
#include<vector>
#include "Util.hpp"
#include "Config.hpp"

namespace cloud
{

    typedef struct BackupInfo
    {
        bool pack_flag; // 是否被压缩标志
        size_t fsize;
        time_t mtime;
        time_t atime;
        string real_path;
        string pack_path;
        string url;
        bool NewBackupInfo(const string &realpath)
        {
            FileUtil fu(realpath);
            if (fu.Exists() == false)
            {
                cout << "File not Exists" << endl;
                return false;
            }
            Config *cnf = Config::GetInstance();
            string packdir = cnf->GetPackDir();
            string packsuffix = cnf->GetPackFileSuffix();
            string download_prefix = cnf->GetDownloadPrefix();
            pack_flag = false;
            fsize = fu.FileSize();
            mtime = fu.LastMTime();
            atime = fu.LastATime();
            real_path = realpath;
            // ./backdir/a.txt -> ./packdir/a.txt.lz
            pack_path = packdir + fu.FileName() + packsuffix;
            // ./backdir/a.txt -> ./download/a.txt
            url = download_prefix + fu.FileName();
            return true;
        }

    } BackupInfo;

    class DataManager
    {
    public:
        DataManager()
        {
            _backup_file = Config::GetInstance()->GetBackupFile();
            pthread_rwlock_init(&_rwlock,NULL);
            InitLoad();
        }
        ~DataManager()
        {
            pthread_rwlock_destroy(&_rwlock);
        }

        bool Insert(const BackupInfo& info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }

        bool Update(const BackupInfo& info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _table[info.url] = info;
            pthread_rwlock_unlock(&_rwlock);
            Storage();
            return true;
        }

        bool GetOneByURL(const string& url,BackupInfo* info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            auto it = _table.find(url);
            if(it == _table.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }
            *info = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }

        bool GetOneByRealpath(const string& realpath,BackupInfo* info)
        {
            pthread_rwlock_wrlock(&_rwlock);
            for(auto &it:_table)
            {
                if(it.second.real_path == realpath)
                {
                    *info = it.second;
                    pthread_rwlock_unlock(&_rwlock);
                    return true;
                }
            }
            pthread_rwlock_unlock(&_rwlock);
            return false;
        }

        bool GetAll(std::vector<BackupInfo> *array)
        {
            pthread_rwlock_wrlock(&_rwlock);
            for(auto &it : _table)
            {
                array->push_back(it.second);
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }

        bool Storage()
        {
            // 1.获取所有数据
            std::vector<BackupInfo> array;
            this->GetAll(&array);
            // 2.添加到JsonValue
            Json::Value root;
            for(int i=0;i<array.size();i++)
            {
                Json::Value item;
                item["pack_flag"] = array[i].pack_flag;
                item["fsize"] = (Json::Int64)array[i].fsize; // 转换成 JsonValue 里面的数据
                item["atime"] = (Json::Int64)array[i].atime;
                item["mtime"] = (Json::Int64)array[i].mtime;
                item["real_path"] = array[i].real_path;
                item["pack_path"] = array[i].pack_path;
                item["url"] = array[i].url;
                root.append(item);
            }
            // 3.对JsonValue序列化
            string body;
            JsonUtil::Serialize(root,&body);
            // 4.写文件
            FileUtil fu(_backup_file);
            fu.SetContent(body);
            return true;
        }

        bool InitLoad()
        {
            // 1.将数据文件中的数据读取出来
            FileUtil fu(_backup_file);
            if(fu.Exists() == false) return true;
            string body;
            fu.GetContent(&body);
            // 2.反序列化
            Json::Value root;
            JsonUtil::UnSerialize(body,&root);
            // 3.将JsonValue里的数据添加到_table
            for(int i=0;i<root.size();i++)
            {
                BackupInfo info;
                info.pack_flag = root[i]["pack_flag"].asBool();
                info.fsize = root[i]["fsize"].asInt64();
                info.atime = root[i]["atime"].asInt64();
                info.mtime = root[i]["mtime"].asInt64();
                info.pack_path = root[i]["pack_path"].asString();
                info.real_path = root[i]["real_path"].asString();
                info.url = root[i]["url"].asString();
                Insert(info);
            }
            return true;
        }

    private:
        string _backup_file;
        pthread_rwlock_t _rwlock;
        std::unordered_map<string, BackupInfo> _table;
    };

} // namespace cloud

#endif