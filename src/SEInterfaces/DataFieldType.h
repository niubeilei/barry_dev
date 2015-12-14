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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldType_h
#define Aos_DataField_DataFieldType_h

#include "Util/String.h"
#include "Util/DataTypes.h"

#define AOSDATAFIELDTYPE_INVALID			"invalid"
#define AOSDATAFIELDTYPE_COLCOMP			"colcomp"
#define AOSDATAFIELDTYPE_MAP				"map"
#define AOSDATAFIELDTYPE_IILENTRY			"iilentry"		// Ken Lee, 2015/03/10
#define AOSDATAFIELDTYPE_IILMAP				"iilmap"
#define AOSDATAFIELDTYPE_STR				"str"
#define AOSDATAFIELDTYPE_CHAR				"char"
#define AOSDATAFIELDTYPE_UCHAR				"uchar"
#define AOSDATAFIELDTYPE_U64				"u64"
#define AOSDATAFIELDTYPE_U32				"u32"
#define AOSDATAFIELDTYPE_INT64				"int64"
#define AOSDATAFIELDTYPE_INT32				"int32"
#define AOSDATAFIELDTYPE_DOUBLE				"double"
#define AOSDATAFIELDTYPE_BOOL				"bool"
#define AOSDATAFIELDTYPE_METADATA			"metadata"

#define AOSDATAFIELDTYPE_JIMO_FIELD			"jimo_field"

#define AOSDATAFIELDTYPE_DOCID				"docid"		// Ketty 2013/12/23
#define AOSDATAFIELDTYPE_BIN_U64			"bin_u64"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_INT64			"bin_int64"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_U32			"bin_u32"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_INT			"bin_int"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_CHAR			"bin_char"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_UCHAR			"bin_uchar"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_BOOL			"bin_bool"		// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_DOUBLE			"bin_double"	// Young, 2014/01/16
#define AOSDATAFIELDTYPE_BIN_DATETIME		"bin_datetime" 	// Young, 2015/06/24
#define AOSDATAFIELDTYPE_NUMBER				"number"
#define AOSDATAFIELDTYPE_DATETIME			"datetime" 		// Young, 2015/06/24
	
#define AOSDATAFIELDTYPE_BUFF				"buff"
#define AOSDATAFIELDTYPE_XPATH				"xpath"
#define AOSDATAFIELDTYPE_EXPR				"expr"
#define AOSDATAFIELDTYPE_U64New				"u64_new"		// Chen Ding, 2014/08/16

#define AOSDATAFIELDTYPE_VIRTUAL 			"virtual"     // Andy Zhang , 2015/05/05

class AosDataFieldType 
{
public:
	enum E
	{
		eInvalid,

		eBinU64,
		eBinInt64,
		eBinU32,
		eBinInt,
		eBinChar,
		eBinUChar,
		eBinBool,
		eBinDouble, 	// Young, 2014/12/10
		eBinDateTime,
		eColComp,
		eMap,
		eIILEntry,		// Ken Lee, 2015/03/10
		eIILMap,
		eStr,
		eChar,
		eUChar,
		eU64,
		eU32,
		eInt64,
		eInt32,
		eDouble,
		eBool,
		eMetaData,

		eJimoField,
		eDocid,
		
		eBuff,		// Ketty 2014/02/18
		eVirtual,	// Andy Zhang  2015/05/06
		eXpath,
		eExpr,

		eU64New, 	// Chen Ding, 2014/08/16
		eNumber,	// Young, 2014/12/30
		eDateTime,	// Young, 2015/06/16
		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	
	static bool isValid(const OmnString &code)
	{
		return isValid(toEnum(code));
	}
	
	static E toEnum(const OmnString &code);
	static OmnString toString(const E code);
	static bool check();
	static bool addName(const OmnString &name, const E code);
};

#endif
