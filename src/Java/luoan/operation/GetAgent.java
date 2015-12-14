package operation;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import util.JDBCFactory;
import bean.Account;

public class GetAgent {
	public static List<Account> getAgent(String zky_class,String zky_pctrs,String zky_otype) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement(); 
		String sql="select zky_class,zky_name,zky_uname from \""+zky_pctrs+"\"";
		String where=" where zky_class='"+zky_class+"' and zky_otype='zky_uact'";
		sql=sql+where;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		List<Account> listacc=new ArrayList<Account>();
		while(rs.next()){
			//m=new Message();
			Account a=new Account();
			a.setZky_class(rs.getString(1));
			a.setZky_name(rs.getString(2));
			a.setZky_uname(rs.getString(3));
			listacc.add(a);
		}
		return listacc;
	}
}
