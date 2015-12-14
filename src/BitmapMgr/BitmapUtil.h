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
// 2013/01/22 Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapMgr_BitmapUtil_h
#define Aos_BitmapMgr_BitmapUtil_h

#include "API/AosApiG.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/HashUtil.h"


class AosBitmapUtil
{
public:
	typedef hash_map<const u64, bool, u64_hash, u64_cmp> secmap_t;
	typedef hash_map<const u64, bool, u64_hash, u64_cmp>::iterator secitr_t;

	typedef hash_map<const u64, AosBitmapObjPtr, u64_hash, u64_cmp> u64bmpmap_t;
	typedef hash_map<const u64, AosBitmapObjPtr, u64_hash, u64_cmp>::iterator u64bmpitr_t;

	enum
	{
		eMaxGlobalSections = 0xffff,
		eAosBmpFrontSectionPoison = 0x26345674,
		eAosBmpBackSectionPoison = 0x62345663,
		eMaxSectionsPerCube = 4096,
		eMaxIILLevel = 4,
		eMaxNodeLevel = 10			// Chen Ding, 2013/09/03
	};

public:
	AosBitmapUtil(){}
	~AosBitmapUtil(){}

	// static u64 getMemberId(const u64 &docid)
	// {
	// 	return ((u64)((docid&0xFFFFFFC000000000ULL>>16) 
	// 				| (docid&0x00000003c0000000ULL>>12) 
	// 				| (docid&0x0000000003C00000ULL>>8)
	// 				| (docid&0x0000000000003FFFULL>>0)));
	// }

	static u64 getDocId(const u64 &bitmap_id, const u64 &section_id)
	{
		return ((u64)((bitmap_id &0x0000FFFFFFFFFFFFULL<<16)
	                | (section_id&0x000000000000FFFFULL)));
	}

	static u32 getLocalSectionId(const u32 section_id);
	static u64 getGlobalSectionId(const u64 local_sid);
	static inline u64 getMaxGlobalSectionId() {return eMaxGlobalSections;}
	static u64 getMaxLocalSectionId();
		
	static bool splitBitmapToCubes(
			const AosRundataPtr &rdata, 
			const AosBitmapObjPtr &orig,
			vector<AosBitmapObjPtr> &cube_list);

	static inline void appendBlockToBuff1(
		const AosBuffPtr &buff, 
		const u32 global_sid,
		const AosBuffPtr bmp_buff)
	{
		buff->setU32(eAosBmpFrontSectionPoison);
		buff->setU32(global_sid);
		buff->appendBuffAsBinary(bmp_buff);
		buff->setU32(eAosBmpBackSectionPoison);
	}

	static inline bool nextBlockFromBuff1(
		const AosBuffPtr &buff, 
		u32 &global_sid,
		int64_t &position, 
		int64_t &len)
	{
		// This function retrieves the next block in 'buff'. 
		// 'buff' format is:
		// 	eAosBmpFrontSectionPoison		(u32)
		// 	global_sid						(u32)
		// 	length							(int)
		// 	buff							(variable length)
		// 	eAosBmpBackSectionPoison		(u32)
		// Upon success, it returns 'local_sid', 'position', 
		// and 'len'. 
		if (!buff->hasMore()) return false;
		u32 poison = buff->getU32(0);
		aos_assert_r(poison == eAosBmpFrontSectionPoison, false);
		global_sid = buff->getU32(0);
		bool rslt = buff->skipBuff(position, len);
		aos_assert_r(rslt, false);
		poison = buff->getU32(0);
		aos_assert_r(poison == eAosBmpBackSectionPoison, false);
		return true;
	}


	static bool isModifyId(const u64 bitmap_id)
	{
		static u64 lsModifyIdPrefix = AosDocid::getBitmapModifyIdPrefix();
		return ((bitmap_id & 0xffffffff00000000ULL) == lsModifyIdPrefix);
	}

	static inline int getMaxNodeLevel() {return eMaxNodeLevel;}

	static inline bool isHigherLevel(const int iil_level, const int node_level)
	{
		return (iil_level > 1 || ((iil_level == 1) && (node_level > 1)));
	}

	static inline bool isValidIILLevel(const int level)
	{
		return (level > 0 && level <= eMaxIILLevel);
	}
};

#endif

