
// TallyExporterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TallyExporter.h"
#include "TallyExporterDlg.h"
#include "afxdialogex.h"
#include <afxinet.h>

#include <iostream>
#include <fstream>

#include "zip.h"

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
	ON_BN_CLICKED(ID_SYNCMASTERANDDATA, &CTallyExporterDlg::OnBnClickedSyncmasteranddata)
	ON_BN_CLICKED(ID_SYNCDATA, &CTallyExporterDlg::OnBnClickedSyncdata)
END_MESSAGE_MAP()


// CTallyExporterDlg message handlers

BOOL CTallyExporterDlg::OnInitDialog()
{	

	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//Also read the XML request for company list from file.
	string line1;
	ifstream companyListXMLFile ("data\\companylist_request.xml");
	if (companyListXMLFile.is_open())
	{
		while ( getline (companyListXMLFile,line1) )
		{
			m_companyListXMLRequest += line1;
		}
		companyListXMLFile.close();
	}
	else AfxMessageBox("Unable to open company list XML file"); 

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
	ifstream tb ("data\\tb.xml");
	if (tb.is_open())
	{
		while ( getline (tb,line) )
		{
			m_trialBalanceXMLRequest += line;
		}
		tb.close();
	}
	else cout << AfxMessageBox("Unable to open tb.xml file");

	//Also read the XML request from file.
	string line2;
	ifstream tb_yearly ("data\\tb_yearly.xml");
	if (tb_yearly.is_open())
	{
		while ( getline (tb_yearly,line2) )
		{
			m_trialBalanceYearlyXMLRequest += line2;
		}
		tb_yearly.close();
	}
	else cout << AfxMessageBox("Unable to open tb_yearly.xml file"); 
	
	//Read the export master file
	string line3;
	ifstream export_master ("data\\export_master.xml");
	if (export_master.is_open())
	{
		while ( getline (export_master,line3) )
		{
			m_masterDataXMLRequest += line3;
		}
		export_master.close();
	}
	else cout << AfxMessageBox("Unable to open export_master.xml file"); 
	
	
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
	  CString strParam = m_companyListXMLRequest.c_str();
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
*         When the "Sync data" button is clicked
*  ===================================================================
*/
void CTallyExporterDlg::OnBnClickedSyncmasteranddata()
{
	CString companyName;
	CString& companyNameRef = companyName;
	CString port;
	CString& portRef = port;
	CString emailId;
	CString& emailIdRef = emailId;

	//Get data from the dialog.
	this->GetDlgItemTextA(IDC_COMPANY_COMBO,companyNameRef);
	m_companyName = ((LPCTSTR)companyNameRef);
	this->GetDlgItemTextA(IDC_PORT,portRef);
	m_strPort = portRef;
	this->GetDlgItemTextA(IDC_EMAIL,emailIdRef);
	m_emailId = emailIdRef;

	m_monthlyCsvFileName = companyNameRef + "__" + emailIdRef + "__"  + "findata_monthly.csv";
	m_yearlyCsvFileName = companyNameRef + "__" + emailIdRef + "__" + "findata_yearly.csv";
	m_masterXMLFileName = companyNameRef + "__" + emailIdRef + "__" + "master.xml";
	m_zipFileFinData = companyNameRef + "__" + emailIdRef + "__"  + "cloudmetricdata.zip";

	CString trialBalanceMonthlyXMLRequestCSTR;
	CString& trialBalanceMonthlyXMLRequestCSTRRef = trialBalanceMonthlyXMLRequestCSTR;
	std::string trialBalanceMonthlyXMLRequestForACompany;
	std::string& trialBalanceMonthlyXMLRequestForACompanyRef = trialBalanceMonthlyXMLRequestForACompany;

    CString trialBalanceYearlyXMLRequestCSTR;
	CString& trialBalanceYearlyXMLRequestCSTRRef = trialBalanceYearlyXMLRequestCSTR;
	std::string trialBalanceYearlyXMLRequestForACompany;
	std::string& trialBalanceYearlyXMLRequestForACompanyRef = trialBalanceYearlyXMLRequestForACompany;

	CString masterDataXMLRequestCSTR;
	CString& masterDataXMLRequestCSTRRef = masterDataXMLRequestCSTR;
	std::string masterDataXMLRequestForACompany;
	std::string& masterDataXMLRequestForACompanyRef = masterDataXMLRequestForACompany;

	string finalCSVContentForMonthlyFile;
	string& finalCSVContentForMonthlyFileRef = finalCSVContentForMonthlyFile;
	string finalCSVContentForYearlyFile;
	string& finalCSVContentForYearlyFileRef = finalCSVContentForYearlyFile;
	string finalXMLContentForMasterFile;
	string& finalXMLContentForMasterFileRef = finalXMLContentForMasterFile;
	finalCSVContentForMonthlyFileRef = "CompanyName," + m_companyName + ",\n" + "EmailID," + m_emailId + ",\n" + "Period,Monthly," + "\n";
	finalCSVContentForYearlyFileRef = "CompanyName," + m_companyName + ",\n" + "EmailID," + m_emailId + ",\n" + "Period,Yearly," + "\n";
	finalXMLContentForMasterFileRef = "<CompanyName>" + m_companyName + "</CompanyName>" + "<EmailID>" + m_emailId + "</EmailID>" + "\n";

	GetMonthlyTBXMLForACompany(trialBalanceMonthlyXMLRequestForACompanyRef);
	trialBalanceMonthlyXMLRequestCSTRRef = trialBalanceMonthlyXMLRequestForACompanyRef.c_str();

	GetYearlyTBXMLForACompany(trialBalanceYearlyXMLRequestForACompanyRef);
	trialBalanceYearlyXMLRequestCSTRRef = trialBalanceYearlyXMLRequestForACompanyRef.c_str();

	GetMasterXMLForACompany(masterDataXMLRequestForACompanyRef);
	masterDataXMLRequestCSTRRef = masterDataXMLRequestForACompanyRef.c_str();

	GetTallyTBResponse(trialBalanceMonthlyXMLRequestCSTRRef,finalCSVContentForMonthlyFileRef, atoi(m_strPort));
	GetTallyTBResponse(trialBalanceYearlyXMLRequestCSTRRef,finalCSVContentForYearlyFileRef, atoi(m_strPort));
	GetTallyTBResponse(masterDataXMLRequestCSTRRef, finalXMLContentForMasterFileRef, atoi(m_strPort));

	//Write everything to a file
	WriteCSVDataToFile(finalCSVContentForMonthlyFileRef, m_monthlyCsvFileName);
	WriteCSVDataToFile(finalCSVContentForYearlyFileRef, m_yearlyCsvFileName);
	WriteCSVDataToFile(finalXMLContentForMasterFileRef, m_masterXMLFileName);

	HZIP hz = CreateZip(("data\\generated\\" + m_zipFileFinData).c_str(),0);
	ZipAdd(hz,m_monthlyCsvFileName.c_str(), ("data\\generated\\" + m_monthlyCsvFileName).c_str());
	ZipAdd(hz,m_yearlyCsvFileName.c_str(), ("data\\generated\\" + m_yearlyCsvFileName).c_str());
	ZipAdd(hz,m_masterXMLFileName.c_str(), ("data\\generated\\" + m_masterXMLFileName).c_str());
	CloseZip(hz);

	//TODO: Need to configure
	uploadZipFile("localhost", "/CM/datasync",8080);
	AfxMessageBox("Sync successful");
}

/* ===================================================================
*         GET Tally TB Response for a given request
*  ===================================================================
*/
BOOL CTallyExporterDlg::GetTallyTBResponse(CString& xmlTBRequestRef,string& csvContent, int port)
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
			 csvContent += (LPCTSTR)csvLinesToBeReadRef;
			 csvContent += "\n";
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
*         Various XML request based on company selected. 
*         TODO: Will be clubbed to one function.
*  ===================================================================
*/
void  CTallyExporterDlg::GetMonthlyTBXMLForACompany(std::string& trialBalanceXMLRequestForACompany)
{	
	trialBalanceXMLRequestForACompany = m_trialBalanceXMLRequest;
	int indexOfCompanyName = trialBalanceXMLRequestForACompany.find("{{CompanyName}}");
	trialBalanceXMLRequestForACompany.replace(indexOfCompanyName, 15, m_companyName);	
}

void  CTallyExporterDlg::GetYearlyTBXMLForACompany(std::string& trialBalanceXMLRequestForACompany)
{	
	trialBalanceXMLRequestForACompany = m_trialBalanceYearlyXMLRequest;
	int indexOfCompanyName = trialBalanceXMLRequestForACompany.find("{{CompanyName}}");
	trialBalanceXMLRequestForACompany.replace(indexOfCompanyName, 15, m_companyName);	
}

void CTallyExporterDlg::GetMasterXMLForACompany(std::string& masterXMLStringRequestForACompany)
{
	masterXMLStringRequestForACompany = m_masterDataXMLRequest;
	int indexOfCompanyName = masterXMLStringRequestForACompany.find("{{CompanyName}}");
	masterXMLStringRequestForACompany.replace(indexOfCompanyName, 15, m_companyName);
}

/* ===================================================================
*         WriteCSVDataToFile
*  ===================================================================
*/
void CTallyExporterDlg::WriteCSVDataToFile(string& csvContent, string& csvFileName)
{	
	ofstream myfile;
	myfile.open ("data\\generated\\" + csvFileName);
	
	myfile<< csvContent + "\n";
	myfile.close();
}



void CTallyExporterDlg::OnBnClickedSyncdata()
{
	// TODO: Add your control notification handler code here
}

void CTallyExporterDlg::uploadZipFile(CString strServerUrl,CString strServerUploadFile, int port)
{
	DWORD dwTotalRequestLength;
	DWORD dwChunkLength;
	DWORD dwReadLength;
	DWORD dwResponseLength;
	CHttpFile* pHTTP = NULL;

	dwChunkLength = 64 * 1024; 
	void* pBuffer = malloc(dwChunkLength);
	CFile file ;

	if (!file.Open(("data\\generated\\" + m_zipFileFinData).c_str(),
	  CFile::modeRead | CFile::shareDenyWrite))
	 {
			return;
	   }

	CInternetSession session("sendFile");
	CHttpConnection *connection = NULL;

	try
	{
		//Create the multi-part form data that goes before and after the actual file upload.

		CString strHTTPBoundary = _T("FFF3F395A90B452BB8BEDC878DDBD152");       
		CString strPreFileData = MakePreFileData(strHTTPBoundary, file.GetFileName());
		CString strPostFileData = MakePostFileData(strHTTPBoundary);
		CString strRequestHeaders = MakeRequestHeaders(strHTTPBoundary);
		dwTotalRequestLength = strPreFileData.GetLength() + strPostFileData.GetLength() + file.GetLength();

		connection = session.GetHttpConnection(/*L"www.YOURSITE.com"*/strServerUrl.GetBuffer(),NULL,port);

		pHTTP = connection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strServerUploadFile.GetBuffer());//_T("/YOUURL/submit_file.pl"));
		pHTTP->AddRequestHeaders(strRequestHeaders);
		pHTTP->SendRequestEx(dwTotalRequestLength, HSR_SYNC | HSR_INITIATE);

		//Write out the headers and the form variables
		pHTTP->Write((LPSTR)(LPCSTR)strPreFileData.GetBuffer(), strPreFileData.GetLength());

		//upload the file.

		dwReadLength = -1;
		int length = file.GetLength(); //used to calculate percentage complete.
		while (0 != dwReadLength)
		{
			dwReadLength = file.Read(pBuffer, dwChunkLength);
			if (0 != dwReadLength)
			{
			pHTTP->Write(pBuffer, dwReadLength);
			}
		}

		file.Close();

		//Finish the upload.
		pHTTP->Write((LPSTR)(LPCSTR)strPostFileData.GetBuffer(), strPostFileData.GetLength());
		pHTTP->EndRequest(HSR_SYNC);


		//get the response from the server.
		LPSTR szResponse;
		CString strResponse;
		dwResponseLength = pHTTP->GetLength();
		while (0 != dwResponseLength )
		{
			szResponse = (LPSTR)malloc(dwResponseLength + 1);
			szResponse[dwResponseLength] = '\0';
			pHTTP->Read(szResponse, dwResponseLength);
			strResponse += szResponse;
			free(szResponse);
			dwResponseLength = pHTTP->GetLength();
		}

		TRACE(L"%s",strResponse.GetBuffer());

		//close everything up.
		pHTTP->Close();
		connection->Close();
		session.Close();
	}
	catch(CInternetException* e)
	{
		TRACE(L"error: %d \n",e->m_dwError);
	}
	catch(CFileException* e)
	{
		TRACE(L"error: %d \n",e->m_cause);
	}
	catch(...)
	{
		TRACE(L" unexpected error");
	}

}

CString CTallyExporterDlg::MakeRequestHeaders(CString& strBoundary)
{
    CString strFormat;
    CString strData;
    strFormat = _T("Content-Type: multipart/form-data; boundary=%s\r\n");
    strData.Format(strFormat, strBoundary);
    return strData;
}

CString CTallyExporterDlg::MakePreFileData(CString& strBoundary, CString& strFileName)
{
	CString strFormat;
	CString strData;

	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"file\"; filename=\"%s\"");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Type: text/plain");
	strFormat += _T("\r\n");
	strFormat += _T(" XXXXX ");
	strFormat += _T("\r\n\r\n");

	strData.Format(strFormat, strBoundary,/* m_Name, strBoundary,*/ strFileName);

	return strData;
}

CString CTallyExporterDlg::MakePostFileData(CString& strBoundary)
{

	CString strFormat;
	CString strData;

	strFormat = _T("\r\n");
	strFormat += _T("--%s");
	strFormat += _T("\r\n");
	strFormat += _T("Content-Disposition: form-data; name=\"submit\"");
	strFormat += _T("\r\n\r\n");
	strFormat += _T("");
	strFormat += _T("\r\n");
	strFormat += _T("--%s--");
	strFormat += _T("\r\n");

	strData.Format(strFormat, strBoundary, strBoundary);

	return strData;

}