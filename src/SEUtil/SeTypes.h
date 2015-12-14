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
// 	Created: 12/13/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_Types_h
#define Omn_SEUtil_Types_h

#define AOS_NULL_STRING "__zky_null__"
#define AOS_INVDID 0
#define AOS_INVSID 0
#define AOS_INVWID 0
#define AOS_INVSECID 0x0fffffff
#define AOS_INVIILID 0
#define AOS_ACTIVE_FLAG 'a'
#define AOSSE_MAX_WORDLEN	64
#define AOS_CONTAINER_SEP ":"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"

#define AOSOBJTEMPTYPE_CONSTANT		"const"
#define AOSOBJTEMPTYPE_SEQNO		"seqno"
#define AOSOBJTEMPTYPE_YEAR			"year"
#define AOSOBJTEMPTYPE_MONTH		"month"
#define AOSOBJTEMPTYPE_DAYOFMONTH	"dom"
#define AOSOBJTEMPTYPE_DAY			"day"

// Chen Ding, 02/29/2012, Moved to XmlUtil/DocTypes.h
// enum
// {
// 	eDocTypeBitsToShift = 60		// Highest 6 bits reserved for doc types
// };

// Chen Ding, 02/29/2012
// Moved to XmlUtil/DocTypes.h
// enum AosDocType
// {
//    eAosNormalDoc = 0,
//
//    eAosAdminDoc,
//    eAosAccessDoc,
//    eAosLogDoc,
//    eAosCounterDoc,
//	eAosTempDoc,
//    eAosUnkonwDoc
//};

enum AosAttrType
{
	eAosAttrTypeChar 		= '_',

	eAosAttrType_Ignore		= 'n',
	eAosAttrType_U64 		= 'u',
	eAosAttrType_I32 		= 'i',
	eAosAttrType_Str 		= 's',
	eAosAttrType_Date   	= 'd',
	eAosAttrType_NumAlpha 	= 'a'
};


inline OmnString AosGetAttrPostfix(const AosAttrType type)
{
	switch (type)
	{
	case eAosAttrType_Ignore:
		 return "__n";

	case eAosAttrType_U64:
		 return "__u";

	case eAosAttrType_I32:
		 return "__i";

	case eAosAttrType_Str:
		 return "__s";

	case eAosAttrType_Date:
		 return "__d";

	case eAosAttrType_NumAlpha:
		 return "__a";

	default:
		 break;
	}
	OmnAlarm << "Unrecognized attribute type: " << type << enderr;
	return "";
}

inline AosAttrType AosGetAttrType(const OmnString &aname)
{
	const char *data = aname.data();
	int len = aname.length();
	if (len <= 3) return eAosAttrType_Str;

	if (data[len-2] != eAosAttrTypeChar || data[len-3] != eAosAttrTypeChar) return eAosAttrType_Str;
	
	switch (data[len-1])
	{
	case 'i' : return eAosAttrType_I32;
	case 'u' : return eAosAttrType_U64;
	case 'n' : return eAosAttrType_Ignore;
	case 'a' : return eAosAttrType_NumAlpha;
	case 'd' : return eAosAttrType_Date;
	default  : break;
	}

	return eAosAttrType_Str;
}


enum AosSeLogType
{
	eAosSeLogType_Invalid 	= 0,

	eAosSeLogType_SysAdmin,			// For system administration log
	eAosSeLogType_UserDefined,		// For user defined logs
	eAosSeLogType_Login,				// For logins (container-based)
	eAosSeLogType_CtnrAdmin,			// For admin of a specific container
};


enum AosIILType
{
	eAosIILType_Invalid,
	eAosIILType_Docid,
	eAosIILType_U16,
	eAosIILType_U32,
	eAosIILType_U64,
	eAosIILType_Str,
	eAosIILType_Variable,
	eAosIILType_Cnt,
	eAosIILType_Hit,
	eAosIILType_NumAlpha,
	eAosIILType_CompStr,
	eAosIILType_CompU64,
	
	eAosIILType_BigStr,
	eAosIILType_BigU64,
	eAosIILType_BigI64,
	eAosIILType_BigD64,
	eAosIILType_BigHit,
	eAosIILType_JimoTable,

	eAosIILType_Total
};


inline bool AosIsValidIILType(const AosIILType type)
{
	return (type > eAosIILType_Invalid && type < eAosIILType_Total);
}


inline bool AosIILType_toCode(AosIILType &iiltype, char value)
{
	if (value < eAosIILType_Docid || value >= eAosIILType_Total)
	{
		OmnAlarm << "Invalid IIL Type: " << value << enderr;
		return false;
	}

	iiltype = (AosIILType)value;
	return true;
}


inline OmnString AosIILType_toStr(const AosIILType code)
{
	switch (code)
	{
	case eAosIILType_Docid:
		 return "Docid";

	case eAosIILType_U16:
		 return "U16";

	case eAosIILType_U32:
		 return "U32";

	case eAosIILType_U64:
		 return "U64";

	case eAosIILType_Str:
		 return "String";

	case eAosIILType_Variable:
		 return "Variable";

	case eAosIILType_Cnt:
		 return "Cnt";

	case eAosIILType_Hit:
		 return "Hit";
	
	case eAosIILType_NumAlpha:
		 return "NumAlpha";

	case eAosIILType_CompStr:
		 return "compStr";
	
	case eAosIILType_CompU64:
		 return "compU64";
	
	case eAosIILType_BigStr:
		 return "BigStr";

	case eAosIILType_BigU64:
		 return "BigU64";

	case eAosIILType_BigI64:
		 return "BigI64";

	case eAosIILType_BigD64:
		 return "BigD64";

	case eAosIILType_JimoTable:
		 return "JimoTable";

	default:
		 return "Invalid";
	}
}


//ken 2011/08/04
inline AosIILType AosIILType_toCode(const OmnString &str)
{
	aos_assert_r(str.length()>=3, eAosIILType_Invalid);
	switch (str.data()[0])
	{
	case 'C' :
		 return eAosIILType_Cnt;
	
	case 'D' :
		 return eAosIILType_Docid;

	case 'H' :
		 return eAosIILType_Hit;
	
	case 'N' :
		 return eAosIILType_NumAlpha;

	case 'S' :
		 return eAosIILType_Str;
	
	case 'U' :
		 if (str.data()[1] == '1') return eAosIILType_U16;
		 if (str.data()[1] == '3') return eAosIILType_U32;
		 if (str.data()[1] == '6') return eAosIILType_U64;
		 return eAosIILType_Invalid;

	case 'V' :
		 return eAosIILType_Variable;
	
	case 'B' :
		 if (str == "BigStr") return eAosIILType_BigStr;
		 if (str == "BigU64") return eAosIILType_BigU64;
		 if (str == "BigI64") return eAosIILType_BigI64;
		 if (str == "BigD64") return eAosIILType_BigD64;
		 return eAosIILType_Invalid;

	case 'J' :
		 return eAosIILType_JimoTable;

	default:
		 return eAosIILType_Invalid;
	}
}

enum
{
	eAosInMemoryFlag = 0x80000000,		// The highest bit
	eStreamingExecutorId = 10,               // Jozhi Streaming for rebuild bitmap
};

inline bool AosIsIILAlphaNumeric(const OmnString &name)
{
	AosAttrType type = AosGetAttrType(name);
	return type == eAosAttrType_NumAlpha;
}


// Chen Ding, 12/07/2010
enum AosCounterId
{
	eAosCounterId_Invalid,

	eAosCounterId_Docid, 
	eAosCounterId_IILID,

	eAosCounterId_Max
};

#endif
