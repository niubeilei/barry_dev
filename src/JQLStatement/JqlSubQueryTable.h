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
#ifndef AOS_JQLStatement_JqlSubQueryTable_H
#define AOS_JQLStatement_JqlSubQueryTable_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelectTable.h"
#include "JQLStatement/JqlSelect.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"

class AosJqlSubQueryTable : public AosJqlSelectTable
{
public:
	AosJqlSelectPtr				mVirtualTable; // by SubQuery
	OmnString					mVirtualName;

public:
	AosJqlSubQueryTable();
	~AosJqlSubQueryTable();

	virtual bool init(const AosRundataPtr &rdata,const AosJqlSelectPtr &select);
	virtual OmnString generateTableConf(const AosRundataPtr &rdata);
	virtual OmnString generateWhereConf(const AosRundataPtr &rdata);
//	virtual OmnString generateOutputDatasetConf(const AosRundataPtr &rdata);

	virtual	bool getAllField(
			const AosRundataPtr &rdata, 
			const AosJqlSelectPtr &select, 
			vector<AosJqlSelectFieldPtr> *&fields);

	bool generateInputDatasetConf(const AosRundataPtr &rdata);

	virtual void setName(char *name);
	virtual void setVirtualTable(AosJqlSelect *virtual_table);
	virtual AosJqlSelectPtr getVirtualTbale();
	virtual OmnString getName();
	//virtual AosJqlSubQueryTablePtr clone(const AosRundataPtr &rdata)const ;

};

#endif
