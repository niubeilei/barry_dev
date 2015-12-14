package operation;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;
import util.JDBCFactory;


import bean.Message;

public class GetMsgList {
	public static List<Message> getMsgListschool(String which,String n,String zky_pctrs,String zky_schoolLaoShi_cloudid,
			String zky_shouLiLaoShi_cloudid,String zky_liuyan_ispublic) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		String sql="select contact_unit,zky_schoolLaoShi_name,zky_instdate,zky_objid,zky_schoolLaoShi_cloudid,zky_shouLiLaoShi_cloudid,zky_liuyan_ispublic from \""+zky_pctrs+"\"";
		String sql2=" order by zky_instdate desc limit 0,"+n;
		String where="";
		if(which.equals("0")){
			System.out.println("学校老师");
			where=" where zky_schoolLaoShi_cloudid='"+zky_schoolLaoShi_cloudid+"'";
		}else if(which.equals("1")){
			System.out.println("中心老师");
			where=" where zky_shouLiLaoShi_cloudid='"+zky_shouLiLaoShi_cloudid+"' and zky_liuyan_ispublic='私有'";
		}else if(which.equals("2")){
			System.out.println("公共");
			where=" where zky_shouLiLaoShi_cloudid='"+zky_shouLiLaoShi_cloudid+"' and zky_liuyan_ispublic='"+zky_liuyan_ispublic+"'";
		}
		sql=sql+where+sql2;
		ResultSet rs=stmt.executeQuery(sql);
		System.out.println(rs);
		List<Message> listmsg=new ArrayList<Message>();
		Message m=null;
		while(rs.next()){
			System.out.println(rs.getString(3)+":=======&&&*89898989798789");
			m=new Message();
			if((rs.getString(1) != null)){
			m.setContact_unit(rs.getString(1));
			}
			if((rs.getString(2) != null)){
				m.setZky_schoolLaoShi_name(rs.getString(2));
			}
			if(rs.getString(3) != null){
				m.setZky_instdate(rs.getString(3));
			}else if((rs.getString(3).equals(""))){
				m.setZky_instdate("");
			}
			if((rs.getString(4) != null)){
			m.setZky_objid(rs.getString(4));
			}
			listmsg.add(m);
		}
		
		return listmsg;
	}
	
	public static void main(String[] args) throws SQLException {
		List<Message> list=GetMsgList.getMsgListschool("0","1","luoan_liuyan","12345678",null,null);
		for(Message m:list){
			System.out.println(m.getContact_unit()+":"+m.getZky_schoolLaoShi_name()+":"+m.getZky_instdate());
		}
	}
}
