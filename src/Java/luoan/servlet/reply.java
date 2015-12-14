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


import operation.Reply;

import org.json.JSONException;
import org.json.JSONObject;

import bean.Replay;

public class reply extends HttpServlet {
	private static final long serialVersionUID = 1L;

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
		
	}

	@SuppressWarnings("unchecked")
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("UTF-8");
		String strreply=request.getParameter("strreply");
		System.out.println(strreply);
		JSONObject jsonObject;
		String zky_pctrs=null;
		String token=null;
		Replay reply=new Replay();
		PrintWriter pw=response.getWriter();
		try {
			jsonObject = new JSONObject(strreply);
	        Iterator iterator = jsonObject.keys();
	        String key = null;
	        while (iterator.hasNext()) {
	            key = (String)iterator.next().toString().trim();
	            if(key.equals("zky_uname")){
	            	reply.setZky_handle_uname(jsonObject.getString(key));
	            }else if(key.equals("zky_name")){
	            	reply.setZky_handle_name(jsonObject.getString(key));
	            }else if(key.equals("zky_cloudid__a")){
	            	reply.setZky_handle_cloudid(jsonObject.getString(key));
	            }else if(key.equals("zky_objid")){
	            	reply.setLiuyan_id(jsonObject.getString(key));
	            }else if(key.equals("note")){
	            	reply.setZky_handle_note(jsonObject.getString(key));
	            }else if(key.equals("zky_pctrs")){
	            	zky_pctrs=jsonObject.getString(key);
	            }else if(key.equals("token")){
	            	token=jsonObject.getString(key);
	            }else if(key.equals("zky_class")){
	            	if(jsonObject.getString(key).equals("中心老师")){
	            		reply.setZky_handle_unit("信息中心");
	            	}
	            }else if(key.equals("zky_role")){
	            	if(jsonObject.getString(key).equals("学校老师")){
	            		reply.setZky_handle_unit(reply.getZky_handle_name());
	            	}
	            }
			} 

	     }
		catch (JSONException e1) {
			e1.printStackTrace();
		}
		 try {
	        	if(token != null){
	    			boolean isequal=false;
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
	    			System.out.println("用户回复留言---已登录");
	    			boolean remsg=Reply.reply(zky_pctrs, reply);
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
	}

}
