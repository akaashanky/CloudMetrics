package com.cloudmetrics.web.servlet;

import java.io.IOException;
import java.io.PrintWriter;

import javax.annotation.Resource;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

import com.cloudmetrics.service.company.EditCompanyService;
import com.cloudmetrics.util.ApplicationConstants;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.web.AppResponse;

public class LoginServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
	private Log _log = LogFactory.getLog(this.getClass().getCanonicalName());
	
	@Autowired
	private EditCompanyService editCompanyService;
	
	public void init(ServletConfig config) {
		  try {
			super.init(config);
			SpringBeanAutowiringSupport.processInjectionBasedOnServletContext(this,config.getServletContext());
		  } catch (ServletException e) {
			_log.fatal("***init failed***", e.getRootCause());
			e.printStackTrace();
		  }
		}
       
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		String email = request.getParameter("email");
		String password = request.getParameter("password");
		//action can either be "signup" or "login"
		String action = request.getParameter("action");
		
		if(!"logout".equals(action)){
			if(email == null || "".equals(email.trim()) || password == null || "".equals(password.trim())){
				throw new IllegalArgumentException("Invalid credentials");
			}
		}
				
      	AppResponse<Integer> svcResponse = null;
		if("choosePassword".equals(action)){
			svcResponse = editCompanyService.saveCompanyPassword(email, password);
		}else if("logout".equals(action)){
			HttpSession session = request.getSession();
			session.invalidate();
			return;
		}else{ //login
			svcResponse = editCompanyService.isCompanyValid(email, password);
		}
		if(svcResponse.getCode() == EventStatus.failure.getValue()){
			throw new RuntimeException(svcResponse.getDescription());
		}else{
			HttpSession session = request.getSession(true);
	        PrintWriter writer = response.getWriter();
	        
	    	response.setContentType("application/json");

			session.setAttribute(ApplicationConstants.EMAIL, email);
			session.setAttribute(ApplicationConstants.COMPANY_ID, svcResponse.getData());
			JSONObject json = new JSONObject();
            try {
				json.put("id", svcResponse.getData());
			} catch (JSONException e) {
				e.printStackTrace();
				throw new RuntimeException(e);
			}finally{
				 writer.write(json.toString());
		         writer.close();
			}
		}
	}
	
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		doGet(request, response);
	}

}
