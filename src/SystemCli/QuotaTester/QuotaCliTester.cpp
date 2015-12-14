////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// File Name: QuotaCliTester.cpp
// Description:
// Below are the list of all the CLI commands:
//
// quota
//1. disk mgr quota status <status> <partition>
//2. disk mgr quota status show
//3. disk mgr quota list
//4. disk mgr quota add user
//5. disk mgr quota delete user
//6. disk mgr quota add dir
//7. disk mgr quota delete dir
//8. disk mgr quota show user
//9. disk mgr quota show dir
//10. disk mgr quota show config
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#include "SystemCli/Tester/QuotaCliTesterSuite.h"
#include "SystemCli/QuotaTester/QuotaCliTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "Util/Random.h"
#include "UtilComm/Ptrs.h"
#include "KernelSimu/timer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

// 
// CLI Command selector definition
//
static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  20</Element>"		// set status 
	        "<Element>2,  10</Element>"		// show status
	        "<Element>3,  5</Element>"		// list info 
	        "<Element>4,  15</Element>"		// add user limit 
	        "<Element>5,  10</Element>"		// delete user limit 
	        "<Element>6,  15</Element>"		// add dir 
	        "<Element>7,  10</Element>"		// delete dir
	        "<Element>8,  5</Element>"		// show user limit
	        "<Element>9,  5</Element>"		// show directory limit
	        "<Element>10, 5</Element>"		// show all config
		"</Elements>";


QuotaCliTester::QuotaCliTester()
{
	mPartNum = 0;
	partNum = 0;
	userNum = 0;
}


QuotaCliTester::~QuotaCliTester()
{
}


bool QuotaCliTester::start()
{
	quotaTorturerInit();
	basicTest();
	return true;
}


bool QuotaCliTester::basicTest()
{
	OmnBeginTest << "Test Quota";
	
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef, 100);

	int count = 0;
	bool rslt;
	while (count < sgCount)
	{
			cout <<"========================== Now run the :"<<count+1<<" time =================================================="<<endl;
			
		sleep(1);
		int index = cliSelector.nextInt();
		switch (index)
		{
		case diskMgrQuotaStatus_ID:
			 rslt = diskMgrQuotaStatus();
			 break;

		case diskMgrQuotaStatusShow_ID:
			rslt = diskMgrQuotaStatusShow();
			break;

		case diskMgrQuotalist_ID:
			rslt = diskMgrQuotaList();
			break;
			
		case diskMgrQuotaAddUser_ID:
			rslt = diskMgrQuotaAddUser();
			break;

		case diskMgrQuotaDelUser_ID:
			rslt = diskMgrQuotaDeleteUser();
			break;

		case diskMgrQuotaAddDir_ID:
			rslt = diskMgrQuotaAddDir();
			break;
			
		case diskMgrQuotaDelDir_ID:
			rslt = diskMgrQuotaDelDir();
			break;
			
		case diskMgrQuotaShowUser_ID: 
			rslt = diskMgrQuotaShowUser();
			break;
			
		case diskMgrQuotaShowDir_ID: 
			rslt = diskMgrQuotaShowDir();
			break;
			
		case diskMgrQuotaShow_ID: 
			rslt = diskMgrQuotaShow();
			break;
		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			 break;
		}

		if (!rslt)
		{
			cout << "quota torturer, some error occur " << index << endl;
			return false;
		}

		count++;
	}

	return true;
}


bool 
QuotaCliTester::saveQuotaStatus(OmnString &partition, OmnString &status)
{
	int i;
	bool isFound = false;
	for (i=0; i<mPartNum; i++)
	{
		if (mPart[i].mName == partition)
		{
			isFound = true;
			mPart[i].status = status;
			mPart[i].userNum = 0;
			mPart[i].dirNum = 0;
			break;
		}		
	}
		
	if(!isFound)
	{
		mPart[mPartNum].mName = partition;
		mPart[mPartNum].status = status;
		mPart[mPartNum].userNum = 0;
		mPart[mPartNum].dirNum = 0;
		mPartNum++;
	}

	return true;
}


bool	
QuotaCliTester::diskMgrQuotaStatus()
{
	OmnString status;
	OmnString partition;
	OmnString cmd = "disk mgr quota status ";
	bool isCorrect = true;
	int i;

	genStatus(status, isCorrect);
	genPart(partition, isCorrect);

	//
	//1, the status is already, the command run failed
	//
	if (statusIsAlready(partition, status) && isCorrect)
		isCorrect = false;
	//
	//save the command
	//
	if (isCorrect)
		saveQuotaStatus(partition, status);

	cmd << status << " " << partition;
	aosRunCli(cmd, isCorrect);

	if (isCorrect)
		cout << cmd << endl;
	
	return true;
}


bool	
QuotaCliTester::diskMgrQuotaStatusShow()
{
	char *p, result[1024];
	int i;
	OmnString rslt;
	OmnString mountPoint;
	OmnString cmd = "disk mgr quota status show";
	aosRunCli_getStr(cmd, true, rslt);
	memset(result, 0, sizeof(result));

	strcpy(result, rslt.data());
	p = strtok(result, "\n");
	p = strtok(NULL, "\n");
	while (p)
	{
		sscanf(p, "%*s %s", mountPoint.getBuffer());

		for (i=0; i<mPartNum; i++)
		{
			if (mPart[i].mName == mountPoint)
				if (mPart[i].status != "on")
				{
					cout << "Error occur" << endl;
				}
		}

		p = strtok(NULL, "\n");
	}

	return true;
}


bool	
QuotaCliTester::diskMgrQuotaList()
{
	OmnString cmd = "disk mgr quota list";
	aosRunCli(cmd, true);
	return true;
}


bool	
QuotaCliTester::diskMgrQuotaAddUser()
{
	//
	//disk mgr quota add user <partition> <user> <size>[M|m|K|k]
	//
	OmnString partition, user, size;
	OmnString cmd =  "disk mgr quota add user ";
	bool isCorrect = true;
	int partIndex, i, j;
	bool isFound = false;
	
	isCorrect = true;
	
	//create partition
	int randomNum = RAND_INT(1, 100);
	if (randomNum < 90 && mPartNum > 0)
	{
		partIndex = RAND_INT(0, mPartNum-1);
		partition = mPart[partIndex].mName;
		if (mPart[partIndex].status == "off")
			isCorrect = false;
	}
	else
	{
		partition = OmnRandom::nextLetterStr(5, 10); 
		isCorrect = false; 
	}
	//create user
	genUser(user, isCorrect);
	
	// create size
	genSize(size, isCorrect);
	cmd << partition << " " << user << " " << size;
	
	//save the user
	if (isCorrect)
	{
		for(i=0; i<mPart[partIndex].userNum; i++)
		{
			if (mPart[partIndex].users[i].mName == user)
			{
				isFound = true;
				mPart[partIndex].users[i].size = size;
			}
		}
		
		if (!isFound)
		{
			i = mPart[partIndex].userNum;
			mPart[partIndex].users[i].mName = user;
			mPart[partIndex].users[i].size = size;
			mPart[partIndex].userNum++;
		}
	}


	if (isCorrect)
		cout << cmd << endl;
	
	aosRunCli(cmd, isCorrect);
	return true;
}


bool	
QuotaCliTester::diskMgrQuotaDeleteUser()
{
	//
	//disk mgr quota delete user <partition> <user> <size>[M|m|K|k]
	//
	OmnString cmd = "disk mgr quota delete user ";
	
	OmnString partition, user;
	bool isCorrect = true;
	int partIndex, userIndex;
	
	int randomNum = RAND_INT(1, 100);
	if (randomNum > 20 && mPartNum >0)
	{
		partIndex = RAND_INT(0, mPartNum-1);
		partition = mPart[partIndex].mName;
		if (mPart[partIndex].status == "off")
			isCorrect = false;
	}
	else
	{
		partition = OmnRandom::nextLetterStr(10, 20); 
		isCorrect = false; 
	}
	
	//create user
	if ((mPart[partIndex].userNum > 0) && (RAND_INT(0, 100) < 90))
	{	
		userIndex = RAND_INT(0, mPart[partIndex].userNum-1);
		user = mPart[partIndex].users[userIndex].mName;
	}
	else
	{
		user = OmnRandom::nextLetterStr(5, 10);
		isCorrect = false;
	} 	
	
	cmd << partition << " " << user;
	if (isCorrect)
		cout << cmd << endl;
	aosRunCli(cmd, isCorrect);

	//save 
	if (isCorrect)
	{
		for (int i=userIndex; i<mPart[partIndex].userNum; i++)
		{
			mPart[partIndex].users[i] = mPart[partIndex].users[i+1];
		}
		mPart[partIndex].userNum--;
	}
	
	return true;
}

bool	
QuotaCliTester::diskMgrQuotaAddDir()
{
	//
	//disk mgr quota add dir <partition> <dir>
	//
	OmnString cmd = "disk mgr quota add dir ";
	OmnString partition, dir, size;
	bool isCorrect = true;
	int partIndex, i;
	bool isFound = false;
		
	//create partition
	if (RAND_INT(0, 100) < 90 && mPartNum >0)
	{
		partIndex = RAND_INT(0, mPartNum-1);
		partition = mPart[partIndex].mName;
		if (mPart[partIndex].status == "off")
			isCorrect = false;
	}
	else
	{
		partition = OmnRandom::nextLetterStr(5, 10); 
		isCorrect = false; 
	}

	// create dir
	genDir(partition, dir, isCorrect);

	//create size
	genSize(size, isCorrect);
	
	cmd << partition << " " << dir << " " << size;
	
	//save the comand
	if (isCorrect)
	{
		for(i=0; i<mPart[partIndex].dirNum; i++)
		{
			if (mPart[partIndex].dirs[i].mName == dir)
			{
				isFound = true;
				mPart[partIndex].dirs[i].size = size;
			}
		}
		if (!isFound)
		{
			mPart[mPartNum].dirs[userNum].mName = dir;
			mPart[mPartNum].dirs[userNum].size = size;
			mPart[mPartNum].dirNum++;
		}
	}
	aosRunCli(cmd, isCorrect);
	if (isCorrect)
		cout << cmd << endl;
	return true;
}

bool	
QuotaCliTester::diskMgrQuotaDelDir()
{
	//
	//disk mgr quota delete dir <dir> <partition>
	//
	OmnString cmd = "disk mgr quota delete dir ";
	OmnString partition, dir;
	bool isCorrect = true;
		
	int partIndex, i, dirIndex;
	//create partition
	int randomNum = RAND_INT(1, 100);
	if (randomNum > 20 && mPartNum >0)
	{
		partIndex = RAND_INT(0, mPartNum-1);
		partition = mPart[partIndex].mName;
		if (mPart[partIndex].status == "off")
			isCorrect = false;
	}
	else
	{
		partition = OmnRandom::nextLetterStr(10, 20); 
		isCorrect = false; 
	}
	
	//create directory
	if (mPart[partIndex].dirNum > 0 && RAND_INT(0, 100) < 90)
	{
		dirIndex = RAND_INT(0, mPart[partIndex].dirNum-1);
		dir = mPart[partIndex].dirs[dirIndex].mName;
	}
	else
	{
		dir = OmnRandom::nextLetterStr(5, 10); 
		isCorrect = false;
	} 
	
	//create command
	cmd << partition << " " << dir;
	
	//save command
	if (isCorrect)
	{
		for (i=dirIndex; i<mPart[partIndex].dirNum; i++)
		{
			mPart[partIndex].dirs[i] = mPart[partIndex].dirs[i+1];
		}
		mPart[partIndex].dirNum--;
	}
	
	aosRunCli(cmd, isCorrect);
	if (isCorrect)
		cout << cmd << endl;
	//save 
	return true;
}


bool	
QuotaCliTester::diskMgrQuotaShowUser()
{
	OmnString user;
	OmnString cmd = "disk mgr quota show user ";
	bool isCorrect = true;
	int userIndex;
	
	int randomNum = RAND_INT(0, 100);
	if (randomNum > 20 && userNum >0)
	{
		userIndex = RAND_INT(0, userNum-1);
		user = existUsers[userIndex];
	}
	else
	{
		user = OmnRandom::nextLetterStr(5, 10); 
		isCorrect = false;
	}
	
	cmd << user;
	aosRunCli(cmd, isCorrect);
	return true;
}

bool	
QuotaCliTester::diskMgrQuotaShowDir()
{
	OmnString partition;
	OmnString dir;
	OmnString cmd = "disk mgr quota show dir ";
	
	bool isCorrect = true;
	int randomNum = RAND_INT(0, 100);
	
	if (mPartNum >0)
	{
		partition = mPart[RAND_INT(0, mPartNum-1)].mName;
		genDir(partition, dir, isCorrect);
	}
	else
		return true;
	cmd << dir;
	aosRunCli(cmd, true);
	return true;
}

bool	
QuotaCliTester::diskMgrQuotaShow()
{
	OmnString cmd = "disk mgr quota show config";
	aosRunCli(cmd, true);
	return true;
}


bool	
QuotaCliTester::diskMgrQuotaClearConfig()
{
	OmnString cmd = "disk mgr quota clear config";
	aosRunCli(cmd, true);
}


bool	
QuotaCliTester::genStatus(OmnString &status, bool &isCorrect)
{
	if (RAND_INT(1, 100) < 90)
	{
		if (RAND_BOOL() == true)
			status = "on";
		else
			status = "off";
		return true;
	}
	
	status =  OmnRandom::nextLetterStr(2, 5);
	if (status == "on" || status == "off")
		return true;
	else
		isCorrect = false;
	return true;
}


bool	
QuotaCliTester::genUser(OmnString &user, bool &isCorrect)
{
	int randomNum = RAND_INT(1, 100);
	if (randomNum > 20)
	{
		int userIndex = RAND_INT(0, userNum-1);
		user = existUsers[userIndex];
	}
	else
	{
		user = OmnRandom::nextLetterStr(5, 20); 
		isCorrect = false; 
	}
	return true;
}


bool	
QuotaCliTester::genDir(OmnString &partition, OmnString &directory, bool &isCorrect)
{
	DIR *dp;
	OmnString dir;
	struct stat statbuf;
	struct dirent *dirp;
	OmnString dirList[20];
	int dirNum = 0;
	

	if (isCorrect)
	{
		if (RAND_INT(0, 100) < 90)
		{
			dp = opendir(partition.data());
			while ((dirp = readdir(dp)) != NULL)
			{
				if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
					continue;
		
				dir = partition; 

				if (partition == "/") 
					dir << dirp->d_name;
				else
					dir << "/" << dirp->d_name;
				
				if (lstat (dir.data(), &statbuf) < 0)
					break;
			
				if (S_ISDIR(statbuf.st_mode) && dirNum < 20)
				{
					dirList[dirNum] = dir;
					dirNum++;
				}
			}

			directory = dirList[RAND_INT(0, dirNum-2)];
			closedir(dp);
		}
		else
		{
			directory = OmnRandom::nextLetterStr(5, 10); 
			isCorrect = false;
		}
	}
	else
	{
		directory = OmnRandom::nextLetterStr(5, 10); 
		isCorrect = false;
	}
	
	return true;
}


bool	
QuotaCliTester::genSize(OmnString &size, bool &isCorrect)
{
	int randomNum = RAND_INT(1, 100);
	char cSize[16];
	memset(cSize, 0, sizeof(cSize));
	char alm;

	char element[] = "kKMm";
	if (randomNum < 90)
	{
		int almIndex  = RAND_INT(0, sizeof(element)-2);
		alm = element[almIndex];
		
		sprintf(cSize, "%ld%c", RAND_INT(0, 40960), alm);
		size = cSize;
	}
	// inCorrect 
	// 1. the unit not be end with char [kKmM]
	// 2. the size is big than 409600000
	else
	{
		isCorrect = false; 

		randomNum = RAND_INT(0, 100);
		if (randomNum < 70) 
		{
			alm = RAND_INT(32, 127);
			if (alm == 'k' || alm == 'K'
				|| alm == 'M' || alm == 'm')
				alm = 'c';

			sprintf(cSize, "%d%c", RAND_INT(0, 40960), alm);
		}
		else if (randomNum < 90)
		{
			int almIndex  = RAND_INT(0, sizeof(element)-2);
			alm = element[almIndex];

			sprintf(cSize, "%ld%c", RAND_INT(409600000, 1000000000), alm);
		}
		else
		{
			alm = RAND_INT(32, 127);
			if (alm == 'k' || alm == 'K'
				|| alm == 'M' || alm == 'm')
				return true;
			sprintf(cSize, "%ld%c", RAND_INT(409600000, 1000000000), alm);

		}
 
		size = cSize; 
	}

	return true;
}


bool	
QuotaCliTester::genPart(OmnString &partition, bool &isCorrect)
{
	int i = 0;
	int randomNum = OmnRandom::nextInt(0, 100);

	if (randomNum < 90)
	{
		i = OmnRandom::nextInt(0, partNum-1);
		partition = existParts[i];
		return true;
	}	

	isCorrect = false;
	partition = OmnRandom::nextLetterStr(5, 10); 
	return true;
}


bool	
QuotaCliTester::initUserList()
{
	char buf[256], name[16], login[16];
	FILE *fp = fopen("/etc/passwd", "r");

	memset(buf, 0, sizeof(buf));
	while(fgets(buf, sizeof(buf), fp))
	{
		memset(login, 0, sizeof(login));
		memset(name, 0, sizeof(name));
		
		sscanf(buf, "%[^:]:%*[^:]:%*[^:]:%*[^:]:%*[^:]:%*[^:]:%[^:]", name, login);
		memset(buf, 0, sizeof(buf));

		if (strcmp(login, "/bin/bash") && strcmp(login, ""))
			continue;

		if (!strcmp(name, "root"))
			continue;

		existUsers[userNum] = name;
		userNum++;
		memset(buf, 0, sizeof(buf));
	}	

	fclose(fp);

	if (userNum < 2)
		return false;

	return true;
}


bool	
QuotaCliTester::initPartList()
{
	FILE *fp;
	struct stat fsDir, fsDev;
	char buf[128];
	char mountPoint[16], devName[16];

	fp = fopen("/etc/fstab", "r");
	while (fgets(buf, sizeof(buf), fp))
	{
		memset(devName, 0, sizeof(devName));
		memset(mountPoint, 0, sizeof(mountPoint));

		if (strstr(buf, "#"))
		{
			memset(buf, 0, sizeof(buf));
			continue;
		}

		sscanf(buf, "%s %s %*s %*s %*d %*d", devName, mountPoint);

		memset(buf, 0, sizeof(buf));
		if (stat(mountPoint, &fsDir) < 0 || stat(devName, &fsDir) < 0)
			continue;
			
		if (S_ISDIR(fsDir.st_mode) || S_ISBLK(fsDev.st_mode)) 
			continue;

		existParts[partNum] = mountPoint;
		partNum++;
	}
	fclose(fp);
	
	return true;
}


bool	
QuotaCliTester::quotaTorturerInit()
{
	diskMgrQuotaClearConfig();

	if (!initUserList()) 
	{
		cout << "Quota tester init exist user list error" << endl;
		exit(false);
	}

	if (!initPartList())
	{
		cout << "Quota tester init exist partition list error" << endl;
		exit(false); 
	}

	return true;
}


bool	
QuotaCliTester::statusIsAlready(OmnString &partition, OmnString &status)
{
	int i;
	bool isFound = false;
	for (i=0; i<mPartNum; i++)
	{
		if (mPart[i].mName == partition) 
		{
			isFound = true;
			if ( mPart[i].status == status)
				return true;
		}
	}

	if (!isFound)
	{
		if (status == "off")
			return true;
	}

	return false;
}


QuotaCliTester::user_t::user_t()
{
}

	
QuotaCliTester::user_t::~user_t()
{
}

QuotaCliTester::dir_t::dir_t()
{
}

	
QuotaCliTester::dir_t::~dir_t()
{
}

QuotaCliTester::partition_t::partition_t()
{
	userNum = 0;
	status = "off";
}

QuotaCliTester::partition_t::~partition_t()
{
}


