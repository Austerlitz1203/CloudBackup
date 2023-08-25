#include"Util.h"
#include"data.h"
#include"cloud.h"
#include"httplib.h"
using namespace std;
using namespace cloud;

#define BACKUP_FILE "./backup.dat"
#define BACKUP_DIR "./backup/"
int main()
{
	//FileUtil fu("./");
	//vector<string> v;
	//fu.ScanDirectory(&v);
	//DataManager dm(BACKUP_FILE);
	//for (auto &a : v)
	//{
	//	dm.Insert(a, "woshishiejdioa");
	//}

	//DataManager dm(BACKUP_FILE);
	//string val;
	//dm.GetOneByKey(".\\Util.h",&val);
	//cout << val << endl;

	cloud::BackUp bu(BACKUP_DIR, BACKUP_FILE);
	bu.RunModule();
	return 0;
}