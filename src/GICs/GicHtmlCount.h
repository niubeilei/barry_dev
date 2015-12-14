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
//
// Modification History:
// 07/26/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlCount_h
#define Aos_GICs_GicHtmlCount_h
			
#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

#define AOSTAG_COUNTDOWN_COLOR     	"count_color"
#define AOSTAG_COUNTDOWN_SIZE     	"count_fontsize"
#define AOSTAG_COUNTDOWN_STYLE     	"count_style"
#define AOSTAG_COUNT_WAY           	"count_way"
#define AOSTAG_TIMEPOINT           	"timepoint"
#define AOSTAG_INTERVAL            	"count_interval"
#define AOSTAG_COUNTSHOWFORMAT     	"count_show_format"
#define AOSTAG_COUNTTIMEFORMAT     	"count_time_format"
#define AOSTAG_DATABIND         	"ct_bind"
#define AOSTAG_COUNTTIMEFORMATDB  	"ctfmt_bind"


class AosGicHtmlCount : public AosGic
{
	OmnDefineRCObject ;

private:
	OmnString			mCountColor;
	OmnString			mCountFontSize;
	OmnString			mCountStyle;
	OmnString			mCountWay;
	OmnString			mCountDB;
	OmnString			mCountTimeFormatDB;
	OmnString			mTimePoint;
	OmnString			mCountShowFormat;
	OmnString			mCountTimeFormat;
	int					mInterval;

public:
	AosGicHtmlCount(const bool flag);
	~AosGicHtmlCount();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
    bool	createJsonCode(AosHtmlCode &code);
    bool	createHtmlCode(AosHtmlCode &code);
    bool	init(const AosXmlTagPtr &vpd);
	OmnString setStyle(const OmnString &style);
};

#endif

