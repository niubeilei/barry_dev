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
#ifndef Aos_IILUtil_IILUtil_h
#define Aos_IILUtil_IILUtil_h

#include "SEUtil/SeTypes.h"
#include "Util/Opr.h"
#include "Util/Orders.h"

#ifndef AosIILFIELDSEP
#define AosIILFIELDSEP 0x02
#endif 

class AosIILUtil
{
public:
	enum AosIILIncType
	{
		eIILIncNormal,
		eIILIncNoAdd,
		eIILIncNoUpdate,
		eIILIncSet,
		eIILIncDict,
		eIILIncUpdateMin,
		eIILIncUpdateMax
	};
	enum CopyType
	{
		eCopyAll = 0,
		eCopyFromHead = 1,
		eCopyToTail = 2,
		eCopyFromTail = 3,
		eCopyToHead = 4,
		eCopyNormal = 5
	};
	enum
	{
		eMaxStrValueLen = 1000
	};
	
	static bool isValueTooLong(const int len) { return len > eMaxStrValueLen; }
	static u64 getManualOrderIncValue() { return (((u64)1) << 32); }
	
	inline static AosIILIncType IILIncType_toEnum(const OmnString &type)
	{
		if (type == "norm") return eIILIncNormal;
		if (type == "sum") return eIILIncNormal;
		if (type == "noadd") return eIILIncNoAdd;
		if (type == "noupdate") return eIILIncNoUpdate;
		if (type == "set") return eIILIncSet;
		if (type == "min") return eIILIncUpdateMin;
		if (type == "max") return eIILIncUpdateMax;
		if (type == "dict") return eIILIncDict;

		OmnAlarm << "type error" << type << enderr;
		return eIILIncNormal;
	}
	
	inline static OmnString IILIncType_toStr(const AosIILIncType &type)
	{
		if (type == eIILIncNormal) return "norm";
		if (type == eIILIncNoAdd) return "noadd";
		if (type == eIILIncNoUpdate) return "noupdate";
		if (type == eIILIncSet) return "set";
		if (type == eIILIncUpdateMin) return "min";
		if (type == eIILIncUpdateMax) return "max";

		OmnAlarm << "type error" << type << enderr;
		return "norm";
	}
	
	inline static int valueMatch(
			const OmnString &v,
			const OmnString &value, 
			const bool isNumAlpha)
	{
		return valueMatch(v.data(), value.data(), isNumAlpha);
	}
	
	inline static int valueNMatch(
			const OmnString &v,
			const OmnString &value, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		return valueNMatch(v.data(), value.data(), isNumAlpha, cmp_len);
	}
	
	inline static int valueMatch(
			const char *v,
			const OmnString &value, 
			const bool isNumAlpha)
	{
		return valueMatch(v, value.data(), isNumAlpha);
	}
	
	inline static int valueNMatch(
			const char *v,
			const OmnString &value, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		return valueNMatch(v, value.data(), isNumAlpha, cmp_len);
	}
	
	inline static int valueMatch(
			const char *v1,
			const char *v2, 
			const bool isNumAlpha)
	{
		if (isNumAlpha)
		{
			int v1Len = strlen(v1);
			int v2Len = strlen(v2);
			if(v1Len < v2Len) return -1;
			else if(v1Len > v2Len) return 1;
		}
		return strcmp(v1, v2);
	}
	
	inline static int valueNMatch(
			const char *v1,
			const char *v2, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		if (isNumAlpha)
		{
			int v1Len = strlen(v1);
			int v2Len = strlen(v2);
			if(v1Len < v2Len) return -1;
			else if(v1Len > v2Len) return 1;
		}
		if(cmp_len)
		{
			return strncmp(v1, v2, cmp_len);
		}
		return strcmp(v1,v2);
	}

	inline static int valueMatch(
			const OmnString &v,
			const u64 &d,
			const OmnString &value,
			const u64 &docid, 
			const bool isNumAlpha)
	{
		return valueMatch(v.data(), d, value.data(), docid, isNumAlpha);
	}
	
	inline static int valueNMatch(
			const OmnString &v,
			const u64 &d,
			const OmnString &value,
			const u64 &docid, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		return valueNMatch(v.data(), d, value.data(), docid, isNumAlpha, cmp_len);
	}
	
	inline static int valueMatch(
			const char *v,
			const u64 &d,
			const OmnString &value,
			const u64 &docid,
			const bool isNumAlpha)
	{
		return valueMatch(v, d, value.data(), docid, isNumAlpha);
	}
	
	inline static int valueNMatch(
			const char *v,
			const u64 &d,
			const OmnString &value,
			const u64 &docid,
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		return valueNMatch(v, d, value.data(), docid, isNumAlpha, cmp_len);
	}
	
	inline static int valueMatch(
			const char *v1,
			const u64 &d,
			const char *v2, 
			const u64 &docid, 
			const bool isNumAlpha)
	{
		if (isNumAlpha)
		{
			int v1Len = strlen(v1);
			int v2Len = strlen(v2);
			if(v1Len < v2Len) return -1;
			else if(v1Len > v2Len) return 1;
		}
		int rslt = strcmp(v1, v2);
		if(rslt != 0) return rslt;
		if(d > docid) return 1;
		if(d < docid) return -1;
		return 0;
	}
	
	inline static int valueNMatch(
			const char *v1,
			const u64 &d,
			const char *v2, 
			const u64 &docid, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		if (isNumAlpha)
		{
			int v1Len = strlen(v1);
			int v2Len = strlen(v2);
			if(v1Len < v2Len) return -1;
			else if(v1Len > v2Len) return 1;
		}
		int rslt = 0;
		if(cmp_len)
		{
			rslt = strncmp(v1, v2, cmp_len);
		}
		else
		{
			rslt = strcmp(v1, v2);
		}
		if(rslt != 0) return rslt;
		if(d > docid) return 1;
		if(d < docid) return -1;
		return 0;
	}

	inline static bool valueMatch(
			const OmnString &v, 
			const AosOpr opr, 
			const OmnString &value1, 
			const OmnString &value2, 
			const bool isNumAlpha)
	{
		return valueMatch(v.data(), opr, value1, value2, isNumAlpha);
	}
	
	inline static bool valueNMatch(
			const OmnString &v, 
			const AosOpr opr, 
			const OmnString &value1, 
			const OmnString &value2, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		return valueNMatch(v.data(), opr, value1, value2, isNumAlpha, cmp_len);
	}

	inline static bool valueMatch(
			const char *v, 
			const AosOpr opr, 
			const OmnString &value1, 
			const OmnString &value2, 
			const bool isNumAlpha)
	{
		// Chen Ding, 2014/02/26
		// aos_assert_r(value1.length() <= eMaxStrValueLen, false);
		
		if(opr == eAosOpr_an)
		{
			return true;
		}
		if(opr == eAosOpr_like)
		{	
			return valueLikeMatch(v, value1);
		}
		
		int rsltInt = valueMatch(v, value1, isNumAlpha);
		switch (opr)
		{
			case eAosOpr_le: return rsltInt <= 0;
			case eAosOpr_lt: return rsltInt < 0;
			case eAosOpr_eq: return rsltInt == 0;
			case eAosOpr_gt: return rsltInt > 0;
			case eAosOpr_ge: return rsltInt >= 0;
			case eAosOpr_ne: return rsltInt != 0;

			// Chen Ding, 2014/02/26
			case eAosOpr_range_ge_le: 
				 return (rsltInt >= 0 && valueMatch(v, value2, isNumAlpha) <= 0);

			case eAosOpr_range_ge_lt: 
				 return (rsltInt >= 0 && valueMatch(v, value2, isNumAlpha) < 0);

			case eAosOpr_range_gt_le:
				 return (rsltInt > 0 && valueMatch(v, value2, isNumAlpha) <= 0);

			case eAosOpr_range_gt_lt:
				 return (rsltInt > 0 && valueMatch(v, value2, isNumAlpha) < 0);

			default: break;
		}
		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}
	
	inline static bool valueNMatch(
			const char *v, 
			const AosOpr opr, 
			const OmnString &value1, 
			const OmnString &value2, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		// Chen Ding, 2014/02/26
		// aos_assert_r(value1.length() <= eMaxStrValueLen, false);
		
		if(opr == eAosOpr_an)
		{
			return true;
		}
		if(opr == eAosOpr_like)
		{	
			return valueLikeMatch(v, value1);
		}
		
		int rsltInt = valueNMatch(v, value1, isNumAlpha, cmp_len);
		switch (opr)
		{
			case eAosOpr_le: return rsltInt <= 0;
			case eAosOpr_lt: return rsltInt < 0;
			case eAosOpr_eq: return rsltInt == 0;
			case eAosOpr_gt: return rsltInt > 0;
			case eAosOpr_ge: return rsltInt >= 0;
			case eAosOpr_ne: return rsltInt != 0;

			// Chen Ding, 2014/02/26
			case eAosOpr_range_ge_le: 
				 return (rsltInt >= 0 && valueNMatch(v, value2, isNumAlpha, cmp_len) <= 0);

			case eAosOpr_range_ge_lt: 
				 return (rsltInt >= 0 && valueNMatch(v, value2, isNumAlpha, cmp_len) < 0);

			case eAosOpr_range_gt_le:
				 return (rsltInt > 0 && valueNMatch(v, value2, isNumAlpha, cmp_len) <= 0);

			case eAosOpr_range_gt_lt:
				 return (rsltInt > 0 && valueNMatch(v, value2, isNumAlpha, cmp_len) < 0);

			default: break;
		}
		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}

	inline static bool valueMatch(
			const u64 &value,
			const AosOpr opr, 
			const u64 &v1, 
			const u64 &v2)
	{
		switch (opr)
		{
		case eAosOpr_le: return value <= v1;
		case eAosOpr_lt: return value < v1;
		case eAosOpr_eq: return value == v1;
		case eAosOpr_gt: return value > v1;
		case eAosOpr_ge: return value >= v1;
		case eAosOpr_ne: return value != v1;
		case eAosOpr_an: return true;
		case eAosOpr_range_ge_le: return (value >= v1 && value <= v2);
		case eAosOpr_range_ge_lt: return (value >= v1 && value < v2);
		case eAosOpr_range_gt_le: return (value > v1 && value <= v2);
		case eAosOpr_range_gt_lt: return (value > v1 && value < v2);
		default: break;
		}
		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}

	inline static bool valueMatch(
			const i64 &value,
			const AosOpr opr, 
			const i64 &v1, 
			const i64 &v2)
	{
		switch (opr)
		{
		case eAosOpr_le: return value <= v1;
		case eAosOpr_lt: return value < v1;
		case eAosOpr_eq: return value == v1;
		case eAosOpr_gt: return value > v1;
		case eAosOpr_ge: return value >= v1;
		case eAosOpr_ne: return value != v1;
		case eAosOpr_an: return true;
		case eAosOpr_range_ge_le: return (value >= v1 && value <= v2);
		case eAosOpr_range_ge_lt: return (value >= v1 && value < v2);
		case eAosOpr_range_gt_le: return (value > v1 && value <= v2);
		case eAosOpr_range_gt_lt: return (value > v1 && value < v2);
		default: break;
		}
		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}
	

	inline static bool valueMatch(
			const d64 &value,
			const AosOpr opr, 
			const d64 &v1, 
			const d64 &v2)
	{
		switch (opr)
		{
		case eAosOpr_le: return value <= v1;
		case eAosOpr_lt: return value < v1;
		case eAosOpr_eq: return value == v1;
		case eAosOpr_gt: return value > v1;
		case eAosOpr_ge: return value >= v1;
		case eAosOpr_ne: return value != v1;
		case eAosOpr_an: return true;
		case eAosOpr_range_ge_le: return (value >= v1 && value <= v2);
		case eAosOpr_range_ge_lt: return (value >= v1 && value < v2);
		case eAosOpr_range_gt_le: return (value > v1 && value <= v2);
		case eAosOpr_range_gt_lt: return (value > v1 && value < v2);
		default: break;
		}
		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}
	
	inline static bool valueLikeMatch(
			const char *v,
			const OmnString &value)
	{
		const char * r = strstr(v, value.data());
		return r != NULL;
	}

	static AosOrder::E getIILOrder(const OmnString &name)
	{
		AosAttrType type = AosGetAttrType(name);
		switch (type)
		{
		case eAosAttrType_NumAlpha: return AosOrder::eNumAlphabetic;
		default: return AosOrder::eAlphabetic;
		}
	}

	inline static bool valueMatch(
			const OmnString &v,
			const u64 &docid1, 
			const AosOpr opr, 
			const OmnString &value, 
			const u64 &docid2, 
			const bool isNumAlpha)
	{
		return valueMatch(v.data(), docid1, opr, value, docid2, isNumAlpha);
	}
	
	inline static bool valueNMatch(
			const OmnString &v,
			const u64 &docid1, 
			const AosOpr opr, 
			const OmnString &value, 
			const u64 &docid2, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		return valueNMatch(v.data(), docid1, opr, value, docid2, isNumAlpha, cmp_len);
	}

	inline static int valueMatch(
			const u64 &v1,
			const u64 &v2)
	{
		if(v1 > v2) return 1;
		if(v1 < v2) return -1;
		return 0;
	}
	

	inline static int valueMatch(
			const u64 &v1,
			const u64 &d1,
			const u64 &v2,
			const u64 &d2)
	{
		if (v1 < v2) return -1;
		if (v1 > v2) return 1;
		if (d1 < d2) return -1;
		if (d1 > d2) return 1;
		return 0;
	}

	inline static bool valueMatch(
			const u64 &v1,
			const u64 &docid1, 
			const AosOpr opr, 
			const u64 &v2, 
			const u64 &docid2)
	{
		if(opr == eAosOpr_an) 
		{
			return true;
		}
		
		int rsltInt = valueMatch(v1, v2);
		if(rsltInt == 0)
		{
			rsltInt = valueMatch(docid1, docid2);
		}
		switch (opr)
		{
		case eAosOpr_le: return rsltInt <= 0;
		case eAosOpr_lt: return rsltInt < 0;
		case eAosOpr_eq: return rsltInt == 0;
		case eAosOpr_gt: return rsltInt > 0;
		case eAosOpr_ge: return rsltInt >= 0;
		case eAosOpr_ne: return rsltInt != 0;
		default: break;
		}

		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}

	inline static bool valueMatch(
			const char *v, 
			const u64 &docid1, 
			const AosOpr opr, 
			const OmnString &value, 
			const u64 &docid2, 
			const bool isNumAlpha)
	{
		aos_assert_r(value.length() <= eMaxStrValueLen, false);
		
		if(opr == eAosOpr_an) 
		{
			return true;
		}
		if(opr == eAosOpr_like)
		{
			return valueLikeMatch(v, value);
		}
		
		int rsltInt = valueMatch(v, value, isNumAlpha);
		if(rsltInt == 0)
		{
			//rsltInt = docid1 - docid2;
			//ken 2012/05/14
			if(docid1 > docid2)
			{
				rsltInt = 1;
			}
			else if(docid1 == docid2)
			{
				rsltInt = 0;
			}
			else
			{
				rsltInt = -1;
			}
		}
		switch (opr)
		{
		case eAosOpr_le: return rsltInt <= 0;
		case eAosOpr_lt: return rsltInt < 0;
		case eAosOpr_eq: return rsltInt == 0;
		case eAosOpr_gt: return rsltInt > 0;
		case eAosOpr_ge: return rsltInt >= 0;
		case eAosOpr_ne: return rsltInt != 0;
		default: break;
		}

		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}
	
	inline static bool valueNMatch(
			const char *v, 
			const u64 &docid1, 
			const AosOpr opr, 
			const OmnString &value, 
			const u64 &docid2, 
			const bool isNumAlpha,
			const u64 &cmp_len)
	{
		aos_assert_r(value.length() <= eMaxStrValueLen, false);
		
		if(opr == eAosOpr_an) 
		{
			return true;
		}
		if(opr == eAosOpr_like)
		{
			return valueLikeMatch(v, value);
		}
		
		int rsltInt = valueNMatch(v, value, isNumAlpha, cmp_len);
		if(rsltInt == 0)
		{
			//rsltInt = docid1 - docid2;
			//ken 2012/05/14
			if(docid1 > docid2)
			{
				rsltInt = 1;
			}
			else if(docid1 == docid2)
			{
				rsltInt = 0;
			}
			else
			{
				rsltInt = -1;
			}
		}
		switch (opr)
		{
		case eAosOpr_le: return rsltInt <= 0;
		case eAosOpr_lt: return rsltInt < 0;
		case eAosOpr_eq: return rsltInt == 0;
		case eAosOpr_gt: return rsltInt > 0;
		case eAosOpr_ge: return rsltInt >= 0;
		case eAosOpr_ne: return rsltInt != 0;
		default: break;
		}

		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}


	inline static int valueMatch(
			const i64 &v1,
			const i64 &v2)
	{
		if(v1 > v2) return 1;
		if(v1 < v2) return -1;
		return 0;
	}

	inline static int valueMatch(
			const i64 &v1,
			const u64 &d1,
			const i64 &v2,
			const u64 &d2)
	{
		if (v1 < v2) return -1;
		if (v1 > v2) return 1;
		if (d1 < d2) return -1;
		if (d1 > d2) return 1;
		return 0;
	}

	inline static bool valueMatch(
			const i64 &v1,
			const u64 &docid1, 
			const AosOpr opr, 
			const i64 &v2, 
			const u64 &docid2)
	{
		if(opr == eAosOpr_an) 
		{
			return true;
		}
		
		int rsltInt = valueMatch(v1, v2);
		if(rsltInt == 0)
		{
			rsltInt = valueMatch(docid1, docid2);
		}
		switch (opr)
		{
		case eAosOpr_le: return rsltInt <= 0;
		case eAosOpr_lt: return rsltInt < 0;
		case eAosOpr_eq: return rsltInt == 0;
		case eAosOpr_gt: return rsltInt > 0;
		case eAosOpr_ge: return rsltInt >= 0;
		case eAosOpr_ne: return rsltInt != 0;
		default: break;
		}

		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}

	inline static int valueMatch(
			const d64 &v1,
			const d64 &v2)
	{
		if(v1 > v2) return 1;
		if(v1 < v2) return -1;
		return 0;
	}

	inline static int valueMatch(
			const d64 &v1,
			const u64 &d1,
			const d64 &v2,
			const u64 &d2)
	{
		if (v1 < v2) return -1;
		if (v1 > v2) return 1;
		if (d1 < d2) return -1;
		if (d1 > d2) return 1;
		return 0;
	}

	inline static bool valueMatch(
			const d64 &v1,
			const u64 &docid1, 
			const AosOpr opr, 
			const d64 &v2, 
			const u64 &docid2)
	{
		if(opr == eAosOpr_an) 
		{
			return true;
		}
		
		int rsltInt = valueMatch(v1, v2);
		if(rsltInt == 0)
		{
			rsltInt = valueMatch(docid1, docid2);
		}
		switch (opr)
		{
		case eAosOpr_le: return rsltInt <= 0;
		case eAosOpr_lt: return rsltInt < 0;
		case eAosOpr_eq: return rsltInt == 0;
		case eAosOpr_gt: return rsltInt > 0;
		case eAosOpr_ge: return rsltInt >= 0;
		case eAosOpr_ne: return rsltInt != 0;
		default: break;
		}

		OmnAlarm << "Unrecognized opr: " << opr << enderr;
		return false;
	}

	inline static int valueMatch2(
		const char * entry,
		const i64 &size,
		const char * key,
		const u64 &value,
		const bool isNumAlpha)
	{
		if(isNumAlpha)
		{
			int len1 = strlen(entry);
			if(len1 > size - (int)sizeof(u64)) len1 = size-sizeof(u64);
			int len2 = strlen(key);
			if(len1 > len2) return 1;
			if(len1 < len2) return -1;
		}
		int rslt = strncmp(entry, key,size-sizeof(u64));
		if (rslt != 0) return rslt;
	
		u64 entry_value = *(u64 *)&entry[size - sizeof(u64)];
		if (entry_value > value) return 1;
		if (entry_value < value) return -1;
		return 0;
	}

	inline static int valueMatch2(
		const char * entry,
		const i64 &size,
		const char * key,
		const bool isNumAlpha)
	{
		if(isNumAlpha)
		{
			int len1 = strlen(entry);
			if(len1 > size - (int)sizeof(u64)) len1 = size-sizeof(u64);
			int len2 = strlen(key);
			if(len1 > len2) return 1;
			if(len1 < len2) return -1;
		}
		return strncmp(entry, key,size-sizeof(u64));
	}



};

#endif

