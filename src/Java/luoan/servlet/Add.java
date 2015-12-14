package servlet;

import java.io.IOException;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import util.DaoUtil1;

import com.zykie.jdbc.Connection;
import com.zykie.jdbc.Statement;


public class Add extends HttpServlet {

	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doPost(request, response);

	}

	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		Connection conn = DaoUtil1.getConnection();
		Statement stmt = null;
		String sql = "create table \"luoan_申请单\"(zky_school varchar(100),zky_title varchar(20),contact_id varchar(40)," +
		"contact varchar(20),contact_phone varchar(20),contact_Email varchar(20),zky_schoolLaoShi_note varchar(20)," +
		"zky_service_type varchar(20),zky_service_item varchar(20),zky_shouLiLaoShi_name varchar(20),zky_shouLiLaoShi_cloudid " +
		" varchar(20),zky_status varchar(20),zky_instdate varchar(20),zky_checkdate varchar(20),zky_shouLiLaoShi_history " +
		"varchar(20),zky_isRollBack varchar(20),zky_role_status varchar(20),zky_objid varchar(20))";
		if(conn!=null) {
			try {
				stmt = (Statement) conn.createStatement();
				int count = stmt.executeUpdate(sql);
				System.out.println(count);
				
				
			} catch (SQLException e) {
				e.printStackTrace();
			} finally{
				DaoUtil1.closeAll(conn, null, stmt);
			}
		}
	}


}

