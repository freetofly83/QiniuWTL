// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include <base.h>
#include <rs.h>
#include <io.h>
#include <fstream>
#include <GdiPlus.h>
#include "log.h"

#define ULONG_PTR ULONG

#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

using namespace std;



LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();
	
	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);


	DoDataExchange();
	//Gdiplus ��ʼ��
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//Ĭ�Ͻ�jpg��ѡ��ťѡ��
	CheckRadioButton(IDC_RADIO_JPG, IDC_RADIO_BMP, IDC_RADIO_JPG);

	return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}


LRESULT CMainDlg::OnBnClickedBtnReadpic(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�жϣ�����ڴ���û���ݣ��򷵻���ʾ
	if (!::OpenClipboard(m_hWnd))
	{
		::MessageBoxA(NULL, "�����������ڴ����ݣ�", "��ʾ", MB_OK);
		::CloseClipboard();
		return 0;
	}
	
	//����ڴ��в���ͼƬ���ݣ��򷵻���ʾ
	HANDLE hBitmap = ::GetClipboardData(CF_BITMAP);
	if (hBitmap == NULL)
	{
		::MessageBoxA(NULL, "����������ͼƬ���ݣ�", "��ʾ", MB_OK);
		::CloseClipboard();
		return 0;
	}

	HDC   hDC = ::GetDC(GetDlgItem(IDC_PIC));   //   ��ȡ�豸������� 
	HDC   hdcMem = CreateCompatibleDC(hDC);   //   �������豸��ص��ڴ滷�� 
	SelectObject(hdcMem, hBitmap);   //   ѡ����� 
	SetMapMode(hdcMem, GetMapMode(hDC));   //   ����ӳ��ģʽ 
	BITMAP   bm;   //   �õ�λͼ���� 
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	
	//����guid�ļ���
	//�ļ���
	CString strFileName;
	newGuid(&strFileName);
	m_edit_filename.SetWindowTextW(strFileName);
	//�ж�picsĿ¼�Ƿ���ڣ��������򴴽�
	if (GetFileAttributes(L"pics") != FILE_ATTRIBUTE_DIRECTORY)
	{
		CreateDirectory(L"pics", NULL);
	}
	strFileName.Format(L"pics\\%s.bmp", strFileName);
	SaveBitmap(bm, hDC, (HBITMAP)hBitmap, strFileName);
	BmpToJpg(strFileName);

	//��ʼ���Ƶ���Ļ��pic�ؼ���
	CWindow picWnd = GetDlgItem(IDC_PIC);
	CStatic *picSt = (CStatic*)&picWnd;
	picSt->ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);
	RECT picRect;
	picWnd.GetWindowRect(&picRect);
	LONG picWidth = picRect.right - picRect.left;
	LONG picHeight = picRect.bottom - picRect.top;
	//���ձ��������Ƶ���Ļ��
	if (picWidth < bm.bmWidth || picHeight < bm.bmHeight)
	{
		FLOAT drwWidth, drwHeight;//��Ҫ���Ƶĳ���

		//�ǿ�ı��������Ǹߵ±�������,�������Ҫ�ı�ͼƬ�Ŀ��
		if (bm.bmWidth / picWidth > bm.bmHeight / picHeight)
		{
			//�����ı����ȸߴ���ͼƬ��С��ȸ�Ϊpic�ؼ��Ŀ��
			drwWidth = picWidth;
			drwHeight = ((FLOAT)picWidth / (FLOAT)bm.bmWidth)*bm.bmHeight;
			
		}
		else
		{	
			drwHeight = picHeight;
			drwWidth = ((FLOAT)picHeight / (FLOAT)bm.bmHeight) * bm.bmWidth;
		}


		//����ͼƬ��С
		HBITMAP hDrwBmp = CreateCompatibleBitmap(hDC, drwWidth, drwHeight);
		HDC hDrwMem = CreateCompatibleDC(hDC);
		SelectObject(hDrwMem, hDrwBmp);   //   ѡ����� 
		SetMapMode(hDrwMem, GetMapMode(hDC));   //   ����ӳ��ģʽ 

		StretchBlt(hDrwMem, 0, 0, drwWidth, drwHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

		picSt->SetBitmap((HBITMAP)hDrwBmp);
		DeleteDC(hDrwMem);
	}
	else
	{
		
		//picDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		picSt->SetBitmap((HBITMAP)hBitmap);
		
	}
	picSt->Invalidate(FALSE);

	//picDC.BitBlt(0, 0, 300, 300, hdcMem, 0, 0, SRCCOPY);
	//picDC.StretchBlt(0, 0, 300, 300, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	::ReleaseDC(m_hWnd, hDC);   //   �ͷ��豸������� 
	DeleteDC(hdcMem);   //   ɾ���ڴ滷�� 
	::CloseClipboard();   //   �رռ�����

	return 0;
}


//���浽���غ���
BOOL   CMainDlg::SaveBitmap(const   BITMAP   &bm, HDC   hDC, HBITMAP   hBitmap, LPCTSTR   szFileName)
{

	int   nBitPerPixel = bm.bmBitsPixel;//�����ɫģʽ 
	int   nW = bm.bmWidth;
	int   nH = bm.bmHeight;
	int   nPalItemC = bm.bmPlanes; //��ɫ����ĸ��� 
	int   nBmpInfSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*nPalItemC;//λͼ��Ϣ�Ĵ�С 
	int   nDataSize = (nBitPerPixel*nW + 31) / 32 * 4 * nH;//λͼ���ݵĴ�С 
														   //��ʼ��λͼ��Ϣ 
	BITMAPFILEHEADER   bfh = { 0 };//λͼ�ļ�ͷ 
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + nBmpInfSize;//sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ���� 
	bfh.bfSize = bfh.bfOffBits + nDataSize;//�ļ��ܵĴ�С 
	bfh.bfType = (WORD)0x4d42;//λͼ��־ 
	char   *   p = new   char[nBmpInfSize + nDataSize];//�����ڴ�λͼ���ݿռ�(������Ϣͷ) 
	memset(p, 0, nBmpInfSize);//����Ϣͷ�����ݳ�ʼ��Ϊ0 
	LPBITMAPINFOHEADER   pBih = (LPBITMAPINFOHEADER)p;//λͼ��Ϣͷ 
	pBih->biCompression = BI_RGB;
	pBih->biBitCount = nBitPerPixel;//ÿ��ͼԪ����ʹ�õ�λ�� 
	pBih->biHeight = nH;//�߶� 
	pBih->biWidth = nW;//��� 
	pBih->biPlanes = 1;
	pBih->biSize = sizeof(BITMAPINFOHEADER);
	pBih->biSizeImage = nDataSize;//ͼ�����ݴ�С 
	char   *pData = p + nBmpInfSize;
	//DDBת��ΪDIB 
	::GetDIBits(hDC, hBitmap, 0, nH, pData, (LPBITMAPINFO)pBih, DIB_RGB_COLORS);//��ȡλͼ���� 
	ofstream   ofs(szFileName, ios::binary);
	if (ofs.fail())
	{
		return   FALSE;
	}
	ofs.write((const   char*)&bfh, sizeof(BITMAPFILEHEADER));//д��λͼ�ļ�ͷ 
	ofs.write((const   char*)pBih, nBmpInfSize);//д��λͼ��Ϣ���� 
	ofs.write((const   char*)pData, nDataSize);//д��λͼ���� 
	return   TRUE;
}

//ת��BMP�ļ���JPG
BOOL CMainDlg::BmpToJpg(LPCTSTR fileName)
{
	//���ñ��뷽ʽ
	CLSID clsid;
	GetEncoderClsid(L"image/jpeg", &clsid);
	
	//���ñ������
	ULONG encoderQuality = 50;                                //ѹ������
	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &encoderQuality;
	
	    //����ͼƬ��ʽת��
	CString strFN = fileName;
	Bitmap bmBitmap(fileName);        //����ԴͼƬ�ļ�����һ��GDI+λͼ����
	CString strSaveSaveFilePath = strFN.Left(strFN.GetLength() - 3);
	strSaveSaveFilePath += "jpg";
	Status status = bmBitmap.Save(strSaveSaveFilePath.AllocSysString(), &clsid, &encoderParameters);

	return TRUE;
}

int   CMainDlg::GetEncoderClsid(const   WCHAR*   format, CLSID*   pClsid)
{
	UINT num = 0;                     //   number   of   image   encoders   
	UINT size = 0;                   //   size   of   the   image   encoder   array   in   bytes   
	ImageCodecInfo*  pImageCodecInfo = NULL;
	::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return   -1;     //   Failure   
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return   -1;     //   Failure   
	::GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return   j;     //   Success   
		}
	}
	free(pImageCodecInfo);
	return   -1;     //   Failure   
}



// ����GUID�ַ���
void CMainDlg::newGuid(CString *strGuid)
{
	//CString strGuid = "";
		
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		//strGuid.Format(L"%sx%sx%sx%sx%sx%sx%sx%sx", guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		strGuid->Format(L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
	}
	//return (LPCTSTR)strGuid;
}


// //�ϴ��ļ�����ţ��
bool CMainDlg::UpdateQiniu(LPCTSTR fileName, LPCTSTR ak, LPCTSTR sk, LPCTSTR bucket, LPCTSTR zone)
{
	Qiniu_Global_Init(-1);
	Qiniu_Mac mac;

	CStringA strTempAk(ak);
	
	mac.accessKey = strTempAk;
	CStringA strTempSk(sk);
	mac.secretKey = strTempSk;
	CStringA strBucket(bucket);
	CStringA key(fileName);
	CStringA localFile("pics\\");
	localFile.Append(key);
	Qiniu_Io_PutRet putRet;
	Qiniu_Client client;
	Qiniu_RS_PutPolicy putPolicy;
	Qiniu_Io_PutExtra putExtra;
	Qiniu_Zero(putPolicy);
	Qiniu_Zero(putExtra);
	putPolicy.scope = strBucket;
	char *uptoken = Qiniu_RS_PutPolicy_Token(&putPolicy, &mac);
	//���û�������
	if (zone == L"beimei" || zone == L"Beimei")
	{
		Qiniu_Use_Zone_Beimei(Qiniu_True);
	}
	else if (zone == L"huabei" || zone == L"Huabei")
	{
		Qiniu_Use_Zone_Huabei(Qiniu_True);
	}
	else if (zone == L"huadong" || zone == L"Huadong")
	{
		Qiniu_Use_Zone_Huadong(Qiniu_True);
	}
	else if (zone == L"huanan" || zone == L"Huanan")
	{
		Qiniu_Use_Zone_Huanan(Qiniu_True);
	}
	else
	{
		Qiniu_Use_Zone_Huabei(Qiniu_True);
	}
	
	Qiniu_Client_InitMacAuth(&client, 1024, &mac);
	Qiniu_Error error = Qiniu_Io_PutFile(&client, &putRet, uptoken, key, localFile, &putExtra);

	bool ret = false;
	CStringA tmp(fileName);
	if (error.code != 200) {
		//printf("upload file %s:%s error.\n", strBucket, key);
		//debug_log(&client, error);
		Write_log("Err filename:%s,code:%s, msg:%s", tmp, error.code, error.message);
		
	}
	else {
		/*200, ��ȷ������, �����ͨ��statRet������ѯһЩ��������ļ�����Ϣ*/
		/*printf("upload file %s:%s success.\n\n", strBucket, key);
		printf("key:\t%s\n", putRet.key);
		printf("hash:\t%s\n", putRet.hash);*/
		ret = true;
		
		Write_log("Suss filename:%s", tmp);
	}
	Qiniu_Free(uptoken);
	Qiniu_Client_Cleanup(&client);
	return ret;
}


// ���ı����ݸ��Ƶ����а���
bool CMainDlg::PasteClp(CString *str)
{
	if (::OpenClipboard(m_hWnd))
	{
		::EmptyClipboard();

		HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, (str->GetLength() + 1) * sizeof(TCHAR));
		LPWSTR pData = (LPWSTR)::GlobalLock(hData);
		::lstrcpy(pData, *str);
		::GlobalUnlock(hData);
		//CF_UNICODETEXT
		::SetClipboardData(CF_UNICODETEXT, hData); //���ַ���
		::CloseClipboard();
	}
	//HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
	////  ��ȡ�ַ���    
	//LPWSTR lpStr = (LPWSTR)GlobalLock(hMem);
	//if (lpStr)
	//{
	//	//printf(lpStr);
	//	// �ͷ��ڴ���    
	//	GlobalUnlock(hMem);
	//}
	//CloseClipboard();
	return true;
}


LRESULT CMainDlg::OnBnClickedBtnUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString ak, sk, bucket, zone, fileName, qiniuUrl;
	m_edit_filename.GetWindowTextW(fileName);
	//�ж�����Ҫ�ϴ����ļ�
	fstream file;
	CString fileFullName;
	if (((CButton)GetDlgItem(IDC_RADIO_JPG)).GetCheck())
	{
		fileFullName = "pics\\" + fileName + ".jpg";
	}
	else
	{
		fileFullName = "pics\\" + fileName + ".bmp";
	}
	file.open(fileFullName, ios::in);
	if (!file)
	{
		//cout << FILENAME << "û�б�����";
		MessageBox(L"û���ҵ��ϴ��ļ�");
		return 0;
	}
	file.close();
	//��ȡ�����ļ�
	GetPrivateProfileString(L"Qiniu", L"Ak", L"", (LPTSTR)ak.GetBuffer(256), 256, L".\\configure.ini");
	GetPrivateProfileString(L"Qiniu", L"Sk", L"", (LPTSTR)sk.GetBuffer(256), 256, L".\\configure.ini");
	GetPrivateProfileString(L"Qiniu", L"Bucket", L"", (LPTSTR)bucket.GetBuffer(256), 256, L".\\configure.ini");
	GetPrivateProfileString(L"Qiniu", L"Zone", L"", (LPTSTR)zone.GetBuffer(256), 256, L".\\configure.ini");
	GetPrivateProfileString(L"Qiniu", L"QiniuUrl", L"", (LPTSTR)qiniuUrl.GetBuffer(256), 256, L".\\configure.ini");

	

	
	if(((CButton)GetDlgItem(IDC_RADIO_JPG)).GetCheck())
	{
		fileName.Append(L".jpg");
	}
	else
	{
		fileName.Append(L".bmp");
	}

	
	if (UpdateQiniu(fileName, ak, sk, bucket, zone))
	{
		//�Զ������ļ����ӵ�ַ
		qiniuUrl.Format(L"%s%s", qiniuUrl, fileName);
		m_edit_adr.SetWindowTextW(qiniuUrl);
		CString markDownUrl;
		markDownUrl.Format(L"![image](%s)", qiniuUrl);
		m_edit_markdown.SetWindowTextW(markDownUrl);
	}
	else
	{
		MessageBox(L"�ϴ�ʧ��");
	}

	return 0;
}


LRESULT CMainDlg::OnBnClickedBtnClp2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_edit_markdown.GetWindowTextW(str);
	PasteClp(&str);
	return 0;
}



LRESULT CMainDlg::OnBnClickedBtnClp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_edit_adr.GetWindowTextW(str);
	PasteClp(&str);
	return 0;
}


