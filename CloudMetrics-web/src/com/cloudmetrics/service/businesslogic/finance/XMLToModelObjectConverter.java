package com.cloudmetrics.service.businesslogic.finance;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import com.cloudmetrics.businesslogic.tally.CompanyFinancialData;
import com.cloudmetrics.businesslogic.tally.GenericGroupOrLedger;
import com.cloudmetrics.businesslogic.tally.Group;
import com.cloudmetrics.businesslogic.tally.Ledger;

public class XMLToModelObjectConverter {
	  static final String TALLYMESSAGE = "TALLYMESSAGE";
	  static final String GROUP = "GROUP";
	  static final String NAME = "NAME";
	  static final String RESERVEDNAME = "RESERVEDNAME";
	  static final String PARENT = "PARENT";
	  static final String ISREVENUE = "ISREVENUE";
	  static final String AFFECTSGROSSPROFIT = "AFFECTSGROSSPROFIT";
	  static final String ISDEEMEDPOSITIVE = "ISDEEMEDPOSITIVE";
	  static final String TRACKNEGATIVEBALANCES = "TRACKNEGATIVEBALANCES";
	  
	  static final String LEDGER = "LEDGER";
	  
	  public static void construct(CompanyFinancialData financialData, String masterXMLStructureFile) throws ParserConfigurationException, SAXException,
      IOException, XPathExpressionException {
		  Map<String, String> reservedNameToActualNameMap = financialData.getReservedNameToActualNameMap();
		  Map<GenericGroupOrLedger, List<GenericGroupOrLedger>> twoLevelGroupAndLedgerMap = financialData.getTwoLevelGroupAndLedgerMap();
		  reservedNameToActualNameMap.clear();
		  twoLevelGroupAndLedgerMap.clear();
		  
		  DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		    factory.setNamespaceAware(true);
		    DocumentBuilder builder;
		    Document doc = null;
		    XPathExpression expr = null;
		    builder = factory.newDocumentBuilder();
		    doc = builder.parse(masterXMLStructureFile);		    
		    XPathFactory xFactory = XPathFactory.newInstance();
		    XPath xpath = xFactory.newXPath();
		    
		    //First get the primary groups and ledgers.
		    expr = xpath.compile("//GROUP[PARENT = \"\"]");
		    Object resultForGroup = expr.evaluate(doc, XPathConstants.NODESET);

     	    NodeList nodes = (NodeList) resultForGroup;
		    for (int i=0; i<nodes.getLength();i++){		
		    	Group group = new Group();
		    	Node groupNode = nodes.item(i);
		    	group.setParent(null);
		    	populateGroupOrLedgerFromNode(groupNode, group);
		    	
		    	if(!"".equals(group.getReservedName())){
		    	  reservedNameToActualNameMap.put(group.getReservedName(), group.getName());
		    	}
		    	twoLevelGroupAndLedgerMap.put(group, new ArrayList<GenericGroupOrLedger>());
		    }
		    
		    expr = xpath.compile("//LEDGER[PARENT = \"\"]");
		    Object resultForLedger = expr.evaluate(doc, XPathConstants.NODESET);

		    nodes = (NodeList) resultForLedger;
		    for (int i=0; i<nodes.getLength();i++){		
		    	Ledger ledger = new Ledger();
		    	Node ledgerNode = nodes.item(i);
		    	ledger.setParent(null);
		    	populateGroupOrLedgerFromNode(ledgerNode, ledger);
		    	
		    	if(!"".equals(ledger.getReservedName())){
		    	  reservedNameToActualNameMap.put(ledger.getReservedName(), ledger.getName());
		    	}
		    	twoLevelGroupAndLedgerMap.put(ledger, new ArrayList<GenericGroupOrLedger>());
		    }	    

		   //iterating over keys only
		   for (GenericGroupOrLedger primaryGroupOrLedger : twoLevelGroupAndLedgerMap.keySet()) {
		       String primaryGroupName = primaryGroupOrLedger.getName();
			   List<GenericGroupOrLedger> groupOrLedgerList = new ArrayList<>();

		       //First populate the GROUPS.
		       expr = xpath.compile("//GROUP[PARENT = \""+ primaryGroupName + "\"]");
			   nodes = (NodeList)expr.evaluate(doc, XPathConstants.NODESET);
			   //Populate all the sub-children of primary parent
			   for (int i=0; i<nodes.getLength();i++){	
			    	Group group = new Group();
			    	Node groupNode = nodes.item(i);
			    	group.setParent(primaryGroupOrLedger);
			    	populateGroupOrLedgerFromNode(groupNode, group);
			    	
			    	groupOrLedgerList.add(group);
			    	if(!"".equals(group.getReservedName())){
				    	  reservedNameToActualNameMap.put(group.getReservedName(), group.getName());
				    }
			    }
			   
			   //And now the ledgers
			   expr = xpath.compile("//LEDGER[PARENT = \""+ primaryGroupName + "\"]");
			   nodes = (NodeList)expr.evaluate(doc, XPathConstants.NODESET);
			   //Populate all the sub-children of primary parent
			   for (int i=0; i<nodes.getLength();i++){	
			    	Ledger ledger = new Ledger();
			    	Node ledgerNode = nodes.item(i);
			    	ledger.setParent(primaryGroupOrLedger);
			    	populateGroupOrLedgerFromNode(ledgerNode, ledger);
			    	
			    	groupOrLedgerList.add(ledger);
			    	if(!"".equals(ledger.getReservedName())){
				    	  reservedNameToActualNameMap.put(ledger.getReservedName(), ledger.getName());
				    }
			    }
			   
			   twoLevelGroupAndLedgerMap.put(primaryGroupOrLedger, groupOrLedgerList);
		   }
		   
	  }
	  
	  private static void populateGroupOrLedgerFromNode(Node groupOrLedgerNode, GenericGroupOrLedger groupOrLedger){
		  Element groupElement = (Element)groupOrLedgerNode;	    	    	
		  	groupOrLedger.setName(groupElement.getAttribute(NAME));
		  	groupOrLedger.setReservedName(groupElement.getAttribute(RESERVEDNAME));
	    	if(groupOrLedger instanceof Group){
		    	String isRevenueStr = ((Element)(groupElement.getElementsByTagName(ISREVENUE)).item(0)).getChildNodes().item(0).getNodeValue();
		    	if("No".equals(isRevenueStr)){
		    		((Group)groupOrLedger).setRevenue(false);
		    	}else{
		    		((Group)groupOrLedger).setRevenue(true);
		    	}
		    	
		    	String affectGrossProfitStr = ((Element)(groupElement.getElementsByTagName(AFFECTSGROSSPROFIT)).item(0)).getChildNodes().item(0).getNodeValue();
		    	if("No".equals(affectGrossProfitStr)){
		    		((Group)groupOrLedger).setAffectsGrossProfit(false);
		    	}else{
		    		((Group)groupOrLedger).setAffectsGrossProfit(true);
		    	}
		    	
		    	String isDeemedPositiveStr = ((Element)(groupElement.getElementsByTagName(ISDEEMEDPOSITIVE)).item(0)).getChildNodes().item(0).getNodeValue();
		    	if("No".equals(isDeemedPositiveStr)){
		    		((Group)groupOrLedger).setDeemedPositive(false);
		    	}else{
		    		((Group)groupOrLedger).setDeemedPositive(true);
		    	}
		    	
		    	String trackNegativeBalances = ((Element)(groupElement.getElementsByTagName(TRACKNEGATIVEBALANCES)).item(0)).getChildNodes().item(0).getNodeValue();
		    	if("No".equals(trackNegativeBalances)){
		    		((Group)groupOrLedger).setTrackNegativeBalances(false);
		    	}else{
		    		((Group)groupOrLedger).setTrackNegativeBalances(true);
		    	}
	    	}
	  }
}
