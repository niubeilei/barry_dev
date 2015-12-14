package com.zykie.actors.thrift;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.thrift.TException;

import zykie.jimoapi.AosJimoAPI;
import zykie.jimoapi.JmoCallData;
import zykie.jimoapi.JmoDocType;
import zykie.jimoapi.JmoRundata;
import zykie.jimoapi.AosJimoAPI.Client;

import com.zykie.actors.IActor;
import com.zykie.actors.ParserXmlDoc;
import com.zykie.actors.jdbc.Key;
import com.zykie.util.GetClient;

public class ActTableTest implements IActor{

	public static void main(String[] args) {
		String table_def = "<obj><doc><![CDATA[<sqltable zky_record_len=\"0\" zky_public_doc=\"true\" " +
				"zky_public_ctnr=\"true\" zky_def_index=\"true\" zky_use_schema=\"_zt4g_schema_\" " +
				"zky_database=\"db\" zky_otype=\"zky_table\" zky_stype=\"zky_table\" zky_tb_r_name=\"tt\" " +
				"zky_pctrs=\"zky__zt_tables\" AOSTAG_TABLENAME=\"tt\" zky_data_format=\"xml\">" +
				"<columns><column name=\"name\" datatype=\"varchar\" size=\"10\" offset=\"0\"/>"+
				"<column name=\"age\" datatype=\"varchar\" size=\"10\" offset=\"10\"/>"+
				"</columns><indexes></indexes><cmp_indexes></cmp_indexes><maps></maps>"+
				"</sqltable>]]></doc><method>create</method>" +
				"<container_objid>zky__zt_tables</container_objid><objid>_zt4g_tablename_db.tt</objid>" +
				"<site_id>100</site_id><databasename>db</databasename></obj>"; 	//createTable
		
//		String table_def="<obj><method>drop</method>" +
//		"<objid>_zt4g_tablename_db.tt</objid>" +
//		"<site_id>100</site_id><databasename>db</databasename></obj>";	//drop Table
		StringBuffer bff = new StringBuffer();
		new ActTableTest().run(table_def, bff);
		System.out.println(bff);
	}
	
	@Override
	public boolean run(String msg, StringBuffer response) {
		Map<Key,List<String>> map = ParserXmlDoc.xmlElements(msg);
		Set<Key> keys = map.keySet();
		Iterator<Key> it = keys.iterator();
		String method = "";
		Map<String,String> attrs = new HashMap<String, String>();
		while(it.hasNext()) {
			Key key = it.next();
			String name = key.getName();
			if(name.equalsIgnoreCase("method")) {	
				method = map.get(key).get(0);
			} else {
				attrs.put(name, map.get(key).get(0));
			}
		}
		boolean flag = false;
		if(method.equals("create")) {
			flag = createTable(GetClient.getClient(),attrs,response);		//create table
		} else {
			flag = dropTable(GetClient.getClient(),attrs,response);		//drop table
		}
		GetClient.close();
		return flag;	
	}
	
	private boolean dropTable(Client client,Map<String,String> attrs,StringBuffer resp){
		JmoCallData call_data = new JmoCallData();
		if(attrs.get("site_id") != null) call_data.setSite_id(Integer.parseInt(attrs.get("site_id")));
		call_data.setDatabase(attrs.get("databasename"));
		try {
			call_data.setSession_id(login(client));
			JmoRundata rdata = client.deleteDocByObjid(call_data, attrs.get("objid"));
			System.out.println("rcode="+rdata.rcode+"\nerrmsg="+rdata.errmsg+"\nstr_value="+rdata.str_value);
			if("SUCCESS".equalsIgnoreCase(rdata.rcode.toString())) {
				return true;
			} else {
				resp.append(rdata.errmsg);
				resp.append("<rcode>");
				resp.append(rdata.rcode);
				resp.append("</rcode>");
			}
		} catch (TException e) {
			e.printStackTrace();
		}
		return false;
	}
	
	
	
	/**
	 * create Table Student (recreate success)
	 * @param client
	 */
	private boolean createTable(Client client,Map<String,String> attrs,StringBuffer resp) {
		JmoCallData call_data = new JmoCallData();
		if(attrs.get("site_id") != null) call_data.setSite_id(Integer.parseInt(attrs.get("site_id").trim()));
		call_data.setDatabase(attrs.get("databasename").trim());
		call_data.setApi_key(null);
		call_data.setSession_id("");
		try {
			call_data.setSession_id(login(client));
			JmoRundata rdata = client.createDoc(call_data, attrs.get("container_objid").trim(), attrs.get("objid").trim(),
					attrs.get("doc").trim(), JmoDocType.XML, null);
			System.out.println("rcode="+rdata.rcode+"\nerrmsg="+rdata.errmsg+"\nstr_value="+rdata.str_value);
			if("SUCCESS".equalsIgnoreCase(rdata.rcode.toString())) {
				return true;
			}
		} catch (TException e) {
			e.printStackTrace();
		}
		return false;
	}
	
	/**
	 * 
	 * @param client
	 * @throws TException
	 */
	private String login(AosJimoAPI.Client client) throws TException
	{
	    
		String login_conf = "";
		login_conf	+= "<obj opr=\"login\""
					+ " container=\"zky_sysuser\""
					+ " login_group=\"zky_sysuser\""
					+ " zky_uname=\"root\">"
					+ "	<zky_passwd__n><![CDATA[12345]]></zky_passwd__n>"
					+ "</obj>";
		
	    JmoRundata rdata = client.login(login_conf);
	    JmoCallData call_data = new JmoCallData();
	    call_data.session_id = rdata.session_id;
	    String doc_str = "";
	    client.executeQuery(call_data, doc_str);
	    System.out.println("ssid : " + rdata.session_id);
	    return rdata.session_id;
	}

}
