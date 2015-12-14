
import socketding.*;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
public class ImportData {

	/**
	 * @param args
	 */
	static SocketConnect mConn; 
	static String mSid;
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			String ip = args[0];
			String port = args[1];
			String dirname = args[2];

			System.out.println("Address: " + ip + ":" + port);
			System.out.println("Dir: " + dirname);
			
			mConn = new SocketConnect(ip, Integer.parseInt(port), 50, 100000);
			boolean rlst =  login("50003", "12345", "zky_sysacct");
			if (!rlst)  return; 
			
			File dir = new File(dirname); 
			readDirectory(dir);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	static void readDirectory(File dir) throws Exception
	{
		File[] files = dir.listFiles(); 
		for (int i = 0; i < files.length; i++) { 
			if(!files[i].isDirectory()){ 
				System.out.print("file name: " + files[i] + "\t");
				BufferedReader br = new BufferedReader(
						new InputStreamReader(new FileInputStream(files[i])));
				StringBuffer buffer = new StringBuffer();
				String lineString = "";
				while((lineString = br.readLine()) != null){
					buffer.append(lineString);
				}
				modifyObj("", buffer.toString());
				br.close();
			}
			else
			{
				readDirectory(files[i]);
			}
		} 
	}
	static private boolean login(String username, String passwd, String ctnr) throws Exception
	{
		String cmd = "<request><item name='zky_siteid'><![CDATA[100]]></item><item name='zkyurldocdid'><![CDATA[190591]]></item><item name='operation'><![CDATA[serverCmd]]></item><item name='trans_id'><![CDATA[3]]></item><command><embedobj zky_cloudid__a=\"50003\" login_group=\"zky_sysuser\" container=\"zky_sysuser\" opr=\"login\"><zky_passwd__n><![CDATA[12345]]></zky_passwd__n></embedobj></command><item name='operation'><![CDATA[retrieveFullVpd]]></item><item name='trans_id'><![CDATA[162]]></item><item name='subOpr'><![CDATA[]]></item><item name='loginobj'><![CDATA[true]]></item></request>";
		mConn.sendMessage(cmd);
		String resp = mConn.getMessage();

		int pos = resp.indexOf("error=\"");
		int pos2 = resp.indexOf("\"", pos+7);
		String rsltstr =resp.substring(pos+7, pos2);
		boolean rslt = rsltstr.equals("false");
		if (!rslt)
		{
			System.out.println("Login Failed!");
			System.out.println(resp);
		}
		else
		{
			//System.out.println(resp);
			pos = resp.indexOf("zky_ssid_190591=");
			pos2 = resp.indexOf(";", pos+1);
			mSid=resp.substring(pos+"zky_ssid_190591=".length(), pos2);
			System.out.println("GetSsid: " + mSid);
		}
		return rslt;
	}
	
	static private void modifyObj(String name, String obj) throws Exception
	{
		String cmdh = "<request><item name='zky_siteid'><![CDATA[100]]></item><item name='objid'><![CDATA[000000000.10011113]]></item><item name='sendto'><![CDATA[seserver]]></item><item name='zkyurldocdid'><![CDATA[190591]]></item><item name='operation'><![CDATA[modifyObj]]></item><item name='username'><![CDATA[nonameyet]]></item><item name='trans_id'><![CDATA[9]]></item><xmlobj>";
		String cmdt = "</xmlobj><zky_cookies><cookie zky_name=\"zky_ssid_190591\"><![CDATA["+ mSid + "]]></cookie></zky_cookies></request>";
		String str = cmdh + obj + cmdt;

		mConn.sendMessage(str);
		String resp = mConn.getMessage();
		int pos = resp.indexOf("error=\"");
		int pos2 = resp.indexOf("\"", pos+7);
		String rslt =resp.substring(pos+7, pos2);
		System.out.println(rslt.equals("false")?true:false);
		return;
	}
}
