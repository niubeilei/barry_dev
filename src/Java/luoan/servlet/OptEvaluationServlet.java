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

import org.json.JSONException;
import org.json.JSONObject;

import com.zykie.jdbc.QueryResponse;

import util.DaoUtil;

public class OptEvaluationServlet extends HttpServlet{

	
	public void doGet(HttpServletRequest request, HttpServletResponse response)
	throws ServletException, IOException {
		doPost(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response)
		throws ServletException, IOException {
		JSONObject json = new JSONObject();
		response.setCharacterEncoding("utf-8");
		request.setCharacterEncoding("UTF-8");
		String token = request.getParameter("token");
		String zky_comment_assessment = request.getParameter("zky_comment_assessment");
		String OBJID = request.getParameter("OBJID");
		int zky_comment_marks = 0;
		if(request.getParameter("zky_comment_marks")!=null) {
			zky_comment_marks = Integer.parseInt(request.getParameter("zky_comment_marks"));
		}
		if(zky_comment_marks<0 || zky_comment_marks>=6) {
			try {
				json.put("status", 1);
				json.put("error", "zky_comment_marks值有问题");
				return;
			} catch (JSONException e) {
				e.printStackTrace();
			}	
		}  
		String contact_id = request.getParameter("contact_id");
		ServletContext application=this.getServletContext();
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
			SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");//设置日期格式
			String sql = "update \"luoan_申请单\" set zky_iscomment=\"已评论\",zky_comment_marks=\""+
				zky_comment_marks+"\",zky_comment_assessment=\""+zky_comment_assessment+"\"," +
				"zky_schoolLaoShi_status=\"已评论\"," +"zky_schoolLaoShi_comment_time=\""+
				format.format(new Date())+"\" " +"where zky_objid=\""+OBJID+"\"";
			if(conn!=null) {
				try {
					stmt = conn.createStatement();
					stmt.executeUpdate(sql);
					QueryResponse resp = ((com.zykie.jdbc.ResultSet)stmt.getResultSet()).getQueryResponse();
					System.out.println("error="+resp.getError()+"********************");
					if(!resp.getError()) {	
						json.put("status", 0);			
					} else {
						json.put("status", 1);		
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
