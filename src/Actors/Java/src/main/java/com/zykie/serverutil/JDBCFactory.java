package com.zykie.serverutil;

import java.io.IOException;
import java.sql.*;
import java.util.Properties;

public class JDBCFactory {
	private static String className;
	private static String url;
	private static String user;
	private static String password;
	static{//加载类时就执行，且只执行一次
		Properties info=new Properties();
		try{
			info.load(JDBCFactory.class.getResourceAsStream("/com/zykie/serverutil/db.properties"));
			className=info.getProperty("serverclassName");
			url=info.getProperty("serverurl");
			user=info.getProperty("serveruser");
			password=info.getProperty("serverpassword");
		}catch(IOException e){
			e.printStackTrace();
		}
	}
	public static Connection getConnection(){
		Connection conn=null;
		try{
			Class.forName(className);
			conn=DriverManager.getConnection(url, user, password);
		}catch(ClassNotFoundException e){
			e.printStackTrace();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return conn;
	}
	public static void close(ResultSet rs,Statement stmt,Connection conn){
		try{
			if(rs!=null){
				rs.close();
			}
			if(stmt!=null){
				stmt.close();
			}
			if(conn!=null){
				conn.close();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	public static void main(String[] args) throws SQLException{
		
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		
		String sql="select name,classname,classpath,type_language,description from \"_jimoActors\"";
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		while(rs.next()){
			System.out.println(rs.getString(1));
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		/*String sql="create table \"luoan_留言\"(contact_unit varchar(50),contact varchar(50),zky_schoolLaoShi_name varchar(50),zky_schoolLaoShi_cloudid varchar(50)," +
				"zky_schoolLaoShi_uname varchar(50),zky_instdate varchar(50),contact_phone varchar(50),contact_Email varchar(50),zky_shouLiLaoShi_cloudid varchar(50),note varchar(200)" +
				",zky_status varchar(50),zky_liuyan_ispublic varchar(50))";*/
		
		
		/*String sql="insert into \"luoan_留言\"(contact_unit,contact,zky_schoolLaoShi_name,zky_schoolLaoShi_cloudid," +
		"zky_schoolLaoShi_uname,zky_instdate,contact_phone,contact_Email,zky_shouLiLaoShi_cloudid,note" +
		",zky_status,zky_liuyan_ispublic)" +
		"values('fhfghfghschool','jack','fge','12345678','123456ertg78'," +
		"'2013-03-05 05:03:36','12345678','123@163.com','757','Ha ha, I have not money. Get me some'" +
		",'待回复','私有')";
		
		stmt.execute(sql);*/
		
		/*String sql="select contact_unit,zky_schoolLaoShi_uname,zky_instdate,zky_schoolLaoShi_cloudid,zky_shouLiLaoShi_cloudid from \"luoan_留言\"";
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		while(rs.next()){
			//System.out.println("dgdfgdfg");
			System.out.println(rs.getString(1)+":"+rs.getString(2)+":"+rs.getString(3)+":"+rs.getString(4)+":"+rs.getString(5));
		}*/
		JDBCFactory.close(null, null, conn);
	}

}
