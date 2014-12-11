package com.cloudmetrics.util;

import java.io.File;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpSession;

public class CollectionOfUtilityMethods {
    public static String getParamValIfNotNullElseGetDefaultVal(String paramName, String defaultValue, HttpServletRequest req){
    	String paramVal = req.getParameter(paramName);
    	if(paramVal == null || "".equals(paramVal.trim())){
    		paramVal = defaultValue;
    	}
    	return paramVal;
    }
    
    public static String[] getFirstNameAndLastNameArrayFromFullName(String fullName){
		String[] firstnameAndLastnameArr = fullName.split(" ");
		if(firstnameAndLastnameArr.length == 2){
			return firstnameAndLastnameArr;
		}else if(firstnameAndLastnameArr.length == 1){
			String firstName = firstnameAndLastnameArr[0];
			firstnameAndLastnameArr = new String[2];
			firstnameAndLastnameArr[0] = firstName; 
			firstnameAndLastnameArr[1] = "";
			return firstnameAndLastnameArr;
		}else if(firstnameAndLastnameArr.length > 2){
		    String firstName = firstnameAndLastnameArr[0];
		    String lastName = firstnameAndLastnameArr[2];
		    
		    firstnameAndLastnameArr = new String[2];
			firstnameAndLastnameArr[0] = firstName; 
			firstnameAndLastnameArr[1] = lastName;
			return firstnameAndLastnameArr;
		}else{
			firstnameAndLastnameArr = new String[2];
		    firstnameAndLastnameArr[0] = "";
		    firstnameAndLastnameArr[1] = "";
		    return firstnameAndLastnameArr;
		}
    }
    
    public static String getMimeType(File file) {
        String mimetype = "";
        if (file.exists()) {
            if (getSuffix(file.getName()).equalsIgnoreCase("png")) {
                mimetype = "image/png";
            }else if(getSuffix(file.getName()).equalsIgnoreCase("jpg")){
                mimetype = "image/jpg";
            }else if(getSuffix(file.getName()).equalsIgnoreCase("jpeg")){
                mimetype = "image/jpeg";
            }else if(getSuffix(file.getName()).equalsIgnoreCase("gif")){
                mimetype = "image/gif";
            }else {
                javax.activation.MimetypesFileTypeMap mtMap = new javax.activation.MimetypesFileTypeMap();
                mimetype  = mtMap.getContentType(file);
            }
        }
        return mimetype;
    }



    public static String getSuffix(String filename) {
        String suffix = "";
        int pos = filename.lastIndexOf('.');
        if (pos > 0 && pos < filename.length() - 1) {
            suffix = filename.substring(pos + 1);
        }
        return suffix;
    }
    
    public static Integer parseToNumber(String number){
    	Integer i = null;
    	try{
    		i = Integer.parseInt(number);
    	}catch(NumberFormatException e){
    		e.printStackTrace();    		
    	}
    	return i;
    }
    
    /**
     * Helper function to check for the session
     * @param req
     * @return
     */
    
    public static boolean isReqInSession(HttpServletRequest req){
    	boolean isReqInSession = false;
    	HttpSession session = req.getSession(false);
    	if(session == null){
    		return isReqInSession;
    	}
    	Integer companyId = (Integer)session.getAttribute(ApplicationConstants.COMPANY_ID);
    	if(companyId == null){
    		return isReqInSession;
    	} 
    	isReqInSession = true;
    	return isReqInSession;
    }
}
