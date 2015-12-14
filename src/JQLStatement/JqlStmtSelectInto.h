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
// 2014/12/11 Created By Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtSelectInto_H
#define AOS_JQLStatement_JqlStmtSelectInto_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelect.h"

#include "Util/String.h"

#include <vector>
#include <map>


class AosJqlStmtSelectInto : public AosJqlStatement
{
public:
	AosRundataPtr				mRundata;
	AosJqlSelectPtr				mSelect;
	OmnString 					mPath;
	OmnString 					mErrmsg;

	vector<OmnString>			*mFormat;
	u32							mMaxThreads;
	OmnString 					mJobName;

public:
	AosJqlStmtSelectInto();
	~AosJqlStmtSelectInto();

	void setSelect(AosJqlSelect *select){mSelect=select;}
	void setFilePath(OmnString path){mPath=path;}

	virtual bool 				run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement*	clone();
	virtual void 				dump();
	virtual OmnString			getErrmsg()const {return mErrmsg;}

	void setFormat(vector<OmnString> *format){ mFormat=format; }
	void setMaxThreads(u32 &max) { mMaxThreads=max; }
	void setJobName(OmnString jobname){ mJobName=jobname; }

private:


};
#endif
