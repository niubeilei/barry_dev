///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/20/2006	
// Comments:
//			torture test package of cliDaemon
// 
// Change History:
//	09/20/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef Omn_CliDaemon_Tester_AosCliDaemonTortureTestPkg_h
#define Omn_CliDaemon_Tester_AosCliDaemonTortureTestPkg_h

#include "Tester/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/String.h"
#include "Util/Array.h"
#include "Tester/TestPkg.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Debug/Debug.h"
#include "UtilComm/TcpClient.h"
#include "CliDeamon/Tester3/Constants.h"
#include "CliDeamon/Tester3/CliDaemonTortureClient.h"
#include "CliDeamon/Tester3/INIFile.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/ValList.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"
#include "Thread/Mutex.h"
#include "CliDeamon/Tester3/Constants.h"

class AosCliDaemonTortureTestPkg : public OmnTestPkg
{
    OmnDefineRCObject;
    private:
	OmnIpAddr	mServerAddr;
	int		mServerPort;
	OmnIpAddr	mClientAddr;
	int		mClientPort;
	int		mMaxNumOfLogin;//maximum number of login in clideamon
	OmnString	mFilePath;
	OmnString    mResultFilePath;
	int		mNumOfTries;
	int 		mNumOfLogin;  
	int          mNumOfCommand;
	
	OmnMutexPtr	mLock;
 public:
	AosCliDaemonTortureTestPkg(const OmnIpAddr &serverAddr,
							   const int	serverPort,
							   const OmnIpAddr	&clientAddr,
							   const int	clientPort,
							   const int 	maxNumOfLogin,
							   const OmnString	&filePath,
							   const OmnString &resultFilePath,
							   const int	numOfTries,
							   UserListEntry *user,
							   OmnMutexPtr mutex
							   );
	~AosCliDaemonTortureTestPkg()
    {
        if (pFile)
		{
		    pFile->CloseConfig();
			delete pFile;
		}
		if (pTortureClient)
		{
			pTortureClient->closeConn();
			delete pTortureClient;
		}
    }
	virtual bool start();
 private:
	UserListEntry mHeader;
	AosCliDaemonTortureClient *pTortureClient;
	CommandEntry mCommandList[DEFAULT_SIZE];
	CommandEntry *mpCurrentCommand;
	UserListEntry *mpCurrentUser;
	AosINIFile *pFile;
	OmnFilePtr logFile;
	bool tortureTest();

  
	void generateData();
	void parseCommand(char *p, CommandEntry *pEntry);
	void doOperations();
	void printStatus();
	void swap(CommandEntry *pSrc, CommandEntry *pDst);
	bool checkTimer();
	int checkRight();
	int  getRight(char *right);
	char *getError(int code);
	bool isSingle();
	bool isLogin(char *p);
	
	bool login(char *user, char *password);
	void logout();
	void init();
	void checkResults(CommandEntry *entry, char *result);
		
};

#endif
