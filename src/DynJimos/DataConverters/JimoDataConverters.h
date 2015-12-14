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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DynJimos_DataConverters_JimoDataConverters_h
#define Aos_DynJimos_DataConverters_JimoDataConverters_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <ctime>


class AosJimoDataConverters : public AosJimo
{
public:
	
	OmnString 		mFormat;
	OmnString 		mStartTime;
	OmnString 		mOutPutFormat;
	OmnString 		mSep;
	bool			mWholeYear;
	u64				mEpochTime;
	u32 			mSepLen;
	u32				mYear;
	u32				mMonth;
	u32				mDay;
	AosRundataPtr	mRundata;


public:
	AosJimoDataConverters();
	AosJimoDataConverters(const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);
	~AosJimoDataConverters();
	
	AosJimoDataConverters(const AosJimoDataConverters &rhs);


	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);

	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);

	virtual AosJimoPtr clone(const AosRundataPtr &rdata) const;
	virtual OmnString toString() const;

	virtual void * getMethod(const AosRundataPtr &rdata, 
							const OmnString &name, 
							AosMethodId::E &method_id);


	virtual bool config( 	const AosRundataPtr &rdata,
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc);
	int getEpochYear();
	int getEpochMonth();
	int getEpochDay();
	int getEpochHour();
	int getEpochTime();
	OmnString getStartTime();


private:
	bool init(				const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc,
							const AosXmlTagPtr &jimo_doc);

	bool registerMethods();
	u32 getMonthIntInArray(	const OmnString &month,
							OmnString *omnStrArr,
							const u32 arrLen);
	bool matchFormat();
	OmnString getYearOfYY(const int year);
	OmnString matchOutPutFormat();
	bool setU64YMD();

	bool setYYMD();
	bool setYMD();
	bool setMDYY();
	bool setMDY();
	bool setUSMDY(const int stact);
	bool setCYMD(const int stact);

	OmnString getYYMD();
	OmnString getYMD();
	OmnString getMDYY();
	OmnString getMDY();
	OmnString getUSMDY(const int stact);
	OmnString getCYMD(const int stact);

	bool getLocalYMD(u32 &year, u32 &month, u32 &day); 
	double diffTime(const int year, const int month, const int day);

	};
#endif
