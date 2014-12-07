package com.cloudmetrics.service;

import java.util.Properties;

import org.springframework.beans.factory.annotation.Autowired;


public class BaseService {
	Properties applicationProperties = null;
	
	public Properties getApplicationProperties() {
		return applicationProperties;
	}

	public void setApplicationProperties(Properties applicationProperties) {
		this.applicationProperties = applicationProperties;
	}

	public String getApplicationProperty(String propertyName) {
		
		if(applicationProperties.containsKey(propertyName)) {
			return (String)applicationProperties.getProperty(propertyName);
		}
		
		return null;
	}
	
}
