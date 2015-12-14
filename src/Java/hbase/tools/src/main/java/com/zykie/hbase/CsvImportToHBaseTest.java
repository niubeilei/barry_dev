package com.zykie.hbase;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import org.apache.commons.io.FileUtils;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Result;

import com.zykie.jdbc.Driver;
import com.zykie.jdbc.Connection;

import au.com.bytecode.opencsv.CSVReader;
import au.com.bytecode.opencsv.CSVWriter;

public class CsvImportToHBaseTest {

	// jimo parameters
	static String jimoUrl = "jdbc:zykie://192.168.99.180:9008";
	static String jimoHost = "192.168.99.180";
	static int jimoPort = 9008;
	static String jimoUser = "root";
	static String jimoPasswd = "12345";
	static Properties info = new Properties();

	// hbase parameters
	static String hbaseHost = "192.168.99.180";
	static String hbasePort = "2181";

	// common parameters
	static String table = "dianli2";
	static String filepath = "";
	static int rownum = 0;
	static HBaseOperation mHbase = null;

	static String colFamilies[] = null;
	static String columns[][] = null;
	static long	  mIndex = 1;

	public static void main(String[] args) throws Exception {

//		String filename = args[0];
//		String config = ReadFile(filename);

		String config = "{'hbaseHost':'192.168.99.180', 'hbasePort':'2181', "
				+ "'filepath':'G:/hbasetest/', 'table':'dianli3', "
				+ "'family':[{'name':'article'},{'name':'author'}],"
				+ "'columns':[{'column':[{ 'columnname':'title'}, {'columnname':'category' }]},{'column':[{ 'columnname':'name'}, {'columnname':'nickname' }]}]"
				+ "}";

		
		JSONObject obj = (JSONObject) JSONSerializer.toJSON(config);
		hbaseHost = obj.getString("hbaseHost");
		hbasePort = obj.getString("hbasePort");
		table = obj.getString("table");
		filepath = obj.getString("filepath");
		
		
		JSONArray familyjson = obj.getJSONArray("family");
		System.out.println(familyjson.size());
		colFamilies = new String[familyjson.size()];
		
		for (int i = 0; i < familyjson.size(); i++) {
			
			JSONObject jo = (JSONObject) familyjson.get(i);
			System.out.println(jo.getString("name"));
			colFamilies[i] = jo.getString("name");
		}
	    
		JSONArray columnjson = obj.getJSONArray("columns");
				
		System.out.println("columns size : " + columnjson.size());
		columns = new String[familyjson.size()][];
		for (int i = 0; i < columnjson.size(); i++) {
			JSONObject jobj = (JSONObject)columnjson.get(i);
			JSONArray tmparray = jobj.getJSONArray("column");
			String [] ss = new String[tmparray.size()];
			for (int j = 0; j < tmparray.size(); j++) {
				JSONObject jo = (JSONObject) tmparray.get(j);
				System.out.println(jo.getString("columnname"));
				ss[j] = jo.getString("columnname");
			}
			columns[i] = ss;
		}
	
		System.out.println("host : " + hbaseHost);
		System.out.println("port : " + hbasePort);
		System.out.println("tablename : " + table);
		System.out.println("filepath : " + filepath);
		System.out.print("colFamilies" + java.util.Arrays.toString(colFamilies));
		System.out.print("columns" + java.util.Arrays.toString(columns));

		int fnum = colFamilies.length;
		int cnum = columns.length;
		
		if(fnum != cnum)
		{
			System.out.println("Failed to config family and columns , family is : " + fnum + ", columns : " + cnum);
		}
		else
		{
			boolean r = initConnectToHbase();
			if(r)
			{
				File f = new File(filepath);
				dir(f);
			}
			else
			{
				System.out.println("Failed to connect Hbase");
			}
			
		}
		


	}

	static public String ReadFile(String path){
		File file = new File(path);
	    BufferedReader reader = null;
	    String laststr = "";
	    try {
	     reader = new BufferedReader(new FileReader(file));
	     String tempString = null;
	     int line = 1;
	     while ((tempString = reader.readLine()) != null) {
	      System.out.println("line : " + tempString);
	      laststr += tempString;
	     }
	     reader.close();
	    } catch (IOException e) {
	     e.printStackTrace();
	    } finally {
	     if (reader != null) {
	      try {
	       reader.close();
	      } catch (IOException e1) {
	      }
	     }
	    }
	    return laststr;
	}
	
	public static void dir(File f) {
		// 获得当前路径下的所有文件和文件夹
		String filepath = f.getAbsolutePath();
		File[] allFiles = f.listFiles();
		// 循环所有路径
		for (int i = 0; i < allFiles.length; i++) {
			// 如果是文件夹
			if (allFiles[i].isDirectory()) {
				// 递归调用
				dir(allFiles[i]);
			} else { // 文件
						// 执行操作，例如输出文件名
				System.out.println("--->" + filepath + File.separator
						+ allFiles[i].getName());
				String name = filepath + File.separator + allFiles[i].getName();
				if (name.endsWith(".csv")) {
					CSVReader csvReader;
					try {
						csvReader = new CSVReader(new FileReader(name));
						List content = null;
						content = csvReader.readAll();

						String[] row = null;
						
						for (Object object : content) {
							row = (String[]) object;
							csv2HBase(mIndex+"", row);
							mIndex++;
						}
						csvReader.close();

					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}

				}
			}
		}
	}

	
	public static boolean initConnectToHbase()
	{
		try {
			System.setProperty("javax.xml.parsers.DocumentBuilderFactory",
					"com.sun.org.apache.xerces.internal.jaxp.DocumentBuilderFactoryImpl");
			System.setProperty("javax.xml.parsers.SAXParserFactory",
					"com.sun.org.apache.xerces.internal.jaxp.SAXParserFactoryImpl");

			// 1.Init HBaseOperation
			Configuration conf = new Configuration();

			// same as hbase/conf/hbase-site.xml's hbase.zookeeper.quorum value
			conf.set("hbase.zookeeper.quorum", hbaseHost);

			// same as hbase/conf/hbase-site.xml's
			// hbase.zookeeper.property.clientPort value
			conf.set("hbase.zookeeper.property.clientPort", hbasePort);

			mHbase = new HBaseOperation(conf);

			// 2.Test steps
			// 2.1 create a table
			mHbase.createTable(table, colFamilies);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		return true;
		
	}
	
	public static boolean csv2HBase(String rowid, String[] row) throws IOException {
		
		int cnum = columns.length;
		
		//check row length is equals to columns length
		
		int config_rows = 0;
		for(int i=0; i<columns.length; i++)
		{
			config_rows += columns[i].length;
		}
		
		if(config_rows != row.length)
		{
			System.out.println("row length is not match, config row length:" + config_rows + ", row length of the file :"+row.length);
			return false;
		}
		
		int rownum = 0;
		for(int i=0; i<colFamilies.length; i++)
		{
			String family = colFamilies[i];
			for(int j=0; j<columns[i].length; j++)
			{
				int currentnum = rownum + j; 
				String column = columns[i][j];
				// 2.2 insert a record
				System.out.println("row " + currentnum + ", row value : " + row[currentnum]);
				mHbase.insertRecord(table, rowid, family, column, row[currentnum]);
			}
			rownum += columns[i].length;
		}
		
		return true;

	}
	
	
}
