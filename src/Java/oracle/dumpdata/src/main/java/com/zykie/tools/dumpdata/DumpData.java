
package com.zykie.tools.dumpdata;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.io.*;
import java.sql.*;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;
import org.apache.commons.io.IOUtils;

public class DumpData {
	// 根据Unicode编码完美的判断中文汉字和符号 
	private static boolean isChinese(char c) {
		Character.UnicodeBlock ub = Character.UnicodeBlock.of(c);
		if (ub == Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS
				|| ub == Character.UnicodeBlock.CJK_COMPATIBILITY_IDEOGRAPHS
				|| ub == Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A
				|| ub == Character.UnicodeBlock.CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B
				|| ub == Character.UnicodeBlock.CJK_SYMBOLS_AND_PUNCTUATION
				|| ub == Character.UnicodeBlock.HALFWIDTH_AND_FULLWIDTH_FORMS
				|| ub == Character.UnicodeBlock.GENERAL_PUNCTUATION) {
			return true;
		}
		return false;
	}
	// 完整的判断中文汉字和符号 
	public static boolean isChinese(String strName) {
		char[] ch = strName.toCharArray();
		for (int i = 0; i < ch.length; i++) {
			char c = ch[i];
			if (isChinese(c)) {
				return true;
			}
		}
		return false;
	}
	//json file
	//static String paramFile = "E:/cvsrepo/AOS/AOS/src/JDBC/src/main/resources/DumpConfig.txt";
	static String paramFile = "";
	
	//parameters to connect to database system
	//support mysql and oracle for now
	static String dbType = "mysql";
	static String url = "";
	static String driver = "";
	static String user = "";
	static String passwd = "";
	static String dumpDir = "";
	static JSONArray databases = null;
	
	//global static vars
	static Connection conn = null;
	static Statement stmt = null;
	static ResultSet res;
	static int progressRows = 100000;
	static int cycleRows = 0;

	public static String generateStmt(String dbName, String tableName, int cycles)
	{
		String str;
		int start;		
		int end;
		
		if (cycleRows == 0)
		{
			// select all data from table in one cycle			
			if (dbType.toLowerCase().equals("mysql"))
				str = "select * from " + dbName + "." + tableName;
			else  //oracle doesn't need dbName
				str = "select * from " + tableName;
		} else {			

			//need to select data cycle by cycle
			if (dbType.toLowerCase().equals("mysql"))
			{
				//mysql uses limit syntax. Starting from 0
				start = cycles * cycleRows;		
				str = "select * from " + dbName + 
						"." + tableName + " limit " + start + ", " + cycleRows;
			} else { 
				//oracle use rownum syntax and subquery, starting from 1
				start = cycles * cycleRows + 1;		
				end = start + cycleRows - 1;
				str = "select * from (select rownum as no, " + tableName + ".* from " + tableName + 
						" where rownum <= " + end + ") where no >= " + start;
			}
		}
		
		System.out.println("Query string: " + str);
		return str;		
	}
	
	public static boolean table2CSV(String dbName, String tableName)
			throws IOException {

		FileWriter fw;
		String data = "";
		int dumpedRows = 0;
		int totalRows = 0;
		long startTime;
		long endTime;
		long timeUsed;
		int columnCount;
		int cycles = 0;

		try {
			// stmt = conn.createStatement();
			System.out.println("Start to dump data from table: " + tableName);
			startTime = System.currentTimeMillis();	
			fw = new FileWriter(dumpDir + File.separator + dbName
					+ File.separator + tableName + ".txt");

			List<String> columnsNameList = new ArrayList<String>();			
			while (true) 
			{
				res = stmt.executeQuery(generateStmt(dbName, tableName, cycles));	
				// column count is necessay as the tables are dynamic and we
				// need to figure out the numbers of columns
				columnCount = getColumnCount(res);
				
				// this loop is used to add column names at the top of file
				// , if you do not need it just comment this loop
				// for (int i = 1; i <= columnCount; i++) {
				// 		fw.append(res.getMetaData().getColumnName(i));
				// 		fw.append(",");
				// }
				// fw.append(System.getProperty("line.separator"));
				
				int startCol = 1;
				//if select records in multiple cycles from oracle, there will be
				//a _no field prepended whiched need to be removed
				if (cycleRows > 0 && dbType.toLowerCase().equals("oracle"))
					startCol = 2;
				
				while (res.next()) {					
					for (int i = startCol; i < columnCount; i++) {
						// you can update it here by using the column type
						// but i am fine with the data so just converting
						// everything to string first and then saving
						if (res.getObject(i) != null)
							data = res.getObject(i).toString();
						else
							data = "";
						//如果有双引号，则替换为两个双引号
						data=data.replaceAll("\"","\"\"");
						//if(isChinese(data)){
						//所有字段都用“"”括起来
						data="\""+data+"\"";
						//}
						fw.append(data);
						/*
						 * 修改，去掉了空格
						 */
						fw.append(",");
					}
	
					// the last field doesn't need ","
					if (res.getObject(columnCount) != null)
						data = res.getObject(columnCount).toString();
					else
						data = "";
					data=data.replaceAll("\"", "\"\"");
					//if(isChinese(data)){
					data="\""+data+"\"";
					//}
					fw.append(data);
	
					// new line entered after each row
					fw.append(System.getProperty("line.separator"));
					dumpedRows++;				
					if (progressRows > 0 && dumpedRows % progressRows == 0) {
						endTime = System.currentTimeMillis();
						timeUsed = (endTime - startTime) / 1000;
						System.out.println("Dumped " + dumpedRows + 
								" records from table: " + tableName + ". Using " + timeUsed + " seconds.");
					}				
				}
	
				//cycleRows = 0 means all the data is selected in one cycle
				if (cycleRows == 0)
					break;
				
				//all the records have been dumped
				//Ater traversing, row id is after the last one which is
				//the total number of rows
				if (dumpedRows < (cycles + 1) * cycleRows)
					break;
				
				cycles++;
			}
			
			//dump data to the file
			fw.flush();
			fw.close();			
			
			endTime = System.currentTimeMillis();
			timeUsed = (endTime - startTime) / 1000;			
			System.out.println("Taken " + timeUsed + 
					" seconds to dump " + dumpedRows + " records from table: " + tableName);
			System.out.println("--------------------------------------------------");

		} catch (SQLSyntaxErrorException e) {
			
			System.out.println("Skip table: " + tableName + ". No permission.");
			System.out.println("--------------------------------------------------");
			return false;
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return true;
	}

	public static boolean db2CSV(JSONObject database) throws IOException {

		String dbName;
		JSONArray tableArray;
		List<String> tableNameList = new ArrayList<String>();
		String name = "";
		
		try {
			//get parameters from jsonObject
			dbName = database.getString("name");
			
			if (database.has("tables"))
			{
				//tables are from jsonArray
				tableArray = database.getJSONArray("tables");
				for (int i = 0; i < tableArray.size(); i++)
				{
					name = tableArray.getString(i);
					tableNameList.add(name);
				}
			} else {
				// All the tables in the database need to be dumped
				// this query gets all the tables in your database(put your db name
				// in the query)
				if (dbType.toLowerCase().equals("oracle"))
				{
					//oracle connection should already has database or SID information
					DatabaseMetaData dmd = conn.getMetaData();
					res = dmd.getTables(null, null, null, new String[] { "TABLE" });
					while (res.next()) {
						tableNameList.add(res.getString("TABLE_NAME"));						
					}
					
				} else 	{
					//should be mysql
					res = stmt
						.executeQuery("SELECT table_name FROM INFORMATION_SCHEMA.TABLES WHERE table_schema = '"
								+ dbName + "'");
	
					// Preparing List of table Names			
					while (res.next()) {
						tableNameList.add(res.getString(1));
					}
				} 
			}

			// create db folder
			File dbFile = new File(dumpDir + File.separator + dbName);
			if (!dbFile.exists())
				dbFile.mkdirs();

			// start iterating on each table to fetch its data and save in a
			// .csv file
			System.out.println();
			System.out.println("******************************************");
			System.out.println("Start to dump data from database: " + dbName);
			System.out.println("******************************************");
			for (String tableName : tableNameList) {
				table2CSV(dbName, tableName);
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		return true;
	}

	// to get numbers of rows in a result set
	public static int getRowCount(ResultSet res) throws SQLException {
		res.last();
		int numberOfRows = res.getRow();
		res.beforeFirst();
		return numberOfRows;
	}

	// to get no of columns in result set
	public static int getColumnCount(ResultSet res) throws SQLException {
		return res.getMetaData().getColumnCount();
	}

	// collect parameters from a json file
	public static boolean parseConfig() {
		
		try {
		    InputStream is = new FileInputStream(new File(paramFile));
		    String config = IOUtils.toString(is);
			
			JSONObject params = (JSONObject)JSONSerializer.toJSON(config);
			dbType = params.getString("dbType");
			url = params.getString("url");
			driver = params.getString("driver");
			user = params.getString("user");
			passwd = params.getString("passwd");
			dumpDir = params.getString("dumpDir");
			cycleRows = params.getInt("cycleRows");
			//0 means no limit selections
			if (cycleRows < 0)
				cycleRows = 0;
			
			progressRows = params.getInt("progressRows");
			//0 means no progress display
			if (progressRows < 0)
				progressRows = 0;
			
			// loop array
			databases = params.getJSONArray("databases");

		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("Wrong parameter file!");
			return false;
		} 
		
		return true;
	}

	public static void main(String[] args) {

		try {
			//System.out.println(args.length);
			//for (int i = 0; i < args.length; i++)
			//	System.out.println(args[i]);
			
			//get command line option
			if (args.length != 2 || !args[0].equals("-f"))
			{
				System.out.println("Usage: >java -cp <classpath> DumpData.class -f <\"config filepath\">");
				return;
			} else {							
				paramFile = args[1];
			}
			
			//collect parameters from param file
			if (!parseConfig())
				return;
			
			Class.forName(driver);
			conn = DriverManager.getConnection(url, user, passwd);
			stmt = conn.createStatement();
			
			// dump tables in a db
			for (int i = 0; i < databases.size(); i++)
			{
				db2CSV(databases.getJSONObject(i));
			}

			conn.close();
			stmt.close();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
