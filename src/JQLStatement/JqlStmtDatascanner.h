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
#ifndef AOS_JQLStatement_JqlStmtDatascanner_H
#define AOS_JQLStatement_JqlStmtDatascanner_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtDataConnector.h"
#include "Util/String.h"

class AosJqlStmtDatascanner : public AosJqlStatement
{
public:

private:
	AosXmlTagPtr 					mDoc;
	//data from JQLParser
	bool							mNeedToOrder;
	OmnString						mName;
	OmnString 						mConnectorName;
	OmnString						mConnectorType;
	OmnString 						mSvrId;
	OmnString						mFileName;
	OmnString 						mEncoding;
	OmnString						mSplitSize;

	vector<AosJqlStmtDataConnector::AosDirList*>				*mDirList;
	OmnString						mErrmsg;

public:
	AosJqlStmtDatascanner();
	AosJqlStmtDatascanner(const OmnString &errmsg);
	~AosJqlStmtDatascanner();

	//getter/setters
	void setName(OmnString name);
	void setOrder();
	void setConnectorName(OmnString name){mConnectorName = name;}
	void setConnectorType(OmnString type){mConnectorType = type;}
	void setSvrId(int64_t svr_id){mSvrId = ""; mSvrId << svr_id;}
	void setFileName(OmnString file_name){mFileName = file_name;}
	void setEncoding(OmnString coding){mEncoding = coding;}
	void setSplitSize(OmnString split_size){mSplitSize = split_size;}
	void setDirList(vector<AosJqlStmtDataConnector::AosDirList*> *dirl);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDatascanner(const AosRundataPtr &rdata);
	bool showDatascanners(const AosRundataPtr &rdata);
	bool describeDatascanner(const AosRundataPtr &rdata);
	bool dropDatascanner(const AosRundataPtr &rdata);

	bool createConn(const AosRundataPtr &rdata);
};

#endif
