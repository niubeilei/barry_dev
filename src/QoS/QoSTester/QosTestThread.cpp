#include "QosTestThread.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/netdevice.h"
#include "KernelSimu/if_ether.h"
#include "Thread/Thread.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "QoS/qos_global.h"
#include "Debug/Debug.h"
#include "QoS/QoSTester/QosTestUtil.h"

AosQoSTestThread::AosQoSTestThread()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = new OmnThread(thisPtr, "QosTestThread", 0, true, true);
	times = 0;
}

AosQoSTestThread::~AosQoSTestThread()
{
}

bool	
AosQoSTestThread::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 last_rcv_time = 0;
	u32 cur_time;
	OmnTrace << "Enter AosQosTestThread::threadFunc." << endl;
	while ( state == OmnThrdStatus::eActive && times < MAX_TEST_TIMES )
	{
		cur_time = QOS_GET_JIFFIES();
		if ((cur_time - last_rcv_time) > 10 && times < MAX_TEST_TIMES)
		{
			last_rcv_time = cur_time;
			times++;
			aos_trace("test times %d", times);
			//qos_rcv_skb(0);
		}
		aos_timer_check_and_run();		
		usleep(2*1000); // sleep 2 ms
	}
}

bool	
AosQoSTestThread::signal(const int threadLogicId)
{
	return true;
}

bool	
AosQoSTestThread::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool	
AosQoSTestThread::start()
{
	mThread->start();
}

