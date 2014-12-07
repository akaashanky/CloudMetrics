package com.cloudmetrics.web;

import java.io.IOException;
import java.util.ArrayList;
import java.util.StringTokenizer;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.annotation.WebFilter;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

public class SessionFilter implements Filter {
	private ArrayList<String> urlList = new ArrayList<String>();
	 
    public SessionFilter() {
    }

	public void destroy() {
	}

	public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) throws IOException, ServletException {
		 HttpServletRequest req = (HttpServletRequest) request;
	        HttpServletResponse res = (HttpServletResponse) response;
	         
	        String uri = req.getRequestURI();	         
	        HttpSession session = req.getSession(false);
	        String signInURL = "../cm/signin.html";
	         
	        if(session == null && !(uri.endsWith("html") || uri.endsWith("logout") 
	        		|| uri.endsWith("choosePassword") || uri.endsWith("login")
	        		|| uri.endsWith("createCompanyWithMinParams") || uri.endsWith("setpass"))){
	            res.sendRedirect(signInURL);
	        }else{
	            // pass the request along the filter chain
	            chain.doFilter(request, response);
	        }
	}

	public void init(FilterConfig config) throws ServletException {
		
	}

}
