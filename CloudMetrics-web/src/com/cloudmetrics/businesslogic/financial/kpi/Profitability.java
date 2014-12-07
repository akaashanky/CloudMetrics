package com.cloudmetrics.businesslogic.financial.kpi;

import java.util.ArrayList;
import java.util.List;

public class Profitability{
	String revenue;
	String grossProfit;
	String netProfit;
	String profitMargin;
	
	List<String> monthlyRevenue = new ArrayList<String>();
	List<String> monthlyGrossProfit = new ArrayList<String>();
	List<String> monthlyNetProfit = new ArrayList<String>();
	List<String> monthlyProfitMargin = new ArrayList<String>();
	
	public Profitability(boolean test){
		revenue = "20,345,677";
		grossProfit = "9,980,879";
		netProfit = "3,987,234";
		profitMargin = "13.23%";
		
		monthlyRevenue.add("3,456,523");
		monthlyRevenue.add("2,873,000");
		monthlyRevenue.add("1,456,523");
		monthlyRevenue.add("3,956,523");
		monthlyRevenue.add("3,423,456");
	}

	public String getRevenue() {
		return revenue;
	}

	public void setRevenue(String revenue) {
		this.revenue = revenue;
	}

	public String getGrossProfit() {
		return grossProfit;
	}

	public void setGrossProfit(String grossProfit) {
		this.grossProfit = grossProfit;
	}

	public String getNetProfit() {
		return netProfit;
	}

	public void setNetProfit(String netProfit) {
		this.netProfit = netProfit;
	}

	public String getProfitMargin() {
		return profitMargin;
	}

	public void setProfitMargin(String profitMargin) {
		this.profitMargin = profitMargin;
	}

	public List<String> getMonthlyRevenue() {
		return monthlyRevenue;
	}

	public void setMonthlyRevenue(List<String> monthlyRevenue) {
		this.monthlyRevenue = monthlyRevenue;
	}

	public List<String> getMonthlyGrossProfit() {
		return monthlyGrossProfit;
	}

	public void setMonthlyGrossProfit(List<String> monthlyGrossProfit) {
		this.monthlyGrossProfit = monthlyGrossProfit;
	}

	public List<String> getMonthlyNetProfit() {
		return monthlyNetProfit;
	}

	public void setMonthlyNetProfit(List<String> monthlyNetProfit) {
		this.monthlyNetProfit = monthlyNetProfit;
	}

	public List<String> getMonthlyProfitMargin() {
		return monthlyProfitMargin;
	}

	public void setMonthlyProfitMargin(List<String> monthlyProfitMargin) {
		this.monthlyProfitMargin = monthlyProfitMargin;
	}

}
