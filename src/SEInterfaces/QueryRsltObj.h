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
// 11/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryRslt_h
#define Aos_SEInterfaces_QueryRslt_h

#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosQueryRsltObj : virtual public OmnRCObject
{
	static AosQueryRsltObjPtr	smObject;

public:
	static AosQueryRsltObjPtr getQueryRsltStatic();
	static void returnQueryRsltStatic(const AosQueryRsltObjPtr &rslt);
	static void setQueryRslt(const AosQueryRsltObjPtr &obj);

	virtual u64 	getDocid(const i64 &idx) const = 0;
//	virtual AosBuffPtr	getValueBuff() const = 0;
	virtual AosBuffArrayPtr getValues()const = 0;
	virtual bool	serializeFromXml(const AosXmlTagPtr &xml) = 0;
	virtual bool 	appendU64Value(const u64 &val) = 0;
	virtual bool 	appendI64Value(const i64 &val) = 0;
	virtual bool 	appendD64Value(const d64 &val) = 0;
	virtual bool 	isWithValues() const = 0;
	virtual bool 	appendStrValue(const char *str) = 0;
	virtual void 	clear() = 0;
	virtual void 	clean() = 0;
	virtual bool 	appendBlockRev(const u64* docs, const i64 &num_docs) = 0;
	virtual bool 	appendBlock(const u64* docs, const i64 &num_docs) = 0;
	virtual void 	setDataReverse(const bool reverse) = 0;
	virtual AosQueryRsltObjPtr getQueryRslt() = 0;
	virtual void 	returnQueryRslt(const AosQueryRsltObjPtr &rslt) = 0;
	virtual bool	hasMoreDocid() = 0;
	virtual u64  	nextDocid(bool &finished) = 0;
	virtual bool 	nextDocid(u64 &docid, bool &finished, const AosRundataPtr &rdata) = 0;
	virtual void	setWithValues(const bool withvalue) = 0;
	virtual bool	fillValueWithDocid() = 0;
	virtual void 	resetByValue() = 0;
	virtual bool    serializeToXml(AosXmlTagPtr &xml) = 0;
	virtual bool 	isEmpty() const = 0;
	virtual void 	reset() = 0;
	virtual bool	isDataReverse() const = 0;
	virtual bool 	moveTo(const i64 &pos, bool &finished, const AosRundataPtr &rdata) = 0;
	virtual i64		getNumDocs() = 0;
	virtual i64		getNumLeft() = 0;
	virtual bool 	nextU64DocidValue(u64 &docid, u64 &value, bool &finished, const AosRundataPtr &rdata) = 0;
	virtual OmnString	getLastValue()const = 0;
	virtual bool		getEntriesBeforeValue(
							const AosBuffArrayPtr &buff_array,
							const OmnString last_value,
							const bool &reverse) = 0;
	virtual AosBuffArrayPtr getValueBuff() const = 0;

	virtual bool 	mergeFrom(const AosQueryRsltObjPtr &rslt) = 0;
	virtual bool	mergeFrom(const AosQueryRsltObjPtr &rhs_rslt,const u64 &startidx, const u64 &len) = 0;
	virtual bool 	merge(const AosQueryRsltObjPtr &rslt, const bool mergeAll) = 0;
	virtual bool 	appendDocid(const u64 docid) = 0;
	virtual void 	setNumDocs(const i64 &numDocs) = 0;
	virtual i64 	getTotal(const AosRundataPtr &rdata) const = 0;
	virtual bool 	checkDocid(const u64 &docid, const AosRundataPtr &rdata) = 0;
	virtual u64* 	getDocidsPtr() const = 0;
	virtual bool 	nextDocidValue(
						u64 &docid, 
						OmnString &value, 
						const AosRundataPtr &rdata) = 0;
	virtual bool 	nextDocidValue(
						u64 &docid, 
						OmnString &value, 
						bool &finished, 
						const AosRundataPtr &rdata) = 0;
	virtual bool 	nextDocidValue(
						u64 &docid, 
						u64 &value, 
						bool &finished, 
						const AosRundataPtr &rdata) = 0;

	virtual bool 	nextDocidValue(
						u64 &docid, 
						d64 &value, 
						bool &finished, 
						const AosRundataPtr &rdata) = 0;

	virtual bool 	nextDocidValue(
						u64 &docid, 
						i64 &value, 
						bool &finished, 
						const AosRundataPtr &rdata) = 0;

	virtual bool 	exchangeContent(
						const AosQueryRsltObjPtr &rslt1,
						const AosQueryRsltObjPtr &rslt2) = 0;
	static bool 	exchangeContentStatic(
						const AosQueryRsltObjPtr &rslt1,
						const AosQueryRsltObjPtr &rslt2);
	// felicia, 2013/03/14
	virtual void setOperator(const AosDataColOpr::E opr) = 0;
	virtual AosDataColOpr::E getOperator() const = 0;
	virtual bool removeOldData(const i64 &pos) = 0;
	virtual u64 removeOldData() = 0;

	virtual void toString(const AosRundataPtr &rdata) = 0;
	virtual bool appendEntry(const u64 docid, const OmnString &value) = 0;
	virtual bool checkAllNum() = 0;// only for tester
};
#endif



