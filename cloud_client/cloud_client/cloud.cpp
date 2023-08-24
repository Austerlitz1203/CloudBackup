#include"Util.h"
#include"data.h"
using namespace std;
using namespace cloud;

#define BACKUP_FILE "./backup.dat"
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

	DataManager dm(BACKUP_FILE);
	string val;
	dm.GetOneByKey(".\\Util.h",&val);
	cout << val << endl;
	return 0;
}