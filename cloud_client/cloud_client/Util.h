#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include<stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include<memory>
#include <sys/stat.h>
#include <experimental/filesystem>


namespace cloud
{
    using std::cout;
    using std::endl;
    using std::string;
    namespace fs = std::experimental::filesystem;

    class FileUtil
    {
    public:
        FileUtil(const string& filename = nullptr)
            : _filename(filename)
        {
        }

        bool remove()
        {
            if (this->Exists() == false) return true;
            std::remove(_filename.c_str());
            return true;
        }

        size_t FileSize()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "get file size error \n";
                return -1;
            }
            return st.st_size;
        }

        time_t LastMTime()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "get file last Modify time error \n";
                return -1;
            }
            return st.st_mtime;
        }

        time_t LastATime()
        {
            struct stat st;
            if (stat(_filename.c_str(), &st) < 0)
            {
                std::cout << "get file last Access time error \n";
                return -1;
            }
            return st.st_atime;
        }

        string FileName()
        {
            auto pos = _filename.find_last_of("/");
            if (pos == string::npos)
            {
                return _filename;
            }
            return _filename.substr(pos + 1);
        }

        bool GetPosLen(string* body, size_t pos, size_t len)
        {
            auto fsize = this->FileSize();
            if (pos + len > fsize)
            {
                cout << "读取内容过长" << endl;
                return false;
            }

            std::ifstream ifs;
            ifs.open(_filename, std::ios::binary);
            if (ifs.is_open() == false)
            {
                cout << "GetPosLen open file error" << endl;
                return false;
            }

            ifs.seekg(pos, std::ios::beg); // 从beg跳转到pos位置
            body->resize(len);
            ifs.read(&((*body)[0]), len);
            if (ifs.good() == false)
            {
                cout << "GetPosLen read file error" << endl;
                return false;
            }
            ifs.close();
            return true;
        }

        bool GetContent(string* body)
        {
            auto fsize = this->FileSize();
            return GetPosLen(body, 0, fsize);
        }

        bool SetContent(const string& body)
        {
            std::ofstream ofs;
            ofs.open(_filename, std::ios::binary);
            if (ofs.is_open() == false)
            {
                cout << "SetContent open file error" << endl;
                return false;
            }

            ofs.write(&body[0], body.size());
            if (ofs.good() == false)
            {
                cout << "SetContent write file error" << endl;
                ofs.close();
                return false;
            }
            ofs.close();
            return true;
        }


        bool Exists()
        {
            return fs::exists(_filename);
        }

        bool CreateDirectory()
        {
            if (this->Exists())
                return true;
            return fs::create_directories(_filename);
        }

        bool ScanDirectory(std::vector<std::string>* array)
        {
            this->CreateDirectory();
            for (auto& p : fs::directory_iterator(_filename))
            {
                if (fs::is_directory(p) == true)
                    continue;
                // relative_path 带有路径的文件名
                array->push_back(fs::path(p).relative_path().string());
            }
            return true;
        }

    private:
        string _filename;
    };

    

}
