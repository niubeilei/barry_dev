/*
 * test.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: root
 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

#include "zykie.h"

//const static int NULL = 0;

typedef unsigned int uint;

static char* opt_host_name = "192.168.99.180"; /*服务器主机名称 默认为localhost*/
static char *opt_user_name = "root"; /*数据库用户名 默认为当前登录名*/
static char *opt_password = NULL; /*密码 默认为空*/
static uint opt_port_num = 0; /*端口 使用内建值*/
static char *opt_socket_name = NULL; /*socket name (use build-in value)*/
static char *opt_db_name = "luaRules"; /*数据库 名称 默认为空*/
static uint opt_flags = 0; /*连接参数*/



void print_error(ZYKIE *conn, const char *title){
	fprintf(stderr,"%s:\nError %u (%s)\n",title,zykie_errno(conn),zykie_error(conn));
}

void process_result_set(ZYKIE *conn,ZYKIE_RES *res_set)
{
	ZYKIE_ROW row;
	uint i;

	while ((row = zykie_fetch_row(res_set)) != NULL)
	{
		for(i=0;i<zykie_num_fields(res_set);i++)
		{
			if (i > 0)
				fputc('\t',stdout);
			fprintf(stdout,"%s",(row[i] != NULL) ? row[i] : "NULL");
		}
		fputc('\n',stdout);
	}

	if(zykie_errno(conn) != 0)
		print_error(conn,"zykie_fetch_row() failed");
	else
		printf("%lu rows returned \n",(ulong)zykie_num_rows(res_set));
}

int main()
{
	ZYKIE* conn;
	/*初始化数据库指针*/
	if( (conn = zykie_init(NULL))== NULL){
		fprintf(stderr,"zykie 初始化失败(可能是内存溢出)!\n");
		exit(1);
	}
	/*连接到数据库*/
	if(zykie_connect(conn,opt_host_name,NULL,NULL))
	{
		fprintf(stdout,"zykie_connect success\n");
	}
	else
	{
		fprintf(stderr,"zykie_connect 失败:\nError %u (%s)\n",zykie_errno(conn),zykie_error(conn));
		zykie_close(conn);
		exit(1);
	}


	//create database
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
		print_error(conn,"执行插入失败");
		exit(1);
	}


	//select query
	if(zykie_query(conn,"select * from t2;"))
	{
		ZYKIE_RES *res_set;
		res_set = zykie_store_result(conn); /*生成结果集*/
		if(res_set == NULL)
			print_error(conn,"zykie_store_result failed");
		else
		{
			process_result_set(conn,res_set);
			zykie_free_result(res_set);
		}
	}
	else
	{
		print_error(conn,"zykie_query() error");
	}

	/*断开*/
	zykie_close(conn);

	//getchar();
	return 0;
}


