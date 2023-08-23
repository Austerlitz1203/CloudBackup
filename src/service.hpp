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

        static void Upload(const httplib::Request &req, httplib::Response &rsp)
        {
            auto ret = req.has_file("file"); // 判断有无文件上传的区域
            if (ret == false)
            {
                rsp.status = 400;
                return;
            }
            const auto &file = req.get_file_value("file");
            // file.filename 文件名称   file.content 文件内容
            string back_dir = Config::GetInstance()->GetBackDir();
            string realpath = back_dir + FileUtil(file.filename).FileName();
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
        static void ListShow(const httplib::Request &req, httplib::Response &rsp)
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
            rsp.body = ss.str();
            rsp.set_header("Content-Type", "text/html");
            rsp.status = 200;
        }

        static string GetETag(const BackupInfo &info)
        {
            // etag: filename-fsize-mtime
            FileUtil fu(info.real_path);
            string etag = fu.FileName();
            etag+="-";
            etag+=std::to_string(info.fsize);
            etag+="-";
            etag+=std::to_string(info.atime);
            return etag;
        }

        static void Download(const httplib::Request &req, httplib::Response &rsp)
        {
            //1.获取客户端请求的资源路径path，  req.path
            //2.根据资源路劲，获取文件备份信息
            BackupInfo info;
            _data->GetOneByURL(req.path,&info);
            //3.判断文件是否被压缩，如果被压缩，要先解压缩
            if(info.pack_flag == true){
                FileUtil fu(info.pack_path);
                fu.UnCompress(info.real_path);
                // 4.删除压缩包，修改备份信息
                fu.remove();
                info.pack_flag = false;
                _data->Update(info);
            }

             // 判断是否需要断点续传
            bool retrans = false;
            string old_etag;
            if(req.has_header("If-Range")){
                old_etag = req.get_header_value("If-Range");
                // 有断点续传标志，判断etag是否改变
                if(old_etag == GetETag(info))
                    retrans = true;
            }
            // 5.读取文件数据，放入rsp.body中
            FileUtil fu(info.real_path);
            if(retrans == false){
                fu.GetContent(&rsp.body);
                // 6.设置响应头部字段：ETag  Accept-Ranges：bytes
                rsp.set_header("Accept-Ranges", "bytes");
                rsp.set_header("ETag", GetETag(info));
                rsp.set_header("Content-Type", "application/octet-stream"); // 告诉浏览器是一个下载，而不是直接渲染
                rsp.status = 200;
            }
            else{
                fu.GetContent(&rsp.body);
                // httplib内部实现了对于区间请求断点续传的处理
                // 只需要用户将文件所有数据读取到rsp.body中，它内部会自动根据请求区间，从body中取出指定区间数据进行响应
                rsp.set_header("Accept-Ranges", "bytes");
                rsp.set_header("ETag", GetETag(info));
                rsp.set_header("Content-Type", "application/octet-stream"); // 告诉浏览器是一个下载，而不是直接渲染
                rsp.status = 206;
            }
        }

    public:
        Service()
        {
            Config *config = Config::GetInstance();
            _server_port = config->GetServerPort();
            _server_ip = config->GetServerIp();
            _download_prefix = config->GetDownloadPrefix();
        }
        bool RunModule()
        {
            _server.Post("/upload", Upload);
            _server.Get("/listshow", ListShow);
            _server.Get("/", ListShow);
            std::string download_url = _download_prefix + "(.*)";
            _server.Get(download_url, Download);
            _server.listen("0.0.0.0", _server_port);
            return true;
        }
    };
}

#endif
