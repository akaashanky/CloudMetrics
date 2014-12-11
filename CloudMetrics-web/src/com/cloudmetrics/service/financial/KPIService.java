package com.cloudmetrics.service.financial;

import java.io.File;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.transaction.annotation.Transactional;

import com.cloudmetrics.businesslogic.financial.kpi.Profitability;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.service.businesslogic.finance.FinancialDataPopulationService;
import com.cloudmetrics.service.businesslogic.finance.XMLToModelObjectConverter;
import com.cloudmetrics.service.company.EditCompanyService;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.util.EnvManager;
import com.cloudmetrics.web.AppResponse;

public class KPIService {
	@Autowired
    EditCompanyService editCompanyService;
	
	@Transactional
	public AppResponse<Profitability> getProfitabilityKPIs(Integer companyId, Profitability profitabilityObjInSession){
		AppResponse<Profitability> appResponse = new AppResponse<Profitability>();
		
		//check whether profitability exists in the session and the fin data in the DB is not dirty
		if(profitabilityObjInSession == null || editCompanyService.isStateDirty(companyId).getData()){		
			CompanyFinancialData financialData = new CompanyFinancialData();
			String financialDataStorageLocation = EnvManager.getFinancialDataStorageLocation();
			try {
				XMLToModelObjectConverter.construct(financialData, financialDataStorageLocation + File.separator + companyId + "_master.xml");
			} catch (Exception e) {
				e.printStackTrace();
			}
			FinancialDataPopulationService.populateMonthlyData(financialData, financialDataStorageLocation + File.separator + companyId + "_findatamonthly.csv");
			FinancialDataPopulationService.populateAnnualData(financialData, financialDataStorageLocation + File.separator + companyId + "_findatayearly.csv");
					
			appResponse.setData(new Profitability(financialData));
			appResponse.setCode(EventStatus.success.getValue());
		}else{
			appResponse.setData(profitabilityObjInSession);
			appResponse.setCode(EventStatus.success.getValue());
		}
		
		return appResponse; 
	}
}
