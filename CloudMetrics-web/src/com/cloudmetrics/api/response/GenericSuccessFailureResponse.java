package com.cloudmetrics.api.response;

public class GenericSuccessFailureResponse implements GenericResponse{
	private boolean success = false;
    private String message = null;
    
    @Override
	public boolean isSuccess() {
		return success;
	}

    @Override
	public void setSuccess(boolean success) {
		this.success = success;
	}

    @Override
	public String getMessage() {
		return message;
	}

    @Override
	public void setMessage(String message) {
		this.message = message;
	}
	
	
}
