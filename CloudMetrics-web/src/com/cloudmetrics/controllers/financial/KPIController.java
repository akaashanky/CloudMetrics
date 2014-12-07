package com.cloudmetrics.controllers.financial;

import javax.servlet.http.HttpServletRequest;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

import com.cloudmetrics.businesslogic.financial.kpi.Profitability;
import com.cloudmetrics.service.financial.KPIService;
import com.cloudmetrics.web.AppResponse;


@Controller
@RequestMapping("/financial/kpi")
public class KPIController {
    @Autowired
    KPIService kpiService;
    
    @RequestMapping(value="/profitability", method = RequestMethod.GET)
    @ResponseBody
    public AppResponse<Profitability> getProfitabilityKPIs(HttpServletRequest req){
    	return kpiService.getProfitabilityKPIs();
    }   

}
