package com.cloudmetrics.common.test;

import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPathExpressionException;

import org.xml.sax.SAXException;

import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.service.businesslogic.finance.FinancialDataPopulationService;
import com.cloudmetrics.service.businesslogic.finance.XMLToModelObjectConverter;

public class MainTest {
  public static void main(String g[]){
	  try {
		CompanyFinancialData financialData = new CompanyFinancialData();
		XMLToModelObjectConverter.construct(financialData, "d:/__junk__/LedgerMaster.xml");
		FinancialDataPopulationService.populate(financialData, "d:/__junk__/tb2_result.csv");
		System.out.println(financialData);
	} catch (XPathExpressionException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	} catch (ParserConfigurationException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	} catch (SAXException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
  }
}
