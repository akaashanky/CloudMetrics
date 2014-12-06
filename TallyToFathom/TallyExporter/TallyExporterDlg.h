
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
	afx_msg void OnBnClickedOk();

private:
	struct CompanyListResponse{ BOOL isError; std::string errorMsg; std::vector<string> companyList;};
	BOOL GetCompanyListFromTallyServer(int port, CompanyListResponse& companyListResponseRef);
	void GetCompanyListFromCSVResponse(CompanyListResponse& companyListResponse, std::string response);
	BOOL GetTallyTBResponse(CString& xmlTBRequestRef, vector<string>& allRRowsOfTBRef,int port=9000);
	void GetAllTallyLedgersAndGroupsFromRowList(vector<std::string>& tallyledgerAndGroupListRef,vector<string>& allRRowsOfTBRef);
	std::string GetGroupFromCSVRow(std::string& csvRow);
	void CTallyExporterDlg::ReplacePrimaryTBAccountsWithTheirChildren(vector<std::string>& tallyPrimaryGroupList, vector<string>& allRowsOfTBRef);
	void GetTBXMLForAParent(std::string parent, std::string& tbXMLStringForAParent);
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

public:
	
	CString m_strPort;
	CString m_strCompanySelectionCombo;
	CComboBox m_ctrlCompanyCombo;
	afx_msg void OnBnClickedGenerate();

private:
	std::string m_trialBalanceXMLRequest;	
	std::string m_companyName;
	vector<string> m_groupsToChangeSign;
	map<string, string> m_accountToPrimaryGroupMap;
    map<string,vector<string>> m_TaggingMappings;
};
