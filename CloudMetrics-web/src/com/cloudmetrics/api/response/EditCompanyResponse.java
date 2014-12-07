package com.cloudmetrics.api.response;

public class EditCompanyResponse extends GenericSuccessFailureResponse{
	private int generatedCompanyId = -1;

	public int getGeneratedCompanyId() {
		return generatedCompanyId;
	}

	public void setGeneratedCompanyId(int generatedCompanyId) {
		this.generatedCompanyId = generatedCompanyId;
	}
	
}