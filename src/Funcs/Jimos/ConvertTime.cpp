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
// 2014/09/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Funcs/Jimos/GenFuncConvertTime.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGenFuncConvertTime_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGenFuncConvertTime(version);
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


AosGenFuncConvertTime::AosGenFuncConvertTime(const int version)
:
AosGenFunc("GenFuncConvertTime", version)
{
}

AosGenFuncConvertTime::AosGenFuncConvertTime()
:
AosGenFunc("GenFuncConvertTime", 1)
{
}



AosGenFuncConvertTime::AosGenFuncConvertTime(const AosGenFuncConvertTime &rhs)
:
AosGenFunc(rhs)
{
}


AosGenFuncConvertTime::~AosGenFuncConvertTime()
{
}


bool
AosGenFuncConvertTime::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


func(name:value, name:value, ...)

bool
AosGenFuncConvertTime::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	// This function converts time from one format to another format. 
	// It assumes the following parms:
	// 	value:	the value to be converted
	// 	from:	the time format id in which the value is expressed
	// 	to:		the time format id in which the converted value is expressed.
	AosExprObj * value_parm = getParm("value");
	if (!value_parm)
	{
		AosLogError(rdata, true, "missing_value_parm") << enderr;
		return false;
	}

	// 1. Retrieve the time value
	AosValueRslt vv;
	if (!(value_parm->getValue(rdata, record, vv))) return false;

	// 2. Retrieve the from time formatter.
	AosExprObj * from_parm = getParm("from");
	if (!from_parm)
	{
		AosLogError(rdata, true, "missing_from_parm") << enderr;
		return false;
	}

	AosValueRslt fmt_name;
	if (!(from_parm->getValue(rdata, record, fmt_name))) return false;

	AosTimeFormatter *from_formatter = AosGetTimeFormatter(rdata, fmt_name.getStr();
	if (!from_formatter)
	{
		AosLogError(rdata, true, "from_time_formatter_not_found") 
			<< from_formatter->toString() << enderr;
		return false;
	}

	// 3. Retrieve the to time formatter
	AosExprObj * to_parm = getParm("to");
	if (!to_parm)
	{
		AosLogError(rdata, true, "missing_to_parm") << enderr;
		return false;
	}

	if (!(to_parm->getValue(rdata, record, fmt_name))) return false;
	AosTimeFormatter *to_formatter = AosGetTimeFormatter(rdata, fmt_name.getStr();
	if (!to_formatter)
	{
		AosLogError(rdata, true, "to_time_formatter_not_found") 
			<< to_formatter->toString() << enderr;
		return false;
	}

	// 4. Convert the original time to JUT
	u64 jut;
	if (!from_formatter->encode(rdata, vv, jut)) return false;

	// 5. Convert the JUT to the target time format
	if (!to_formatter->decode(rdata, jut, value)) return false;

	return true;
}


bool 
AosGenFuncConvertTime::syntaxCheck(
		AosRundata *rdata, 
		OmnString &errmsg)
{
	if (!getParm("value")) 
	{
		AosLogError(rdata, true, "missing_value_parm") << toString() << enderr;
		return false;
	}

	if (!getParm("from")) 
	{
		AosLogError(rdata, true, "missing_from_parm") << toString() << enderr;
		return false;
	}

	if (!getParm("to")) 
	{
		AosLogError(rdata, true, "missing_to_parm") << toString() << enderr;
		return false;
	}

	return true;
}


AosJimoPtr
AosGenFuncConvertTime::cloneJimo()  const
{
	return OmnNew AosGenFuncConvertTime(*this);
}

#endif
