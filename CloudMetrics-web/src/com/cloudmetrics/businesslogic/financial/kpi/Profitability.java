package com.cloudmetrics.businesslogic.financial.kpi;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import com.cloudmetrics.businesslogic.financial.BaseFinancialInfo;
import com.cloudmetrics.businesslogic.financial.FinancialUtils;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.businesslogic.tally.GenericGroupOrLedger;
import com.cloudmetrics.businesslogic.tally.Group;
import com.cloudmetrics.domain.Company;

public class Profitability extends BaseFinancialInfo{
	Double revenue;
	String formattedRevenue;
	Double momRevenueGrowth;
	List<Double> monthlyRevenueList = new ArrayList<Double>();
	
	Double grossProfit;
	String formattedGrossProfit;
	Double momGrossProfitGrowth;
	List<Double> monthlyGrossProfitList = new ArrayList<Double>();
	
	Double netProfit;
	String formattedNetProfit;
	Double momNetProfitGrowth;
	List<Double> monthlyNetProfitList = new ArrayList<Double>();
	
	Double netProfitMargin;
	
	/**
	 * Revenue = 
	 * @param financialData
	 */
	public Profitability(Company company,CompanyFinancialData financialData){
		super(company);
		//Get Direct income node, Direct expense, Indirect income & indirect expense.
		Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> twoLevelGroupAndLedgerMap = financialData.getTwoLevelGroupAndLedgerMap();
		Double grossRevenue = 0.0;
		Double grossExpense = 0.0;
		Double nonGrossRevenue = 0.0;
		Double nonGrossExpense = 0.0;
		
		List<Double> monthlyGrossRevenue = new ArrayList<Double>();
		List<Double> monthlyGrossExpense = new ArrayList<Double>();
		List<Double> monthlyNonGrossRevenue = new ArrayList<Double>();
		List<Double> monthlyNonGrossExpense = new ArrayList<Double>();
				
		//Get a suspect list for 2 sides. 1. Revenue 2. Expense
		for (GenericGroupOrLedger primaryGroupOrLedger : twoLevelGroupAndLedgerMap.keySet()) {
			//The top most is the closing balance for this year.
			Double thisYearClosingBalance = primaryGroupOrLedger.getAnnualClosingBalance();
			List<Double> monthlyClosingBalance = primaryGroupOrLedger.getMonthlyClosingBalance();
			if(thisYearClosingBalance == null || monthlyClosingBalance == null)
				continue;
			if(primaryGroupOrLedger instanceof Group){
				Group group = (Group)primaryGroupOrLedger;
				if(group.isRevenue()){ //Remember the negativity or positivity when deciding on "add" or "subtract"
					if(group.isAffectsGrossProfit()){
						if(!group.isDeemedPositive()){
							grossRevenue += thisYearClosingBalance;
							FinancialUtils.addOneAmountListToAnother(monthlyGrossRevenue, monthlyClosingBalance);
						}else{
							grossExpense += thisYearClosingBalance;
							FinancialUtils.addOneAmountListToAnother(monthlyGrossExpense, monthlyClosingBalance);
						}
					}else{
						if(!group.isDeemedPositive()){
							nonGrossRevenue += thisYearClosingBalance;
							FinancialUtils.addOneAmountListToAnother(monthlyNonGrossRevenue, monthlyClosingBalance);
						}else{
							nonGrossExpense += thisYearClosingBalance;
							FinancialUtils.addOneAmountListToAnother(monthlyNonGrossExpense, monthlyClosingBalance);
						}
					}
				}
			}
		}
		//now calculate the actual stuff!
		this.revenue = grossRevenue;
		this.revenue = (double)Math.round(this.revenue);//Round it
		this.monthlyRevenueList = monthlyGrossRevenue;
		this.momRevenueGrowth = FinancialUtils.getLastPeriodGrowthNumber(this.monthlyRevenueList);
		this.momRevenueGrowth = (double)Math.round(this.momRevenueGrowth * 100) / 100;
		
		this.grossProfit = grossRevenue + grossExpense; //"Plus" because the amount is already sign changed
		this.grossProfit = (double)Math.round(this.grossProfit);//Round it
		FinancialUtils.addOneAmountListToAnother(this.monthlyGrossProfitList, monthlyGrossRevenue);
		FinancialUtils.addOneAmountListToAnother(this.monthlyGrossProfitList, monthlyGrossExpense);
		this.momGrossProfitGrowth = FinancialUtils.getLastPeriodGrowthNumber(this.monthlyGrossProfitList);
		this.momGrossProfitGrowth = (double)Math.round(this.momGrossProfitGrowth * 100) / 100;
		
		this.netProfit = grossProfit + nonGrossRevenue + nonGrossExpense;
		this.netProfit = (double)Math.round(this.netProfit);//Round it
		FinancialUtils.addOneAmountListToAnother(this.monthlyNetProfitList, this.monthlyGrossProfitList);
		FinancialUtils.addOneAmountListToAnother(this.monthlyNetProfitList, monthlyNonGrossRevenue);
		FinancialUtils.addOneAmountListToAnother(this.monthlyNetProfitList, monthlyNonGrossExpense);
		this.momNetProfitGrowth = FinancialUtils.getLastPeriodGrowthNumber(this.monthlyNetProfitList);
		this.momNetProfitGrowth = (double)Math.round(this.momNetProfitGrowth * 100) / 100;
		
		this.netProfitMargin = this.netProfit/this.revenue * 100;
		this.netProfitMargin = (double)Math.round(this.netProfitMargin * 100) / 100;
				
		this.formattedRevenue = FinancialUtils.getFormattedAmount(this.revenue);
		this.formattedGrossProfit = FinancialUtils.getFormattedAmount(this.grossProfit);
		this.formattedNetProfit = FinancialUtils.getFormattedAmount(this.netProfit);
		
		super.setFinancialYear(financialData.getFinancialYear());
	}

	public Double getRevenue() {
		return revenue;
	}

	public void setRevenue(Double revenue) {
		this.revenue = revenue;
	}

	public Double getMomRevenueGrowth() {
		return momRevenueGrowth;
	}

	public void setMomRevenueGrowth(Double momRevenueGrowth) {
		this.momRevenueGrowth = momRevenueGrowth;
	}

	public Double getGrossProfit() {
		return grossProfit;
	}

	public void setGrossProfit(Double grossProfit) {
		this.grossProfit = grossProfit;
	}

	public Double getMomGrossProfitGrowth() {
		return momGrossProfitGrowth;
	}

	public void setMomGrossProfitGrowth(Double momGrossProfitGrowth) {
		this.momGrossProfitGrowth = momGrossProfitGrowth;
	}

	public Double getNetProfit() {
		return netProfit;
	}

	public void setNetProfit(Double netProfit) {
		this.netProfit = netProfit;
	}

	public Double getMomNetProfitGrowth() {
		return momNetProfitGrowth;
	}

	public void setMomNetProfitGrowtth(Double momNetProfitGrowth) {
		this.momNetProfitGrowth = momNetProfitGrowth;
	}

	public Double getNetProfitMargin() {
		return netProfitMargin;
	}

	public void setNetProfitMargin(Double profitMargin) {
		this.netProfitMargin = profitMargin;
	}

	public List<Double> getMonthlyRevenueList() {
		return monthlyRevenueList;
	}

	public void setMonthlyRevenueList(List<Double> monthlyRevenueList) {
		this.monthlyRevenueList = monthlyRevenueList;
	}

	public List<Double> getMonthlyGrossProfitList() {
		return monthlyGrossProfitList;
	}

	public void setMonthlyGrossProfitList(List<Double> monthlyGrossProfitList) {
		this.monthlyGrossProfitList = monthlyGrossProfitList;
	}

	public List<Double> getMonthlyNetProfitList() {
		return monthlyNetProfitList;
	}

	public void setMonthlyNetProfitList(List<Double> monthlyNetProfitList) {
		this.monthlyNetProfitList = monthlyNetProfitList;
	}

	public String getFormattedRevenue() {
		return formattedRevenue;
	}

	public void setFormattedRevenue(String formattedRevenue) {
		this.formattedRevenue = formattedRevenue;
	}

	public String getFormattedGrossProfit() {
		return formattedGrossProfit;
	}

	public void setFormattedGrossProfit(String formattedGrossProfit) {
		this.formattedGrossProfit = formattedGrossProfit;
	}

	public String getFormattedNetProfit() {
		return formattedNetProfit;
	}

	public void setFormattedNetProfit(String formattedNetProfit) {
		this.formattedNetProfit = formattedNetProfit;
	}

	public void setMomNetProfitGrowth(Double momNetProfitGrowth) {
		this.momNetProfitGrowth = momNetProfitGrowth;
	}
	
}
