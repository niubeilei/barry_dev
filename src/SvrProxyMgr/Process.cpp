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
#include "SvrProxyMgr/Process.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/IpcSvrInfo.h"
#include "SvrProxyMgr/AdminProcess.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"

#include "SysMsg/TriggerResendMsg.h"	
#include "SysMsg/StartProcFinishMsg.h"
#include "SysMsg/ProcDeathMsg.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

extern AosProcessPtr  sgProc[AosProcessType::eMax];

AosProcess::AosProcess(
		const AosProcessType::E tp,
		const OmnString &name,
		const bool reg_flag)
:
mType(tp)
{
	if(reg_flag)
	{
		AosProcessPtr thisptr(this, false);
		registerProc(thisptr, name);
	}
}

AosProcess::AosProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const AosProcessType::E proc_tp,
		const OmnString &exe_name,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mType(proc_tp),
mExePath(exe_name),
mLogicPid(logic_pid),
mChildPid(0),
mListenerPid(0),
mSvrProxy(proxy),
mProcMgr(proc_mgr),
mProActiveKill(false),
mAutoStart(auto_start),
mShowLog(show_log),
mTmpDir(tmp_dir),
mNeedMonitor(true),
mDiskError(false)
{
	init();
}


AosProcess::~AosProcess()
{
}


bool
AosProcess::registerProc(const AosProcessPtr &proc, const OmnString &name)
{
	aos_assert_r(AosProcessType::isValid(proc->mType), false);
	
	if(!sgProc[proc->mType])
	{
		sgProc[proc->mType] = proc;
		//AosProcessType::addName(name, proc->mType);
	}
	return true;
}

AosProcessPtr
AosProcess::createProcStatic(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const bool auto_start,
		const OmnString tmp_dir,
		const bool show_log)
{
	AosProcessType::E tp = proc_conf.mType;
	aos_assert_r(tp != AosProcessType::eAdmin, 0);
	
	aos_assert_r(sgProc[tp], 0);
	
	AosProcessPtr proc = sgProc[tp]->clone(proc_mgr, proxy,
			logic_pid, proc_conf, tmp_dir, auto_start, show_log);
	return proc;
}


AosProcessPtr 
AosProcess::createAdminProcStatic(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const bool auto_start,
		const OmnString tmp_dir,
		const bool show_log,
		int argc, 
		char **argv)
{
	AosProcessPtr proc = OmnNew AosAdminProcess(proc_mgr, proxy,
			logic_pid, tmp_dir, auto_start, argc, argv, show_log);
	return proc;
}


bool
AosProcess::init()
{
	aos_assert_r(mExePath != "", false);
	OmnString real_path = OmnApp::getAppDir();
	real_path << "/" << mExePath;
	mExePath = real_path;
	int rslt = access(mExePath.data(), 0);
	if(rslt != 0)
	{
		OmnAlarm << "process path not exist!" << mExePath 
			<< enderr;
		return false;
	}

	AosProcessPtr thisptr(this, false);
	AosRecvEventHandlerPtr recv_hd = mProcMgr->getRecvHandler();
	u32 max_cache_size = mProcMgr->getMaxCacheSize();
	
	mSvr = OmnNew AosIpcSvrInfo(thisptr, max_cache_size, recv_hd, mShowLog);
	return true;
}


OmnString
AosProcess::getExeName()
{
	int pos = mExePath.find('/', true);
	if(pos == -1)	return mExePath;
	
	return mExePath.substr(pos + 1);
}


bool
AosProcess::start()
{
	mProActiveKill = false;
	mDiskError = false;

	if (mProcMgr->isTaskProcess(mType))
	{
		mAutoStart = true;
	}
	if(!mAutoStart)
	{
		OmnScreen << "run command (" << AosProcessType::toStr(mType) << "): "
			<< "; logic_pid: " << mLogicPid 
			<< ": exe_path: \n" << mExePath << "\n"
			<< " " << getArgs() << endl;
		return true;
	}

	mChildPid = fork();
	if(mChildPid < 0)
	{
		OmnAlarm << "fork error" << enderr;
		return false;
	}
	
	if(mChildPid == 0)
	{
		startProcess();
	}

	cout << "--------------"
		<< " logic_pid:" << mLogicPid
		<< "; child pid:" << mChildPid << endl;
	return true;
}


bool
AosProcess::startProcess()
{
	cout << "execv(): " << mExePath << " " << getArgs() << endl;
	//OmnScreen << "execv(): ./" << mExePath << " " << getArgs() << endl;
	bool rslt = execv(mExePath.data(), mArgs);
	if(rslt == -1)
	{
		OmnAlarm << "execlp error!" 
			<< "; errno:" << errno << enderr;
		exit(0);
	}
	return true;
}


bool
AosProcess::stop(const int num)
{
	// maybe this proc not start.
	if(!mChildPid)	return true;
	
	OmnScreen << "------to stop child proc:" << mChildPid << endl;
	killProc(num);
	return true;
}


bool
AosProcess::recvNewConnSock(const OmnString &id, const int child_pid, const int sock_id)
{
	aos_assert_r(sock_id > 0 && child_pid > 0, false);

	bool rslt;
	mLock->lock();
	mChildPid = child_pid;

	mSvr->setName(id);
	rslt = mSvr->setConnSock(sock_id);
	aos_assert_rl(rslt, mLock, false);

	rslt = mSvr->svrIsUp();
	aos_assert_rl(rslt, mLock, false);
	
	mNeedMonitor = true;
	mLock->unlock();
	
	rslt = handProcUp();
	aos_assert_r(rslt, false);
	return true;
}

bool
AosProcess::handProcUp()
{
	//Jozhi 2015-04-15,
	//bugs : proc up, this process may not be used
	//if this process's main.cpp run to "All Service Started",
	//send AppMsg to the ProcessMgr, this time means this process is really up
	//this function called by ProcessMgr
	//bool rslt = notifyListenerPidUp();
	//aos_assert_r(rslt, false);
	
	bool rslt = procUpTriggerResend();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosProcess::notifyListenerPidUp()
{

	if(mListenerPid == 0)	return true;
OmnScreen << "notify ListenerPidUp"
		 << " , listener_svr_id: " << mListenerSvrId
		 << " , listener_pid: " << mListenerPid
		 << endl;
	AosAppMsgPtr msg = OmnNew AosStartProcFinishMsg(
			mListenerPid, mType, mLogicPid, mChildPid, mListenerSvrId); 
	bool succ;
	bool rslt = mSvrProxy->sendAppMsg(msg, succ);
	aos_assert_r(rslt && succ, false);	

	return true;
}


bool
AosProcess::procUpTriggerResend()
{
	OmnString reason = "proc up. svr_id:";
	reason << AosGetSelfServerId() << "; proc_id:" << mLogicPid;
	AosTriggerResendMsgPtr msg = OmnNew AosTriggerResendMsg(
			reason, AosGetSelfServerId(), mLogicPid, true);
	
	mProcMgr->notifyResendTrans(msg);
	return mSvrProxy->broadcastMsg(msg);
}


bool
AosProcess::sendAppMsg(const AosAppMsgPtr &msg, bool &succ)
{
	return mSvr->sendAppMsg(msg, succ);
}

bool
AosProcess::sendTrans(const AosTransPtr &trans)
{
	return mSvr->sendTrans(trans);
}


bool
AosProcess::createConfig(const AosXmlTagPtr &norm_config)
{
	aos_assert_r(norm_config, false);
	
	bool rslt = setNetworkConf(norm_config);
	aos_assert_r(rslt, false);
	
	rslt = setBaseDirConfig(norm_config);
	aos_assert_r(rslt, false);

	rslt = setStorageDir(norm_config);
	aos_assert_r(rslt, false);
	
	rslt = setIpcCltConf(norm_config);
	aos_assert_r(rslt, false);
	
	rslt = saveConfigToFile(norm_config);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosProcess::setNetworkConf(const AosXmlTagPtr &norm_config)
{
	aos_assert_r(norm_config, false);
	
	AosXmlTagPtr net_conf = norm_config->getFirstChild("networkmgr");
	aos_assert_r(net_conf, false);

	net_conf->setAttr("zky_procid", mLogicPid);
	net_conf->setAttr("zky_phyclientid", AosGetSelfClientId());
	net_conf->setAttr("zky_physerverid", AosGetSelfServerId());
	net_conf->setAttr("proc_type",AosProcessType::toStr(mType));

	AosXmlTagPtr site_mgr_tag = norm_config->getFirstChild("sitemgr");
	aos_assert_r(site_mgr_tag, false);
	AosXmlTagPtr site_tags = site_mgr_tag->getFirstChild("sites");
	aos_assert_r(site_tags, false);
	AosXmlTagPtr site_tag = site_tags->getFirstChild();
	aos_assert_r(site_tag, false);
	site_tag->setAttr("create", (AosGetSelfServerId() == 0)?"true":"false");
	return true;	
}


bool
AosProcess::setIpcCltConf(const AosXmlTagPtr &norm_config)
{
	AosXmlTagPtr ipc_clt_conf = norm_config->getFirstChild("IpcClt");
	bool add = false;
	if (!ipc_clt_conf)
	{
		AosXmlParser parser;
		ipc_clt_conf = parser.parse("<IpcClt/>", "" AosMemoryCheckerArgs);
		add = true;
	}
	aos_assert_r(ipc_clt_conf, false);
	
	//char bb[256];
	//OmnString full_path = getcwd(bb, 256);
	//full_path << "/" << mTmpDir;

	//ipc_clt_conf->setAttr("tmp_data_dir", full_path);
	ipc_clt_conf->setAttr("tmp_data_dir", mTmpDir);
	if(add) norm_config->addNode(ipc_clt_conf);	
	return true;
}


bool
AosProcess::setStorageDir(const AosXmlTagPtr &config)
{
	//StorageMgr config:
	//<storage_mgr>
    //	<storage_device>
    //		<storage_partition device_id="0" userdir="/home/jozhi/Jimo_Servers0/Jimo_Data/Data0" totalsize="100000">
    //			<storage_policy/>
    //		</storage_partition>
    //		<storage_partition device_id="1" userdir="/home/jozhi/Jimo_Servers0/Jimo_Data/Data1" totalsize="100000">
    //			<storage_policy/>
    //		</storage_partition>
    //	</storage_device>
    //  <storage_space_alloc/>
    //</storage_mgr>
	AosXmlTagPtr storage_mgr_tag = config->getFirstChild("storage_mgr");
	aos_assert_r(storage_mgr_tag, false);
	AosXmlTagPtr storage_devs_tag = storage_mgr_tag->getFirstChild("storage_device");
	aos_assert_r(storage_devs_tag, false);
	AosXmlTagPtr part_tag = storage_devs_tag->getFirstChild(true);
	while(part_tag)
	{
		OmnString userdir = part_tag->getAttrStr("userdir", "");
		aos_assert_r(userdir != "", false);
		if (mProcMgr->isTaskProcess(mType))
		{
			userdir << "/" << AosProcessType::toStr(AosProcessType::eFrontEnd);
		}
		else
		{
			userdir << "/" << AosProcessType::toStr(mType);
		}
		part_tag->setAttr("userdir", userdir);
		part_tag = storage_devs_tag->getNextChild();
	}
	return true;
}


	
bool
AosProcess::setBaseDirConfig(const AosXmlTagPtr &config)
{
	//Jozhi 2015-04-20 base dir is the first partion define "userdir"
	/*
	aos_assert_r(config, false);
	OmnString base_dir = config->getAttrStr("base_dir");
	if (base_dir == "")
	{
		OmnString basedir = OmnApp::getAppBaseDir();
		basedir.setLength(basedir.length()-1);
		base_dir = basedir.subString(0, basedir.find('/', true));
	}
	base_dir << "/" << getStrBaseDir(); 
	config->setAttr("base_dir", base_dir);
	OmnApp::createDir(base_dir);
	mBaseDir = base_dir;
	return true;
	*/
	AosXmlTagPtr storage_mgr_tag = config->getFirstChild("storage_mgr");
	aos_assert_r(storage_mgr_tag, false);

	AosXmlTagPtr storage_devs_tag = storage_mgr_tag->getFirstChild("storage_device");
	aos_assert_r(storage_devs_tag, false);

	AosXmlTagPtr part_tag = storage_devs_tag->getFirstChild(true);
	aos_assert_r(part_tag, false);
	OmnString userdir = part_tag->getAttrStr("userdir", "");
	aos_assert_r(userdir != "", false);
	if (mProcMgr->isTaskProcess(mType))
	{
		userdir << "/" << AosProcessType::toStr(mType) << "_" << mLogicPid;
	}
	else
	{
		userdir << "/" << AosProcessType::toStr(mType);
	}
	config->setAttr("base_dir", userdir);
	OmnApp::createDir(userdir);
	mBaseDir = userdir;
	return true;
}

bool
AosProcess::saveConfigToFile(const AosXmlTagPtr &norm_config)
{
	mConfigPath = mTmpDir;
	if (mProcMgr->isTaskProcess(mType))
	{
		mConfigPath << AosProcessType::toStr(mType) << "_" << mLogicPid << "_config.txt";
	}
	else if (mType == AosProcessType::eMsg || 
			 mType == AosProcessType::eIndexEngine ||
			 mType == AosProcessType::eDocEngine ||
			 mType == AosProcessType::eDocStore)
	{
		mConfigPath << AosProcessType::toStr(mType) << "_config.txt";
	}
	else
	{
		mConfigPath << mLogicPid << "_config.txt";
	}
	OmnFilePtr conf_file = OmnNew OmnFile(mConfigPath, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(conf_file && conf_file->isGood(), false);
	conf_file->put(0, (char *)(norm_config->getData()), norm_config->getDataLength(), true);
	return true;
}


void
AosProcess::setArgs(const OmnString &args)
{
	AosStrSplit split(args.data(), " ");
	vector<OmnString> argv = split.entriesV();
	mArgsNum = getArgsNum(argv.size());
	mArgs = OmnNew char*[mArgsNum];
	memset(mArgs, 0, sizeof(char*) * mArgsNum);
	
	OmnString exe_name = getExeName();
	aos_assert(exe_name != "");
	mArgs[0] = (char *)exe_name.data();
	
	mCrtArgsIdx = 1;
	for(u32 i = 0; i<argv.size(); i++)
	{
		if(strcmp(argv[i].data(), "-config") == 0)
		{
			i++;
			continue;
		}
		if(strcmp(argv[i].data(), "-base_dir") == 0)
		{
			i++;
			continue;
		}
		if(strcmp(argv[i].data(), "-tmp_data_dir") == 0)
		{
			i++;
			continue;
		}
		if(strcmp(argv[i].data(), "-rdcout") == 0)
		{
			continue;
		}
	
		mArgs[mCrtArgsIdx] = OmnNew char[argv[i].length() + 1];
		strcpy(mArgs[mCrtArgsIdx], argv[i].data());
		mCrtArgsIdx++;
	}
	
	addNewArgs();
	aos_assert(mCrtArgsIdx <= mArgsNum);
	mArgs[mCrtArgsIdx] = 0;
}


u32
AosProcess::getArgsNum(const u32 pp_args_num)
{
	return pp_args_num + 4;
}

void
AosProcess::addNewArgs()
{
	//mArgs[mCrtArgsIdx] = (char *)"-config";
	OmnString tmp = "-config";
	mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], tmp.data());
	mCrtArgsIdx++;

	//char bb[256];
	//OmnString full_path = getcwd(bb, 256);
	//full_path << "/" << mConfigPath;
	//mArgs[mCrtArgsIdx] = OmnNew char[full_path.length() + 1];
	//strcpy(mArgs[mCrtArgsIdx], full_path.data());
	mArgs[mCrtArgsIdx] = OmnNew char[mConfigPath.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], mConfigPath.data());
	mCrtArgsIdx++;	
	
	if(mAutoStart)
	{
		//mArgs[mCrtArgsIdx] = (char *)("-alarm");
		tmp = "-alarm";
		mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1];
		strcpy(mArgs[mCrtArgsIdx], tmp.data());
		mCrtArgsIdx++;
	}
}

OmnString
AosProcess::getArgs()
{
	OmnString s;
	for (u32 i=1; i< mCrtArgsIdx; i++)
	{
		s << mArgs[i] << " ";
	}
	return s;	
}


bool
AosProcess::recvResendEnd(const u32 from_sid, const u32 from_pid)
{
	return mSvr->recvResendEnd(from_sid, from_pid);
}


bool
AosProcess::killProc(const int num)
{
	if(mChildPid == 0 || isDeath())	return true;

	OmnScreen << "kill proc"
		<< "; logic_pid:" << mLogicPid 
		<< endl;
	
	mProActiveKill = true;
	kill(mChildPid, num);
	waitDeath();
	return true;
}


bool
AosProcess::waitDeath()
{
	// check death is by processMgr.
	// if child is auto start. only parent wait it.
	// otherwise,  this child will be zombie proc.
	
	int  i = 60;
	bool death;
	while(i--)
	{
		death = isDeath();
		if(death) return true;

		OmnSleep(1);
		OmnScreen << "=== svr not death: "
			<< "; logic_pid:" << mLogicPid 
			<< "; child_pid:" << mChildPid << endl; 
	}

	//Jozhi 2014-03-06	It's not Alarm
	OmnScreen << "proc not death. but ipc conn death."
		<< "; logic_pid:" << mLogicPid 
		<< "; child_pid:" << mChildPid
		<< endl;
	//OmnAlarm << "proc not death. but ipc conn death."
	//OmnScreen << "force to kill child_pid:" << mChildPid 
	//	<< "; logic_pid:" << mLogicPid 
	//	<< endl;
	//kill(mChildPid, 9); 
	return true;
}


bool
AosProcess::restart()
{
	OmnScreen << "restart proc"
		<< "; logic_pid:" << mLogicPid 
		<< endl;
	
	aos_assert_r(mSvr->isConnGood(), false);

	resetArgs();
	
	bool rslt = start();
	aos_assert_r(rslt, false);
	return true;
}

bool
AosProcess::setDeath()
{
	OmnScreen << "recv child death;"
		<< "; logic_pid:" << mLogicPid 
		<< "; child_pid:" << mChildPid 
		<< "; proc_type:" << getStrType()
		<< endl; 
	aos_assert_r(!isDeath(), false);
	mSvr->setSvrDeath();
	return true;
}


bool
AosProcess::handProcDeath()
{
	OmnScreen << "jozhi handProcDeath: "
			  << "; logic_pid:" << mLogicPid 
			  << "; child_pid:" << mChildPid 
			  << "; proc_type:" << getStrType()
			  << endl;
	setDeath();
	OmnScreen << "jozhi proc is Death" << isDeath() << endl;
	if(mProActiveKill)	return true;
	
	bool rslt = notifyListenerPidDeath();
	aos_assert_r(rslt, false);
	
	OmnString reason = "proc death. svr_id:";
	reason << AosGetSelfServerId() << "; proc_id:" << mLogicPid;
	return mProcMgr->procDeathTriggerResend(reason);
}


bool
AosProcess::notifyListenerPidDeath()
{
	if(mListenerPid == 0)	return true;
	
	OmnScreen << "send proc death:"
		<< "; listen_pid:" << mListenerPid 
		<< "; logic_pid:" << mLogicPid 
		<< endl;  
	
	u32 self_sid = AosGetSelfServerId();
	AosAppMsgPtr msg = OmnNew AosProcDeathMsg(mType, mListenerSvrId, 
			mListenerPid, self_sid, mLogicPid); 
	bool succ;
	bool rslt = mSvrProxy->sendAppMsg(msg, succ);
	//Jozhi 2014-03-06
	//aos_assert_r(rslt && succ, false);
	if (!rslt || !succ)
	{
		OmnScreen << "send ProcDeathMsg failed, may be the listen proc has death" 
			<< mListenerPid << endl;
	}

	return true;
}


bool
AosProcess::handDiskDamage()
{
	mDiskError = true;
	bool rslt = killProc();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosProcess::isDeath()
{
	return mSvr->isDeath();
}
