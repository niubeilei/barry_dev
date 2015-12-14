#include "CliDeamon/Tester3/CliDaemonTortureTestPkg.h"

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
#include <stdio.h>
#include <stdlib.h>
#include "Util1/Time.h"
#include "CliDeamon/Tester3/INIFile.h"
#include "CliDeamon/Tester3/Constants.h"
#include "Thread/Mutex.h"
#include <string.h>

#define CLIDAEMON_DEBUG 1


//debug utility
static char EndTag[3];
static char SessionId[9];

static char* lowerstr(char *p)
{
  int len = strlen(p);
  int i = 0;
  for (i = 0; i < len; i++)
    {
	  p[i] = tolower(p[i]);
    }
  return p;
}
static char* trim(char *p)
{
  while (*p == ' ' || *p == '\t')p++;
    
  int i = strlen(p);
  char *q = p + i - 1;
    
  while (*q == ' ' || *q == '\t' || *q == '\n' || *q == '\r\n')
    {
	  *q = '\0';
	  q--;
	}

  return p;
}
void printUserList(UserListEntry *pHeader)
{
  cout<<"starting:printUserList"<<endl;
  UserListEntry *pos;
  struct aos_list_head *q = NULL;
  q = &pHeader->list;
  aos_list_for_each_entry(pos, q,list)
	{
	  printf("user:%s password:%s\n", pos->userName, pos->userPassword);
    }
}

//end
AosCliDaemonTortureTestPkg::AosCliDaemonTortureTestPkg(const OmnIpAddr &serverAddr,
													   const int	serverPort,
													   const OmnIpAddr	&clientAddr,
													   const int	clientPort,
													   const int 	maxNumOfLogin,
													   const OmnString	&filePath,
													   const OmnString  &resultFilePath,
													   const int	numOfTries,
													   UserListEntry *user,
													   OmnMutexPtr mutex
													   ):
  mServerAddr(serverAddr),
  mServerPort(serverPort),
  mClientAddr(clientAddr),
  mClientPort(clientPort),
  mMaxNumOfLogin(maxNumOfLogin),
  mFilePath(filePath),
  mResultFilePath(resultFilePath),
  mNumOfTries(numOfTries)
{
  mpCurrentUser = user;
  mLock = mutex;
}

bool AosCliDaemonTortureTestPkg::start()
{
  init();
  tortureTest();
  return true;
}

bool AosCliDaemonTortureTestPkg::tortureTest()
{
  
  cout<<"starting:tortureTest"<<endl;
  int i = 0;
  while (i < mNumOfTries){
	if (login(mpCurrentUser->userName, mpCurrentUser->userPassword))
	  {
		generateData();
		doOperations();
		logout();
	  }
	i++;
  }
    
  return true;
}

//generate the test data for every user session
void AosCliDaemonTortureTestPkg::generateData()
{
  //re-order the sequence of command list
  for (int i = 0; i < mNumOfCommand; i++)
	{
	  int random = OmnRandom::nextInt(0, mNumOfCommand - 1);
	  if (SWITCH_RANDOM)
		swap(&mCommandList[i], &mCommandList[random]);
	}
  //end
}

void AosCliDaemonTortureTestPkg::doOperations()
{
  cout<<"starting:doOperations"<<endl;
  char *p = NULL;
  
  //starting operations
  for (int i = 0; i < mNumOfCommand; i++)
    {
	  OmnString cmdMsg;
	  cmdMsg<<SessionId<<trim(mCommandList[i].command)<<EndTag;
		
	  //set the current command
	  mpCurrentCommand = &mCommandList[i];
		
	  //if current user
	  if (strcmp(mCommandList[i].user, mpCurrentUser->userName) == 0){
		   
		// if (CLIDAEMON_DEBUG)
		cout<<"send command:"<<cmdMsg<<endl;

		if (!(pTortureClient->sendMsg(cmdMsg)))
		  {
			cout<<"Error:Send command---"<<cmdMsg<<endl;
		  }
		
		if (!(p = pTortureClient->receiveMsg()))
		  {
			cout<<"Error:not receive msg of command ---" <<cmdMsg<<endl;
		  }
		//if (CLIDAEMON_DEBUG)
		cout<<"======================================================================================="<<endl;
		cout<<"receive msg:"<<p<<endl;
			
		if (checkTimer() == false)
		  {
			checkRight();
		  }
		checkResults(&mCommandList[i], p);
			
	  }
    }
}

void AosCliDaemonTortureTestPkg::printStatus()
{

}

bool AosCliDaemonTortureTestPkg::login(char *user, char *password)
{
  cout<<"starting:login"<<endl;
  OmnString loginMsg;
  OmnString mutexName(user);
  char *p = NULL;
    
  loginMsg<<DEFAULT_SESSION_ID<<"user login:"<<user<<":"<<trim(password)<<EndTag;
  cout<<loginMsg << "-----" << endl;
  if (pTortureClient == NULL){
	pTortureClient = new AosCliDaemonTortureClient(mServerAddr,
												   mServerPort,
												   1,
												   mClientAddr,
												   mClientPort,
												   100,
												   mutexName);
	if (!pTortureClient->openConn())
	  {
		return false;
	  }
  }
	
  if (CLIDAEMON_DEBUG)
	cout<<"send login command:"<<loginMsg<<endl;
	
	
  if (!pTortureClient->sendMsg(loginMsg))
	{
	  return false;
	}
  if (!(p = pTortureClient->receiveMsg()))
	{
	  return false;
	}
  if (CLIDAEMON_DEBUG)	
	cout<<"Receive login msg:"<<p<<endl;
    		
  //catch the session id,it is passport
  memcpy(SessionId, p, sizeof(char) * 8);
	

  if (CLIDAEMON_DEBUG)
	cout<<"login session Id:"<<SessionId<<endl;
	
  //start timer
  mpCurrentUser->lastTick = OmnTime::getSecTick();
  //end

  return isLogin(p);
}

//
//the function is not implemented completely
//
bool AosCliDaemonTortureTestPkg::isLogin(char *p)
{
   if (strcmp(SessionId, DEFAULT_SESSION_ID) == 0)
	{
	  cout<<"Failed to login"<<endl;
	  return false;
	}
    return true;
}
void AosCliDaemonTortureTestPkg::logout()
{
  OmnString logoutMsg;
  char *p = NULL;
	
  //read logout section
  pFile->RewindConfig();
  if (!(p = pFile->ReadConfig(SECTION_LOGOUT_CLI_BODY)))
    {
	  cout<<"Error:open logout body"<<endl;
	  return;
    } 
  logoutMsg<<SessionId<<p<<EndTag;
  
  //send commands
  if (!(pTortureClient->sendMsg(logoutMsg)))
    {
	  cout<<"Error:send commands for logout"<<endl;
	  return;
    }
  //receive reponse
  if (!(p = pTortureClient->receiveMsg()))
    {
	  cout<<"Error:not receive commands for logout"<<endl;
	  return;
    }
  if (CLIDAEMON_DEBUG)
	cout<<"Logout Info:"<<p<<endl;
}

void AosCliDaemonTortureTestPkg::init()
{
  //read the user information
	
  char *p = NULL;
  if  (CLIDAEMON_DEBUG)
	cout<<"starting to parse ini file"<<endl;
	
  pFile = new  AosINIFile(mFilePath.data(), OPEN_RD);
  if (!pFile->OpenConfig())
	{
	  cout<<"open config file error."<<endl;
	  return;
    }

  //init the end tag
  EndTag[0] = 0xfb;
  EndTag[1] = 0xfe;
  EndTag[2] = '\0';
  //end
  //init log file
  logFile = OmnNew OmnFile(mResultFilePath, OmnFile::eAppend);

  //init the command list of CLI Daemon
  mNumOfCommand = 0;
  //read command ,parse the ini file
  while ((p = pFile->ReadConfig(SECTION_OP_CLI_BODY)) != NULL)
	{
	  parseCommand(p, &mCommandList[mNumOfCommand]);
	  if (CLIDAEMON_DEBUG)
		cout<<"reading command:"<<mCommandList[mNumOfCommand].command<<endl;
	  mNumOfCommand++;
	}

}

void AosCliDaemonTortureTestPkg::checkResults(CommandEntry *entry, char *result)
{
  OmnString logMsg;
  logMsg<<"\n";
  logMsg<<"user:"<<entry->user<<"\n";
  logMsg<<"level:"<<entry->level<<"\n";
  logMsg<<"command:"<<entry->command<<"\n";
  logMsg<<"expected:"<<entry->result<<"\n";
  logMsg<<"result:"<<result<<"\n";
  logMsg<<"\n";
  cout<<"checkResult msg:"<<logMsg<<endl;
  logFile->put(logMsg.data(), true);
}

void AosCliDaemonTortureTestPkg::parseCommand(char *p, CommandEntry *pEntry)
{
  cout<<"starting:parseCommand"<<endl;
  cout<<"parse command line:"<<p<<endl;
  char *prev = NULL;
  char *next = NULL;
  int i = 0;
  prev = p;
  //user
  while (*prev == '<' || *prev == '\t' || *prev == ' ')
	prev++;
  next = prev;
  while (*next != '>')
	next++;
  
  while (prev != next)
	{
	  pEntry->user[i] = *prev;
	  i++;
	  prev++;
	}
  pEntry->user[i] = '\0';
  //cout<<"parse command user:"<<pEntry->user<<endl;
	
  //command
  next++;
  prev = next;
  i = 0;
  while (*prev == '<' || *prev == '\t' || *prev == ' ')
	prev++;
  next = prev;
  while (*next != '>')
	next++;
  while (prev != next)
	{
	  pEntry->command[i] = *prev;
	  prev++;
	  i++;
	}
  pEntry->command[i] = '\0';
  //cout<<"parse command command:"<<pEntry->command<<endl;
	
  //level
  next++;
  prev = next;
  i = 0;
  while (*prev == '<' || *prev == '\t' || *prev == ' ')
	prev++;
  next = prev;
  while (*next != '>')
	next++;
  while (prev != next)
	{
	  pEntry->level[i] = *prev;
	  prev++;
	  i++;
	}
  pEntry->level[i] = '\0';
  //cout<<"parse command level:"<<pEntry->level<<endl;
	
  //result
  next++;
  prev = next;
  i = 0;
  while (*prev == '<' || *prev == '\t' || *prev == ' ')
	prev++;
  while (*next != '>')
	next++;
  while (prev != next)
	{
	  pEntry->result[i] = *prev;
	  prev++;
	  i++;
	}
  pEntry->result[i] = '\0';
	
  //cout<<"parse command result:"<<pEntry->result<<endl;
}

//swap between two command entry
void AosCliDaemonTortureTestPkg::swap(CommandEntry *pSrc, CommandEntry *pDst)
{
  char command[DEFAULT_SIZE];
  char result[DEFAULT_SIZE];
  char level[DEFAULT_SIZE];
  char user[DEFAULT_SIZE];

  strcpy(command, pSrc->command);
  strcpy(result, pSrc->result);
  strcpy(level, pSrc->level);
  strcpy(user, pSrc->user);

  strcpy(pSrc->command, pDst->command);
  strcpy(pSrc->result, pDst->result);
  strcpy(pSrc->level, pDst->level);
  strcpy(pSrc->user, pDst->user);

  strcpy(pDst->command, command);
  strcpy(pDst->result, result);
  strcpy(pDst->level, level);
  strcpy(pDst->user, user);
}

//
//check the timeout of connection
//return true if timeout is not occured
//otherwise return true.
//
bool  AosCliDaemonTortureTestPkg::checkTimer()
{
  if (CLIDAEMON_DEBUG)
	{
	  cout<<"start checkTimer"<<endl;
	  cout<<"current timeout is "<<mpCurrentUser->timeout<<", current time is "<<OmnTime::getSecTick()<<", last time is "<<mpCurrentUser->lastTick<<endl;
	}
	if (mpCurrentUser->timeout >= (OmnTime::getSecTick() - mpCurrentUser->lastTick))
	{
	  return true;
	}
	
  cout<<"timeout is occured."<<endl;
  return false;
}

//
//check the right of user
//return 0 if success.
//otherwise return error code
//
int  AosCliDaemonTortureTestPkg::checkRight()
{
  cout <<"starting checkRight..."<<endl;
  static char CMD_SET[DEFAULT_SIZE] = "user level set "; 
	
	
  if (strstr(lowerstr(mpCurrentCommand->command), lowerstr(CMD_SET)))
	{
	  if (strcmp(trim(&(mpCurrentCommand->command[strlen(CMD_SET)])),
				 RIGHT_ENGINEERING) == 0
		  && (strcmp(mpCurrentUser->right, RIGHT_USER) == 0 
			  || strcmp(mpCurrentUser->right, RIGHT_ENABLE) == 0
			  )
		  )
		{
		  strcpy(mpCurrentCommand->result, ERROR_FORBID_ENGINEERING_CHANGE_TEXT);

		  return ERROR_FORBID_ENGINEERING_CHANGE; 
		}
	  else if (isSingle() == false)
		{
		  return ERROR_FORBID_CONFIG_NUM;
		}
	  else
		{
		  strcpy(mpCurrentUser->right, &(mpCurrentCommand->command[strlen(CMD_SET)]));
		  if (strcmp(trim(&(mpCurrentCommand->command[strlen(CMD_SET)])),
					 RIGHT_USER) == 0)
			{
			  strcpy(mpCurrentUser->right, RIGHT_USER);
			  mpCurrentUser->timeout = DEFAULT_TIMEOUT;
			}
		  if (strcmp(trim(&(mpCurrentCommand->command[strlen(CMD_SET)])),
					 RIGHT_ENABLE) == 0)
			{
			  strcpy(mpCurrentUser->right, RIGHT_ENABLE);
			  mpCurrentUser->timeout = DEFAULT_TIMEOUT;
			}
		  if (strcmp(trim(&(mpCurrentCommand->command[strlen(CMD_SET)])),
					 RIGHT_CONFIG) == 0)
			{
			  strcpy(mpCurrentUser->right, RIGHT_CONFIG);
			  mpCurrentUser->timeout = DEFAULT_TIMEOUT;
			}
		  if (strcmp(trim(&(mpCurrentCommand->command[strlen(CMD_SET)])),
					 RIGHT_ENGINEERING) == 0)
			{
			  strcpy(mpCurrentUser->right, RIGHT_ENGINEERING);
			  mpCurrentUser->timeout = TIMEOUT_ENGINEERING;
			}
		  return 0;
		}
	}
  else
    {
	  if (getRight(mpCurrentUser->right) < getRight(mpCurrentCommand->level))
	    {
		  strcpy(mpCurrentCommand->result, ERROR_FORBID_NORMAL_TEXT);
		    
		  return ERROR_FORBID_NORMAL;
	    }
	  return 0;
	}
}

//
//check whether level 'config' and 'engineering' is synchronized
//
bool AosCliDaemonTortureTestPkg::isSingle()
{
  mLock->lock();
  UserListEntry *pos;
  int counter = 0;
  struct aos_list_head *tmp = NULL;
  tmp = &(gUserListEntry.list);
  aos_list_for_each_entry(pos, tmp,list)
	{
	  if (strcmp(lowerstr(pos->right), lowerstr(RIGHT_CONFIG)) == 0)
		{
		  counter++;
		}
	  if (strcmp(lowerstr(pos->right), lowerstr(RIGHT_ENGINEERING)) == 0)
		{
		  counter++;
		}
	}
  mLock->unlock();
  if (counter >= 1)
	return false;
    	
  return true;
}

//
//find right value by right name
//return -1 if 
//
int  AosCliDaemonTortureTestPkg::getRight(char *right)
{
  if (strcmp(right, RIGHT_USER) == 0)
	{
	  return RIGHT_USER_INT;
	}
	
  if (strcmp(right, RIGHT_ENABLE) == 0)
	{
	  return RIGHT_ENABLE_INT;
	}

  if (strcmp(right, RIGHT_CONFIG) == 0)
	{
	  return RIGHT_CONFIG_INT;
	}
	
  if (strcmp(right, RIGHT_ENGINEERING) == 0)
	{
	  return RIGHT_ENGINEERING_INT;
	}
	
  return -1;
}

char* AosCliDaemonTortureTestPkg::getError(int code)
{
  if (code == ERROR_TIMEOUT)
	{
	  return ERROR_TIMEOUT_TEXT;
	}
  if (code == ERROR_FORBID_ENGINEERING_CHANGE)
	{
	  return ERROR_FORBID_ENGINEERING_CHANGE_TEXT;
	} 

  if (code == ERROR_FORBID_CONFIG_NUM)
	{
	  return ERROR_RORBID_CONFIG_NUM_TEXT;
	}
  if (code == ERROR_FORBID_NORMAL)	
	{
	  return ERROR_FORBID_NORMAL_TEXT;
	}
	
  return NULL;
}
