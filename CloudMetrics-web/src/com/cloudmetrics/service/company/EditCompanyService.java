package com.cloudmetrics.service.company;

import org.springframework.transaction.annotation.Transactional;

import com.cloudmetrics.common.CommonException;
import com.cloudmetrics.common.MessageCollection;
import com.cloudmetrics.dao.CompanyDao;
import com.cloudmetrics.domain.Company;
import com.cloudmetrics.util.ApplicationConstants;
import com.cloudmetrics.util.EnumCollection.EventStatus;
import com.cloudmetrics.web.AppResponse;

public class EditCompanyService {
	private CompanyDao companyDao;
	
	@Transactional
	public boolean doesTheEmailExist(String email){
		boolean doesTheEmailExist = false;
		
		Company Company = null;
		try {
			Company	= (Company)companyDao.doesTheEmailExist(email);
			if(Company != null){
				doesTheEmailExist = true;
			}else{
				doesTheEmailExist = false;
			}
		} catch (CommonException e) {
			e.printStackTrace();
		}
		
		return doesTheEmailExist;		
	}
		
	/**
	 * This method is called first to create an Company in the system.
	 * This must be entry point.
	 * 
	 * @param firstName
	 * @param lastName
	 * @param email
	 * @param phone
	 * @param isHidden
	 * @param password
	 * @return
	 */
	@Transactional
	public AppResponse<Integer> createCompanyWithMinimumParams(
			String companyName,
			String email, 
			String password){
		AppResponse<Integer> response = new AppResponse<Integer>();
		response.setCode(EventStatus.failure.getValue());
		//Check if the email is not duplicate
		if(doesTheEmailExist(email)){
			response.setDescription(MessageCollection.THIS_EMAIL_ALREADY_EXISTS);
			return response;
		}
		
		Company company = new Company();
		company.setEmail(email);
		company.setCompanyName(companyName);
		company.setPassword(password);
		
		Integer CompanyId = -1;
		try {
			CompanyId = (Integer)companyDao.save(company);
			response.setData(CompanyId);
			response.setCode(EventStatus.success.getValue());
		} catch (CommonException e) {
			e.printStackTrace();
			response.setDescription(MessageCollection.INTERNAL_ERROR_WHILE_ADDING_Company);			
			return response;
		}

		return response;
	}

	/**
	 * This method is called first to create an Company in the system.
	 * This must be entry point.
	 * 
	 * @param firstName
	 * @param lastName
	 * @param email
	 * @param phone
	 * @param isHidden
	 * @param password
	 * @return
	 */
	@Transactional
	public AppResponse<Company> createCompanyFromSyncTool(
			String companyName,
			String email){
		AppResponse<Company> response = new AppResponse<>();
		response.setCode(EventStatus.failure.getValue());
		//Check if the email is not duplicate
		if(doesTheEmailExist(email)){
			response.setDescription(MessageCollection.THIS_EMAIL_ALREADY_EXISTS);
			return response;
		}
		
		Company company = new Company();
		company.setEmail(email);
		company.setCompanyName(companyName);
		company.setIsDirty("1");
		
		Integer CompanyId = -1;
		try {
			CompanyId = (Integer)companyDao.save(company);
			company.setCompanyId(CompanyId);
			response.setData(company);
			response.setCode(EventStatus.success.getValue());
		} catch (CommonException e) {
			e.printStackTrace();
			response.setDescription(MessageCollection.INTERNAL_ERROR_WHILE_ADDING_Company);			
			return response;
		}

		return response;
	}

	/**
	 * Get Company by Id
	 */
	@Transactional
	public AppResponse<Company> getCompanyById(Integer id){
		Company company = null;
		AppResponse<Company> appResponse = new AppResponse<Company>();
		appResponse.setCode(EventStatus.failure.getValue());
		try {
			company	= (Company)companyDao.readById(id);
			//Set the section fill information
			
			appResponse.setCode(EventStatus.success.getValue());
			appResponse.setData(company);
		} catch (CommonException e) {
			e.printStackTrace();
			appResponse.setDescription(MessageCollection.ERROR_ENGINEERS_WILL_FIX_IT);
		}
		
		return appResponse; 
	}
	
	/**
	 * Get Company by email and company name
	 * TODO extend it to include company name after the multipart request is fixed
	 */
	@Transactional
	public Company getCompanyByEmail(String email){
		Company company = null;
		try {
			company	= (Company)companyDao.readByEmail(email);
		} catch (CommonException e) {
			e.printStackTrace();
		}
		
		return company; 
	}
	
	/**
	 * Save Company password. Create an account if Company does not exist.
	 * @param id
	 * @param password
	 * @return
	 */
	@Transactional
	public AppResponse<Integer> saveCompanyPassword(String email, String password){
		Company company = null;
		AppResponse<Integer> appResponse = new AppResponse<Integer>();
		appResponse.setCode(EventStatus.failure.getValue());
		try {
			company	= (Company)companyDao.readByEmail(email);
		} catch (CommonException e) {
			e.printStackTrace();
		}
		//If the account is not created first create it.
		if(company == null){
			appResponse.setDescription(ApplicationConstants.COMPANY_NOT_FOUND);
		}else{		
			company.setPassword(password);
		
			try {
				companyDao.update(company);
				appResponse.setCode(EventStatus.success.getValue());
				appResponse.setData(company.getCompanyId());
				appResponse.setDescription(MessageCollection.PROFILE_SUCCESSFULLY_UPDATED);
			} catch (CommonException e) {
				e.printStackTrace();
				appResponse.setDescription(MessageCollection.PASSWORD_COULD_NOT_BE_SET);
				return appResponse;
			}
		}
			
		return appResponse;
	}
	
	/**
	 * Validate Company & return the Id associated
	 * @param email
	 * @param password
	 * @return
	 */
	@Transactional
	public AppResponse<Integer> isCompanyValid(String email, String password){
		Company company = null;
		AppResponse<Integer> appResponse = new AppResponse<Integer>();
		appResponse.setCode(EventStatus.failure.getValue());
		try {
			company	= (Company)companyDao.validateCompany(email, password);
			if(company != null){
			  appResponse.setData(company.getCompanyId());
				appResponse.setCode(EventStatus.success.getValue());
			}else{
				appResponse.setDescription(MessageCollection.LOGIN_FAILED);
			}
		} catch (CommonException e) {
			e.printStackTrace();
			appResponse.setDescription(MessageCollection.ERROR_ENGINEERS_WILL_FIX_IT);
		}
		
		return appResponse; 
	}

	/**
	 * Set clean or dirty state for a company.
	 * @param id
	 * @param password
	 * @return
	 */
	@Transactional
	public AppResponse<Integer> setCleanOrDirtyState(Integer companyId, boolean isDirty){
		Company company = null;
		AppResponse<Integer> appResponse = new AppResponse<Integer>();
		appResponse.setCode(EventStatus.failure.getValue());
		try {
			company	= (Company)companyDao.readById(companyId);
		} catch (CommonException e) {
			e.printStackTrace();
			return appResponse;
		}	
		
		if(isDirty){
			company.setIsDirty("1");
		}else{
			company.setIsDirty("0");
		}
	
		try {
			companyDao.update(company);
			appResponse.setCode(EventStatus.success.getValue());
			appResponse.setData(company.getCompanyId());
		} catch (CommonException e) {
			e.printStackTrace();
			appResponse.setDescription(MessageCollection.INTERNAL_ERROR);
			return appResponse;
		}			
		return appResponse;
	}
	
	/**
	 * Set clean or dirty state for a company.
	 * @param id
	 * @param password
	 * @return
	 */
	@Transactional
	public AppResponse<Integer> changeCompanyName(Integer companyId, String companyName){
		Company company = null;
		AppResponse<Integer> appResponse = new AppResponse<Integer>();
		appResponse.setCode(EventStatus.failure.getValue());
		try {
			company	= (Company)companyDao.readById(companyId);
		} catch (CommonException e) {
			e.printStackTrace();
			return appResponse;
		}	
		company.setCompanyName(companyName);
		try {
			companyDao.update(company);
			appResponse.setCode(EventStatus.success.getValue());
			appResponse.setData(company.getCompanyId());
		} catch (CommonException e) {
			e.printStackTrace();
			appResponse.setDescription(MessageCollection.INTERNAL_ERROR);
			return appResponse;
		}			
		return appResponse;
	}
	
	/**
	 * 
	 * @param companyId
	 * @param isDirty
	 * @return
	 */	
	@Transactional
	public AppResponse<Boolean> isStateDirty(Integer companyId){
		Company company = null;
		AppResponse<Boolean> appResponse = new AppResponse<>();
		appResponse.setCode(EventStatus.failure.getValue());
		try {
			company	= (Company)companyDao.readById(companyId);
		} catch (CommonException e) {
			e.printStackTrace();
			return appResponse;
		}	
		String state = company.getIsDirty();
		if("0".equals(state)){
			appResponse.setData(false);
		}else{
			appResponse.setData(true);
		}
		appResponse.setCode(EventStatus.success.getValue());
		return appResponse;
	}

	public CompanyDao getCompanyDao() {
		return companyDao;
	}

	public void setCompanyDao(CompanyDao CompanyDao) {
		this.companyDao = CompanyDao;
	}
}
