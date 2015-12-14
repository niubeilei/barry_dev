package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import oracle.net.aso.l;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import operation.GetMsg;

import bean.Message;
import bean.Replay;

public class getMsg extends HttpServlet {

	private static final long serialVersionUID = 1L;

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
		
	}

	
	@SuppressWarnings("unchecked")
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		String token=request.getParameter("token");
		String OBJID=request.getParameter("OBJID");
		String zky_pctrs=request.getParameter("zky_pctrs");
		String liuyan_id=request.getParameter("liuyan_id");
		response.setCharacterEncoding("UTF-8");
		PrintWriter pw=response.getWriter();
		
		if(token != null && zky_pctrs != null){
			token=token.trim();
			zky_pctrs=zky_pctrs.trim();
		System.out.println("token="+token+"\nOBJID="+OBJID+"\nzky_pctrs="+zky_pctrs);
		//String ssid=(String) request.getSession().getAttribute("ssid");
		//System.out.println(ssid+"#################");
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
			System.out.println("用户查看留言或评论---已登录");
		try {
			
			if(OBJID != null){
				JSONObject json=new JSONObject();
				JSONObject jsonmem=new JSONObject();
				OBJID=OBJID.trim();
				System.out.println("留言查看");
				Message m=GetMsg.getMsg(token, OBJID, zky_pctrs);
				jsonmem.put("contact_unit", m.getContact_unit());
				jsonmem.put("zky_schoolLaoShi_name", m.getZky_schoolLaoShi_name());
				jsonmem.put("zky_instdate", m.getZky_instdate());
				jsonmem.put("note", m.getNote());
				json.put("message", jsonmem);
				json.put("status", "0");
				pw.print(json.toString());
				System.out.println(json.toString());
			}else if(liuyan_id != null){
				JSONObject json=new JSONObject();
				JSONArray jsonarray = new JSONArray();
				JSONObject jsonmem=null;
				liuyan_id=liuyan_id.trim();
				System.out.println("留言评论查看");
				//Replay r=GetMsg.getMsgReplay(token,zky_pctrs, liuyan_id.trim());
				List<Replay> list=GetMsg.getMsgReplay(token,zky_pctrs, liuyan_id.trim());
				for(Replay r:list){
				jsonmem=new JSONObject();
				jsonmem.put("zky_handle_unit",r.getZky_handle_unit());
				jsonmem.put("zky_handle_name", r.getZky_handle_name());
				jsonmem.put("zky_handle_date", r.getZky_handle_date());
				jsonmem.put("zky_handle_note", r.getZky_handle_note());
				jsonarray.put(jsonmem);
				}
				json.put("replay", jsonarray);
				json.put("status", "0");
				pw.print(json.toString());
				System.out.println(json.toString());
			}else if(OBJID == null && liuyan_id==null){
				pw.print("{\"status\":1,\"message\":\"传入的参数不全\"}");
			}
		} catch (SQLException e) {
			e.printStackTrace();
		} catch (JSONException e) {
			e.printStackTrace();
		}
		
	}else{
		pw.print("{\"status\":1,\"message\":\"没有登录\"}");
	}
	}else{
		pw.print("{\"status\":1,\"message\":\"没有登录\"}");
	}
	}else{
		pw.print("{\"status\":1,\"message\":\"传入的参数不全\"}");
	}
	}

}
