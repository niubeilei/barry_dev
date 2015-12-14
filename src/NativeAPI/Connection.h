/*
 * Connection.h
 *
 *  Created on: Nov 28, 2014
 *      Author: root
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>

using namespace std;

class Connection
{
private:
	string mhost;
	int mport;
	string muser;
	string mpwd;
	AosSEClientPtr mconn;

public:
	Connection(const string& host,int port,const string& user,const string& pwd);

	bool connect();

};



#endif /* CONNECTION_H_ */
