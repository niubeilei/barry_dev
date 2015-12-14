//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 04/13/2009: Created by Sharon Shen
//////////////////////////////////////////////////////////////////////////
#include "Client/LogEntry.h"

#include "alarm_c/alarm.h"
#include "util_c/strutil.h"

AosLogEntry::AosLogEntry()
{
	mEntry = OmnNew TiXmlElement("entry");
}


AosLogEntry::~AosLogEntry()
{
}


void
AosLogEntry::setOper(const OmnString oper)
{
	mOper = oper;
}


OmnString
AosLogEntry::getOper()
{
	return mOper;
}


TiXmlElement *
AosLogEntry::getRoot()
{
	return mEntry;
}


//'pathName' should be in the form:"tagName/attributeName, attribute value".
bool
AosLogEntry::addAttribute(const OmnString &pathName, 
		const OmnString &attrValue)
{
	//
	// 'pathName' should be in the form:
	// 		"[tagname/tagname/.../tagname/]attrname"
	
	char *parts[2];
	int num = aos_str_split(pathName.data(), '/', parts, 2);
	aos_assert_r(num == 2, false);
	OmnString tagName = parts[0];
	OmnString attrName = parts[1];
	
	if(tagName == "entry")
	{
		mEntry->SetAttribute(attrName, attrValue);
		return true;
	}

	TiXmlNode * node = 0;
	TiXmlElement *nodes[eMaxNode];
	int idx = 0;
	nodes[idx++] = mEntry;
	while (idx > 0 && idx < eMaxNode)
	{
		TiXmlElement *crtNode = nodes[--idx];
		node = crtNode->FirstChild();
		while (node)
		{
			if (OmnString(node->Value()) == tagName)
			{
				(node->ToElement())->SetAttribute(attrName, attrValue);
				return true;
			}

			TiXmlNode *nodeChild = node->FirstChild();
			if (nodeChild)
			{
				nodes[idx++] = node->ToElement();
			}
			node = node->NextSibling();
		}
	}
			
	OmnAlarm << "Node not found" << enderr;
	return false;
}


bool
AosLogEntry::addTag(const OmnString &pathName, const OmnString &tagValue)
{
	char *parts[2];
	int num = aos_str_split(pathName.data(), '/', parts, 2);
	aos_assert_r(num, 2);
	OmnString tagName = parts[0];
	OmnString addTagName = parts[1];

	TiXmlElement *element = OmnNew TiXmlElement(addTagName);
	TiXmlText *text = OmnNew TiXmlText(tagValue);
	element->LinkEndChild(text);

    if(tagName == "entry")
    {
        mEntry->LinkEndChild(element);
        return true;
    }

	
    TiXmlNode * node = 0;
    TiXmlElement *nodes[eMaxNode];
    int idx = 0;
    nodes[idx++] = mEntry;
    while (idx > 0 && idx < eMaxNode)
    {
        TiXmlElement *crtNode = nodes[--idx];
        node = crtNode->FirstChild();
        while (node)
        {
            if (OmnString(node->Value()) == tagName)
            {
                (node->ToElement())->LinkEndChild(element);
                return true;
            }

            TiXmlNode *nodeChild = node->FirstChild();
            if (nodeChild)
            {
                nodes[idx++] = node->ToElement();
            }
            node = node->NextSibling();
        }
    }

    OmnAlarm << "Node not found" << enderr;
    return false;
}


TiXmlElement *
AosLogEntry::getEntryXml()
{
	return mEntry;
}


AosLogEntryPtr &
AosLogEntry::operator << (const OmnString &value)
{
	mValue << value;
	return *this;
}


AosLogEntry &
AosLogEntry::operator << (const int &value)
{
	mValue << value;
	return *this;
}


void
AosLogEntry::operator << (const AosEndLog &el)
{
	AosLogClientPtr client = AosLogClient::getSelf();
	aos_assert(client);
	AosLogEntryPtr thisPtr(this, false);
	client->addLogEntry(thisPtr);
}

