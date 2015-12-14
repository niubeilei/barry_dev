#include "zykie.h"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
int gAosShowNetTraffic = 0;
static char* opt_host_name = "0.0.0.0"; /*服务器主机名称 默认为localhost*/
static char *opt_user_name = "root"; /*数据库用户名 默认为当前登录名*/
static char *opt_password = "zykie"; /*密码 默认为空*/
static uint opt_port_num = 27080; /*端口 使用内建值*/
static char *opt_socket_name = NULL; /*socket name (use build-in value)*/
static char *opt_db_name = "luaRules"; /*数据库 名称 默认为空*/
static uint opt_flags = 0; /*连接参数*/

int main()
{
	// call a function
	ZYKIE* conn;
	if( (conn = zykie_init(NULL))== NULL){
		fprintf(stderr,"zykie 初始化失败(可能是内存溢出)!\n");
	}
	else
	{
		fprintf(stdout,"zykie init sucess\n");
	}
	
	if(zykie_connect(conn,opt_host_name,opt_port_num,opt_user_name,opt_password))
	{
		fprintf(stdout,"zykie_connect success\n");
	}
	else
	{
		fprintf(stderr,"zykie_connect 失败:\nError %u (%s)\n",zykie_errno(conn),zykie_error(conn));
		zykie_close(conn);
		exit(1);
	}

	if(zykie_query(conn,"create database mydb;"))
	{
		printf("create database success\n");
	}
	else
	{
		fprintf(stderr,"create database failed\n");
		exit(1);
	}

	//select database
	if(zykie_query(conn,"use mydb;"))
	{
		printf("use database success\n");
	}
	else
	{
		fprintf(stderr,"use database failed\n");
		exit(1);
	}


	//create table
	if(zykie_query(conn,"create table t2 (name char(20),age int);"))
	{
		printf("create table success\n");
	}
	else
	{
		fprintf(stderr,"create table failed\n");
		exit(1);
	}


	//insert record
	if(zykie_query(conn,"insert into t2 (name,age) values ('tom','16');"))
	{
		printf("插入成功,受影响行数:%lu\n",(ulong)zykie_affected_rows(conn));
	}
	else
	{
//		print_error(conn,"执行插入失败");
		exit(1);
	}


	//select query
	if(zykie_query(conn,"select * from t2;"))
	{
		ZYKIE_RES *res_set;
		res_set = zykie_store_result(conn); /*生成结果集*/
		if(res_set == NULL)
		{
//			print_error(conn,"zykie_store_result failed");
		}
		else
		{
//			process_result_set(conn,res_set);
			zykie_free_result(res_set);
		}
	}
	else
	{
//		print_error(conn,"zykie_query() error");
	}

	zykie_close(conn);

	return 0;
}
