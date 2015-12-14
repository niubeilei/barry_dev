#ifndef _Aos_PCTESTCASE
#define _Aos_PCTESTCASE
#include "PCTestcaseConfig.h"
#include "UtilComm/TrafficGenThread.h"
#include "Thread/Mutex.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/ConnBuff.h"

class AosPCTestcase;
typedef std::vector<AosPCTestcase*> AosPCTestcasePtrArray;

enum CheckResult
{
	eCheckResult_Failed = -1,
	eCheckResult_Ok = 0	
};


class AosPCTestcase : public AosTcpTrafficGenListener 
{
public:

	virtual int check(char* recvBuffer, int recvLen)=0;

	void setTCData(TCData* tcData){mTCData = tcData;};
	TCData* getTCData(){return mTCData;};
				
protected:
	TCData* mTCData;
};

#endif
