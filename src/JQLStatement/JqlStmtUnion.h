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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtUnion_H
#define AOS_JQLStatement_JqlStmtUnion_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelect.h"

#include "Util/String.h"

#include <vector>
#include <map>


class AosJqlStmtUnion : public AosJqlStatement
{
public:
	AosRundataPtr				mRundata;
	vector<AosJqlSelectPtr>		mSelects;
	AosJqlSelectPtr				mFirstSelect;
	AosJqlStmtUnionPtr			mUnion;
	OmnString 					mErrmsg;
	AosXmlTagPtr				mTableDoc;
	AosXmlTagPtr				mTablesDoc;

	bool						mIsUnionAll;


public:
	AosJqlStmtUnion();
	~AosJqlStmtUnion();

	void setIsUnionAll(bool v){mIsUnionAll = v;}
	void setSelect(AosJqlSelect *select){mSelects.push_back(select);}
	void setFirstSelect(AosJqlSelectPtr select){mFirstSelect = select;}
	void setUnion(AosJqlStmtUnion* _union){mUnion = _union;}
	AosXmlTagPtr getTableDoc(){return mTableDoc;}

	virtual bool 				run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement*	clone();
	virtual void 				dump();
	virtual OmnString			getErrmsg()const {return mErrmsg;}



private:

	bool init(const AosRundataPtr &rdata);
	bool generateQueryConf(const AosRundataPtr &rdata);
	bool output_data(const AosRundataPtr &rdata, const AosXmlTagPtr &data);

};
#endif
