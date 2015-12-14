////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
          
#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h" 
#include "AppMgr/App.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "Util/UtUtil.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/StrSplit.h"
#include "UserMgmt/Python.h"
//#include "UserMgmt/Pyemail.h"
  
AosXmlTagPtr gAosAppConfig;
int 
main(int argc, char **argv)
{
	//AosPyemail py[100];
	OmnApp theApp(argc, argv);
	theApp.startSingleton(OmnNew AosPythonSingleton());
	int i = 0;
	for (int i=0; i<1; i++)
	{
		OmnString errmsg;
		//bool rslt = AosPythonSelf->SendEmail("diqiu05@163.com","linxiaoyu315@sina.com", 
		//bool rslt = AosPythonSelf->SendEmail("linxiaoyu315@gmail.com","linxiaoyu315@sina.com", 
		bool rslt = AosPythonSelf->SendEmail("linxiaoyu315@sina.com","linxiaoyu315@gmail.com", 
				"linxiaoyu", "1111", "22222", "smtp.gmail.com", "usesleep");
		//bool rslt = AosPythonSelf->SendEmail("454630293@qq.com","linxiaoyu315@gmail.com", 

		//bool rslt = AosPythonSelf->SendEmail("454630293@qq.com","zykier@163.com", 
		//		"lindalin", "bbbb", "tttt", "smtp.163.com");;
		//aos_assert_r(rslt, 0);
		//bool rslt =  py[i].connPython("454630293@qq.com","zykier@163.com", 
		//		"lindalin", "aaaa", "dddd", "smtp.163.com");
		aos_assert_r(rslt, 0);
		cout << "email: " << i << endl;
	}


	return 0;
} 

