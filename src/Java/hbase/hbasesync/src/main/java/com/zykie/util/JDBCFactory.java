package com.zykie.util;

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
			info.load(JDBCFactory.class.getResourceAsStream("db.properties"));
			className=info.getProperty("className");
			url=info.getProperty("url");
			user=info.getProperty("user");
			password=info.getProperty("password");
		}catch(IOException e){
			e.printStackTrace();
		}
	}
	public static Connection getConnection(){
		Connection conn=null;
		try{
			//Class.forName("com.zykie.jdbc.Driver");
			//conn=DriverManager.getConnection("jdbc:zykie://192.168.99.83:9012/db", "root", "12345");
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
	public static boolean isExists(String tablename,String rowkey) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		String sql="select * from "+tablename+" where rowkey=\""+rowkey+"\"";
		ResultSet rs = stmt.executeQuery(sql);
		boolean isexists=false;
		while(rs.next()){
			isexists=true;
		}
		return isexists;
	}

	public static void main(String[] args) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		System.out.println(conn+"888888888");
		Statement stmt=conn.createStatement();
		System.out.println(stmt+"=========");
		/*String sql="select * from sym_node";
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		while(rs.next()){
			System.out.println(rs.getString(1));
		}
		JDBCFactory.close(null, null, conn);
		System.out.println(conn.getCatalog()+"=========");
		ResultSet rs=conn.getMetaData().getTables("syncdb", null, "SYM_NODE", null);
		System.out.println(rs);
		while(rs.next()){
			System.out.println(rs.getString(1));
			System.out.println(rs.getString(2));
			System.out.println(rs.getString(3));
			System.out.println(rs.getString(4));
			System.out.println(rs.getString(5));
			System.out.println(rs.getString(6));
			System.out.println(rs.getString(7));
		}*/
		//JDBCFactory.close(null, null, conn);
		/*String sql = "describe table t8";
		ResultSet rs = stmt.executeQuery(sql);
		System.out.println(rs.getRow());
		while(rs.next()){
			System.out.println(rs.getString(1));
			//System.out.println("sfds");
		}*/
		//System.out.println(isExists("t8", "r1"));
		String tablename="t8";
		String rowkey="r5";
		String sql="select * from "+tablename+" where rowkey=\""+rowkey+"\"";
		//String sql="update "+tablename+" set fm1_c1=\"lll\" where rowkey=\"r2\"";
		ResultSet rs = stmt.executeQuery(sql);
		int columncount=rs.getMetaData().getColumnCount();
		//boolean isexists=false;
		boolean isallempty=true;
		while(rs.next()){
			//isexists=true;
			
			//System.out.println(rs.getMetaData().getColumnCount());
			for(int i=2;i<=columncount;i++){
				if(rs.getString(i) != null){
					isallempty=false;
					break;
				}
			}
			
		}
		System.out.println(isallempty);
		if(isallempty){
			System.out.println("sdgsd");
		}
		//stmt.execute(sql);
		
		
	}

}
