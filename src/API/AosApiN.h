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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiN_h
#define AOS_API_ApiN_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocidMgrObj.h"


extern u64 AosGetNextDocid(OmnString &objid, AosRundata *rdata);
extern u64 AosNextBitmapId(
				AosRundata *rdata, 
				const u64 iilid, 
				const int node_level);

extern int AosNextEnWord(const u8 * const data, 
		const u8 *theMap,
		const unsigned char *delis,
		const int delis_len,
		const int start_idx, 
		const int datalen,
		int &word_start,
		int &len);
extern bool AosNextWordChar(char *data, const int datalen, char &ch, int &idx);
extern bool AosNextWord(
		char *data, 
		const char *delis,
		const int delis_len,
		const int start_idx, 
		const int datalen,
		int &word_start,
		char *buff,
		u32 &len, 
		int &wordlen);
extern bool AosNextQuotedStr(const u8 * const data, 
		const int datalen,
		const unsigned char quote, 
		int &crtIdx, 
		int &str_start,
		int &str_len, 
		const u8 *exclude, 
		const int exclen);
extern int AosNextCdataWord(const u8 * const data, 
		const int datalen,
		const int start_idx, 
		int &word_start,
		int &len, 
		bool &closed);
#endif
