////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.h
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef ParentCtrl_WebwallTorturer_h
#define ParentCtrl_WebwallTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"


class AosWebwallTorturer : public OmnTestPkg
{
private:
	enum CliId
	{
		eProxyAddress				= 1,
		eProxyStatus				= 2,
		eProxyShowConfig  			= 3,
		eGroupPolicyMode			= 4,
		eGroupPolicyName			= 5,
		eGroupPolicyShow			= 6,
		eGroupCreate				= 7,
		eGroupRemove				= 8,
		eGroupNaughtynesslimit		= 9
	};
	
	enum
	{
		eMaxGroupCount = 9,
		eMaxStrLen     = 16
		
	};
	
	
	
	
	struct proxy
	{
		OmnString 	ipaddr;
		unsigned int port;
		bool		isStatusOn;
	};
	
	struct group
	{
		OmnString groupId;
		OmnString mode;
		OmnString alias;
		int naughtyness;
		
		group();
		~group();
		group& operator = (const group &grp);
		void clear();
	};
	
	
	
	
	
	struct proxy			mProxyStat;
    struct group			mGroupList[eMaxGroupCount];
    int						mGroupNum;
    	
    
    
    
    
public:
	AosWebwallTorturer();
	~AosWebwallTorturer() {}

	virtual bool		start();

private:
	bool 			clitest();
	bool			runProxyAddress();
	bool			runProxyStatus();
	bool			runProxyShowConfig();
	bool			runGroupPolicyMode();
	bool			runGroupPolicyName();
	bool			runGroupPolicyShow();
	bool			runGroupCreate();
	bool			genMode(OmnString &mode);
	bool			genGroupId(OmnString &name);
	bool			runGroupRemove();
	bool			runGroupNaughty();
	bool			initGroupList();
	bool			saveToGroupList(OmnString &group_id);
	bool			delFromGroupList(const int &index);
	int 			genBadPort();
	OmnString  		genBadGroupId();
	
	
};
#endif

