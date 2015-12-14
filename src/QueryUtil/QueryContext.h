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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_QueryContext_h
#define Aos_QueryUtil_QueryContext_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "SEUtil/IILIdx.h"
#include "QueryCond/Ptrs.h"
#include "QueryUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEBase/SeUtil.h"
#include "Util/Buff.h"
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"



class AosQueryContext : public AosQueryContextObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eInitMemSize = 10,
		eSmallDocNum = 300,
		eLargeDocNum = 10000,
		
		eDefaultBlockSize = 30,
		eBlockSize = 500000,
		eBuffInitSize = 10000,
		eBuffIncSize = 100000
	};

	struct AosMultiCond
	{
		AosOpr			opr;
		OmnString		strValue;	
		u64				u64Value;
		OmnString		strValue2;	
		u64				u64Value2;
		i64				index;
		i64				iilIndex;
		bool			finished;	
		AosIILIdx       iilIdx2;

		AosMultiCond(
				const AosOpr &opr1,
				const OmnString &strvalue,
				const u64 &u64value,
				const OmnString &strvalue2,
				const u64 &u64value2)
		:
		opr(opr1),
		strValue(strvalue),
		u64Value(u64value),
		strValue2(strvalue2),
		u64Value2(u64value2),
		index(-10),
		iilIndex(-10),
		finished(false)
		{
		}

		AosMultiCond(const AosXmlTagPtr &xml)
		{
			opr = AosOpr_toEnum(xml->getAttrStr("opr", "eq"));
			strValue = xml->getAttrStr("strValue");
			if (strValue == "")
			{
				strValue = xml->getNodeText("strValue");
			}
			u64Value = xml->getAttrU64("u64Value", 0);
			strValue2 = xml->getAttrStr("strValue2");
			if (strValue2 == "")
			{
				strValue2 = xml->getNodeText("strValue2");
			}
			u64Value2 = xml->getAttrU64("u64Value2", 0);
			index = xml->getAttrInt64("index", -10);
			iilIndex = xml->getAttrInt64("iilidx", -10);
			finished = xml->getAttrBool("finished", false);
			iilIdx2.setValue(xml->getAttrStr("iilIdx2"));
		}

		~AosMultiCond(){}

		OmnString toString()
		{
			OmnString str;
			str << "<cond opr=\"" << AosOpr_toStr(opr)<< "\" ";	
			if(u64Value) str << "u64Value=\"" << u64Value << "\" ";
			if(u64Value2) str << "u64Value2=\"" << u64Value2 << "\" ";
			if(index) str << "index=\"" << index << "\" ";
			if(iilIndex) str << "iilidx=\"" << iilIndex << "\" ";
			if(finished) str << "finished=\"" << finished << "\" ";
			if(iilIdx2.toString() != "") str << "iilIdx2=\"" << iilIdx2.toString() << "\" ";
			str << ">";
			if (strValue != "")
			{
				str << "<strValue>" << strValue << "</strValue>";
			}

			if (strValue2 != "")
			{
				str <<"<strValue2>" << strValue2 << "</strValue2>";
			}

			str << "</cond>";
			return str;
		}
	};

private:
	// for grouping
	AosIILIdx						mIILIdx2;
	vector<AosQueryGroupFilterPtr>	mGroupFilters;

	//Linda, 2013/02/22
	AosFieldFilterPtr           	mFieldFilter;
	vector<AosMultiCond>        	mMultiCond;

	// Ken Lee, 2014/08/19
	vector<AosQueryFilterObjPtr>	mFilters;

public:
	AosQueryContext();
	~AosQueryContext();

	virtual i64 getBlockSize() 
	{
//		if (mBlockSize <= 0) mBlockSize = eDefaultBlockSize;
		return mBlockSize;
	}

	virtual AosQueryContextObjPtr createQueryContext();
	virtual void returnQueryContext(const AosQueryContextObjPtr &context);
	virtual AosQueryContextObjPtr clone();

	static bool initQueryContext();

	virtual void 	clear();
	virtual bool	serializeToXml(AosXmlTagPtr &xml, const AosRundataPtr &rdata);
	virtual bool	serializeFromXml(const AosXmlTagPtr &xml, const AosRundataPtr &rdata);
	virtual void	setSaperator(const OmnString &saperator);
	virtual void 	setIILIndex2(const AosIILIdx &iilidx) {mIILIdx2 = iilidx;}
	virtual void 	setIILIndex2(const int level, const i64 &idx) {mIILIdx2.setIdx(level, idx);}
	virtual void	resetIILIndex2() {mIILIdx2.reset();}
	virtual AosIILIdx	getIILIndex2()const{return mIILIdx2;}

	virtual bool	copyFrom(const AosQueryRsltObjPtr &query_rslt);
	virtual bool	copyTo(const AosQueryRsltObjPtr &query_rslt);

	virtual void	addGroupFilter(
						const GroupFilterType type,
						const AosOpr opr, 
						const OmnString &value1,
						const OmnString &value2,
						const bool createDefault); 
	virtual void	addGroupFilter(
						const GroupFilterType type,
						const AosOpr opr, 
						const u64 &value1,
						const u64 &value2,
						const bool createDefault); 
	virtual void	removeAllGroupFilter();

	// Grouping functions
	virtual void	appendGroupEntryStrStart(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryStrFinish(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryU64Start(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryU64Finish(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryI64Start(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryI64Finish(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryD64Start(const AosQueryRsltObjPtr &queryRslt);
	virtual void	appendGroupEntryD64Finish(const AosQueryRsltObjPtr &queryRslt);
	virtual bool 	isFull(const u64 &num) const;

	QueryKeyGroupingType	getKeyGroupingType()const{return mKeyGroupingType;}
	QueryValueGroupingType	getValueGroupingType()const{return mValueGroupingType;}

	virtual void	appendGroupEntry(
						const u64 &key, 
						const u64 &value, 
						const AosIILIdx &curIdx,
						const AosQueryRsltObjPtr &queryRslt);

	virtual void	appendGroupEntry(
						const OmnString &key, 
						const u64 &value, 
						const AosIILIdx &curIdx,
						const AosQueryRsltObjPtr &queryRslt, 
						const bool num_alpha);

	virtual void	setFieldFilterObj(const AosFieldFilterPtr &b);
	virtual bool	isFieldFilterValid(){if (!mFieldFilter) return false; return true;}
	virtual bool	filterFieldCond(const OmnString &key);

	virtual u32 	getNumMultiCond(){return mMultiCond.size();}
	virtual bool	retrieveMultiCond(const u32 &idx);
	virtual bool	updateMultiCond(const u32 &idx);

	virtual void	addMultiCond(
						const AosOpr &opr, 
						const OmnString &strvalue,
						const u64 &u64value,
						const OmnString &strvalue2,
						const u64 &u64value2);

	// Ken Lee, 2014/08/19
	virtual bool	hasFilter() {return !mFilters.empty();}
	virtual void	addFilter(const AosQueryFilterObjPtr &filter) {mFilters.push_back(filter);}
	virtual void	clearFilter() {mFilters.clear();}
	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const;
	virtual bool	evalFilter(
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata) const;

private:
	OmnString		getCmpKey(const OmnString &key);
	void			procGroupStr(const AosQueryRsltObjPtr &queryRslt);
	void			procGroupU64(const AosQueryRsltObjPtr &queryRslt);
	bool			initMemberData();

public:
	// Chen Ding, 2013/02/15
	virtual bool	serializeToStr(OmnString &str, const AosRundataPtr &rdata);

};

#endif

