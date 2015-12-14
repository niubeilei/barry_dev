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
// 05/27/2011	Created by Wynn
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocImport_h
#define Aos_SmartDoc_SdocImport_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include <vector>
#include <map>
using namespace std;

struct AosImport
{
	AosXmlTagPtr					mXml;
	AosXmlTagPtr					mActions;
	bool							mModify;
	AosRundataPtr					mRundata;
	
	AosImport(const AosXmlTagPtr &xml,
			const AosXmlTagPtr &actions, 
			const bool &modify,
			const AosRundataPtr &rdata)
		:
		mXml(xml),
		mActions(actions),
		mModify(modify),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
	{}
	~AosImport(){}
};

struct AosSplit
{
	AosXmlTagPtr					mActions;
	bool							mCover;
	bool							mHeaderCheckedFlag;	
	OmnString						mTableName;
	OmnString						mIdentify;
	OmnString						mContainer;
	OmnConnBuffPtr					mBuff;
	AosRundataPtr					mRundata;
	
	AosSplit(
			const AosXmlTagPtr &actions,
			const bool &cover,
			const OmnString	&tname,
			const OmnString &identify,
			const OmnString &container,
			const OmnConnBuffPtr &buff,
			const AosRundataPtr &rdata)
		:
		mActions(actions),
		mCover(cover),
		mHeaderCheckedFlag(false),
		mTableName(tname),
		mIdentify(identify),
		mContainer(container),
		mBuff(buff),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
	{}
	~AosSplit(){}
};
class AosSdocImport : public AosSmartDoc, virtual public OmnThreadedObj
{
public:
	enum
	{
		eCreateDocId = 0,
		eSplitXmlId = 1,
		eSplitXmlLevel = 4000000 // 4M
	};

private:
	typedef vector<OmnString> 						AosColumnVect;
	typedef map<OmnString, OmnString>				AosHeader2AttrMap;
	typedef map<OmnString, OmnString>				AosSysBdMap;
	typedef	map<OmnString, OmnString>::iterator		AosHeader2AttrMapItr;
	typedef	map<OmnString, OmnString>::iterator		AosSysBdMapItr;
	
	
	int                 			mNumReqs;
	int                 			mPageSize;
	AosColumnVect					mColumnVect;
	AosHeader2AttrMap				mHeader2AttrMap;
	AosSysBdMap						mSysBdMap;
	queue<AosImport> 				mQueue;
	queue<AosSplit> 				mSplitQueue;
	OmnThreadPtr        			mCreateDocThread;
	OmnThreadPtr        			mSplitXmlThread;
	OmnString						mCreateMethod;

public:
	AosSdocImport(const bool flag);
	~AosSdocImport();

	// Smartdoc Interface
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	virtual AosSmartDocObjPtr clone(){return OmnNew AosSdocImport(false);} 

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
private:
	bool					getExcel(
								const bool &cover,
								const OmnString &identify,
								const OmnString &container,
								const OmnString &tname,
								const AosXmlTagPtr &actions,
								OmnString &filedir,
								OmnString &filename, 
								const AosRundataPtr &rdata);
	bool    				createDoc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool    				splitXml(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool					createDoc(
								const AosXmlTagPtr &xml,
								const AosXmlTagPtr &actions,
								const bool &modify,         
								const AosRundataPtr &rdata);
	bool					createDoc(const AosImport &import);
	bool					splitXml(const AosSplit &split);
	bool					initHeader(
								const AosXmlTagPtr &sdoc,
								const AosRundataPtr &rdata);
	bool					initSysBd(
								const AosXmlTagPtr &sdoc, 
								const AosRundataPtr &rdata);
	bool					isInvalidSysBd(const OmnString &sysbd);
	bool					addRequest(         
								const AosXmlTagPtr &xml,
								const AosXmlTagPtr &actions,
								const bool &modify,
								const AosRundataPtr &rdata);
	bool					addSplitRequest(         
								const AosXmlTagPtr &actions,
								const bool &cover,
								const OmnString &identify,
								const OmnString &tname,
								const OmnString &container,
								const OmnConnBuffPtr &docBuff,
								const AosRundataPtr &rdata);
	void					startThread();
	bool					splitXml(                  
				        		const OmnConnBuffPtr &buff,
								const AosXmlTagPtr &actions,
								const bool &cover,
								const OmnString &identify,
								const OmnString &tname,
								const OmnString &container,
								const AosRundataPtr &rdata);
	bool					createXml(
								const char *start,
								const int &len,
								const AosXmlTagPtr &actions,
								const bool &cover,
								const OmnString identify,
								const OmnString &container,
								const AosRundataPtr &rdata);
	bool					checkHeader(
								const AosXmlTagPtr &row, 
								const AosRundataPtr &rdata);
	bool					initAttrs(
								const OmnString &identify,
								OmnString &identify_value,
								const AosXmlTagPtr &row,
								map<OmnString, OmnString> &attrs,
								const AosRundataPtr &rdata);
	AosXmlTagPtr			createCoverDoc(
								bool &modify,
								const OmnString &container,
								const OmnString &identify,
								const OmnString &identify_value,
								map<OmnString, OmnString> &attrs,
								const AosRundataPtr &rdata);
	AosXmlTagPtr			createNewDoc(
								const OmnString &container,
								map<OmnString, OmnString> &attrs);
	bool					createSysDoc(
								const bool &modify,
								const AosXmlTagPtr &doc,
								const AosRundataPtr &rdata);
	bool
							createTable(
								AosXmlTagPtr doc,
								const AosXmlTagPtr &actions,
								const bool &cover,
								const OmnString identify,
								OmnString &identify_value,
								const OmnString &container,
								const AosRundataPtr &rdata);
	bool
							createRowNode(
								const char *start,
								const int &len,
								AosXmlTagPtr &doc,
								const OmnString identify,
								OmnString &identify_value,
								const AosRundataPtr &rdata);
};
#endif
