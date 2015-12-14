package com.zykie.hbase;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.util.Bytes;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;
//import java.sql.Date;
import java.sql.Time;
import java.sql.Timestamp;

import java.util.Calendar;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

import com.zykie.jdbc.Statement;
import com.zykie.jdbc.Connection;

public class JimoTable {

	// HBaseConfiguration conf;
	Connection conn;
	static String db = "hbaseDB";
	String table = "";
	String sql = "";
	Statement stmt;
	String colFamilies[];
	String columns[][];
	HashMap<String, String> columnMap = new HashMap<String, String>();
	
	//For saving data to local files
	boolean saveFlag;
	Date dt;
	int count;  //NO. of the file
	int maxSize;
	int curSize;
	
	String rootDir = null;
	String dirSep = File.separator;
	String fileTime = null;
	String fileSuffix = ".csv";
	BufferedWriter bw;
	FileOutputStream out;

	public JimoTable(Connection conn, String table) throws Exception {

		this.conn = conn;
		this.table = table;
		this.count = 0;
		this.maxSize = 10 * 1024 * 1024; //10M 
		this.curSize = 0;
		this.saveFlag = true;
		
		setRootDir(System.getProperty("user.dir")
					+ dirSep + "jimoData" + dirSep);		
		stmt = new Statement(conn, db);
	}

	//we only need to create the db for the first time
	public void createDB() throws Exception {
		// create database dbName;
		//
		sql = "create database " + db + ";";
		stmt.execute(sql);
	}
	
	// create a table in jimo system
	public void create(String table, String[] colFamilies, String columns[][])
			throws Exception {
		
		String cols = "";
		String colFamily = "";
		String[] colArray;
		String jimoColumn;
		
		this.colFamilies = colFamilies;
		this.columns = columns;

		for (int i = 0; i < colFamilies.length; i++) {
			colFamily = colFamilies[i];
			colArray = columns[i];
			
			for (int j = 0; j < colArray.length; j++) {
				if (i != 0 || j != 0)
					cols += ",";
				
				//field name:   _colFamily_field				
				jimoColumn = getColumn(colFamily, colArray[j]);
				cols += jimoColumn + " str"; 
				columnMap.put(jimoColumn, "true");
			}
		}

		// create table db.table
		// 		(col1 str, col2 str, ....);
		//
		sql = "use " + db + ";";
		sql += "create table " + table + " (" + cols + ");";
		stmt.execute(sql);
	}

	public void put(Put put) throws Exception {

		String cols = "";
		String vals = "";
		String column = "";

		cols += "_rowkey";
		vals += "\"" + Bytes.toString(put.getRow()) + "\"";
		for (List<KeyValue> kvList : put.getFamilyMap().values()) {

			// _rowkey will be mapped to JimoDB docid
			for (KeyValue kv : kvList) {
				column = getColumn(
						Bytes.toString(kv.getFamily()), 
						Bytes.toString(kv.getQualifier())
						);
				
				if (!columnMatchTable(column)) 
					continue;				
				
				cols += ", " + column;
				vals += ", " + "\"" + Bytes.toString(kv.getValue()) + "\"" ;
			}
		}

		if (!this.saveFlag) {
			// insert into db.table
			// values (rowkey, valueField1, valueField2, …);
			//
			// In jimoDB, if the docid exist, it will be a modification
			// operation consists
			sql = "use " + db + "; ";		
			sql += "insert into " + table + " (" + cols + ")" + " values"
					+ " (" + vals + ");";
	
			stmt.executeUpdate(sql);
		} else {

			//save the value line to a file
			saveRecord(vals);
		}
			
	}

	public void put(List<Put> puts) throws Exception {

		for (Put put : puts) {
			this.put(put);
		}
	}

	public void delete(Delete delete) throws Exception {

		// delete from db.table
		// where keyField = rowkey;
		//
		// this is the pseudo code. We will use a loop to set the SQL
		sql = "use " + db + "; ";		
		sql += "delete from " + table + " where _rowkey = \""
				+ Bytes.toString(delete.getRow()) + "\";";

		stmt.executeUpdate(sql);
	}

	public void delete(List<Delete> deletes) throws Exception {

		for (Delete delete : deletes) {
			this.delete(delete);
		}
	}
	
	public void useDB() throws Exception {
		// "use hbaseDB;"
		sql = "use " + db + "; ";

		stmt.execute(sql);	
	}	
	
	public String getColumn(String family, String qualifier) {
		
		return "_" + family + "_" + qualifier;
	}
	
	public boolean columnMatchTable(String column) {

		return columnMap.containsKey(column);
	}
	
	public void setSaveFlag(boolean saveFlag) {
		this.saveFlag = saveFlag;
	}
	
	public void setRootDir(String rootDir) {
		this.rootDir = rootDir;
	}
	
	synchronized public void saveRecord(String line) throws Exception {
		//Saving put request into a client file
		//which will be loaded from jimo side later on
		//
		//The file is in CSV format 
		//The file is saved in a file directory named by 
		//db, table and date, as the follows:
		//
		// jimoData/20130412/hbaseDB/table1/1.csv, 
		// jimoData/20130412/hbaseDB/table1/2.csv,
		// ..............
		Date now = new java.util.Date(); 
		String dateStr;
		String dirName;
		String fileName;
		File fDir;
		boolean isNewFile = false;
		
		//simple.applyPattern("yyyy-MM-dd HH:mm:ss:SSS"); 
		//date = simple.parse("2011-1-1 13:13:13");		
			
		//check if file dir exist
		dateStr = new java.text.SimpleDateFormat("yyyyMMdd").format(now);
		dirName = rootDir + dateStr + dirSep +
					db + dirSep + table + dirSep;
		fDir = new File(dirName);
		try {
			if (!fDir.exists()) {
				//create the folder and needed parent folders
				fDir.mkdirs();
				System.out.println("Created a new data folder: " + dirName);
			}
		} catch (Exception e) {
			System.out.println("Failed to create data folder: " + dirName);
			e.printStackTrace();
		}
		
		//check if we need to save into a new file
		line += "\n";
		if (curSize + line.length() <= maxSize && count > 0) {
			//save into an existing file
			curSize += line.length();
			fileName = dirName + fileTime + fileSuffix;
		} else {
			//save into a new file
			count++;
			curSize = line.length();
			fileTime = new java.text.SimpleDateFormat("hhmmssSSS").format(now);
			fileName = dirName + fileTime + fileSuffix;
			System.out.println("Need to create a new data file: " + fileName);
		} 
		
		try {
			bw = new BufferedWriter(new FileWriter(fileName, true));		
			bw.write(line);
			bw.flush();
			bw.close();
		} catch (Exception e) {
			System.out.println("Failed to write data to the file: " + fileName);
			e.printStackTrace();
		}
	}
	
}
