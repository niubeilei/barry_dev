package operation;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import util.JDBCFactory;

public class GetSchools {
	public static List<String> getSchools(String zky_class) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement(); 
		String sql="select zky_name,zky_class,zky_otype from \"luoan_account\"";
		String where=" where zky_class='"+zky_class+"' and zky_otype='zky_uact' limit 300";
		sql=sql+where;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		List<String> listschool=new ArrayList<String>();
		while(rs.next()){
			//m=new Message();
			String s=null;
			if(rs.getString(1) !=null){
				System.out.println("不null==========");
				s=rs.getString(1);
				listschool.add(s);
			}
			
			
		}
		return listschool;
	}
}
