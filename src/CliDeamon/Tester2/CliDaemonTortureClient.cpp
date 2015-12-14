#include "Util/String.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "aosUtil/Alarm.h"
#include "Util/IpAddr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "Util/OmnNew.h"
#include "CliDeamon/Tester2/CliDaemonTortureClient.h"

AosCliDaemonTortureClient::AosCliDaemonTortureClient(const OmnIpAddr &serverAddr,
						     const int serverPort,
						     const int numOfServerPort,
						     const OmnIpAddr &clientAddr,
						     const int clientPort,
						     const int numOfClientPort,
						     const OmnString &mutexName):
  mServerAddr(serverAddr),
  mServerPort(serverPort),
  mNumOfServerPort(numOfServerPort),
  mClientAddr(clientAddr),
  mClientPort(clientPort),
  mNumOfClientPort(numOfClientPort),
  mMutexName(mutexName)
{
  //initialize the tcp client
  mTcpClient = OmnNew OmnTcpClient(mServerAddr,
				   mServerPort,
				   mNumOfServerPort,
				   mClientAddr,
				   mClientPort,
				   mNumOfClientPort,
				   mMutexName);
  if (!mTcpClient)
    {
      aos_alarm(eAosMD_UtilComm, eAosAlarm_FailedToConnect, 
		"Creat TcpClient failed\n");
    }
}
bool AosCliDaemonTortureClient::openConn()
{
  OmnString errMsg;
  if(!mTcpClient->connect(errMsg))
    {
      aos_alarm(eAosMD_UtilComm, eAosAlarm_FailedToConnect, 
		"Failed to connect: %s:%d. Errmsg: %s",
		mServerAddr.toString().data(), 
		mServerPort, 
		errMsg.data());
      mTcpClient->closeConn();
      return false;
    }
  return true;
}
bool AosCliDaemonTortureClient::sendMsg(OmnString &msg)
{
  if (!mTcpClient->writeTo(msg.data(),msg.length()))
    {
      aos_alarm(eAosMD_UtilComm, eAosAlarm_FailedToConnect, 
		"Failed to Send Data: %s to  %s:%d",
		msg.data(),
		mServerAddr.toString().data(), 
		mServerPort);
      return false;
    }
  return true; 
}
char* AosCliDaemonTortureClient::receiveMsg()
{
  OmnConnBuffPtr connBuffPtr;
  
  if (!mTcpClient->readFrom1(connBuffPtr, 
			     mIsConnBroken,
			     true))
    {
      aos_alarm(eAosMD_UtilComm, eAosAlarm_FailedToConnect, 
		"Failed to Receive Data From %s %d",
		mServerAddr.toString().data(), 
		mServerPort);
      return NULL;
    }
  return connBuffPtr->getBuffer();
}
bool AosCliDaemonTortureClient::closeConn()
{
  mTcpClient->closeConn();
  return true;
}
