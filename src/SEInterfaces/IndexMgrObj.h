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
// 2014/01/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IndexMgrObj_h
#define Aos_SEInterfaces_IndexMgrObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#define AOSINDEXTYPE_NORM			"norm"
#define AOSINDEXTYPE_PARAL_IIL		"paral_iil"

class AosIndexMgrObj : public AosJimo
{
	static  AosIndexMgrObjPtr smIndexMgrObj;

public:
	AosIndexMgrObj(const int version);

	static AosIndexMgrObjPtr getIndexMgr(AosRundata *rdata);

	static bool getIILNamesStatic(
					AosRundata *rdata, 
					const AosQueryReqObjPtr &term, 
					const OmnString &iilname, 
					vector<OmnString> &iilnames);

	static bool isParalIILStatic(
					AosRundata *rdata, 
					const OmnString &iilname);

	static bool createIndexEntryStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);

	static AosXmlTagPtr getIndexDefDocStatic(
					AosRundata *rdata, 
					const OmnString &iilname);

	static AosXmlTagPtr getIndexDefDocStatic(
					AosRundata *rdata, 
					const OmnString &table_name, 
					const OmnString &field_name);

	static bool resolveIndexStatic(
					AosRundata *rdata, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					OmnString &iilname);

	static bool convertIILNameStatic(
					AosRundata *rdata, 
					const int epoch_day, 
					const OmnString &iilname,
					bool &converted,
					OmnString &new_iilname);

	static bool isNormalIILStatic(
					AosRundata *rdata, 
					const OmnString &iilname);

	virtual bool isNormalIIL(
					AosRundata *rdata, 
					const OmnString &iilname) = 0;

	virtual AosXmlTagPtr getIndexDefDoc(
					AosRundata *rdata, 
					const OmnString &iilname) = 0;

	virtual AosXmlTagPtr getIndexDefDoc(
					AosRundata *rdata, 
					const OmnString &table_name, 
					const OmnString &field_name) = 0;

	virtual bool isParalIIL(
					AosRundata *rdata, 
					const OmnString &iilname) = 0;

	virtual bool createEntry(
					AosRundata *rdata,
					const AosXmlTagPtr &def) = 0;
	
	static bool createEntryStatic(
					AosRundata *rdata,
					const AosXmlTagPtr &def);
	
	virtual bool modifyEntry(
					AosRundata *rdata,
					const AosXmlTagPtr &def) = 0;

	static bool modifyEntryStatic(
					AosRundata *rdata,
					const AosXmlTagPtr &def);

	virtual bool removeEntry(
					AosRundata *rdata,
					const OmnString &table_name,
					const OmnString &field_name) = 0;

	static bool removeEntryStatic(
					AosRundata *rdata,
					const OmnString &table_name,
					const OmnString &field_name);

	virtual bool resolveIndex(
					AosRundata *rdata, 
					const OmnString &table_name,
					const OmnString &field_name, 
					OmnString &iilname) = 0;

	virtual bool getIILNames(
					AosRundata *rdata, 
					const AosQueryReqObjPtr &query_req, 
					const OmnString &iilname, 
					vector<OmnString> &iilnames) = 0;

	virtual bool convertIILName(
					AosRundata *rdata, 
					const int epoch_day, 
					const OmnString &iilname, 
					bool &converted,
					OmnString &new_iilname) = 0;

	virtual bool createEntry(
					AosRundata *rdata, 
					const OmnString iilname, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					const OmnString &index_type) = 0;

	static bool createEntryStatic(
					AosRundata *rdata, 
					const OmnString iilname, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					const OmnString &index_type);
};
#endif

