package com.cloudmetrics.businesslogic.financial.kpi;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.cloudmetrics.businesslogic.financial.BaseFinancialInfo;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.businesslogic.tally.GenericGroupOrLedger;
import com.cloudmetrics.businesslogic.tally.Group;

public class Profitability extends BaseFinancialInfo{
	Double revenue;
	Double momRevenueGrowth;
	List<Double> monthlyRevenueList = new ArrayList<Double>();
	
	Double grossProfit;
	Double momGrossProfitGrowth;
	List<Double> monthlyGrossProfitList = new ArrayList<Double>();
	
	Double netProfit;
	Double momNetProfitGrowtth;
	List<Double> monthlyNetProfitList = new ArrayList<Double>();
	
	Double profitMargin;
	
	/**
	 * Revenue = 
	 * @param financialData
	 */
	public Profitability(CompanyFinancialData financialData){
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
		
		this.grossProfit = grossRevenue + grossExpense; //"Plus" because the amount is already sign changed
		this.monthlyGrossProfitList = addOneAmountListToAnother(monthlyGrossRevenue, monthlyGrossExpense);
		this.momGrossProfitGrowth = getLastPeriodGrowthNumber(this.monthlyGrossProfitList);
		
		this.netProfit = grossProfit + nonGrossRevenue + nonGrossExpense;
		//this.monthlyNetProfitList = addOneAmountListToAnother(monthlynet, target)
		//TODO Add them
	}
	
	public List<Double> addOneAmountListToAnother(List<Double> result, List<Double> target){
		for(int i = 0; i < target.size(); i++){
			result.set(i, result.get(i) + target.get(i));
		}
		return result;
	}
	
	public List<Double> subtractOneAmountListFromAnother(List<Double> result, List<Double> target){
		for(int i = 0; i < target.size(); i++){
			result.set(i, result.get(i) - target.get(i));
		}
		return result;
	}
	
	public Double getLastPeriodGrowthNumber(List<Double> listOfAmounts){
		int noOfElements = listOfAmounts.size();
		if(noOfElements <= 1){
			return 0.0;
		}else{
			return listOfAmounts.get(noOfElements - 1) - listOfAmounts.get(noOfElements - 2);
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

	public Double getMomNetProfitGrowtth() {
		return momNetProfitGrowtth;
	}

	public void setMomNetProfitGrowtth(Double momNetProfitGrowtth) {
		this.momNetProfitGrowtth = momNetProfitGrowtth;
	}

	public Double getProfitMargin() {
		return profitMargin;
	}

	public void setProfitMargin(Double profitMargin) {
		this.profitMargin = profitMargin;
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
