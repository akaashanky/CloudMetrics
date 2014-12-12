package com.cloudmetrics.businesslogic.financial.kpi;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.cloudmetrics.businesslogic.financial.BaseFinancialInfo;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.businesslogic.tally.GenericGroupOrLedger;
import com.cloudmetrics.businesslogic.tally.Group;
import com.cloudmetrics.domain.Company;

public class Profitability extends BaseFinancialInfo{
	Double revenue;
	Double momRevenueGrowth;
	List<Double> monthlyRevenueList = new ArrayList<Double>();
	
	Double grossProfit;
	Double momGrossProfitGrowth;
	List<Double> monthlyGrossProfitList = new ArrayList<Double>();
	
	Double netProfit;
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
							addOneAmountListToAnother(monthlyGrossRevenue, monthlyClosingBalance);
						}else{
							grossExpense += thisYearClosingBalance;
							addOneAmountListToAnother(monthlyGrossExpense, monthlyClosingBalance);
						}
					}else{
						if(!group.isDeemedPositive()){
							nonGrossRevenue += thisYearClosingBalance;
							addOneAmountListToAnother(monthlyNonGrossRevenue, monthlyClosingBalance);
						}else{
							nonGrossExpense += thisYearClosingBalance;
							addOneAmountListToAnother(monthlyNonGrossExpense, monthlyClosingBalance);
						}
					}
				}
			}
		}
		//now calculate the actual stuff!
		this.revenue = grossRevenue;
		this.monthlyRevenueList = monthlyGrossRevenue;
		this.momRevenueGrowth = getLastPeriodGrowthNumber(this.monthlyRevenueList);
		this.momRevenueGrowth = (double)Math.round(this.momRevenueGrowth * 100) / 100;
		
		this.grossProfit = grossRevenue + grossExpense; //"Plus" because the amount is already sign changed
		addOneAmountListToAnother(this.monthlyGrossProfitList, monthlyGrossRevenue);
		addOneAmountListToAnother(this.monthlyGrossProfitList, monthlyGrossExpense);
		this.momGrossProfitGrowth = getLastPeriodGrowthNumber(this.monthlyGrossProfitList);
		this.momGrossProfitGrowth = (double)Math.round(this.momGrossProfitGrowth * 100) / 100;
		
		this.netProfit = grossProfit + nonGrossRevenue + nonGrossExpense;
		addOneAmountListToAnother(this.monthlyNetProfitList, this.monthlyGrossProfitList);
		addOneAmountListToAnother(this.monthlyNetProfitList, monthlyNonGrossRevenue);
		addOneAmountListToAnother(this.monthlyNetProfitList, monthlyNonGrossExpense);
		this.momNetProfitGrowth = getLastPeriodGrowthNumber(this.monthlyNetProfitList);
		this.momNetProfitGrowth = (double)Math.round(this.momNetProfitGrowth * 100) / 100;
		
		this.netProfitMargin = this.netProfit/this.revenue * 100;
		this.netProfitMargin = (double)Math.round(this.netProfitMargin * 100) / 100;
		super.setFinancialYear("FY 13-14");
	}
	
	public List<Double> addOneAmountListToAnother(List<Double> result, List<Double> target){
		boolean isThisACopyTask = false; //If result and target are not of same size, simply copy target data to result
		if(result.size() != target.size()){
			isThisACopyTask = true;
			result.clear();
		}
		for(int i = 0; i < target.size() - 1; i++){
			if(isThisACopyTask == false){
				result.set(i, result.get(i) + target.get(i));	
			}else{
				result.add(target.get(i));
			}
		}
		return result;
	}
	
	public List<Double> subtractOneAmountListFromAnother(List<Double> result, List<Double> target){
		boolean isThisACopyTask = false; //If result and target are not of same size, simply copy target data to result
		if(result.size() != target.size()){
			isThisACopyTask = true;
			result.clear();
		}
		for(int i = 0; i < target.size() - 1; i++){
			if(isThisACopyTask == false){
				result.set(i, result.get(i) - target.get(i));	
			}else{
				result.add(target.get(i) * -1.0 );
			}
		}
		return result;
	}
	
	public Double getLastPeriodGrowthNumber(List<Double> listOfAmounts){
		int noOfElements = listOfAmounts.size();
		if(noOfElements <= 1){
			return 0.0;
		}else{
			return (listOfAmounts.get(noOfElements - 1) - listOfAmounts.get(noOfElements - 2))/listOfAmounts.get(noOfElements - 1);
		}
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
	

}
