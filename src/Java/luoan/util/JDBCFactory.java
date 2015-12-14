package util;

import java.io.IOException;
import java.sql.*;
import java.util.Properties;

import bean.Replay;

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
	public static void main(String[] args) throws SQLException{
		
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		
		//创建留言表
		/*String sql="create table \"luoan_liuyan\"(contact_unit varchar(50),contact varchar(50),zky_schoolLaoShi_name varchar(50),zky_schoolLaoShi_cloudid varchar(50)," +
		"zky_schoolLaoShi_uname varchar(50),zky_instdate varchar(50),contact_phone varchar(50),contact_Email varchar(50),zky_shouLiLaoShi_cloudid varchar(50),note varchar(200)" +
		",zky_status varchar(50),zky_liuyan_ispublic varchar(50))";
		stmt.execute(sql);*/
		
		//插入留言数据
		/*String sql="insert into \"luoan_liuyan\"(contact_unit,contact,zky_schoolLaoShi_name,zky_schoolLaoShi_cloudid," +
		"zky_schoolLaoShi_uname,zky_instdate,contact_phone,contact_Email,zky_shouLiLaoShi_cloudid,note" +
		",zky_status,zky_liuyan_ispublic)" +
		"values('fhfghfghschool','jack','fge','12345678','123456ertg78'," +
		"'2013-03-05 05:03:36','12345678','123@163.com','757','Ha ha, I have not money. Get me some'" +
		",'daihuifu','private')";*/
		/*String sql="insert into \"luoan_liuyan\"(contact_unit,contact,zky_schoolLaoShi_name,zky_schoolLaoShi_cloudid," +
		"zky_schoolLaoShi_uname,zky_instdate,contact_phone,contact_Email,zky_shouLiLaoShi_cloudid,note" +
		",zky_status,zky_liuyan_ispublic)" +
		"values('chefangschool','hqj','huqingjing','12345678','123456ertg78'," +
		"'2013-03-08 05:03:36','12345678','123@163.com','757','Ha ha'" +
		",'daihuifu','public')";
		stmt.execute(sql);*/
		//查询留言数据
		/*String n="10";
		String sql="select zky_schoolLaoShi_cloudid,contact_unit,zky_schoolLaoShi_uname,zky_instdate,zky_shouLiLaoShi_cloudid,zky_liuyan_ispublic,zky_objid from \"luoan_liuyan\"" +
				" where zky_liuyan_ispublic='public' order by zky_instdate desc limit 0,"+n;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		while(rs.next()){
			System.out.println(rs.getString(1)+":"+rs.getString(2)+":"+rs.getString(3)+":"+rs.getString(4)+":"+rs.getString(5)+":"+rs.getString(6)+":======"+rs.getString(7));
		}*/
		
		//创建留言回复表
		/*String sql="create table \"luoan_liuyan_huifu\"(zky_handle_date varchar(50),zky_handle_uname varchar(50)," +
				"zky_handle_name varchar(50),zky_handle_cloudid varchar(50),zky_handle_unit varchar(50)," +
				"zky_handle_note varchar(50),zky_status varchar(50),liuyan_id varchar(50))";
		stmt.execute(sql);*/
		//插入回复数据
		/*String sql="insert into \"luoan_liuyan_huifu\"(zky_handle_date,zky_handle_uname," +
				"zky_handle_name,zky_handle_cloudid,zky_handle_unit," +
				"zky_handle_note,zky_status,liuyan_id)values('2013-03-15 05:03:36','cdm','chengdaming','666666','pudong school'," +
				"'OK!','yichakan','0001')";*/
	/*	String sql="insert into \"luoan_liuyan_huifu\"(zky_handle_date,zky_handle_uname," +
		"zky_handle_name,zky_handle_cloudid,zky_handle_unit," +
		"zky_handle_note,zky_status,liuyan_id)values('2013-03-18 05:03:36','tbxh','tuobixiaohei','99999','suzhou school'," +
		"'OK!!!!!','yichakan','0003')";*/
		//stmt.execute(sql);
		
		
		
		
		
		//创建留言表
		/*String sql="create table \"luoan_留言\"(contact_unit varchar(50),contact varchar(50),zky_schoolLaoShi_name varchar(50),zky_schoolLaoShi_cloudid varchar(50)," +
		"zky_schoolLaoShi_uname varchar(50),zky_instdate varchar(50),contact_phone varchar(50),contact_Email varchar(50),zky_shouLiLaoShi_cloudid varchar(50),note varchar(200)" +
		",zky_status varchar(50),zky_liuyan_ispublic varchar(50))";
		stmt.execute(sql);*/
		
		/*String sql="insert into \"luoan_留言\"(contact_unit,contact,zky_schoolLaoShi_name,zky_schoolLaoShi_cloudid," +
		"zky_schoolLaoShi_uname,zky_instdate,contact_phone,contact_Email,zky_shouLiLaoShi_cloudid,note" +
		",zky_status,zky_liuyan_ispublic)" +
		"values('浦东一中','jack','陈大明','12345678','chengdaming'," +
		"'2013-03-01 05:03:36','15250324567','123@163.com','12345679','你好！'" +
		",'待回复','私有')";
		stmt.execute(sql);*/
		
		
		
		/*String sql="create table \"luoan_留言_回复\"(zky_handle_date varchar(50),zky_handle_uname varchar(50)," +
		"zky_handle_name varchar(50),zky_handle_cloudid varchar(50),zky_handle_unit varchar(50)," +
		"zky_handle_note varchar(50),zky_status varchar(50),liuyan_id varchar(50))";
		stmt.execute(sql);*/
		
		/*String sql="insert into \"luoan_留言_回复\"(zky_handle_date,zky_handle_uname," +
		"zky_handle_name,zky_handle_cloudid,zky_handle_unit," +
		"zky_handle_note,zky_status,liuyan_id)values('2013-04-25 05:03:36','cdm','程大明','666666','浦东一中'," +
		"'真好！','已查看','zkydoid0000005055')";
		stmt.execute(sql);*/
		
		/*String sql="drop table \"luoan_留言\"";
		stmt.execute(sql);*/
		
		
		String sql="select zky_objid from \"luoan_留言_回复\"";
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
