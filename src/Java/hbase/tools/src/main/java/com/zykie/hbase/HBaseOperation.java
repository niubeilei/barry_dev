package com.zykie.hbase;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.HTableDescriptor;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.HBaseAdmin;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;

public class HBaseOperation {

	private Configuration conf;
	private HBaseAdmin admin;

	public HBaseOperation(Configuration conf) throws IOException {
		this.conf = HBaseConfiguration.create(conf);
		this.admin = new HBaseAdmin(this.conf);
	}

	public HBaseOperation() throws IOException {
		Configuration cnf = new Configuration();
		this.conf = HBaseConfiguration.create(cnf);
		this.admin = new HBaseAdmin(this.conf);
	}

	// 1.create a hbase table
	public void createTable(String tableName, String colFamilies[])
			throws IOException {

		if (this.admin.tableExists(tableName)) {
			System.out.println("Table: " + tableName + " already exists !");
		} else {
			HTableDescriptor dsc = new HTableDescriptor(tableName);
			int len = colFamilies.length;

			for (int i = 0; i < len; i++) {
				HColumnDescriptor family = new HColumnDescriptor(colFamilies[i]);
				dsc.addFamily(family);
			}

			admin.createTable(dsc);
			System.out.println("Create table successfully.");
		}
	}

	// 2.Delete a table
	public void deleteTable(String tableName) throws IOException {

		if (this.admin.tableExists(tableName)) {
			admin.deleteTable(tableName);
			System.out.println("Delete table successfully.");
		} else {
			System.out.println("Table Not Exists !");
		}
	}

	// 3.insert one record
	public void insertRecord(String tableName, String rowkey, String family,
			String qualifier, String value) throws IOException {

		HTable table = new HTable(this.conf, tableName);
		Put put = new Put(rowkey.getBytes());
		put.add(family.getBytes(), qualifier.getBytes(), value.getBytes());
		table.put(put);

		System.out.println("Insert row successfully.");
	}

	// 4.delete one record
	public void deleteRecord(String tableName, String rowkey)
			throws IOException {

		HTable table = new HTable(this.conf, tableName);
		Delete del = new Delete(rowkey.getBytes());
		table.delete(del);
		System.out.println("Delete a row successfully.");
	}

	// 5.get a record
	public Result getOneRecord(String tableName, String rowkey)
			throws IOException {

		HTable table = new HTable(this.conf, tableName);
		Get get = new Get(rowkey.getBytes());
		Result rs = table.get(get);
		return rs;
	}

	// 6.get all the records
	public List<Result> getAllRecord(String tableName) throws IOException {

		HTable table = new HTable(this.conf, tableName);
		Scan scan = new Scan();

		ResultScanner scanner = table.getScanner(scan);
		List<Result> list = new ArrayList<Result>();

		for (Result r : scanner) {
			list.add(r);
		}

		scanner.close();
		return list;
	}
}
