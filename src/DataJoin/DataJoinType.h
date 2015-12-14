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
#ifndef Aos_DataJoin_DataJoinType_h
#define Aos_DataJoin_DataJoinType_h


#define AOSDATAJOIN_CPNTOWNCODE		"cpntowncode"
#define AOSDATAJOIN_ENGINE			"engine"
#define AOSDATAJOIN_NET				"net"
#define AOSDATAJOIN_NORM			"norm"
#define AOSDATAJOIN_ONLINE			"online"
#define AOSDATAJOIN_RANK			"rank"
#define AOSDATAJOIN_CELLFLUCTUATE	"cell_fluctuate"
#define AOSDATAJOIN_CELLWORK		"cell_work"
#define AOSDATAJOIN_CRI				"cri"
#define AOSDATAJOIN_CRIDAY			"cri_day"
#define AOSDATAJOIN_PREFIXADDMOD    "prefixaddmod"
#define AOSDATAJOIN_CARRIER			"carrier"
#define AOSDATAJOIN_UNICOM_IMEI		"unicom_imei"
#define AOSDATAJOIN_CONVERT			"convert"


class AosDataJoinType 
{
public:
	enum E
	{
		eInvalid,

		eCpnTownCode,
		eEngine,
		eNet,
		eNorm,
		eOnLine,
		eRank,
		eCellFluctuate,
		eCellWork,
		eCRI,
		eCRIDay,
		ePrefixAddMod,
		eCarrier,
		eUnicomIMEI,
		eConvert,

		eMax
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	
	static E toEnum(const OmnString &name)
	{
		if (name == AOSDATAJOIN_CPNTOWNCODE) return eCpnTownCode;
		if (name == AOSDATAJOIN_ENGINE) return eEngine;
		if (name == AOSDATAJOIN_NET) return eNet;
		if (name == AOSDATAJOIN_NORM) return eNorm;
		if (name == AOSDATAJOIN_ONLINE) return eOnLine;
		if (name == AOSDATAJOIN_RANK) return eRank;
		if (name == AOSDATAJOIN_CELLFLUCTUATE) return eCellFluctuate;
		if (name == AOSDATAJOIN_CELLWORK) return eCellWork;
		if (name == AOSDATAJOIN_CRI) return eCRI;
		if (name == AOSDATAJOIN_CRIDAY) return eCRIDay;
		if (name == AOSDATAJOIN_PREFIXADDMOD) return ePrefixAddMod;
		if (name == AOSDATAJOIN_CARRIER) return eCarrier;
		if (name == AOSDATAJOIN_UNICOM_IMEI) return eUnicomIMEI;
		if (name == AOSDATAJOIN_CONVERT) return eConvert;
		return eInvalid;
	}
	
	static OmnString toString(const E code)
	{
		if (code == eCpnTownCode) return AOSDATAJOIN_CPNTOWNCODE;
		if (code == eEngine) return AOSDATAJOIN_ENGINE;
		if (code == eNet) return AOSDATAJOIN_NET;
		if (code == eNorm) return AOSDATAJOIN_NORM;
		if (code == eOnLine) return AOSDATAJOIN_ONLINE;
		if (code == eRank) return AOSDATAJOIN_RANK;
		if (code == eCellFluctuate) return AOSDATAJOIN_CELLFLUCTUATE;
		if (code == eCellWork) return AOSDATAJOIN_CELLWORK;
		if (code == eCRI) return AOSDATAJOIN_CRI;
		if (code == eCRIDay) return AOSDATAJOIN_CRIDAY;
		if (code == ePrefixAddMod) return AOSDATAJOIN_PREFIXADDMOD;
		if (code == eCarrier) return AOSDATAJOIN_CARRIER;
		if (code == eUnicomIMEI) return AOSDATAJOIN_UNICOM_IMEI;
		if (code == eConvert) return AOSDATAJOIN_CONVERT;
		return "";
	}
};

#endif
