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
//
// Modification History:
// 05/15/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturers_XmlDocTurturerAosTesterId_TesterId_h
#define Aos_Torturers_XmlDocTurturerAosTesterId_TesterId_h

#include "Random/RandomUtil.h"
#include "Util/String.h"

#define AosSengTester_CreateObj			"createobj"
#define AosSengTester_Login				"login"
#define AosSengTester_CreateUserNew		"createusernew"
#define AosSengTester_CreateCtnr		"createctnr"
#define AosSengTester_AccessControl		"accessctrl"
#define AosSengTester_Logout			"logout"
#define AosSengTester_ModifyObj			"modifyobj"
#define AosSengTester_ModifyAttrStr		"modattr"
#define AosSengTester_DeleteObj			"deleteobj"
#define AosSengTester_GetArcd			"getacrd"
#define AosSengTester_GetDomain			"getdomain"
#define AosSengTester_ManualOrder		"manualorder"
#define AosSengTester_CreateUserDomain	"createuserdomain"
#define AosSengTester_SdocTriggering	"sdoctriggering"
#define AosSengTester_PartialDoc		"partialdoc"
#define AosSengTester_Vote				"vote"
#define AosSengTester_Operations		"operations"
#define AosSengTester_Statemachine		"statemachine"
#define AosSengTester_DomainOprArd		"domainoprard"
#define AosSengTester_UserOprArd		"useroprard"
#define AosSengTester_DocLock			"doclock"


class AosTesterId
{
public:
	enum E
	{
		eInvalid,

		eCreateObj,
		eLogin,
		eCreateUserNew,
		eCreateCtnr,
		eAccessCtrl,
		eLogout,
		eModifyObj,
		eModifyAttrStr,
		eDeleteObj,
		eGetArcd,
		eGetDomain,
		eManualOrder,
		eCreateUserDomain,
		eSdocTriggering,
		ePartialDoc,
		eVote,
		eOperations,
		eStatemachine,
		eDomainOprArd,
		eUserOprArd,
		eDocLock,

		eMax
	};

public:
	static E toEnum(const OmnString &name);
	static bool isValid(const E code) {return code > eInvalid && code < eMax;}
	static bool addName(const OmnString &name, const E code);
	static E pickTesterId()
	{
		return (E)OmnRandom::nextInt(eInvalid+1, eMax-1);
	}
};
#endif

