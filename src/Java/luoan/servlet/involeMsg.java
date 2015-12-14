package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import operation.SendMsg;

import org.json.JSONException;
import org.json.JSONObject;

import com.zykie.jdbc.ResultSet;

import bean.Message;

public class involeMsg extends HttpServlet {
	private static final long serialVersionUID = 1L;

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
		
	}

	@SuppressWarnings("unchecked")
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("UTF-8");
		String strmsg=request.getParameter("strmsg");
		System.out.println(strmsg);
		JSONObject jsonObject;
		Message msg=new Message();
		String zky_pctrs=null;
		String token=null;
		PrintWriter pw=response.getWriter();
		try {
			jsonObject = new JSONObject(strmsg);
	        Iterator iterator = jsonObject.keys();
	        String key = null;
	        while (iterator.hasNext()) {
	            key = (String)iterator.next().toString().trim();
	            if(key.equals("contact_unit")){
	            	msg.setContact_unit(jsonObject.getString(key));
	            }else if(key.equals("contact")){
	            	System.out.println("又content");
	            	msg.setContact(jsonObject.getString(key));
	            }else if(key.equals("zky_schoolLaoShi_name")){
	            	msg.setZky_schoolLaoShi_name(jsonObject.getString(key));
	            }else if(key.equals("zky_schoolLaoShi_cloudid")){
	            	msg.setZky_schoolLaoShi_cloudid(jsonObject.getString(key));
	            }else if(key.equals("zky_schoolLaoShi_uname")){
	            	msg.setZky_schoolLaoShi_uname(jsonObject.getString(key));
	            }else if(key.equals("zky_instdate")){
	            	msg.setZky_instdate(jsonObject.getString(key));
	            }else if(key.equals("contact_phone")){
	            	msg.setContact_phone(jsonObject.getString(key));
	            }else if(key.equals("contact_Email")){
	            	msg.setContact_Email(jsonObject.getString(key));
	            }else if(key.equals("zky_shouLiLaoShi_cloudid")){
	            	msg.setZky_shouLiLaoShi_cloudid(jsonObject.getString(key));
	            }else if(key.equals("note")){
	            	msg.setNote(jsonObject.getString(key));
	            }else if(key.equals("zky_status")){
	            	msg.setZky_status(jsonObject.getString(key));
	            }else if(key.equals("zky_liuyan_ispublic")){
	            	msg.setZky_liuyan_ispublic(jsonObject.getString(key));
	            }else if(key.equals("zky_pctrs")){
	            	zky_pctrs=jsonObject.getString(key);
	            }else if(key.equals("token")){
	            	token=jsonObject.getString(key);
	            }
			} 

	     }
		catch (JSONException e1) {
			e1.printStackTrace();
		}
        
        try {
        	//String ssid=(String) request.getSession().getAttribute("ssid");
        	if(token != null){
    			boolean isequal=true;
    			ServletContext  application=this.getServletContext();
    			Map<String, String> mapuser=(Map<String, String>)application.getAttribute("userinfo");
    			if(mapuser!=null){
    			Collection<String> c = mapuser.values();        
    			Iterator<String> it = c.iterator();        
    			while(it.hasNext()) {            
    				String ssid=it.next();
    				System.out.println(ssid+"===========it.next()=========");
    				if(token.trim().equals(ssid)){
    					isequal=true;
    				}
    			}
    			}
    		if(isequal==true){
    			System.out.println("用户查看留言或评论---已登录");
    			boolean remsg=SendMsg.sengMsg(msg,zky_pctrs);
    			JSONObject json=new JSONObject();
			try {
			if(remsg==false){
					json.put("status", "0");
					pw.print(json.toString());
			}else{
				json.put("status", "1");
				pw.print(json.toString());
			}
			} catch (JSONException e) {
				e.printStackTrace();
			}
    		}else{
    			pw.print("{\"status\":1,\"message\":\"没有登录\"}");
    		}
        	}else{
        		pw.print("{\"status\":1,\"message\":\"没有登录\"}");
        	}
		} catch (SQLException e) {
			e.printStackTrace();
		}
        System.out.println(msg.getContact()+":"+msg.getContact_unit());




		
	}

}
