package com.cloudmetrics.service.financial;

import org.springframework.transaction.annotation.Transactional;

import com.cloudmetrics.businesslogic.financial.kpi.Profitability;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.web.AppResponse;

public class KPIService {
	@Transactional
	public AppResponse<Profitability> getProfitabilityKPIs(){
		AppResponse<Profitability> appResponse = new AppResponse<Profitability>();
		appResponse.setCode(EventStatus.success.getValue());
		appResponse.setData(new Profitability(true));
		
		return appResponse; 
	}
}
