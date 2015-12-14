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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataRecordType_h
#define Aos_SEInterfaces_DataRecordType_h

#include "Util/String.h"


#define AOSRECORDTYPE_FIXBIN			"fixbin"
#define AOSRECORDTYPE_VARIABLE			"variable"
#define AOSRECORDTYPE_FIXED_BUFF		"fixedbuff"
#define AOSRECORDTYPE_VARIABLE_BUFF		"varybuff"
#define AOSRECORDTYPE_JIMO_RECORD 		"jimorecord"
#define AOSRECORDTYPE_CSV				"csv"
#define AOSRECORDTYPE_IIL				"iil"
#define AOSRECORDTYPE_CTNR				"ctnr"
#define AOSRECORDTYPE_BINARY_JIMO		"bjm"
#define AOSRECORDTYPE_BUFF				"buff"
#define AOSRECORDTYPE_XML				"xml"
#define AOSRECORDTYPE_JOIN				"join"
#define AOSRECORDTYPE_STAT				"stat"
#define AOSRECORDTYPE_BSON				"bson"
#define AOSRECORDTYPE_PARM_LIST         "ParmList"
#define AOSRECORDTYPE_SIMPLE_XML		"SimpleXml"
#define AOSRECORDTYPE_STAT_RECORD		"StatRecord" //Phil, 2015/01/01
#define AOSRECORDTYPE_MULTI				"multi"		 //Andy, 2015/05/05
#define AOSRECORDTYPE_INVALID			"invalid"

class AosDataRecordType
{
public:
	enum E
	{
		eInvalid,

		eFixedBinary,
		eFixedBuff,
		eVariableBuff,
		eVariable,
		eJimoRecord,
		eCSV,
		eIIL,
		eContainer,
		eBinaryJimo,
		eBuff,
		eXml,
		eJoin,
		eStat,
		eBSON,
		eParmList,
		eSimpleXml,
		eStatRecord, //Phil, 2015/01/01
		eMulti,		 //Andy, 2015/05/05

		eMax
	};

	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}

	inline static E toEnum(const OmnString &name)
	{
		if (name == AOSRECORDTYPE_FIXBIN) 			return eFixedBinary;
		if (name == AOSRECORDTYPE_VARIABLE) 		return eVariable;
		if (name == AOSRECORDTYPE_FIXED_BUFF) 		return eFixedBuff;
		if (name == AOSRECORDTYPE_VARIABLE_BUFF) 	return eVariableBuff;
		if (name == AOSRECORDTYPE_CSV) 				return eCSV;
		if (name == AOSRECORDTYPE_IIL) 				return eIIL;
		if (name == AOSRECORDTYPE_CTNR) 			return eContainer;
		if (name == AOSRECORDTYPE_BINARY_JIMO)		return eBinaryJimo;
		if (name == AOSRECORDTYPE_BUFF)				return eBuff;
		if (name == AOSRECORDTYPE_XML)				return eXml;
		if (name == AOSRECORDTYPE_JOIN)				return eJoin;
		if (name == AOSRECORDTYPE_STAT)				return eStat;
		if (name == AOSRECORDTYPE_PARM_LIST)        return eParmList;
		if (name == AOSRECORDTYPE_SIMPLE_XML)		return eSimpleXml;
		if (name == AOSRECORDTYPE_MULTI)			return eMulti;
		return eInvalid;
	}

	inline static OmnString toStr(const AosDataRecordType::E type)
	{
		if (type == eFixedBinary) 			return AOSRECORDTYPE_FIXBIN;
		if (type == eVariable) 				return AOSRECORDTYPE_VARIABLE;
		if (type == eFixedBuff) 			return AOSRECORDTYPE_FIXED_BUFF;
		if (type == eVariableBuff) 			return AOSRECORDTYPE_VARIABLE_BUFF;
		if (type == eCSV) 					return AOSRECORDTYPE_CSV;
		if (type == eIIL) 					return AOSRECORDTYPE_IIL;
		if (type == eContainer) 			return AOSRECORDTYPE_CTNR;
		if (type == eBinaryJimo)			return AOSRECORDTYPE_BINARY_JIMO;
		if (type == eBuff)					return AOSRECORDTYPE_BUFF;
		if (type == eXml)					return AOSRECORDTYPE_XML;
		if (type == eJoin)					return AOSRECORDTYPE_JOIN;
		if (type == eStat)					return AOSRECORDTYPE_STAT;
		if (type == eParmList)        		return AOSRECORDTYPE_PARM_LIST;
		if (type == eSimpleXml)				return AOSRECORDTYPE_SIMPLE_XML;
		if (type == eMulti)					return AOSRECORDTYPE_MULTI;
		return AOSRECORDTYPE_INVALID;

	}
};

#endif

