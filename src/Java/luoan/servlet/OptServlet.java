package servlet;

import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Date;
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

public class OptServlet extends HttpServlet{

	
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
//		if(isequal){
			Connection conn = DaoUtil.getConnection();
			Statement stmt = null;
			ResultSet rs = null;
			JSONObject json = new JSONObject();
			JSONArray child = new JSONArray();
			String zky_pctrs = request.getParameter("zky_pctrs");
			String sql = "select zky_status,contact_id,zky_iscomment,zky_comment_marks,zky_comment_assessment," +
					"zky_schoolLaoShi_status,zky_schoolLaoShi_comment_time from \"luoan_申请单\"";
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
						jsonchild.put("zky_status", rs.getString("zky_status"));
						jsonchild.put("zky_iscomment", rs.getObject("zky_iscomment"));
						jsonchild.put("zky_comment_marks", rs.getString("zky_comment_marks"));
						jsonchild.put("zky_comment_assessment", rs.getString("zky_comment_assessment"));
						jsonchild.put("contact_id", rs.getString("contact_id"));
						jsonchild.put("zky_schoolLaoShi_comment_time", rs.getString("zky_schoolLaoShi_comment_time"));
						jsonchild.put("zky_schoolLaoShi_status", rs.getString("zky_schoolLaoShi_status"));
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
//		}else{
//			response.getWriter().print("{\"status\":1,\"errormessage\":\"没有登录\"}");
//		}
	}
}