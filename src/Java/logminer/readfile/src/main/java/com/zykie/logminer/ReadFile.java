package com.zykie.logminer;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class ReadFile {
	public static void main(String[] args) {
		try {
			cope_File();
		} catch (IOException e) {
			e.printStackTrace();
		}
		//read file 
		readFile();
	}
	
	 public static void cope_File() throws IOException {
	        // old file
	        String url1 = "D:/java";
	        // new file
	        String url2 = "D:/java_01";
	        (new File(url2)).mkdirs();
	        // get source file
	        File[] file = (new File(url1)).listFiles();
	        for (int i = 0; i < file.length; i++) {
	            if (file[i].isFile()) {
	                // copy file
	                copyFile(file[i], new File(url2 +"/"+ file[i].getName()));
	            }
	            if (file[i].isDirectory()) {
	                // copy dir
	                String sourceDir = url1 + File.separator + file[i].getName();
	                String targetDir = url2 + File.separator + file[i].getName();
	                copyDirectiory(sourceDir, targetDir);
	            }
	        }
	        del(url1);
	        System.out.println("finish!!!");
	    }

	    // copy file
	    public static void copyFile(File sourceFile, File targetFile) throws IOException {
	        BufferedInputStream inBuff = null;
	        BufferedOutputStream outBuff = null;
	        try {
	            inBuff = new BufferedInputStream(new FileInputStream(sourceFile));
	            outBuff = new BufferedOutputStream(new FileOutputStream(targetFile));
	            byte[] b = new byte[1024 * 5];
	            int len;
	            while ((len = inBuff.read(b)) != -1) {
	                outBuff.write(b, 0, len);
	            }
	            outBuff.flush();
	        } finally {
	            if (inBuff != null)
	                inBuff.close();
	            if (outBuff != null)
	                outBuff.close();
	        }
	    }

	    public static void copyDirectiory(String sourceDir, String targetDir) throws IOException {
	        (new File(targetDir)).mkdirs();
	        File[] file = (new File(sourceDir)).listFiles();
	        for (int i = 0; i < file.length; i++) {
	            if (file[i].isFile()) {
	                File sourceFile = file[i];
	                File targetFile = new File(new File(targetDir).getAbsolutePath() + File.separator + file[i].getName());
	                copyFile(sourceFile, targetFile);
	            }
	            if (file[i].isDirectory()) {
	                String dir1 = sourceDir + "/" + file[i].getName();
	                String dir2 = targetDir + "/" + file[i].getName();
	                copyDirectiory(dir1, dir2);
	            }
	        }
	    }
	    
	    /**
	     * 
	     * @param srcFileName
	     * @param destFileName
	     * @param srcCoding
	     * @param destCoding
	     * @throws IOException
	     */
	    public static void copyFile(File srcFileName, File destFileName, String srcCoding, String destCoding) throws IOException {
	        BufferedReader br = null;
	        BufferedWriter bw = null;
	        try {
	            br = new BufferedReader(new InputStreamReader(new FileInputStream(srcFileName), srcCoding));
	            bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(destFileName), destCoding));
	            char[] cbuf = new char[1024 * 5];
	            int len = cbuf.length;
	            int off = 0;
	            int ret = 0;
	            while ((ret = br.read(cbuf, off, len)) > 0) {
	                off += ret;
	                len -= ret;
	            }
	            bw.write(cbuf, 0, off);
	            bw.flush();
	        } finally {
	            if (br != null)
	                br.close();
	            if (bw != null)
	                bw.close();
	        }
	    }

	    /**
	     * 
	     * @param filepath
	     * @throws IOException
	     */
	    public static void del(String filepath) throws IOException {
	        File f = new File(filepath);
	        if (f.exists() && f.isDirectory()) {
	            if (f.listFiles().length == 0) {
	                f.delete();
	            } else {
	                File delFile[] = f.listFiles();
	                int i = f.listFiles().length;
	                for (int j = 0; j < i; j++) {
	                    if (delFile[j].isDirectory()) {
	                        del(delFile[j].getAbsolutePath());	                    }
	                    delFile[j].delete();
	                }
	            }
	        }
	    }
	
	/**
	 * read file
	 */
	public static void readFile(){
		FileInputStream fis = null;
		InputStreamReader isr = null;
		BufferedReader br = null; 
		try {
			while(true){
				String str = "";
				// String str1 = "";
				fis = new FileInputStream("C:\\app\\Administrator\\oradata\\practice\\LOGMNR\\sql.log");// FileInputStream
				isr = new InputStreamReader(fis);
				br = new BufferedReader(isr);
												
				 while ((str = br.readLine()) != null) {
				// str1 += str + "\n";
					 	Connection conn = getConnection();
						Statement stmt = null;
						String sql = str;
						try {
							stmt = conn.createStatement();
							int count = stmt.executeUpdate(sql);
							System.out.println(count);
						} catch (SQLException e) {
							e.printStackTrace();
						} finally{
							closeAll(conn, null, stmt);
						}
				 }
				 try {
					Thread.sleep(3000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			
			 }
			// System.out.println(str1);
		} catch (FileNotFoundException e) {
			System.out.println("file not find");
		} catch (IOException e) {
			System.out.println("read fail");
		} finally {
			try {
				br.close();
				isr.close();
				fis.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	/**
	 * connect database
	 * @return
	 */
	public static Connection getConnection(){
		Connection conn = null;
		try {
			Class.forName("com.zykie.jdbc.Driver");
			conn = DriverManager.getConnection("jdbc:zykie://192.168.99.207:2010/db" , "root", "12345");
			System.out.println("JDBC conn success !!!");
			} catch(SQLException e) {
				System.out.println("error:"+e.getStackTrace());
			} catch(ClassNotFoundException e) {
				System.out.println("error:"+e.getStackTrace());
			}
		return conn;
	}
	
	/**
	 * close database
	 * @param conn
	 * @param rs
	 * @param stmt
	 */
	public static void closeAll(Connection conn,ResultSet rs,Statement stmt){
		try{
			if(conn != null) {
				conn.close();
				conn = null;
			}
			if(rs != null) {
				rs.close();
				rs = null;
			}
			if(stmt != null) {
				stmt.close();
				stmt = null;
			}
		} catch(SQLException e) {
			e.printStackTrace();
		}
	}
}
