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
#ifndef AOS_JQLStatement_JqlStmtDataConnector_H
#define AOS_JQLStatement_JqlStmtDataConnector_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDataConnector : public AosJqlStatement
{
public:
	
	struct AosDirList
	{
		OmnString mFileName;
		u32		  mSvrId;
	};

	typedef AosJqlStmtDataConnector::AosDirList AosDirList; 

private:
	//data from JQLParser
	OmnString				mConnectorName;
	OmnString				mConnectorType;
	OmnString				mSvrId;
	OmnString				mFileName;
	OmnString				mCoding;
	OmnString				mSplitSize;
	vector<AosDirList*>		*mDirList;

public:
	OmnString				mErrmsg;

public:
	AosJqlStmtDataConnector(const OmnString &errmsg);
	AosJqlStmtDataConnector();
	~AosJqlStmtDataConnector();

	//getter/setters
	void setName(OmnString name);
	void setType(OmnString type);
	void setSvrId(int64_t svr_id);
	void setFileName(OmnString file_name);
	void setCoding(OmnString coding);
	void setSplitSize(OmnString split_size);
	void setDirList(vector<AosDirList*> *dirl);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDataConnector(const AosRundataPtr &rdata);  
	bool showDataConnectors(const AosRundataPtr &rdata);   
	bool describeDataConnector(const AosRundataPtr &rdata);
	bool dropDataConnectors(const AosRundataPtr &rdata);   
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
};

#endif
