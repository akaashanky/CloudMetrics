#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include "afxwin.h"
using namespace std;

#pragma once

#define MAX_NOOF_COLUMN_IN_ONE_SHEET 7

// CTaggingDialog dialog

class CTaggingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CTaggingDialog)

public:
	CTaggingDialog(CWnd* pParent = NULL);   // standard constructor
	CTaggingDialog(vector<string> groupAndLedgerList, vector<string> *finalTagListForGroupAndLedger, map<string,vector<string>>& TaggingMappings, map<string, string>& accountToReservedGroupNameMap, CWnd* pParent = NULL);
	virtual ~CTaggingDialog();

// Dialog Data
	enum { IDD = IDD_TAGGING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	string CTaggingDialog::GetParentNameForGroupOrLedger(string groupOrLedgerName);
	void CTaggingDialog::GetAssociatedFathomTagsForReservedGroup(vector<string>& dropDownList,string groupOrLedgerName);

private:
	vector<string> m_groupAndLedgerList;
	vector<string> *m_finalTagListForGroupAndLedger;
	map<string,vector<string>> m_TaggingMappings;
    map<string, string> m_accountToReservedGroupNameMap;
	int m_nLastDisplayedLedgerIndex;

protected:
	int m_nCurHeight;
	int m_nScrollPos;
	CRect m_rect;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnStnClickedStaticTopheadline();
};
