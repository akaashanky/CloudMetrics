package com.cloudmetrics.controllers.financial;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import com.cloudmetrics.businesslogic.financial.FinancialConstants;
import com.cloudmetrics.businesslogic.financial.kpi.Liquidity;
import com.cloudmetrics.businesslogic.financial.kpi.Profitability;
import com.cloudmetrics.common.MessageCollection;
import com.cloudmetrics.service.financial.KPIService;
import com.cloudmetrics.util.ApplicationConstants;
import com.cloudmetrics.util.CollectionOfUtilityMethods;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.web.AppResponse;


@Controller
@RequestMapping("/financial/kpi")
public class KPIController {
    @Autowired
    KPIService kpiService;
    
    @RequestMapping(value="/profitability", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Profitability> getProfitabilityKPIs(HttpServletRequest req){
    	if(!CollectionOfUtilityMethods.isReqInSession(req)){
    		return new AppResponse<Profitability>(EventStatus.nosession.getValue(), null, MessageCollection.NEED_TO_LOGIN_TO_ACCESS_THIS_FEATURE);
    	}    	    	
    	//Look for the Profitability in the session.
    	HttpSession session = req.getSession(false);
    	Integer companyId = (Integer)session.getAttribute(ApplicationConstants.COMPANY_ID);
    	Profitability profitabilityObjInSession = (Profitability)session.getAttribute(FinancialConstants.KPI_PROFITABILITY);
    	
    	return kpiService.getProfitabilityKPIs(companyId, profitabilityObjInSession);
    }   
    @RequestMapping(value="/profitability_test", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Profitability> getProfitabilityKPIsTest(HttpServletRequest req){
    	return kpiService.getProfitabilityKPIs(2, null);
    }   
    
    @RequestMapping(value="/liquidity", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Liquidity> getLiquidityKPIs(HttpServletRequest req){
    	if(!CollectionOfUtilityMethods.isReqInSession(req)){
    		return new AppResponse<Liquidity>(EventStatus.nosession.getValue(), null, MessageCollection.NEED_TO_LOGIN_TO_ACCESS_THIS_FEATURE);
    	}    	    	
    	//Look for the Liquidity in the session.
    	HttpSession session = req.getSession(false);
    	Integer companyId = (Integer)session.getAttribute(ApplicationConstants.COMPANY_ID);
    	Liquidity liquidityObjInSession = (Liquidity)session.getAttribute(FinancialConstants.KPI_PROFITABILITY);
    	
    	return kpiService.getLiquidityKPIs(companyId, liquidityObjInSession);
    } 
    @RequestMapping(value="/liquidity_test", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Liquidity> getLiquidityKPIsTest(HttpServletRequest req){
    	AppResponse<Liquidity> appResponse = kpiService.getLiquidityKPIs(2, null);
    	return appResponse;
    } 

}
