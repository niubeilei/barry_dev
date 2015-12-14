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


public class TaskListServlet extends HttpServlet {

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
			String contact_id = request.getParameter("contact_id");
			String n = request.getParameter("n");
			String sql = "select zky_title,zky_instdate,zky_objid,zky_status,contact_id,zky_iscomment,zky_comment_marks,zky_comment_assessment from \""+zky_pctrs+"\" where contact_id=\""+contact_id+"\" limit 0,"+n;
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
						if(rs.getString("zky_title") != null)	jsonchild.put("zky_title", rs.getString("zky_title"));
						if(rs.getString("zky_instdate") != null)	jsonchild.put("zky_instdate", rs.getString("zky_instdate"));
						if(rs.getString("zky_objid") != null)	jsonchild.put("zky_objid", rs.getString("zky_objid"));
						if(rs.getString("zky_status") != null)	jsonchild.put("zky_status", rs.getString("zky_status"));
						if(rs.getString("zky_comment_marks") != null)	jsonchild.put("zky_comment_marks", rs.getString("zky_comment_marks"));
						if(rs.getString("zky_comment_assessment") != null)	jsonchild.put("zky_comment_assessment", rs.getString("zky_comment_assessment"));
						if(rs.getString("zky_iscomment") != null)	{
							jsonchild.put("zky_iscomment", rs.getString("zky_iscomment"));
						} else {
							jsonchild.put("zky_iscomment", "未评论");
						}
						child.put(jsonchild);
					}
					if(!flag) {
						json.put("status", 1);		
					} else {
						json.put("message", child);
					}
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
