#include "SyBaseMgr/SyBaseMgr.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"

#include "Util/OmnNew.h"
#include <sybfront.h> //freetds
#include <sybdb.h> //freetds 

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>


const int MaxColumnNums = 100;
const int MaxColumnSize = 100;

bool 
queryCmd(DBPROCESS *dbprocess,const char* strSql)
{    
	dbcmd(dbprocess,strSql);
	if(dbsqlexec(dbprocess) == FAIL)
	{        
		printf("Query error.\n");
		return false;
	}    
	DBINT result_code;    
	char infArr[MaxColumnNums][MaxColumnSize];    
	int retCode = 1;    
	while ((result_code = dbresults(dbprocess)) != NO_MORE_RESULTS)    
	{        
		if (result_code == SUCCEED)        
		{            
			int i=1;            
			int sz = 0;            
			while(true)     
			{               
				retCode = dbbind(dbprocess,i, CHARBIND, (DBINT)0, (BYTE*)infArr[i]);
				if(retCode != 1) break; 
				i++;            
			}
			sz = i;   
			while (dbnextrow(dbprocess) != NO_MORE_ROWS)
			{                
				for(i=1; i<=sz; i++)
				{                    
					printf("%s ",infArr[i]); 
					memset(infArr[i],0,sizeof(infArr[i]));  
				}               
			}        
		}    
	}    
	return true;
} 

bool 
updateCmd(DBPROCESS *dbprocess,const char* strSql)
{    
	dbcmd(dbprocess,strSql);    
	if(dbsqlexec(dbprocess) == FAIL)    
	{        
		printf("error : update fail\n");
		return false; 
	} 
	return true;
}

bool query()
{
	/*
	std::string charset = "UTF-8";
	std::string username = "sa";
	std::string passwd = "123456";
	std::string dbname = "Tester";
	std::string server = "192.168.99.231:1433";
	//dbinit();
	LOGINREC *loginrec = dblogin();
	DBSETLUSER(loginrec, username.data());
	DBSETLPWD(loginrec, passwd.data());
	DBSETLCHARSET(loginrec, charset.data());
	DBPROCESS* dbprocess = dbopen(loginrec, server.data());

	if (dbprocess == FAIL)
	{
		printf("connect to MS SQL SERVER fail\n");
		return 0;
	}
	printf("connect to MS SQL SERVER success\n");

	if (dbuse(dbprocess, dbname.data()) == FAIL)
	{
		printf("open database name [%s] fail\n", dbname.data());
		return 0;
	}
	printf("open database name [%s] success\n", dbname.data());

	bool rslt = queryCmd(dbprocess, "select * from test");
	*/
	return false;
}

int main(int argc, char** argv)
{
	AosSyBaseMgrPtr syBase = OmnNew AosSyBaseMgr(
			"192.168.99.231",
			//"ZYKIE-PC",
			"sa",
			"123456",
			"Tester");
	syBase->initConnection();
	if (syBase->getConnectionStatus())
	{
	int n = 1;
	while(n--)
	{
std::cout << n << " =============================================================" << std::endl;
	vector< vector<AosColumnPtr> > ds = syBase->executeDataSet("select * from test");
	for (u32 i=0; i<ds.size(); i++)
	{
		vector<AosColumnPtr> cols = ds[i];
		std::cout << "<";
		for (u32 j=0; j<cols.size(); j++)
		{
			AosColumnPtr col = cols[j];
			OmnString name = col->getColName();
			OmnString value = col->getColBuffer();
			int type = col->getColType();
			int size = col->getColSize();
			int status = col->getColStatus();
			value.normalizeWhiteSpace(true, true);
			std::cout << name << "=\"" << value << ":" << size << ":" << type << ":" << status << "\" ";
		}
		std::cout << "/>";
		std::cout << endl;
	}
	std::cout << "effect " << ds.size() << std::endl;
	}

	}
	while(1)
	{
		sleep(10);
	}

	/*
	bool rslt = syBase->runSQLCommand("select * from student");
	vector< map<OmnString, OmnString> > rows = syBase->getResult();
	map<OmnString, OmnString> cols;
	map<OmnString, OmnString>::iterator itr;
	cols = rows[0];
	for (itr = cols.begin(); itr != cols.end(); itr++)
	{
		//std::cout << itr->first << "\t\t";
	}
	std::cout << endl;
	for(u32 i=0; i<rows.size(); i++)
	{
		cols = rows[i];
		for (itr = cols.begin(); itr != cols.end(); itr++)
		{
			std::cout << itr->second << "\t";
		}
		std::cout << std::endl;
	}
	rows.clear();
	cols.clear();

	std::cout << "======================================================================" << std::endl;
	rslt = syBase->runSQLCommand("select * from teacher");
	aos_assert_r(rslt, 0);
	rows = syBase->getResult();
	cols = rows[0];
	for (itr = cols.begin(); itr != cols.end(); itr++)
	{
		//std::cout << itr->first << "\t\t";
	}
	std::cout << endl;
	for(u32 i=0; i<rows.size(); i++)
	{
		cols = rows[i];
		for (itr = cols.begin(); itr != cols.end(); itr++)
		{
			std::cout << itr->second << "\t";
		}
		std::cout << std::endl;
	}
	rows.clear();
	cols.clear();

	std::cout << "======================================================================" << std::endl;
	rslt = syBase->runSQLCommand("select * from score");
	aos_assert_r(rslt, 0);
	rows = syBase->getResult();
	cols = rows[0];
	for (itr = cols.begin(); itr != cols.end(); itr++)
	{
		//std::cout << itr->first << "\t\t";
	}
	std::cout << endl;
	for(u32 i=0; i<rows.size(); i++)
	{
		cols = rows[i];
		for (itr = cols.begin(); itr != cols.end(); itr++)
		{
			std::cout << itr->second << "\t";
		}
		std::cout << std::endl;
	}
	rows.clear();
	cols.clear();

	std::cout << "======================================================================" << std::endl;
	rslt = syBase->runSQLCommand("select * from course");
	aos_assert_r(rslt, 0);
	rows = syBase->getResult();
	cols = rows[0];
	for (itr = cols.begin(); itr != cols.end(); itr++)
	{
		//std::cout << itr->first << "\t\t";
	}
	std::cout << endl;
	for(u32 i=0; i<rows.size(); i++)
	{
		cols = rows[i];
		for (itr = cols.begin(); itr != cols.end(); itr++)
		{
			std::cout << itr->second << "\t";
		}
		std::cout << std::endl;
	}
	rows.clear();
	cols.clear();
	*/
	return 0;
}
