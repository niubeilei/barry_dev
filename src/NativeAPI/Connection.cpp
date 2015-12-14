/*
 * Connection.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: root
 */



#include "NativeAPI/Connection.h"
#include "NativeAPI/zykie.h"





Connection::Connection(const string& host,int port,const string& user,const string& pwd)
{
	mhost=host;
	mport=port;
	muser=user;
	mpwd=pwd;
	mconn = NULL;
}


bool Connection::connect()
{
	OmnString hostname = mhost;
	int port=mport;
	// login database
	OmnString req, resp, err;
	mconn = OmnNew AosSEClient(hostname, port);
	aos_assert_r(mconn, 1);
	if(loginDB())
		return true;
	return false;
}
