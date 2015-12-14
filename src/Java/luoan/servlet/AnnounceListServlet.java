package servlet;

import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import util.DaoUtil;

public class AnnounceListServlet extends HttpServlet {


	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		request.setCharacterEncoding("UTF-8");
		doPost(request, response);
	}


	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("utf-8");
		String token = request.getParameter("token");
		/*String ssid = null;
		if(request.getSession().getAttribute("ssid")!=null) {
			ssid = (String) request.getSession().getAttribute("ssid");
		}
		*/
		ServletContext  application=this.getServletContext();
		Map<String, String> mapuser=(Map<String, String>)application.getAttribute("userinfo");
		boolean isequal = false;
		if(mapuser!=null){
			Collection<String> c = mapuser.values();        
			Iterator<String> it = c.iterator();        
			while(it.hasNext()) {            
				String ssid=it.next();
				System.out.println(ssid+"===========it.next()=========");
				if(token!=null && token.equals(ssid)){
					isequal=true;
				}
			}
		}

		if(isequal){
			Connection conn = DaoUtil.getConnection();
			Statement stmt = null;
			ResultSet rs = null;
			JSONObject json = new JSONObject();
			JSONArray child = new JSONArray();
			try{
				String status = request.getParameter("zky_status");
				int n = Integer.parseInt(request.getParameter("n"));
				String container = request.getParameter("container");
				String order = request.getParameter("Order");
				String sql = "select zky_title,zky_ctime,zky_content xpath \"zky_content/_#text\",zky_objid,zky_status from \""+container+"\" where zky_status=\""+status+"\" order by zky_ctime "+order+" limit 0,"+n;
//				sql = new String(sql.getBytes("ISO-8859-1"),"utf-8");
				if(conn!=null) {
						stmt = conn.createStatement();
						rs = stmt.executeQuery(sql);
						boolean flag = false;
						while(rs.next()) {	
							if(!flag) {			
								json.put("status", 0);					
								flag = true;
							}
							JSONObject jsonchild = new JSONObject();
							if(rs.getString("zky_title") != null)	jsonchild.put("zky_title", rs.getString("zky_title"));
							if(rs.getString("zky_ctime") != null) jsonchild.put("zky_ctime", rs.getString("zky_ctime"));
							if(rs.getString("zky_content") != null) jsonchild.put("zky_content", rs.getString("zky_content"));
							if(rs.getString("zky_objid") != null) jsonchild.put("zky_objid", rs.getString("zky_objid"));
							child.put(jsonchild);
						}
						
						if(!flag) {
							json.put("status", 1);		
						} else{
							json.put("message", child);
						}
						response.getWriter().write(json.toString());
					} 
			} catch(NumberFormatException e){
				response.getWriter().print("{\"status\":1,\"errormessage\":\"参数不全\"}");
			}catch (SQLException e) {
				e.printStackTrace();
			} catch(JSONException e){
				e.printStackTrace();
			}finally{
				DaoUtil.closeAll(conn, rs, stmt);
			}			
		}else{
			response.getWriter().print("{\"status\":1,\"errormessage\":\"没有登录\"}");
		}
	}

}
