package com.cloudmetrics.businesslogic.financial;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class FinancialUtils {
	/**
	 * @author Indicoder
	 * @param result
	 * @param target
	 * @return
	 */
	public static List<Double> addOneAmountListToAnother(List<Double> result, List<Double> target){
		boolean isThisACopyTask = false; //If result and target are not of same size, simply copy target data to result
		if(result.size() != target.size()){
			isThisACopyTask = true;
			result.clear();
		}
		for(int i = 0; i < target.size(); i++){
			if(isThisACopyTask == false){
				result.set(i, result.get(i) + target.get(i));	
			}else{
				result.add(target.get(i));
			}
		}
		return result;
	}
	
	/**
	 * @author Indicoder
	 * @param result
	 * @param target
	 * @return
	 */
	public static List<Double> subtractOneAmountListFromAnother(List<Double> result, List<Double> target){
		boolean isThisACopyTask = false; //If result and target are not of same size, simply copy target data to result
		if(result.size() != target.size()){
			isThisACopyTask = true;
			result.clear();
		}
		for(int i = 0; i < target.size(); i++){
			if(isThisACopyTask == false){
				result.set(i, result.get(i) - target.get(i));	
			}else{
				result.add(target.get(i) * -1.0 );
			}
		}
		return result;
	}
	
	/**
	 * @author Indicoder
	 * @param result
	 * @param target
	 * returns a new list, Assumes that both list have equal no of elements.
	 * Else return null
	 * @return 
	 */
	public static List<Double> DivideOneAmountListByAnother(List<Double> upper, List<Double> lower){
		List<Double> result = null;
		if(upper.size() != lower.size()){
			return result;
		}		
		result = new ArrayList<Double>();
		for(int i = 0; i < upper.size(); i++){
			if(lower.get(i) != 0){
				result.add(upper.get(i)/lower.get(i));
			}else{
				result.add(0.0);
			}
		}
		return result;
	}
	
	/**
	 * 
	 * @param listOfAmounts
	 * @return
	 */
	public static Double getLastPeriodGrowthNumber(List<Double> listOfAmounts){
		int lastPeriodToBeConsidered = 0;
		for(int i = listOfAmounts.size() - 1; i >= 0; i--){
			if(listOfAmounts.get(i) != 0.0){
				lastPeriodToBeConsidered = i;
				break;
			}
		}
		if(lastPeriodToBeConsidered <= 0){
			return 0.0;
		}else{
			return (listOfAmounts.get(lastPeriodToBeConsidered) - listOfAmounts.get(lastPeriodToBeConsidered - 1))/listOfAmounts.get(lastPeriodToBeConsidered - 1)*100;
		}
	}
	
	public static String getFormattedAmount(Double amount){
		NumberFormat formatter = NumberFormat.getCurrencyInstance(new Locale("en", "in"));
		DecimalFormatSymbols decimalFormatSymbols = ((DecimalFormat) formatter).getDecimalFormatSymbols();
		decimalFormatSymbols.setCurrencySymbol("");
		((DecimalFormat) formatter).setDecimalFormatSymbols(decimalFormatSymbols);
		formatter.setMaximumFractionDigits(0);
		return formatter.format(amount);
	}
}
