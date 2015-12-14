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
// This is a utility to select docs.
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "CustomizedProcs/INotify/INotifyMgr.h"

#include "XmlUtil/SeXmlParser.h"
#include "SEModules/ObjMgr.h"
#include "API/AosApi.h"

OmnSingletonImpl(AosINotifyMgrSingleton,
                 AosINotifyMgr,
                 AosINotifyMgrSelf,
                 "AosINotifyMgr");


AosINotifyMgr::AosINotifyMgr()
:
mFd(-1),
mRootDir(""),
mContainer(""),
mPublic("true")
{
}


AosINotifyMgr::~AosINotifyMgr()
{
}

bool
AosINotifyMgr::config(const AosXmlTagPtr &config)
{
	//may be specify config objid
	//<inotify dir="xxxx" container="xxxx" public="true">
	//	<sys_attributes> 
	//		<A>zky_date</A>
	//		<B>zky_localnum</B>
	//		<C>zky_compresstype</C>
	//		<D>zky_starttime</D>
	//		<E>zky_direction</E>
	//		<F>zky_duration</F>
	//		<G>zky_number</G>
	//		<H>zky_hungway</H>
	//		<Path>zky_file_path</Path>
	//	<sys_attributes/>
	//</inotify>
	aos_assert_r(config, false);
	AosXmlTagPtr inotify_config = config->getFirstChild("inotify");
	if (inotify_config)
	{
		mRootDir = inotify_config->getAttrStr("dir", "");
		mContainer = inotify_config->getAttrStr("container", "");
		mPublic = inotify_config->getAttrStr("public", "true");
		AosXmlTagPtr sattrs = inotify_config->getFirstChild("sys_attributes");
		if (sattrs)
		{
			AosXmlTagPtr attr_tag = sattrs->getFirstChild();
			while(attr_tag)
			{
				mAttrs[attr_tag->getTagname()] = attr_tag->getNodeText();
				attr_tag = sattrs->getNextChild();
			}
		}
		if (mRootDir != "")
		{
			OmnThreadedObjPtr thisPtr(this, false);
			mThread = OmnNew OmnThread(thisPtr, "INotifyMgrThrd", 0, true, true, __FILE__, __LINE__);
			mThread->start();
		}
	}
	return true;
}


bool
AosINotifyMgr::start()
{
	return true;
}


bool
AosINotifyMgr::stop()
{
	return true;
}

bool
AosINotifyMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	aos_assert_r(mRootDir != "", false);
	if (!initWatch(eMask, mRootDir))
	{
		state = OmnThrdStatus::eExit;
	}
	return true;
}

bool
AosINotifyMgr::signal(const int threadLogicId)
{
	return true;
}

bool
AosINotifyMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosINotifyMgr::initWatch(const int mask, const OmnString &root)
{
	mDirset.clear();
	mFd = inotify_init();
	if (mFd < 0)
	{
		OmnAlarm << "inotify_init error" << enderr;
		return false;
	}

	DIR *odir;
	if ((odir = opendir(root.data())) == NULL)
	{
		OmnAlarm << "root directory is not exist: " << root << enderr;
		return false;
	}

	int wd = inotify_add_watch(mFd, root.data(), mask);
	mDirset.insert(make_pair(wd, root));

	//calculate current day directory
	char year[10], month[10], day[10];
	OmnGetTime(AosLocale::getDftLocale(), year, month, day);
	OmnString subdir = "";
	subdir << mRootDir << "/" << year << month << day;
	if (subdir != "")
	{
		addWatch(mFd, subdir, mask);
	}
	bool rslt = watchMonitor(mFd);
	return rslt;
}

bool
AosINotifyMgr::removeWatch(const OmnString &dir)
{
	//the dir format is: 20121114(yyyymmdd)
	DIR *odir;
	struct dirent *dent;
	if ((odir = opendir(dir.data())) == NULL)
	{
		OmnScreen << "fail to opendir , ignore: " << dir << endl;
		return true;
	}
OmnScreen << "remove listen directory name: " << dir << endl;

	while(((dent = readdir(odir))) != NULL)
	{
		if (strcmp(dent->d_name, ".") == 0
			|| strcmp(dent->d_name, "..") == 0)
		{
			continue;
		}

		if (dent->d_type == DT_DIR)
		{
			OmnString subdir;
			subdir << dir << "/" << dent->d_name;
			map<int, OmnString>::iterator itr;
			for (itr = mDirset.begin(); itr != mDirset.end(); itr++)
			{
				if (itr->second == subdir)
				{
					int wd = itr->first;
					inotify_rm_watch(mFd, wd);
					mDirset.erase(itr);
					break;
				}
			}
			removeWatch(subdir);
		}
	}
	return true;
}

bool
AosINotifyMgr::addWatch(
		const int fd, 
		const OmnString &dir, 
		const int mask)
{
	DIR *odir;
	struct dirent *dent;
	if ((odir = opendir(dir.data())) == NULL)
	{
		OmnScreen << "fail to open directory , ignore: " << dir << endl;
		return true;
	}
OmnScreen << "listen directory name: " << dir << endl;
	int wd = inotify_add_watch(fd, dir.data(), mask);
	mDirset.insert(make_pair(wd, dir));
	while((dent = readdir(odir)) != NULL)
	{
		if (strcmp(dent->d_name, ".") == 0
			|| strcmp(dent->d_name, "..") == 0)
		{
			continue;
		}
		if (dent->d_type == DT_DIR)
		{
			OmnString subdir;
			subdir << dir << "/" << dent->d_name;
			addWatch(fd, subdir, mask);
		}
	}
	closedir(odir);
	return true;
}


bool
AosINotifyMgr::watchMonitor(const int fd)
{
	int i, length;
	char *buf = OmnNew char[eBuffSize];
	struct inotify_event *event;
OmnScreen << "====================start to watch dir: " << mRootDir << " success!!!" << endl;
	while ((length = read(fd, buf, eBuffSize)) >= 0 || errno == 4)
	{
		i = 0;
		while(i < length)
		{
			event = (struct inotify_event*)(buf + i);
			if (event->len)
			{
				bool rslt = doAction(fd, event);
				aos_assert_r(rslt, false);

				///////////////listen directory//////////////////////////
				//map<int, OmnString>::iterator itr;
				//for(itr = mDirset.begin(); itr != mDirset.end(); itr++)
				//{
				//	OmnScreen << "directory : " << itr->second << endl;
				//}
				/////////////////////////////////////////////////////////
			}
			i += eEventSize + event->len;
		}
	}
	close(fd);
	OmnDelete [] buf;
OmnScreen << "===================stop to watch errno: " << errno << endl;
	return true;
}

bool
AosINotifyMgr::removeDir(const OmnString &name, const int wd)
{
	map<int, OmnString>::iterator itr = mDirset.find(wd);
	if (itr != mDirset.end())
	{
		OmnString crtdir;
		crtdir << itr->second << "/" << name;
		for (itr = mDirset.begin(); itr != mDirset.end(); itr++)
		{
			if (itr->second == crtdir)
			{
				int cwd = itr->first;
				inotify_rm_watch(mFd, cwd);
				mDirset.erase(itr);
OmnScreen << "remove listen directory: " << crtdir << endl;
				break;
			}
		}
	}
	return true;
}


bool
AosINotifyMgr::doAction(
		const int fd, 
		struct inotify_event *event)
{
	if ((event->mask & eNewDir) == eNewDir)
	{
		//add new dir
		appendDir(fd, event, eMask);
	}
	else if (event->mask & eDelDir)
	{
		//delete dir
		if ((event->mask & eDelDir) == eDelDir)
		{
			removeDir(event->name, event->wd);
		}
	}
	else if ((event->mask & IN_MOVED_TO) == eMoveTo)
	{
		//rename file
		if ((event->mask & IN_ISDIR) == IN_ISDIR)
		{
			return true;
		}
		createMapDoc(event->name, event->wd);
	}
	else
	{
		//create file
		if ((event->mask & IN_ISDIR) == IN_ISDIR)
		{
			return true;
		}
		createMapDoc(event->name, event->wd);
	}
	return true;
}

bool
AosINotifyMgr::testDate(char* strdate)
{
	int year, month, day;
	sscanf(strdate, "%04d%02d%02d", &year, &month, &day);
	if(year<0||month<0||day<0)return false;
	if(month==0||day==0)return false;
	if (month<=12&&day<=31)
	{ 
		if (year%400==0&&month==2&&day<=29)
		{
			return true;
		}
		else if (year%100!=0&&year%4==0&&month==2&&day<=29) 
		{
			return true;
		}
		else if ((month==1||month==3||month==5||month==7||month==8||month==10||month==12)&&day<=31)
		{
			return true;
		}
		else if ((month==2||month==4||month==6||month==9||month==11)&&day<=30)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else 
	{
		return false;
	}
}

bool 
AosINotifyMgr::appendDir(
		const int fd, 
		struct inotify_event *event, 
		const int mask)
{
	OmnString pdir = mDirset.find(event->wd)->second;
	OmnString cdir = event->name;
	if (pdir == mRootDir)
	{
		if (!testDate(cdir.getBuffer()))
		{
			OmnScreen << "invalid directory name, ignore: " << cdir << endl;
			return true;
		}

		//to remove yestoday dir watch
		char t_buff[10], y_buff[10];
		time_t t_time = OmnGetSecond();
		time_t y_time = t_time - 24*60*60;

		tm t_date = *localtime(&t_time);
		tm y_date = *localtime(&y_time);

		sprintf(t_buff, "%04d%02d%02d", t_date.tm_year - 100 + 2000, t_date.tm_mon+1, t_date.tm_mday);
		sprintf(y_buff, "%04d%02d%02d", y_date.tm_year - 100 + 2000, y_date.tm_mon+1, y_date.tm_mday);

		OmnString t_str(t_buff, 8);
		OmnString y_str(y_buff, 8);

		if (t_str != cdir)
		{
			OmnScreen << "is not today directory name, ignore: " << cdir << " , " << t_str << endl;
			return true;
		}

		OmnString y_dir;
		y_dir << mRootDir << "/" << y_str;

		map<int, OmnString>::iterator itr;
		for (itr = mDirset.begin(); itr != mDirset.end(); itr++)
		{
			if (itr->second == y_dir)
			{
				removeWatch(y_dir);
				break;
			}
		}
	}
	else
	{
		if (!cdir.isDigitStr())	
		{
			OmnScreen << "invalid directory name, ignore: " << cdir << endl;
			return true;
		}
	}
	OmnString ndir;
	ndir << pdir << "/" << cdir;
OmnScreen << "listen directory name: " << ndir << endl;
	int wd = inotify_add_watch(fd, ndir.data(), mask);
	mDirset.insert(make_pair(wd, ndir));
	return true;
}

bool
AosINotifyMgr::createMapDoc(
		const OmnString &filename,
		const int wd)
{
	//“C:\TMNData\20070708\66668888\P-113912-I-0006-1234567890-S.Dat”
	//            |______| |______| |_|____| | |__| |________| |        
	//                A        B     C   D   E   F       G     H
	//说明：
	//A：表示日期，由8位字符组成，格式是"YYYYMMDD"，分别表示年月日；
	//B：本地号码，字符数量不定，所以建议服务器端设置本地号码时不要包含“\”字样；
	//C：由2个字符组成，表示当前文件的语音压缩格式，
	//"P-"表示2倍压缩的DIALOGIC ADPCM压缩格式;
	//"V-"表示2倍压缩的IMA ADPCM压缩格式;
	//"Q-"表示4倍压缩的ADPCM格式;
	//"G-"或者没有字符标志则表示GSM格式。
	//D：通话开始时间，由6个字符组成，格式是"HHNNSS"，分别表示时分秒；
	//E：通话方向，一个字符长度，"O"表示打出，"I"表示打入；
	//F：通话时长，由4个字符组成，时长不够4位的以字符"0"填充，单位是秒；
	//G：对方号码，长度不定，视实际来电情况为准；
	//H：谁先挂机，"S"本地先挂机，"R"对方先挂机，此功能只有数字电话录音才生效。

	//A: zky_date, 
	//B: zky_localnum,
	//C: zky_compresstype,
	//D: zky_starttime,
	//E: zky_direction,
	//F: zky_duration,
	//G: zky_number,
	//H: zky_hungway,
	
	int pidx = filename.indexOf(".", 0);
	if (pidx < 0)
	{
		OmnScreen << "no file extension. ignore: " << filename << endl;
		return true;
	}
	OmnString ex = filename.substr(pidx, filename.length()-1);
	if (ex.toLower() != ".dat")
	{
		OmnScreen << "invalid file extension. ignore: " << filename << endl;
		return true;
	}
	OmnString path = mDirset.find(wd)->second;
	OmnString fullPath;
	fullPath << path << "/" << filename;
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse("<doc/>", "" AosMemoryCheckerArgs);
	aos_assert_r(doc, false);
	int rl = mRootDir.length();
	int fl = fullPath.length();
	OmnString splitPath = fullPath.substr(rl, fl-1);
	OmnStrParser1 strparser(splitPath, "/");
	OmnString subdir;
	int idx = 0;
	while((subdir = strparser.nextWord()) != "")
	{
		if (idx == 0)
		{
			if (subdir == "" || subdir.length() != 8)
			{
				OmnScreen << "file format is invalid. ignore: " << subdir << endl;
				return true;
			}
			doc->setAttr(mAttrs["A"], subdir);
		}
		else if (idx == 1)
		{
			doc->setAttr(mAttrs["B"], subdir);
		}
		else if (idx == 2)
		{
			int subIdx = 0;
			OmnStrParser1 subParser(subdir, "-");
			OmnString tmp;
			while((tmp = subParser.nextWord()) != "")
			{
				if (subIdx == 0)
				{
					if (tmp == "" && tmp.length() != 1)
					{
						OmnScreen << "file format is invalid. ignore: " << subdir << endl;
						return true;
					}
					doc->setAttr(mAttrs["C"], tmp);
				}
				else if (subIdx == 1)
				{
					if (tmp == "" && tmp.length() != 6)
					{
						OmnScreen << "file format is invalid. ignore: " << subdir << endl;
						return true;
					}
					doc->setAttr(mAttrs["D"], tmp);
				}
				else if (subIdx == 2)
				{
					if (tmp == "" && tmp.length() != 1)
					{
						OmnScreen << "file format is invalid. ignore: " << subdir << endl;
						return true;
					}

					doc->setAttr(mAttrs["E"], tmp);
				}
				else if (subIdx == 3)
				{
					if (tmp == "" && tmp.length() != 4)
					{
						OmnScreen << "file format is invalid. ignore: " << subdir << endl;
						return true;
					}

					doc->setAttr(mAttrs["F"], tmp);
				}
				else if (subIdx == 4)
				{
					if (tmp == "" || !tmp.isDigitStr())	
					{
						OmnScreen << "file format is invalid. ignore: " << subdir << endl;
						return true;
					}
					doc->setAttr(mAttrs["G"], tmp);
				}
				else
				{
					OmnString hungway = tmp.subString(0, 1);
					if (hungway == "" || hungway.length() != 1)
					{
						OmnScreen << "file format is invalid. ignore: " << subdir << endl;
						return true;
					}
					doc->setAttr(mAttrs["H"], hungway);
					break;
				}
				subIdx++;
			}
			break;
		}
		idx++;
	}
	doc->setAttr(mAttrs["Path"], fullPath);
	doc->setAttr("zky_pctrs", mContainer);
	doc->setAttr("zky_public_ctnr", mPublic);
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));
	AosXmlTagPtr xmldoc = AosCreateDoc(doc, rdata);
	aos_assert_r(xmldoc, false);
OmnScreen << "createdoc : " << xmldoc->toString() << endl;
	return true;
}
