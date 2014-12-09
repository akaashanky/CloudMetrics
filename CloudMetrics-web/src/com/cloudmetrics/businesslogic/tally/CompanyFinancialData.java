package com.cloudmetrics.businesslogic.tally;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CompanyFinancialData {
	Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> twoLevelGroupAndLedgerMap = new HashMap<>();
	Map<String, String> reservedNameToActualNameMap = new HashMap<>();
	List<Calendar> usedMonths = new ArrayList<>();
	
	public Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> getTwoLevelGroupAndLedgerMap() {
		return twoLevelGroupAndLedgerMap;
	}
	public void setTwoLevelGroupAndLedgerMap(
			Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> twoLevelGroupAndLedgerMap) {
		this.twoLevelGroupAndLedgerMap = twoLevelGroupAndLedgerMap;
	}
	public Map<String, String> getReservedNameToActualNameMap() {
		return reservedNameToActualNameMap;
	}
	public void setReservedNameToActualNameMap(
			Map<String, String> reservedNameToActualNameMap) {
		this.reservedNameToActualNameMap = reservedNameToActualNameMap;
	}
	public List<Calendar> getUsedMonths() {
		return usedMonths;
	}
	public void setUsedMonths(List<Calendar> usedMonths) {
		this.usedMonths = usedMonths;
	}	
	
}
