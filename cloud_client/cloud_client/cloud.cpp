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
	cloud::BackUp bu(BACKUP_DIR, BACKUP_FILE);
	bu.RunModule();
	return 0;
}
