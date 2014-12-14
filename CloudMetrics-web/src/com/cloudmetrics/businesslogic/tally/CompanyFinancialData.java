package com.cloudmetrics.businesslogic.tally;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

public class CompanyFinancialData {
	Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> twoLevelGroupAndLedgerMap = new HashMap<>();
	Map<String, String> reservedNameToActualNameMap = new HashMap<>();
	List<Calendar> usedMonths = new ArrayList<>();
	
	/**
	 * 
	 * @param reservedName
	 * @return
	 */
	public GenericGroupOrLedger getGroupOrLedgerByReservedName(String reservedName){
		GenericGroupOrLedger groupOrLedger = null;
		//What is its actual name?
		String actualName = reservedNameToActualNameMap.get(reservedName); 
		if(actualName != null){
			GenericGroupOrLedger dummyGroupOrLedger = new GenericGroupOrLedger(actualName); 
			if(twoLevelGroupAndLedgerMap.containsKey(dummyGroupOrLedger)){ //TODO: we just do it by "name" now which is a shame
				for(GenericGroupOrLedger localGroupOrLedger : twoLevelGroupAndLedgerMap.keySet()){
					if(dummyGroupOrLedger.equals(localGroupOrLedger)){
						groupOrLedger = localGroupOrLedger;
					}
				}
			}else{//Find in the lists contained in the keys. tedious :(
				boolean found = false;
				for(GenericGroupOrLedger localGroupOrLedger : twoLevelGroupAndLedgerMap.keySet()){
					if(found == true) break;
					List<GenericGroupOrLedger> localGroupOfLedgerListForAParent = twoLevelGroupAndLedgerMap.get(localGroupOrLedger);
					for(GenericGroupOrLedger subLocalGroupOrLedger : localGroupOfLedgerListForAParent){
						if(dummyGroupOrLedger.equals(subLocalGroupOrLedger)){
							groupOrLedger = subLocalGroupOrLedger;
							found = true;
							break;
						}
					}
				}
			}
		}
		return groupOrLedger;
	}
	
	public String getFinancialYear(){
		String finYear = "";
		if(usedMonths.size() > 0){
			Calendar cal = usedMonths.get(0);
			finYear = "FY " + (cal.get(Calendar.YEAR) + 1);
		}else{
			Calendar cal = Calendar.getInstance();
			finYear = "FY " + (cal.get(Calendar.YEAR) + 1);
		}
		return finYear;
	}
	
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
