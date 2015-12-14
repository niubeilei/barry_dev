package com.zykie.serverutil;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;


import com.zykie.bean.Actor;

public class GetActorInfo {
	public static Actor getActorInfo(String actname) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement(); 
		String sql="select Name,ClassName,ClassPath,Type_language,Description from \"_jimoActors\"";
		String where=" where Name='"+actname+"'";
		sql=sql+where;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		Actor actor=new Actor();
		while(rs.next()){
			//m=new Message();
			actor.setName(rs.getString(1));
			actor.setClassnmae(rs.getString(2));
			actor.setClasspath(rs.getString(3));
			actor.setDescription(rs.getString(4));
			
		}
		return actor;
	}
	
	public static void main(String[] args) throws SQLException {
		System.out.println(GetActorInfo.getActorInfo("helloWorld").getClassnmae());
	}
}
