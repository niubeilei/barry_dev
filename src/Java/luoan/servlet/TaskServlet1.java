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

public class TaskServlet1 extends HttpServlet {

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("utf-8");
		String token = request.getParameter("token");
		ServletContext  application=this.getServletContext();
		Map<String, String> mapuser=(Map<String, String>)application.getAttribute("userinfo");
		boolean isequal = false;
		if(mapuser!=null){
			Collection<String> c = mapuser.values();        
			Iterator<String> it = c.iterator();        
			while(it.hasNext()) {            
				String ssid=it.next();
//				System.out.println(ssid+"===========it.next()=========");
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
			String zky_pctrs = request.getParameter("zky_pctrs");
			String sql = "select zky_objid,zky_school,zky_instdate,contact_id,zky_schoolLaoShi_note xpath \"zky_schoolLaoShi_note/_#text\",zky_handle_note xpath \"zky_handle_note/_#text\" from \""+zky_pctrs+"\"";
//			sql = new String(sql.getBytes("ISO-8859-1"),"utf-8");
			if(conn!=null) {
				try {
					stmt = conn.createStatement();
					rs = stmt.executeQuery(sql);
					boolean flag = false;
					while(rs.next()) {	
						if(!flag) {			
							json.put("status", 0);					
							flag = true;
						}
						JSONObject jsonchild = new JSONObject();
						if(rs.getString(1) != null && !rs.getString(1).equals(""))	jsonchild.put("zky_objid", rs.getString("zky_objid"));
						if(rs.getString(2) != null && !rs.getString(2).equals(""))jsonchild.put("zky_school", rs.getString("zky_school"));
						if(rs.getString(3) != null && !rs.getString(3).equals(""))jsonchild.put("zky_instdate", rs.getString("zky_instdate"));
						if(rs.getString(4) != null && !rs.getString(4).equals(""))jsonchild.put("contact_id", rs.getString("contact_id"));
						if(rs.getString(5) != null && !rs.getString(5).equals(""))jsonchild.put("zky_schoolLaoShi_note", rs.getString("zky_schoolLaoShi_note"));
						if(rs.getString(6) != null && !rs.getString(6).equals(""))jsonchild.put("zky_handle_note", rs.getString("zky_handle_note"));
						System.out.println("1:"+rs.getString(1)+"\t2:"+rs.getString(2)+"\t3:"+rs.getString(3)+"\t4:"+rs.getString(4)+"\t5:"+rs.getString("zky_handle_note"));
						child.put(jsonchild);
					}
					
					if(!flag) {
						json.put("status", 1);		
					} else{
						json.put("message", child);
					}
//					System.out.println(json);
					response.getWriter().write(json.toString());
				} catch (SQLException e) {
					e.printStackTrace();
				} catch (JSONException e) {
					e.printStackTrace();
				} finally{
					DaoUtil.closeAll(conn, rs, stmt);
				}	
			}
		}else{
			response.getWriter().print("{\"status\":1,\"errormessage\":\"没有登录\"}");
		}
	}

}
