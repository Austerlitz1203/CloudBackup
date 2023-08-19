#include"Util.hpp"
#include"Config.hpp"
#include<iostream>
#include<string>
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
    cout<<config->GetServerIP()<<endl;
    cout<<config->GetDownloadPrefix()<<endl;
    cout<<config->GetPackFileSuffix()<<endl;
    cout<<config->GetPackDir()<<endl;
    cout<<config->GetBackDir()<<endl;
    cout<<config->GetBackupFile()<<endl;
}

int main(int argc,char* argv[])
{
    //FileUtilTest(argv[1]);
    //JsonUtilTest();
    ConfigTest();
    return 0;
}