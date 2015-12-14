package com.zykie.actors.jdbc;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.zykie.actors.IActor;
import com.zykie.actors.ParserXmlDoc;
import com.zykie.jdbc.QueryResponse;
import com.zykie.jdbc.ResultSet;
import com.zykie.util.JDBCFactory;

public class ActDatabase implements IActor{

	@Override
	public boolean run(String msg, StringBuffer response){
		Map<Key,List<String>> map = ParserXmlDoc.xmlElements(msg);
		QueryResponse resp = null;
//		String operate = map.get("operate");
		Set<Key> keys = map.keySet();
		Iterator<Key> it = keys.iterator();
		System.out.println(keys.size()+"=size");
		String operate = "";
		String databasename = "";
		StringBuffer column = new StringBuffer();
		while(it.hasNext()) {
			Key key = it.next();
			String name = key.getName();
			if(name.equalsIgnoreCase("operate")) {
				
				operate = map.get(key).get(0);
				System.out.println("operate="+operate);
			} else if(name.equalsIgnoreCase("databasename")) {
				databasename = map.get(key).get(0);
			} 
		}
		System.out.println("operate="+operate);
		boolean flag = false;
		try {
			if(operate.equalsIgnoreCase("show")) {
				resp = showDatabase();
				flag = !resp.getError();
//				System.out.println("fields="+resp.getFields().getClass()+"\nrecord="+resp.getRecords());
				if(resp.getRecords() != null) {
					Iterator its = resp.getRecords().iterator();
					while(it.hasNext()) {
						response.append("<databasename>");
						response.append(its.next().toString());
						response.append("</databasename>");
					}
				}
				
			} else {
				if(operate.equalsIgnoreCase("create")) {
					resp = createDatabase(databasename);
					
				} else if(operate.equalsIgnoreCase("drop")) {
					resp = dropDatabase(databasename);
				} 
				flag = !resp.getError();
				response.append(resp.getMsg());
			}
		} catch (SQLException e) {
			
			e.printStackTrace();
		}
		return flag;
	}
	
	private QueryResponse common(String sql) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		stmt.execute(sql);
		ResultSet rs=(ResultSet) stmt.getResultSet();
		System.out.println(rs.getQueryResponse().getMsg()+"=============");
		JDBCFactory.close(conn, rs, stmt);
		QueryResponse resp = rs.getQueryResponse();
		return resp;
	}
	//创建数据库
	private QueryResponse createDatabase(String databasename) throws SQLException{
		String sql="create database "+databasename;
		return common(sql);
	}
	//显示数据库列表
	private QueryResponse showDatabase() throws SQLException{
		String sql="show databases";
		return common(sql);
	}

	//描述数据库
	private QueryResponse dropDatabase(String databasename)throws SQLException{
		String sql="drop database "+databasename;
		return common(sql);
	}
	
	public static void main(String[] args) {
		String msg ="<obj><databasename>db1</databasename><operate>create</operate></obj>";
		StringBuffer response = new StringBuffer();
		System.out.println(new ActDatabase().run(msg, response));
		System.out.println("response="+response);
	}
}
