//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TransTest.h
// Description:
//  This is the test class for transactions.
//
// Modification History:
// 2011/06/08	Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransServer_Torturer_TransCheck_h
#define Aos_TransServer_Torturer_TransCheck_h

#ifdef Aos_Trans_Debug
#define AosTransCheckStatus(a,b) AosTransCheck::check(a,b)
#define AosTransCheckGetSignal AosTransCheck::GetSignal
#define AosTransCheckServerSignal() AosTransCheck::ServerSignal()
#define AosTransCheckModifyTries() AosTransCheck::ModifyTries()
#define AosTransCheckClientAddSync(a,b) AosTransCheck::ClientAddSync(a,b)
#define AosTransCheckClean(a) AosTransCheck::CleanTransStatus(a)
#define AosTransCheckKillClientLock AosTransCheck::KillClientLocked() 
#define AosTransCheckKillClientUnLock AosTransCheck::KillClientUnLocked() 
#define AosTransCheckKillServerLock AosTransCheck::KillServerLocked() 
#define AosTransCheckKillServerUnLock AosTransCheck::KillServerUnLocked() 
	#ifdef Aos_Trans_Sync
	#define AosTransCheckServerAddSync(a,b) AosTransCheck::ServerAddSync(a,b)
	#else
	#define AosTransCheckServerAddSync(a,b)
	#endif	
#else
#define AosTransCheckStatus(a,b) 
#define AosTransCheckLastCheck SIG_IGN 
#define AosTransCheckServerSignal() 
#define AosTransCheckClientAddSync(a,b)
#define AosTransCheckServerAddSync(a,b)
#define AosTransCheckClean(a)  
#define AosTransCheckModifyTries() true 
#define AosTransCheckKillClientLock 
#define AosTransCheckKillClientUnLock  
#define AosTransCheckKillServerLock  
#define AosTransCheckKillServerUnLock  
#endif


#include "aosUtil/Types.h"
#include "Util/String.h"

class AosTransCheck
{
public:
	
	static bool smSignal[2];
	static bool smHasCheck;

	AosTransCheck();
	~AosTransCheck();

	static void check(const OmnString fname, const u64 &tid);
	static bool	ClientNewTrans(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ClientSendToServer(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ClientRecAck(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool ClientRecResp(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ClientSaveFinish(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ClientRecover(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);

	static bool	ServerNewTrans(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ServerSaveFinish(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ServerSendResp(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ServerSendAck(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);
	static bool	ServerRecover(
					const OmnString &fname,
					volatile u16* transAddr,
					const u64 &tid);

	static void CleanTransStatus(const u64 &tid);

	static void	AlarmError(
				const OmnString &fname,
				volatile u16* transAddr,
				const u64 &tid);
	static void	PrintStatus(
				const OmnString &fname,
				volatile u16* transAddr,
				const u64 &tid);
	static bool ModifyTries();
	static void ServerSignal();
	static void GetSignal(int signal);
	static void LastCheck();
	static void	ClientAddSync(u32 conn_id, u64 tid);
	static void	ServerAddSync(u32 conn_id, u64 tid);
	static void	KillClientLocked();
	static void	KillClientUnLocked();
	static void	KillServerLocked();
	static void	KillServerUnLocked();
};
#endif
