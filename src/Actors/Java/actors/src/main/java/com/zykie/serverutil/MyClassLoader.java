package com.zykie.serverutil;

import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;


public class MyClassLoader extends URLClassLoader{
	 public MyClassLoader() {
	        super(new URL[0]);
	    }
	 
	    public void loadJar(String str) {
	        try {
	            URL url = new URL(str);
	            addURL(url);
	        } catch (Exception ex) {
	            ex.printStackTrace();
	        }
	    }
	 
	    public Class<?> findClass(String name) throws ClassNotFoundException {
	        return super.findClass(name);
	    }
	 
	    public Object invokeMethod(Class<?> clazz, String methodName,
	            Class<?>[] parametersTypes, Object[] args) throws Exception {
	        Object obj = clazz.newInstance();
	        Method method = clazz.getMethod(methodName, parametersTypes);
	        Object result = method.invoke(obj, args);
	        return result;
	    }
	 
	    public static void main(String[] args) {
	        MyClassLoader loader = new MyClassLoader();
	        loader.loadJar("file:///D:/TestJar/actor_fat.jar");
	        try {
	        	StringBuffer str=new StringBuffer();
	        	 Class<?> clazz = loader.findClass("com.zykie.actors.jdbc.ActDatabase");
	             Object result = loader.invokeMethod(clazz, "run", new Class[] {
	                     String.class, StringBuffer.class}, new Object[] {
	                     "<actparams><databasename>db4</databasename><operate>create</operate></actparams>", str });
	             System.out.println(result+":"+str);
			} catch (Exception e) {
				e.printStackTrace();
				// TODO: handle exception
			}
	    }
}
