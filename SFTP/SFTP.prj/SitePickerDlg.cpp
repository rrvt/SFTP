// Site Picker Dialog


#include "pch.h"
#include "SitePickerDlg.h"
#include "IniFile.h"


// SitePickerDlg dialog

IMPLEMENT_DYNAMIC(SitePickerDlg, CDialogEx)

SitePickerDlg::SitePickerDlg(CWnd* pParent) : CDialogEx(IDD_SitePicker, pParent), section(_T("")) { }

SitePickerDlg::~SitePickerDlg() { }



BOOL SitePickerDlg::OnInitDialog() {
IniSectIter iter(iniFile);
TCchar*     sect;

  CDialogEx::OnInitDialog();

  for (sect = iter(); sect; sect = iter++) {

    String s = sect;   if (s.find(_T(" Site")) < 0) continue;

    listCtrl.AddString(sect);
    }

  listCtrl.SetCurSel(0);   return TRUE;
  }


void SitePickerDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX,  IDC_SiteList, listCtrl);
  DDX_CBString(pDX, IDC_SiteList, section);
  }


BEGIN_MESSAGE_MAP(SitePickerDlg, CDialogEx)
END_MESSAGE_MAP()
