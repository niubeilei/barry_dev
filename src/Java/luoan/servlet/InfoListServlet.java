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


public class InfoListServlet extends HttpServlet {

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		
		response.setCharacterEncoding("utf-8");
//		request.setCharacterEncoding("UTF-8");
		String old = request.getParameter("zky_pctrs");
		old = new String(old.getBytes("iso-8859-1"), "utf-8");
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
			String table = request.getParameter("zky_pctrs");
			int n = Integer.parseInt(request.getParameter("n"));
//			table = new String(table.getBytes("ISO-8859-1"),"UTF-8");
			String sql = "select zky_content xpath \"zky_content/_#text\",zky_ctime,zky_objid from \""+table +"\" limit 0,"+n;
//System.out.println(sql);
			if(conn!=null) {
				try {
					stmt = conn.createStatement();
					rs = stmt.executeQuery(sql);
					boolean flag = false;
					JSONArray child = new JSONArray();
					while(rs.next()) {	
						if(!flag) {			
							json.put("status", 0);
							flag = true;
						}
//System.out.println("***********"+rs.getString(1)+"-----------"+rs.getString(2)+"************"+rs.getString(3));
						JSONObject jsonchild = new JSONObject();
						if(rs.getString(1) != null)	jsonchild.put("zky_content", rs.getString(1));
						if(rs.getString(2) != null)	jsonchild.put("zky_ctime", rs.getString(2));
						if(rs.getString(3) != null)	jsonchild.put("zky_objid", rs.getString(3));
						child.put(jsonchild);
					}			
					if(!flag) {
						json.put("status", 1);	
					} else {
						json.put("message", child);
					}
System.out.println(json);
					response.getWriter().write(json.toString());
				} catch (SQLException e) {
					e.printStackTrace();
				} catch (JSONException e) {
					e.printStackTrace();
				}	finally{
					DaoUtil.closeAll(conn, rs, stmt);
				}
			}
		}else{
			response.getWriter().print("{\"status\":1,\"errormessage\":\"没有登录\"}");
		}
	}

}
