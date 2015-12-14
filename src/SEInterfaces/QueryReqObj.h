////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/03/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryReqObj_h
#define Aos_SEInterfaces_QueryReqObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RecordsetObj.h"
//#include "StatQueryAnalyzer/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include "Util/Buff.h"
#include <vector>
class AosQueryReqObj : virtual public OmnRCObject
{
	static AosQueryReqObjPtr smQueryReqObj;
	static AosQueryReqObjPtr smQueryReqObjNew;

public:
	static void setObject(const AosQueryReqObjPtr &q) {smQueryReqObj = q;}
	static AosQueryReqObjPtr getObject() {return smQueryReqObj;}

	static void setObjectNew(const AosQueryReqObjPtr &q) {smQueryReqObjNew = q;}
	static AosQueryReqObjPtr getObjectNew() {return smQueryReqObjNew;}

	static AosQueryReqObjPtr createQueryStatic(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata);

	static AosQueryReqObjPtr createQueryStaticNew(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata);
	
	virtual AosQueryReqObjPtr createQuery(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata) = 0;

	virtual void	setQueryId(const u64 &id) = 0;
	virtual u64		getQueryId() const = 0;
	virtual bool	isGood() const = 0;
	virtual void	setStartIdx(const i64 &s) = 0;

	virtual bool	procPublic(
						const AosXmlTagPtr &cmd,
						OmnString &contents,
						const AosRundataPtr &rdata) = 0;
	virtual AosQueryTermObjPtr	getQueryTerm() const = 0;
	virtual void	setQueryData(const AosQueryRsltObjPtr &query_rslt) = 0;
	virtual void	setQueryContext(const AosQueryContextObjPtr &query_context) = 0;
	virtual i64		getTotal() = 0;
	virtual bool	finished()const = 0;
	virtual OmnString toString() const = 0;
	virtual AosXmlTagPtr getOrigReq() const = 0;
	virtual vector<AosBuffPtr>	getBatchData() = 0;
	virtual int	getBatchNum() = 0;
	virtual bool setNoMoreDocs() = 0;
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fieldname, 
						int &num_matched, 
						int *start_days, 
						int *end_days) = 0;
	/*
	virtual bool isFirstBlock() const = 0;
	virtual void setOrigStartPos(const AosIILIdx &idx) = 0;
	virtual void setEndPos(const AosIILIdx &idx) = 0;
	virtual bool isEmpty() const = 0;
	virtual AosIILIdx getStartPos() const = 0;
	virtual AosIILIdx getEndPos() const = 0;
	virtual bool setPartialBitmap(const AosBitmapObjPtr &bitmap) = 0;
	virtual bool appendNode(const AosRundataPtr &rdata, 
							const i64 &level, 
							const u64 &node_id) = 0;
	virtual bool addNumEntries(
							const AosRundataPtr &rdata, 
							const AosIILIdx &start_pos, 
							const AosIILIdx &end_pos) = 0;
	virtual bool addNumEntries(
							const AosRundataPtr &rdata, 
							const AosIILIdx &start_pos) = 0;
	virtual bool addNumEntries(const i64 &num) = 0;
	virtual bool blockFinished() = 0;
	virtual bool isReverse() const = 0;
	virtual bool isFinished() const = 0;
	virtual bool nextSubiil(const AosRundataPtr &rdata, const i64 &level) = 0;

	virtual u64					getQueryStartTime() const = 0;
	virtual AosRundataPtr		getRundata() const = 0;
	*/
	virtual i64 getPageSize() const = 0;
	virtual i64 getStartIdx() const = 0;
	// Chen Ding, 2013/08/18
	virtual bool	runQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		OmnString &contents) = 0;

	// Chen Ding, 2013/08/18
	virtual bool	runQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		OmnString &contents, 
		const AosQueryProcCallbackPtr &callback) = 0;
	// Chen Ding, 2013/08/18
	virtual bool generateResults(
						const AosRundataPtr &rdata, 
						const AosQueryRsltObjPtr &query_rslt,
						OmnString &records) = 0;
	// Chen Ding, 2013/09/12
	virtual bool setQueryProc(const AosQueryProcObjPtr &query_proc) = 0;
	virtual AosQueryProcObjPtr getQueryProc() const = 0;
	

	// Ketty 2014/02/13
	//virtual AosQueryTermObjPtr getOrTerm() = 0;;
	//virtual vector<OmnString> & getTables() = 0;
	//virtual vector<AosQrUtil::FieldDef> & getSelectedFields() = 0;
	//virtual vector<OmnString> & getGroupByFields() = 0;
	//virtual void setStatQueryUnit(const AosStatQueryUnitPtr &) = 0;
	//virtual AosStatQueryUnitPtr getStatQueryUnit() = 0;

	virtual bool	runQuery(const AosRundataPtr &rdata) = 0;

	virtual bool	generateRslt(
						AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata) = 0;

	virtual bool	getRsltInfor(
						AosXmlTagPtr &infor,
						const AosRundataPtr &rdata) = 0;
};

#endif
