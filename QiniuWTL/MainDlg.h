// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainDlg : public CDialogImpl<CMainDlg>,
	public CWinDataExchange<CMainDlg>
{
protected:
	CEdit m_edit_filename;
	CEdit m_edit_adr;
	CEdit m_edit_markdown;

public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_BTN_ReadPIC, BN_CLICKED, OnBnClickedBtnReadpic)
		COMMAND_HANDLER(ID_BTN_Update, BN_CLICKED, OnBnClickedBtnUpdate)
		COMMAND_HANDLER(IDC_BTN_CLP2, BN_CLICKED, OnBnClickedBtnClp2)
		COMMAND_HANDLER(IDC_BTN_CLP, BN_CLICKED, OnBnClickedBtnClp)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CMainDlg)
		DDX_CONTROL_HANDLE(IDC_EDIT_FileName, m_edit_filename)
		DDX_CONTROL_HANDLE(IDC_EDIT_Adr, m_edit_adr)
		DDX_CONTROL_HANDLE(IDC_EDIT_MARKDOWN, m_edit_markdown)
	END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnReadpic(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	BOOL SaveBitmap(const   BITMAP   &bm, HDC   hDC, HBITMAP   hBitmap, LPCTSTR   szFileName);
	BOOL BmpToJpg(LPCTSTR fileName);
	int GetEncoderClsid(const   WCHAR*   format, CLSID*   pClsid);
	
	// 生成GUID字符串
	void newGuid(CString *strGuid);



	// //上传文件至七牛云
	bool UpdateQiniu(LPCTSTR fileName, LPCTSTR ak, LPCTSTR sk, LPCTSTR bucket, LPCTSTR Zone);
	LRESULT OnBnClickedBtnUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnClp2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	// 将文本内容复制到剪切板中
	bool PasteClp(CString *str);
	LRESULT OnBnClickedBtnClp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
