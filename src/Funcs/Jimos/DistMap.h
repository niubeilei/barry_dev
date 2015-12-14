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
// 2015/04/30 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_DistMap_h
#define Aos_Funcs_Jimos_DistMap_h

#include "Funcs/GenFunc.h"
#include "JQLStatement/JqlStatement.h"

#include "IILEntryMap/Ptrs.h"
#include "IILEntryMap/IILEntryMapMgr.h"

class AosDistMap : public AosGenFunc
{
	OmnDefineRCObject;

private:
	AosIILEntryMapPtr	mDistMap;
	AosDataType::E		mDataType;

public:
	AosDistMap(const int version);
	AosDistMap();
	AosDistMap(const AosDistMap &rhs);
	~AosDistMap();
	
	virtual AosDataType::E getDataType(
					AosRundata *rdata, 
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata *rdata, 
					AosValueRslt &value, 
					AosDataRecordObj *record);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

	virtual bool syntaxCheck(
				const AosRundataPtr &rdata, 
				OmnString &errmsg);

private:
	
	OmnString 	getCubeId(const OmnString &key);

};
#endif

