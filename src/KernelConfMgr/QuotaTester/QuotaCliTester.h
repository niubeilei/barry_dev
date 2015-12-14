////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemCliTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef SystemCli_Tester_QuotaCliTester_h
#define SystemCli_Tester_QuotaCliTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"

#define		MAXUSER		128
#define		MAXPART		128


class QuotaCliTester : public OmnTestPkg
{
private:
	enum CliId
	{
		diskMgrQuotaStatus_ID = 1,
		diskMgrQuotaStatusShow_ID = 2,
		diskMgrQuotalist_ID = 3,
		diskMgrQuotaAddUser_ID = 4,
		diskMgrQuotaDelUser_ID = 5,
		diskMgrQuotaAddDir_ID = 6,
		diskMgrQuotaDelDir_ID= 7,
		diskMgrQuotaShowUser_ID = 8,
		diskMgrQuotaShowDir_ID = 9,
		diskMgrQuotaShow_ID = 10
	};

	//
	//there two List is init by the program start
	//

	OmnString existParts[MAXPART];
	int partNum;

	OmnString existUsers[MAXPART];
	int userNum;
	
	struct user_t
	{
		OmnString		mName;
		OmnString		size;
		
		user_t();
		~user_t();
	};
	
	struct dir_t
	{
		OmnString		mName;
		OmnString		size;
		
		dir_t();
		~dir_t();
	};
	
	
	struct partition_t
	{
		OmnString 		mName;
		int 			userNum;
		int 			dirNum;
		user_t			users[MAXUSER];
		dir_t			dirs[MAXUSER];
		OmnString 		status;
		
		partition_t();
		~partition_t();
	};

	partition_t mPart[MAXPART];
	int	mPartNum;

public:
	QuotaCliTester();
	~QuotaCliTester();

	virtual bool		start();

private:
	bool	basicTest();
	
	bool	diskMgrQuotaStatus();
	bool	diskMgrQuotaStatusShow();
	bool	diskMgrQuotaList();
	bool 	diskMgrQuotaAddUser();
	bool 	diskMgrQuotaDeleteUser();
	bool	diskMgrQuotaAddDir();
	bool	diskMgrQuotaDelDir();
	bool 	diskMgrQuotaShowUser();
	bool 	diskMgrQuotaShowDir();
	bool 	diskMgrQuotaShow();
	bool	diskMgrQuotaClearConfig();
	
	bool	quotaTorturerInit();
	bool	genPart(OmnString &partition, bool &isCorrect);
	bool 	genStatus(OmnString &status, bool &isCorrect);
	bool	genUser(OmnString &user, bool &isCorrect);
	bool	genDir(OmnString &partition, OmnString &user, bool &isCorrect);
	bool 	genSize(OmnString &size, bool &isCorrect);
	bool	saveQuotaStatus(OmnString &partition, OmnString &status);
	bool    statusIsAlready(OmnString &partition, OmnString &status);
	bool	initUserList();
	bool	initPartList();
};
	
	extern int sgCount;
#endif

