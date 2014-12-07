package com.cloudmetrics.controllers;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import com.cloudmetrics.common.MessageCollection;
import com.cloudmetrics.domain.Company;
import com.cloudmetrics.service.company.EditCompanyService;
import com.cloudmetrics.util.CollectionOfUtilityMethods;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.web.AppResponse;

@Controller
@RequestMapping("/company")
public class CompanyController {
    @Autowired
    EditCompanyService editCompanyService;
    
    @RequestMapping(value="/createCompanyWithMinParams", method = RequestMethod.POST)
    @ResponseBody
    public AppResponse<Integer> addCompanyWithMinParams(HttpServletRequest req){
    		String companyName = req.getParameter("company_name");
    		String email = req.getParameter("company_email");
    		String phone = req.getParameter("company_phone");
    		String password = req.getParameter("company_password");

    	    return editCompanyService.createCompanyWithMinimumParams(companyName, email, password);
    }   
    
       
    @RequestMapping(value="/profile", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Company> getCompanyForSession(HttpServletRequest req){
    	if(!CollectionOfUtilityMethods.isReqInSession(req)){
    		return new AppResponse<Company>(EventStatus.nosession.getValue(), null, MessageCollection.NEED_TO_LOGIN_TO_ACCESS_THIS_FEATURE);
    	}
    	HttpSession session = req.getSession(false);
    	Integer CompanyId = (Integer)session.getAttribute("Companyid");
    	if(CompanyId == null){
    		return new AppResponse<Company>(EventStatus.nosession.getValue(), null, MessageCollection.NEED_TO_LOGIN_TO_ACCESS_THIS_FEATURE);
    	}    	
    	return editCompanyService.getCompanyById(CompanyId);
    }
    
  
    @RequestMapping(value="/profile/{id}", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Company> getCompanyById(Model model, @PathVariable("id") int id){
    	return editCompanyService.getCompanyById(id);
    }
    
    @RequestMapping(value="/testlogin", method = RequestMethod.POST)
    @ResponseBody
    public AppResponse<Integer> isCompanyValid(HttpServletRequest req){
    	String email = req.getParameter("email");
    	String password = req.getParameter("password");
    	return editCompanyService.isCompanyValid(email, password);
    }
    
    @RequestMapping(value="/setpass", method = RequestMethod.POST)
    @ResponseBody
    public AppResponse<Integer> setPassword(HttpServletRequest req){
    	String email = req.getParameter("email");
    	String password = req.getParameter("password");
    	return editCompanyService.saveCompanyPassword(email, password);
    }
}
