////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlParser.h
//
// Modification History:
// 09/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef XmlParser_Util_h
#define XmlParser_Util_h

#include "Util/String.h"


// extern OmnString AosNextWord(
// 		const u8 * const data, 
// 		const char *delis,
// 		const int idx, 
// 		const int datalen,
// 		u32 &len);
extern bool AosNextQuotedStr(const u8 * const data, 
		const int datalen,
		const unsigned char quote, 
		int &start_idx, 
		int &str_start,
		int &str_len, 
		const unsigned char *exclude, 
		const int exclen);
extern int AosNextCdataWord(const u8 * const data,
        const int datalen,
        const int start_idx,
        int &word_start,
        int &len,
        bool &closed);
extern int AosNextEnWord(const u8 * const data,
		const u8 *theMap,
        const unsigned char *delis,
        const int delis_len,
        const int start_idx,
        const int datalen,
        int &word_start,
        int &len);
extern bool AosNextWord(
		char * data,
        const char *delis,
        const int delis_len,
        const int start_idx,
        const int datalen,
        int &word_start,
		char *buff,
        u32 &len, 
		int &wordlen);
extern int AosSkipWhitespaces(const u8 * const data,
        const int datalen,
        int start_idx);
#endif
