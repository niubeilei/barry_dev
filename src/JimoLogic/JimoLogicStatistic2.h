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
// Modification History:
// 08/08/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogic_JLStatistic2_h
#define AOS_JimoLogic_JLStatistic2_h

#include "JimoLogic/JimoLogic.h"
#include "SEInterfaces/JimoProgObj.h"
#include <vector>

class AosJLStatistic2: public AosJimoLogic
{
private:
	OmnString				mStatName;
	OmnString				mInput;
	OmnString				mTableName;
	OmnString				mTimeField;
	OmnString				mTimeUnit;
	OmnString				mTimeFormat;
	OmnString				mStatIdx;
	vector<OmnString>		mKeys;
	vector<OmnString>		mMeasures;
	AosJimoProgObjPtr		mJimoProg;
	AosXmlTagPtr 			mTableDoc;
	OmnString 				mCondsStr;
	OmnString				mStatModelName;
	
public:
	AosJLStatistic2(
			const AosRundataPtr &rdata,
			const string &input_name,
	//		const AosXmlTagPtr &def,
			const OmnString &table_name,
			const AosJimoProgObjPtr &jimo_prog);
	//		const AosXmlTagPtr table_doc);

	AosJLStatistic2(const AosJLStatistic2 &rhs);
	AosJLStatistic2(int version);
	~AosJLStatistic2();

	virtual AosJimoPtr cloneJimo()const {return 0;}

private:
	bool createJsonStr(
			const AosRundataPtr &rdata,
			const vector<OmnString> &keys,
			const vector<OmnString> &measures,
			const OmnString &record_name);

	bool checkIsVirtualTable(const AosRundataPtr &rdata);
	bool createJsonStrForMutilTable(const AosRundataPtr &rdata);

};
#endif

