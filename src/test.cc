#include"Util.hpp"
#include"Config.hpp"
#include"data.hpp"
#include"hot.hpp"
#include"service.hpp"
#include<iostream>
#include<string>
#include<thread>
using namespace std;

void FileUtilTest(const std::string& filename)
{
    // cloud::FileUtil fu(filename);
    // cout<<fu.FileSize()<<endl;
    // cout<<fu.LastMTime()<<endl;
    // cout<<fu.LastATime()<<endl;
    // cout<<fu.FileName()<<endl;


    // cloud::FileUtil fu(filename);
    // string body;
    // fu.GetContent(&body);

    // cloud::FileUtil dfu("1.txt");
    // dfu.SetContent(body);



    // string packname = filename + ".lz";
    // cloud::FileUtil fu(filename);
    // fu.Compress(packname);

    // cloud::FileUtil pfu(packname);
    // pfu.UnCompress("2.txt");

    cloud::FileUtil fu(filename);
    fu.CreateDirectory();
    vector<string> array;
    fu.ScanDirectory(&array);
    for(auto s:array)
    {
        cout<<s<<endl;
    }

    return;
}


void JsonUtilTest()
{
    Json::Value root;
    float score[] = {22.5,80,99.5};
    root["姓名"] = "小明";
    root["年龄"] = 20;
    root["成绩"].append(score[0]);
    root["成绩"].append(score[1]);
    root["成绩"].append(score[2]);
    string s;
    cloud::JsonUtil::Serialize(root,&s);
    cout<<s<<endl;

    Json::Value val;
    cloud::JsonUtil::UnSerialize(s,&val);
    cout<<val["姓名"].asString()<<endl;
    cout<<val["年龄"].asFloat()<<endl;
    for(int i=0;i<root["成绩"].size();i++)
    {
        cout<<root["成绩"][i]<<endl;
    }
    return;
}

void ConfigTest()
{
    cloud::Config *config = cloud::Config::GetInstance();
    cout<<config->GetHotTime()<<endl;
    cout<<config->GetServerPort()<<endl;
    cout<<config->GetServerIp()<<endl;
    cout<<config->GetDownloadPrefix()<<endl;
    cout<<config->GetPackFileSuffix()<<endl;
    cout<<config->GetPackDir()<<endl;
    cout<<config->GetBackDir()<<endl;
    cout<<config->GetBackupFile()<<endl;
}

void DataTest(const string& filename)
{
    // cloud::BackupInfo info;
    // info.NewBackupInfo(filename);
    // cloud::DataManager dm;
    // dm.Insert(info);

    // cloud::BackupInfo tmp;
    // dm.GetOneByURL("/download/bundle.h",&tmp);
    // cout << "----------------------getonebyurl---------------------" << endl;
    // cout<<tmp.pack_flag<<endl;
    // cout<<tmp.atime<<endl;
    // cout<<tmp.mtime<<endl;
    // cout<<tmp.pack_path<<endl;
    // cout<<tmp.real_path<<endl;
    // cout << tmp.fsize<< endl;
    // cout << tmp.url<< endl;

    // info.pack_flag = true;
    // dm.Update(info);
    // vector<cloud::BackupInfo> array;
    // dm.GetAll(&array);
    // cout << "----------------------getall---------------------" << endl;
    // for(auto &a:array)
    // {
    //     cout << a.pack_flag << endl;
    //     cout << a.atime << endl;
    //     cout << a.mtime << endl;
    //     cout << a.pack_path << endl;
    //     cout << a.real_path << endl;
    //     cout << a.fsize << endl;
    //     cout << a.url << endl;
    // }

    // cout<<"----------------------realpath---------------------"<<endl;
    // dm.GetOneByRealpath(filename,&tmp);
    // cout << tmp.pack_flag << endl;
    // cout << tmp.atime << endl;
    // cout << tmp.mtime << endl;
    // cout << tmp.pack_path << endl;
    // cout << tmp.real_path << endl;
    // cout << tmp.fsize << endl;
    // cout << tmp.url << endl;


    cloud::DataManager dm;
    vector<cloud::BackupInfo> array;
    dm.GetAll(&array);
    cout << "----------------------getall---------------------" << endl;
    for(auto &a:array)
    {
        cout << a.pack_flag << endl;
        cout << a.atime << endl;
        cout << a.mtime << endl;
        cout << a.pack_path << endl;
        cout << a.real_path << endl;
        cout << a.fsize << endl;
        cout << a.url << endl;
    }
}

cloud::DataManager *_data;
void HotTest()
{
    cloud::HotManager hot;
    hot.RunModle();
}

void ServiceTest()
{
    cloud::Service srv;
    srv.RunModule();
}

int main(int argc,char* argv[])
{
    _data = new cloud::DataManager();
    //FileUtilTest(argv[1]);
    //JsonUtilTest();
    //ConfigTest();
    //DataTest(argv[1]);
    //HotTest();
    //ServiceTest();
    thread thread_hot(HotTest);
    thread thread_service(ServiceTest);
    thread_hot.join();
    thread_service.join();
    return 0;
}
