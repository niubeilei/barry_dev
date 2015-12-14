
#include "Porting/Mutex.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif
void getfirstlevel(const string &in, string &out){cout << "111111111" << endl;} //return 1.1;}

float chending(float arg) 
{
	cout << "results: " << arg * 100 << endl; 
	float rslt = arg*100;
	cout << "finished: " << rslt << endl;
	return rslt;
}

bool ChenDingTest(const AosRundataPtr &rdata)
{
	cout << "This is Chen Ding Testing: " << rdata->getSiteid() << endl;
	rdata->setSiteid(200);
	OmnScreen << "Siteid: " << rdata->getSiteid() << endl;
	cout << "Siteid: " << rdata->getSiteid() << endl;
	return true;
}
#ifdef __cplusplus
}
#endif


//-shared
//编译动态库文件
// g++ -DOMN_PLATFORM_UNIX -I../.. -shared -Wall Test.cpp -o libTest.so
// 显示动态库中函数名
// nm -D libTest.so
//
// gcc -fpic -shared -Wall -I../.. -DOMN_PLATFORM_UNIX Test.cpp -o libTest.so
//
//
//
