package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;


import org.apache.thrift.TException;
import org.apache.thrift.transport.TTransportException;
import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.json.JSONException;
import org.json.JSONObject;



import util.GetClient;
import zykie.jimoapi.JmoRundata;
import zykie.jimoapi.AosJimoAPI.Client;

public class LoginServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
		
	}
	@SuppressWarnings("unchecked")
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("UTF-8");
		String username=request.getParameter("username");
		String passwd =request.getParameter("passwd");
		PrintWriter pw=response.getWriter();
		if(username != null && passwd != null){
			username=username.trim();
			passwd=passwd.trim();
		String container=GetClient.getContainer();
		System.out.println(container+"&&&&&&&&");
	    try {
			String login_conf = "";
			login_conf	+= "<loginobj opr=\"login\""
							+ " container=\""+container+"\""
							+ " zky_uname=\""+username+"\">"
							+ "	<zky_passwd__n><![CDATA["+passwd+"]]></zky_passwd__n>"
							+ "</loginobj>";
			System.out.println(login_conf+"======");
			Client client=GetClient.getClient();
			JmoRundata rdata;
			rdata = client.login(login_conf);
			System.out.println(rdata.str_value+"&&&&&&&&&&&&&&&&&");
			String ssid=rdata.session_id;
			System.out.println("ssid : " + ssid);
			System.out.println(rdata.loginobj+"=================loginobj==========================");
			 
			GetClient.close();
			JSONObject json=new JSONObject();
			
			if(ssid==null || ssid.equals("")){
				json.put("status", "1");
				pw.print(json.toString());
				pw.close();
			}else{
				Document document;
				try {
					 document = DocumentHelper.parseText(rdata.loginobj);
					 Element root = document.getRootElement();  
					 System.out.println(root);
					 List<Attribute> attrList = root.attributes(); 
					 for (int i = 0; i < attrList.size(); i++) {    
						 //属性的取得    
						 Attribute item = attrList.get(i);    
						 System.out.println(item.getName() + "=" + item.getValue());
						 if(item.getName().equals("zky_name")){
							 json.put("zky_name", item.getValue());
						 }else if(item.getName().equals("zky_cloudid__a")){
							 json.put("zky_cloudid_a", item.getValue());
						 }else if(item.getName().equals("zky_atclctnr")){
							 json.put("zky_atclctnr", item.getValue());
						 }else if(item.getName().equals("zky_role")){
							 json.put("zky_role", item.getValue());
						 }
					}
				} catch (DocumentException e) {
					e.printStackTrace();
				}  
			json.put("token", ssid);
			json.put("status", "0");
			pw.print(json.toString());
			pw.close();
			}
			//保存sessionid到session中
			//request.getSession().setAttribute("ssid",ssid);
			ServletContext application = this.getServletContext(); 
			Map<String, String> mapuser=(Map<String, String>)application.getAttribute("userinfo");
			System.out.println("mapuser=******************************"+mapuser);
			
			/*Set<String> key = mapuser.keySet();        
			for (Iterator<String> it = key.iterator(); it.hasNext();) 
			 {            
				 //String s = (String) it.next();            
				 //System.out.println(map.get(s));    
				 if(it.next().equals(username)){
					mapuser.put(username, ssid);
					application.setAttribute("userinfo", mapuser);
				 }else{
					 
				 }
			 }*/
			if(mapuser==null){
				mapuser=new HashMap<String, String>();
			}
			mapuser.put(username, ssid);
			application.setAttribute("userinfo", mapuser);
			 Collection<String> c = mapuser.values();        
				Iterator<String> it = c.iterator();        
				for (; it.hasNext();) {            
					 //System.out.println(it.next()); 
					System.out.println(it.next()+"===========Loginservlet it.next()=========");
				}

			System.out.println(json.toString());
			
		} catch (TTransportException e1) {
			e1.printStackTrace();
		}
		catch (JSONException e) {
			e.printStackTrace();
		} catch (TException e) {
			e.printStackTrace();
		}
		
	}else{
		pw.print("{\"status\":1,\"message\":\"传入的参数不全\"}");
	}
	}

}
