package com.zykie.jdbc.test;
import java.util.*;
import java.sql.*;

public class TestJDBC {

	public static void main(String args[]) {
		
		String drv = null;
		Connection conn = null;
		Statement stmt = null;
		ResultSet rs = null;
		

		Properties p = new Properties();
		
		try {
			// 注册数据库驱动程序为zykie驱动
			Class.forName("com.zykie.jdbc.Driver");
		} catch (java.lang.ClassNotFoundException e) {
			System.err.println("error : " + e.getMessage());
		}

		try {
			conn = DriverManager.getConnection(
					"jdbc:zykie://192.168.99.180:2011/db1", "root",
					"12345");
		} catch (SQLException ex) {
			System.err.println("conn:" + ex.getMessage());
		}

		if (conn != null)
			System.out.println("connection successful");
		else
			System.out.println("connection failure");

		try {
			stmt = conn.createStatement();

		} catch (SQLException ex) {
			System.err.println("createStatement();" + ex.getMessage());

		}

	/*	
		try {
	//		rs = stmt.executeQuery("insert into tab13 (age, name, marriage) values(131, \"ghg\", 68);");
			int r = stmt.executeUpdate("insert into tab13 (age, name, marriage) values(11, \"ttt3\", 82);");
			System.out.println("result : " +  r);

		} catch (SQLException ex) {
			System.err.println("stmt.excuteQuery();" + ex.getMessage());

		}
	*/	
		
		
		try {
			rs = stmt.executeQuery("select note from \"luoan_liuyan\"");

		} catch (SQLException ex) {
			System.err.println("stmt.excuteQuery();" + ex.getMessage());

		}
		
		try {
			while (rs.next()) {
				
				System.out.println("1: " + rs.getString("note"));
				//System.out.println("2: " + rs.getString("zky_title"));

			}

		} catch (SQLException ex) {
			System.err.println("A ERROR is failure" + ex.getMessage());

		}

		// test1();
		p = System.getProperties();

		// p.list(System.out);
		drv = p.getProperty("java.class.path");
		// System.out.println("driver="+drv);

	}

	public static void test1() {
		cn2 cn2obj = new cn2();
		printClassName(cn2obj);
	}

	public static void printClassName(Object obj) {
		System.out.println("The class of " + obj + " is "
				+ obj.getClass().getName());
	}

}

class cn2 {

	public void test() {
		System.out.println("11111111111");

	}

	public String toString() {
		return "1111";
	}

}