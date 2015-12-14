////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransPest_TransPest_h
#define AOS_TransPest_TransPest_h

#include <map>
#include "SEUtil/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Util/String.h"
#include "Util/Queue.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include "XmlInterface/XmlRc.h"
#include "Porting/TimeOfDay.h"
#include "TransPest/Ptrs.h"



class AosTransPest : virtual public OmnRCObject, 
				  virtual public OmnThreadedObj,
				  virtual public OmnCommListener 
{
	OmnDefineRCObject;

	enum
	{
		eHeartbeatThrdId,
		eSendThrdId,
		eCheckThrdId,
		eReconnectFreq = 10,
		eCheckThrdFreq = 30,
		eHearbeatThrdFreq = 60,
		eMaxFileSize = 2000000000 

	};

public:
	struct Trans
	{
		OmnString		trans;
		int 			tid;
		u32 			time;
		u32 			length;
		u32				mFileSize;
		int 			sending;

		Trans(OmnString tr, int id):trans(tr),tid(id),sending(0)
		{
			time = OmnGetSecond();
			length = trans.length();
		}

		bool saveToFile(const OmnFilePtr &file)
		{
			OmnString tmp;
			tmp << length << ":" << time << "\n\r";
			tmp << trans  << "\n\r\n\r";
			bool rslt = file->append(tmp, true);
			aos_assert_r(rslt, false);
			mFileSize += tmp.length();
			return true;
		}
	};

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr   mCondVar;
	OmnString		mRemoteAddr;
	int				mRemotePort;
	OmnTcpCommCltPtr	mConn;
	OmnThreadPtr    mSendThread;
	OmnThreadPtr    mHeartbeatThread;
	OmnThreadPtr    mCheckThread;
	OmnQueue<Trans*, 10, 10, 200> mSendQueue;
	map<u64, Trans*> mProcQueue;
	OmnFilePtr		mFile;
	u32				mFileSize;
	OmnString		mDirName;
	OmnString		mFileName;
	int				mCrtSeqno;


	static OmnMutex			sLock;
	static u32 				sCrtTransId;

public:
	AosTransPest();
	~AosTransPest();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	// OmnCommListener Interface
	virtual bool 	msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString getCommListenerName() const {return "";};

	bool 	saveTrans(const OmnString &trans);
	int static getTransId();
	bool	init(const AosXmlTagPtr &config);

private:

	bool 	sendRequestPriv(Trans *trans);
	void	connect();
	bool	reconnect();
	void 	sendHeartbeat();
	bool    heartbeatThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool	sendThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	OmnFilePtr	getFile(Trans *trans, bool readOnly = false);
};
#endif

