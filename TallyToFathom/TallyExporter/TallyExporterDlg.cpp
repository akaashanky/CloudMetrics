
// TallyExporterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TallyExporter.h"
#include "TallyExporterDlg.h"
#include "afxdialogex.h"
#include <afxinet.h>
#include "TaggingDialog.h"

#include "rapidxml/rapidxml.hpp"

#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MASTER_XML_STORAGE_PATH "data\\generated\\master.xml"
#define RESTRICTED_NAME_TO_FATHOM_TAG_MAPPING "data\\RestrictedNameToFathomTagMapping.csv"
#define FATHOM_FORMATTED_FILE "data\\generated\\fathomformatted.csv"
#define TALLY_TO_FATHOM_ACCOUNTS_MAPPING_FILE "data\\generated\\mapping.csv"
#define LOG_FILE "data\\generated\\log.txt"
#define GENERATED_DIR "data\\generated"

using namespace rapidxml;
// CTallyExporterDlg dialog



CTallyExporterDlg::CTallyExporterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTallyExporterDlg::IDD, pParent)
	, m_strPort(_T(""))
	, m_strCompanySelectionCombo(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTallyExporterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PORT, m_strPort);
	DDX_CBString(pDX, IDC_COMPANY_COMBO, m_strCompanySelectionCombo);
	DDX_Control(pDX, IDC_COMPANY_COMBO, m_ctrlCompanyCombo);
}

BEGIN_MESSAGE_MAP(CTallyExporterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTallyExporterDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_GENERATE, &CTallyExporterDlg::OnBnClickedGenerate)
	ON_BN_CLICKED(IDCANCEL, &CTallyExporterDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTallyExporterDlg message handlers

BOOL CTallyExporterDlg::OnInitDialog()
{	
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	string errorMsg = "";
	string& errorMsgRef = errorMsg;
	//Initialize logging
	m_logFile.open (LOG_FILE);
	WriteToLog("Logging initialised", false);

	string& m_companyListXMLRequestRef = m_companyListXMLRequest;
	//Read company list XMl request file.
	if(!ReadFileIntoString("data\\companylist_request.xml", m_companyListXMLRequest, errorMsgRef))
	{
		WriteToLog("Unable to open companylist_request.xml file");
		errorMsg = "Unable to open companylist_request.xml file";
	    AfxMessageBox("Unable to open companylist_request.xml file");
	}


	//TODO: Delete
	LoadTaggingMappings();

    //Check whether we can connect to Tally over the local 9000 port
    CompanyListResponse companyListResponse;
    CompanyListResponse& companyListResponseRef = companyListResponse;
    companyListResponseRef.isError = true;
    companyListResponseRef.errorMsg = "";
  	
	if(companyListResponseRef.isError != false)
	{
		SetDlgItemText(IDC_PORT, "9000");
	}
	GetCompanyListFromTallyServer(9000, companyListResponseRef);
	
	if(companyListResponseRef.companyList.size() == 0)
	{
		errorMsg = "No company found. Make sure that at least one company is loaded in Tally";
	   WriteToLog("No company found. Make sure that at least one company is loaded in Tally");
	}

	//for(std::vector<string>::iterator it = companyListResponseRef.companyList.begin(); it != companyListResponseRef.companyList.end(); ++it) {
	for(int i = 0; i < companyListResponseRef.companyList.size(); i++)
	{
		CString cstr(companyListResponseRef.companyList[i].c_str());
         m_ctrlCompanyCombo.AddString(cstr);
	}

	string& m_balanceSheetXMLRequestRef = m_balanceSheetXMLRequest;
	if(!ReadFileIntoString("data\\bl.xml", m_balanceSheetXMLRequestRef, errorMsgRef))
	{
		WriteToLog("Unable to open bl.xml file");
		errorMsg = "Unable to open bl.xml file";
	    AfxMessageBox("Unable to open bl.xml file");
	}

	string& m_trialBalanceXMLRequestRef = m_trialBalanceXMLRequest;
	if(!ReadFileIntoString("data\\tb.xml", m_trialBalanceXMLRequestRef, errorMsgRef))
	{
		WriteToLog("Unable to open tb.xml file");
		errorMsg = "Unable to open tb.xml file";
	    AfxMessageBox("Unable to open tb.xml file");
	}

	string& m_masterDataXMLRequestRef = m_masterDataXMLRequest;
	if(!ReadFileIntoString("data\\export_master.xml", m_masterDataXMLRequestRef, errorMsgRef))
	{
		WriteToLog("Unable to open export_master.xml file");
		errorMsg = "Unable to open export_master.xml file";
		AfxMessageBox("Unable to open export_master.xml file");
	}
   
	//create the generated dir inside data
	if (!CreateDirectory(GENERATED_DIR,NULL)) {
	  if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// directory already exists. Do nothing
	  } else {
		  WriteToLog("Could not create \"generated\" directory!");
		  errorMsg = "Could not create \"generated\" directory!";
		AfxMessageBox("Could not create \"generated\" directory!");
		return false;
	  }
	}
	
	if(errorMsg == "")
	  WriteToLog("Select a company to generate Fathom formatted file..");
	else
		WriteToLog(errorMsg);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTallyExporterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTallyExporterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTallyExporterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


/*
 * Connect to the Tally over a specific port and get all the companies
 */
BOOL CTallyExporterDlg::GetCompanyListFromTallyServer(int port, CompanyListResponse& companyListResponseRef)
{
   WriteToLog("Getting company list from Tally.");
   CInternetSession session(_T("My Session"));
   CHttpConnection* pServer = NULL;
   CHttpFile* pFile = NULL;
   try
   {
      CString strServerName;
      DWORD dwRet = 0;
	  CString strParam = m_companyListXMLRequest.c_str();
	  CString strHeaders  = _T("Content-Type: application/x-www-form-urlencoded;Accept-Encoding: gzip,deflate");
	  CString acceptedTypes[] = {_T("text/html")};

      pServer = session.GetHttpConnection(_T("localhost"),NULL, port, NULL, NULL);
      pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T(""), NULL,1,(LPCTSTR*)acceptedTypes,NULL,INTERNET_FLAG_EXISTING_CONNECT);

	  WriteToLog("Sending request to Tally.");
	  pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strParam, strParam.GetLength());
      pFile->QueryInfoStatusCode(dwRet);

      if (dwRet == HTTP_STATUS_OK)
      {
         CHAR szBuff[1024];
         while (pFile->Read(szBuff, 1024) > 0)
         {
            printf_s("%1023s", szBuff);
         }
		 GetCompanyListFromCSVResponse(companyListResponseRef, szBuff);
      }
	  else
	  { 
		  WriteToLog("Tally did not respond with data. Is Tally running?");
		  //Do something as server is not sending response as expected.
	  }
      delete pFile;
      delete pServer;
   }
   catch (CInternetException* pEx)
   {
       //catch errors from WinInet
      TCHAR pszError[64];
      pEx->GetErrorMessage(pszError, 64);
      _tprintf_s(_T("%63s"), pszError);
	  WriteToLog(pszError);
   }
   session.Close();
   return false;
}


/* ===================================================================
*          Finding Company List from CSV response.
*  ===================================================================
*/

void CTallyExporterDlg::GetCompanyListFromCSVResponse(CompanyListResponse& companyListResponse, std::string response)
{
	// different member versions of find in the same order as above:
	std::size_t found = response.find("<RESPONSE>");
	if (found!=std::string::npos)
	{//Found <RESPONSE>. So some error
		//Check for any error
		WriteToLog("Encountered some error. See the log file");
		WriteToLog(response, false);
		found = response.find("<LINEERROR>");
		if(found != std::string::npos)
		{
			std::size_t foundErrorEnd = response.find("</LINEERROR>");
			int lenOfError = foundErrorEnd - std::string("</LINEERROR>").length() + 1 - found;
        
			companyListResponse.isError = true;
			companyListResponse.errorMsg = response.substr (found + std::string("</LINEERROR>").length()-1, lenOfError);
			//companyListResponse.companyList
			return;
		}
	}
      //If no <RESPONSE> element was found then things must have worked.
	  // different member versions of find in the same order as above:
	  int lastStop = 1;
	  companyListResponse.isError = false;
	  while(true)
	  {
		  std::size_t found = response.find(",\r\n", lastStop);
		  if(found == std::string::npos)
			break;
		  companyListResponse.companyList.push_back(response.substr (lastStop, found - lastStop - 1));
		  lastStop = found + 4;
	  }

}

void BrowseToFile(LPCTSTR filename)
{
	char full[_MAX_PATH];
     _fullpath( full, filename, _MAX_PATH );
    ITEMIDLIST *pidl = ILCreateFromPath(full);
    if(pidl) {
        SHOpenFolderAndSelectItems(pidl,0,0,0);
        ILFree(pidl);
    }
}
/* ===================================================================
*         When the "generate" button is clicked
*  ===================================================================
*/
void CTallyExporterDlg::OnBnClickedGenerate()
{	
	//ShellExecute(NULL, NULL, "/select, data\\generated\\fathomformatted.csv", NULL, NULL, SW_SHOWNORMAL);
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	CString companyName;
	CString& companyNameRef = companyName;
	CString port;
	CString& portRef = port;

	this->GetDlgItemTextA(IDC_COMPANY_COMBO,companyNameRef);
	m_companyName = ((LPCTSTR)companyNameRef);
	this->GetDlgItemTextA(IDC_PORT,portRef);
	m_strPort = portRef;

	CString csvStringFromTallyResponse;	
	CString& csvStringFromTallyResponseRef = csvStringFromTallyResponse;
	CString trialBalanceXMLRequestCSTR;
	CString& trialBalanceXMLRequestCSTRRef = trialBalanceXMLRequestCSTR;
	std::string stringToBePassed;
	std::string& stringToBePassedRef = stringToBePassed;
	std::vector<string> allRowsOfTB;
	std::vector<string>& allRowsOfTBRef = allRowsOfTB;
	std::string trialBalanceXMLRequestForAParent;
	std::string& trialBalanceXMLRequestForAParentRef = trialBalanceXMLRequestForAParent;
	vector<string> groupAndLedgerList;
	vector<string>& groupAndLedgerListRef = groupAndLedgerList;
	vector<std::string> unmappedGroupAndLedgerList;
	vector<std::string>& unmappedGroupAndLedgerListRef = unmappedGroupAndLedgerList;
	vector<string> finalTagListForGroupAndLedgerAfterTaggingDialog;
	vector<string>& finalTagListForGroupAndLedgerAfterTaggingDialogRef = finalTagListForGroupAndLedgerAfterTaggingDialog;

	//First request for "#Primary" group
	GetTBXMLForAParent("#Primary",trialBalanceXMLRequestForAParentRef);
	trialBalanceXMLRequestCSTR = trialBalanceXMLRequestForAParent.c_str();
	
	WriteToLog("Getting Trial Balance Data for company: " + m_companyName);
	if(!GetTallyTBResponse(trialBalanceXMLRequestCSTRRef,allRowsOfTBRef, atoi(m_strPort)))
	{
		WriteToLog("Error Whille Getting TB Data");
		return;
	}
	
	//Append the last row, i.e. Profit and Loss row.
	string balanceSheetXMLReqStrForACompany = GetRequestXMLForACompany(m_balanceSheetXMLRequest);
	CString balanceSheetXMLReqStrForACompanyCStr = balanceSheetXMLReqStrForACompany.c_str();
	CString& balanceSheetXMLReqStrForACompanyCStrRef = balanceSheetXMLReqStrForACompanyCStr;
	WriteToLog("Getting Balance Sheet Data..");
	if(!AppendProfitAndLossRowToTBData(balanceSheetXMLReqStrForACompanyCStrRef, allRowsOfTBRef, atoi(m_strPort)))
	{
		WriteToLog("Error Getting Balance Sheet Data");
		return;
	}

	//Get Master XML data to infer various things.
	string masterXMLReqStrForACompany = GetRequestXMLForACompany(m_masterDataXMLRequest);
	CString masterXMLReqStrForACompanyCStr = masterXMLReqStrForACompany.c_str();
	CString& masterXMLReqStrForACompanyCStrRef = masterXMLReqStrForACompanyCStr;
	WriteToLog("Getting Master XML Data..");
	if(!FetchAndSaveMasterXMLFromTally(masterXMLReqStrForACompanyCStrRef, atoi(m_strPort)))
	{
		WriteToLog("Error Getting Master XML Data");
		return;
	}
		
	PopulatePrimaryGroupOrLedgerCharacteristicsMap();
	//Get the Groups, which are children of primary
	vector<std::string> tallyPrimaryGroupList;
	vector<std::string>& tallyPrimaryGroupListRef = tallyPrimaryGroupList;

	stringToBePassed = ((LPCTSTR)csvStringFromTallyResponse);

	//Find the Tally groups from the CSV data
	GetAllTallyLedgersAndGroupsFromRowList(tallyPrimaryGroupListRef, allRowsOfTBRef);

	//For each group post a message and replace with the actual csv
	ReplacePrimaryTBAccountsWithTheirChildren(tallyPrimaryGroupListRef, allRowsOfTBRef);

	//+++++++++++++++++  Open the Tagging dialog  ++++++++++++++++
	//First get the list of all groups and ledgers.
	GetAllTallyLedgersAndGroupsFromRowList(groupAndLedgerListRef, allRowsOfTBRef);
	PopulateKnownMappingsForTheLedgersAndGroups(groupAndLedgerListRef, unmappedGroupAndLedgerListRef, allRowsOfTBRef); 
	
	WriteToLog("Map Tally Groups/Ledgers to Fathom.");	
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	if(unmappedGroupAndLedgerListRef.size() > 0)
	{
		CTaggingDialog* taggingDialog = new CTaggingDialog(unmappedGroupAndLedgerList, &finalTagListForGroupAndLedgerAfterTaggingDialog, m_TaggingMappings,m_accountToReservedGroupNameMap, this);
		taggingDialog->DoModal();
	}

	PopulateMappingsForTheLedgersAndGroups(unmappedGroupAndLedgerListRef,finalTagListForGroupAndLedgerAfterTaggingDialogRef, allRowsOfTBRef);
	//Write everything to a file
	WriteCSVDataToFile(allRowsOfTBRef);
	WriteMappingToFile(allRowsOfTBRef);
	WriteToLog("Fathom formatted file generated.");
	BrowseToFile(FATHOM_FORMATTED_FILE);
}

/* ===================================================================
*         PopulateKnownMappingsForTheLedgersAndGroups
*  ===================================================================
*/
void CTallyExporterDlg::PopulateKnownMappingsForTheLedgersAndGroups(vector<std::string>& groupAndLedgerListRef, vector<std::string>& unmappedGroupAndLedgerListRef, vector<std::string>& allRowsOfTBRef)
{
	map<string,string> ledgerAndGroupToTagMapping;
	map<string,string>& ledgerAndGroupToTagMappingRef = ledgerAndGroupToTagMapping;
	GetMappingForKnownLedgerAndGroup(ledgerAndGroupToTagMappingRef);
	//Now iterate thru the ledger and group list and figure out whether it's there
	for(int i=0;i<groupAndLedgerListRef.size();i++)
	{
		//Got the name in the pre-existing map. So populate the main TB Row list
		if(ledgerAndGroupToTagMappingRef[groupAndLedgerListRef.at(i)]  != "")
		{
			PopulateMappingForARow(ledgerAndGroupToTagMappingRef[groupAndLedgerListRef.at(i)],  i+1, allRowsOfTBRef);
		}
		else
		{
			unmappedGroupAndLedgerListRef.push_back(groupAndLedgerListRef.at(i));
		}
	}
	//populate "Opening Balance"
	PopulateMappingForARow("CE",allRowsOfTBRef.size() - 1, allRowsOfTBRef);
}

/* ===================================================================
*         PopulateMappingsForTheLedgersAndGroups
*  ===================================================================
*/
void CTallyExporterDlg::PopulateMappingsForTheLedgersAndGroups(vector<std::string>& unmappedGroupAndLedgerListRef, vector<string>& finalTagListForGroupAndLedgerAfterTaggingDialog, vector<std::string>& allRowsOfTBRef)
{
	for(int i=0;i<unmappedGroupAndLedgerListRef.size();i++)
	{
		for(int j=0;j<allRowsOfTBRef.size();j++)//Inefficient, Algorithm order of O(N2). But who cares!
		{
			if(GetGroupFromCSVRow(allRowsOfTBRef.at(j)) == unmappedGroupAndLedgerListRef.at(i))
			{
				PopulateMappingForARow(finalTagListForGroupAndLedgerAfterTaggingDialog.at(i), j, allRowsOfTBRef);
			}
		}
	}
}

/* ===================================================================
*         PopulateMappingForARow
*  ===================================================================
*/
void CTallyExporterDlg::PopulateMappingForARow(string mapping, int index, vector<string>& allRowsOfTBRef)
{
	string csvLine = allRowsOfTBRef[index];
	csvLine.insert(0, mapping + ",");
	allRowsOfTBRef[index] = csvLine;
}

/* ===================================================================
*         GetMappingForKnownLedgerAndGroup
*  ===================================================================
*/
void CTallyExporterDlg::GetMappingForKnownLedgerAndGroup(map<string,string>& ledgerAndGroupToTagMapping)
{
	string line;
	ifstream myfile (TALLY_TO_FATHOM_ACCOUNTS_MAPPING_FILE);

	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			string mapping = "";
			string ledgerOrGroupname = "";
			//It's totally inefficient, but who cares at this stage!
			std::size_t found = line.find(",");
			if (found!=std::string::npos)
			{
				int lenOfGroupName = found;
				mapping = line.substr(0, lenOfGroupName);
				ledgerOrGroupname = line.substr(lenOfGroupName + 1, line.length() - lenOfGroupName);
			}
			else continue;	
			ledgerAndGroupToTagMapping[ledgerOrGroupname] = mapping;
		}
		myfile.close();
	}

	else cout << "Unable to open file"; 
}

/* ===================================================================
*         ReplacePrimaryTBAccountsWithTheirChildren
*  A lot of things happen here. Like negating or positing(!) amounts
*  Populating the parent "reserved name" from Tally Master data.
*  ===================================================================
*/
void CTallyExporterDlg::ReplacePrimaryTBAccountsWithTheirChildren(vector<std::string>& tallyPrimaryGroupListRef, vector<string>& allRowsOfTBRef)
{	
	std::vector<string> currentRowsOfTB;
	std::vector<string>& currentRowsOfTBRef = currentRowsOfTB;
	std::string trialBalanceXMLRequestForAParent;
	std::string& trialBalanceXMLRequestForAParentRef = trialBalanceXMLRequestForAParent;
	CString trialBalanceXMLRequestCSTR;
	CString& trialBalanceXMLRequestCSTRRef = trialBalanceXMLRequestCSTR;
	
	//Put rows into a new TB vector
	vector<string> newAllRowsOfTB;
	newAllRowsOfTB.push_back(allRowsOfTBRef.at(0));

	//Ignore the first row. It contains the date range. And the last. contains opening balance
	//Hence the primary group corresponds with i+1 of the allrowofTB
	int currentRowOfMainTBVectorWhereChildrenNeedToBeInserted = 1;

	for(int i = 0; i < tallyPrimaryGroupListRef.size(); i++)
	{		
		currentRowsOfTBRef.clear();
		GetTBXMLForAParent(tallyPrimaryGroupListRef.at(i), trialBalanceXMLRequestForAParentRef);
		trialBalanceXMLRequestCSTR = trialBalanceXMLRequestForAParent.c_str();
		GetTallyTBResponse(trialBalanceXMLRequestCSTR, currentRowsOfTBRef, 9000);
				
		//Change sign if needed
		bool signChangeOfAmountReqd = IsSignChangeOfAmountsNeeded(tallyPrimaryGroupListRef.at(i));

		//if no row is returned, just copy the row from old vector to new
		if(currentRowsOfTBRef.size() <= 1)
		{
			string rowToBePushed = allRowsOfTBRef[i+1];
			if(signChangeOfAmountReqd)
			{
				ChangeSignOfAmountsInARow(rowToBePushed);
			}
			newAllRowsOfTB.push_back(rowToBePushed);
			m_accountToReservedGroupNameMap[GetGroupFromCSVRow(rowToBePushed)] = GetReservedNameForDisplayName(tallyPrimaryGroupListRef.at(i)); //Also, insert things into map
			continue;
		}

		//insert the children rows into the new vector
		for(int j = 1; j < currentRowsOfTBRef.size(); j++)
		{
			string rowToBePushed = currentRowsOfTBRef[j];
			if(signChangeOfAmountReqd)
			{
				ChangeSignOfAmountsInARow(rowToBePushed);
			}
			newAllRowsOfTB.push_back(rowToBePushed);
			m_accountToReservedGroupNameMap[GetGroupFromCSVRow(rowToBePushed)] = GetReservedNameForDisplayName(tallyPrimaryGroupListRef.at(i)); //Also, insert things into map
		}
	}
	//Put the last open balance row
    string lastRow = allRowsOfTBRef.at(allRowsOfTBRef.size() - 1);
	newAllRowsOfTB.push_back(lastRow);
	allRowsOfTBRef = newAllRowsOfTB;
}

/* ===================================================================
*         GetTallyGroupsWhichAreChildrenOfPrimary
*  ===================================================================
*/

void CTallyExporterDlg::GetAllTallyLedgersAndGroupsFromRowList(vector<std::string>& tallyledgerAndGroupListRef, vector<string>& allRowsOfTBRef)
{
	//Ignore the first row. It contains the date range and the last
	for(int i = 1; i < allRowsOfTBRef.size() - 1; i++)
	{
		//For a row, parse it
		tallyledgerAndGroupListRef.push_back(GetGroupFromCSVRow(allRowsOfTBRef.at(i)));
	}
}

/* ===================================================================
*         GetGroupFromCSVRow
*  ===================================================================
*/
std::string CTallyExporterDlg::GetGroupFromCSVRow(std::string& csvRow)
{
	//Find the first comma. If not present, drop it.
	std::size_t found = csvRow.find(",");
	if (found!=std::string::npos)
	{
		int lenOfGroupName = found - 2;
		return csvRow.substr(1, lenOfGroupName);
	}
	else return NULL;
}

/* ===================================================================
*         GET Tally TB Response for a given request
*  ===================================================================
*/
BOOL CTallyExporterDlg::GetTallyTBResponse(CString& xmlTBRequestRef,vector<string>& allRowsOfTBRef, int port)
{
   CInternetSession session(_T("TB Session"));
   CHttpConnection* pServer = NULL;
   CHttpFile* pFile = NULL;   

   try
   {
      CString strServerName;
      DWORD dwRet = 0;
	  CString strHeaders  = _T("Content-Type: application/x-www-form-urlencoded;Accept-Encoding: gzip,deflate");
	  CString acceptedTypes[] = {_T("text/html")};

	  //CString logContent(_T("Connecting to get TB - localhost at port ") + port);
	  //WriteToLog((LPCTSTR)logContent);
      pServer = session.GetHttpConnection(_T("localhost"),NULL, port, NULL, NULL);
      pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T(""), NULL,1,(LPCTSTR*)acceptedTypes,NULL,INTERNET_FLAG_EXISTING_CONNECT);

	  pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)xmlTBRequestRef, xmlTBRequestRef.GetLength());
      pFile->QueryInfoStatusCode(dwRet);

	  CString csvLinesToBeRead;
	  CString& csvLinesToBeReadRef = csvLinesToBeRead;
      if (dwRet == HTTP_STATUS_OK)
      {
         while (pFile->ReadString(csvLinesToBeReadRef) == TRUE)
         {			 
			 allRowsOfTBRef.push_back((LPCTSTR)csvLinesToBeReadRef);
         }
      }
	  else
	  { 
		  //Do something as server is not sending response as expected.
	  }
      delete pFile;
      delete pServer;
   }
   catch (CInternetException* pEx)
   {
       //catch errors from WinInet
      TCHAR pszError[64];
      pEx->GetErrorMessage(pszError, 64);
      _tprintf_s(_T("%63s"), pszError);
	   WriteToLog("Error encountered while connecting to Tally");
	  WriteToLog(pszError, false);
	  return false;
   }
   session.Close();
   return true;
}

/* ===================================================================
*         AppendProfitAndLossRowToTBData
*  ===================================================================
*/
BOOL CTallyExporterDlg::AppendProfitAndLossRowToTBData(CString& balanceSheetXMLReqStrForACompany, vector<string>& allRowsOfTBRef,int port)
{
   vector<string> tempStoreForBalanceSheetRows;
   CInternetSession session(_T("BalanceSheet Session"));
   CHttpConnection* pServer = NULL;
   CHttpFile* pFile = NULL;   

   try
   {
      CString strServerName;
      DWORD dwRet = 0;
	  CString strHeaders  = _T("Content-Type: application/x-www-form-urlencoded;Accept-Encoding: gzip,deflate");
	  CString acceptedTypes[] = {_T("text/html")};

      pServer = session.GetHttpConnection(_T("localhost"),NULL, port, NULL, NULL);
      pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T(""), NULL,1,(LPCTSTR*)acceptedTypes,NULL,INTERNET_FLAG_EXISTING_CONNECT);

	  pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)balanceSheetXMLReqStrForACompany, balanceSheetXMLReqStrForACompany.GetLength());
      pFile->QueryInfoStatusCode(dwRet);

	  CString csvLinesToBeRead;
	  CString& csvLinesToBeReadRef = csvLinesToBeRead;
      if (dwRet == HTTP_STATUS_OK)
      {
         while (pFile->ReadString(csvLinesToBeReadRef) == TRUE)
         {			 
			 tempStoreForBalanceSheetRows.push_back((LPCTSTR)csvLinesToBeReadRef);
         }
      }
	  else
	  { 
		  //Do something as server is not sending response as expected.
	  }
      delete pFile;
      delete pServer;
   }
   catch (CInternetException* pEx)
   {
       //catch errors from WinInet
      TCHAR pszError[64];
      pEx->GetErrorMessage(pszError, 64);
      _tprintf_s(_T("%63s"), pszError);
	  WriteToLog("Error encountered while connecting to Tally");
	  WriteToLog(pszError, false);
	  return false;
   }
   session.Close();

   //Now find the "Profit & Loss A/c" from the list of accounts.
   //TODO: Get this from the XML
   for(int i = 0; i < tempStoreForBalanceSheetRows.size(); i++)
   {
	   string balanceSheetRow = tempStoreForBalanceSheetRows[i];
	   string& balanceSheetRowRef = balanceSheetRow;
	   if(GetGroupFromCSVRow(balanceSheetRow) == "Profit & Loss A/c")
	   {
		   replaceDoubleCommaInBalanceSheetWithSingle(balanceSheetRowRef);
		   allRowsOfTBRef.push_back(balanceSheetRowRef);
		   break;
	   }
   }
   return true;
}

/* ===================================================================
*         AppendProfitAndLossRowToTBData
*  ===================================================================
*/
BOOL CTallyExporterDlg::FetchAndSaveMasterXMLFromTally(CString& masterXMLReqStrForACompany, int port)
{
   CString masterXMLData;
	CInternetSession session(_T("Master XML Session"));
   CHttpConnection* pServer = NULL;
   CHttpFile* pFile = NULL;   

   try
   {
      CString strServerName;
      DWORD dwRet = 0;
	  CString strHeaders  = _T("Content-Type: application/x-www-form-urlencoded;Accept-Encoding: gzip,deflate");
	  CString acceptedTypes[] = {_T("text/html")};

      pServer = session.GetHttpConnection(_T("localhost"),NULL, port, NULL, NULL);
      pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T(""), NULL,1,(LPCTSTR*)acceptedTypes,NULL,INTERNET_FLAG_EXISTING_CONNECT);

	  pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)masterXMLReqStrForACompany, masterXMLReqStrForACompany.GetLength());
      pFile->QueryInfoStatusCode(dwRet);

	  CString csvLinesToBeRead;
	  CString& csvLinesToBeReadRef = csvLinesToBeRead;
      if (dwRet == HTTP_STATUS_OK)
      {
         while (pFile->ReadString(csvLinesToBeReadRef) == TRUE)
         {			 
			 masterXMLData += csvLinesToBeReadRef;
         }
      }
	  else
	  { 
		  //Do something as server is not sending response as expected.
	  }
      delete pFile;
      delete pServer;
   }
   catch (CInternetException* pEx)
   {
       //catch errors from WinInet
      TCHAR pszError[64];
      pEx->GetErrorMessage(pszError, 64);
      _tprintf_s(_T("%63s"), pszError);
	  WriteToLog("Error encountered while getting master XML from Tally");
	  WriteToLog(pszError, false);
	  return false;
   }
   session.Close();

   //Write it to a file
   string stringToBeWrittenToFile = masterXMLData;
   string& stringToBeWrittenToFileRef= stringToBeWrittenToFile;
   WriteStringToAFile(stringToBeWrittenToFileRef, MASTER_XML_STORAGE_PATH);
   return true;
}

/* ===================================================================
*         GetTBXMLForAParent
*  ===================================================================
*/
string CTallyExporterDlg::GetRequestXMLForACompany(std::string xmlRequestStringForACompany)
{	
	string modifiedXMLString = xmlRequestStringForACompany;
	int indexOfCompanyName = modifiedXMLString.find("{{CompanyName}}");
	modifiedXMLString.replace(indexOfCompanyName, 15, m_companyName);	
	return modifiedXMLString;
}

/* ===================================================================
*         GetTBXMLForAParent
*  ===================================================================
*/
void  CTallyExporterDlg::GetTBXMLForAParent(std::string parent, std::string& trialBalanceXMLRequestForAParent)
{	
	trialBalanceXMLRequestForAParent = m_trialBalanceXMLRequest;
	int indexOfCompanyName = trialBalanceXMLRequestForAParent.find("{{CompanyName}}");
	trialBalanceXMLRequestForAParent.replace(indexOfCompanyName, 15, m_companyName);
	int indexOfReplacingTerm1 = trialBalanceXMLRequestForAParent.find("{{ParentOFTB1}}");
	trialBalanceXMLRequestForAParent.replace(indexOfReplacingTerm1, 15, parent);
	int indexOfReplacingTerm2 = trialBalanceXMLRequestForAParent.find("{{ParentOFTB2}}");
	trialBalanceXMLRequestForAParent.replace(indexOfReplacingTerm2, 15, parent);
}

/* ===================================================================
*         WriteCSVDataToFile
*  ===================================================================
*/
void CTallyExporterDlg::WriteCSVDataToFile(vector<string>& allRowsOfTBRef)
{	
	//Total number of months?
	int noOfMonthsInCSVData = NumberOfMonthsInCSVData(allRowsOfTBRef);

	ofstream myfile;
	myfile.open (FATHOM_FORMATTED_FILE);

	//spit out the company name
	myfile << "Name:," + m_companyName + ",";
	for(int i=0; i<noOfMonthsInCSVData-1; i++)
	{
		myfile<< ",";
	}
	myfile<< "\n";

	myfile << "Financial Year Start:,April,";
	for(int i=0; i<noOfMonthsInCSVData-1; i++)
	{
		myfile<< ",";
	}
	myfile<< "\n";

	myfile << "Period Frequency:,Monthly,";
	for(int i=0; i<noOfMonthsInCSVData-1; i++)
	{
		myfile<< ",";
	}
	myfile<< "\n";

	string headerRow;
	string& headerRowRef = headerRow;
	processHeaderRow(headerRowRef, allRowsOfTBRef);

	myfile<< headerRowRef + "\n";
	//out all rows
	for(int i = 1; i < allRowsOfTBRef.size(); i++)
	{
		myfile << allRowsOfTBRef[i]+"\n";
	}	
	myfile.close();
}

/* ===================================================================
*         WriteMappingToFile
*  ===================================================================
*/
void CTallyExporterDlg::WriteMappingToFile(vector<string>& allRowsOfTBRef)
{	
	ofstream myfile;
	myfile.open (TALLY_TO_FATHOM_ACCOUNTS_MAPPING_FILE);

	for(int i = 1; i < allRowsOfTBRef.size(); i++)
	{
		string csvRow = allRowsOfTBRef[i];
		int found = csvRow.find(",", 0);
		string mapping = csvRow.substr(0, found);
		int found1 = csvRow.find(",",found + 1);
		string ledgerGroup = csvRow.substr(found + 2, found1 - found - 3);
		myfile<< mapping<<","<<ledgerGroup<<"\n";
	}	
	myfile.close();
}

/* ===================================================================
*         WriteMappingToFile
*  ===================================================================
*/
void CTallyExporterDlg::WriteStringToAFile(string& stringToBeWrittenToFile, string relativeFilePathAndName)
{	
	ofstream myfile;
	myfile.open (relativeFilePathAndName);

	myfile << stringToBeWrittenToFile;
	myfile.close();
}

/* ===================================================================
*         processHeaderRow
*  ===================================================================
*/
void CTallyExporterDlg::processHeaderRow(string& headerRowRef, vector<string>& allRowsOfTBRef)
{
	string tmpHeaderRow = allRowsOfTBRef.at(0);
	headerRowRef.insert(0,"Account Tag,Account Name,"); 
	//Next lines are fragile! TODO: Change at TDL level.
	int startPos = 0;
	while(1)
	{
		std::size_t found = tmpHeaderRow.find(",", startPos);
		std::size_t rfound = tmpHeaderRow.rfind(",");
		if (found!=std::string::npos && found!=rfound)
		{
			headerRowRef = headerRowRef + tmpHeaderRow.substr(found + 4, 4);
			headerRowRef = headerRowRef + tmpHeaderRow.substr(found + 10, 2) + ",";
			startPos = found + 1;
		}
		else break;
	}
}

/* ===================================================================
*         NumberOfMonthsInCSVData
*  ===================================================================
*/
int CTallyExporterDlg::NumberOfMonthsInCSVData(vector<string>& allRowsOfTBRef)
{
  string csvRow = allRowsOfTBRef.at(0);
  int count = 0;
  int startPos = 0;
  while(1)
  {
    std::size_t found = csvRow.find(",", startPos);
	if (found!=std::string::npos)
	{
		count++;	
		startPos = found + 1;
	}
	else break;
  }
  return count;
}

/* ===================================================================
*         replaceDoubleCommaInBalanceSheetWithSingle
*  ===================================================================
*/
void CTallyExporterDlg::replaceDoubleCommaInBalanceSheetWithSingle(string& balanceSheetRow)
{
	size_t index = 0;
    while (true) {
         /* Locate the substring to replace. */
         index = balanceSheetRow.find(",,", index);
         if (index == std::string::npos) break;
    
         /* Make the replacement. */
         balanceSheetRow.replace(index, 2, ",");
    
         /* Advance index forward so the next iteration doesn't pick it up as well. */
         index += 1;
     }
}

/* ===================================================================
*         IsSignChangeOfAmountsNeeded
*  ===================================================================
*/
bool CTallyExporterDlg::IsSignChangeOfAmountsNeeded(string groupName)
{
	if(m_primaryGroupOrLedgerCharacteristicMap.find(groupName) != m_primaryGroupOrLedgerCharacteristicMap.end()) 
	  return m_primaryGroupOrLedgerCharacteristicMap.find(groupName)->second.shouldTheSignChange;
	else //This condition should not happen. The primary group name must exist in the map.
		return FALSE;
}

/* ===================================================================
*         ChangeSignOfAmountsInARow
*  ===================================================================
*/
void CTallyExporterDlg::ChangeSignOfAmountsInARow(string& csvRow)
{
    string newCsvRow;
    string& newCsvRowRef = newCsvRow;
  //After every comma, see if content is there. If yes insert a - or delete one.
	  int startPos = 0;
	  while(1)
	  {
		std::size_t found = csvRow.find(",", startPos);
		newCsvRowRef += csvRow.substr(startPos, found - startPos + 1);		
		if (found!=std::string::npos)
		{
		    if(found +1 < csvRow.length()) //Thr is something after this comma
		    {		        
				if(isdigit(csvRow[found + 1])) //Is it positive?
				{
					newCsvRowRef += '-';						
					startPos = found + 1;
				}
				else if(csvRow[found + 1] == '-')//Is it a minus?
				{
					startPos = found + 2;
				}
				else
				{
					startPos = found + 1;
				}
		    }else break;
			
		}
		else break;
	  }
	  csvRow = newCsvRowRef;
}

/* ===================================================================
*         LoadTaggingMappings
*  ===================================================================
*/
void CTallyExporterDlg::LoadTaggingMappings()
{
	//Read the groups for which sign change is needed.
	string csvRow;
	ifstream taggingMappingFile (RESTRICTED_NAME_TO_FATHOM_TAG_MAPPING);
	if (taggingMappingFile.is_open())
	{
		while ( getline (taggingMappingFile,csvRow) )
		{
			//parse the csvline and populate the map.
			int startPos = 0;		
			string groupName;
			vector<string> tagging;
			while(1)
			{
				std::size_t found = csvRow.find(",", startPos);				
				if (found!=std::string::npos)
				{
					if(startPos == 0) //First cell should be the group name
					{
						groupName = csvRow.substr(startPos, found - startPos);
						startPos = found + 1;
						continue;
					}
					if(found < csvRow.length()) //
					{		        
						tagging.push_back(csvRow.substr(startPos, found - startPos));
						startPos = found + 1;
					}else break;
			
				}
				else break;
			}
			m_TaggingMappings[groupName] = tagging;
		}
		taggingMappingFile.close();
	}
	else cout << "Unable to open file"; 
}

void CTallyExporterDlg::WriteToLog(string LogContent, boolean writeStatusToDialog)
{	
	if(writeStatusToDialog)
		SetDlgItemText(IDC_MESSAGE_STATIC, (LPCTSTR)LogContent.c_str());
	 time_t rawtime;
     struct tm * timeinfo;

     time (&rawtime);
     timeinfo = localtime (&rawtime);
	 m_logFile<< asctime(timeinfo) + LogContent + "\n";
	 m_logFile.flush();
}



void CTallyExporterDlg::OnBnClickedCancel()
{
	if(m_logFile.is_open())
	   m_logFile.close();
	CDialogEx::OnCancel();
}

boolean CTallyExporterDlg::ReadFileIntoString(string filePath, string& contentString, string& errorMsg)
{
	boolean isSuccess = false;
	string line;
	ifstream fileToBeRead (filePath);
	if (fileToBeRead.is_open())
	{
		while ( getline (fileToBeRead,line) )
		{
			contentString += line;
		}
		fileToBeRead.close();
		isSuccess = true;
	}
	return isSuccess;
}

/* ======================================================
 *     PopulatePrimaryGroupOrLedgerCharacteristicsMap
 * ======================================================
 */
boolean CTallyExporterDlg::PopulatePrimaryGroupOrLedgerCharacteristicsMap()
{
	string errorMsg = "";
	string& errorMsgRef = errorMsg;
	TiXmlDocument *doc = new TiXmlDocument;   
	string masterXMLContent;
	string& masterXMLContentRef = masterXMLContent;
	WriteToLog("Analyzing accounting master data..");
	ReadFileIntoString(MASTER_XML_STORAGE_PATH, masterXMLContentRef, errorMsgRef);
	doc->Parse(masterXMLContent.c_str()); 
	TiXmlElement * XEp_main;
	const TiXmlNode * node;
	XEp_main = doc->RootElement ();
	TinyXPath::xpath_processor xp_proc (XEp_main, "ENVELOPE/BODY/DATA/TALLYMESSAGE/GROUP/PARENT[text()='']/parent::*");    // build the list of all element nodes in the tree
	TinyXPath::expression_result xresult;
	xresult = xp_proc.er_compute_xpath();
	TinyXPath::node_set* ns = xresult.nsp_get_node_set(); 
	int noOfPrimaryGroupNodes = ns->u_get_nb_node_in_set ();
	for(int i = 0; i < noOfPrimaryGroupNodes; i++)
	{
		LedgerOrGroupCharacteristics ledgerOrGroupCharacteristics;
		node = ns->XNp_get_node_in_set(i);
		const TiXmlElement *Xelement = node->ToElement();

		ledgerOrGroupCharacteristics.reservedNameOfItsParent = Xelement->Attribute("RESERVEDNAME");		
		string shouldChangeSignStr = node->FirstChild("ISDEEMEDPOSITIVE")->FirstChild()->Value();
		if(shouldChangeSignStr == "Yes")
		{
			ledgerOrGroupCharacteristics.shouldTheSignChange = true;
		}
		else
		{
			ledgerOrGroupCharacteristics.shouldTheSignChange = false;
		}
		string groupOrLedgerName = Xelement->Attribute("NAME");
		m_primaryGroupOrLedgerCharacteristicMap[groupOrLedgerName] = ledgerOrGroupCharacteristics;
	}
	WriteToLog("Done Analyzing..");
	return true;
}

/*  =================================================
 *   PopulatePrimaryGroupOrLedgerCharacteristicsMap
 *  =================================================
 */
boolean CTallyExporterDlg::PopulatePrimaryGroupOrLedgerCharacteristicsMap1()
{
	xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile (MASTER_XML_STORAGE_PATH);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	return false;
}

/*  =====================================
 *    GetReservedNameForDisplayName
 *  =====================================
 */
string CTallyExporterDlg::GetReservedNameForDisplayName(string displayName)
{
	string reservedName;
	if(m_primaryGroupOrLedgerCharacteristicMap.find(displayName) != m_primaryGroupOrLedgerCharacteristicMap.end())
		reservedName = m_primaryGroupOrLedgerCharacteristicMap.find(displayName)->second.reservedNameOfItsParent;
	else
		reservedName = displayName;
	//TODO: Do different things for revenue, expense etc.
	if(reservedName == "")
		reservedName = displayName;
	return reservedName;
}

BOOL CTallyExporterDlg::DestroyWindow()
{
    if(m_logFile.is_open())
	   m_logFile.close();
	return CDialogEx::DestroyWindow();
}
