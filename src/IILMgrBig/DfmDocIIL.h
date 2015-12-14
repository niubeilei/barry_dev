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
// 10/30/2012 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgrBig_DfmDocIIL_h
#define AOS_IILMgrBig_DfmDocIIL_h

#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"

class AosDfmDocIIL : public AosDfmDoc 
{
public:
	enum
	{
		eIILTypeOff = 0,
		eIILIDOff = eIILTypeOff + 1,
		eWordIdOff = eIILIDOff + 8,
		eNumDocsOff = eWordIdOff + 8,
		eFlagOff = eNumDocsOff + 4,
		eHitCountOff = eFlagOff + 1,
		eVersionOff = eHitCountOff + 4,
		
		eHeaderCommonInfoSize = eVersionOff + 4,	// 30 byte.
		eHeaderNormalInfoSize = 16,

		eHeaderSize = 200,		// this eHeaderSize == AosIIL::eIILHeaderSize.
	
		ePersisBitOnVersionFlag = 0x80000000,	// moved from AosIIL.
	};

public:
	static bool			smShowLog;

public:
	AosDfmDocIIL(const u64 docid);
	//AosDfmDocIIL(const u64 docid, const AosBuffPtr &header_buff);
	~AosDfmDocIIL();
	
	virtual AosDfmDocPtr clone(const u64 docid);
	virtual bool		initBodySeqOff(const AosBuffPtr &header_buff);
	virtual bool 		resetBodySeqOff(const u32 seq, const u64 offset);

	void 	setIILType(const AosIILType type);
	void 	setIILID(const u64 iilid);
	void 	setWordId(const u64 wordid);
	void 	setVersion(const u32 version, const bool is_persis);
	void 	setHitCount(const u32 hit_count);
	void 	setNumDocs(const u32 num_docs);
	void 	setFlag(const char flag);
	
	AosIILType	getIILType();
	u64		getIILID();
	u64		getWordId();
	u32		getNumDocs();
	char	getFlag();
	u32		getHitCount();
	u32		getVersion();
	bool	getPersis();
};

#endif
