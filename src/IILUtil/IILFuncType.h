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
// 	Created: 07/07/2011 by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILTransClient_IILFuncType_h
#define AOS_IILTransClient_IILFuncType_h

//#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
//#include "Debug/Debug.h"

class AosIILFuncType
{
public:
	enum E
	{
		eInvalid 							= 0,
		
		eCreateIIL							= 1,
		eHitAddDocById						= 2,
		eHitAddDocByName					= 3,
		eHitRemoveDocById					= 4,
		eHitRemoveDocByName					= 5,
		eQueryDocById						= 6,
		eQueryDocByName						= 7,
		eQueryRangeDocById					= 8,
		eStrAddInlineSearchValue			= 9,

		eStrAddValueDocById					= 10,
		eStrAddValueDocByName				= 11,				
		eStrBindCloudid						= 12,
		eStrBindObjid						= 13,
		eStrGetDocidById					= 14,
		eStrGetDocidByName					= 15,
		eStrIncrementDocidById				= 16,
		eStrIncrementDocidByName			= 17,
		eStrIncrementInlineSearchValue		= 18,
		eStrModifyInlineSearchValue			= 19,

		eStrModifyValueDocById				= 20,
		eStrModifyValueDocByName			= 21,
		eStrRemoveFirstValueDocByName		= 22,
		eStrRemoveInlineSearchValue			= 23,
		eStrRemoveValueDocById				= 24,
		eStrRemoveValueDocByName			= 25,
		eStrSetValueDocUniqueById			= 26,
		eU64AddAncestor						= 27,
		eU64AddDescendant					= 28,
		eU64AddValueDocById					= 29,

		eU64AddValueDocByName				= 30,
		eU64GetAncestor						= 31,
		eU64GetDocidById					= 32,
		eU64GetDocidByName					= 33,
		eU64IncrementDocidById				= 34,
		eU64IncrementDocidByName			= 35,
		eU64ModifyValueDocById				= 36,
		eU64ModifyValueDocByName			= 37,
		eU64RemoveAncestor					= 38,
		eU64RemoveDescendant				= 39,

		eU64RemoveValueDocById				= 40,
		eU64RemoveValueDocByName			= 41,
		eU64SetValueDocUniqueById			= 42,
		eCheckDocExist						= 43,
		eCreateIILs							= 44,
		eGetIILID							= 45,
		eGetIILType							= 46,
		eGetTotalByOpr						= 47,
		eGetTotalNumDocs					= 48,
		eHitGetCtnrMemDocid					= 49,

		eHitGetCtnrMemDocidByTwoKey			= 50,
		eHitQueryDoc						= 51,
		eStrCheckDocid						= 52,
		eStrGetDocid						= 53,
		eStrGetDocidBN						= 54,
		eStrQueryValue						= 55,
		eU64ManualOrderAppend				= 56,
		eU64ManualOrderMove					= 57,
		eU64ManualOrderRemove				= 58,
		eU64ManualOrderSwap					= 59,

		eU64ModifyValueDoc					= 60,
		eU64QueryValue						= 61,
		eU64RemoveDocByValue				= 62,
		eU64SetValueDocUnique				= 63,
		eU64UpdateKeyedValue				= 64,
		eU64UpdateKeyedValueInc				= 65,
		eStrGetDocids						= 66,
		eStrGetDocidByPrefix				= 67,
		eStrGetDocidByValue					= 68,
		eStrGetDocidDft						= 69,

		eStrGetDocidsByKeys					= 70,	// Ken Lee, 2012/04/18
		eStrRemoveDocByPrefix				= 71,
		eStrQueryRangesById					= 72,	// Chen Ding, 04/03/2012
		eStrQueryColumn						= 73,	// Chen Ding, 05/05/2012
		eStrQueryColumnRange				= 74,	// Chen Ding, 05/05/2012
		eCreateIILByName                    = 75,
		eQueryRangeDocByName				= 76,
		eStrGetSplitValue					= 77,
		eGetSplitValueByName				= 78,	// Ken Lee, 2012/07/31
		
		eDeleteIILById						= 79,
		eDeleteIILByName					= 80,
		ePreQueryById						= 81,
		ePreQueryByName						= 82,
		eCounterRangeDocById				= 83,
		eDistQueryDocByName					= 84,
		eHitBatchAdd						= 85,	// Chen Ding, 03/25/2012
		eRetrieveQueryProcBlock				= 86,	//Chen Ding, 01/03/2013
		eRetrieveNodeList					= 87,	//Chen Ding, 01/03/2013
		eCounterRangeDocByName				= 88,
		
		eStrBatchAdd						= 89,	// Ken Lee, 2013/03/25	
		eStrBatchInc						= 90,	// Ken Lee, 2013/03/25
		
		eBitmapQueryByPhysical				= 91,	// Ken Lee, 2013/03/29

		eAddValueDoc 						= 92,
		eRemoveValueDoc 					= 93,
		eModifyValueDoc 					= 94,
		eIncrementDocid 					= 95,
		eGetDocid 							= 96,
		eBatchDel 							= 97,
		eBatchAdd 							= 98,

		eMax								= 99
	};

	static inline E toEnum(const int value)
	{
		E type = (AosIILFuncType::E)value;
		if (type <= eInvalid || type >= eMax)
		{
			OmnAlarm << "Invalid IILFuncType: " << value << enderr;
			return eInvalid;
		}
		return type;
	}

	static inline bool isValid(const E type)
	{
		return (type > eInvalid && type < eMax);
	}
};


#endif

