///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//			test package of cliDeamon
// 
// Change History:
//	09/15/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef Omn_CliDeamon_Tester_AosCliDeamonTestPkg_h
#define Omn_CliDeamon_Tester_AosCliDeamonTestPkg_h

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

#define MAX_ARRAY_SIZE 1000
class AosCliDeamonTestPkg : public OmnTestPkg
{
  OmnDefineRCObject;
 private:
  static OmnString     mCommandList[MAX_ARRAY_SIZE];
  static OmnString     mRandomCommandList[MAX_ARRAY_SIZE];
  OmnIpAddr	mServerAddr;
  int		mServerPort;
  OmnIpAddr	mClientAddr;
  int		mClientPort;
  int		mMaxNumOfLogin;//maximum number of login in clideamon
  OmnString	mFilePath;
  int		mNumOfRepeat;
  int		mNumOfConcurConn;
  int		mNumOfTries;
  int 		mNumOfLogin;
  OmnFilePtr	mFile;  
  int           mNumOfCommand;
 public:
  AosCliDeamonTestPkg(const OmnIpAddr &serverAddr,
		      const int	serverPort,
		      const OmnIpAddr	&clientAddr,
		      const int	clientPort,
		      const int 	maxNumOfLogin,
		      const OmnString	&filePath,
		      const int	numOfRepeat,
		      const int	numOfConcurConn,
		      const int	numOfTries
		      );
  ~AosCliDeamonTestPkg()
    {
    }
  virtual bool start();
 private:
  bool tortureTest();
  void generateData();
  void doOperations();
  void printStatus();
		
};

#endif
