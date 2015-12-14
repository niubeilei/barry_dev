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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/XmlTorturer/XmlDiffTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlTag.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

#include <deque>
#include <vector>


AosXmlDiffTester::AosXmlDiffTester()
{
	mName = "XmlDiffTester";
	mXmlTagNum = 0;
	mRandNum = 0;
	
}


bool 
AosXmlDiffTester::start()
{
	cout << "Start AosXmlDiff Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosXmlDiffTester::basicTest()
{
	AosXmlTagPtr oldXml = createXml();	

	int tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		AosXmlTagPtr oldxmlcp = oldXml->clone();
		AosXmlTagPtr newxml = modifyXmlRand(oldxmlcp);
		checkXmlsDiff(oldxmlcp, newxml);
		difftemp.clear();
		diffproc.clear();
	}
	cout << endl;
	return true;
}

AosXmlTagPtr AosXmlDiffTester::modifyXmlRand(AosXmlTagPtr oldxml)
{
	AosXmlTagPtr newxml = oldxml->clone();
	// newxml->getFirstChild();
	int t = rand() % 100;
	for(int i=0; i<t; i++)
	{
		int vv = rand() %100;
		if(vv < 10)
		{
			//modify tagname
			modifyTagName(oldxml, newxml);
		}
		else if(vv < 50)
		{
			//modify attr 
			modifyAttr(oldxml, newxml);
		}
		else if(vv < 80) 
		{
			//modify nodetext
			modifyNodeText(oldxml, newxml);
		}
		else
		{
			//modify node
			modifyNode(oldxml, newxml);
		}

	}
	return newxml;
}

void AosXmlDiffTester::getPathRand(OmnString &path, char type, const AosXmlTagPtr oldxml)
{
	int stopNum = (rand() % mXmlTagNum) + 1;
	oldxml->mCrtChild=-1;
	mRandNum = 0;
	getElementList(oldxml, stopNum);
	vector <OmnString> vtagname;
	vector <OmnString>::reverse_iterator Iter1;
	int num=0;
	while(AosXmlTagPtr parentnd = mRandTag->getParentTag())
	{
		num++;
		OmnString tagname = parentnd->getTagname();
		if(tagname != "<")
		{
			vtagname.push_back(tagname); 
		}
		if(parentnd)
		{
			mRandTag = parentnd;
		}
		else
		{
			break;
		}
	}
	if(num == 2)
	{
		return;
	}
	for ( Iter1 = vtagname.rbegin() ; Iter1 != vtagname.rend() ; ++Iter1)
	{
		if(Iter1 == (vtagname.rend()-1))
		{
			path << *(vtagname.rend() -1);
		}
		else if(*Iter1 && Iter1 != vtagname.rbegin())
		{
			path << *Iter1 << "/";
		}
	}
	/*if (type == eNodeText)
	{
		if(path != "")
		{
			path << "/_#text";
		}
		else
		{
			path << "_#text";
		}
	}
*/
cout << "path ========== : " << path << endl;
}

void AosXmlDiffTester::getElementList(const AosXmlTagPtr parentxml, int stopNum)
{
	cout << endl;

	for(int i=0; i<parentxml->mChildTags.entries(); i++)
	{
		mRandNum++;
		AosXmlTagPtr child = parentxml->mChildTags[i];
		if(mRandNum == stopNum)
		{
			mRandTag = child;
			break;
		}
		getElementList(child, stopNum);
	}
}

AosXmlTagPtr AosXmlDiffTester::modifyTagName(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml)
{
	//the format of the path as follow:
	//tagname : tag1/tag2,  ignore the root path
	//nodetext : _#text; tag1/_#text, ignore the root path
	//attr :  @attr1; tag1/@attr1, ignore the root path
	//node :tag1; tag1/tag2, ignore the root path

	//only modify the tagname, it is the same as delete old tag and create new tag :
	//0.randomly create the path;
	//1.get old tagname  by path;
	//2.create the new tagname by random;
	//3.store the old tag by old tagname and store new tag by new tagname into difftemp map;
	//4.add new tag in the newxml and delete old tag in newxml;
	//5.return newxml;
	//notice : before step 3 , check there is the similar path in the difftemp map, delete the child path
	//		for an example: 
	//		the path of 'root/tag1/tag2/_#text' exist in the difftemp map and you need to delete the path as 'root/tag1/tag2',
	//		then you must delete the path of 'root/tag1/tag2/_#text' in the difftemp map
	
	OmnString oldpath="";
	getPathRand(oldpath, 'n', oldxml);
	if(oldpath == "")
	{
		return newxml;
	}
	AosXmlTagPtr oldtag = oldxml->xpathGetFirstChild(oldpath);
	AosXmlTagPtr newtag = newxml->xpathGetFirstChild(oldpath);
	if(!newtag)
	{
		return newxml;
	}
	int len = (rand() % 10) + 5;
	char ch[20];
	AosRandomLetterStr11(len, ch);
	OmnString newtgname ;
	newtgname << ch;
cout << "zhao 001 setNodeName : new1: " << newxml->toString() << endl;
cout << "zhao 001 setNodeName : path =: " << oldpath << endl;
cout << endl;
	newtag->setNodeName(newtgname);
cout << "zhao 001 setNodeName : new2: " << newxml->toString() << endl;
	//OmnStrParser parser(oldpath, "/");
	
	OmnString newpath = oldpath;
	int index = oldpath.indexOf(0, '/' , true) + 1;
	newpath.replace((u32)index, (u32)(oldpath.length()-index), newtgname);
cout << "zhao 001 setNodeName : newpath =: " << newpath << endl;
/*	while(parser.hasMore())
	{
		if(parser.finished())
		{

			newpath << "/" << newtgname;
		}
		else
		{
			newpath << parser.nextWord() << "/";
		}
	}
*/	//notice
	map<OmnString, pair<OmnString, OmnString> >::iterator it;
	for( it = difftemp.begin(); it!=difftemp.end();)
	{
		OmnString key = it->first;
		int find = key.findSubString(oldpath, 0, false);
		if(find != -1)
		{
			difftemp.erase(it++);
		}
		else
		{
			++it;
		}
//		it = difftemp.begin();
	}
	difftemp[oldpath] = make_pair(oldtag->toString(), "");
	difftemp[newpath] = make_pair("", newtag->toString());
cout << "zhao 001 setNodeName : difftemp.size(): " << difftemp.size() << endl;
	return newxml; 
}

void AosXmlDiffTester::AosRandomLetterStr11(const int len, char *data)
{
	const char AosLetterMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefhijklmnopqrstuvwxyz";
	int i = 0;
	while (i<len)
	{
		 u32 idx = rand();
		data[i++] = AosLetterMap[idx % 0x33];
		if (i < len) data[i++] = AosLetterMap[(idx>>8) % 0x33];
		if (i < len) data[i++] = AosLetterMap[(idx>>16) % 0x33];
		if (i < len) data[i++] = AosLetterMap[(idx>>24) % 0x33];
	}
	data[len] = 0;
}

AosXmlTagPtr AosXmlDiffTester::modifyNodeText(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml)
{
	//only modify the nodetext, it is the same as delete old text of the node and create new text of the node :
	//0.randomly create the path;
	//1.get the old text of one node by path;
	//2.create the new text of the node  by random;
	//3.store the old text of the node and new text of the node in difftemp map by path
	//4.modify the newxml;
	//5.return the newxml
	//notice : 1.check if there is a child by path
	//		   2.if the path is not in the newxml and it is in the oldxml, and tag with the path in the oldxml has some attributes,
	//		   and you already modify the attributes , and tagname has modified, you will lost some information in the difftemp.
	//		   for an example:  (setNodeText function can set text by not exist path)
	//		   the path : tag11/tag12
	//		   oldxml:
	//		   <tag11 attr13="nighteen">
	//		      <tag12 attr14="nighteen"/>
	//		   </tag11>
	//		   newxml:
	//		   <tag11 attr13="nighteen">
	//		      <xmkddk attr14="nighteen"/>
	//		   </tag11>
	
	OmnString path="";
	getPathRand(path, 't', oldxml);
	int len = (rand() % 10) + 5;
	char ndt[20];
	AosRandomLetterStr11(len, ndt);
	OmnString newnodetxt;
	newnodetxt << ndt;
	//notice
	if(path != "")
	{
		AosXmlTagPtr oldtag = oldxml->xpathGetFirstChild(path); 
		if(oldtag->mChildTags.entries())
		{
			return newxml;
		}
		AosXmlTagPtr newtag = newxml->xpathGetFirstChild(path); 
		if(!newtag || newtag->mChildTags.entries())
		{
			return newxml;
		}
	}
	else
	{
		return newxml;
	}

cout << "zhao 002 setNodeText : new1: " << newxml->toString() << endl;
cout << "zhao 002 setNodeText : path: " << path << endl;
	newxml->setNodeText(path, newnodetxt, false);
cout << "zhao 002 setNodeText : new2: " << newxml->toString() << endl;
	OmnString oldtext = oldxml->getNodeText(path); 
	difftemp[path] = make_pair(oldtext, newnodetxt);
cout << "zhao 002 setNodeText : difftemp.size(): " << difftemp.size() << endl;
	return newxml;
}

AosXmlTagPtr AosXmlDiffTester::modifyAttr(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml)
{
	//0.randomly create the path;
	//1.check the action type: modify , add or delete;
	//2.if modify as follow: 
	//  2.1 get old value of attr by path;
	//  2.2 create the new value of the attr by random;
	//  2.3 store the old value of the attr and new value of the attr in difftemp map by path;
	//  2.4 modify the newxml;
	//  2.5 return the newxml;
	//3.if add as follow :
	//  3.1 create the new attr and value by path;
	//  3.2 store the new value of the attr in difftemp map by path;
	//  3.3 modify the newxml;
	//  3.4 return the newxml;
	//4.if delete as follow:
	//  4.1 get old attr and value by path;
	//  4.2 store the old attr and value in difftemp map by path;
	//  4.3 modify the newxml;
	//  4.4 return the newxml;
	//
	//  notice :1. there is the odds of the modify ,add and delete


	OmnString path="";
	getPathRand(path, 'n', oldxml);
	//selete randomly the child 
	AosXmlTagPtr oldchild;
	AosXmlTagPtr newchild;
	if(path =="")
	{
		oldchild = oldxml;
		newchild = newxml;
	}
	else
	{
		oldchild = oldxml->xpathGetFirstChild(path);
		newchild = newxml->xpathGetFirstChild(path);
		if(!newchild)
		{
			return newxml;
		}
	}
	int vv = rand() %100;
cout << "zhao 003 setAttr : new1: " << newxml->toString() << endl;
cout << "zhao 003 setAttr : path: " << path << endl;
	if(vv < 30)
	{
		// modify attribute
		for (int i=0; i<newchild->mAttrs.entries(); i++)
		{
			int len = (rand() % 2);
			if(len)
			{
			
				char *buffData = newchild->mBuff->getData();
				int nlen = newchild->mAttrs[i].nameEnd - newchild->mAttrs[i].nameStart + 1;
				OmnString name((char *)&buffData[newchild->mAttrs[i].nameStart], nlen);
				OmnString oldvalue((char *)&buffData[newchild->mAttrs[i].valueStart], newchild->mAttrs[i].valueLen);

				int len2 = (rand() % 10) + 5;
				char ch[20];
				AosRandomLetterStr11(len2, ch);
				OmnString newattrvl;
				newattrvl << ch;

				OmnString pathName;
				if(path != "")
				{
					pathName << path;
					pathName << "/@" << name;
				}
				else
				{
					pathName << "@" << name;
				}
				newchild->setAttr(name, newattrvl);
				difftemp[pathName] = make_pair(oldvalue, newattrvl);
			}
		}
cout << "zhao 003 setAttr  md : difftemp.size(): " << difftemp.size() << endl;
cout << "zhao 003 setAttr  md : new2: " << newxml->toString() << endl;
		return newxml;
	}
	else if (vv < 80)
	{
		//add attribute
		int times = (rand() % 10);
		for(int i=0; i<times; i++)
		{
			int len1 = (rand() % 10) + 5;
			char at[20];
			AosRandomLetterStr11(len1, at);
			OmnString newattr;
			newattr << at;
			int len2 = (rand() % 10) + 5;
			char vl[30];
			AosRandomLetterStr11(len2, vl);
			OmnString newvl;
			newvl << vl;
			newchild->setAttr(newattr, newvl);
			OmnString pathName;
			if(path != "") 
			{
				pathName << path;
				pathName << "/@" << newattr;
			}
			else
			{
				pathName << "@" << newattr;
			}
			difftemp[pathName] = make_pair("", newvl);
		}
cout << "zhao 003 setAttr ad: difftemp.size(): " << difftemp.size() << endl;
cout << "zhao 003 setAttr ad: new2: " << newxml->toString() << endl;
		return newxml;
	}
	else
	{
		//delete attribute
		//friend class
		for (int i=0; i<newchild->mAttrs.entries(); i++)
		{
			int len = (rand() % 2);
			if(len)
			{
			
				char *buffData = newchild->mBuff->getData();
				int nlen = newchild->mAttrs[i].nameEnd - newchild->mAttrs[i].nameStart + 1;
				OmnString name((char *)&buffData[newchild->mAttrs[i].nameStart], nlen);
				OmnString value((char *)&buffData[newchild->mAttrs[i].valueStart], newchild->mAttrs[i].valueLen);

				OmnString pathName;
				if(path != "") 
				{
					pathName << path;
					pathName << "/@" << name;
				}
				else
				{
					pathName << "@" << name;
				}
				OmnString oldvalue = oldchild->getAttrStr(name);
				if(!oldvalue || oldvalue=="")
				{
					return newxml;
				}
				newchild->removeAttr(name);
				//notice
			/*	map<OmnString, pair<OmnString, OmnString> >::iterator it;
				for( it = difftemp.begin(); it!=difftemp.end();)
				{
					OmnString key = it->first;
					if(key == pathName)
					{
						difftemp.erase(it++);
					}
					else
					{
						++it;
					}
				}
			*/
				difftemp[pathName] = make_pair(value,"");
			}
cout << "zhao 003 setAttr de: difftemp.size(): " << difftemp.size() << endl;
cout << "zhao 003 setAttr de: new2: " << newxml->toString() << endl;
		}
		return newxml;
	}
}

AosXmlTagPtr AosXmlDiffTester::modifyNode(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml)
{
	//0.randomly create the path;
	//1.check the action type : add or delete;
	//2.if add:
	//  2.1 randomly create the new node;
	//  2.2 store the new node in the difftemp;
	//  2.3 insert the new node into newxml by path;
	//  2.4 return the newxml;
	//
	//3.if delete:
	//  3.1 store the old node into difftemp map by path;
	//  3.2 delete old node in the newxml;
	//  3.3 return the newxml;
	// 	notice : 1.there is the odds of the add, delete; 
	// 			 2.if the operation is delete, you must delete the all of child path in the difftemp.
	// 			 for an example:
	// 			 there is the path of 'root/tag1/tag2/@attr1' in the difftemp and you want to delete the path of 'root/tag1/tag2'
	// 			 then you must delete the path of 'root/tag1/tag2/@attr1' in the difftemp;
	

	OmnString path="";
	getPathRand(path, 'n', oldxml);
//	AosXmlTagPtr node = createXml();
	int vv = rand() %100;
cout << "zhao 004 setNode : new1: " << newxml->toString() << endl;
cout << "zhao 004 setNode : path: " << path << endl;
	if(vv < 50)
	{
		//add node
		AosXmlTagPtr newtag = newxml->xpathGetFirstChild(path);
		if(!newtag)
		{
			return newxml;
		}
		int len = (rand() % 10) + 5;
		char tgn[20];
		AosRandomLetterStr11(len, tgn);
		OmnString newtagname;
		newtagname << tgn;
		newtag->addNode1(newtagname);
		
		if(path == "")
		{
			path << newtagname;
		}
		else
		{
			path << "/" << newtagname;
		}
		AosXmlTagPtr node = newxml->xpathGetFirstChild(path);
		difftemp[path] = make_pair("", node->toString());
cout << "zhao 004 setNode ad: difftemp.size(): " << difftemp.size() << endl;
cout << "zhao 004 setNode ad: new2: " << newxml->toString() << endl;
		return newxml;
	}
	else
	{
		//delete node
		if(path == "")
		{
			return newxml;
		}
		AosXmlTagPtr nodedel = oldxml->xpathGetFirstChild(path);
		if(nodedel)
		{
			return newxml;
		}
		bool finish =  newxml->xpathRemoveNode(path);
		if(!finish)
		{
			return newxml;
		}

		//notice
		map<OmnString, pair<OmnString, OmnString> >::iterator it;
		for( it = difftemp.begin(); it!=difftemp.end();)
		{
			OmnString key = it->first;
			int find = key.findSubString(path, 0, false);
			if(find != -1)
			{
				difftemp.erase(it++);
			}
			else
			{
				++it;
			}
		}
		difftemp[path] = make_pair(nodedel->toString(), "");
cout << "zhao 004 setNode de: difftemp.size(): " << difftemp.size() << endl;
cout << "zhao 004 setNode de: new2: " << newxml->toString() << endl;
		return newxml;
	}

}

bool AosXmlDiffTester::checkXmlsDiff(AosXmlTagPtr oldxml, AosXmlTagPtr newxml)
{
	OmnString path="";
	oldxml->isDocSame1(newxml, path, diffproc);
	map<OmnString, pair<OmnString, OmnString> >::iterator it1 = difftemp.begin();
	bool notfind = false;
cout << "zhao 001 old ----------" << oldxml->toString() << endl;
cout << endl;
cout << endl;
cout << "zhao 001 new ----------" << newxml->toString() << endl;
	for(; it1!=difftemp.end(); it1++)
	{
		OmnString key1 = it1->first;
cout << "zhao key 1 : " << key1 << endl;
		map<OmnString, pair<OmnString, OmnString> >::iterator it2 = diffproc.find(key1);
		if(it2 == diffproc.end())
		{
			notfind = true;
			break;
		}

	}
	map<OmnString, pair<OmnString, OmnString> >::iterator it3 = diffproc.begin();
	for(; it3!=diffproc.end(); it3++)
	{
		OmnString key2 = it3->first;
cout << "zhao key 2 : " << key2 << endl;
		map<OmnString, pair<OmnString, OmnString> >::iterator it4 = difftemp.find(key2);
		if(it4 == difftemp.end())
		{
			notfind = true;
			break;
		}
	}

	if(notfind == true)
	{
		cout << "there is a problem";
		exit(0);
		return false;
	}
	else
	{
		cout << "there is no a problem";
		return true;
	}

}

AosXmlTagPtr 
AosXmlDiffTester::createXml()
{
	// 1. get vector 
	// 2. define how deep the xml is (deepest is about 4)
	// 3. random create attribute, attr name like: attr1, attr2, attr3...
	// 4. tagname, attrvalue, nodetext, those three parts are one part of nextWord
	
	vector<OmnString> wordlist;
	vector<OmnString> disable_attr;
	OmnString strs[50] = {"one","tow","three","four","five","six", "seven", "eight", "night","ten",
		        		"eleven","twelve", "thriteen", "fourteen", "fifteen", "sixteen", "seventeen","eighteen",
				        "nighteen", "twenty","thrity", "forty", "fifty","sixty","senventy","eighty","nighty",
						"hundred","thousand", "million", "billion","monday", "tuesday", "wednesday", "thursday",
						"friday", "saturday", "sunday", "january", "februry","marth", "april", "may", "jun", "july",
						"august","september","obtober","november","december"};

	for (int i = 0; i < 50; i++)
		    wordlist.push_back(strs[i]);


	deque<OmnString> endlist;

	struct timeval tv;

	//root tag
	OmnString tagstr;

	int i = 0, j = 0, k = 0;
	bool hastext = false;
	bool tagend = false;
	bool root_attr = true;
	bool closed = false;

	tagstr << "<tag" << k << " ";
	OmnString tmp = "</tag";
	tmp << k++ << ">";
	endlist.push_front(tmp);
	endlist.push_front(">");

	while(endlist.size())
	{
		if (!closed)
		{
			//atttr or end
			gettimeofday(&tv, NULL);
			srand(tv.tv_usec);
			if (rand()%10<5 && i<(int)wordlist.size())
			{
				//attr
				tagstr << "attr" << j << "=\"" << wordlist[i++] << "\" ";
				OmnString disattr = "";
				disattr << "attr" << j++;

				vector<OmnString>::iterator it = find(disable_attr.begin(), disable_attr.end(), disattr);
				if (root_attr && it == disable_attr.end())
				{
					disable_attr.push_back(disattr);
				}
				continue;
			}
			else
			{
				//end
				tagstr << endlist[0];
				endlist.pop_front();
				closed = true;
				hastext = false;
				tagend = false;
				continue;
			}
		}
		else
		{
			//tag text end tag
			gettimeofday(&tv, NULL);
			srand(tv.tv_usec);
			int rd =  rand();
	
			if (i==(int)wordlist.size())
				tagend=true;

			if (rd%3 ==0)
				//&& quanrd < 40)
			{
				//text
				if (!hastext && i<(int)wordlist.size() && !root_attr)
				{
					tagstr << wordlist[i++];
					hastext = true;
					tagend= true;
				}
				continue;
			}
			else if (rd %3==1 && i<(int)wordlist.size() && !tagend)
			{
				//tag
				tagstr << "<tag" << k << " ";
				OmnString tmp = "</tag";
				tmp << k++ << ">";
				endlist.push_front(tmp);
				endlist.push_front(">");
				closed = false;
				tagend = true;
				hastext = true;
				root_attr = false;
				//set mXmlTagNum
				mXmlTagNum++;
			}
			else if(tagend)
			{
				//end tag
				if (endlist.size()>1)
				{
					tagstr << endlist[0];
					endlist.pop_front();
					tagend= false;
					hastext = true;
				}
				else if (i == (int)wordlist.size())
				{
					tagstr << endlist[0];
					endlist.pop_front();
					tagend= false;
				}
			}
		}
	}

	AosXmlParser parser;
	AosXmlTagPtr ctag = parser.parse(tagstr, "");
	if (!ctag)
	{
		cout << "parse tagstr error!" << endl;
		return NULL;
	}
cout << "root 11111111111111 : " << ctag->toString() << endl;
	return ctag;
}
