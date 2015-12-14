package util;

import java.io.IOException;
import java.util.Properties;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TTransportException;

import com.sun.org.apache.bcel.internal.generic.IINC;

import zykie.jimoapi.AosJimoAPI;
import zykie.jimoapi.AosJimoAPI.Client;

public class GetClient {
	private static String ip;
	private static String port;
	private static String container;
	static{//加载类时就执行，且只执行一次
		Properties info=new Properties();
		try{
			info.load(JDBCFactory.class.getResourceAsStream("db.properties"));
			ip=info.getProperty("ip");
			port=info.getProperty("port");
			container=info.getProperty("container");
			System.out.println(ip+"==="+port);
		}catch(IOException e){
			e.printStackTrace();
		}
	}
	private static TTransport transport;
	public static Client getClient() throws TTransportException{
		      //TTransport transport;
		      transport = new TSocket(ip, Integer.parseInt(port));
		      transport.open();
		      TProtocol protocol = new  TBinaryProtocol(transport);
		      AosJimoAPI.Client client = new AosJimoAPI.Client(protocol);
		      return client;
	}
	
	public static void close(){
		transport.close();
	}
	
	public static  String getContainer(){
		return container;
	}
	public static void main(String[] args) throws TTransportException {
		GetClient.getClient();
	}
}
