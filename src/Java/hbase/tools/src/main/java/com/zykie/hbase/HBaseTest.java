package com.zykie.hbase;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Result;

import com.zykie.jdbc.Driver;
import com.zykie.jdbc.Connection;

public class HBaseTest {

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
	static String dirname = "";
	static int rownum = 0;
	static HBaseOperation mHbase = null;

	static String colFamilies[] = { "article", "author" };
	static String columns[][] = { { "title", "category" },
			{ "name", "nickname" } };

	public static void main(String[] args) throws Exception {

		String configfile = args[0];
		String config =  ReadFile(configfile);

		 /*String config =
		 "{'hbaseHost':'192.168.99.207', 'hbasePort':'2181', 'dirname':'/home/lina/test', " +
		 "'table':'C_CONS','rownum':'1'" +
		 "'family':[{'name':'cf'}],'columns':[[{'columnname':'f1'},{'columnname':'f2'}]]}";*/

		JSONObject obj = (JSONObject) JSONSerializer.toJSON(config);
		hbaseHost = obj.getString("hbaseHost");
		hbasePort = obj.getString("hbasePort");
		table = obj.getString("table");
		dirname = obj.getString("dirname");
		rownum = obj.getInt("rownum");
		
		System.out.println("host : " + hbaseHost);
		System.out.println("port : " + hbasePort);
		System.out.println("tablename : " + table);
		System.out.println("dirname : " + dirname);
		System.out.println("rownum : " + rownum);
		
		JSONArray familyjson = obj.getJSONArray("family");
		System.out.println(familyjson.size());
		for (int i = 0; i < familyjson.size(); i++) {
			
			JSONObject jo = (JSONObject) familyjson.get(i);
			System.out.println(jo.getString("name"));
			colFamilies[i] = jo.getString("name");
		}
	    
		JSONArray columnjson = obj.getJSONArray("columns");
				
		for (int i = 0; i < columnjson.size(); i++) {
			JSONArray tmparray = (JSONArray) columnjson.get(i);
			String [] ss = new String[tmparray.size()];
			for (int j = 0; j < tmparray.size(); j++) {
				JSONObject jo = (JSONObject) tmparray.get(j);
				System.out.println(jo.getString("columnname"));
				ss[j] = jo.getString("columnname");
			}
			columns[i] = ss;
		}
		
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
				testHBase();
				//testJimo();
			}
			else
			{
				System.out.println("Failed to connect Hbase");
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

	public static void testJimo() throws Exception {

		Connection conn;
		JimoTable jTable;
		Put put;
		Delete delete;

		info.setProperty("user", jimoUser);
		info.setProperty("password", jimoPasswd);

		// driver = new Driver();
		// conn = driver.connect(jimoUrl, info);
		conn = new Connection(jimoHost, jimoPort, info, "hbaseDB", jimoUrl);
		jTable = new JimoTable(conn, table);

		jTable.createDB();
		jTable.create(table, colFamilies, columns);

		put = new Put("row1".getBytes());
		put.add("article".getBytes(), "title".getBytes(),
				"The lost world".getBytes());
		jTable.put(put);

		put = new Put("row2".getBytes());
		put.add("article".getBytes(), "title".getBytes(),
				"Star war2".getBytes());
		jTable.put(put);

		put = new Put("row3".getBytes());
		put.add("article".getBytes(), "title".getBytes(),
				"Star war3".getBytes());
		jTable.put(put);

		put = new Put("row4".getBytes());
		put.add("article".getBytes(), "title".getBytes(),
				"Star war4".getBytes());
		jTable.put(put);

		delete = new Delete("row1".getBytes());
		jTable.delete(delete);
	}

	public static void testHBase() throws IOException {

		// 2.4 query the whole Table
		List<Result> list = null;
		list = mHbase.getAllRecord(table);
		Iterator<Result> it = list.iterator();

		File file = null;
		FileOutputStream out = null;
		OutputStreamWriter osw = null;
		BufferedWriter bw = null;

		int fileno = 0;
		String name = dirname + File.separator + fileno + ".csv";
		file = new File(name);
		out = new FileOutputStream(file);
		osw = new OutputStreamWriter(out, "UTF8");
		bw = new BufferedWriter(osw);

		int index = 0;
		
		String content = "";
		while (it.hasNext()) {

			Result rs2 = it.next();

			String record = "";
			String rowkey = "";
			for (KeyValue kv : rs2.raw()) {
/*				System.out.print("row key is : " + new String(kv.getRow()));
				System.out
						.print(", family is  : " + new String(kv.getFamily()));
				System.out.print(", qualifier is:"
						+ new String(kv.getQualifier()));
				System.out.print(", timestamp is:" + kv.getTimestamp());
				System.out.println(", Value  is  : "
						+ new String(kv.getValue()));
*/
				if (rowkey.equals("")) {
					rowkey = new String(kv.getRow());
				}
				record += "," + new String(kv.getValue());
			}

//			System.out.println("--------  rownum:" + rownum + ",index:" + index);
			

			if (rownum == index) {
				System.out.println("++++++new file ++++++");
				
				//save file
				bw.write(content);
				bw.close();
				osw.close();
				out.close();

				file = null;
				out = null;
				osw = null;
				bw = null;

				fileno++;
				name = dirname + File.separator + fileno + ".csv";
				file = new File(name);
				out = new FileOutputStream(file);
				osw = new OutputStreamWriter(out, "UTF8");
				bw = new BufferedWriter(osw);
				index = 0;	
				content = rowkey + record + "\r\n";
			}
			else
			{
				content += rowkey + record + "\r\n";
			}
			index++;
			
		}

		if(!content.equals(""))
		{
			bw.write(content);
		}
		bw.close();
		osw.close();
		out.close();

	}
}
