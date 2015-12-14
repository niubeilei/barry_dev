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
#include "DataAssembler/DataAssemblerType.h"


AosDataAssemblerType::E
AosDataAssemblerType::toEnum(const OmnString &name)
{
	const char *data = name.data();
	if (name.length() < 2) return eInvalid;
	switch (data[0])
	{
	case 'f':
		 if (name == AOSDATAASSEMBLER_FILE) return eFile;
	case 'd':
		 if (name == AOSDATAASSEMBLER_DOC_NORM) return eDocNorm;
		 if (name == AOSDATAASSEMBLER_DOC_VAR) return eDocVar;
		 if (name == AOSDATAASSEMBLER_DOC_CSV) return eDocCSV;
		 break;

	case 'h':
		 if (name == AOSDATAASSEMBLER_IIL_HIT) return eHit;
		 break;

	case 'j':
		 if (name == AOSDATAASSEMBLER_JIMOTABLE_ADD) return eJimoTableAdd;
		 if (name == AOSDATAASSEMBLER_JIMO) return eJimoDataAssembler;
		 break;

	case 's':
		 if (name == AOSDATAASSEMBLER_IIL_STRADD) return eStrAdd;
		 if (name == AOSDATAASSEMBLER_IIL_STRDEL) return eStrDel;
		 if (name == AOSDATAASSEMBLER_IIL_STRINC) return eStrInc;
		 if (name == AOSDATAASSEMBLER_IIL_STRDEC) return eStrDec;
		 break;

	case 'u':
		 if (name == AOSDATAASSEMBLER_IIL_U64ADD) return eU64Add;
		 if (name == AOSDATAASSEMBLER_IIL_U64DEC) return eU64Del;
		 if (name == AOSDATAASSEMBLER_IIL_U64INC) return eU64Inc;
		 if (name == AOSDATAASSEMBLER_IIL_U64DEC) return eU64Dec;
		 break;

	default:
		 break;
	}

	return eInvalid;
}


OmnString
AosDataAssemblerType::toString(const E code)
{
	switch (code)
	{
	case eDocNorm	: return AOSDATAASSEMBLER_DOC_NORM;
	case eDocVar	: return AOSDATAASSEMBLER_DOC_VAR;
	case eDocCSV	: return AOSDATAASSEMBLER_DOC_CSV;
	case eHit		: return AOSDATAASSEMBLER_IIL_HIT;
	case eStrAdd 	: return AOSDATAASSEMBLER_IIL_STRADD; 
	case eStrDel	: return AOSDATAASSEMBLER_IIL_STRDEL;
	case eStrInc	: return AOSDATAASSEMBLER_IIL_STRINC;
	case eStrDec	: return AOSDATAASSEMBLER_IIL_STRDEC;
	case eU64Add	: return AOSDATAASSEMBLER_IIL_U64ADD;
	case eU64Del	: return AOSDATAASSEMBLER_IIL_U64DEC;
	case eU64Inc	: return AOSDATAASSEMBLER_IIL_U64INC;
	case eU64Dec	: return AOSDATAASSEMBLER_IIL_U64DEC;
	case eJimoTableAdd : return AOSDATAASSEMBLER_JIMOTABLE_ADD;
	default :
		 break;
	}
	return AOSDATAASSEMBLER_INVALID;
}

	
bool
AosDataAssemblerType::isValidDocAsmType(const OmnString &name)
{
	E type = toEnum(name);
	return type >= eDocNorm && type <= eDocCSV;
}


bool
AosDataAssemblerType::isValidIILAsmType(const OmnString &name)
{
	E type = toEnum(name);
	return type >= eHit && type <= eU64Dec;
}

