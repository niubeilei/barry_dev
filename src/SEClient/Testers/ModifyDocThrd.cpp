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
// This class runs as a separate thread. It randomly sleeps for a 
// a period of time. When it wakes up, it creates a new doc. After
// that, it sleeps again, until it creates a given number of docs. 
//   
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/Testers/ModifyDocThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Porting/Process.h"
#include "SEUtil/DocTags.h"
#include "SEClient/SEClient.h"
#include "SEClient/Testers/SearchEngTester.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "SEClient/Testers/TestXmlDoc.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosModifyDocThrd::AosModifyDocThrd(
		const AosSearchEngTesterPtr &torturer,
		const int total, 
		const int weight,
		const OmnIpAddr &raddr,
		const int rport, 
		const OmnString &attrtname)
:
mTorturer(torturer),
mTotal(total),
mWeight(weight),
mNumModified(0),
mRaddr(raddr),
mRport(rport),
mAttrTname(attrtname)
{
	mConn = OmnNew AosSEClient(raddr, rport);
	if (mWeight <= 0) mWeight = eDftWeight;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "ModifyDocThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosModifyDocThrd::~AosModifyDocThrd()
{
}


bool
AosModifyDocThrd::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosModifyDocThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	mIsCreating = false;
    while (state == OmnThrdStatus::eActive)
    {
		if (mNumModified < mTotal) 
		{
			mIsCreating = true;
			modifyDoc();
			// OmnScreen << "Finished modifying" << endl;
			mIsCreating = false;
		}
		else
		{
			int wtime = rand() % mWeight;
			OmnWaitSelf->wait(wtime);
		}
	}
	return true;
}


bool
AosModifyDocThrd::modifyDoc()
{
	// This function modifies a doc. If the torturer locks the modification,
	// it does nothing.
	if (mTorturer->isUpdateStopped()) return true;

	mNumModified++;
	OmnScreen << "Modifying: " << mNumModified << endl;

	// 1. Reads a doc
	AosXmlTagPtr root = mTorturer->readDoc();
	if (!root)
	{
		OmnAlarm << "Failed to get the doc to modify" << enderr;
		exit(0);
	}
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	mDocid = child->getAttrStr(AOSTAG_DOCID, "");
	mSiteid = child->getAttrStr(AOSTAG_SITEID, "");
	OmnString objid = child->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(mDocid != "", false);
	aos_assert_r(mSiteid != "", false);

	int vv = rand();

	OmnString docstr;
	mCrtOpr = "";
	mCrtValue = "";
	OmnString docsource;
	if ((vv%100) < 15)
	{
		// Generate a totally different doc (but with the same
		// docid). 
		int depth = vv % eDepth;
		AosTestXmlDocPtr doc = OmnNew AosTestXmlDoc(mSiteid, mDocid, objid, depth);
		aos_assert_r(doc, false);
		doc->composeData(docstr);
		docsource = "newcreated";
	}
	else
	{
		// Modify the doc. It randomly determines the number of
		// modifications to make to the doc. For each modification,
		// it can be:
		// 1. Add an attribute
		// 2. Delete an attribute
		// 3. Modify an attribute
		// 4. Add words to texts
		// 5. Remove words from texts
		// 6. Add texts if a tag does not have texts
		// 7. Remove texts if a tag has texts
		// 8. Add a tag
		// 9. Remove a tag
		// 10. Do the modification to one of its subtag
		int numMods = vv % eMaxMods;
		int cc = vv % 100;
		int idx = 0;
		AosXmlTagPtr crttag = child;
		docsource = "modified";
		// bool tagJustSelected = false;
		while (idx < numMods)
		{
			// if (!tagJustSelected && cc < 30)
			// {
			// 	// Select a child to modify
			// 	crttag = selectSubtag(child);
			// 	tagJustSelected = true;
			// 	continue;
			// }

			idx++;
			if (     cc <= 10) addAttr(crttag);
			else if (cc <= 20) deleteAttr(crttag);
			else if (cc <= 30) modifyAttr(crttag);
			else if (cc <= 40) addTextWords(crttag);
			else if (cc <= 50) removeTextWords(crttag);
			else if (cc <= 60) modifyTexts(crttag);
			else if (cc <= 70) addTexts(crttag);
			else if (cc <= 80) removeTexts(crttag);
			else if (cc <= 90) addTag(crttag);
			else 			   removeTag(crttag);

			char *data = (char *)child->getData();
			int len = child->getDataLength();

			OmnString ss(data, len);
			AosXmlParser parser;
			AosXmlTagPtr tag = parser.parse(ss, "");
			if (!tag)
			{
				OmnScreen << "Previous opr: " << mCrtOpr << endl;
				OmnScreen << ss << endl;
			}
		}

		char *dd = (char *)child->getData();
		int len = child->getDataLength();
		docstr.assign(dd, len);
	}

	// Need to construct a request, which is in the form:
	//	<request>
    //		<item name='username'>chending</item>
    //		<item name='appname'>testapp</item>
	//		<xmlobj>
	//			the xml generate above
	//		</xmlobj>
	//	</request>

	OmnString appname = "testapp";
	appname << mNumModified;

	OmnString user = "chending";
	user << mNumModified;

	OmnString ctnr = "ctnr";
	ctnr << mNumModified;

	// Convert the generated XML into a string
	// Add the header to the generated xml
	OmnString hh = 
		"<request><item name=\"operation\">serverCmd</item><item name='zky_uname'>";
	hh << user << "</item><item name='appname'>"
		<< appname << "</item><item name='zky_siteid'>"
		<< mSiteid << "</item><item name='containers'>"
		<< ctnr << "</item>"
		<< "<cmd opr=\"saveobj\"/>"
		<< "<xmlobj>"
		<< docstr << "</xmlobj></request>";

	AosXmlParser parser;
	root = parser.parse(hh, "");
	aos_assert_r(root, false);
	AosXmlTagPtr xmldoc;
	aos_assert_r(root && 
				(xmldoc = root->getFirstChild()) &&
				(xmldoc = xmldoc->getFirstChild("xmlobj")), false);
	aos_assert_r(addMetadata(xmldoc), false);
	xmldoc->setAttr(AOSTAG_DOCID, mDocid);
	hh = (char*)root->getData();

	OmnString errmsg;
	OmnString resp;
	// OmnScreen << "Request: " << hh << endl;
	mConn->procRequest(mSiteid, appname, user, hh, resp, errmsg);
	aos_assert_r(resp != "", false);
	// OmnScreen << "Response: " << resp << endl;

	// Check the response. Response should be in the form:
	// 	<response error="false" 
	// 		code="200"
	// 		docid="xxx"/>
	AosXmlDocPtr header;
	root = parser.parse(resp, "");
	child = root->getFirstChild();
	aos_assert_r(child, false);
	child = child->getFirstChild("status");
	aos_assert_r(child, false);
	aos_assert_r(child->getAttrStr("code") == "200", false);
	mTorturer->docModified(mSiteid, mDocid, docstr);
	return true;
}


bool
AosModifyDocThrd::signal(const int threadLogicId)
{
	return true;	
}


bool    
AosModifyDocThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosModifyDocThrd::addMetadata(const AosXmlTagPtr &doc)
{
	OmnString word = "docname";
	word << rand();
	doc->setAttr(AOSTAG_OBJNAME, word);

	word = "tnail";
	word << rand();
	doc->setAttr(AOSTAG_TNAIL, word);

	word = "doctype";
	word << rand();
	doc->setAttr(AOSTAG_OTYPE, word);

	word = "subtype";
	word << rand();
	doc->setAttr(AOSTAG_SUBTYPE, word);

	word = "appname";
	word << rand();
	doc->setAttr(AOSTAG_APPNAME, word);

	return true;
}


bool
AosModifyDocThrd::addAttr(const AosXmlTagPtr &crttag)
{
	// It adds a new attribute to the tag. 
	// OmnScreen << "To add add!" << endl;
	mCrtOpr = "addattr";
	int num = 0;
	OmnString aname;
	while (num < 10)
	{
		aname = AosTestXmlDoc::getAttrname(mSiteid);
		if (aname == AOSTAG_SITEID ||
			aname == AOSTAG_DOCID ||
			aname == AOSTAG_VERSION ||
			aname == AOSTAG_OBJID) return true;

		OmnString vv = crttag->getAttrStr(aname, "");
		if (vv == "") break;
		num++;
	}

	OmnString avalue = AosTestXmlDoc::getAttrvalue(aname, mSiteid, mDocid);
	aos_assert_r(avalue != "", false);
	if (!crttag->xpathSetAttr(aname, avalue)) return true;

	// Need to save the attribute to the db
	OmnString stmt = "insert into ";
	stmt << mAttrTname << " (siteid, docid, aname, avalue) values ('";
	stmt << mSiteid << "', '" << mDocid << "', '";
	stmt << aname << "', '" << avalue << "')";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store->runSQL(stmt), false);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::deleteAttr(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To delete attr" << endl;
	mCrtOpr = "delattr";
	int num = crttag->getNumAttrs();
	if (num == 0) return true;
	num = rand() % num;
	OmnString aname, avalue;
	aos_assert_r(crttag->getAttr(num, aname, avalue), false);

	// We cannot delete AOSTAG_SITEID or AOSTAG_OBJID
	if (aname == AOSTAG_SITEID || 
		aname == AOSTAG_OBJID ||
		aname == AOSTAG_VERSION ||
		aname == AOSTAG_DOCID) return true;

	OmnString name = crttag->removeAttr(num);
	aos_assert_r(name != "", false);

	// Need to remove the name from db
	OmnString stmt = "delete from ";
	stmt << mAttrTname << " where siteid='" << mSiteid
		<< "' and docid='" << mDocid << "' and aname='" << name << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store->runSQL(stmt), false);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::modifyAttr(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To modify attr" << endl;
	mCrtOpr = "modattr";
	int num = crttag->getNumAttrs();
	if (num == 0) return true;
	num = rand() % num;
	OmnString aname, oldvalue;
	aos_assert_r(crttag->getAttr(num, aname, oldvalue), false);

	if (aname == AOSTAG_SITEID || 
		aname == AOSTAG_OBJID ||
		aname == AOSTAG_VERSION ||
		aname == AOSTAG_DOCID) return true;

	OmnString newvalue = AosTestXmlDoc::getAttrvalue(aname, mSiteid, mDocid);
	if (newvalue == "")
	{
		newvalue << rand();
	}

	aos_assert_r(crttag->setAttr(aname, newvalue), false);

	OmnString stmt = "update ";
	stmt << mAttrTname << " set avalue='"
		<< newvalue << "' where siteid='" << mSiteid
		<< "' and docid='" << mDocid << "' and aname='" << aname
		<< "' and avalue='" << oldvalue << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store->runSQL(stmt), false);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::addTextWords(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To add text words!" << endl;
	mCrtOpr = "addtextword";
	int len;
	char *data = (char *)crttag->getNodeText(len);
	OmnString texts(data, len); 
	OmnString word = AosTestXmlDoc::getDocWord();
	if (texts.length() == 0) texts = word;
	else texts.replace(rand() % texts.length(), 0, word);
	crttag->setText(texts, (rand()%100) > 60);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::removeTextWords(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To remove text words" << endl;
	mCrtOpr = "removetextword";
	int len;
	char *data = (char *)crttag->getNodeText(len);
	OmnString texts(data, len);

	OmnString word = AosTestXmlDoc::getDocWord();
	if (texts.length() == 0) return true;

	int vv = rand();
	int removelen = vv % texts.length();
	int nn = texts.length() - removelen;
	int idx = 0;
	if (nn > 0) idx = vv % nn;

	// Make sure it does not break '<![CDATA[' or ']]>'
	if (len >= 12 && strncmp(data, "<![CDATA[", 9) == 0)
	{
		if (idx < 9 || idx >= len-3) return true;
	}
	texts.replace(idx, removelen, "");
	crttag->setText(texts, (rand()%100) > 60);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::modifyTexts(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To modify texts!" << endl;
	mCrtOpr = "modifytexts";
	int len;
	char *data = (char *)crttag->getNodeText(len);
	OmnString texts(data, len);
	OmnString word = AosTestXmlDoc::getDocWord();
	if (texts.length() == 0) return true;

	int vv = rand();
	int removelen = vv % texts.length();
	int nn = texts.length() - removelen;
	int idx = 0;
	if (nn > 0) idx = vv % nn;

	OmnString newstr = AosTestXmlDoc::getTexts();
	texts.replace(idx, removelen, newstr);
	crttag->setText(texts, (rand()%100) > 60);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::addTexts(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To add text!" << endl;
	mCrtOpr = "addtexts";
	OmnString newstr = AosTestXmlDoc::getTexts();
	crttag->setText(newstr, (rand()%100) > 60);
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::removeTexts(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To remove texts" << endl;
	mCrtOpr = "removetexts";
	crttag->removeTexts();
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


bool
AosModifyDocThrd::addTag(const AosXmlTagPtr &crttag)
{
	mCrtOpr = "addtag";
	// OmnScreen << "To add tag" << endl;
	// Currently not supported yet.
	/*
	int vv = rand() % 100;
	int level;
	if (vv < 50) level = 0;
	else if (vv < 90) level = 1;
	else level = 2;

	AosTestXmlDocPtr doc = OmnNew AosTestXmlDoc("", level);
	aos_assert_r(doc, false);
	OmnString str;
	doc->composeData(str);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(str, "");
	AosXmlTagPtr node = root->getFirstChild();
	aos_assert_r(node, false);
	aos_assert_r(crttag->addNode(node), false);
	*/
	return true;
}


bool
AosModifyDocThrd::removeTag(const AosXmlTagPtr &crttag)
{
	// OmnScreen << "To remove tag" << endl;
	mCrtOpr = "removetag";
	int vv = rand() % 100;
	if (vv < 20)
	{
		int num = crttag->getNumSubtags();
		if (num == 0) return true;
		int idx = rand() % num;
		aos_assert_r(crttag->removeNode(idx), false);
	}
	else
	{
		crttag->removeNode();
	}
	// OmnScreen << (char *)crttag->getData() << endl;
	return true;
}


AosXmlTagPtr
AosModifyDocThrd::selectSubtag(const AosXmlTagPtr &child)
{
	OmnNotImplementedYet;
	return 0;
}


