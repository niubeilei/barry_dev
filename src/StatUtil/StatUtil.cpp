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
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatUtil.h"
#include "API/AosApi.h"

bool
AosStatUtil::include()
{
	return true;
}
	
bool
AosStatUtil::setStatValueToValueRslt(
		const AosRundataPtr &rdata,
		char * value,
		const AosDataType::E data_type,
		AosValueRslt &value_rslt)
{
	switch (data_type)
	{
	case AosDataType::eU32:
		{
			value_rslt.setU64(*(u64 *)value);
			return true;
		}
	
	case AosDataType::eU64:
		{
			value_rslt.setU64(*(u64 *)value);
			return true;
		}
		
	case AosDataType::eInt64:
		{
			value_rslt.setI64(*(int64_t *)value);
			return true;
		}

	default:
		OmnNotImplementedYet;
		break;
	}
	return false;
}


#if 0
bool
AosStatUtil::setValueRsltToBuff(
		AosRundata *rdata,
		const AosValueRslt &value,
		const AosDataType::E data_type,
		const AosBuffPtr &buff)
{
	bool rslt;
	switch (data_type)
	{
	case AosDataType::eU32:
		{
			u32 vv = value.getU32Value(rdata);
			buff->setU32(vv);
			return true;
		}
	
	case AosDataType::eU64:
		{
			u64 vv;
			rslt = value.getU64Value(vv, rdata);
			aos_assert_r(rslt, false);
			buff->setU64(vv);
			return true;
		}
		
	case AosDataType::eInt64:
		{
			int64_t vv = value.getInt64Value(rdata);
			buff->setI64(vv);
			return true;
		}

	default:
		OmnNotImplementedYet;
		break;
	}
	return false;
}
	
bool
AosStatUtil::setStatValueToBuff(
		const AosRundataPtr &rdata,
		char * value,
		const AosDataType::E data_type,
		const AosBuffPtr &buff)
{
	switch (data_type)
	{
	case AosDataType::eU32:
		{
			buff->setU32(*(u32 *)value);
			return true;
		}
	
	case AosDataType::eU64:
		{
			buff->setValue(*(u64 *)value);
			return true;
		}
		
	case AosDataType::eInt64:
		{
			buff->setI64(*(int64_t *)value);
			return true;
		}

	default:
		OmnNotImplementedYet;
		break;
	}
	return false;
	
}	

bool
AosStatUtil::setStatValueToValueRslt(
		const AosRundataPtr &rdata,
		char * value,
		const AosDataType::E data_type,
		AosValueRslt &value_rslt)
{
	switch (data_type)
	{
	case AosDataType::eU32:
		{
			value_rslt.setU32(*(u32 *)value);
			return true;
		}
	
	case AosDataType::eU64:
		{
			value_rslt.setValue(*(u64 *)value);
			return true;
		}
		
	case AosDataType::eInt64:
		{
			value_rslt.setValue(*(int64_t *)value);
			return true;
		}

	default:
		OmnNotImplementedYet;
		break;
	}
	return false;
}


bool
AosStatUtil::setStatValueToStr(
		const AosRundataPtr &rdata,
		const AosBuffPtr &value,
		const AosDataType::E data_type,
		OmnString &content)
{
	// will delete later.
	switch (data_type)
	{
	case AosDataType::eU32:
		{
			u32 vv = value->getU32(0);
			content << vv;
			return true;
		}
	
	case AosDataType::eU64:
		{
			u64 vv = value->getU64(0);;
			content << vv;
			return true;
		}
		
	case AosDataType::eInt64:
		{
			int64_t vv = value->getInt64(0);
			content << vv;
			return true;
		}

	default:
		OmnNotImplementedYet;
		break;
	}
	return false;
}

/*
bool
AosStatUtil::getStatValue(
		char *array,
		const int64_t data_len,
		const int pos,
		AosDataType::E data_type,
		const AosBuffPtr &buff) 
{
	aos_assert_r(pos + AosDataType::getValueSize(data_type) <= data_len, false);
	switch (data_type)
	{
	case AosDataType::eU32:
		 {
			 u32 vv = *(u32*)&array[pos];
			 buff->setU32(vv);
		 }
		 break;

	case AosDataType::eU64:
		 {
			 u64 vv = *(u64*)&array[pos];
			 buff->setU64(vv);
		 }
		 break;

	case AosDataType::eInt64:
		 {
			 int64_t vv = *(int64_t*)&array[pos];
			 buff->setI64(vv);
		 }
		 break;

	default:	
		 break;
	}
	return true;
}
*/


bool
AosStatUtil::setInt64ToBuff(
		const AosRundataPtr &rdata,
		const int64_t &vv,
		const AosDataType::E data_type,
		const AosBuffPtr &buff)
{
	switch (data_type)
	{
	case AosDataType::eU32:
		{
			buff->setU32(vv);
			return true;
		}
	
	case AosDataType::eU64:
		{
			buff->setU64(vv);
			return true;
		}
		
	case AosDataType::eInt64:
		{
			buff->setI64(vv);
			return true;
		}

	default:
		OmnNotImplementedYet;
		break;
	}
	return false;
}


/*
bool
AosStatUtil::checkIsCountRecds(
		const AosRundataPtr &rdata,
		AosExprObj* expr)
{
	AosExprType::E type = expr->getType();
	AosValueRslt v;
	if (AosExprType::eGenFunc == type)
	{
		AosExprGenFunc* func_expr = dynamic_cast<AosExprGenFunc*>(expr);
		OmnString func_name = func_expr->getFuctName();
		if (func_name.toLower() != "count") return false;
		AosExprList *expr_list = func_expr->getParmList();
		AosExprObj* expr;
		if (expr_list)
		{
			if (expr_list->size() != 1) return false;
			expr = (*expr_list)[0];
			bool rslt = expr->getValue(0, 0, v);
			aos_assert_r(rslt, false);
			OmnString rname = v.getValueStr1();
			if (rname != "*" && rname != "_rec_count") return false;
			return true;
		}
		return false;
	}
	return false;
}
*/

int
AosStatUtil::getSvrIdByStatid(
		const u64 stat_id, 
		const int docs_per_dist)
{
	u32 groupid = stat_id / docs_per_dist;
	int vid = groupid % AosGetNumCubes();

	int cube_grp_id = AosGetCubeGrpIdByCubeId(vid);
	aos_assert_r(cube_grp_id >=0, false);

	return AosGetConfigMaster(cube_grp_id);
}
#endif
