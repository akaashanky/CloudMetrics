package com.cloudmetrics.web.servlet;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

@WebServlet("/ImageServlet")
public class ImageServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
       
    
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		int BUFF_SIZE = 1024;
		byte[] buffer = new byte[BUFF_SIZE];
		File fileMp3 = new File("/Users/mpanda/Documents/chanderi2.jpg");
		FileInputStream fis = new FileInputStream(fileMp3);
		response.setContentType("audio/mpeg");
		response.setHeader("Content-Disposition", "filename=\"hoge.txt\"");
		response.setContentLength((int) fileMp3.length());
		OutputStream os = response.getOutputStream();

		try {
		    int byteRead = 0;
		    while ((byteRead = fis.read()) != -1) {
		       os.write(buffer, 0, byteRead);

		    }
		    os.flush();
		} catch (Exception excp) {
		    //downloadComplete = "-1";
		    excp.printStackTrace();
		} finally {
		    os.close();
		    fis.close();
		}
	}

	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
	}

}
