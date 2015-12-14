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
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_StatModel_h
#define Aos_SEInterfaces_StatModel_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/StatModelObj.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/Ptrs.h"
#include "StatUtil/Jimos/StatMeasure.h"
#include "StatUtil/Jimos/StatTime.h"
#include "StatUtil/Jimos/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

#define AOS_EMPTY_KEY   "_0x28_EMPTY_KEY_0x29_"

using namespace std;


class AosStatModel : public AosStatModelObj
{
	OmnDefineRCObject;

private:
	OmnString 						mName;
	OmnString 						mTimeFieldName;
	vector<AosStatMeasureObjPtr> 	mMeasureList;
	vector<AosDataFieldObjPtr> 		mKeyList;
	AosXmlTagPtr					mStatDoc;

	OmnString 						mCondStr;
	AosStatTimePtr					mStatTime;
	vector<OmnString>				mKeyFields;
	vector<OmnString>				mMeasureFields;
	vector<OmnString>				mShuffleFields;
public:
	AosStatModel(const OmnString &statModelName);

	AosStatModel(const int version);
	
	virtual ~AosStatModel();

	bool config(const AosXmlTagPtr &conf);
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual OmnString getName() const {return mName;}
	virtual OmnString getTimeFieldName() const {return mTimeFieldName;}
	virtual vector<AosDataFieldObjPtr> getKeys() {return mKeyList;}
	virtual AosDataFieldObjPtr getKey(const OmnString &name);

	virtual vector<AosStatMeasureObjPtr> getMeasures() const {return mMeasureList; }
	virtual AosStatMeasureObjPtr getMeasure(const OmnString &name) const;

	virtual AosStatModelObjPtr createStatModel(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);

	vector<OmnString> getKeyFields();

	vector<OmnString> getMeasures();
	
	vector<OmnString> getShuffleFields();
	
	AosStatTimePtr getTime();

	OmnString getCond();

	static OmnString getDbTableName(
				AosRundata *rdata,
				const OmnString &statModelName);

};

#endif
