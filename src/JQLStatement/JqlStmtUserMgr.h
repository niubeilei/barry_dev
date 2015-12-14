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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtUserMgr_H
#define AOS_JQLStatement_JqlStmtUserMgr_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtUserMgr : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString		mUserName;
	OmnString		mPwd;

public:
	OmnString		mErrmsg;

public:
	
	AosJqlStmtUserMgr(const OmnString errmsg);
	AosJqlStmtUserMgr();
	~AosJqlStmtUserMgr();

	void setErrmsg(const OmnString &errmsg) {mErrmsg = errmsg;}
	//getter/setters
	void setUserName(OmnString name);
	void setPwd(OmnString pwd);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool alter(const AosRundataPtr &rdata);
	bool create(const AosRundataPtr &rdata);  
	bool show(const AosRundataPtr &rdata);   
	bool describe(const AosRundataPtr &rdata);
	bool drop(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

	static OmnString getObjid(OmnString const name);

private:
	
};

#endif
