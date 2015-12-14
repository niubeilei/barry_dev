package com.zykie.util;

import java.io.IOException;
import java.sql.SQLException;
import java.util.Properties;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TTransportException;

import zykie.jimoapi.AosJimoAPI;
import zykie.jimoapi.AosJimoAPI.Client;

import com.zykie.jdbc.Connection;
import com.zykie.jdbc.ResultSet;
import com.zykie.jdbc.Statement;

public class GetClient {
	private static String ip;
	private static String port;
	private static TTransport transport;
	
	static{
		Properties info=new Properties();
		try{
			info.load(GetClient.class.getResourceAsStream("db.properties"));
			ip=info.getProperty("ip");
			port=info.getProperty("port");
		}catch(IOException e){
			e.printStackTrace();
		}
	}
	
	public static Client getClient(){
	  transport = new TSocket(ip, Integer.parseInt(port));
      try {
		transport.open();
	} catch (TTransportException e) {
		e.printStackTrace();
	}
      TProtocol protocol = new  TBinaryProtocol(transport);
      AosJimoAPI.Client client = new AosJimoAPI.Client(protocol);
      return client;
	}
	
	public static void close(){
		transport.close();
	}
	public static void main(String[] args) throws TTransportException {
		GetClient.getClient();
	}
}
