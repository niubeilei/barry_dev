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
#ifndef AosSengTorUtil_StOprTranslator_h
#define AosSengTorUtil_StOprTranslator_h

#include "Conds/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEBase/SecOpr.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/StType.h"
#include "SengTorUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosStOprTranslator : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum E
	{
		eInvalid,

		eDirectMap,
		eDocAttr,

		eMax
	};

protected:
	AosSecOpr::E			mSecOpr;
	AosStOprTranslatorPtr	mPrev;
	AosStOprTranslatorPtr	mNext;
	E						mType;
	AosXmlTagPtr			mCondSdoc;
	AosConditionObjPtr		mCond;
	int						mOprSeqno;

public:
	AosStOprTranslator(const E type, const bool flag);
	~AosStOprTranslator();

	// StOprTranslator Interface
	virtual OmnString determineOperation(
		const AosStDocPtr &local_doc,
		const AosSecOpr::E opr_id, 
		const AosSengTestThrdPtr &thread);
	virtual OmnString setRandContents(const AosRundataPtr &rdata) = 0;
	virtual AosStOprTranslatorPtr clone() const = 0;
	virtual bool pickCommand(OmnString &cmd, 
						OmnString &operation, 
						const AosSengTestThrdPtr &thread);

	// virtual bool checkOperation(
	// 					const AosStContainerPtr &st_ctnr,
	// 					const AosXmlTagPtr &xml_ctnr,
	//					const AosSengTestThrdPtr &thread);
	virtual bool removeOperation(
						const OmnString &opr_id,
						const AosStContainerPtr &st_ctnr,
						const AosXmlTagPtr &xml_ctnr,
						const AosSengTestThrdPtr &thread);

	AosStOprTranslatorPtr getPrev() const {return mPrev;}
	AosStOprTranslatorPtr getNext() const {return mNext;}
	void setOprSeqno(const int s) {mOprSeqno = s;}
	void resetPointers() {mPrev = 0; mNext = 0;}
	void setPrev(const AosStOprTranslatorPtr &p) {mPrev = p;}
	void setNext(const AosStOprTranslatorPtr &n) {mNext = n;}

	static bool addOperation(
						const AosXmlTagPtr &xml_ctnr, 
						const AosStContainerPtr &st_ctnr, 
						const AosSengTestThrdPtr &thread);
	static bool isValid(const E type) {return type > eInvalid && type < eMax;}
	static AosStOprTranslatorPtr getTranslator(const AosStOprTranslator::E type);

protected:
	bool generateRandCond(const AosRundataPtr &rdata);

private:
	bool registerTranslator(const AosStOprTranslatorPtr &translator);

	static E getRandType()
	{
		E idx = (E)OmnRandom::intByRange(
				eDirectMap, eDirectMap, 30,
				eDocAttr, eDocAttr, 70);
		aos_assert_r(isValid(idx), eInvalid);
		return idx;
	}
};
#endif

