////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StContainer_h
#define AosSengTorUtil_StContainer_h

#include "Rundata/Ptrs.h"
#include "SEBase/SecReq.h"
#include "SEBase/SecOpr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/StType.h"
#include "SengTorUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include <vector>
using namespace std;

class AosStContainer1 : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Type
	{
		eInvalid,
		eUserCtnr,
		eDocCtnr
	};

	vector<u64>		mMembers;
	OmnString		mObjid;
	u64 	 		mLocdid;
	u64 	 		mSvrdid;
	Type 			mType;
	vector<AosStOprTranslatorPtr>	mTranslators[AosSecOpr::eMax];
	AosXmlTagPtr	mDomainOprArd;	

	AosStContainer1()
	:
	mLocdid(0),
	mSvrdid(0),
	mType(eInvalid)
	{
	}

	~AosStContainer1() {}

	OmnString getOperation(
					const AosStDocPtr &loal_doc, 
					const AosSecOpr::E opr_id, 
					const AosSengTestThrdPtr &thread);
	OmnString getObjid() const {return mObjid;}
	bool canDoOpr(
					const AosStDocPtr &local_doc,
					const AosSecOpr::E opr, 
					const AosSengTestThrdPtr &thread);
	bool canDoOpr(
					const AosStDocPtr &local_doc,
					const OmnString &operation,
					bool &granted,
					bool &denied, 
					bool &rollback, 
					const AosSengTestThrdPtr &thread);
	bool canDoOprNormal(
					const AosStDocPtr &local_doc, 
					const AosSecOpr::E opr, 
					const AosSengTestThrdPtr &thread);
	AosXmlTagPtr getOperationArd();
	bool addOperation(const AosXmlTagPtr &ctnr_xml, AosSengTestThrdPtr &thread);
	bool removeOperation(const AosXmlTagPtr &ctnr_xml, AosSengTestThrdPtr &thread);
	bool modifyOperation(const AosXmlTagPtr &ctnr_xml, AosSengTestThrdPtr &thread)
	{
		aos_assert_r(removeOperation(ctnr_xml, thread), false);
		aos_assert_r(addOperation(ctnr_xml, thread), false);
		return true;
	}
	bool removeTranslator(const OmnString &opr_id, const AosStOprTranslatorPtr &translator);
	bool addTranslator(
					const OmnString &opr_id, 
					const AosStOprTranslatorPtr &translator);
	AosXmlTagPtr getDomainOprArd() const {return mDomainOprArd;}
	void setDomainOprArd(const AosXmlTagPtr &p) {mDomainOprArd = p;}
	bool pickOperation(
					OmnString &cmd, 
					OmnString &operation, 
					bool &valid,
					const AosSengTestThrdPtr &thread);
	AosStOprTranslatorPtr pickTranslator();
	OmnString pickCommand(const AosSengTestThrdPtr &thread);
	bool pickOperation(
					OmnString &cmd, 
					OmnString &operation, 
					const AosSengTestThrdPtr &thread);
};
#endif

