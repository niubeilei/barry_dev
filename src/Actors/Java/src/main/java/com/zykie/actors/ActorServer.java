package com.zykie.actors;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

import com.zykie.bean.Actor;
import com.zykie.serverutil.GetActorInfo;
import com.zykie.serverutil.MyClassLoader;
import com.zykie.serverutil.ParserXmlDoc;

public class ActorServer extends Thread{
	private Socket socket;
	private OutputStream out=null;
	private InputStream in=null;
	
	public ActorServer(Socket socket) {
		try{
		this.socket = socket;
		out=socket.getOutputStream();
		in=socket.getInputStream();
	
		}catch (Exception e) {
			e.printStackTrace();
		}
	}
	@Override
	public void run() {
		try{	
			while(true){
				
				byte[] b=new byte[4];
				while(in.available()!=0){
					/*Date dt= new Date();
					Long time= dt.getTime();
					System.out.println("time="+time);*/
					//读取xml长度
					for(int i=0;i<4;i++){
						b[i]=(byte)in.read();
					}
					//把byte型的长度转化为int
					int len=byteArrayToInt(b,0);
					System.out.println("len="+len);
					//读取版版号version
					byte[] bversion=new byte[4];
					for(int i=0;i<4;i++){
						bversion[i]=(byte)in.read();
					}
					int version=byteArrayToInt(bversion, 0);
					System.out.println("version="+version);
					//读取xml字符串
					byte[] bytestr=new byte[len];
					for(int j=0;j<len;j++){
						bytestr[j]=(byte)in.read();
					}
					String str=new String(bytestr,"UTF-8");
					System.out.println("str="+str+"*****************************");
					//解析xml字符串。得到actname
					String actname=ParserXmlDoc.getactname(str);
					//String actname=ParserXml.getactname(str);
					System.out.println("actname="+actname);
					//取出actparams传给actor
					int paramsstartindex=str.indexOf("<actparams>");
					int paramsendindex=str.indexOf("</actparams>");
					int paramsendlen="</actparams>".length();
					String toactorxml=str.substring(paramsstartindex, paramsendindex+paramsendlen);
					System.out.println("toactorxml"+toactorxml);
					Actor actor=GetActorInfo.getActorInfo(actname);
					String classname=actor.getClassnmae();
					String classpath=actor.getClasspath();
					System.out.println(classpath+"********************");
					MyClassLoader loader = new MyClassLoader();
					loader.loadJar(classpath.trim());
				    try {
				        StringBuffer response=new StringBuffer();
				        Class<?> clazz = loader.findClass(classname);
				        Object result = loader.invokeMethod(clazz, "run", new Class[] {
				              String.class, StringBuffer.class}, new Object[] {
				                     toactorxml, response});
				             System.out.println("result="+result+"\nresponse="+response);
				             String xml="<response><error>false</error><returncode>200</returncode><message>"+response+"</message><proctime>0.35</proctime></response>";
						System.out.println("xml="+xml+"========================");
				           //xml长度
				     		int length=xml.length();
				     		//版本号，默认为1
				     		int resversion=1;
				     		byte[] lenbyte=intToByteArray(length);
				     		byte[] versionbyte=intToByteArray(resversion);
				     		byte[] strxmlbyte=xml.getBytes("UTF-8");
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
				     		 out.write(all);
				             out.flush();
				    } catch (Exception e) {
							e.printStackTrace();
						}
						/*Date dt2= new Date();
						Long time2= dt2.getTime();
						System.out.println("time2="+time2);*/
					
					
				}
				
				//sleep(6000);
			}
		}catch (Exception e) {
			e.printStackTrace();
		}finally{
			try {
				if(socket != null)
					socket.close();
				if(in != null)
					in.close();
				if(out != null)
					out.close();
			} catch (Exception e2) {
				
			}
		
		}
	}
	
	public static int byteArrayToInt(byte[] b, int offset) {
        int value = 0;
        for (int i = 0; i < 4; i++) {
            int shift = (4 - 1 - i) * 8;
            value += (b[i + offset] & 0x000000FF) << shift;
        }
        return value;
 }
	private static byte[] intToByteArray (final int integer) {
		int byteNum = (40 - Integer.numberOfLeadingZeros (integer < 0 ? ~integer : integer)) / 8;
		byte[] byteArray = new byte[4];

		for (int n = 0; n < byteNum; n++)
		byteArray[3 - n] = (byte) (integer >>> (n * 8));

		return (byteArray);
	}


	public static void main(String[] args) {
		try {
			ServerSocket server = new ServerSocket(10001);
			System.out.println("server is runing....");
			while(true){
				Socket socket = server.accept();
				System.out.println("connect....");
				new ActorServer(socket).start();
			}
		} catch (NumberFormatException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
}
