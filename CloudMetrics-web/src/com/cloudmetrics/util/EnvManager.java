package com.cloudmetrics.util;

import org.apache.commons.configuration.Configuration;
import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.cloudmetrics.util.EnumCollection.Environment;

public class EnvManager {

	private static Configuration appConfig_ = null;
	private static Log logger_ = LogFactory.getLog(EnvManager.class);

	/*
	 * Support both dev and production path for config properties
	 */
	static {
		try {
			appConfig_ = new PropertiesConfiguration(ApplicationConstants.CONFIG_FILE_NAME);
			logger_.debug("Loaded app properties from " + ApplicationConstants.CONFIG_FILE_NAME);
		} catch (ConfigurationException e) {			
				logger_.fatal("Exception loading " + ApplicationConstants.CONFIG_FILE_NAME, e);
				throw new RuntimeException(
						"Failed to load " + ApplicationConstants.CONFIG_FILE_NAME, e);			
		}
	}
	
	public static Environment getEnvironment(){
		 String env = System.getenv("ENV");
	        if ( env != null && env.equalsIgnoreCase("dev")){
	        	return Environment.Dev;
	        }else{
	        	return Environment.Prod;
	        }
	}	
	
	public static String getFinancialDataStorageLocation(){
		if(getEnvironment() == Environment.Dev){
			return appConfig_.getString(ApplicationConstants.UPLOADED_FINDATA_LOCATION_DEV);
		}else{
			return appConfig_.getString(ApplicationConstants.UPLOADED_FINDATA_LOCATION_PROD);
		}
	}
	public EnvManager(){}
}
