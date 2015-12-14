package operation;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import util.JDBCFactory;

public class GetUsers {
	public static String getUsers(String zky_name) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement(); 
		String sql="select zky_uname,zky_name,zky_otype from \"luoan_account\"";
		String where=" where zky_name='"+zky_name+"' and zky_otype='zky_uact'";
		sql=sql+where;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		String s="";
		while(rs.next()){
			//m=new Message();
			if(rs.getString(1) != null)
			s=rs.getString(1);
		}
		return s;
	}
}
