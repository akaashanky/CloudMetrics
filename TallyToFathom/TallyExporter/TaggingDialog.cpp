// TaggingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TallyExporter.h"
#include "TaggingDialog.h"
#include "afxdialogex.h"
#include "TallyExporterDlg.h"


// CTaggingDialog dialog

IMPLEMENT_DYNAMIC(CTaggingDialog, CDialogEx)

CTaggingDialog::CTaggingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTaggingDialog::IDD, pParent)
{

}

CTaggingDialog::CTaggingDialog(vector<string> groupAndLedgerList, vector<string> *finalTagListForGroupAndLedger, map<string,vector<string>>& taggingMappings, map<string, string>& accountToReservedGroupNameMap, CWnd* pParent)
	: CDialogEx(CTaggingDialog::IDD, pParent)
{
	m_groupAndLedgerList = groupAndLedgerList;
	m_TaggingMappings = taggingMappings;
	m_accountToReservedGroupNameMap = accountToReservedGroupNameMap;
	m_nLastDisplayedLedgerIndex = 0;
	m_finalTagListForGroupAndLedger = finalTagListForGroupAndLedger;
}

CTaggingDialog::~CTaggingDialog()
{
}

void CTaggingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTaggingDialog, CDialogEx)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK, &CTaggingDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTaggingDialog::OnBnClickedCancel)
	ON_STN_CLICKED(IDC_STATIC_TOPHEADLINE, &CTaggingDialog::OnStnClickedStaticTopheadline)
END_MESSAGE_MAP()


// CTaggingDialog message handlers

//Render with 8 fields first
BOOL CTaggingDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//Set font
	CFont font;
	LOGFONT logFont;
	GetDlgItem(IDC_STATIC_TOPHEADLINE)->GetFont()->GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;
	font.CreateFontIndirect(&logFont);
	GetDlgItem(IDC_STATIC_TOPHEADLINE)->SetFont(&font,TRUE);

	CString strTopHeadingMsg; 
	strTopHeadingMsg = "MAPPING " +  CString(to_string(m_groupAndLedgerList.size()).c_str()) +  " GROUPS AND LEDGERS";
	SetDlgItemText(IDC_STATIC_TOPHEADLINE, strTopHeadingMsg);

	int noOfColumnsOnThisPage = m_groupAndLedgerList.size();
	if(noOfColumnsOnThisPage > MAX_NOOF_COLUMN_IN_ONE_SHEET)
	{
		noOfColumnsOnThisPage = MAX_NOOF_COLUMN_IN_ONE_SHEET;
	}
	for(int i = 0; i < noOfColumnsOnThisPage ; i++)
	{
		SetDlgItemText(10000 + i + 1, (LPCTSTR)m_groupAndLedgerList.at(i).c_str());
		SetDlgItemText(20000 + i + 1, (LPCTSTR)GetParentNameForGroupOrLedger(m_groupAndLedgerList.at(i)).c_str());
		vector<string> dropDownList;
		vector<string>& dropDownListRef = dropDownList;
		GetAssociatedFathomTagsForReservedGroup(dropDownListRef, m_groupAndLedgerList.at(i));
		for(int j = 0; j < dropDownList.size(); j++)
		{
			((CComboBox*)GetDlgItem(30000 + i + 1))->AddString((LPCTSTR)dropDownList.at(j).c_str());
		}
		//Make them visible
		GetDlgItem(10000 + i + 1)->ShowWindow(1);
		GetDlgItem(20000 + i + 1)->ShowWindow(1);
		GetDlgItem(30000 + i + 1)->ShowWindow(1);		
	}

	return true;
}

/* ===================================================================
*          OnBnClickedOk - Here we ve to decide if all options
*          exhausted, we decide to finally do "onok"
*  ===================================================================
*/
void CTaggingDialog::OnBnClickedOk()
{
	//First check if all are selected
	for(int i = 0; i < MAX_NOOF_COLUMN_IN_ONE_SHEET; i++)
	{
		CString tagName;
	    CString& tagNameRef = tagName;
		GetDlgItemText(30000 + i + 1, tagNameRef);
		if(tagName == "" && GetDlgItem(30000 + i + 1)->IsWindowVisible())
			return;
	}

	//If all selected, populate the final taglist
	for(int i = 0; i < MAX_NOOF_COLUMN_IN_ONE_SHEET; i++)
	{
		CString tagName;
	    CString& tagNameRef = tagName;
		GetDlgItemText(30000 + i + 1, tagNameRef);
		if(GetDlgItem(30000 + i + 1)->IsWindowVisible())
		{
			string tagNameStr = (LPCTSTR)tagName;
			std::size_t found = tagNameStr.find("(",0);
			if (found!=std::string::npos)
			{
				m_finalTagListForGroupAndLedger->push_back(tagNameStr.substr(found+1, tagNameStr.length() - found - 2));
			}			 
		}
	}

	//Add 7 to the last index. If it still exceeds, just add 7. Else call onok. We are done.
	//Else we ve to go another screen.
    if(m_nLastDisplayedLedgerIndex + MAX_NOOF_COLUMN_IN_ONE_SHEET < m_groupAndLedgerList.size())
	{
		if(m_nLastDisplayedLedgerIndex == 0)
		   m_nLastDisplayedLedgerIndex += MAX_NOOF_COLUMN_IN_ONE_SHEET - 1;
		else
			m_nLastDisplayedLedgerIndex += MAX_NOOF_COLUMN_IN_ONE_SHEET;

		//Hide all controls and clear the combos
		for(int i = 0; i < MAX_NOOF_COLUMN_IN_ONE_SHEET; i++)
		{
			GetDlgItem(10000 + i + 1)->ShowWindow(0);
			GetDlgItem(20000 + i + 1)->ShowWindow(0);
			GetDlgItem(30000 + i + 1)->ShowWindow(0);
			((CComboBox*)GetDlgItem(30000 + i + 1))->ResetContent();
		}

		//Real confusion/action begins now.
		int noOfColumnsOnThisPage = MAX_NOOF_COLUMN_IN_ONE_SHEET;
		if(m_groupAndLedgerList.size() - m_nLastDisplayedLedgerIndex - 1 < MAX_NOOF_COLUMN_IN_ONE_SHEET)
		{
			noOfColumnsOnThisPage = m_groupAndLedgerList.size() - m_nLastDisplayedLedgerIndex - 1;
		}
		for(int i = 0; i < noOfColumnsOnThisPage ; i++)
		{
			SetDlgItemText(10000 + i + 1, (LPCTSTR)m_groupAndLedgerList.at(m_nLastDisplayedLedgerIndex + i + 1).c_str());
			SetDlgItemText(20000 + i + 1, (LPCTSTR)GetParentNameForGroupOrLedger(m_groupAndLedgerList.at(m_nLastDisplayedLedgerIndex + i + 1)).c_str());
			vector<string> dropDownList;
		    vector<string>& dropDownListRef = dropDownList;
			GetAssociatedFathomTagsForReservedGroup(dropDownListRef,m_groupAndLedgerList.at(m_nLastDisplayedLedgerIndex + i + 1));
			for(int j = 0; j < dropDownList.size(); j++)
			{
				((CComboBox*)GetDlgItem(30000 + i + 1))->AddString((LPCTSTR)dropDownList.at(j).c_str());
			}
			//Make them visible
			GetDlgItem(10000 + i + 1)->ShowWindow(1);
			GetDlgItem(20000 + i + 1)->ShowWindow(1);
			GetDlgItem(30000 + i + 1)->ShowWindow(1);
		
		}
	}
	else
	{
	    CDialogEx::OnOK(); //we are good to go!
	}
}

/* =====================================
 *   GetParentNameForGroupOrLedger
 * =====================================
 */
string CTaggingDialog::GetParentNameForGroupOrLedger(string groupOrLedgerName)
{
	return m_accountToReservedGroupNameMap.find(groupOrLedgerName)->second;//should exist. No need to check for else
}

/* ===========================================
*   GetAssociatedFathomTagsForReservedGroup
*  ===========================================
*/
void CTaggingDialog::GetAssociatedFathomTagsForReservedGroup(vector<string>& dropDownList,string groupOrLedgerName)
{
	string reservedName = m_accountToReservedGroupNameMap.find(groupOrLedgerName)->second;
	if( m_TaggingMappings.find(reservedName) != m_TaggingMappings.end())
	{
		dropDownList = m_TaggingMappings.find(reservedName)->second;
	}
	else
	{
		dropDownList = m_TaggingMappings.find("")->second;
	}
}





void CTaggingDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here.
	m_nCurHeight = cy;
	int nScrollMax;
	if (cy < m_rect.Height())
	{
	     nScrollMax = m_rect.Height() - cy;
	}
	else
	     nScrollMax = 0;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nMin = 0;
	si.nMax = nScrollMax;
	si.nPage = si.nMax/10;
	si.nPos = 0;
        SetScrollInfo(SB_VERT, &si, TRUE); 
}


void CTaggingDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default.
	int nDelta;
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/100,nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/100,m_nScrollPos);
		break;

         case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;
	
         default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
	ScrollWindow(0,-nDelta);
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CTaggingDialog::OnBnClickedCancel()
{
	m_finalTagListForGroupAndLedger->clear();
	CDialogEx::OnCancel();
}


void CTaggingDialog::OnStnClickedStaticTopheadline()
{
}
