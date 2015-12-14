package com.zykie.actors.jdbc;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.zykie.actors.IActor;
import com.zykie.actors.ParserXmlDoc;
import com.zykie.jdbc.QueryResponse;
import com.zykie.jdbc.ResultSet;
import com.zykie.util.JDBCFactory;

public class ActTable implements IActor{

	@Override
	public boolean run(String msg, StringBuffer response) {
		Map<Key,List<String>> map = ParserXmlDoc.xmlElements(msg);
		System.out.println("map="+map);
		QueryResponse resp = null;
		Set<Key> keys = map.keySet();
		Iterator<Key> it = keys.iterator();
		System.out.println(keys.size()+"=size");
		String method = "";
		String tablename = "";
		StringBuffer column = new StringBuffer();
		List<Key> columns = new ArrayList<Key>();
		String jql = "";
		while(it.hasNext()) {
			Key key = it.next();
			String name = key.getName();
			if(name.equalsIgnoreCase("method")) {
				
				method = map.get(key).get(0);
				System.out.println("method="+method);
			} else if(name.equalsIgnoreCase("tablename")) {
				tablename = map.get(key).get(0);
			} else if(name.equalsIgnoreCase("column")) {
				columns.add(key);
			} else if(name.equalsIgnoreCase("sql")) {
				jql = map.get(key).get(0);
			}
		}
		System.out.println(column);
		System.out.println("method="+method);
		boolean flag = false;
		try {
			if(method.equalsIgnoreCase("showTables")) {	//show tables
				String sql = "show tables";
				resp = common(sql);
				flag = !resp.getError();
				System.out.println("column="+resp.getFields().getClass()+"\nrecord="+resp.getRecords());
				response.append(getResp(resp));
			} else {
				if(method.equalsIgnoreCase("create")) {	//create table
					if(columns != null) {
						for(Key k : columns) {
							getColumns(map.get(k),column);	//获取字段信息
						}	
					}	
					String sql = "create table "+ tablename + " (" + column.substring(0, column.length()-1)+")";
					resp = common(sql);
				} else if(method.equalsIgnoreCase("drop")) {	//drop table
					String sql = "drop table "+tablename;
					resp = common(sql);
					response.append(resp.getMsg());
				} else if(method.equalsIgnoreCase("describe")){	//describe table(error)
					String sql = "describe table "+tablename;
					resp = common(sql);
//					response.append(getResp(resp));
					System.out.println("fields="+resp.getFields()+"\nrecords="+resp.getRecords());
				} else if(method.equalsIgnoreCase("select")){		//select table
					Connection conn=JDBCFactory.getConnection();
					Statement stmt=conn.createStatement();
					stmt.execute(jql);
					Pattern pattern = Pattern.compile("select(.*?)from");				
			        Matcher matcher = pattern.matcher(jql);
			        String[] fields = null;
			        while(matcher.find()) {
			            System.out.println("group="+matcher.group(1));
			            fields = matcher.group(1).split(",");
			        }	
			        
					ResultSet rs=(ResultSet) stmt.getResultSet();
					
					while(rs.next()){
						response.append("<column ");
						for(int i = 0;i < fields.length;i ++) {
							response.append(fields[i]+"="+rs.getString(fields[i].trim())+" ");
						}
						response.append("></column>");
					}
					JDBCFactory.close(conn, rs, stmt);
					resp = rs.getQueryResponse();
				} else if(method.equalsIgnoreCase("insert")){		//insert table
					resp = common(jql);
				} else if(method.equalsIgnoreCase("delete")) {
					resp = common(jql);
				}  else if(method.equalsIgnoreCase("update")) {
					resp = common(jql);
				}
				flag = !resp.getError();
				response.append("<msg>");
				response.append(resp.getMsg());
				response.append("</msg>");
			}
		} catch (SQLException e) {
			
			e.printStackTrace();
		}
		return flag;
	}
	
	private StringBuffer getResp(QueryResponse resp){
		StringBuffer response = new StringBuffer();
		Iterator its = resp.getRecords().iterator();
		while(its.hasNext()) {
			response.append("<tablename>");
			response.append(its.next().toString());
			response.append("</tablename>");
		}
		return response;
	}

	private void getColumns(List<String> list,StringBuffer columns) {
		String name = "";
		String datatype = "";
		String size = "";
		if(list != null) {
			for(int i = 0; i < list.size();i++) {
				String[] attrs = list.get(i).split("=");
				if(attrs[0].equals("size")) {
					size = attrs[1];
				} else if(attrs[0].equals("name")) {
					name = attrs[1];
				} else if(attrs[0].equals("datatype")) {
					datatype = attrs[1];
				}
			}
			columns.append(name + " " + datatype);
			if(datatype.equalsIgnoreCase("int") || datatype.equalsIgnoreCase("string")) {
				columns.append(",");
			} else if(datatype.equalsIgnoreCase("varchar")) {
				columns.append("("+size+"),");
			} 
		}
		
		
	}
	
	private QueryResponse common(String sql) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		stmt.execute(sql);
		ResultSet rs=(ResultSet) stmt.getResultSet();
		System.out.println(rs.getQueryResponse().getMsg()+"=============");
		JDBCFactory.close(conn, rs, stmt);
		QueryResponse resp = rs.getQueryResponse();
		return resp;
	}
	
	public static void main(String[] args) {
//		String msg = "<obj><tablename>student</tablename><method>create</method>" +
//				"<column name=\"name\" datatype=\"varchar\" size=\"10\"></column>"+
//				"<column name=\"age\" datatype=\"varchar\" size=\"10\"></column>"+
//				"</obj>";
//		String msg = "<obj><tablename>student</tablename><method>insert</method>" +
//				"<column name=\"name\" value=\"zzb\"></column>" +
//				"<column name=\"age\" value=\"10\"></column></obj>";
		String msg = "<obj><method>insert</method><sql>insert into student(name,age) values(\"caoyue\",12)</sql></obj>";
//		String msg = "<obj><method>select</method><sql>select name,age from student</sql></obj>";
//		String msg = "<obj><method>select</method><sql>update student  set name=\"lina1\" where name=\"lina\"</sql></obj>";
		StringBuffer response = new StringBuffer();
		new ActTable().run(msg, response);
		System.out.println("response="+response);
	}
	
}
