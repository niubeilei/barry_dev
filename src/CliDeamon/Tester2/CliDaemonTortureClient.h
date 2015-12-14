///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/20/2006	
// Comments:
//			torture test client of cliDaemon
// 
// Change History:
//	09/20/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef AOS_CLIDAEMON_CLIDAEMON_TORTURE_CLIENT_H
#define AOS_CLIDAEMON_CLIDAEMON_TORTURE_CLIENT_H
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpClient.h"

class AosCliDaemonTortureClient: public OmnRCObject
{
	OmnDefineRCObject;
 public:
  AosCliDaemonTortureClient(const OmnIpAddr &serverAddr,
			 const int serverPort,
			 const int numOfServerPort,
			 const OmnIpAddr &clientAddr,
			 const int clientPort,
			 const int numOfClientPort,
			 const OmnString &mutexName);
  ~AosCliDaemonTortureClient(){}
 private:
  OmnIpAddr mServerAddr;
  int mServerPort;
  int mNumOfServerPort;
  OmnIpAddr mClientAddr;
  int mClientPort;
  int mNumOfClientPort;
  int mClientId;
  OmnTcpClientPtr mTcpClient; 
  OmnString mMutexName;
  bool mIsConnBroken;
 public:
  //interface definition
  //
  void setClientId(int clientId)
    {
      mClientId = clientId;
    }
  bool openConn();
  bool sendMsg(OmnString &msg);
  char* receiveMsg();
  bool closeConn();
  //  
  //end of interface defintion
};
#endif
