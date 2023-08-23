#ifndef __MY_CONFIG__
#define __MY_CONFIG__

#include <mutex>
#include "Util.hpp"

namespace cloud
{
    #define ConfFile "./cloud.conf"

    class Config
    {
    private:
        Config()
        {
            ReadConfigFile();
        }
        static Config *_instance;
        static std::mutex _mutex;

    private:
        int _hot_time;
        int _server_port;
        string _server_ip;
        string _download_prefix;
        string _packfile_suffix;
        string _pack_dir;
        string _back_dir;
        string _backup_file;

        bool ReadConfigFile()
        {
            FileUtil fu(ConfFile);
            string body;
            if (fu.GetContent(&body) == false)
            {
                cout << "ReadConf GetContent error" << endl;
                return false;
            }

            Json::Value root;
            if (JsonUtil::UnSerialize(body, &root) == false)
            {
                cout << "ReadConf UnSerialize error" << endl;
                return false;
            }

            _hot_time = root["hot_time"].asInt();
            _server_port = root["server_port"].asInt();
            _server_ip = root["server_ip"].asString();
            _download_prefix = root["download_prefix"].asString();
            _packfile_suffix = root["packfile_suffix"].asString();
            _pack_dir = root["pack_dir"].asString();
            _back_dir = root["back_dir"].asString();
            _backup_file = root["backup_file"].asString();

            return true;
        }

    public:
        static Config *GetInstance()
        {
            if (_instance == NULL)
            {
                _mutex.lock();
                if (_instance == NULL)
                {
                    _instance = new Config();
                }
                _mutex.unlock();
            }
            return _instance;
        }

        int GetHotTime()
        {
            return _hot_time;
        }

        int GetServerPort()
        {
            return _server_port;
        }

        string GetServerIp()
        {
            return _server_ip;
        }

        string GetDownloadPrefix()
        {
            return _download_prefix;
        }

        string GetPackFileSuffix()
        {
            return _packfile_suffix;
        }

        string GetPackDir()
        {
            return _pack_dir;
        }

        string GetBackDir()
        {
            return _back_dir;
        }

        string GetBackupFile()
        {
            return _backup_file;
        }
    };

    Config* Config::_instance = NULL;
    std::mutex Config::_mutex;
}

#endif