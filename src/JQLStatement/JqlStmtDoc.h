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
#ifndef AOS_JQLStatement_JqlStmtDoc_H
#define AOS_JQLStatement_JqlStmtDoc_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDoc : public AosJqlStatement
{
private:
	//data from JQLParser
	u64					mDocDocid;
	OmnString			mDocObjid;
	OmnString			mShowType;
	OmnString			mDocConf;

public:
	OmnString			mErrmsg;

public:
	AosJqlStmtDoc(const OmnString &errmsg);
	AosJqlStmtDoc();
	~AosJqlStmtDoc();

	//getter/setters
	void setDocObjid(OmnString objid);
	void setDocDocid(unsigned long docid);
	void setDocConf(OmnString doc_conf);


	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDoc(const AosRundataPtr &rdata);
	bool showDoc(const AosRundataPtr &rdata);

};

#endif
