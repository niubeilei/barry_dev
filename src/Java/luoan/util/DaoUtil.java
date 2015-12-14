package util;

import java.io.IOException;
import java.sql.*;
import java.util.Properties;

public class DaoUtil {
	private static String className;
	private static String url;
	private static String user;
	private static String password;
	static{//加载类时就执行，且只执行一次
		Properties info=new Properties();
		try{
			info.load(DaoUtil.class.getResourceAsStream("db.properties"));
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
			Class.forName(className);
			conn=DriverManager.getConnection(url, user, password);
		}catch(ClassNotFoundException e){
			e.printStackTrace();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return conn;
	}
	
	public static void closeAll(Connection conn,ResultSet rs,Statement stmt){
		try{
			if(conn != null) {
				conn.close();
				conn = null;
			}
			if(rs != null) {
				rs.close();
				rs = null;
			}
			if(stmt != null) {
				stmt.close();
				stmt = null;
			}
		} catch(SQLException e) {
			e.printStackTrace();
		}
		
		
	}

}
