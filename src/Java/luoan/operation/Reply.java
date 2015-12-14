package operation;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.SimpleDateFormat;
import java.util.Date;

import util.JDBCFactory;

import bean.Replay;

import com.zykie.jdbc.ResultSet;

public class Reply {
	public static boolean reply(String zky_pctrs,Replay reply) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		Date date=new Date();
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		String handle_date=formatter.format(date);
		String sql="insert into \""+zky_pctrs+"\"(zky_handle_date,zky_handle_uname,zky_handle_name,zky_handle_cloudid," +
		"zky_handle_unit, \"zky_handle_note/_#text\"" +
		",zky_status,liuyan_id)" +
		"values('"+handle_date+"','"+reply.getZky_handle_uname()+"','"+reply.getZky_handle_name()+"','"+reply.getZky_handle_cloudid()+"','"+reply.getZky_handle_unit()+"','"+reply.getZky_handle_note()+"'," +
		"'已回复','"+reply.getLiuyan_id()+"')";
		System.out.println(reply.getZky_handle_note()+"note-=======");
		stmt.executeUpdate(sql);
		ResultSet result=(ResultSet) stmt.getResultSet();
		boolean b=result.getQueryResponse().getError();
		return b;
	}
}
