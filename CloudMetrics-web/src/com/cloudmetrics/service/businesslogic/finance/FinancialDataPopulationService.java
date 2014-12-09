package com.cloudmetrics.service.businesslogic.finance;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.businesslogic.tally.GenericGroupOrLedger;

public class FinancialDataPopulationService {
	public static void populate(CompanyFinancialData companyFinancialData, String financialCsvDataFile){
		BufferedReader br = null;
		String line = "";
		if(companyFinancialData == null)
			return;
		List<Calendar> usedMonths = companyFinancialData.getUsedMonths();
		usedMonths.clear(); //Clean the list as we are going to repopulate
		Map<String, List<String>> groupOrLedgerNameToClosingBalanceMap = new HashMap<String, List<String>>();
		try{
			br = new BufferedReader(new FileReader(financialCsvDataFile));
			boolean isFirstLine = true;
			while ((line = br.readLine()) != null) {			 
			    String[] cells = line.split(",", -1);
			    if(isFirstLine){//First line has all the months.
			    	for(int i = 1; i < cells.length - 1; i++){//First cell is blank. So skip it
			    		Calendar cal = processDateString(cells[i]);
			    		usedMonths.add(cal);
			    	}
			    	isFirstLine = false;
			    }else{
			    	String groupOrLedgerName = getLedgerOrGroupFromCsvCell(cells[0]);
			    	List<String> closingBalanceList = new ArrayList<>();
			    	for(int i = 1; i < cells.length - 1; i++){
			    		closingBalanceList.add(cells[i]);
			    	}
			    	groupOrLedgerNameToClosingBalanceMap.put(groupOrLedgerName, closingBalanceList);
			    }	
		    }
		}catch(FileNotFoundException fnfe) {
			fnfe.printStackTrace();
		} catch (IOException ioe) {
			ioe.printStackTrace();
		} finally {		
			if (br != null) {
				try {
					br.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
		//Now finally populate the CompanyFinancialData
		Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> twoLevelGroupAndLedgerMap = companyFinancialData.getTwoLevelGroupAndLedgerMap();
		for (GenericGroupOrLedger primaryGroupOrLedger : twoLevelGroupAndLedgerMap.keySet()) {
			//populate data in the key			
			if(groupOrLedgerNameToClosingBalanceMap.get(primaryGroupOrLedger.getName()) != null){
				primaryGroupOrLedger.setMonthlyClosingBalance(groupOrLedgerNameToClosingBalanceMap.get(primaryGroupOrLedger.getName()));
				//Traverse thru the children list and populate them as well
				List<GenericGroupOrLedger> secondLevelChildrenList = twoLevelGroupAndLedgerMap.get(primaryGroupOrLedger);
				for(GenericGroupOrLedger groupOrLedger : secondLevelChildrenList){
					if(groupOrLedgerNameToClosingBalanceMap.get(groupOrLedger.getName()) != null){
						groupOrLedger.setMonthlyClosingBalance(groupOrLedgerNameToClosingBalanceMap.get(groupOrLedger.getName()));
					}else{
						groupOrLedger.setMonthlyClosingBalance(null);
					}
				}
			}else{//Remove the amount we already have for this node as well as its children
				primaryGroupOrLedger.setMonthlyClosingBalance(null);
				List<GenericGroupOrLedger> secondLevelChildrenList = twoLevelGroupAndLedgerMap.get(primaryGroupOrLedger);
				for(GenericGroupOrLedger groupOrLedger : secondLevelChildrenList){
					groupOrLedger.setMonthlyClosingBalance(null);
				}
				//TODO:Send signal down that we need a schema update
			}
		}
	}
	
	public static Calendar processDateString(String dateString){
		//We have to parse "1-Apr-2013 to 30-Apr-2013" to a Calendar month
		Calendar cal = null;
		try{
			String temp1 = (dateString.split("to"))[0];
			cal = Calendar.getInstance();
			SimpleDateFormat sdf = new SimpleDateFormat("dd-MMM-yyyy");
			String temp2 = ((temp1.trim().split("\"", -1))[1]).trim();
			cal.setTime(sdf.parse(temp2));
		}catch(ParseException pe){
			pe.printStackTrace();
		}
		return cal;
	}
	
	public static String getLedgerOrGroupFromCsvCell(String cell){
		return (cell.split("\"", -1))[1];
	}
	
	public static void main(String[] args) {
		populate(null, "d:/__junk__/tb2_result.csv");
	}
}
