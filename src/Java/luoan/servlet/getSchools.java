package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import operation.GetSchools;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class getSchools extends HttpServlet {
	private static final long serialVersionUID = 1L;


	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
	}

	
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		String zky_class=request.getParameter("agent");
		response.setCharacterEncoding("UTF-8");
		PrintWriter pw=response.getWriter();
		if(zky_class != null){
			zky_class=zky_class.trim();
		JSONObject json=new JSONObject();
		JSONArray jsonarr=new JSONArray();
		try {
			List<String> listschool=GetSchools.getSchools(zky_class);
			for(String s:listschool){
				jsonarr.put(s);
			}
			json.put("school", jsonarr);
			json.put("status", "0");
			pw.print(json.toString());
			System.out.println(json.toString());
		} catch (SQLException e) {
			try {
				json.put("status", "1");
				pw.print(json.toString());
				System.out.println(json.toString());
			} catch (JSONException e1) {
				e1.printStackTrace();
			}
			e.printStackTrace();
		} catch (JSONException e) {
			try {
				json.put("status", "1");
				pw.print(json.toString());
				System.out.println(json.toString());
			} catch (JSONException e1) {
				e1.printStackTrace();
			}
			e.printStackTrace();
		}
	
	}else{
		pw.print("{\"status\":1,\"message\":\"传入的参数不全\"}");
	}
	}

}
