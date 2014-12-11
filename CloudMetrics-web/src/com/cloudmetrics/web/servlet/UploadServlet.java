package com.cloudmetrics.web.servlet;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.List;

import javax.annotation.Resource;
import javax.imageio.ImageIO;
import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.FileUploadException;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.imgscalr.Scalr;
import org.json.JSONArray;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.context.support.SpringBeanAutowiringSupport;

import com.cloudmetrics.common.MessageCollection;
import com.cloudmetrics.domain.Company;
import com.cloudmetrics.service.company.EditCompanyService;
import com.cloudmetrics.util.CollectionOfUtilityMethods;
import com.cloudmetrics.util.UnZip;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.util.EnvManager;
import com.cloudmetrics.web.AppResponse;

/**
 * This class handles the image upload for the time.
 * But can be extended to handle upload of other files as well.
 * @author mpanda
 *
 */
public class UploadServlet extends HttpServlet {

	private static final long serialVersionUID = 14123L;
	private Log _log = LogFactory.getLog(this.getClass().getCanonicalName());
	
	@Autowired
	private EditCompanyService editCompanyService;
	
	public void init(ServletConfig config) {
		  try {
			super.init(config);
			SpringBeanAutowiringSupport.processInjectionBasedOnServletContext(this,config.getServletContext());
		  } catch (ServletException e) {
			_log.fatal("***init failed***", e.getRootCause());
			e.printStackTrace();
		  }
		}

	@Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
            PrintWriter writer = response.getWriter();
            writer.write("call POST with multipart form data");
    }
    
    
    @SuppressWarnings("unchecked")
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    	if (!ServletFileUpload.isMultipartContent(request)) {
            throw new IllegalArgumentException("Request is not multipart, please 'multipart/form-data' enctype for your form.");
        }

        ServletFileUpload uploadHandler = new ServletFileUpload(new DiskFileItemFactory());
        PrintWriter writer = response.getWriter();
        response.setContentType("application/json");
        JSONArray json = new JSONArray();
        try {
            List<FileItem> items = uploadHandler.parseRequest(request);
      	           	
            for (FileItem item : items) {
                if (!item.isFormField()) { //It will enter into this loop just once!
            	    String finDataStorageLocation = EnvManager.getFinancialDataStorageLocation();
            	    File storageDir = new File(finDataStorageLocation);
            	    //If storage dir does not exist, create it.
            	    if(!storageDir.exists()){
            	    	storageDir.mkdir();
            	    }
            	    //get the name and other data of the customers from the file name.
                	String fileName = item.getName();
                	String[] temp = fileName.split("__",-1);
                	String emailId = temp[temp.length - 2];
                	String companyName = temp[0];
                	Company company = editCompanyService.getCompanyByEmail(emailId);
                	//If company does not exist, create it
                	if(company == null){
                		AppResponse<Company> companyCreationResponse = editCompanyService.createCompanyFromSyncTool(companyName, emailId);
                		if(companyCreationResponse.getCode() == EventStatus.failure.getValue()){
                			log("Company creation was not successful");
                		}else{
                			company = companyCreationResponse.getData();
                		}
                	} 
                	//At this point we have a "Company"
                    File zipFile = new File(finDataStorageLocation, item.getName());
                    item.write(zipFile);    
                    //Now unzip it
                    UnZip.unZipSyncData(zipFile.getAbsolutePath(), finDataStorageLocation, company.getCompanyId());
                    editCompanyService.setCleanOrDirtyState(company.getCompanyId(), true);
                }
          } 
        } catch (FileUploadException e) {
                throw new RuntimeException(e);
        } catch (Exception e) {
                throw new RuntimeException(e);
        } finally {
            writer.write(json.toString());
            writer.close();
        }

    }
   
}