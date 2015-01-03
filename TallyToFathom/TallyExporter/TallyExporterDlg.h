
// TallyExporterDlg.h : header file
//

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include "afxwin.h"
#include <fstream>
#include "xpath_static.h"
#include "htmlutil.h"

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
	virtual void DoDataExchange(CDataExchange* pDX);	// FetchAndSaveMasterXMLFromTally/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

private:
	struct CompanyListResponse{ BOOL isError; std::string errorMsg; std::vector<string> companyList;};
	struct LedgerOrGroupCharacteristics{ BOOL shouldTheSignChange; std::string reservedNameOfItsParent;};
	BOOL GetCompanyListFromTallyServer(int port, CompanyListResponse& companyListResponseRef);
	void GetCompanyListFromCSVResponse(CompanyListResponse& companyListResponse, std::string response);
	BOOL GetTallyTBResponse(CString& xmlTBRequestRef, vector<string>& allRRowsOfTBRef,int port=9000);
	BOOL AppendProfitAndLossRowToTBData(CString& balanceSheetXMLReqStrForACompany, vector<string>& allRRowsOfTBRef,int port=9000);
	void GetAllTallyLedgersAndGroupsFromRowList(vector<std::string>& tallyledgerAndGroupListRef,vector<string>& allRRowsOfTBRef);
	std::string GetGroupFromCSVRow(std::string& csvRow);
	void CTallyExporterDlg::ReplacePrimaryTBAccountsWithTheirChildren(vector<std::string>& tallyPrimaryGroupList, vector<string>& allRowsOfTBRef);
	string GetRequestXMLForACompany(std::string xmlRequestStringForAParent);
	void WriteCSVDataToFile(vector<string>& allRowsOfTBRef);
	void PopulateKnownMappingsForTheLedgersAndGroups(vector<std::string>& groupAndLedgerListRef, vector<std::string>& unmappedGroupAndLedgerListRef, vector<std::string>& allRowsOfTBRef);
	void CTallyExporterDlg::GetMappingForKnownLedgerAndGroup(map<string,string>& ledgerAndGroupToTagMapping);
	void CTallyExporterDlg::PopulateMappingForARow(string mapping, int index, vector<string>& allRowsOfTBRef);
	int CTallyExporterDlg::NumberOfMonthsInCSVData(vector<string>& allRowsOfTBRef);
	void CTallyExporterDlg::processHeaderRow(string& headerRowRef, vector<string>& allRowsOfTBRef);
	bool CTallyExporterDlg::IsSignChangeOfAmountsNeeded(string groupName);
	void CTallyExporterDlg::ChangeSignOfAmountsInARow(string& csvRow);
	void CTallyExporterDlg::LoadTaggingMappings();
	void CTallyExporterDlg::PopulateMappingsForTheLedgersAndGroups(vector<std::string>& groupAndLedgerListRef, vector<std::string>& unmappedGroupAndLedgerListRef, vector<std::string>& allRowsOfTBRef);
	void CTallyExporterDlg::WriteMappingToFile(vector<string>& allRowsOfTBRef);
	void CTallyExporterDlg::WriteToLog(string LogContent, boolean writeStatusToDialog=true);
	boolean CTallyExporterDlg::ReadFileIntoString(string filePath, string& contentString, string& errorMsg);
	void  CTallyExporterDlg::GetTBXMLForAParent(std::string parent, std::string& trialBalanceXMLRequestForAParent);
	void CTallyExporterDlg::replaceDoubleCommaInBalanceSheetWithSingle(string& balanceSheetRow);
	BOOL CTallyExporterDlg::FetchAndSaveMasterXMLFromTally(CString& masterXMLReqStrForACompany,int port);
	void CTallyExporterDlg::WriteStringToAFile(string& stringToBeWrittenToFile, string relativeFilePathAndName);
	boolean CTallyExporterDlg::PopulatePrimaryGroupOrLedgerCharacteristicsMap();
	string CTallyExporterDlg::GetReservedNameForDisplayName(string displayName);
	boolean CTallyExporterDlg::PopulatePrimaryGroupOrLedgerCharacteristicsMap1();

public:
	
	CString m_strPort;
	CString m_strCompanySelectionCombo;
	CComboBox m_ctrlCompanyCombo;
	afx_msg void OnBnClickedGenerate();

private:
	ofstream m_logFile;
	string m_companyListXMLRequest;
	std::string m_trialBalanceXMLRequest;	
	std::string m_balanceSheetXMLRequest;
    string m_masterDataXMLRequest;

	std::string m_companyName;
	vector<string> m_groupsToChangeSign;
	map<string, string> m_accountToReservedGroupNameMap;
    map<string,vector<string>> m_TaggingMappings;
	map<string, LedgerOrGroupCharacteristics> m_primaryGroupOrLedgerCharacteristicMap;
public:
	afx_msg void OnBnClickedCancel();
	virtual BOOL DestroyWindow();
};
