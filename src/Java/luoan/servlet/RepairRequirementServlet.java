package servlet;

import java.io.IOException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.json.JSONException;
import org.json.JSONObject;

import util.DaoUtil;

import com.zykie.jdbc.QueryResponse;

public class RepairRequirementServlet extends HttpServlet {

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setCharacterEncoding("utf-8");
		request.setCharacterEncoding("UTF-8");
		String str = request.getParameter("json");			
		Connection conn = DaoUtil.getConnection();
		Statement stmt = null;	
//			System.out.println(str);
//			str = new String(str.getBytes("ISO-8859-1"),"utf-8");
		if(conn!=null) {
			try {
				stmt = (Statement) conn.createStatement();
				JSONObject json = new JSONObject();
				JSONObject jsonObject = new JSONObject(str);
				String token = jsonObject.getString("token");
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
					Map<String,String> map = new HashMap<String,String>(); 
			        for(Iterator iter = jsonObject.keys(); iter.hasNext();){ 
			            String key = (String)iter.next(); 
			            map.put(key, jsonObject.get(key).toString()); 
			        } 
			        Set<String> keys = map.keySet();
					String sql = "insert into \""+map.get("zky_pctrs")+"\"(";
					int i = 0;
					for(String key : keys) {
						i++;
						if(!key.equals("zky_pctrs") && !key.equals("token")) {
							if(key.equals("zky_schoolLaoShi_note")){
								sql += "\""+key+"/_#text\"";
							} else {
								key = java.net.URLDecoder.decode(key,"UTF-8");
								sql += key;
							}
						}
						if(i<keys.size() && !key.equals("zky_pctrs") && !key.equals("token"))
							sql += ",";
					}
					i = 0;
					sql += ") values(";
					for(String key : keys) {
						i++;
						if(!key.equals("zky_pctrs") && !key.equals("token")){
							String name = java.net.URLDecoder.decode(map.get(key),"UTF-8");
							sql += "\""+name+"\"";
						}
							
						if(i<keys.size() && !key.equals("zky_pctrs") && !key.equals("token"))
							sql += ",";
					}
					sql += ")";
					System.out.println("---------"+sql+"==");
					stmt.executeUpdate(sql);
					QueryResponse resp = ((com.zykie.jdbc.ResultSet)stmt.getResultSet()).getQueryResponse();
					System.out.println("error="+resp.getError()+"********************");
					if(!resp.getError()) {	
						json.put("status", 0);			
					} else {
						json.put("status", 1);		
					}
					response.getWriter().write(json.toString());
				}else{
					response.getWriter().print("{\"status\":1,\"errormessage\":\"没有登录\"}");
				}
			} catch (SQLException e) {
				e.printStackTrace();
			} catch (JSONException e) {
				e.printStackTrace();
			} finally{
				DaoUtil.closeAll(conn, null, stmt);
			}	
		}
	}

}
