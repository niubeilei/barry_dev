package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import operation.GetUsers;

import org.json.JSONException;
import org.json.JSONObject;

public class getUsers extends HttpServlet {
	private static final long serialVersionUID = 1L;


	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
	}


	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		String zky_name=request.getParameter("school");
		response.setCharacterEncoding("UTF-8");
		PrintWriter pw=response.getWriter();
		if(zky_name != null){
			zky_name=zky_name.trim();
		JSONObject json=new JSONObject();
		try {
			String user=GetUsers.getUsers(zky_name);
			json.put("users", user);
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
