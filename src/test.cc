#include"Util.hpp"
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

    string packname = filename + ".lz";
    cloud::FileUtil fu(filename);
    fu.Compress(packname);

    cloud::FileUtil pfu(packname);
    pfu.UnCompress("2.txt");
    return;
}


int main(int argc,char* argv[])
{
    FileUtilTest(argv[1]);
    return 0;
}