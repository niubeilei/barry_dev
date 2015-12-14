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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogic_JimoLogicDoc2_h
#define AOS_JimoLogic_JimoLogicDoc2_h

#include "JimoProg/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "JimoLogic/JimoLogic.h"
#include "Thread/Ptrs.h"
#include "AosConf/DataSet.h" 
#include "SEUtil/JqlTypes.h"
#include <vector>
#include <map>

class AosJLDoc2: public AosJimoLogic
{
private:
	OmnString					mInput;
	OmnString					mTableName;
	AosJimoProgObjPtr			mJimoProg;
	vector<OmnString>			mRecords;
	vector<OmnString>			mVirtualFieldsExpr;
	bool 						mPrime;


public:
	// jimodb-632
	AosJLDoc2(
		const AosRundataPtr &rdata,
		const string &input_name,
		const AosXmlTagPtr &table_doc,
		const string &tablename,
		const AosJimoProgObjPtr &jimo_prog,
		const bool prime_table);

	AosJLDoc2(const AosJLDoc2 &rsh);
	AosJLDoc2(int version);
	~AosJLDoc2();

	virtual AosJimoPtr cloneJimo()const;

private:
	bool 	createJsonStr(const AosRundataPtr &rdata);
	bool	procVirtualField(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &tabledoc);
};
#endif

