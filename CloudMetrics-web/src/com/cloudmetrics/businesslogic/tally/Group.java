package com.cloudmetrics.businesslogic.tally;

public class Group extends GenericGroupOrLedger{
	boolean isRevenue;
	boolean affectsGrossProfit;
	boolean isDeemedPositive;
	boolean trackNegativeBalances;
	
	public Group(){}
	
	@Override
	public String toString() {		
		return super.toString() + "(G)";
	}
	
	public boolean isRevenue() {
		return isRevenue;
	}
	public void setRevenue(boolean isRevenue) {
		this.isRevenue = isRevenue;
	}
	public boolean isAffectsGrossProfit() {
		return affectsGrossProfit;
	}
	public void setAffectsGrossProfit(boolean affectsGrossProfit) {
		this.affectsGrossProfit = affectsGrossProfit;
	}
	public boolean isDeemedPositive() {
		return isDeemedPositive;
	}
	public void setDeemedPositive(boolean isDeemedPositive) {
		this.isDeemedPositive = isDeemedPositive;
	}
	public boolean isTrackNegativeBalances() {
		return trackNegativeBalances;
	}
	public void setTrackNegativeBalances(boolean trackNegativeBalances) {
		this.trackNegativeBalances = trackNegativeBalances;
	}
}
