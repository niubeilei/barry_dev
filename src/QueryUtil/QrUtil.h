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
// 01/02/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryUtil_QrUtil_h
#define Aos_QueryUtil_QrUtil_h

#include "Query/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "SEInterfaces/AggregationType.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::local_time;

using namespace std;

class AosQrUtil
{
public:
	enum
	{
		eMaxTags = 20,
		eMaxWords = 20,
		eMaxAssign = 50,
		eMaxDocs = 1000,
		eMaxFields = 50,
		eMaxCondWords = 500,
		eMaxOrderNames = 30,
		eMaxClause = 30,
		eMaxCtnrs = 5,
		eMaxJoins = 5,
		eDftPsize = 20,
		AOS_ATTR_FLAG = '1',
		AOS_CDATA_FLAG = '2',
		AOS_TEXT_FLAG = '3'
	};

	enum SqlType
	{
		eInvalidQuery,
		eSelect,
		eUpdate,
		eDelete
	};

	enum FieldType
	{
		eAttr,
		eAttrAdd,
		eAttrSubtag,
		eText,
		eSubtag,
		eCdata,
		eFunc,
		eXml,
		eExpr,
		eEpoch2TimeStr,
		eEpoch2LocalTime,
		eEpochDay2TimeStr,
		eDayOfWeekToStr,
		eEpochHour2TimeStr
	};

	struct FieldDef
	{
		OmnString   oname;
		OmnString   cname;
		FieldType	type;
		bool		exist;
		int			join_idx;
		OmnString   sdoc_objid;
		AosXmlTagPtr sdoc;
		AosAggregationType::E mAggregation;	// Ketty 2014/04/09	
		int			mStatFieldIdx;			// Ketty 2014/04/09

		AosDataRecordObjPtr  		mRcdPtr;
		AosExprObjPtr 				mExprPtr;


		bool serializeToBuff(const AosBuffPtr &buff)
		{
			buff->setOmnStr(oname);
			buff->setOmnStr(cname);
			buff->setU8(type);
			buff->setU8(exist);
			buff->setInt(join_idx);
			buff->setOmnStr(sdoc_objid);
			OmnString content;
			if(sdoc)
			{
				content = sdoc->toString();
				int len = content.length();
				aos_assert_r(len >0, false);
				buff->setInt(len);
				buff->setOmnStr(content);
			}
			else
			{
				int len = 0;
				buff->setInt(len);
			}
			return true;
		}
		bool serializeFromBuff(const AosBuffPtr &buff)
		{
			oname = buff->getOmnStr("");
			cname = buff->getOmnStr("");
			type = (FieldType)buff->getU8(0);
			exist = buff->getU8(0);
			join_idx = buff->getInt(0);
			sdoc_objid = buff->getOmnStr("");
			int len = buff->getInt(0);
			if(len > 0 )
			{
				OmnString content = buff->getOmnStr("");
				if(content != "")
				{
					AosXmlParser parser;
					sdoc = parser.parse(content, "" AosMemoryCheckerArgs);
				}
			}
			return true;
		}

		// Ketty 2014/04/09
		bool isStatValueField(){ return AosAggregationType::isValid(mAggregation); }
		bool isTimeField()
		{ 
			// Ketty 2014/04/09
			return type == eEpoch2TimeStr ||
				type == eEpoch2LocalTime ||
				type == eEpochDay2TimeStr ||
				type == eDayOfWeekToStr ||
				type == eEpochHour2TimeStr; }
	};

	enum JoinType
	{
		eInvalidJoin, 

		eDocidJoin,
		eObjidJoin,
		eCloudidJoin,

		eMaxJoin
	};

	struct JoinDef
	{
		OmnString	name;
		JoinType	type;
		int			index;
	};

public:
	// Chen Ding, 2013/07/27
	// static bool parseFnames(
	// 				const AosXmlTagPtr &fnames, 
	// 				vector<FieldDef> &fielddef, 
	// 				AosXmlRc &errcode,
	// 				OmnString &errmsg);
	static bool parseFnames(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &fnames, 
					vector<FieldDef> &fielddef); 

	static bool parseFnames(
					const AosRundataPtr &rdata,
	 				const OmnString &fnames,
	 				vector<FieldDef> &fields, 
	 				const OmnString &entry_sep, 
	 				const OmnString &attr_sep); 

	// Chen Ding, 2013/07/27
	// static bool createRecord(
	// 				OmnString &records, 
	// 				const AosXmlTagPtr &xml,
	// 				OmnString &errmsg,
	// 				vector<FieldDef> &fields, 
	// 				const AosRundataPtr &rdata);
	static bool createRecord(
					const AosRundataPtr &rdata,
					OmnString &records, 
					const AosXmlTagPtr &xml,
					vector<FieldDef> &fields); 
	static bool createRecords(
					const AosRundataPtr &rdata,
					map<OmnString, AosBuffPtr> &columns,
					vector<FieldDef> &fields,
					vector<AosBuffPtr> &results,
					const AosAsyncRespCallerPtr &resp_caller,
					int	&num_async_calls);

	// Chen Ding, 2013/07/27
	// static bool createRecord(
	// 				OmnString &records, 
	// 				vector<AosXmlTagPtr> &xmls,
	// 				OmnString &errmsg,
	// 				vector<FieldDef> &fields, 
	// 				const AosRundataPtr &rdata);
	static bool createRecord(
					const AosRundataPtr &rdata,
					OmnString &records, 
					vector<AosXmlTagPtr> &xmls,
					vector<FieldDef> &fields); 

	// Chen Ding, 2013/07/27
	// static bool	createRecord(
	// 				OmnString &records, 
	// 				vector<AosXmlTagPtr> &xmls,
	// 				OmnString &errmsg,
	// 				vector<FieldDef> &fields, 
	// 				const AosRundataPtr &rdata);
	static bool	createRecordByXml(
					const AosRundataPtr &rdata,
					OmnString &records, 
					vector<AosXmlTagPtr> &xmls,
					vector<FieldDef> &fields); 

	static bool procOneField(
					const AosXmlTagPtr &xml,
					const FieldDef &field, 
					OmnString &record,
					OmnString &subtags, 
					const AosRundataPtr &rdata);

	static bool	procOneField(
					const AosXmlTagPtr &record,
					const AosXmlTagPtr &xml,
					const FieldDef &field, 
					const AosRundataPtr &rdata);

	static bool procOneField(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &xml,
					const FieldDef &field, 
					OmnString &value);

	static bool addFieldFlag(
					OmnString &record,
					vector<FieldDef> &fields, 
					vector<AosXmlTagPtr> &xmls,
					const AosRundataPtr &rdata);

	static bool	addFieldFlag(
					const AosXmlTagPtr &record,
					vector<FieldDef> &fields, 
					vector<AosXmlTagPtr> &xmls,
					const AosRundataPtr &rdata);

	static bool procOneColumn(
					const AosRundataPtr &rdata,
					int	&field_idx,
					const FieldDef &field, 
					const AosBuffPtr &column,
					vector<AosBuffPtr> &results,
					const AosAsyncRespCallerPtr &resp_caller,
					int &num_async_calls);

	static bool isValidJoin(const JoinType type)
	{
		return type > eInvalidJoin && type < eMaxJoin;
	}

	// Chen Ding, 09/22/2011
	static void procQueryVars(
			OmnString &value, 
			const OmnString &dftproc, 
			const OmnString &dftvalue, 
			bool &ignore,
			const AosRundataPtr &rdata);

	static bool calculateTimeToEpoch(
			const int number,
			const OmnString &timetype,
			i64 &crt_epoch,
			i64 &cal_epoch,
			const AosRundataPtr &rdata);

private:
};

struct AosCondInfo
{
	u64		mMin;
	u64		mMax;
	u32		mDeviceId;

	void reset() 
	{
		mMin = 0;
		mMax = 0;
		mDeviceId = 0;
	}

	void mergeOr(const AosCondInfo &cond_info)
	{
		if (mMin > cond_info.mMin) mMin = cond_info.mMin;
		if (mMax < cond_info.mMax) mMax = cond_info.mMax;
	}
};

#endif


