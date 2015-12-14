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
// 2011/05/02	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_IILMgr_IILUtil_h
#define Omn_IILMgr_IILUtil_h

#include "Util/Opr.h"

class AosIILUtil
{
public:
	enum
	{
		eMaxStrValueLen = 80
	};
	static bool isValueTooLong(const int len) {return len > eMaxStrValueLen;}
	static u64 getManualOrderIncValue() {return (((u64)1) << 32);}

	inline static int valueMatch(
					const OmnString &v1,
					const OmnString &value, 
					const bool isNumAlpha)
	{
		return valueMatch(v1.data(), value, isNumAlpha);
	}
	inline static int valueMatch(
					const char *v1,
					const OmnString &value, 
					const bool isNumAlpha)
	{
		int vaLen = value.length();
		aos_assert_r(vaLen <= eMaxStrValueLen, -1);

		// Chen Ding, 01/29/2012
		// if (isNumAlpha)
		if (isNumAlpha)
		{
			int vLen = strlen(v1);
			if(vLen < vaLen) return -1;
			else if(vLen > vaLen) return 1;
		}
		
		// Ken, 05/30/2011
		// if ((u32)vaLen <= eMaxStrValueLen)
		// {
			return strcmp(v1, value.data());
		// }
		// else
		// {
		//	return strncmp(v1, value.data(), eMaxStrValueLen);
		// }
	}

	inline static bool valueMatch(
			const OmnString &v1, 
			const AosOpr opr, 
			const OmnString &value, 
			const bool isNumAlpha)
	{
		return valueMatch(v1.data(), opr, value, isNumAlpha);
	}

	inline static bool valueMatch(
			const char *v1, 
			const AosOpr opr, 
			const OmnString &value, 
			const bool isNumAlpha)
	{
		if(opr == eAosOpr_an)return true;
		if(opr == eAosOpr_like)
		{	
			const char * r = strstr(v1, value.data());
			return r != NULL;
		}
		int rslt;
		int vaLen = value.length();
		aos_assert_r(vaLen <= eMaxStrValueLen, -1);
		
		bool haverslt = false;
		if(isNumAlpha)
		{
			int vLen = strlen(v1);
			if(vLen < vaLen)
			{
				rslt = -1;
				haverslt = true;
			}
			else if(vLen > vaLen)
			{
				rslt = 1;
				haverslt = true;
			}
		}

		if (!haverslt)
		{
			// Ken, 05/30/2011
			//if ((u32)vaLen <= eMaxStrValueLen)
			//{
				rslt = strcmp(v1, value.data());
			//}
			//else
			//{
			//	rslt = strncmp(v1, value.data(), eMaxStrValueLen);
			//}
		}

		switch (opr)
		{
			case eAosOpr_le: return rslt <= 0;
			case eAosOpr_lt: return rslt < 0;
			case eAosOpr_eq: return rslt == 0;
			case eAosOpr_gt: return rslt > 0;
			case eAosOpr_ge: return rslt >= 0;
			case eAosOpr_ne: return rslt != 0;
			default: return false;
		}
		return false;
	}
};
#endif
#endif
