////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/ProcessMgr.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "TransBasic/Trans.h"
#include "SvrProxyMgr/IpcSvr.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/CubeProcess.h"
#include "SvrProxyMgr/BkpCubeProcess.h"
#include "Util/File.h"
#include "Util/OmnNew.h"

#include "SysMsg/TriggerResendMsg.h"	
#include "SysMsg/StartJobMgrMsg.h"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


AosProcessMgr::AosProcessMgr(
		const AosSvrProxyPtr &svr_proxy,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mSvrProxy(svr_proxy),
mAutoStartChild(false),
mAutoStartAdmin(false),
mMaxLogicPid(AOSTAG_LOGIC_PID_START),
mDiskDamagedHanded(false),
mShowLog(show_log)
{
}


AosProcessMgr::~AosProcessMgr()
{
}

bool
AosProcessMgr::config(const AosXmlTagPtr &conf)
{
	mAutoStartChild = conf->getAttrBool("auto_start", true);
	mAutoStartAdmin = conf->getAttrBool("auto_start_admin", true);
	
	if (mAutoStartChild || mAutoStartAdmin)
	{
		aos_alarm_init(0);
	}
	
	OmnThreadedObjPtr thisPtr(this, false);
	mMonitorThrd = OmnNew OmnThread(thisPtr, "ProcessMgrMonitorThrd",
		eMonitorThrdId, true, true, __FILE__, __LINE__);
	
	mHandDeathProcThrd = OmnNew OmnThread(thisPtr, "ProcessMgrProcThrd",
		eHandProcDeathThrdId, true, true, __FILE__, __LINE__);

	mTmpDir = conf->getAttrStr("tmp_data_dir", "proxy_tmp/");
	OmnApp::createDir(mTmpDir);
	if(mTmpDir != "" && mTmpDir.find('/', true) != mTmpDir.length() - 1)
	{
		mTmpDir << "/";
	}

	AosProcessMgrPtr thisptr(this, false);
	mIpcSvr = OmnNew AosIpcSvr(mTmpDir, thisptr, mShowLog);
	
	bool rslt;
	OmnString pid_fname = mTmpDir;
	pid_fname << "ChildPid";
	mPidTmpFile = OmnNew OmnFile(pid_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!mPidTmpFile || !mPidTmpFile->isGood())
	{
		mPidTmpFile = OmnNew OmnFile(pid_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
		aos_assert_r(mPidTmpFile && mPidTmpFile->isGood(), false);
	}
	else
	{
		rslt = cleanPidTmpFile();
		aos_assert_r(rslt, false);
	}

	rslt = cleanProxySockFile();
	return true;
}


bool
AosProcessMgr::start(int argc, char **argv)
{
	mIpcSvr->startListen();

	// first need start admin svr.
	AosProcessPtr proc = createAdminProcess(argc, argv);
	aos_assert_r(proc, false);
	
	startProcPriv(proc);
	mMonitorThrd->start();
	mHandDeathProcThrd->start();
	return true;
}


bool
AosProcessMgr::startProcPriv(const AosProcessPtr &proc)
{
	aos_assert_r(proc, false);
	
	proc->start();
	if(!proc->isAutoStart())	return true;
	
	bool rslt = saveToPidTmpFile(proc->getLogicPid(),
			proc->getChildPid(), proc->getType());
	aos_assert_r(rslt, false);
	return true;
}

bool
AosProcessMgr::stopProcExcludeAdmin(const int num)
{
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		if (proc->getType() == AosProcessType::eAdmin)
		{
			continue;
		}
OmnScreen << "==============stop process type: " << AosProcessType::toStr(proc->getType()) << endl;
		proc->stop(num);
	}

	itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		if (proc->getType() == AosProcessType::eAdmin)
		{
			continue;
		}
		
		bool rslt = proc->waitDeath();
		aos_assert_r(rslt, false);
	}

	itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		if (proc->getType() == AosProcessType::eAdmin)
		{
			continue;
		}
		killProcessAndCleanSock(proc->getLogicPid(), proc->getChildPid(), proc->getType());
	}

	return true;

}


bool
AosProcessMgr::stopAll()
{
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		proc->stop(14);
	}

	itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		
		bool rslt = proc->waitDeath();
		aos_assert_r(rslt, false);
	}

	itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		killProcessAndCleanSock(proc->getLogicPid(), proc->getChildPid(), proc->getType());
	}

	mIpcSvr->stop();
	return true;
}


bool
AosProcessMgr::killAll()
{
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		proc->stop(9);
	}
	
	return true;
}


bool
AosProcessMgr::cleanPidTmpFile()
{
	aos_assert_r(mPidTmpFile, false);

	bool rslt;
	u32 file_len = mPidTmpFile->getLength();
	if(file_len == 0)	return true;

	AosBuffPtr cont;
	rslt = mPidTmpFile->readToBuff(cont, file_len + 1);
	aos_assert_r(rslt, false);

	//set<int> killed_pids;
	while(cont->getCrtIdx() < cont->dataLen())
	{
		u32 old_logic_pid = cont->getU32(0);
		int old_child_pid = cont->getInt(0);
		AosProcessType::E ptype = (AosProcessType::E)(cont->getInt(0));
		if(old_logic_pid == 0)	continue;		// 0 is svr_proxy.
		aos_assert_r(old_child_pid > 0, false);

		rslt = killProcessAndCleanSock(old_logic_pid, old_child_pid, ptype);
		aos_assert_r(rslt, false);

		rslt = cleanConfig(old_logic_pid);
		aos_assert_r(rslt, false);
		//killed_pids.insert(old_child_pid);
	}
	
	if(mShowLog)
	{
		OmnScreen << "reset pid_tmp file: "
			<< mPidTmpFile->getFileName() << endl;
	}
	mPidTmpFile->resetFile();

	// Ketty tmp. wait this killed svr death.
	OmnScreen << "Ketty Tmp wait this killed child proc death." << endl;
	OmnSleep(1);
	return true;
}


bool
AosProcessMgr::cleanProxySockFile()
{
	OmnString proxy_sock_fname = mTmpDir;
	proxy_sock_fname << "SvrProxy.sock";
	if(mShowLog)
	{
		OmnScreen << "remove proxy sock file: "
			<< proxy_sock_fname << endl;
	}
	
	int rs = unlink(proxy_sock_fname.data());
	if(rs == -1)
	{
		OmnScreen << "maybe sock_fname not exist: " << proxy_sock_fname 
			<< "; errno:" << errno << endl;
	}
	return true;
}


bool
AosProcessMgr::killProcessAndCleanSock(
		const u32 logic_pid,
		const int child_pid,
		const AosProcessType::E &type)
{
	if (child_pid == 0) return true;
	aos_assert_r(child_pid > 0, false);

	//if(mShowLog)
	{
		OmnScreen << "kill child pid: " << child_pid
			<< "; logic_pid:" << logic_pid 
			<< "; remove sock and conf file." << endl;
	}
	
	int rs = kill(child_pid, 9);
	if(rs == -1)
	{
		OmnScreen << "maybe proc not exist! child pid:" << child_pid
			<< "; errno:" << errno << endl;
	}

	// clean the sock file.
	OmnString sock_fname = mTmpDir;
	sock_fname << child_pid << "_" << logic_pid << "_" << AosProcessType::toStr(type) << ".sock";
	rs = unlink(sock_fname.data());
	if(rs == -1)
	{
		OmnScreen << "maybe sock_fname not exist:" << sock_fname 
			<< "; errno:" << errno << endl;
	}
	return true;
}


bool
AosProcessMgr::cleanConfig(const u32 logic_pid)
{
	// clean the conf file.
	OmnString conf_fname = mTmpDir;
	conf_fname << logic_pid << "_config.txt";
	int rs = unlink(conf_fname.data());
	if(rs == -1)
	{
		OmnScreen << "maybe conf_fname not exist:" << conf_fname 
			<< "; errno:" << errno << endl;
	}

	return true;
}

	
bool
AosProcessMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u32 thread_id = thread->getLogicId();
	if(thread_id == eMonitorThrdId)
	{
		return monitorThrdFunc(state, thread);
	}
	
	if(thread_id == eHandProcDeathThrdId)
	{
		return handProcDeathThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosProcessMgr::monitorThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		monitorAutoStartChild();
		monitorNoAutoStartChild();
		
		OmnSleep(1);
	}
	return true;
}


bool
AosProcessMgr::monitorAutoStartChild()
{
	int child_pid = waitpid(-1, NULL, WNOHANG);
	if(child_pid == -1)	 return true;		 // maybe all child death no child.
	if(child_pid == 0)	return true;		// no child death.
	
	OmnScreen << "has child death:" << child_pid << endl;
	AosProcessPtr proc = getProcByChildPid(child_pid);	
	aos_assert_r(proc, false);

	mLock->lock();
	monitorChildDeathPriv(proc);
	mLock->unlock();
	return true;
}


bool
AosProcessMgr::monitorNoAutoStartChild()
{
	bool death;
	mLock->lock();
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		if(proc->isAutoStart())	continue;
		if(proc->isDeath())		continue;
		if(!proc->isNeedMonitor())	continue;

		death = (kill(proc->getChildPid(), 0) == 0) ? false : true;
		if(!death)	continue;
		
		proc->setNeedMonitor(false);
		monitorChildDeathPriv(proc);
	}
	mLock->unlock();
	return true;
}


bool
AosProcessMgr::monitorChildDeathPriv(const AosProcessPtr &proc)
{
	aos_assert_r(proc, false);
	//proc->setDeath();
	//if(proc->isProActiveKill())	return true;
	
	OmnScreen << "<ProxyMsg> ProcessMgr local proc death; "
		<< "proc_id:" << proc->getLogicPid()<< "; "
		<< "proc_type:" << proc->getStrType()
		<< endl;
	
	mDeathedReqs.push(proc);
	mCondVar->signal();
	return true;
}


AosProcessPtr
AosProcessMgr::getProcByChildPid(const int child_pid)
{
	mLock->lock();
	AosProcessPtr proc;
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	for(; itr != mChildProcess.end(); itr++)
	{
		proc = itr->second;	
		aos_assert_rl(proc, mLock, 0);
		int crt_child_pid = proc->getChildPid();
		if(crt_child_pid != child_pid)
		{
			continue;
		}
		mLock->unlock();
		return proc;
	}
	mLock->unlock();
	return 0;
}


bool
AosProcessMgr::handProcDeathThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mDeathedReqs.empty())
		{
			mCondVar->wait(mLock);	
			mLock->unlock();
			continue;
		}
		
		AosProcessPtr proc = mDeathedReqs.front(); 
		mDeathedReqs.pop();
		mLock->unlock();
		
		proc->handProcDeath();
	}
	return true;
}



AosProcessPtr
AosProcessMgr::createAdminProcess(int argc, char** argv)
{
	u32 logic_pid = AOSTAG_ADMIN_PID;
	
	AosProcessMgrPtr thisptr(this, false);
	AosProcessPtr proc = AosProcess::createAdminProcStatic(thisptr, mSvrProxy, logic_pid, 
			mAutoStartAdmin, mTmpDir, mShowLog, argc, argv);

	bool rslt = addProcessPriv(logic_pid, proc);
	aos_assert_r(rslt, 0);	
	return proc;
}


AosProcessPtr
AosProcessMgr::createProcess(
		const ProcInfo &proc_conf,
		const OmnString &args, 
		const bool auto_start)
{
	aos_assert_r(mChildNormConf, 0);
	
	u32 logic_pid = allocLogicPid(); 
	aos_assert_r(logic_pid >= AOSTAG_LOGIC_PID_START, 0);

	AosProcessMgrPtr thisptr(this, false);
	AosProcessPtr proc = AosProcess::createProcStatic(thisptr, mSvrProxy, 
			logic_pid, proc_conf, auto_start, mTmpDir, mShowLog);
	
	bool rslt = addProcessPriv(logic_pid, proc);
	aos_assert_r(rslt, 0);

	AosXmlTagPtr clone_norm_config = mChildNormConf->clone(AosMemoryCheckerArgsBegin);
	rslt = proc->createConfig(clone_norm_config);
	aos_assert_r(rslt, 0);
	proc->setArgs(args);
	return proc;
}

bool
AosProcessMgr::addProcessPriv(const u32 logic_pid, const AosProcessPtr &proc)
{
	aos_assert_r(proc, false);
	mLock->lock();
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.find(logic_pid);
	aos_assert_rl(itr == mChildProcess.end(), mLock, false);
	mChildProcess.insert(make_pair(logic_pid, proc));
	mLock->unlock();
	
	return true;
}


u32
AosProcessMgr::allocLogicPid()
{
	mLock->lock();
	u32 logic_pid = mMaxLogicPid++;
	mLock->unlock();

	return logic_pid;
}


bool
AosProcessMgr::recvNewConnSock(
		const OmnString &id,
		const u32 logic_pid,
		const int child_pid,
		const int conn_sock,
		const AosProcessType::E ptype)
{
	aos_assert_r(logic_pid> 0 && conn_sock > 0, false);
	
	AosProcessPtr proc = getProcByLogicPid(logic_pid);
	aos_assert_r(proc, false);
	
	OmnScreen << "<ProxyMsg> ProcessMgr local proc up; recv conn; "
		<< "; logic_pid:" << logic_pid
		<< "; child_pid:" << child_pid
		<< "; proc_type:" << proc->getStrType()
		<< "; sock:" << conn_sock
		<< endl; 

	bool rslt = proc->recvNewConnSock(id, child_pid, conn_sock);
	aos_assert_r(rslt, false);
	
	//if(!mAutoStartChild)
	if(needSaveToTmpFile(logic_pid))
	{
		rslt = saveToPidTmpFile(logic_pid, child_pid, ptype);
		aos_assert_r(rslt, false);
	}
	
	//rslt = localProcUp(proc);
	//aos_assert_r(rslt, false);
	return true;
}


bool
AosProcessMgr::needSaveToTmpFile(const u32 logic_pid)
{
	if(logic_pid == AOSTAG_ADMIN_PID)
	{
		return !mAutoStartAdmin;
	}
	
	return !mAutoStartChild;
}


bool
AosProcessMgr::saveToPidTmpFile(const u32 logic_pid, const int child_pid, const AosProcessType::E &type)
{
	aos_assert_r(mPidTmpFile && mPidTmpFile->isGood(), false);
	
	OmnScreen << "!! save logic_pid to PidTmpFile; "
		<< "; logic_pid:" << logic_pid
		<< "; child_pid:" << child_pid << endl;

	bool rslt;
	AosBuffPtr cont = OmnNew AosBuff(eEntrySize AosMemoryCheckerArgs);
	memset(cont->data(), 0, eEntrySize);
	
	//u32 off = logic_pid * (sizeof(u32) + sizeof(int));
	u32 off = logic_pid * eEntrySize;
	u32 file_len = mPidTmpFile->getLength();
	if(file_len > off)
	{
		int bytes_read = mPidTmpFile->readToBuff(off, eEntrySize, cont->data());
		aos_assert_r(bytes_read == eEntrySize, false);
		cont->setDataLen(eEntrySize);
		u32 old_logic_pid = cont->getU32(0);
		int old_child_pid = cont->getInt(0);
		AosProcessType::E type = (AosProcessType::E)(cont->getInt(0));
		if(old_child_pid != 0 && old_child_pid != child_pid)
		{
			// means has old_child_pid.
			rslt = killProcessAndCleanSock(old_child_pid, old_logic_pid, type);
			aos_assert_r(rslt, false);
		}
	}
	
	cont->setCrtIdx(0);
	cont->setU32(logic_pid);
	cont->setInt(child_pid);
	cont->setInt((int)(type));

	rslt = mPidTmpFile->put(off, cont->data(), cont->dataLen(), true);
	aos_assert_r(rslt, false);

	if(mShowLog)
	{
		OmnScreen << "save to tmp file"
			<< "; logic_pid:" << logic_pid
			<< "; child_pid:" << child_pid
			<< endl;
	}

	return true;
}

AosProcessType::E
AosProcessMgr::getProcType(const u32 logic_pid)
{
	AosProcessPtr proc = getProcByLogicPid(logic_pid);
	aos_assert_r(proc, AosProcessType::eInvalid);
	return proc->getType();
}


AosRecvEventHandlerPtr
AosProcessMgr::getRecvHandler()
{
	return mSvrProxy;
}

u32
AosProcessMgr::getMaxCacheSize()
{
	return mSvrProxy->getMaxCacheSize();
}

AosProcessPtr
AosProcessMgr::getProcByLogicPid(const u32 logic_pid)
{
	aos_assert_r(logic_pid>= 1, 0);
	AosProcessPtr proc;
	mLock->lock();
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.find(logic_pid);
	if(itr != mChildProcess.end())
	{
		proc = itr->second;
	}
	mLock->unlock();
	return proc;
}

AosProcessPtr
AosProcessMgr::getProcByType(const AosProcessType::E tp)
{
	aos_assert_r(tp != AosProcessType::eInvalid && tp != AosProcessType::eCube, 0);
	AosProcessPtr proc;
	bool find;

	mLock->lock();
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	for(; itr != mChildProcess.end(); itr++)
	{
		proc = itr->second;
		if(proc->getType() == tp)
		{
			find = true;
			break;
		}
	}
	mLock->unlock();
	
	if(find)	return proc;
	return 0;
}

AosProcessPtr
AosProcessMgr::getCubeProcByCubeGrpId(const u32 cube_grp_id)
{
	AosProcessPtr proc;
	
	mLock->lock();
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr crt_proc = itr->second;
		if(crt_proc->hasCubeGrpId(cube_grp_id))
		{
			proc = crt_proc;
			break;
		}
	}

	mLock->unlock();
	if(proc) aos_assert_r(proc->getType() != AosProcessType::eAdmin, 0); 
	return proc;
}


bool
AosProcessMgr::sendTrans(const AosTransPtr &trans)
{
	aos_assert_r(trans, false);	
	
	//AosProcessPtr proc = getProcByAppMsg((AosAppMsg *)trans.getPtr());
	//AosProcessPtr proc = tryGetProcByAppMsg((AosAppMsg *)trans.getPtr());
	AosProcessPtr proc = getProcByAppMsg(trans.getPtr());
	if(!proc)
	{
		// means this proc not start yet.
		OmnScreen << "send Trans Failed. proc not start yet."
			<< "trans_id:" << trans->getTransId().toString() << "; "
			<< "proc_id:" << trans->getToProcId() << "; "
			<< "proc_tp:" << trans->getToProcType()
			<< endl;
		return true;
	}
	//aos_assert_r(proc, false);
		
	if(proc->getType() == AosProcessType::eAdmin)
	{
		OmnScreen << "send msg to admin; msg:"
			<< trans->getStrType() << endl;
	}

	bool rslt = proc->sendTrans(trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosProcessMgr::sendAppMsg(
		const AosAppMsgPtr &msg, 
		bool &succ)
{
	aos_assert_r(msg, false);	
	
	AosProcessPtr proc = tryGetProcByAppMsg(msg);
	aos_assert_r(proc, false);

	bool rslt = proc->sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	if(succ)	return true;
	
	rslt = contSendAppMsg(proc, msg, succ);
	aos_assert_r(rslt, false);
	return true;
}

AosProcessPtr
AosProcessMgr::tryGetProcByAppMsg(const AosAppMsgPtr &msg)
{
	AosProcessPtr proc;
	while(1)
	{
		proc = getProcByAppMsg(msg);
		if(proc)	break;
		
		// means the proc not start yet.  // try again.
		OmnSleep(1);
	}

	return proc;
}

bool
AosProcessMgr::contSendAppMsg(
		const AosProcessPtr &proc, 
		const AosAppMsgPtr &msg, 
		bool &succ)
{
	succ = false;
	bool rslt;
	while(1)
	{
		rslt = proc->sendAppMsg(msg, succ);
		aos_assert_r(rslt, false);
		if(succ)	return true;
		
		if(proc->isDeath())	return true;

		OmnScreen << "continue send msg; msg:" << msg->getStrType() << endl;
		OmnSleep(1);
	}
	
	succ = false;
	return false;
}


AosProcessPtr
AosProcessMgr::getProcByAppMsg(const AosAppMsgPtr &msg)
{
	u32 to_pid = msg->getToProcId();
	AosProcessType::E to_proc_tp = msg->getToProcType();
	if(to_pid != 0)	return getProcByLogicPid(to_pid);
	
	aos_assert_r(to_proc_tp != AosProcessType::eInvalid, 0);
	if(to_proc_tp != AosProcessType::eCube)
	{
		if(to_proc_tp == AosProcessType::eAdmin)
		{
			OmnScreen << "send msg to admin; msg:"
				<< msg->getStrType() << endl;
		}
		return getProcByType(to_proc_tp);
	}
	
	int cube_grp_id = msg->getCubeGrpId();
	aos_assert_r(cube_grp_id >=0, 0);
	return getCubeProcByCubeGrpId(cube_grp_id);
}


bool
AosProcessMgr::removeProc(const u32 logic_pid)
{
	mLock->lock();
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.find(logic_pid);
	if (itr != mChildProcess.end())
	{
		mChildProcess.erase(itr);
	}
	mLock->unlock();
	return true;
}


bool
AosProcessMgr::killProc(const u32 logic_pid)
{
	AosProcessPtr proc = getProcByLogicPid(logic_pid);
	aos_assert_r(proc, false);
	
	bool rslt = proc->killProc();
	aos_assert_r(rslt, false);
	return true;
}

bool
AosProcessMgr::startProc(const u32 logic_pid)
{
	AosProcessPtr proc = getProcByLogicPid(logic_pid);
	aos_assert_r(proc, false);
	
	return proc->restart();
}

u32
AosProcessMgr::startProc(
		const AosProcessType::E tp,
		const u32 listener_pid,
		const OmnString &args,
		const int listen_svr_id)
{
	ProcInfo proc_conf = {tp, -1};
		
	AosProcessPtr proc = createProcess(proc_conf,
			args, mAutoStartChild);
	aos_assert_r(proc, 0);
OmnScreen << "startProc"
		  << " , listen_svr_id: " << listen_svr_id
		  << " , listener_pid: " << listener_pid
		  << endl;
	proc->setListenerPid(listener_pid);
	proc->setListenerSvrId(listen_svr_id);
	startProcPriv(proc);
	return proc->getLogicPid();
}

bool
AosProcessMgr::startDefProc()
{
	AosServerInfoPtr svr_info = AosGetSvrInfo(AosGetSelfServerId());
	aos_assert_r(svr_info, false);
	vector<ProcInfo> &total_procs = svr_info->getDefProcs();
	set<AosProcessType::E> types;
	for (u32 i=0; i < total_procs.size(); i++)
	{
		types.insert(total_procs[i].mType);
	}

	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin(); 
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		aos_assert_r(proc, false);
		if (types.find(proc->getType()) != types.end())
		{
OmnScreen << "================start process type: " << AosProcessType::toStr(proc->getType()) << endl;
			proc->start();
		}
	}
	return true;
}

bool
AosProcessMgr::startDefProc(
		const u32 listener_pid,
		const OmnString &args, 
		const AosXmlTagPtr &norm_config, 
		const AosXmlTagPtr &cluster_config)
{
	//args must be :  -s -log 2 -memorycheck 600 -config
	mDiskDamagedHanded = false;

	bool rslt = initChildNormConf(norm_config, cluster_config);
	aos_assert_r(rslt && mChildNormConf, false);

	AosServerInfoPtr svr_info = AosGetSvrInfo(AosGetSelfServerId());
	aos_assert_r(svr_info, false);
	vector<ProcInfo> &total_procs = svr_info->getDefProcs();

	for(u32 i=0; i<total_procs.size(); i++)
	{
		ProcInfo proc_conf = total_procs[i];

		AosProcessPtr proc = createProcess(proc_conf,
				args, mAutoStartChild);
		aos_assert_r(proc, false);
		
		//proc->setListenerPid(listener_pid);	// finish later. need listener sid.
		startProcPriv(proc);
	}
	return true;
}

bool
AosProcessMgr::initChildNormConf(
		const AosXmlTagPtr &norm_config, 
		const AosXmlTagPtr &cluster_config)
{
	if(mChildNormConf)	return true;
	
	mChildNormConf = norm_config->clone(AosMemoryCheckerArgsBegin);

	bool add_net_conf = false;
	AosXmlTagPtr net_conf = mChildNormConf->getFirstChild("networkmgr");
	if (!net_conf)
	{
		AosXmlParser parser;
		net_conf = parser.parse("<networkmgr />", "" AosMemoryCheckerArgs);
		add_net_conf = true;
	}
	aos_assert_r(net_conf, false);
	
	AosXmlTagPtr svrs_conf = getNetworkSvrsConf(cluster_config); 
	aos_assert_r(svrs_conf, false);
	net_conf->addNode(svrs_conf);
	
	AosXmlTagPtr cubes_conf = cluster_config->getFirstChild("cubes");
	if(cubes_conf)
	{
		net_conf->addNode(cubes_conf);
		OmnString replic_policy = cubes_conf->getAttrStr("replic_policy", "");
		if (replic_policy != "")
		{
			net_conf->setAttr("replic_policy", replic_policy);
		}
	}

	if(add_net_conf)	mChildNormConf->addNode(net_conf);	
	return true;
}

AosXmlTagPtr
AosProcessMgr::getNetworkSvrsConf(const AosXmlTagPtr &cluster_config)
{
	AosXmlTagPtr svrs_conf = cluster_config->getFirstChild("servers");
	
	set<u32> exist_sids;
	AosXmlTagPtr s_conf = svrs_conf->getFirstChild(AOSCONFIG_SERVER);
	while (s_conf)
	{
		int exist_sid  = s_conf->getAttrInt("server_id", -1);
		aos_assert_r(exist_sid >=0 && (u32)exist_sid< eAosMaxServerId, 0);
	
		exist_sids.insert(exist_sid);
		s_conf = svrs_conf->getNextChild(AOSCONFIG_SERVER);
	}
	
	AosXmlTagPtr clone_svrs_conf = svrs_conf->clone(AosMemoryCheckerArgsBegin);
	vector<u32> total_sids = AosGetServerIds();
	AosXmlParser parser;
	AosXmlTagPtr new_s_conf;
	for(u32 i=0; i<total_sids.size(); i++)
	{
		u32 sid = total_sids[i];	
		if(exist_sids.find(sid) != exist_sids.end())	continue;
		
		OmnString new_s_conf_str = "<server server_id=\"";
		new_s_conf_str << sid << "\" />";
		new_s_conf = parser.parse(new_s_conf_str, "" AosMemoryCheckerArgs);
		clone_svrs_conf->addNode(new_s_conf);
	}

	return clone_svrs_conf;
}


int
AosProcessMgr::getCrtMaster(const u32 cube_grp_id)
{
	return mSvrProxy->getCrtMaster(cube_grp_id);	
}


bool
AosProcessMgr::notifyResendTrans(const AosTriggerResendMsgPtr &msg)
{
	bool succ;
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	for(; itr != mChildProcess.end(); itr++)
	{
		u32 proc_id = itr->first;
		AosProcessPtr proc = itr->second;
		
		AosConnMsgPtr c_msg = msg->copy();
		AosAppMsgPtr new_msg = (AosAppMsg *)c_msg.getPtr();
		new_msg->setToProcId(proc_id);
	
		proc->sendAppMsg(new_msg, succ);
		// if not succ. the SvrInfo will send resendTrans when wait timeout.
	}
	return true;
}


bool
AosProcessMgr::recvResendEnd(const u32 from_sid, const u32 from_pid, const u32 target_pid)
{
	AosProcessPtr proc = getProcByLogicPid(target_pid);
	aos_assert_r(proc, false);
	
	return proc->recvResendEnd(from_sid, from_pid);
}


bool
AosProcessMgr::handDiskDamaged()
{
	// need lock, maybe recv disk damaged many times.
	mLock->lock();
	if(mDiskDamagedHanded)
	{
		mLock->unlock();
		return true;
	}

	mDiskDamagedHanded = true;	
	mLock->unlock();
	OmnScreen << "Important! disk has damaged" << endl; 
	
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	for(; itr != mChildProcess.end(); itr++)
	{
		AosProcessPtr proc = itr->second;
		proc->handDiskDamage();
	}
	return true;
}


bool
AosProcessMgr::handSvrDeath(const int death_svr_id)
{
	// other svr has death. and self is judger svr.
	aos_assert_r(death_svr_id >= 0 && (u32)death_svr_id < eAosMaxServerId, false);

	bool rslt = monitorCubeSvrsDeath(death_svr_id);
	aos_assert_r(rslt, false);

	// Ketty 2014/03/07 for JobMgr Switch. self is monitor svr.
	// monitor always is the jobMgr svr.
	int self_sid = AosGetSelfServerId();
	if(death_svr_id > self_sid)	return true;
OmnScreen << "=================jozhi job start, handSvrDeath, death_svr_id: " << death_svr_id << endl;	
	rslt = startJobSvr();
	aos_assert_r(rslt, false);
	return true;
}

bool
AosProcessMgr::startJobSvr()
{
	// Ketty 2014/03/07	
	// begin start self frontend tobe bkp job svr.
OmnScreen << "===================jozhi job start" << endl;
	AosProcessPtr proc = getProcByType(AosProcessType::eFrontEnd);
	if(!proc)
	{
		OmnScreen << "<ProxyMsg> ProcessMgr start Job Svr Failed!" << endl;
		return true;
	}

	AosAppMsgPtr msg = OmnNew AosStartJobMgrMsg();

	bool succ;
	bool rslt = sendAppMsg(msg, succ);
	aos_assert_r(rslt && succ, false);	
	return true;
}


bool
AosProcessMgr::killAllTaskProcess()
{
	map<u32, AosProcessPtr>::iterator itr = mChildProcess.begin();
	while(itr != mChildProcess.end())
	{
		AosProcessPtr proc = itr->second;
		if (proc->getType() == AosProcessType::eTask || 
				proc->getType() == AosProcessType::eMsg)
		{
			proc->killProc();
		}
		itr++;
	}
	return true;
}


bool
AosProcessMgr::procDeathTriggerResend(const OmnString &reason)
{
	AosTriggerResendMsgPtr msg = OmnNew AosTriggerResendMsg(reason);
	
	notifyResendTrans(msg);
	mSvrProxy->broadcastMsg(msg);
	return true;
}

bool
AosProcessMgr::notifyProcIsUp(const int child_pid)
{
	AosProcessPtr proc = getProcByChildPid(child_pid);
	aos_assert_r(proc, false);
	bool rslt = proc->notifyListenerPidUp();
	aos_assert_r(rslt, false);
	rslt = proc->procUpTriggerResend();
	aos_assert_r(rslt, false);
	return true;
}


bool	
AosProcessMgr::isTaskProcess(const AosProcessType::E type)
{
	if (type == AosProcessType::eTask || type == AosProcessType::eService)
	{
		return true;
	}
	return false;
}
