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

public class TaskServlet extends HttpServlet {

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("utf-8");
		request.setCharacterEncoding("UTF-8");
		String token = request.getParameter("token");
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
			String zky_pctrs = request.getParameter("zky_pctrs");
			String objid = request.getParameter("OBJID");
			String sql = "select zky_school,contact,zky_instdate,zky_schoolLaoShi_note xpath" +
					" \"zky_schoolLaoShi_note/_#text\",zky_handle_note xpath \"zky_handle_note/_#text\"," +
					"zky_objid,zky_iscomment,zky_comment_marks,zky_comment_assessment" +
					" from \""+zky_pctrs+"\" where zky_objid=\""+objid+"\"";
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
						if(rs.getString("zky_school") != null)	jsonchild.put("zky_school", rs.getString("zky_school"));
						if(rs.getString("contact") != null) 	jsonchild.put("contact", rs.getString("contact"));
						if(rs.getString("zky_instdate") != null)	jsonchild.put("zky_instdate", rs.getString("zky_instdate"));
						if(rs.getString("zky_schoolLaoShi_note") != null)	jsonchild.put("zky_schoolLaoShi_note", rs.getString("zky_schoolLaoShi_note"));
						if(rs.getString("zky_handle_note") != null)	jsonchild.put("zky_handle_note", rs.getString("zky_handle_note"));
						if(rs.getString("zky_comment_marks") != null)	jsonchild.put("zky_comment_marks", rs.getString("zky_comment_marks"));
						if(rs.getString("zky_comment_assessment") != null)	jsonchild.put("zky_comment_assessment", rs.getString("zky_comment_assessment"));
						if(rs.getString("zky_iscomment") != null) {
							jsonchild.put("zky_iscomment", rs.getString("zky_iscomment"));
						} else {
							jsonchild.put("zky_iscomment", "未评论");
						}
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
