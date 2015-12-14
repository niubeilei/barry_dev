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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IILScannerObj_h
#define Aos_SEInterfaces_IILScannerObj_h

#include "IILUtil/IILEntrySelType.h"
#include "IILScanner/IILValueType.h"
#include "IILScanner/IILMatchType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILSelector.h"
#include "SEInterfaces/DataProcStatus.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/StrElemType.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosIILScannerObj : virtual public OmnRCObject
{
public:
	struct ComposorInfo
	{
		AosStrElemType::E	mType;
		int					mIndex;
		OmnString			mConstants;
		int					mStartPos;
		int					mEndPos;
	};

protected:
	vector<OmnString>		mComposorVector;
	vector<ComposorInfo>	mComposorInfo;
	OmnString				mOrigSep;
	OmnString				mTargetSep;

private:
	static AosIILScannerObjPtr		smIILScannerObj;

public:
	vector<OmnString>& getComposorVector() {return mComposorVector;}
	vector<ComposorInfo> &getComposorInfo() {return mComposorInfo;}
	OmnString getOrigSep() const {return mOrigSep;}
	OmnString getTargetSep() const {return mTargetSep;}
	
	virtual AosIILMatchType::E getMatchType() const = 0;
	virtual bool isControllingScanner() const = 0;
	virtual void setListener(const AosIILScannerListenerPtr &listner) = 0;
	virtual OmnString getIILName() const = 0;
	virtual bool isReject(const bool is_match) const = 0;
	virtual u64 getCrtValue() const = 0;
	virtual int getProgress() = 0;
	virtual bool start(const AosRundataPtr &rdata) = 0;

	virtual AosDataProcStatus::E moveNext(
						const char *&new_data, 
						int &len,
						const int total_matched,
						const AosRundataPtr &rdata) = 0;
	virtual AosDataProcStatus::E moveNext(
						OmnString &data,
						bool &has_more,
						const AosRundataPtr &rdata) = 0;
	
	virtual u32	getMatchedSize() = 0;
	virtual void resetMatchedIdx() = 0;
	virtual bool setMatchedValue(
						const AosDataRecordObjPtr &record, 
						bool &has_more,
						const AosRundataPtr &rdata) = 0;
	virtual bool setNotMatchedValue(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata) = 0;

	virtual AosQueryContextObjPtr getQueryContext() const = 0;
	virtual bool setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata) = 0;

	virtual bool reloadData(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata) = 0; 

	virtual void clear() = 0;
	
	virtual AosIILScannerObjPtr createIILScanner(
						const AosIILScannerListenerPtr &caller,
						const int seqno,
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata) = 0;
	
	static AosIILScannerObjPtr createIILScannerStatic(
						const AosIILScannerListenerPtr &caller,
						const int seqno,
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	static void setIILScannerObj(const AosIILScannerObjPtr &d) {smIILScannerObj = d;}
	static AosIILScannerObjPtr getIILScanner() {return smIILScannerObj;}
};
#endif
