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
// 12/16/2009: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/Passwd.h"

#include "alarm_c/alarm.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "TinyXml/TinyXml.h"
#include "XmlInterface/Ptrs.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "Util/File.h"
#include "UtilComm/TcpClient.h"


static OmnString sgDirname = "/usr/AOS/Data";
static OmnString sgFnameForgotPwdPreface = "ForgotPwdPreface";
static OmnString sgFnameForgotPwdPost = "ForgotPwdPost";
static OmnString sgForgotPwdPreface;
static OmnString sgForgotPwdPost;
static OmnString sgForgotPasswdSubline = "Password Recovery from Zykie";
static OmnString sgForgotPasswdSender = "admin@zykie.com";

AosPasswdSvr::AosPasswdSvr()
{
}


AosPasswdSvr::~AosPasswdSvr()
{
}


bool
AosPasswdSvr::sendmail(
		const OmnString &username,
		const AosNetRequestPtr &req, 
		TiXmlElement *childelem,
		OmnString &contents,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	// It is to send an email. 
	// 'childelem' should be in the form:
	//	<operation opr="sendmail"
	//		type="xxx"
	//		receiver="xxx"
	//	</operation>
	
	OmnString type = childelem->Attribute("type");
	if (type == "passwd")
	{
		if (sgForgotPwdPreface == "") 
		{
			if (!readFile())
			{
				errmsg = "System Error (1001)!";
				OmnAlarm << errmsg << enderr;
				errcode = eAosXmlInt_General;
				return false;
			}
		}

		// It is 'forgot password'.
		OmnString receiver = childelem->Attribute("rmailaddr");
		if (receiver == "")
		{
			errmsg = "Missing the receiver email address!";
			OmnAlarm << errmsg << enderr;
			errcode = eAosXmlInt_General;
			return false;
		}

		if (username == "")
		{
			errmsg = "Missing the username!";
			OmnAlarm << errmsg << enderr;
			errcode = eAosXmlInt_General;
			return false;
		}

		// Create the file
		OmnString passwd = generatePwd();
		OmnString fname = "/tmp/forgotpwd/";
		fname << username << "_" << passwd;
		OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if (!ff->isGood())
		{
			system("mkdir /tmp/forgotpwd");
			if (!ff->openFile1(fname, OmnFile::eCreate) || !ff->isGood())
			{
				errmsg = "System error (1002)!";
				OmnAlarm << errmsg << enderr;
				errcode = eAosXmlInt_General;
				return false;
			}
		}

		// Create the contents.
		ff->append(sgForgotPwdPreface, false);
		ff->append(passwd, false);
		ff->append(sgForgotPwdPost, true);
		//ff->closeFile();
		ff = 0;

		// Construct the command:
		// 	"cat <thepath>/forgot_password | mail -s <subject> <receiver_addr>
		// 		-- -f <sender_addr>
		OmnString cmd = "cat ";
		cmd << fname << " | mail -s '"
			<< sgForgotPasswdSubline << "' " << receiver 
			<< " -- -f " << sgForgotPasswdSender;
		OmnTrace << "To send email: " << cmd << endl;
		system(cmd.data());
	
		cmd = "rm ";
		cmd << fname;
		errcode = eAosXmlInt_Ok;
		// system(cmd);
		return true;
	}

	errcode = eAosXmlInt_IncorrectOperation;
	errmsg = "Operation failed!";
	return false;
}

	
OmnString 
AosPasswdSvr::generatePwd()
{
	// This function generates a new password, which is 7
	// to 10 chars long and contains only letters and digits.
	char word[100];
	int len = (rand() % 4) + 7;
	int idx = 0;
	while (idx < len)
	{
		char c = (rand() % 94) + 32;
		if ((c >= 'a' && c <='z') ||
			(c >= 'A' && c <='Z') ||
			(c >= '0' && c <= '9'))
		{
			word[idx++] = c;
		}
	}
	word[len] = 0;
	return word;
}


bool
AosPasswdSvr::readFile()
{
	OmnString fname = sgDirname;
	fname << "/" << sgFnameForgotPwdPreface;
	OmnFile ff(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(ff.isGood(), false);
	aos_assert_r(ff.readToString(sgForgotPwdPreface), false);

	fname = sgDirname;
	fname << "/" << sgFnameForgotPwdPost;
	OmnFile ff1(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	aos_assert_r(ff1.isGood(), false);
	aos_assert_r(ff1.readToString(sgForgotPwdPost), false);
OmnTrace << "Preface: " << sgForgotPwdPreface << endl;
OmnTrace << "Post: " << sgForgotPwdPost << endl;
	return true;
}

