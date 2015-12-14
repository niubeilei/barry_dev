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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ActionObj_h
#define Aos_SEInterfaces_ActionObj_h

#include "Jimo/Jimo.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/ActionType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskDataObj.h"

#include <vector>

class AosValueRslt;

class AosActionObj : public AosJimo, public AosTaskReporter
{
public:
	enum RunMode
	{
		eInvalidRunMode,

		eRunOnSmartDoc,
		eRunOnValue,
		eRunOnRecord,
		eRunOnBlob,
		eRunOnCacher,

		eMaxActionRunMode
	};

private:
	static AosActionObjPtr smAction;

protected:
	AosTaskObjPtr		mTask;
	AosBuffPtr			mBuff;
	AosActionType::E    mType;
	AosActionObjPtr    	mPrev;
	AosActionObjPtr    	mNext;
	bool				mIsTemplate;
	OmnString 			mErrorMsg;

public:
	AosActionObj(const u32 version);

	// Action interface
	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rundata) = 0;
	virtual bool run(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual bool run(const char *data, const int len, const AosRundataPtr &rdata) = 0;

	virtual bool run(		const AosTaskObjPtr &task, 
							const AosXmlTagPtr &sdoc, 
							const AosRundataPtr &rundata) = 0;
	virtual bool checkConfig(const AosXmlTagPtr &def, 
							const AosTaskObjPtr &task, 
							const AosRundataPtr &rdata) = 0;

	virtual AosActionObjPtr clone(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &) const = 0;

	virtual bool reset(const AosRundataPtr &rdata) {return true;}
	//----------------------------------
	// these interfacese use for taskActions.
	// the function call by one thread	
	virtual bool initAction(
							const AosTaskObjPtr &task, 
							const AosXmlTagPtr &sdoc,
							const AosRundataPtr &rdata) = 0;

	// the function call by multiple thread	
	virtual void onThreadInit(const AosRundataPtr &, void **data){};
	virtual bool runAction(const AosRundataPtr &rdata, void* data = 0) {return true;};
	virtual void onThreadExit(const AosRundataPtr &, void *data){};

	// the function call by onethread	
	virtual bool finishedAction(const AosRundataPtr &rdata) = 0;
	//----------------------------------

	//Jozhi used by ReduceTask2 to DataBase
	virtual bool createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata);
	virtual bool commit(const AosRundataPtr &rdata) { return true; }

	static bool runActions(const AosXmlTagPtr &def, 
							const OmnString &tagname,
							const AosRundataPtr &rdata);

	// Chen Ding, 2013/10/28
	static bool runAction( const AosRundataPtr &rdata, 
							const OmnString &act_def);

	static bool runActions( const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	static bool runAction( const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	// Chen Ding, 2013/06/18
	static bool runAction( 	const AosRundataPtr &rdata, 
							const OmnString &action_id,
							const AosXmlTagPtr &worker)
	{
		AosActionObjPtr action = getAction(action_id, worker, rdata);
		aos_assert_r(action, false);
		return action->run(worker, rdata);
	}

	// Chen Ding, 2013/02/07
	static bool runAction( const AosRundataPtr &rdata, 
							const AosXmlTagPtr &def, 
							const AosBuffPtr &buff);

	static AosActionObjPtr getAction(
							const AosActionType::E type,
							const AosXmlTagPtr &def,
							const AosRundataPtr &rdata);

	static AosActionObjPtr	getAction(
							const OmnString &type, 
							const AosXmlTagPtr &def,
							const AosRundataPtr &rdata)
	{
		return getAction(AosActionType::toEnum(type), def, rdata);
	}

	static inline AosActionObjPtr getAction(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata)
	{
		aos_assert_r(def, 0);

		OmnString actionid = def->getAttrStr(AOSTAG_ZKY_TYPE);
		if (actionid == "") actionid = def->getAttrStr(AOSTAG_TYPE);
		AosActionType::E type = AosActionType::toEnum(actionid);
		return getAction(type, def, rdata);
	}

	static OmnString getActionId(const AosXmlTagPtr &action);

	static AosActionObjPtr getActionObj() {return smAction;}
	static void setActionObj(const AosActionObjPtr &d) {smAction = d;}

	bool registerAction(const AosActionObjPtr &action, const OmnString &name);

	// Actions can be chained. These functions are used to chain actions
	AosActionType::E    getType() const {return mType;}
	AosActionObjPtr    	prevAction() const {return mPrev;}
	AosActionObjPtr    	nextAction() const {return mNext;}
	void                setNext(const AosActionObjPtr &a) {mNext = a;}
	void                setPrev(const AosActionObjPtr &a) {mPrev = a;}
	void                setTask(const AosTaskObjPtr &task) {mTask = task;}
	bool setErrorMsg(const OmnString &msg)
	{ 
		mErrorMsg = msg;
		return true;
	}
	OmnString getErrorMsg()
	{
		return mErrorMsg;
	}

	bool actionFinished(const AosRundataPtr &rdata)
	{
		if (mTask)
		{
			AosActionObjPtr thisptr(this, true);
			return mTask->actionFinished(thisptr, rdata);
		}
		return true;
	}

	bool actionFailed(const AosRundataPtr &rdata)
	{
		if (mTask)
		{
			AosActionObjPtr thisptr(this, true);
			return mTask->actionFailed(thisptr, rdata);
		}
		return true;
	}
	
	//Phil, 2014/08/05
	void setBuff(AosBuffPtr buff)
	{
		mBuff = buff;
	}

	AosBuffPtr getBuff()
	{
		return mBuff;
	}
	
	// Ketty 2013/12/26
	//static bool checkConfigStatic(
	//		const AosRundataPtr &rdata,
	//		const AosXmlTagPtr &conf,
	//		set<OmnString> &data_col_ids);
	
	// Ketty 2013/12/26
	virtual bool checkConfig(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf,
			set<OmnString> &data_col_ids) = 0;
	
	virtual bool config(const u64 task_docid,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata) = 0;

	virtual int getMaxThreads() {return 2;}

	virtual void outPutPost() = 0;

};
#endif
