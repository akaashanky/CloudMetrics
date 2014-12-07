package com.cloudmetrics.web;

public class AppResponse<T> {

	private int code;
	private T data;
	private String description;
	
	public AppResponse() {}
	
	public AppResponse(int code, T data, String description) {
		this.code = code;
		this.data = data;
		this.description = description;
	}

	public int getCode() {
		return code;
	}

	public void setCode(int code) {
		this.code = code;
	}

	public T getData() {
		return data;
	}

	public void setData(T data) {
		this.data = data;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}
}
