#include "CliDeamon/Tester/CliDeamonTestPkg.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Util/String.h"
#include "Porting/Sleep.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "UtilComm/TcpClient.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpClient.h"
#include "Thread/Mutex.h"
#include "UtilComm/ConnBuff.h"
#include "stdio.h"
#include "stdlib.h"
#define CLIDEAMON_DEBUG 1
//variable definition
OmnString    AosCliDeamonTestPkg:: mCommandList[MAX_ARRAY_SIZE];
OmnString    AosCliDeamonTestPkg:: mRandomCommandList[MAX_ARRAY_SIZE];
AosCliDeamonTestPkg::AosCliDeamonTestPkg(const OmnIpAddr &serverAddr,
					 const int	serverPort,
					 const OmnIpAddr	&clientAddr,
					 const int	clientPort,
					 const int 	maxNumOfLogin,
					 const OmnString	&filePath,
					 const int	numOfRepeat,
					 const int	numOfConcurConn,
					 const int	numOfTries
					 ):
  mServerAddr(serverAddr),
  mServerPort(serverPort),
  mClientAddr(clientAddr),
  mClientPort(clientPort),
  mMaxNumOfLogin(maxNumOfLogin),
  mFilePath(filePath),
  mNumOfRepeat(numOfRepeat),
  mNumOfConcurConn(numOfConcurConn),
  mNumOfTries(numOfTries)
{
 
  bool isFinished = false;
  int index = 0;
  OmnString lineString;
  char endTag[3];
  endTag[0] = 0xfb;
  endTag[1] = 0xfe;
  endTag[2] = '\0';
  //check exception and open file
  mFile = OmnNew OmnFile(mFilePath, OmnFile::eReadOnly);
  //initialize the command list
  bool isValid = mFile->openFile(OmnFile::eReadOnly);
  if (isValid)
    {
      cout<<"starting for parsing the command file:"<<endl;
      while (isFinished == false)
	{
	  lineString = mFile->getLine(isFinished);
	  if (isFinished)
	    break;
	  /*if (CLIDEAMON_DEBUG)
	    {
	    cout<<index<<":"<<endl;
	    cout<< lineString.data()<<endl;
	    }
	  */
	  mCommandList[index] << "00000000";
	  mCommandList[index] << lineString; 
	  mCommandList[index]	<< endTag;
	  index++;
	}
      mNumOfCommand = index;
      if (CLIDEAMON_DEBUG)
	{
	  for (index = 0; index < mNumOfCommand; index++)
	    {
	      cout<<index<<":"<<mCommandList[index]<<endl;
	    }
	}
    }

  mFile->closeFile();  
}

bool AosCliDeamonTestPkg::start()
{
  tortureTest();
  return true;
}
bool AosCliDeamonTestPkg::tortureTest()
{
  if (CLIDEAMON_DEBUG)
    {
      cout<<"generateData  begin:"<<endl;
    }
  //generateData();
  if (CLIDEAMON_DEBUG)
    {
      cout<<"doOperations begin:"<<endl;
    }
  doOperations();
  printStatus();
  return true;
}
void AosCliDeamonTestPkg::generateData()
{
  //generate the login number randomly
  mNumOfLogin = OmnRandom::nextInt(1, mMaxNumOfLogin);
  mRandomCommandList[0] = mCommandList[0];
  //end
  
  int index = 0;
  int *pBitMaps = new int[mNumOfCommand];
  memset(pBitMaps, 0, sizeof(int));
  //generate the command list randomly
  for (int i = 1; i < mNumOfCommand; i++)
    {
      index = OmnRandom::nextInt(1, mNumOfCommand - 1);
      cout<<index<<":index"<<endl;
      if (pBitMaps[index] == 0)
	{
	  mRandomCommandList[i] = mCommandList[index];
	  pBitMaps[index] = 1;
	}
      else
	{
	  while (1)
	    {
	      index = OmnRandom::nextInt(1, mNumOfCommand);
	      if (pBitMaps[index] == 0)
		{
		  mRandomCommandList[i] = mCommandList[index];
		  pBitMaps[index] = 1;
		  break;
		}
	    }
	}
    } 
  delete pBitMaps;
  /*if (CLIDEAMON_DEBUG)
    {
    for (int i = 0; i < mNumOfCommand; i++)
    {
    cout<<i<<":"<<mRandomCommandList[i]<<endl;
    }
    }*/
  //end
  
}
void AosCliDeamonTestPkg::doOperations()
{
  OmnString mutexName("cliDeamonTestPkg");
  int DEFAULT_CLIENT_ID = 1;
  bool isTimeout = false;
  bool isConnBroken = false;
  OmnConnBuffPtr connBuffPtr;
  OmnString errMsg;
  
  //open the connection
  OmnTcpClientPtr tcpClient = OmnNew OmnTcpClient(mServerAddr,
						  mServerPort,
						  1,
						  mClientAddr,
						  mClientPort,
						  1,
						  mutexName);
  tcpClient->setClientId(DEFAULT_CLIENT_ID);

  if(!tcpClient->connect(errMsg))
    {
      aos_alarm(eAosMD_UtilComm, eAosAlarm_FailedToConnect, 
		"Failed to connect: %s:%d. Errmsg: %s",
		mServerAddr.toString().data(), 
		mServerPort, 
		errMsg.data());
      tcpClient->closeConn();
      return;
    }
  if (CLIDEAMON_DEBUG)
    cout << "Create conn for client: " 
	 << DEFAULT_CLIENT_ID << ". Sock: " << tcpClient->getSock() << endl;
  //end

  //starting login test
  if (!tcpClient->writeTo(mCommandList[0].getBuffer(),mCommandList[0].length()))
    {
      cout<<"send login info failed."<<endl;
      tcpClient->closeConn();
      return;
    }
  if (CLIDEAMON_DEBUG)
    {
      cout<<"send data:"<<mCommandList[0].getBuffer()<<endl;
    }
  if (!tcpClient->readFrom1(connBuffPtr, 
			    isConnBroken,
			    true))
    {
      cout<<"not recieved login authorization"<<endl;
      tcpClient->closeConn();
      return;
    }
  if (CLIDEAMON_DEBUG)
    {
      cout<<"recieve data:"<<connBuffPtr->getString()<<endl;
    }
  //get the session Id
  char sessionString[9];
  char *p = NULL;
  strcpy(sessionString, connBuffPtr->getBuffer());
  sessionString[8] = '\0';
  OmnString sessionStr(sessionString);
  if (CLIDEAMON_DEBUG)
    {
      cout<<"session id:"<<sessionString<<endl;
    }
  //end
  for (int i = 1; i < mNumOfCommand; i++)
    {
      mCommandList[i].insert(sessionStr,0);
      if (CLIDEAMON_DEBUG)
	{
	  cout<<"send msg:"<<mCommandList[i].getBuffer()<<endl;
	}
      if (!tcpClient->writeTo(mCommandList[i].getBuffer(), strlen(mCommandList[i].getBuffer())))
	{
	  cout<<"send failure"<<endl;
	  tcpClient->closeConn();
	  break;
	}
      if (!tcpClient->readFrom1(connBuffPtr, isConnBroken, true))
	{
	  cout<<"not recieved information"<<endl;
	  tcpClient->closeConn();
	  break;
	}
      i++;
      cout<<"received :"<<connBuffPtr->getBuffer()<<endl;
      cout<<"expected:"<<mCommandList[i]<<endl;
    }
  //end
  tcpClient->closeConn();
}
void AosCliDeamonTestPkg::printStatus()
{

}
