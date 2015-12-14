package operation;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

import util.JDBCFactory;
import bean.Message;
import bean.Replay;

public class GetMsg {
	public static Message getMsg(String token,String OBJID,String zky_pctrs) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement(); 
		String sql="select contact_unit,zky_schoolLaoShi_name,zky_instdate,zky_content xpath \"note/_#text\",zky_objid from \""+zky_pctrs+"\"";
		String where=" where zky_objid='"+OBJID+"'";
		sql=sql+where;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		Message m=new Message();
		while(rs.next()){
			//m=new Message();
			System.out.println(rs.getString(1)+":"+rs.getString(2)+":"+rs.getString(3)+":"+rs.getString(4)+"========");
			System.out.println("rs.getString(1)="+rs.getString(1)+"&&&&&&&&&&&&&&&&&&&&&");
			if((rs.getString(1) != null)){
				m.setContact_unit(rs.getString(1));
			}
			if((rs.getString(2) != null)){
				m.setZky_schoolLaoShi_name(rs.getString(2));
			}
			//if(rs.getString(3) != null && !(rs.getString(3).equals("null"))&& !(rs.getString(3).equals(""))){
			if(rs.getString(3) != null){
				//m.setZky_instdate(rs.getString(3).substring(0, 10));
				m.setZky_instdate(rs.getString(3));
			}else if((rs.getString(3).equals(""))){
				m.setZky_instdate("");
			}
			if((rs.getString(4) != null)){
				m.setNote(rs.getString(4));
			}
		}
		return m;
	}
	
	public static List<Replay> getMsgReplay(String token,String zky_pctrs,String liuyan_id) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement(); 
		String sql="select zky_handle_unit,zky_handle_name,zky_handle_date,zky_handle_note xpath \"zky_handle_note/_#text\",liuyan_id from \""+zky_pctrs+"\"";
		String where=" where liuyan_id='"+liuyan_id+"'";
		sql=sql+where;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		List<Replay> list=new ArrayList<Replay>();
		Replay r=null;
		while(rs.next()){
			r=new Replay();
			System.out.println(rs.getString(1)+":"+rs.getString(2)+":"+rs.getString(3)+":"+rs.getString(4)+"========");
			if((rs.getString(1)!= null)){
				r.setZky_handle_unit(rs.getString(1));
			}
			if((rs.getString(2)!= null)){
				r.setZky_handle_name(rs.getString(2));
			}
			
			if(rs.getString(3) != null){
				r.setZky_handle_date(rs.getString(3));
			}else if((rs.getString(3).equals(""))){
				r.setZky_handle_date("");
			}
			if((rs.getString(4)!= null)){
				r.setZky_handle_note(rs.getString(4));
			}
			list.add(r);
		}
		
		return list;
	}
	
	public static void main(String[] args) throws SQLException {
		Message m=GetMsg.getMsg("12412", "adfsdf", "luoan_liuyan");
		System.out.println(m.getContact_unit());
		System.out.println(m.getZky_schoolLaoShi_name());
		System.out.println(m.getZky_instdate());
		System.out.println(m.getNote());
	}
}
