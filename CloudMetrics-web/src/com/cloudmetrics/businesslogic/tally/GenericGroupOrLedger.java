package com.cloudmetrics.businesslogic.tally;

import java.util.List;

public class GenericGroupOrLedger {
	GenericGroupOrLedger parent;
	String name;
	String reservedName;
	
	List<Double> monthlyClosingBalance;
	List<Double> quarterlyClosingBalance;
	Double annualClosingBalance;
	
	@Override
	public String toString() {
		return name;
	}
	
	@Override
	public boolean equals(Object obj) {
		if(obj == null) 
			return false;
		if(obj == this)
			return true;
		if(!(obj instanceof GenericGroupOrLedger))
			return false;
		GenericGroupOrLedger o = (GenericGroupOrLedger)obj;
		return (this.name.equals(o.name));
	}
	
	@Override
	public int hashCode() {
		return this.name.hashCode();
	}	

	public GenericGroupOrLedger getParent() {
		return parent;
	}

	public void setParent(GenericGroupOrLedger parent) {
		this.parent = parent;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getReservedName() {
		return reservedName;
	}

	public void setReservedName(String reservedName) {
		this.reservedName = reservedName;
	}

	public List<Double> getMonthlyClosingBalance() {
		return monthlyClosingBalance;
	}

	public void setMonthlyClosingBalance(List<Double> monthlyClosingBalance) {
		this.monthlyClosingBalance = monthlyClosingBalance;
	}

	public List<Double> getQuarterlyClosingBalance() {
		return quarterlyClosingBalance;
	}

	public void setQuarterlyClosingBalance(List<Double> quarterlyClosingBalance) {
		this.quarterlyClosingBalance = quarterlyClosingBalance;
	}

	public Double getAnnualClosingBalance() {
		return annualClosingBalance;
	}

	public void setAnnualClosingBalance(Double annualClosingBalance) {
		this.annualClosingBalance = annualClosingBalance;
	}
}
