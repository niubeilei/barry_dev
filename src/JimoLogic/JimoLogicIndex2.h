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
#ifndef AOS_JimoLogic_JLIndex2_h
#define AOS_JimoLogic_JLIndex2_h

#include "JimoLogic/JimoLogic.h"
#include "SEInterfaces/JimoProgObj.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosJLIndex2: public AosJimoLogic
{
	OmnDefineRCObject;
private:
	//OmnString 			mIdxName;
	OmnString 			mInput;
	OmnString			mCondsStr;
	OmnString			mTableName;
	vector<OmnString>	mKeys;
	AosXmlTagPtr		mTableDoc;
	AosJimoProgObjPtr	mJimoProg;

public:
	AosJLIndex2(
			const AosRundataPtr &rdata,
			const OmnString &input_name,
			//const AosXmlTagPtr &def,
			const OmnString &tablename,
			const AosJimoProgObjPtr &jimo_prog,
			const AosXmlTagPtr &table_doc,
			const bool prime);

	AosJLIndex2(const AosJLIndex2 &rhs);
	AosJLIndex2(int version);
	~AosJLIndex2();

	virtual AosJimoPtr cloneJimo() const;

private:
	bool createJsonStr(
			const AosRundataPtr &rdata,
			//const vector<OmnString> &keys,
			const OmnString &record_name);

	bool checkIsVirtualTable(const AosRundataPtr &rdata);
	bool createJsonStrForMutilTable(const AosRundataPtr &rdata);

};
#endif

