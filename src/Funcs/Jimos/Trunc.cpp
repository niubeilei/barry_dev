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
#include "Funcs/Jimos/Trunc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include "boost/date_time/gregorian/gregorian.hpp"  
#include <boost/exception/all.hpp>

#include <algorithm>
using namespace boost::gregorian;    


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTrunc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosTrunc(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosTrunc::AosTrunc(const int version)
:
AosGenFunc("Trunc", version),
mFormat(AosTrunc::eInvalid)
{
}

AosTrunc::AosTrunc()
:
AosGenFunc("Trunc", 1),
mFormat(AosTrunc::eInvalid)
{
}



AosTrunc::AosTrunc(const AosTrunc &rhs)
:
AosGenFunc(rhs)
{
	mFormat = rhs.mFormat;
}


AosTrunc::~AosTrunc()
{
}


bool
AosTrunc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosTrunc::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosTrunc::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if (mSize == 2)
	{
		//getValue(rdata);
		if (mValue.getType() == AosDataType::eDateTime)
		{
			return getDateValue(value);
		}
		else if (mValue.getType() == AosDataType::eString)
		{
			if (mValue.getStr() == "sysdate")
			{
				time_t tt = time(NULL);
				AosDateTime dt(tt, "");
				mValue.setDateTime(dt);
				return getDateValue(value);
			}
			else
			{
				try
				{
					AosDateTime dt(mValue.getStr(), "");
					mValue.setDateTime(dt);
					return getDateValue(value);
				}
				catch(boost::exception &e)
				{
					OmnScreen << diagnostic_information(e) << endl;
					rdata->setJqlMsg("Error: incorrect date format.");
					return false;
				}
			}
		}
		else
		{
			//OmnNotImplementedYet;
			//return false;
			return true;
		}
		return false;
	}
	else
	{
		value = mValue;
		return true;
	}
}


bool 
AosTrunc::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize < 1)
	{
		errmsg << "Substr needs at least 1 parameters";
		return false;
	}
	else if (mSize > 2)
	{
		errmsg << "Substr needs at most 2 parameters"; 
		return false;
	}

	if(mSize == 2)
	{
		if (!AosGenFunc::getValue(rdata.getPtr(), 1, 0)) return false;
		mFormat = toEnum(mValue.getStr());
	}

	return true;
}


AosJimoPtr
AosTrunc::cloneJimo()  const
{
	try
	{
		return OmnNew AosTrunc(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


AosTrunc::FMT 
AosTrunc::toEnum(const string &name)
{
	if (name.length() < 1) return eInvalid;
	switch (name.data()[0])
	{
	case 'D':
		if (name == AOSTRUNC_DATE_FMT_D) return eD;
		if (name == AOSTRUNC_DATE_FMT_DAY) return eDAY;
		if (name == AOSTRUNC_DATE_FMT_DD) return eDD;
		if (name == AOSTRUNC_DATE_FMT_DDD) return eDDD;
		if (name == AOSTRUNC_DATE_FMT_DY) return eDY;
	case 'H':
		if (name == AOSTRUNC_DATE_FMT_HH) return eHH;
		if (name == AOSTRUNC_DATE_FMT_HH12) return eHH12;
		if (name == AOSTRUNC_DATE_FMT_HH24) return eHH24;
	case 'I':
		if (name == AOSTRUNC_DATE_FMT_I) return eI;
		if (name == AOSTRUNC_DATE_FMT_IW) return eIW;
		if (name == AOSTRUNC_DATE_FMT_IY) return eIY;
		if (name == AOSTRUNC_DATE_FMT_IYYY) return eIYYY;
	case 'J':
		if (name == AOSTRUNC_DATE_FMT_J) return eJ;
	case 'M':	
		if (name == AOSTRUNC_DATE_FMT_MI) return eMI;
		if (name == AOSTRUNC_DATE_FMT_MM) return eMM;
		if (name == AOSTRUNC_DATE_FMT_MON) return eMON;
		if (name == AOSTRUNC_DATE_FMT_MONTH) return eMONTH;
	case 'Q':
		if (name == AOSTRUNC_DATE_FMT_Q) return eQ;
	case 'R':
		if (name == AOSTRUNC_DATE_FMT_RM) return eRM; 
	case 'S':
		if (name == AOSTRUNC_DATE_FMT_SYEAR) return eSYEAR; 
		if (name == AOSTRUNC_DATE_FMT_SYYYY) return eSYYYY;
	case 'W':
		if (name == AOSTRUNC_DATE_FMT_W) return eW;
		if (name == AOSTRUNC_DATE_FMT_WW) return eWW;
	case 'Y':
		if (name == AOSTRUNC_DATE_FMT_Y) return eY;
		if (name == AOSTRUNC_DATE_FMT_YEAR) return eYEAR;
		if (name == AOSTRUNC_DATE_FMT_YY) return eYY;
		if (name == AOSTRUNC_DATE_FMT_YYY) return eYYY;
		if (name == AOSTRUNC_DATE_FMT_YYYY) return eYYYY;
	default:
		return eInvalid;
	}
	return AosTrunc::eInvalid;
}


//bool
//AosTrunc::getValue(AosRundata *rdata)
//{
//	if (mValue.getType() == AosDataType::eDateTime)
//	{
//		return getDateValue(rdata);
//	}
//	else if (mValue.getType() == AosDataType::eString)
//	{
//		if (mValue.getStr() == "sysdate")
//		{
//			time_t tt = time(NULL);
//			AosDateTime dt(tt, "");
//			mValue.setDateTime(dt);
//			return getDateValue(rdata);
//		}
//		else
//		{
//			try
//			{
//				AosDateTime dt(mValue.getStr(), "");
//				mValue.setDateTime(dt);
//				return getDateValue(rdata);
//			}
//			catch(boost::exception &e)
//			{
//				OmnScreen << diagnostic_information(e) << endl;
//				rdata->setJqlMsg("Error: incorrect date format.");
//				return false;
//			}
//		}
//	}
//	else
//	{
//		OmnNotImplementedYet;
//		return false;
//	}
//	return false;
//}


bool
AosTrunc::getDateValue(AosValueRslt &value)
{
	// We assume 'value' is DateTime.
	AosDateTime dt = mValue.getDateTime();
	ptime& pt = dt.getPtime();
	switch (mFormat)
	{
		// Day
		case eDD:
		case eDDD:
		case eJ:
		// Start day of the week
		case eDAY:
		case eDY:
		case eD:
		{
			pt = ptime(pt.date());
			break;
		}

		// Quarter
		case eMONTH:
		case eQ:
		// Month
		case eMON:
		case eMM:
		case eRM:
		{
			greg_year year = pt.date().year();
			greg_month month = pt.date().month();
			date d(year, month, 1);
			pt = ptime(d);
			break;
		}

		// Hour
		case eHH24:
		case eHH:
		case eHH12:
		{
			pt = ptime(pt.date(), hours(to_tm(pt).tm_hour));
			break;
		}

		// Year
		case eSYYYY:
		case eYYYY:
		case eYEAR:
		case eSYEAR:
		case eYYY:
		case eYY:
		case eY:
		// ISO Year
		case eIYYY:
		case eIY:
		case eI:
		{
			greg_year year = pt.date().year();
			date d(year, 1, 1);
			pt = ptime(d);
			break;
		}

		// Week
		case eWW:
		// IW
		case eIW:
		// W
		case eW:
			OmnNotImplementedYet;
			return false;
		
		// Minute
		case eMI:
		{
			tm tm = to_tm(pt);
			int hour = tm.tm_hour;
			int minute = tm.tm_min;
			pt = ptime(pt.date(), hours(hour)+ minutes(minute));
			break;
		}
		default:
			return false;
	}
	value.setDateTime(dt);
	return true;
}
