//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "JimoAgentMonitor/JimoInstall.h"
#include "JimoAgentMonitor/JimoAgentMonitor.h"
#include "XmlUtil/XmlTag.h"
#include "Util/File.h"
#include "API/AosApi.h"
#include <pwd.h>

#define AOSTAG_JIMOINSTALL_WORKHOME					"workhome"
#define AOSTAG_JIMOINSTALL_BROADCASTPORT			"broadcast-port"
#define AOSTAG_JIMOINSTALL_PORT						"port"

//script name
#define AOSTAG_JIMOINSTALL_JIMODB					"jimodb"
#define AOSTAG_JIMOINSTALL_RUN						"run"
#define AOSTAG_JIMOINSTALL_RUNHTML					"runhtml"
#define AOSTAG_JIMOINSTALL_RUNACCESS				"runaccess"
#define AOSTAG_JIMOINSTALL_RMDATA					"rmdata"
#define AOSTAG_JIMOINSTALL_RMTRANS					"rmtrans"
#define AOSTAG_JIMOINSTALL_KILLSVR					"killsvr"
#define AOSTAG_JIMOINSTALL_CHECKSO					"checkso"
#define AOSTAG_JIMOINSTALL_PORTGEN					"portgen"
#define AOSTAG_JIMOINSTALL_INSTALLSETNOPWD			"install_setnopwd"
#define AOSTAG_JIMOINSTALL_JIMODBSETNOPWD			"jimodb_setnopwd"
#define AOSTAG_JIMOINSTALL_SETNOPWD					"setnopwd"
#define AOSTAG_JIMOINSTALL_BROADCAST_PORTGEN		"broadcast_portgen"

//config name
#define AOSTAG_JIMOINSTALL_ADMIN					"config_admin.txt"
#define AOSTAG_JIMOINSTALL_NORM						"config_norm.txt"
#define AOSTAG_JIMOINSTALL_CLUSTER					"config_cluster.txt"
#define AOSTAG_JIMOINSTALL_NETWORK					"config_network.txt"
#define AOSTAG_JIMOINSTALL_PROXY					"config_proxy.txt"
#define AOSTAG_JIMOINSTALL_ACCESS					"config_access.txt"
#define AOSTAG_JIMOINSTALL_HTML						"config_html.txt"

//dirs
#define AOSTAG_JIMOINSTALL_SHAREDLIBS				"shared_libs"
#define AOSTAG_JIMOINSTALL_WEBSITE					"WebSite"
#define AOSTAG_JIMOINSTALL_SYSTEMDOCS				"systemDocs"

//directroy name
#define AOSTAG_JIMOINSTALL_JIMOSERVERS				"Jimo_Servers"
#define AOSTAG_JIMOINSTALL_JIMODATA					"Jimo_Data"
#define AOSTAG_JIMOINSTALL_PROXYTMP					"proxy_tmp"
#define AOSTAG_JIMOINSTALL_ALLCLUSTER				"all_cluster"

AosJimoInstall::AosJimoInstall()
{
}

bool
AosJimoInstall::initTpls(const AosXmlTagPtr &tpls, const AosRundataPtr &rdata)
{
	AosXmlTagPtr tag = tpls->getFirstChild();
	while(tag)
	{
		OmnString name = tag->getAttrStr("name");
		OmnString objid = tag->getNodeText();
		AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
		aos_assert_r(doc, false);
		doc = doc->clone(AosMemoryCheckerArgsBegin);
		mTpls[name] = doc;
		tag = tpls->getNextChild();
	}
	return true;
}

bool
AosJimoInstall::initIps(const AosXmlTagPtr &ips)
{
	AosXmlTagPtr tag = ips->getFirstChild();
	while(tag)
	{
		OmnString ip_str = tag->getNodeText();
		mIpList.push_back(ip_str);
		tag = ips->getNextChild();
	}
	return true;
}

bool
AosJimoInstall::clean()
{
	OmnString file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_ADMIN;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_NORM;
	unlink(file.data());
	file = mClusterHome;
	file <<  "/" << AOSTAG_JIMOINSTALL_CLUSTER;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_NETWORK;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_PROXY;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_ACCESS;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_HTML; 
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_PORTGEN;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_BROADCAST_PORTGEN;
	unlink(file.data());
	file = mClusterHome;
	file << "/" << AOSTAG_JIMOINSTALL_BROADCASTPORT;
	unlink(file.data());

	for (u32 i=0; i<mIpList.size(); i++)
	{
		OmnString port_file;
		port_file << mClusterHome << "/" << mIpList[i] << "-port";
		unlink(port_file.data());
	}
	return true;
}

bool
AosJimoInstall::init(const OmnString &cluster_name, const AosRundataPtr &rdata)
{
	mClusterName = cluster_name;
	AosXmlTagPtr doc = AosGetDocByObjid(mClusterName, rdata);
	aos_assert_r(doc, false);
	AosXmlTagPtr config = doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(config, false);
	mLocalIp = AosJimoAgentMonitor::getSelf()->getLocalIp();
	AosXmlTagPtr install_tag = config->getFirstChild("install");
	mUser = install_tag->getAttrStr("user");
	mPwd = install_tag->getAttrStr("pwd");
	mUserHome = install_tag->getAttrStr("user_home");
	if (mUserHome == "")
	{
		passwd* pa = getpwnam(mUser.data());
		mUserHome = pa->pw_dir;
	}
	mInstallHome = install_tag->getAttrStr("install_home");
	mDataHome = install_tag->getAttrStr("data_home");
	mBkpType = install_tag->getAttrStr("nobkp");
	mVirtuals = atoi(install_tag->getAttrStr("virtuals").data());
	AosXmlTagPtr tags = config->getFirstChild("ips");
	aos_assert_r(tags, false);
	bool rslt = initIps(tags);
	aos_assert_r(rslt, false);
	tags = config->getFirstChild("tpls");
	aos_assert_r(tags, false);
	rslt = initTpls(tags, rdata);
	aos_assert_r(rslt, false);
	mScripts = install_tag->getAttrStr("scripts");
	mExe = install_tag->getAttrStr("exe");
	mSharedLibs = install_tag->getAttrStr("shared_libs");
	mSystemDocs = install_tag->getAttrStr("systemDocs");
	mWebSite = install_tag->getAttrStr("WebSite");
	mClusterHome << OmnApp::getAppDir() << "/" << AOSTAG_JIMOINSTALL_ALLCLUSTER << "/" << mClusterName;
	return true;
}

bool
AosJimoInstall::jimodb(const OmnString &args)
{
	OmnString cmd;
	cmd << mClusterHome << "/" << AOSTAG_JIMOINSTALL_JIMODB << " " << args;
	command(mUser, cmd);
	return true;
}

bool
AosJimoInstall::update()
{
	//update [exe shared_libs]
	for (u32 i=0; i<mIpList.size(); i++)
	{
		OmnString ip = mIpList[i];
		OmnString workhome;
		workhome << mInstallHome << "/" << AOSTAG_JIMOINSTALL_JIMOSERVERS << i;
		OmnString file;
		file << createExe(i) << " ";
		OmnString dir;
		dir << mSharedLibs << " ";
		if (mLocalIp != ip)
		{
			OmnString scp = "scp ";
			scp << file << mUser << "@" << ip << ":" << workhome << "/";
			command(mUser, scp.data());
			scp = "scp -r ";
			scp << dir << mUser << "@" << ip << ":" << workhome << "/";
			command(mUser, scp.data());
		}
		else
		{
			OmnString cp = "cp ";
			cp << file << workhome << "/";
			command(mUser, cp.data());
			cp = "cp -r ";
			cp << dir << workhome << "/";
			command(mUser, cp.data());
		}
	}
	return true;
}

bool
AosJimoInstall::unInstall()
{
	//stop cluster
	//remove cluster
	OmnString stop;
	stop << mClusterHome << "/" << AOSTAG_JIMOINSTALL_JIMODB << " stop";
	command(mUser, stop.data());
	for (u32 i=0; i<mIpList.size(); i++)
	{
		OmnString ip = mIpList[i];
		OmnString workhome;
		workhome << mInstallHome << "/" << AOSTAG_JIMOINSTALL_JIMOSERVERS << i;
		OmnString rm = "rm -r ";
		rm << workhome;
		if (mLocalIp != ip)
		{
			OmnString remote_rm;
			remote_rm << mUser << "@" << ip << " " << rm;
			command(mUser, remote_rm.data());
		}
		else
		{
			command(mUser, rm.data());
		}
	}
	return true;
}

bool
AosJimoInstall::install()
{
	OmnString mkdir = "mkdir -p ";
	mkdir << mClusterHome;
	system(mkdir.data());
	OmnString chown = "chown -R ";
	chown << mUser << "." << mUser << " " << mClusterHome;
	system(chown.data());
	OmnString cp = "cp ";
	cp << mScripts << "/portgen" << " " 
	   << mScripts << "/broadcast_portgen" << " " 
	   << mScripts << "/jimodb" << " " 
	   << mClusterHome;
	command(mUser, cp);
	bool rslt = setNoPassword();
	aos_assert_r(rslt, false);
	OmnString workhome_list;
	OmnString net_str;
	OmnString servers_str;
	OmnString cubes_str;
	OmnString master_workhome;
	OmnString master_file;
	OmnString workhome_file;
	workhome_file << mClusterHome << "/" << AOSTAG_JIMOINSTALL_WORKHOME;
	OmnString port_file;
	port_file << mClusterHome << "/" << AOSTAG_JIMOINSTALL_PORT;
	master_file << mScripts << "/jimodb" << " "
				<< mScripts << "/setnopwd" << " "
				<< mScripts << "/jimodb_setnopwd" << " "
				<< mScripts << "/killsvr_other" << " "
				<< mScripts << "/runhtml" << " "
				<< mScripts << "/runaccess" << " "
				<< workhome_file << " "
				<< port_file << " ";
	for (u32 i=0; i<mIpList.size(); i++)
	{
		OmnString workhome;
		workhome << mInstallHome << "/" << AOSTAG_JIMOINSTALL_JIMOSERVERS << i;
		workhome_list << mIpList[i] << " " << workhome << ",\n";
		int proxy_port = getPort(i);
		net_str << createNetServerStr(i, proxy_port);
		servers_str << createServerStr(i);
		cubes_str << createCubeStr(i);
		OmnString file;
		file << createSvrProxyConfig(i, proxy_port, workhome) << " "
			 << createAdminConfig() << " "
			 << mScripts << "/run" << " "
			 << mScripts << "/checkso" << " "
			 << mScripts << "/killsvr" << " "
			 << mScripts << "/killsvr_14" << " "
			 << mScripts << "/check_stop" << " "
			 << mScripts << "/rmdata" << " "
			 << mScripts << "/rmtrans" << " "
			 << mScripts << "/showpid" << " "
			 << createExe(i) << " ";
		if (i == 0)
		{
			master_workhome = workhome;
			int thrift_port = getPort(i);
			int front_port = getPort(i);
			int access_port = getPort(i);
			int html_port = getPort(i);
			OmnString port_str;
			port_str << "thrift_port=" << thrift_port << "\n"
					 << "front_port=" << front_port << "\n"
					 << "access_port=" << access_port << "\n"
					 << "html_port=" << html_port;
			createFile(port_str, port_file);
			file << createNormConfig(thrift_port, front_port) << " "
				 << createAccessConfig(front_port, access_port, thrift_port) << " "
				 << createHtmlConfig(html_port, front_port, master_workhome) << " ";
		}
		OmnString mkdir = "mkdir -p ";
		mkdir << workhome;
		if (mLocalIp != mIpList[i])
		{
			OmnString ssh = "ssh ";
			ssh << mUser << "@" << mIpList[i] << " ";
			OmnString remote_mkdir;
			remote_mkdir << ssh << mkdir;
			command(mUser, remote_mkdir);
			OmnString scp = "scp ";
			scp << file << mUser << "@" << mIpList[i] << ":" << workhome << "/";
			command(mUser, scp);
			OmnString dir;
			dir << mSharedLibs << " ";
			scp = "scp -r ";
			scp << dir << mUser << "@" << mIpList[i] << ":" << workhome << "/";
			command(mUser, scp);
		}
		else
		{
			command(mUser, mkdir);
			OmnString cp = "cp ";
			cp << file << workhome << "/";
			command(mUser, cp);
			OmnString dir;
			dir << mSharedLibs << " ";
			cp = "cp -r ";
			cp << dir << workhome << "/";
			command(mUser, cp);
		}

	}
	int broadcast_port = getBroadcastPort();
	master_file << createNetworkConfig(broadcast_port, net_str) << " "
				<< createClusterConfig(servers_str, cubes_str) << " ";
	createFile(workhome_list, workhome_file);
	OmnString master_dir;
	master_dir << mSystemDocs << " "
			   << mWebSite << " ";
	OmnString master_ip = mIpList[0];
	if (mLocalIp != master_ip)
	{
		OmnString scp = "scp ";
		scp << master_file << mUser << "@" << master_ip << ":" << master_workhome << "/";
		command(mUser, scp);
		scp = "scp -r ";
		scp << master_dir << mUser << "@" << master_ip << ":" << master_workhome << "/";
		command(mUser, scp);
		OmnString ssh = "ssh ";
		ssh << mUser << "@" << master_ip << " ";
	}
	else
	{
		OmnString cp = "cp ";
		cp << master_file << master_workhome << "/";
		command(mUser, cp);
		cp = "cp -r ";
		cp << master_dir << master_workhome << "/";
		command(mUser, cp);
	}
	clean();
	return true;
}

OmnString
AosJimoInstall::createServerStr(const u32 svr_id)
{
	OmnString str;
	str << "<server server_id=\"" << svr_id << "\">"
		<< "<proc proc_type=\"cube\" cube_grp_id=\"" << svr_id << "\"/>"
		<< "<proc proc_type=\"frontend\"/>"
		<< "</server>";
	return str;
}

OmnString
AosJimoInstall::createNetServerStr(const u32 svr_id, const int port)
{
	OmnString str;
	str << "<server server_id=\"" << svr_id << "\" remote_addr=\"" << mIpList[svr_id] << "\" remote_port=\"" << port << "\"/>";
	return str;
}

OmnString
AosJimoInstall::createCubeStr(const u32 svr_id)
{
	OmnString vir_str;
	for(int i=0; i<mVirtuals ;i++)
	{
		vir_str << i * mVirtuals + svr_id;
		if (i != mVirtuals-1)
		{
			vir_str << ",";
		}
	}
	OmnString cube_str;
	cube_str << "<cube_grp grp_id=\"" << svr_id << "\" vids=\"" << vir_str << "\"/>";
	return cube_str;
}

bool
AosJimoInstall::setNoPassword()
{
	OmnString ip_str;
	for (u32 i=0; i<mIpList.size(); i++)
	{
		ip_str << mIpList[i] << " ";
	}
	ip_str.setLength(ip_str.length() - 1);
	OmnString cmd;
	cmd << mScripts << "/install_setnopwd" << " '" << ip_str << "' " << mPwd;
	command(mUser, cmd);
	return true;
}

OmnString
AosJimoInstall::createAdminConfig()
{
	AosXmlTagPtr tpl = mTpls["admin"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	OmnString str = config->toString();
	OmnString admin;
	admin << mClusterHome << "/" << AOSTAG_JIMOINSTALL_ADMIN;
	createFile(str, admin);
	return admin;
}

OmnString
AosJimoInstall::createNormConfig(
			const int thrift_port,
			const int front_port)
{
	AosXmlTagPtr tpl = mTpls["norm"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	config = config->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(config, "");

	//thrift_servers
	AosXmlTagPtr tag = config->getFirstChild("thrift_servers");
	aos_assert_r(tag, "");
	tag = tag->getFirstChild();
	aos_assert_r(tag, "");
	tag->setAttr("port", thrift_port);

	//seserver
	tag = config->getFirstChild("seserver");
	aos_assert_r(tag, "");
	OmnString port = "";
	port << front_port << "-" << front_port;
	tag->setAttr("local_port", port);
	OmnString str = config->toString();
	OmnString norm;
	norm << mClusterHome << "/" << AOSTAG_JIMOINSTALL_NORM;
	createFile(str, norm);
	return norm;
}

OmnString
AosJimoInstall::createNetworkConfig(const int broadcast_port, const OmnString &net_str)
{
	AosXmlTagPtr tpl = mTpls["network"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	OmnString servers;
	servers << "<servers>"
			<< net_str
			<< "</servers>";
	AosXmlTagPtr tag = AosXmlParser::parse(servers AosMemoryCheckerArgs);
	aos_assert_r(tag, "");
	tag->setAttr("broadcast_port", broadcast_port);
	config->addNode(tag);
	OmnString str = config->toString();
	OmnString network;
	network << mClusterHome << "/" << AOSTAG_JIMOINSTALL_NETWORK;
	createFile(str, network);
	return network;
}

OmnString
AosJimoInstall::createClusterConfig(
		const OmnString &servers_str,
		const OmnString &cubes_str)
{
	AosXmlTagPtr tpl = mTpls["cluster"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	OmnString svrs;
	svrs << "<servers>"
		 << servers_str
		 << "</servers>";
	AosXmlTagPtr tag = AosXmlParser::parse(svrs AosMemoryCheckerArgs);
	aos_assert_r(tag, "");
	config->addNode(tag);
	OmnString cubes;
	cubes << "<cubes>"
		  << cubes_str
		  << "</cubes>";
	tag = AosXmlParser::parse(cubes AosMemoryCheckerArgs);
	aos_assert_r(tag, "");
	tag->setAttr("replic_policy", mBkpType);
	config->addNode(tag);
	OmnString str = config->toString();
	OmnString cluster;
	cluster << mClusterHome << "/" << AOSTAG_JIMOINSTALL_CLUSTER;
	createFile(str, cluster);
	return cluster;
}

OmnString
AosJimoInstall::createSvrProxyConfig(
		const u32 svr_id, 
		const int proxy_port, 
		const OmnString &workhome)
{
	AosXmlTagPtr tpl = mTpls["proxy"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	config = config->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(config, "");
	OmnString base_dir;
	if (mDataHome != "")
	{
		base_dir << mDataHome << svr_id;
	}
	else
	{
		base_dir << workhome << "/" << AOSTAG_JIMOINSTALL_JIMODATA << svr_id;
	}
	config->setAttr("base_dir", base_dir);
	OmnString port;
	port << proxy_port << "-" << proxy_port;
	config->setAttr("local_port", port);
	AosXmlTagPtr tag = config->getFirstChild("SvrProxy");
	aos_assert_r(tag, "");
	OmnString proxy_tmp;
	proxy_tmp << workhome << "/" << AOSTAG_JIMOINSTALL_PROXYTMP;
	tag->setAttr("tmp_data_dir", proxy_tmp);
	OmnString str = config->toString();
	OmnString proxy;
	proxy << mClusterHome << "/" << AOSTAG_JIMOINSTALL_PROXY;
	createFile(str, proxy);
	return proxy;
}

OmnString
AosJimoInstall::createAccessConfig(
					const int front_port, 
					const int access_port, 
					const int thrift_port)
{
	AosXmlTagPtr tpl = mTpls["access"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	config = config->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(config, "");
	config->setAttr("remote_port", front_port);
	OmnString port;
	port << access_port << "-" << access_port;
	config->setAttr("local_port", port);
	AosXmlTagPtr tag = config->getFirstChild("thrift_servers");
	aos_assert_r(tag, "");
	tag = tag->getFirstChild();
	aos_assert_r(tag, "");
	tag->setAttr("port", thrift_port);
	OmnString str = config->toString();
	OmnString access;
	access << mClusterHome << "/" << AOSTAG_JIMOINSTALL_ACCESS;
	createFile(str, access);
	return access;
}

OmnString
AosJimoInstall::createHtmlConfig(
				const int html_port, 
				const int front_port, 
				const OmnString &master_workhome)
{
	AosXmlTagPtr tpl = mTpls["html"];
	aos_assert_r(tpl, "");
	AosXmlTagPtr config = tpl->getFirstChild("config");
	aos_assert_r(config, "");
	config = config->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(config, "");
	OmnString file_path;
	file_path << master_workhome << "/WebSite/Ext/";
	config->setAttr("file_path", file_path);
	config->setAttr("remote_port", front_port);
	OmnString port;
	port << html_port << "-" << html_port;
	config->setAttr("local_port", port);
	OmnString str = config->toString();
	OmnString html;
	html << mClusterHome << "/" << AOSTAG_JIMOINSTALL_HTML;
	createFile(str, html);
	return html;
}

bool
AosJimoInstall::createFile(const OmnString &str, const OmnString &filename)
{
	int rslt = access(filename.data(), 0);
	if (rslt != 0)
	{
		OmnString touch = "touch ";
		touch << filename;
		command(mUser, touch);
	}
	OmnFile file(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	file.put(0, str.data(), str.length(), true);
	return true;
}

OmnString
AosJimoInstall::getLocalIp()
{
	struct hostent *hent;
	hent = gethostent();
	return inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));
}

int
AosJimoInstall::getPort(const u32 svr_id)
{
	//get port from file [192.168.99.82]-port
	OmnString cmd;
	cmd << mClusterHome << "/" << AOSTAG_JIMOINSTALL_PORTGEN << " " << mUser << " " << mIpList[svr_id];
	command(mUser, cmd);
	OmnString port_file;
	port_file << mClusterHome << "/" << mIpList[svr_id] << "-port";
	OmnFile file(port_file, OmnFile::eReadOnly AosMemoryCheckerArgs);
	OmnString buff;
	file.readToString(buff);
	int port = atoi(buff.data());
	aos_assert_r(port > 1024 && port < 65536, -1);
	return port;
}

int
AosJimoInstall::getBroadcastPort()
{
	OmnString ip_str;
	for (u32 i=0; i<mIpList.size(); i++)
	{
		ip_str << mIpList[i] << " ";
	}
	ip_str.setLength(ip_str.length() - 1);
	OmnString cmd;
	cmd << mClusterHome << "/" << AOSTAG_JIMOINSTALL_BROADCAST_PORTGEN << " '" << ip_str << "'";
	command(mUser, cmd.data());

	OmnString port_file;
	port_file << mClusterHome << "/" << AOSTAG_JIMOINSTALL_BROADCASTPORT;
	OmnFile file(port_file, OmnFile::eReadOnly AosMemoryCheckerArgs);
	OmnString buff;
	file.readToString(buff);
	int port = atoi(buff.data());
	aos_assert_r(port > 1024 && port < 65536, -1);
	return port;
}

OmnString
AosJimoInstall::createExe(const u32 svr_id)
{
	OmnString exe;
	exe << mExe << "/SvrProxy.exe "
		<< mExe << "/admin.exe "
		<< mExe << "/cube.exe "
		<< mExe << "/frontend.exe "
		<< mExe << "/task.exe ";
	if (svr_id == 0)
	{
		exe << mExe << "/access.exe "
			<< mExe << "/JQLClient.exe "
			<< mExe << "/htmlserver.exe ";
	}
	return exe;
}

bool
AosJimoInstall::command(const OmnString &user, const OmnString &cmd)
{
	OmnString run;
	run << "su " << user << " --command=\"" << cmd << "\"";
	system(run.data());
	return true;
}
