package operation;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

import com.zykie.jdbc.ResultSet;

import bean.Message;

import util.JDBCFactory;

public class SendMsg {
	public static boolean sengMsg(Message msg,String zky_pctrs) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		String sql="insert into \""+zky_pctrs+"\"(contact_unit,contact,zky_schoolLaoShi_name,zky_schoolLaoShi_cloudid," +
		"zky_schoolLaoShi_uname,zky_instdate,contact_phone,contact_Email,zky_shouLiLaoShi_cloudid,\"note/_#text\"" +
		",zky_status,zky_liuyan_ispublic)" +
		"values('"+msg.getContact_unit()+"','"+msg.getContact()+"','"+msg.getZky_schoolLaoShi_name()+"','"+msg.getZky_schoolLaoShi_cloudid()+"','"+msg.getZky_schoolLaoShi_uname()+"'," +
		"'"+msg.getZky_instdate()+"','"+msg.getContact_phone()+"','"+msg.getContact_Email()+"','"+msg.getZky_shouLiLaoShi_cloudid()+"','"+msg.getNote()+"'" +
		",'"+msg.getZky_status()+"','"+msg.getZky_liuyan_ispublic()+"')";
		//stmt.execute(sql);
		stmt.executeUpdate(sql);
		ResultSet result=(ResultSet) stmt.getResultSet();
		boolean b=result.getQueryResponse().getError();
		return b;
		
	}
	
	public static void main(String[] args) throws SQLException {
		//SendMsg.sengMsg(null);
	}
}
