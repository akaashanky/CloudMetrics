package com.cloudmetrics.service.financial;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.transaction.annotation.Transactional;

import com.cloudmetrics.businesslogic.financial.kpi.Liquidity;
import com.cloudmetrics.businesslogic.financial.kpi.Profitability;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.service.businesslogic.finance.FinancialDataPopulationService;
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
			FinancialDataPopulationService.parseMasterAndPopulateDataForTheCompany(companyId, financialData, financialDataStorageLocation);
			
			editCompanyService.setCleanOrDirtyState(companyId, false);
			appResponse.setData(new Profitability(editCompanyService.getCompanyById(companyId).getData(), financialData));
			appResponse.setCode(EventStatus.success.getValue());
		}else{
			appResponse.setData(profitabilityObjInSession);
			appResponse.setCode(EventStatus.success.getValue());
		}
		
		return appResponse; 
	}
	
	/**
	 * 
	 * @param companyId
	 * @param liquidityObjInSession
	 * @return
	 */
	//TODO: Store the companyFinancialData in session and don't recalculate for each metric.
	@Transactional
	public AppResponse<Liquidity> getLiquidityKPIs(Integer companyId, Liquidity liquidityObjInSession){
		AppResponse<Liquidity> appResponse = new AppResponse<>();
		
		//check whether profitability exists in the session and the fin data in the DB is not dirty
		if(liquidityObjInSession == null || editCompanyService.isStateDirty(companyId).getData()){		
			CompanyFinancialData financialData = new CompanyFinancialData();
			String financialDataStorageLocation = EnvManager.getFinancialDataStorageLocation();
			FinancialDataPopulationService.parseMasterAndPopulateDataForTheCompany(companyId, financialData, financialDataStorageLocation);
			
			editCompanyService.setCleanOrDirtyState(companyId, false);
			appResponse.setData(new Liquidity(editCompanyService.getCompanyById(companyId).getData(), financialData));
			appResponse.setCode(EventStatus.success.getValue());
		}else{
			appResponse.setData(liquidityObjInSession);
			appResponse.setCode(EventStatus.success.getValue());
		}
		
		return appResponse; 
	}

}
