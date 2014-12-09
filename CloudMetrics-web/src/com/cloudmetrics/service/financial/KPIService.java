package com.cloudmetrics.service.financial;

import org.springframework.transaction.annotation.Transactional;

import com.cloudmetrics.businesslogic.financial.kpi.Profitability;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.service.businesslogic.finance.FinancialDataPopulationService;
import com.cloudmetrics.service.businesslogic.finance.XMLToModelObjectConverter;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.web.AppResponse;

public class KPIService {
	@Transactional
	public AppResponse<Profitability> getProfitabilityKPIs(){
		AppResponse<Profitability> appResponse = new AppResponse<Profitability>();
		//TODO check whether the file has changed in the file store or things are null.
		//For now just parse schema and finance data for every request.
		CompanyFinancialData financialData = new CompanyFinancialData();
		try {
			XMLToModelObjectConverter.construct(financialData, "d:/__junk__/LedgerMaster.xml");
		} catch (Exception e) {
			e.printStackTrace();
		}
		FinancialDataPopulationService.populate(financialData, "d:/__junk__/tb2_result.csv");
		
		appResponse.setCode(EventStatus.success.getValue());
		appResponse.setData(new Profitability(true));
		
		return appResponse; 
	}
}
