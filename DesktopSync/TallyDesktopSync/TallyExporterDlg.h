
// TallyExporterDlg.h : header file
//

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include "afxwin.h"
using namespace std;

#pragma once

// CTallyExporterDlg dialog
class CTallyExporterDlg : public CDialogEx
{
// Construction
public:
	CTallyExporterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TALLYEXPORTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

private:
	struct CompanyListResponse{ BOOL isError; std::string errorMsg; std::vector<string> companyList;};
	BOOL GetCompanyListFromTallyServer(int port, CompanyListResponse& companyListResponseRef);
	void GetCompanyListFromCSVResponse(CompanyListResponse& companyListResponse, std::string response);
	BOOL GetTallyTBResponse(CString& xmlTBRequestRef, string& finalCSVFile,int port=9000);
	void GetMonthlyTBXMLForACompany(std::string& tbXMLStringForACompany);
	void WriteCSVDataToFile(string& csvFileContent, string& fileName);
	void GetYearlyTBXMLForACompany(std::string& tbXMLStringForACompany);
	void GetMasterXMLForACompany(std::string& masterXMLStringRequestForACompany);	
	void uploadZipFile(CString strServerUrl,CString strServerUploadFile, int port=80);
	CString CTallyExporterDlg::MakePostFileData(CString& strBoundary);
	CString CTallyExporterDlg::MakePreFileData(CString& strBoundary, CString& strFileName);
	CString CTallyExporterDlg::MakeRequestHeaders(CString& strBoundary);

public:
	
	CString m_strPort;
	CString m_strCompanySelectionCombo;
	CComboBox m_ctrlCompanyCombo;
	afx_msg void OnBnClickedGenerate();

private:
	CStdioFile m_logFile;
	string m_companyListXMLRequest;
	std::string m_trialBalanceXMLRequest;	
	string m_trialBalanceYearlyXMLRequest;
	string m_masterDataXMLRequest;

	std::string m_companyName;
	string m_emailId;
	string m_monthlyCsvFileName;
	string m_yearlyCsvFileName;
	string m_zipFileFinData;
	string m_masterXMLFileName;
	string m_cloudServerURL;

public:
	afx_msg void OnBnClickedSyncmasteranddata();
	afx_msg void OnBnClickedSyncdata();
};
