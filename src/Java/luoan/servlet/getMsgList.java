package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import bean.Message;
import operation.GetMsgList;

public class getMsgList extends HttpServlet {

	private static final long serialVersionUID = 1L;


	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {

		doPost(request, response);
	}

	
	@SuppressWarnings("unchecked")
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("UTF-8");
		//sessionid
		String token=request.getParameter("token");
		//需要查看的留言条数
		String n=request.getParameter("n");
		//值为luoan_留言
		String zky_pctrs=request.getParameter("zky_pctrs");
		//留言者云号
		String zky_schoolLaoShi_cloudid=request.getParameter("zky_schoolLaoShi_cloudid");
		//受理老师云号
		String zky_shouLiLaoShi_cloudid=request.getParameter("zky_shouLiLaoShi_cloudid");
		
		//是否公共留言
		String zky_liuyan_ispublic=request.getParameter("zky_liuyan_ispublic");
		
		System.out.println("token="+token+"\nn="+n+"\nzky_pctrs="+zky_pctrs+"\nzky_schoolLaoShi_cloudid="+zky_schoolLaoShi_cloudid);
		//String ssid=(String) request.getSession().getAttribute("ssid");
		PrintWriter pw=response.getWriter();
		if(token != null && n != null && zky_pctrs!= null){
			token=token.trim();
			n=n.trim();
			zky_pctrs.trim();
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
			List<Message> listm1=new ArrayList<Message>();
			boolean isparmcomplete=true;
			try {
				if(zky_schoolLaoShi_cloudid != null){
					System.out.println("学校老师查看留言");
					listm1=GetMsgList.getMsgListschool("0",n, zky_pctrs,zky_schoolLaoShi_cloudid.trim(),null,null);
				}else if(zky_shouLiLaoShi_cloudid != null){
					if(zky_liuyan_ispublic != null){
						System.out.println("中心老师查看公共留言");
						listm1=GetMsgList.getMsgListschool("2",n, zky_pctrs,null,zky_shouLiLaoShi_cloudid.trim(),zky_liuyan_ispublic.trim());
						
					}else{
						System.out.println("中心老师查看留言");
						listm1=GetMsgList.getMsgListschool("1",n, zky_pctrs,null,zky_shouLiLaoShi_cloudid.trim(),null);
					} 
				}else if(zky_schoolLaoShi_cloudid == null && zky_shouLiLaoShi_cloudid == null){
					System.out.println("上面===================");
					pw.print("{\"status\":1,\"message\":\"传入的参数不全\"}");
					isparmcomplete=false;
				}
				if(isparmcomplete==true){
				JSONObject json=new JSONObject();
				JSONArray jsonarray = new JSONArray();  
				JSONObject jsonmem= null; 
				for(Message m:listm1){
					jsonmem=new JSONObject();
					jsonmem.put("contact_unit", m.getContact_unit());
					jsonmem.put("zky_schoolLaoShi_name", m.getZky_schoolLaoShi_name());
					jsonmem.put("zky_instdate", m.getZky_instdate());
					jsonmem.put("zky_objid", m.getZky_objid());
					jsonarray.put(jsonmem);
				}
				json.put("message", jsonarray);
				json.put("status", "0");
				pw.print(json.toString());
				System.out.println(json.toString());
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
		System.out.println("下面==============");
		pw.print("{\"status\":1,\"message\":\"传入的参数不全\"}");
	}
	}
		
	

}
