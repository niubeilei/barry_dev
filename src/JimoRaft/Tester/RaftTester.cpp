////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// 1. It maintains a list of seeds of fixed length for each file.
// 2. When adding new data to a file, it randomly determines the number of
//    seeds to add: "0123456789". "222222222222" "33333"
//    files[0] = "xxxx";
//    files[1] = "xxxx";
//
// 3. vector<u64> mDeletedDocids[num_threads];
//    vector<u64> mAllocatedBlocks[num_threads];
//
//    mAllocatedBlocks[0] = [1000, 18000000, 36000000];
//    mAllocatedBlocks[1] = [1000, 18000000, 36000000];
//
// Modification History:
// 2015-1-8 Created by White
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/Tester/RaftTester.h"

#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Thread/Mutex.h"
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/BitmapObj.h"
#include <arpa/inet.h>
#include <limits.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <signal.h>
#include <iostream>
#include <fstream>

using namespace std;

//raft server Id --->  raft process id
static map<u32, int>				sgRaftProcessMap;

AosRaftTester::AosRaftTester()
{
}

AosRaftTester::~AosRaftTester()
{
}

bool
AosRaftTester::start()
{
	//kill all the existing raftserver and rafttester
	//system("killall -9 rafttester.exe");
	system("killall -9 raftserver.exe");

	//run all test cases one by one
	test1();
	test2();
	test3();
	test4();
	//test5();
	test6();
	test7();
	//test8();
	//test9();
	//test10();
	//test11();
	//test12();

	return true;
}

//////////////////////////////////////////////////////
//  Test cases
//////////////////////////////////////////////////////

//
//1.RoleChange::OneNodeUp
//	a.Start a node’s raft process
//	b.Check its status after 5 seconds. It should be in candidate status
bool
AosRaftTester::test1()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);

	//sleep for a while
	OmnSleep(15);

	//read the file line by line and parse
	OmnString info = getLastLine("0.out", "(ServerId=.*)");

	//get role's word from the line
	if (getRole(info) != "candidate")
		rslt = false;

	testFinished("Test1", rslt);
	return rslt;
}

bool
AosRaftTester::test2()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);
	aos_assert_r(startNode(1), false); 

	//sleep for a while
	OmnSleep(40);

	vector<u32> nodeList;
	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);

	rslt = checkAll(nodeList);
	testFinished("Test2", rslt);

	return true;
}

bool
AosRaftTester::test3()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);
	aos_assert_r(startNode(1), false); 
	aos_assert_r(startNode(2), false); 

	//sleep for a while
	OmnSleep(40);

	vector<u32> nodeList;
	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	nodeList.push_back(2);

	rslt = checkAll(nodeList);
	testFinished("Test3", rslt);

	return rslt;

}

//Test4: RoleChange::OneFollowerDown
//a.	Peroform steps same as RoleChange::ThreeNodeUp 
//b.	Kill one of the follower process
//c.	Check the rest two nodes’ status after 5 seconds. The first one should be leader and the other one is a follower. Both of them have the same termId.
bool
AosRaftTester::test4()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);
	aos_assert_r(startNode(1), false); 
	aos_assert_r(startNode(2), false); 

	//sleep for a while
	OmnSleep(40);

	vector<u32> nodeList;
	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	nodeList.push_back(2);

	rslt = checkAll(nodeList);
	if (!rslt)
	{
		testFinished("Test4", rslt);
		return rslt;
	}

	aos_assert_r(stopNode(2), false);

	//sleep for a while
	OmnSleep(10);

	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	rslt = checkAll(nodeList);
	testFinished("Test4", rslt);
	return rslt;
}

/*
bool
AosRaftTester::test5()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);
	OmnSleep(5);
	aos_assert_r(startNode(1), false); 
	OmnSleep(5);
	aos_assert_r(startNode(2), false); 

	//sleep for a while
	OmnSleep(10);

	vector<u32> nodeList;
	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	nodeList.push_back(2);

	rslt = checkAll(nodeList);
	if (!rslt)
	{
		testFinished("Test5", rslt);
		return rslt;
	}

	aos_assert_r(stopNode(0), false);

	//sleep for a while
	OmnSleep(5);

	nodeList.clear();
	nodeList.push_back(1);
	nodeList.push_back(2);
	rslt = checkAll(nodeList);
	testFinished("Test5", rslt);

	return rslt;
}
*/

bool
AosRaftTester::test6()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);
	aos_assert_r(startNode(1), false); 
	aos_assert_r(startNode(2), false); 

	//sleep for a while
	OmnSleep(40);

	vector<u32> nodeList;
	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	nodeList.push_back(2);

	rslt = checkAll(nodeList);
	if (!rslt)
	{
		testFinished("Test6", rslt);
		return rslt;
	}

	aos_assert_r(stopNode(0), false);

	//sleep for a while
	OmnSleep(5);

	nodeList.clear();
	nodeList.push_back(1);
	nodeList.push_back(2);
	rslt = checkAll(nodeList);
	if (!rslt)
	{
		testFinished("Test6", rslt);
	}

	aos_assert_r(startNode(0), false);

	//sleep for a while
	OmnSleep(10);

	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	nodeList.push_back(2);
	rslt = checkAll(nodeList); 
	testFinished("Test6", rslt);

	return rslt;
}

bool
AosRaftTester::test7()
{
	bool rslt = true;

	//start node1's raft
	aos_assert_r(startNode(0), false);
	aos_assert_r(startNode(1), false); 

	//sleep for a while
	OmnSleep(30);

	vector<u32> nodeList;
	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);

	rslt = checkAll(nodeList);
	if (!rslt)
	{
		testFinished("Test7", rslt);
		return rslt;
	}

	aos_assert_r(stopNode(0), false);
	aos_assert_r(stopNode(1), false);

	//sleep for a while
	OmnSleep(10);

	aos_assert_r(startNode(0), false);
	aos_assert_r(startNode(1), false);

	//sleep for a while
	OmnSleep(20);      

	nodeList.clear();
	nodeList.push_back(0);
	nodeList.push_back(1);
	rslt = checkAll(nodeList);
	testFinished("Test7", rslt);

	return rslt;
}

bool
AosRaftTester::test8()
{
	return true;
}

bool
AosRaftTester::test9()
{
	return true;
}

bool
AosRaftTester::test10()
{
	return true;
}

bool
AosRaftTester::test11()
{
	return true;
}

bool
AosRaftTester::test12()
{
	return true;
}

//////////////////////////////////////////////////////
//  verification methods
//////////////////////////////////////////////////////
bool
AosRaftTester::testFinished(
		OmnString testName, 
		bool rslt)
{
	stopAllNodes();
	OmnScreen << "*********************" << endl;
	if (rslt)
	{
		//print pass result
		OmnScreen << "***" << testName << " passed    ***" << endl;
	}
	else
	{
		//print failed result
		OmnScreen << "***" << testName << " FAILED!!! ***" << endl;
	}

	OmnScreen << "*********************" << endl;
	return true;
}

bool
AosRaftTester::checkAll(vector<u32> &nodeList)
{
	aos_assert_r(nodeList.size() > 0, false);

	OmnString outFileSuffix = ".out";
	OmnString pattern = "(ServerId=.*)";
	OmnString infoStr;
	OmnString outFile;

	vector<OmnString> infoList;
	infoList.clear();
	for (u32 i = 0; i < nodeList.size(); i++)
	{
		outFile = "";
		outFile << nodeList[i] << outFileSuffix;

		//read the file line by line and parse
		infoStr = getLastLine(outFile, pattern);
		infoList.push_back(infoStr);
	}

	if (!checkTermId(infoList))
		return false;

	if (!checkLastLogId(infoList))
		return false;

	if (!checkRole(infoList))
		return false;

	if (!checkLogFile(nodeList))
		return false;

	return true;
}

bool
AosRaftTester::checkTermId(vector<OmnString> &infoList)
{
	aos_assert_r(infoList.size() > 1, false);
	OmnString termId = getTermId(infoList[0]);

	for (u32 i = 1; i < infoList.size(); i++)
	{
		if (termId != getTermId(infoList[i]))
		{
			OmnScreen << "Different termids: "
				<< termId << " vs " << getTermId(infoList[i]) << endl;

			return false;	
		}
	}

	return true;
}

bool
AosRaftTester::checkLastLogId(vector<OmnString> &infoList)
{
	aos_assert_r(infoList.size() > 1, false);
	OmnString LastLogId = getLastLogId(infoList[0]);

	for (u32 i = 1; i < infoList.size(); i++)
	{
		if (LastLogId != getLastLogId(infoList[i]))
		{
			OmnScreen << "Different LastLogids: "
				<< LastLogId << " vs " << getLastLogId(infoList[i]) << endl;

			return false;	
		}
	}

	return true;
}

bool
AosRaftTester::checkRole(vector<OmnString> &roles)
{
	int numLeaders = 0;
	int numCandidates = 0;
	OmnString roleStr;

	for (u32 i = 0; i < roles.size(); i++)
	{
		roleStr = getRole(roles[i]);
		if (roleStr == "leader")
			numLeaders++;
		else if (roleStr == "candidate")
			numCandidates++;
	}

	if (numLeaders == 1 && numCandidates == 0)
		return true;

	OmnScreen << "Wrong roles: " << numLeaders 
		<< " leaders and " << numCandidates << " candidates!" << endl;

	return false;	
}

bool
AosRaftTester::checkLogFile(vector<u32> &nodeList)
{
	aos_assert_r(nodeList.size() > 1, false);

	int maxLogSize =  10000;
	AosBuffPtr buff1 = 0;
	AosBuffPtr buff2 = 0;
	char *data1 = NULL;
	char *data2 = NULL;

	//get the first node's log data
	if (!getLogFileData(nodeList[0], buff1))
	{
		OmnScreen << "Failed to get log file data for node "
			<< nodeList[0] << endl;

		return false;
	}
	data1 = buff1->data();

	for (u32 i = 1; i < nodeList.size(); i++)
	{
		//get the next node's log data
		if (!getLogFileData(nodeList[i], buff2))
		{
			OmnScreen << "Failed to get log file data for node "
				<< nodeList[i] << endl;

			return false;
		}

		//compare content in buff1 and buff2 byte by byte
		if (buff1->dataLen() != buff2->dataLen())
		{
			OmnScreen << "Log files are in different length: "
				<< buff1->dataLen() << " for node " << nodeList[0]
				<< " and " << buff2->dataLen() << " for node " 
				<< nodeList[i] << endl;

			return false;
		}

		data2 = buff2->data();
		for (u32 j = 0; j < buff1->dataLen(); j++)
		{
			if (data1[j] != data2[j])
			{
				OmnScreen << "Log files have different data at the position "
					<< j << " for node " << nodeList[0]
					<< " and " << nodeList[i] << endl;

				return false;	
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////
//  Node control methods
//////////////////////////////////////////////////////
int
AosRaftTester::startNode(int nodeId)
{
	OmnString cmd = "";
	OmnString fConfig = "";
	OmnString fOutput = "";
	cmd << "../Exe/raftserver.exe -c ../Exe/docstore"
		<< nodeId << ".xml > " << nodeId << ".out";

	fConfig << "../Exe/docstore" << nodeId << ".xml";
	fOutput << nodeId << ".out";

	pid_t pid = fork();
	if (pid == 0) {
		OmnScreen << "start a raftserver process for node "
			<< nodeId << endl;

		//redirect the output to the output file
		//ofstream out(fOutput.getBuffer());
		//cout.rdbuf(out.rdbuf()); 
		aos_assert_r(freopen(fOutput.getBuffer(), "w", stdout), false);

		//start a raft node
		execlp("../Exe/raftserver.exe", 
				"raftserver.exe", "-c", fConfig.getBuffer(), NULL);

		exit(0);
		//system(cmd.getBuffer());
	}
	else {
		OmnScreen << "create child pid : " << pid 
			<< " for node " << nodeId << endl;
		//record the child's process id
		sgRaftProcessMap[nodeId] = pid;

		//sleep to wait the node startup completely
		//OmnSleep(15);
	}

	return true;
}

bool
AosRaftTester::stopNode(int nodeId)
{
	map<u32, int>::iterator itr;
	pid_t pid;

	itr = sgRaftProcessMap.find(nodeId);
	if (itr != sgRaftProcessMap.end())
	{
		pid = itr->second;
		OmnScreen << "kill node " << nodeId 
			<< "'s process " << pid << endl;
		kill(pid, 9);
	}

	return true;
}

bool
AosRaftTester::stopAllNodes()
{
	int totalNodes = 3;

	for (int i = 0; i < totalNodes; i++)
		stopNode(i);

	return true;
}


//////////////////////////////////////////////////////
//  Helper methods
//////////////////////////////////////////////////////
//
//the line is:
//<0x14427180:2015-05-30 16:45:14:RaftServer.cpp:178>  (ServerId= 0 TermId=1 LastLogId=9 Role=leader)
//
OmnString
AosRaftTester::getTermId(OmnString line)
{
	OmnString str;

	str = getKeyValue(line, "TermId=", " LastLogId=");
	OmnScreen << "term Id is: " << str << endl;
	return str;
}
	
OmnString
AosRaftTester::getLastLogId(OmnString line)
{
	OmnString str;

	str = getKeyValue(line, " LastLogId=", " Role=");
	OmnScreen << "last log Id is: " << str << endl;
	return str;
}

OmnString
AosRaftTester::getRole(OmnString line)
{
	OmnString str;

	str = getKeyValue(line, " Role=", ")");
	OmnScreen << "Role is: " << str << endl;
	return str;
}

OmnString
AosRaftTester::getKeyValue(
		OmnString line,
		OmnString keyBefore,
		OmnString keyAfter)
{
	OmnString str;
	int start = line.indexOf(keyBefore, 0);
	if (start < 0) return "";

	start += keyBefore.length();

	int end = line.indexOf(keyAfter, start);
	if (end <= start) return "";

	str = line.substr(start, end - 1);
	return str;
}

OmnString
AosRaftTester::getLastLine(
		OmnString fName, 
		OmnString pattern)
{
	OmnString cmd = "";
	cmd << "tail -n 1000 " << fName << " | sed -n '/"<< pattern << "/h;${g;p}'"; 
	OmnString str;
	FILE* fp = popen(cmd.data(), "r");
	aos_assert_r(fp != NULL, "");

	char buff[1024];                    
	while (!feof(fp))                       
	{                                       
		int len = fread(buff, 1, 1024, fp); 
		buff[len-1]=0;                      
		str << buff;                       
	}                                     

	pclose(fp);
	OmnScreen << "Looking for " << pattern 
		<< " in " << str << endl;
	return str;
}

//
//read a node's log file data into a buffer.
//The buffer space is allocsated by 
//getLogFileData
//
bool
AosRaftTester::getLogFileData(
		u32 nodeId,
		AosBuffPtr &buff)
{
	//aos_assert_r(buff, false);

	u64 maxLogSize = 10000;
	OmnString fNamePrefix = "Jimo_Servers0/Data/cube/vir_0_orig/site_100/raftlog_";
	OmnString fName;

	OmnFilePtr fLog;
	//open the node's log file
	fName = fNamePrefix;
	fName << nodeId;
	fLog = OmnNew OmnFile(fName, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!fLog->isGood())
	{   
		//The log file is damaged. 
		OmnScreen << "Log file is not good: " << fName << endl;
		return false;
	}

	//get the first node's log file and read into buff
	aos_assert_r(fLog->openFile(OmnFile::eReadOnly), false);
	aos_assert_r(fLog->readToBuff(buff, maxLogSize), false);
	aos_assert_r(fLog->closeFile(), false);

	OmnScreen << "Node " << nodeId 
		<< "'s log data length is " << buff->dataLen() << endl;
	return true;
}
