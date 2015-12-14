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
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_Jimos_Trunc_h
#define Aos_Funcs_Jimos_Trunc_h

#include "Funcs/GenFunc.h"


#define AOSTRUNC_DATE_FMT_SYYYY 				"SYYYY"
#define AOSTRUNC_DATE_FMT_YYYY 					"YYYY"
#define AOSTRUNC_DATE_FMT_YEAR					"YEAR"
#define AOSTRUNC_DATE_FMT_SYEAR					"SYEAR"
#define AOSTRUNC_DATE_FMT_YYY					"YYY"
#define AOSTRUNC_DATE_FMT_YY					"YY"
#define AOSTRUNC_DATE_FMT_Y						"Y"
#define AOSTRUNC_DATE_FMT_IYYY					"IYYY"
#define AOSTRUNC_DATE_FMT_IY					"IY"
#define AOSTRUNC_DATE_FMT_I						"I"
#define AOSTRUNC_DATE_FMT_Q						"Q"
#define AOSTRUNC_DATE_FMT_MONTH					"MONTH"
#define AOSTRUNC_DATE_FMT_MON					"MON"
#define AOSTRUNC_DATE_FMT_MM					"MM"
#define AOSTRUNC_DATE_FMT_RM					"RM"
#define AOSTRUNC_DATE_FMT_WW					"WW"
#define AOSTRUNC_DATE_FMT_IW					"IW"
#define AOSTRUNC_DATE_FMT_W						"W"
#define AOSTRUNC_DATE_FMT_DDD					"DDD"
#define AOSTRUNC_DATE_FMT_DD					"DD"
#define AOSTRUNC_DATE_FMT_J						"J"
#define AOSTRUNC_DATE_FMT_DAY					"DAY"
#define AOSTRUNC_DATE_FMT_DY					"DY"
#define AOSTRUNC_DATE_FMT_D						"D"
#define AOSTRUNC_DATE_FMT_HH					"HH"
#define AOSTRUNC_DATE_FMT_HH12					"HH12"
#define AOSTRUNC_DATE_FMT_HH24					"HH24"
#define AOSTRUNC_DATE_FMT_MI					"MI"


class AosTrunc : public AosGenFunc
{
	OmnDefineRCObject;

private:
	enum FMT
	{
		eInvalid,

		// Year 
		eSYYYY,
		eYYYY, 
		eYEAR,
		eSYEAR, 
		eYYY,
		eYY,
		eY,

		// ISO Year
		eIYYY, 
		eIY, 
		eI, 

		// Quarter
		eQ,

		// Month
		eMONTH, 
		eMON, 
		eMM, 
		eRM, 
		
		// Week
		eWW, 

		// IW
		eIW, 

		// W
		eW,

		// Day
		eDDD, 
		eDD, 
		eJ, 

		// Start day of the week
		eDAY, 
		eDY, 
		eD, 

		// Hour
		eHH, 
		eHH12, 
		eHH24, 

		// Minute
		eMI,

		eMax
	};
	AosTrunc::FMT mFormat;

public:
	AosTrunc(const int version);
	AosTrunc();
	AosTrunc(const AosTrunc &rhs);
	~AosTrunc();

	AosTrunc::FMT toEnum(const string &fmt);
	
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
	bool			getDateValue(AosValueRslt &value);

	//bool 			getValue(AosRundata *rdata);

};
#endif

