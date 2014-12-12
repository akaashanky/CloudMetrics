package com.cloudmetrics.businesslogic.financial;

import com.cloudmetrics.domain.Company;

public class BaseFinancialInfo {
	Company company;
	String financialYear;
	
	public BaseFinancialInfo(Company company) {
		this.company = company;
	}
	public Company getCompany() {
		return company;
	}
	public void setCompany(Company company) {
		this.company = company;
	}
	public String getFinancialYear() {
		return financialYear;
	}
	public void setFinancialYear(String financialYear) {
		this.financialYear = financialYear;
	}	
}
