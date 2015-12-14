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
// 06/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataAssembler_DataAssemblerType_h
#define Aos_DataAssembler_DataAssemblerType_h

#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#define AOSDATAASSEMBLER_INVALID		"invalid"

#define AOSDATAASSEMBLER_DOC_NORM		"docnorm"
#define AOSDATAASSEMBLER_DOC_VAR		"docvar"
#define AOSDATAASSEMBLER_DOC_CSV		"doccsv"

#define AOSDATAASSEMBLER_IIL_HIT		"hit"
#define AOSDATAASSEMBLER_IIL_STRADD		"stradd"
#define AOSDATAASSEMBLER_IIL_STRDEL		"strdel"
#define AOSDATAASSEMBLER_IIL_STRINC		"strinc"
#define AOSDATAASSEMBLER_IIL_STRDEC		"strdec"
#define AOSDATAASSEMBLER_IIL_U64ADD		"u64add"
#define AOSDATAASSEMBLER_IIL_U64DEL		"u64del"
#define AOSDATAASSEMBLER_IIL_U64INC		"u64inc"
#define AOSDATAASSEMBLER_IIL_U64DEC		"u64dec"

#define AOSDATAASSEMBLER_JIMOTABLE_ADD	"jimotable_add"

#define AOSDATAASSEMBLER_JIMO			"jimo"
#define AOSDATAASSEMBLER_FILE			"file"
#define AOSDATAASSEMBLER_SORT			"sort"
#define AOSDATAASSEMBLER_GROUP			"group"
#define AOSDATAASSEMBLER_BUFF			"buff"					//Barry 2015/11/17

class AosDataAssemblerType 
{
public:
	enum E
	{
		eInvalid,

		eDocNorm,
		eDocVar,
		eDocCSV,

		eHit,
		eStrAdd,
		eStrDel,
		eStrInc,
		eStrDec,
		eU64Add,
		eU64Del,
		eU64Inc,
		eU64Dec,

		eJimoTableAdd,

		eJimoDataAssembler,
		eFile,
		eSort,
		eGroup,
		eBuff, 					//Barry 2015/11/17

		eMax
	};

	static E toEnum(const OmnString &name);
	static OmnString toString(const E code);
	
	static bool isValid(const E code) {return code > eInvalid && code < eMax;}
	static bool isValid(const OmnString &name) {return isValid(toEnum(name));}
	static bool isValidDocAsmType(const OmnString &name);
	static bool isValidIILAsmType(const OmnString &name);
};

#endif
