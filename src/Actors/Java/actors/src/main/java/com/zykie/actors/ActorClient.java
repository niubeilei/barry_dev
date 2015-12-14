package com.zykie.actors;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;

public class ActorClient {
	public static void main(String[] args) throws IOException {
		Socket scoket = null;
		OutputStream out=null;
		InputStream in=null;
		scoket=new Socket();
		SocketAddress address=new InetSocketAddress("192.168.99.83", 10001);
		scoket.connect(address, 6000);
		out=scoket.getOutputStream();
		in=scoket.getInputStream();
		//helloworld
		//String strxml="<request><actname>helloWorld</actname><actparams><str>Hello Server!</str></actparams></request>";
		
		//create database
		//String strxml="<request><actname>operationdatabase</actname><actparams><databasename>db4</databasename><operate>create</operate></actparams></request>";
		
		//show database
		//String strxml="<request><actname>operationdatabase</actname><actparams><operate>show</operate></actparams></request>";
		
		//drop database
		//String strxml="<request><actname>operationdatabase</actname><actparams><databasename>db4</databasename><operate>drop</operate></actparams></request>";
		
		//create table
		/*String strxml=
				"<request>" +
				"<actname>operationtable</actname>" +
				"<actparams>" +
				"<tablename>testtb</tablename>" +
				//"<fields>" +
				"<column name=\"name\" datatype=\"varchar\" size=\"20\"/>" +
				"<column name=\"age\" datatype=\"int\" size=\"4\"/>" +
				//"</fields>" +
				"<operate>create</operate>" +
				"</actparams>" +
				"</request>";*/
		
		//insert table
		/*String strxml="<request>" +
				"<actname>operationtable</actname>" +
				"<actparams>" +
				"<operate>insert</operate>" +
				"<sql>insert into testtb(name,age)values(\"Tom\",20)</sql>"+
				"</actparams>" +
				"</request>";
		*/
		
		//select
		/*String strxml="<request>" +
		"<actname>operationtable</actname>" +
		"<actparams>" +
		"<operate>select</operate>" +
		"<sql>select name,age from testtb;</sql>"+
		"</actparams>" +
		"</request>";*/
		
		//update
		/*String strxml="<request>" +
		"<actname>operationtable</actname>" +
		"<actparams>" +
		"<operate>update</operate>" +
		"<sql>update testtb set age=35 where name=\"Tom\"</sql>"+
		"</actparams>" +
		"</request>";*/
		
		//deacribe 
		/*String strxml="<request>" +
		"<actname>operationtable</actname>" +
		"<actparams>" +
		"<operate>describe</operate>" +
		"<tablename>testtb</tablename>"+
		"</actparams>" +
		"</request>";*/
		
		//show 
		/*String strxml="<request>" +
		"<actname>operationtable</actname>" +
		"<actparams>" +
		"<operate>show</operate>" +
		"</actparams>" +
		"</request>";*/
		
		//drop 
		String strxml="<request>" +
		"<actname>operationtable</actname>" +
		"<actparams>" +
		"<operate>drop</operate>" +
		"<tablename>testtb</tablename>"+
		"</actparams>" +
		"</request>";

		
		
		//xml长度
		int length=strxml.length();
		//版本号，默认为1
		int version=1;
		byte[] lenbyte=intToByteArray(length);
		byte[] versionbyte=intToByteArray(version);
		byte[] strxmlbyte=strxml.getBytes("UTF-8");
		byte[] all=new byte[8+length];
		System.out.println(lenbyte.length+":"+strxmlbyte.length+":"+all.length);
		for(int i=0;i<lenbyte.length;i++){
			all[i]=lenbyte[i];
		}
		for(int i=0;i<versionbyte.length;i++){
			all[4+i]=versionbyte[i];
		}
		for(int i=0;i<strxmlbyte.length;i++){
			all[8+i]=strxmlbyte[i];
		}
		//out.write(strxml.getBytes("UTF-8"));
		out.write(all);
        out.flush();
        /*byte[] b=new byte[4];
        System.out.println("=======客户端读取============");
		while(in.available()!=0){
			
			//读取xml长度
			for(int i=0;i<4;i++){
				b[i]=(byte)in.read();
			}
			//把byte型的长度转化为int
			int len=byteArrayToInt(b,0);
			System.out.println("clientlen="+len);
			//读取版版号version
			byte[] bversion=new byte[4];
			for(int i=0;i<4;i++){
				bversion[i]=(byte)in.read();
			}
			int version2=byteArrayToInt(bversion, 0);
			System.out.println("clientversion="+version2);
			//读取xml字符串
			byte[] bytestr=new byte[len];
			for(int j=0;j<len;j++){
				bytestr[j]=(byte)in.read();
			}
			String str=new String(bytestr,"UTF-8");
			System.out.println("clientstr="+str+"*****************************");
		}*/
		try {
			if(in!=null)in.close();
			if(out!=null)out.close();
			if(scoket!=null)scoket.close();
		} catch (Exception e2) {
				e2.printStackTrace();
		}
	}
	private static byte[] intToByteArray (final int integer) {
		int byteNum = (40 - Integer.numberOfLeadingZeros (integer < 0 ? ~integer : integer)) / 8;
		byte[] byteArray = new byte[4];

		for (int n = 0; n < byteNum; n++)
		byteArray[3 - n] = (byte) (integer >>> (n * 8));

		return (byteArray);
	}
	public static int byteArrayToInt(byte[] b, int offset) {
        int value = 0;
        for (int i = 0; i < 4; i++) {
            int shift = (4 - 1 - i) * 8;
            value += (b[i + offset] & 0x000000FF) << shift;
        }
        return value;
 }
}
