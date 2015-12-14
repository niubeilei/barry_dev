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
////////////////////////////////////////////////////////////////////////////
#include "Jimold/JimoldReqProc.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Ptrs.h"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;
using boost::shared_ptr;
using namespace boost::filesystem;
map<string, string> p;
AosJimoldReqProc::AosJimoldReqProc()
{
}


AosJimoldReqProc::~AosJimoldReqProc()
{
}


AosNetReqProcPtr
AosJimoldReqProc::clone()
{
	return OmnNew AosJimoldReqProc();
}

void AosJimoldReqProc::receiveInfo(const OmnTcpClientPtr &client)
{
	OmnConnBuffPtr buff;
    client->smartRead(buff);
	char *data = buff->getData();
	OmnString str;
	str << data;
	OmnScreen<<str<<endl;
	//system();
}          

map<string,string> AosJimoldReqProc::splitStr(string s)
{
	map<string,string> args;
	size_t size=s.size();
	string key,value;
	size_t i=6,j;
	while(i<size)
	{
		if(s[i]=='-')
		{
			j=0;
			while(s[i]!=' ')
			{
				i++;
				j++;
		    }
			key=s.substr(i-j,j);
		}
		i++;
		j=0;
		while(s[i]!=' '&&i<size)
		{
			i++;
			j++;
	    }
		value=s.substr(i-j,j);
		args[key]=value;
		i++;
	}
	return args;
}
map<string, string> AosJimoldReqProc::getInfo(char *data)
{
	int i=0;
	while(data[i]!='+')
	{
		i++;
	}
	string args(data,i);
	//int args_len=args.size();
	map<string,string> map_list=splitStr(args);
	int buf_len=atoi(map_list["-size"].c_str());
	string list(data,i+buf_len+1);
	string file_buf(list,i+1,buf_len+1);
	map_list["-file_buf"]=file_buf;
	return map_list;
}

string
AosJimoldReqProc::getBasedir(char *data)
{
	char *p=strstr(data,"-basedir");
	int i=0;
	//char *basedir=new char[512];
	OmnScreen<<"p is "<<p<<endl;
	if(p==NULL)
	{
		OmnScreen<<"find fail "<<endl;
		return "";
	}
	else
	{
		int len=strlen(p);
		int n=len-1;
		while(p[n]!=' ')
		{
			n--;
			i++;
		}
	string data_str=p;
	string basedir(data_str,n+1,i);
	return basedir;
	}
}


bool
AosJimoldReqProc::procRequest(const OmnConnBuffPtr &buff)
{
	OmnTcpClientPtr client = buff->getConn();
	OmnScreen<<"server begin receive"<<endl;
	AosWebRequestPtr req = OmnNew AosWebRequest(client, buff);
	OmnScreen<<"server end receive"<<endl;
	aos_assert_r(req, false);

	char *data = req->getData();
	//for start
	if(strncmp(data,"-start",6)==0)
	{
	    OmnScreen<<"recive start success:"<<data<<endl;
	    string s=getBasedir(data);
		OmnScreen<<"base dir is "<<s.data()<<endl;
		string startCmd=s+"/jimo/tools/startshell";
		::system("chmod -R 777 /home/*");
		recordFile(data);
		::system(startCmd.c_str());
	}
	//end start
	//for stop
	if(strncmp(data,"-stop",5)==0)
	{
        //string s=getBasedir(data);
		//string stopCmd="./"+s+"/jimo/tools/stopshell";

		//::system(stopCmd.c_str());
	}
	//end stop
	//for clean
	if(strncmp(data,"-clear",6)==0)
	{
        //string s=getBasedir(data);
		//string clearCmd="./"+s+"/jimo/tools/clearshell";

		//::system(clearCmd.c_str());
	}
	//end clean/
	if(data[0]=='-'&&data[1]=='f')
	{
	p=getInfo(data);
	OmnScreen<<"get buf success"<<endl;
	getDirectory();
	}
	OmnString str;
	str << data;

	return procCommand(str, client);
}


bool AosJimoldReqProc::writeToFile(const string fileName,const string fileInfo)
{
	FILE *fp;
	fp =fopen( fileName.c_str(), "wb+" ); 
	if(fp==NULL)
	{
		OmnScreen<<"open error"<<endl;
		return false;
	}
	int size=atoi(p["-size"].c_str());
	OmnScreen<<"open success"<<endl;
	if(fwrite(fileInfo.c_str(),size,1,fp)==1)
	{
		OmnScreen<<"write success"<<endl;
		fclose(fp);
		return true;
	}
	return false;
}


bool AosJimoldReqProc::checkCluster(const string &cluster_name)
{
	path path = ""+cluster_name;
	bool rslt=exists(path);
	return rslt;
}


bool AosJimoldReqProc::recordFile(char *data)
{
	struct passwd *pwd;
	pwd = getpwuid(getuid());
	string pwd_name=pwd->pw_name;
	char *cluster=strstr(data,"-cluster");
	int i=0;
	OmnScreen<<"cluster is "<<cluster<<endl;
	if(cluster==NULL)
	{
		OmnScreen<<"find fail "<<endl;
		return "";
	}
	else
	{
		while(*cluster!=' ')
		{
			cluster++;
		}
		cluster++;
		while(cluster[i]!=' ')
		{
			i++;
		}
		string clustername(cluster,i);
		string base=getBasedir(data);
		string filedir="/var/jimo/"+clustername+"/";   
		if(checkCluster(filedir)==false)
		{
			char *cmd=new char[strlen(filedir.c_str())+strlen("mkdir -p ")+1];
			strcpy(cmd,"mkdir -p ");
			strcat(cmd,filedir.c_str());
			OmnScreen<<"dir is "<<cmd<<endl;
			::system(cmd);
		}
		string filebuf=pwd_name+"/"+base+"/jimo/\n";
		OmnScreen<<"filebuf is "<<filebuf.data()<<endl;
		string filename=filedir+base;
		OmnScreen<<"filename is "<<filename.data()<<endl;
		//writeToFile(filename.c_str(),filebuf);
		FILE *fp;
	    fp =fopen( filename.c_str(), "w+" ); 
	    if(fp==NULL)
	    {
			OmnScreen<<"open error"<<endl;
		    return false;
	    }
	    int size=strlen(filebuf.c_str());
	    OmnScreen<<"open success"<<endl;
	    if(fwrite(filebuf.c_str(),size,1,fp)==1)
	    {
			OmnScreen<<"write success"<<endl;
		    fclose(fp);
		    return true;
	    }

		return true;
	}
}


bool AosJimoldReqProc::getDirectory()
{
	string buf=p["-serverdir"];
	OmnScreen<<buf.data()<<endl;
	string filebuf=p["-file_buf"];
	size_t size=buf.size();
	int n=size-1;
	while(buf[n]!='/')
	{
		n--;
	}
	string dir=buf.substr(0,n+1);
	struct passwd *pwd;
	pwd = getpwuid(getuid());
	string pwd_name=pwd->pw_name;
	string dir_path;
	if(checkCluster(dir)==false)
	{
		path dest_path="/home/"+pwd_name+"/"+dir;
		dir_path="/home/"+pwd_name+"/"+dir;
		//create_directory(dest_path);
		char *p=new char[strlen(dir_path.c_str())+strlen("mkdir -p ")+1];
		strcpy(p,"mkdir -p ");
		strcat(p,dir_path.c_str());
		OmnScreen<<"dir is "<<p<<endl;
		::system(p);
		OmnScreen<<"create success "<<endl;
	}
	else
	{
		OmnScreen<<"dirname is in"<<endl;
	}
	string filename="/home/"+pwd_name+"/"+buf;
	bool rslt=writeToFile(filename.c_str(),filebuf);
	return rslt;
}


bool
AosJimoldReqProc::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosJimoldReqProc::procCommand(const OmnString &command, const OmnTcpClientPtr &client)
{
	//command : jimo -start -cluster my_cluster -virtuals nnn -servers 192.168.99.96 192.168.99.97 192.168.99.98 -basedir "/home/cding" -port 123456
	OmnString resp = "helld word!!!!";
	bool rslt = false;
	if (!client || !client->isConnGood())
	{
		return false;
	}
	OmnScreen<<"server begin send"<<endl;
	rslt = client->smartSend(resp.data(), resp.length());
	if(rslt)
	{
		OmnScreen<<"server send success"<<endl;
	}
	if (!rslt)
	{
		client->closeConn();
		return false;
	}
	return true;
}

bool
AosJimoldReqProc::procStart(OmnString &resp, const OmnString &args)
{
	return true;
}
