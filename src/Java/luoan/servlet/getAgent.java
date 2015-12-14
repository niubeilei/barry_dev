package servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import operation.GetAgent;


import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import bean.Account;

public class getAgent extends HttpServlet {
	private static final long serialVersionUID = 1L;


	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);
		
	}

	
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		String zky_class=request.getParameter("zky_class").trim();
		String zky_pctrs=request.getParameter("zky_pctrs").trim();
		String zky_otype=request.getParameter("zky_otype").trim();
		response.setCharacterEncoding("UTF-8");
		PrintWriter pw=response.getWriter();
		JSONObject json=new JSONObject();
		JSONArray jsonarr=new JSONArray();
		JSONObject jsonmem=new JSONObject();
		List<Account> listacc=new ArrayList<Account>();
		try {
			listacc = GetAgent.getAgent(zky_class, zky_pctrs, zky_otype);
			for(Account a:listacc){
				jsonmem.put("zky_class", a.getZky_class());
				jsonmem.put("zky_name", a.getZky_name());
				jsonmem.put("zky_uname", a.getZky_uname());
				jsonarr.put(jsonmem);
			}
			
			json.put("msg", jsonmem);
			json.put("status", "0");
			pw.print(json.toString());
			System.out.println(json.toString());
		} catch (SQLException e) {
			try {
				json.put("status", "1");
			} catch (JSONException e1) {
				e1.printStackTrace();
			}
			e.printStackTrace();
		} catch (JSONException e) {
			try {
				json.put("status", "1");
			} catch (JSONException e1) {
				e1.printStackTrace();
			}
			e.printStackTrace();
		}
		
		
	}

}
