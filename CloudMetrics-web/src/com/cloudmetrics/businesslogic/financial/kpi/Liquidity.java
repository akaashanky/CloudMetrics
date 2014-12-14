package com.cloudmetrics.businesslogic.financial.kpi;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.cloudmetrics.businesslogic.financial.BaseFinancialInfo;
import com.cloudmetrics.businesslogic.financial.FinancialConstants;
import com.cloudmetrics.businesslogic.financial.FinancialUtils;
import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.businesslogic.tally.GenericGroupOrLedger;
import com.cloudmetrics.domain.Company;

public class Liquidity extends BaseFinancialInfo{
	Double cashAndCashEq;
	String formattedcashAndCashEq;
	Double momCashAndCashEqGrowth;
	List<Double> monthlyCashAndCashEqList = new ArrayList<Double>();
	
	Double currentRatio;
	String formattedCurrentRatio;
	Double momCurrentRatioGrowth;
	List<Double> monthlyCurrentRatioList = new ArrayList<Double>();
	
	Double bankAccount;
	String formattedBankAccount;
	Double momBankAccountGrowth;
	List<Double> monthlyBankAccountList = new ArrayList<Double>();
	
	/**
	 * @param financialData
	 */
	public Liquidity(Company company,CompanyFinancialData financialData){
		super(company);
        GenericGroupOrLedger currentAssetsGroup = financialData.getGroupOrLedgerByReservedName(FinancialConstants.ACC_CURRENT_ASSETS);
        GenericGroupOrLedger currentLiabilitiesGroup = financialData.getGroupOrLedgerByReservedName(FinancialConstants.ACC_CURRENT_LIABILITIES);
        GenericGroupOrLedger cashInHandGroup = financialData.getGroupOrLedgerByReservedName(FinancialConstants.ACC_CASH_IN_HAND);
        GenericGroupOrLedger bankAccountsGroup = financialData.getGroupOrLedgerByReservedName(FinancialConstants.ACC_BANK_ACCOUNTS);
        GenericGroupOrLedger sundryDebtorsGroup = financialData.getGroupOrLedgerByReservedName(FinancialConstants.ACC_SUNDRY_DEBTORS);
        
        //Now calculate the actual stuff
        this.cashAndCashEq = 0.0 - cashInHandGroup.getAnnualClosingBalance(); //The assets are negative. So we must negate them.
        FinancialUtils.subtractOneAmountListFromAnother(this.monthlyCashAndCashEqList, cashInHandGroup.getMonthlyClosingBalance());
        this.momCashAndCashEqGrowth = FinancialUtils.getLastPeriodGrowthNumber(this.monthlyCashAndCashEqList);
        
        if(currentLiabilitiesGroup.getAnnualClosingBalance() != 0)
        	this.currentRatio = currentAssetsGroup.getAnnualClosingBalance()/currentLiabilitiesGroup.getAnnualClosingBalance();
        else
        	this.currentRatio = 0.0;
        this.currentRatio = this.currentRatio * -1.0;
        this.monthlyCurrentRatioList = FinancialUtils.DivideOneAmountListByAnother(currentAssetsGroup.getMonthlyClosingBalance(), currentLiabilitiesGroup.getMonthlyClosingBalance());
        List<Double> dummyList = new ArrayList<>();
        FinancialUtils.subtractOneAmountListFromAnother(dummyList, this.monthlyCurrentRatioList);
        this.monthlyCurrentRatioList = dummyList;
        this.momCurrentRatioGrowth = FinancialUtils.getLastPeriodGrowthNumber(this.monthlyCurrentRatioList);
        
        this.bankAccount = bankAccountsGroup.getAnnualClosingBalance() * -1.0;
        FinancialUtils.subtractOneAmountListFromAnother(this.monthlyBankAccountList, bankAccountsGroup.getMonthlyClosingBalance());
        this.momBankAccountGrowth = FinancialUtils.getLastPeriodGrowthNumber(this.monthlyBankAccountList);
        
        this.formattedcashAndCashEq = FinancialUtils.getFormattedAmount(this.cashAndCashEq);
        this.formattedCurrentRatio = FinancialUtils.getFormattedAmount(this.currentRatio) + " : 1";
        this.formattedBankAccount = FinancialUtils.getFormattedAmount(this.bankAccount);
        
        super.setFinancialYear(financialData.getFinancialYear());
	}

	public Double getCashAndCashEq() {
		return cashAndCashEq;
	}

	public void setCashAndCashEq(Double cashAndCashEq) {
		this.cashAndCashEq = cashAndCashEq;
	}

	public String getFormattedcashAndCashEq() {
		return formattedcashAndCashEq;
	}

	public void setFormattedcashAndCashEq(String formattedcashAndCashEq) {
		this.formattedcashAndCashEq = formattedcashAndCashEq;
	}

	public Double getMomCashAndCashEqGrowth() {
		return momCashAndCashEqGrowth;
	}

	public void setMomCashAndCashEqGrowth(Double momCashAndCashEqGrowth) {
		this.momCashAndCashEqGrowth = momCashAndCashEqGrowth;
	}

	public List<Double> getMonthlyCashAndCashEqList() {
		return monthlyCashAndCashEqList;
	}

	public void setMonthlyCashAndCashEqList(List<Double> monthlyCashAndCashEqList) {
		this.monthlyCashAndCashEqList = monthlyCashAndCashEqList;
	}

	public Double getCurrentRatio() {
		return currentRatio;
	}

	public void setCurrentRatio(Double currentRatio) {
		this.currentRatio = currentRatio;
	}

	public String getFormattedCurrentRatio() {
		return formattedCurrentRatio;
	}

	public void setFormattedCurrentRatio(String formattedCurrentRatio) {
		this.formattedCurrentRatio = formattedCurrentRatio;
	}

	public Double getMomCurrentRatioGrowth() {
		return momCurrentRatioGrowth;
	}

	public void setMomCurrentRatioGrowth(Double momCurrentRatioGrowth) {
		this.momCurrentRatioGrowth = momCurrentRatioGrowth;
	}

	public List<Double> getMonthlyCurrentRatioList() {
		return monthlyCurrentRatioList;
	}

	public void setMonthlyCurrentRatioList(List<Double> monthlyCurrentRatioList) {
		this.monthlyCurrentRatioList = monthlyCurrentRatioList;
	}

	public Double getBankAccount() {
		return bankAccount;
	}

	public void setBankAccount(Double bankAccount) {
		this.bankAccount = bankAccount;
	}

	public String getFormattedBankAccount() {
		return formattedBankAccount;
	}

	public void setFormattedBankAccount(String formattedBankAccount) {
		this.formattedBankAccount = formattedBankAccount;
	}

	public Double getMomBankAccountGrowth() {
		return momBankAccountGrowth;
	}

	public void setMomBankAccountGrowth(Double momBankAccountGrowth) {
		this.momBankAccountGrowth = momBankAccountGrowth;
	}

	public List<Double> getMonthlyBankAccountList() {
		return monthlyBankAccountList;
	}

	public void setMonthlyBankAccountList(List<Double> monthlyBankAccountList) {
		this.monthlyBankAccountList = monthlyBankAccountList;
	}	
}
