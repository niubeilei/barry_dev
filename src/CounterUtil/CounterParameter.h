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
// This is a utility class.  
//
// Modification History:
// 02/28/2013: Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_CounterParameter_h
#define Aos_CounterUtil_CounterParameter_h

#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "Util/BuffArray.h"

struct AosCounterParameter :virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxCnameLen = 301
	};

private:
	AosRecordFormatPtr				record_format;

public:
	vector<OmnString> 				counter_ids;	
	vector<AosStatType::E>  		stat_types;
	vector<OmnString>       		cnames;
	bool							use_iilname;
	OmnString						cname_format;
	AosCounterQueryPtr				counter_query;

	//order by value(docid)
	bool                            order_by_value;
	OmnString                       sort_counter_id;
	bool                            reverse;
	
	bool                            need_convert_int64;

public:
	AosCounterParameter()
	:
	record_format(0),
	use_iilname(false),
	cname_format(""),
	counter_query(0),
	order_by_value(false),
	sort_counter_id(""),
	reverse(false),
	need_convert_int64(false)
	{
	}
	~AosCounterParameter(){}


	AosBuffArrayPtr	getBuffArray(const AosRundataPtr &rdata)
	{
		int bufflen = eMaxCnameLen + sizeof(u64);
		OmnString config = "<zky_buffarray";
		config << " zky_withdocid=\"true\" "
			<< " zky_stable = \"false\" >"
			<< "<zky_cmparefunc  "
			<< " cmpfun_type=\"custom\" "
			<< " cmpfun_size=\"" << bufflen << "\" "
			<< " cmpfun_alphabetic=\"true\" "
			<< " cmp_pos1 = \"" << eMaxCnameLen-1 << "\" "
			<< " cmp_datatype1 = \"u64\"  "
			<< " cmp_size1 = \"" << sizeof(u64) << "\" "
			<< " cmpfun_reserve=\"" << reverse <<"\" "
			<< "/> </zky_buffarray>";

		AosXmlParser parser;       
		AosXmlTagPtr array_tag = parser.parse(config, "" AosMemoryCheckerArgs);
		aos_assert_r(array_tag, 0);
		AosBuffArrayPtr buff_array = OmnNew AosBuffArray(array_tag, rdata.getPtr());
		aos_assert_r(buff_array, 0);
		return buff_array;
	}

	AosRecordFormatPtr getRecordFormat(){return record_format;}
	void setRecordFormat(const AosRecordFormatPtr &rf){record_format = rf;}

};
#endif

