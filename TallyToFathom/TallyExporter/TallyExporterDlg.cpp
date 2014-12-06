
// TallyExporterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TallyExporter.h"
#include "TallyExporterDlg.h"
#include "afxdialogex.h"
#include <afxinet.h>
#include "TaggingDialog.h"

#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
END_MESSAGE_MAP()


// CTallyExporterDlg message handlers

BOOL CTallyExporterDlg::OnInitDialog()
{	

	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

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

	//for(std::vector<string>::iterator it = companyListResponseRef.companyList.begin(); it != companyListResponseRef.companyList.end(); ++it) {
	for(int i = 0; i < companyListResponseRef.companyList.size(); i++)
	{
		CString cstr(companyListResponseRef.companyList[i].c_str());
         m_ctrlCompanyCombo.AddString(cstr);
	}

	//Also read the XML request from file.
	string line;
	ifstream myfile ("data\\tb.xml");
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			m_trialBalanceXMLRequest += line;
		}
		myfile.close();
	}
	else cout << "Unable to open file"; 

	//Read the groups for which sign change is needed.
	string csvline;
	ifstream changeSignCsvFile ("data\\changesign.csv");
	if (changeSignCsvFile.is_open())
	{
		while ( getline (changeSignCsvFile,csvline) )
		{
			m_groupsToChangeSign.push_back(csvline);
		}
		changeSignCsvFile.close();
	}
	else cout << "Unable to open file"; 
	
	
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
   CInternetSession session(_T("My Session"));
   CHttpConnection* pServer = NULL;
   CHttpFile* pFile = NULL;
   try
   {
      CString strServerName;
      DWORD dwRet = 0;
	  CString strParam = "<ENVELOPE><HEADER><TALLYREQUEST>Export Data</TALLYREQUEST></HEADER><BODY><EXPORTDATA><REQUESTDESC><REPORTNAME>List of Companies</REPORTNAME></REQUESTDESC></EXPORTDATA></BODY></ENVELOPE>";
	  CString strHeaders  = _T("Content-Type: application/x-www-form-urlencoded;Accept-Encoding: gzip,deflate");
	  CString acceptedTypes[] = {_T("text/html")};

      pServer = session.GetHttpConnection(_T("localhost"),NULL, port, NULL, NULL);
      pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T(""), NULL,1,(LPCTSTR*)acceptedTypes,NULL,INTERNET_FLAG_EXISTING_CONNECT);

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

/* ===================================================================
*         When the "generate" button is clicked
*  ===================================================================
*/
void CTallyExporterDlg::OnBnClickedGenerate()
{
	CString companyName;
	CString& companyNameRef = companyName;
	this->GetDlgItemTextA(IDC_COMPANY_COMBO,companyNameRef);
	m_companyName = ((LPCTSTR)companyNameRef);

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

	GetTallyTBResponse(trialBalanceXMLRequestCSTRRef,allRowsOfTBRef, 9000);
		
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
	
	if(unmappedGroupAndLedgerListRef.size() > 0)
	{
		CTaggingDialog* taggingDialog = new CTaggingDialog(unmappedGroupAndLedgerList, &finalTagListForGroupAndLedgerAfterTaggingDialog, m_TaggingMappings,m_accountToPrimaryGroupMap, this);
		taggingDialog->DoModal();
	}

	PopulateMappingsForTheLedgersAndGroups(unmappedGroupAndLedgerListRef,finalTagListForGroupAndLedgerAfterTaggingDialogRef, allRowsOfTBRef);
	//Write everything to a file
	WriteCSVDataToFile(allRowsOfTBRef);
	WriteMappingToFile(allRowsOfTBRef);
	AfxMessageBox("Fathom Formatted File generated");
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
	ifstream myfile ("data\\mapping.csv");

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
		if(currentRowsOfTBRef.size() <= 2)
		{
			string rowToBePushed = allRowsOfTBRef[i+1];
			if(signChangeOfAmountReqd)
			{
				ChangeSignOfAmountsInARow(rowToBePushed);
			}
			newAllRowsOfTB.push_back(rowToBePushed);
			m_accountToPrimaryGroupMap[GetGroupFromCSVRow(rowToBePushed)] = tallyPrimaryGroupListRef.at(i); //Also, insert things into map
			continue;
		}

		//insert the children rows into the new vector
		for(int j = 1; j < currentRowsOfTBRef.size() - 1; j++)
		{
			string rowToBePushed = currentRowsOfTBRef[j];
			if(signChangeOfAmountReqd)
			{
				ChangeSignOfAmountsInARow(rowToBePushed);
			}
			newAllRowsOfTB.push_back(rowToBePushed);
			m_accountToPrimaryGroupMap[GetGroupFromCSVRow(rowToBePushed)] = tallyPrimaryGroupListRef.at(i); //Also, insert things into map
		}
	}
	//Put the last open balance row
    string lastRow = allRowsOfTBRef.at(allRowsOfTBRef.size() - 1);
	ChangeSignOfAmountsInARow(lastRow);
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
   }
   session.Close();
   return false;
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
	myfile.open ("data\\autogenerate.csv");

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
*         WriteCSVDataToFile
*  ===================================================================
*/
void CTallyExporterDlg::WriteMappingToFile(vector<string>& allRowsOfTBRef)
{	
	ofstream myfile;
	myfile.open ("data\\mapping.csv");

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
*         IsSignChangeOfAmountsNeeded
*  ===================================================================
*/
bool CTallyExporterDlg::IsSignChangeOfAmountsNeeded(string groupName)
{
	for(int index = 0; index < m_groupsToChangeSign.size(); index++)
	{
		if(m_groupsToChangeSign.at(index) == groupName)
		{
			return true;
		}
	}
	return false;
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
	ifstream taggingMappingFile ("data\\taggingMapping.csv");
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
