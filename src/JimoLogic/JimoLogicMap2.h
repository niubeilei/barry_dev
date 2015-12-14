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
// 2015/04/23 Created by Andy
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogic_JLMap2_h
#define AOS_JimoLogic_JLMap2_h

#include "JimoLogic/JimoLogic.h"
#include "SEInterfaces/JimoProgObj.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosJLMap2: public AosJimoLogic
{
	OmnDefineRCObject;
private:
	OmnString 			mMapName;
	OmnString 			mValue;
	OmnString			mInput;
	OmnString			mCondsStr;
	OmnString			mTableName;
	vector<OmnString>	mKeys;

public:
	AosJLMap2(
			const AosRundataPtr &rdata,
			const AosJimoProgObjPtr &prog,
			const string &inputds_name,
			//const AosXmlTagPtr &def,
			const string &tablename,
			const bool prime);
	AosJLMap2(int version);
	~AosJLMap2();

	virtual AosJimoPtr cloneJimo() const;

private:
	bool createJsonStr(
			const AosRundataPtr &rdata,
			const AosJimoProgObjPtr &prog);
};
#endif

