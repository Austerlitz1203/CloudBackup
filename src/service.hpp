#ifndef __MY_SERVICE__
#define __MY_SERVICE__
#include "data.hpp"
#include "httplib.h"

extern cloud::DataManager *_data;
namespace cloud
{
    class Service
    {
    private:
        int _server_port;
        string _server_ip;
        string _download_prefix;
        httplib::Server _server;

    private:

        static void Upload(const httplib::Request &req, httplib::Response &resp)
        {
            auto ret = req.has_file("file"); // 判断有无文件上传的区域
            if (ret == false)
            {
                resp.status = 400;
                return;
            }
            const auto &file = req.get_file_value("file");
            // file.filename 文件名称   file.content 文件内容
            string back_dir = Config::GetInstance()->GetBackDir();
            string realpath = back_dir + FileUtil(file.name).FileName();
            FileUtil fu(realpath);
            fu.SetContent(file.content); // 数据写入文件中
            BackupInfo info;
            info.NewBackupInfo(realpath);
            _data->Insert(info); // 向数据管理模块添加备份的文件信息
        }

        static string TimetoStr(time_t t)
        {
            string tmp = std::ctime(&t);
            return tmp;
        }
        static void ListShow(const httplib::Request &req, httplib::Response &resp)
        {
            // 1.获取所有文件的备份信息
            std::vector<BackupInfo> array;
            _data->GetAll(&array);
            // 2.根据备份信息，组织html数据
            std::stringstream ss;
            ss << "<html><head><title>Download</title></head>";
            ss << "<body><h1> Download</ hl><table>";
            for (auto &a : array)
            {
                ss << "<tr>";
                std::string filename = FileUtil(a.real_path).FileName();
                ss << "<td><a href='" << a.url << "'>" << filename << "</a></td>";
                ss << "<td align='right'>" << TimetoStr(a.mtime) << "</td>";
                ss << "<td align='right'>" << a.fsize / 1024 << "k</td>";
                ss << "</tr>";
            }
            ss << "</table></body></html>";
            resp.body = ss.str();
            resp.set_header("Content-Type", "text/html");
            resp.status = 200;
        }
        static void Download(const httplib::Request &req, httplib::Response &resp)
        {

        }

    public:
        Service()
        {
            Config *cnf = Config::GetInstance();
            _server_port = cnf->GetServerPort();
            _server_ip = cnf->GetServerIP();
            _download_prefix = cnf->GetDownloadPrefix();
        }

        bool RunModule()
        {
            _server.Post("/upload", Upload);
            _server.Get("/listshow", ListShow);
            _server.Get("/", ListShow);
            string download_url = _download_prefix + "(.*)";
            _server.Get(download_url, Download);
            _server.listen("0.0.0.0", _server_port);
            return true;
        }
    };
}

#endif
