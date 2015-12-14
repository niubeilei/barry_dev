//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//  This is the super class for transactions.
//
// Modification History:
//
////////////////////////////////////////////////////////////////////////////
#include "TransServer/Torturer/TransCheck.h"
#include "Porting/Sleep.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"

#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <bitset>
using namespace std;

extern char *firstAddr;
extern char *triesAddr;
extern char *clientSyncAddr;
extern char *serverSyncAddr;
extern int firstAddrSize;
extern int syncAddrSize; 
extern int semidTries;
extern int semidClient;
extern int semidServer;
extern int semidKillClient;
extern int semidKillServer;
extern int semaphoreP(int semid);
extern int semaphoreV(int semid);
extern int shareMemConnSize;
extern int cltSendConnNum;
//extern int cltSMConnSize;
//extern int svrSMConnSize;
extern u32 totalTransNum;

bool AosTransCheck::smSignal[2] = {false, false};
bool AosTransCheck::smHasCheck = false;

AosTransCheck::AosTransCheck()
{
}


AosTransCheck::~AosTransCheck()
{
}

	
void
AosTransCheck::check(const OmnString fname, const u64 &tid)
{
	volatile u16 *transAddr = (u16*)(firstAddr + tid * 2);
	if( ((int)transAddr-(int)firstAddr) > firstAddrSize )
	{
		OmnAlarm << "Error!"
				<< "; transAddr:" << (int)transAddr
				<< "; firstAddrSize:" << firstAddrSize
				<< enderr;
	}

	if(fname == "ClientNewTrans") ClientNewTrans(fname, transAddr, tid);
	else if(fname == "ClientSendToServer") ClientSendToServer(fname, transAddr, tid);
	else if(fname == "ClientRecAck") ClientRecAck(fname, transAddr, tid);
	else if(fname == "ClientRecResp") ClientRecResp(fname, transAddr, tid);
	else if(fname == "ClientSaveFinish") ClientSaveFinish(fname, transAddr, tid);
	else if(fname == "ClientRecover") ClientRecover(fname, transAddr, tid);
	else if(fname == "ServerNewTrans") ServerNewTrans(fname, transAddr, tid);
	else if(fname == "ServerSaveFinish") ServerSaveFinish(fname, transAddr, tid);
	else if(fname == "ServerSendResp") ServerSendResp(fname, transAddr, tid);
	else if(fname == "ServerSendAck") ServerSendAck(fname, transAddr, tid);
	else if(fname == "ServerRecover") ServerRecover(fname, transAddr, tid);
}


bool
AosTransCheck::ClientRecover(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	//if((*transAddr & 0b00100000000) == 0b00100000000)
	{
	//	AlarmError(fname, transAddr, tid);
	//	return true;
	}
	// in client recover. not all recover trans will mark this mark. 
	// because this statement: in TransClient.cpp::713
	// " if(mHosts[host_id]->isOverFlown(header.conn_id))    return true;"
	
	*transAddr |= 0b01000000000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ServerRecover(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00000010000) == 0b00000010000)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}
	*transAddr |= 0b10000000000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ClientNewTrans(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if(*transAddr != 0b00000000000)
	{
		AlarmError(fname, transAddr, tid);
		return false;
	}
	*transAddr = 0b00000000001;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ClientSendToServer(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	//if((*transAddr & 0b00100000000) == 0b00100000000)
	//{
	//	AlarmError(fname, transAddr, tid);
	//	return true;
	//}
	*transAddr |= 0b00000000010;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ClientRecAck(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00000000100) != 0b00000000100)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}

	*transAddr |= 0b00001000000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ClientRecResp(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00000100000) != 0b00000100000)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}

	*transAddr |= 0b00010000000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ClientSaveFinish(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00100000000) == 0b0010000000)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}

	*transAddr |= 0b00100000000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ServerNewTrans(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00000001000) == 0b00000001000)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}
	*transAddr |= 0b00000001000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ServerSaveFinish(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00000010000) == 0b00000010000)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}
	*transAddr |= 0b00000010000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ServerSendResp(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	if((*transAddr & 0b00000010000) != 0b00000010000)
	{
		AlarmError(fname, transAddr, tid);
		return true;
	}
	
	*transAddr |= 0b00000100000;
	PrintStatus(fname, transAddr, tid);
	return true;
}


bool
AosTransCheck::ServerSendAck(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	//if((*transAddr & 0b00000000100) != 0b00000000100)
	//{
	//	AlarmError(fname, transAddr, tid);
	//	return true;
	//}
	*transAddr |= 0b00000000100;
	PrintStatus(fname, transAddr, tid);
	return true;
}


void
AosTransCheck::AlarmError(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	OmnAlarm << "Error! :" << fname 
			<<  "\tTransStatus:" << *transAddr
	 	    << "\tTransId:" << tid
		 	<< "\t addr:" << (int)transAddr
	 	    << enderr;
}


void
AosTransCheck::PrintStatus(
		const OmnString &fname,
		volatile u16* transAddr,
		const u64 &tid)
{
	cout << "\n----------"
		 << ":" << fname 
	     << ";\tTransStatus:" << bitset<sizeof(u16)*8-5>(*transAddr) 
		 << "; ClientId:" << (tid >> 32)
	     << "; TransId:" << (u32)tid
		 << ";\t addr:" << (int)transAddr
	     << "-----------"
	     << endl;
	//OmnSleep(1);
}


bool
AosTransCheck::ModifyTries()
{
	// called by transClient/Tester/TransClientTester.cpp
	
	semaphoreP(semidTries);
	u32 crt_tries = *(u32 *)(triesAddr);
	if(crt_tries > totalTransNum)
	{
		OmnScreen << "------------thread stop: crt_tries" << crt_tries << endl; 
		if(smSignal[0] == false)
		{
			smSignal[0] = true;
			//kill(getppid(), SIGUSR1);
		}
		semaphoreV(semidTries);
		return false;
	}
	
	*(u32 *)(triesAddr) = crt_tries + 1;
	semaphoreV(semidTries);
	return true;
}


void
AosTransCheck::ServerSignal()
{
	// called ty transServer.
	//kill(getppid(), SIGUSR2);	
}

	
void
AosTransCheck::GetSignal(int signal)
{
	if(signal == SIGUSR1)
	{
		while(1)
		{
			cout << "Sleep...." << endl;
			OmnSleep(100);
		}
	}

	if(signal == SIGUSR2)
	{
		// called by Torturer/main.cpp 
		LastCheck();
	}
}


void
AosTransCheck::LastCheck()
{
	cout << "111111111111111" << endl;
	u16 transStatus;
	for(u32 i=0; i<totalTransNum; i++)
	{
		transStatus = *(u16*)(firstAddr + i * 2);
		//if( transStatus != 0b00000000000 &&
		//		((transStatus & 0b00000010000) != 0b00000010000 || 
		//		(transStatus & 0b00100000000) != 0b00100000000) )
		if( transStatus != 0b00000000000)
		{
			cout << "~~~~~~~~Error! GID:" << i 
	     		<< ";\tTransStatus:" << bitset<sizeof(u16)*8-5>(transStatus) 
		 		<< ";\t addr:" << (int)(firstAddr + i * 2) 
				<< endl;	
		}
	}

	cout << "2222222222222" << endl;
	for(u32 i=0; i<(u32)cltSendConnNum; i++)
	{
		char* client_conn_start_addr = clientSyncAddr + i * shareMemConnSize;
		char* server_conn_start_addr = serverSyncAddr + i * shareMemConnSize;
		u32 crt_trans_num = *(u32 *)client_conn_start_addr;
		for(u32 j=0; j<crt_trans_num; j++)
		{
			u32* client_tid_pointer = (u32 *)(client_conn_start_addr + sizeof(u32) + j * sizeof(u32));
			u32* server_tid_pointer = (u32 *)(server_conn_start_addr + sizeof(u32) + j * sizeof(u32));
			if(*client_tid_pointer != *server_tid_pointer)
			{
				cout << "~~~~~~~~~Error! "
					<< "client_connid:" << i 
					<< "; client_GID:" << *client_tid_pointer
					<< "; client_addr:" << (int)client_tid_pointer
					<< "; server_GID:" << *server_tid_pointer
					<< "; server_addr:" << (int)server_tid_pointer
					<< endl;
			}
			else
			{
				//cout << "___________ "
				//	<< "client_connid:" << i 
				//	<< "; client_GID:" << client_tid
				//	<< "; server_GID:" << server_tid << endl;
				
			}
		}
	}
}


void
AosTransCheck::ClientAddSync(u32 conn_id, u64 tid)
{
	if(conn_id >= (u32)cltSendConnNum)
	{
		OmnAlarm << "Error!" << enderr;
	}
	char* crt_conn_start_addr = clientSyncAddr + conn_id * shareMemConnSize;
	semaphoreP(semidClient);
	u32 crt_trans_num = *(u32 *)crt_conn_start_addr;
	
	u16 *transAddr = (u16*)(firstAddr + tid * 2);
	if( ((int)transAddr-(int)firstAddr) > firstAddrSize )
	{
		OmnAlarm << "Error!" << enderr;
	}
	if( (*transAddr & 0b00000000010) == 0b00000000010)
	{
		// it's by recover.
		//u32 last_send_tid = *(u32 *)(crt_conn_start_addr + sizeof(u32) + (crt_trans_num-1) * sizeof(u32));
		//if(last_send_tid == tid)
		//{
			OmnScreen << "---Check add Sync. recover. the same. TransId:" << tid << endl;
			semaphoreV(semidClient);
			return;	
		//}
	}

	*(u32 *)(crt_conn_start_addr + sizeof(u32) + crt_trans_num * sizeof(u32)) = (u32)tid;	// just save the last 4 bytes. the first 4 bytes is module_id
	*(u32 *)crt_conn_start_addr = crt_trans_num + 1;
	semaphoreV(semidClient);
		 	
	OmnScreen << "---client add sync addr:"
			<< (int)(crt_conn_start_addr + sizeof(u32) + crt_trans_num * sizeof(u32))
			<< "; conn_id:" << conn_id
			<< "; conn_start_addr:" << (int)crt_conn_start_addr
			<< "; crt_trans_num:" << crt_trans_num
			<< "; ClientId:" << (tid >> 32)
			<< "; TransId:" << (u32)tid
			<< endl;
	
	u32 cs = crt_conn_start_addr + sizeof(u32) + crt_trans_num * sizeof(u32) - clientSyncAddr;
	if( cs > (u32)syncAddrSize )
	{
		OmnAlarm << "Error!" << enderr;	
	}
}


void
AosTransCheck::ServerAddSync(u32 conn_id, u64 tid)
{
	if(conn_id >= (u32)cltSendConnNum)	OmnScreen << "!!!!!! Error!" << endl;
	
	char* crt_conn_start_addr = serverSyncAddr + conn_id * shareMemConnSize;
	semaphoreP(semidServer);
	u32 crt_trans_num = *(u32 *)crt_conn_start_addr;
	
	u16 *transAddr = (u16*)(firstAddr + tid * 2);
	if( ((int)transAddr-(int)firstAddr) > firstAddrSize )
	{
		OmnAlarm << "Error!" << enderr;
	}
	
	if((*transAddr & 0b10000000000) == 0b10000000000)
	{
		// it's by recover.
		u32 last_proc_tid = *(u32 *)(crt_conn_start_addr + sizeof(u32) + (crt_trans_num-1) * sizeof(u32));
		if(last_proc_tid == tid)
		{
			OmnScreen << "recover. the same." << endl;
			semaphoreV(semidServer);
			return;	
		}
	}

	*(u32 *)(crt_conn_start_addr + sizeof(u32) + crt_trans_num * sizeof(u32)) = (u32)tid;	// just save the last 4 bytes. the first 4 bytes is module_id
	*(u32 *)crt_conn_start_addr = crt_trans_num + 1;
	semaphoreV(semidServer);
	
	OmnScreen << "server add sync addr:" 
			<< (int)(crt_conn_start_addr + sizeof(u32) + crt_trans_num * sizeof(u32))
			<< "; conn_id:" << conn_id
			<< "; conn_start_addr:" << (int)crt_conn_start_addr
			<< "; crt_trans_num:" << crt_trans_num
			<< "; ClientId:" << (tid >> 32)
			<< "; TransId:" << (u32)tid
			<< endl;
	
	u32 cs = crt_conn_start_addr + sizeof(u32) + crt_trans_num * sizeof(u32) - serverSyncAddr;
	if( cs > (u32)syncAddrSize )
	{
		OmnAlarm << "Error!" << enderr;
	}
}


void
AosTransCheck::KillClientLocked()
{
	semaphoreP(semidKillClient);
}


void
AosTransCheck::KillClientUnLocked()
{
	semaphoreV(semidKillClient);
}


void
AosTransCheck::KillServerLocked()
{
	semaphoreP(semidKillServer);
}


void
AosTransCheck::KillServerUnLocked()
{
	semaphoreV(semidKillServer);
}

	
void
AosTransCheck::CleanTransStatus(const u64 &tid)
{
	volatile u16 *transAddr = (u16*)(firstAddr + tid * 2);
	if( ((int)transAddr-(int)firstAddr) > firstAddrSize )
	{
		OmnAlarm << "Error!"
				<< "; transAddr:" << (int)transAddr
				<< "; firstAddrSize:" << firstAddrSize
				<< enderr;
	}

	if( (*transAddr & 0b00000010000) != 0b00000010000 || 
			(*transAddr & 0b00100000000) != 0b00100000000 )
	{
		OmnAlarm << "Error!"
				<< "; transAddr:" << (int)transAddr
				<< "; ClientId:" << (tid >> 32)
				<< "; TransId:" << (u32)tid
				<< "\tTransStatus:" << *transAddr
				<< enderr;
	}

	
	cout << "\n---------- CleanTransStatus---"
		 << "; ClientId:" << (tid >> 32)
	     << "; TransId:" << (u32)tid
	     << "\tTransStatus:" << bitset<sizeof(u16)*8-5>(*transAddr) 
		 << ";\t addr:" << (int)transAddr
	     << "-----------"
	     << endl;
	*transAddr = 0b00000000000;
}

