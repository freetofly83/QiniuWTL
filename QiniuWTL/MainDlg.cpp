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
	//Gdiplus 初始化
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//默认将jpg单选按钮选中
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
	// TODO: 在此添加控件通知处理程序代码
	//判断，如果内存中没数据，则返回提示
	if (!::OpenClipboard(m_hWnd))
	{
		::MessageBoxA(NULL, "剪贴板中无内存数据！", "提示", MB_OK);
		::CloseClipboard();
		return 0;
	}
	
	//如果内存中不是图片数据，则返回提示
	HANDLE hBitmap = ::GetClipboardData(CF_BITMAP);
	if (hBitmap == NULL)
	{
		::MessageBoxA(NULL, "剪贴板中无图片数据！", "提示", MB_OK);
		::CloseClipboard();
		return 0;
	}

	HDC   hDC = ::GetDC(GetDlgItem(IDC_PIC));   //   获取设备环境句柄 
	HDC   hdcMem = CreateCompatibleDC(hDC);   //   创建与设备相关的内存环境 
	SelectObject(hdcMem, hBitmap);   //   选择对象 
	SetMapMode(hdcMem, GetMapMode(hDC));   //   设置映射模式 
	BITMAP   bm;   //   得到位图对象 
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	
	//生成guid文件名
	//文件名
	CString strFileName;
	newGuid(&strFileName);
	m_edit_filename.SetWindowTextW(strFileName);
	//判断pics目录是否存在，不存在则创建
	if (GetFileAttributes(L"pics") != FILE_ATTRIBUTE_DIRECTORY)
	{
		CreateDirectory(L"pics", NULL);
	}
	strFileName.Format(L"pics\\%s.bmp", strFileName);
	SaveBitmap(bm, hDC, (HBITMAP)hBitmap, strFileName);
	BmpToJpg(strFileName);

	//开始绘制到屏幕的pic控件上
	CWindow picWnd = GetDlgItem(IDC_PIC);
	CStatic *picSt = (CStatic*)&picWnd;
	picSt->ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);
	RECT picRect;
	picWnd.GetWindowRect(&picRect);
	LONG picWidth = picRect.right - picRect.left;
	LONG picHeight = picRect.bottom - picRect.top;
	//按照比例，绘制到屏幕上
	if (picWidth < bm.bmWidth || picHeight < bm.bmHeight)
	{
		FLOAT drwWidth, drwHeight;//需要绘制的长宽

		//是宽的比例差别大还是高德比例差别大,计算出需要改变图片的宽高
		if (bm.bmWidth / picWidth > bm.bmHeight / picHeight)
		{
			//如果宽的比例比高大，则图片大小宽度改为pic控件的宽度
			drwWidth = picWidth;
			drwHeight = ((FLOAT)picWidth / (FLOAT)bm.bmWidth)*bm.bmHeight;
			
		}
		else
		{	
			drwHeight = picHeight;
			drwWidth = ((FLOAT)picHeight / (FLOAT)bm.bmHeight) * bm.bmWidth;
		}


		//更改图片大小
		HBITMAP hDrwBmp = CreateCompatibleBitmap(hDC, drwWidth, drwHeight);
		HDC hDrwMem = CreateCompatibleDC(hDC);
		SelectObject(hDrwMem, hDrwBmp);   //   选择对象 
		SetMapMode(hDrwMem, GetMapMode(hDC));   //   设置映射模式 

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

	::ReleaseDC(m_hWnd, hDC);   //   释放设备环境句柄 
	DeleteDC(hdcMem);   //   删除内存环境 
	::CloseClipboard();   //   关闭剪贴板

	return 0;
}


//保存到本地函数
BOOL   CMainDlg::SaveBitmap(const   BITMAP   &bm, HDC   hDC, HBITMAP   hBitmap, LPCTSTR   szFileName)
{

	int   nBitPerPixel = bm.bmBitsPixel;//获得颜色模式 
	int   nW = bm.bmWidth;
	int   nH = bm.bmHeight;
	int   nPalItemC = bm.bmPlanes; //调色板项的个数 
	int   nBmpInfSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*nPalItemC;//位图信息的大小 
	int   nDataSize = (nBitPerPixel*nW + 31) / 32 * 4 * nH;//位图数据的大小 
														   //初始化位图信息 
	BITMAPFILEHEADER   bfh = { 0 };//位图文件头 
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + nBmpInfSize;//sizeof(BITMAPINFOHEADER);//到位图数据的偏移量 
	bfh.bfSize = bfh.bfOffBits + nDataSize;//文件总的大小 
	bfh.bfType = (WORD)0x4d42;//位图标志 
	char   *   p = new   char[nBmpInfSize + nDataSize];//申请内存位图数据空间(包括信息头) 
	memset(p, 0, nBmpInfSize);//将信息头的数据初始化为0 
	LPBITMAPINFOHEADER   pBih = (LPBITMAPINFOHEADER)p;//位图信息头 
	pBih->biCompression = BI_RGB;
	pBih->biBitCount = nBitPerPixel;//每个图元像素使用的位数 
	pBih->biHeight = nH;//高度 
	pBih->biWidth = nW;//宽度 
	pBih->biPlanes = 1;
	pBih->biSize = sizeof(BITMAPINFOHEADER);
	pBih->biSizeImage = nDataSize;//图像数据大小 
	char   *pData = p + nBmpInfSize;
	//DDB转换为DIB 
	::GetDIBits(hDC, hBitmap, 0, nH, pData, (LPBITMAPINFO)pBih, DIB_RGB_COLORS);//获取位图数据 
	ofstream   ofs(szFileName, ios::binary);
	if (ofs.fail())
	{
		return   FALSE;
	}
	ofs.write((const   char*)&bfh, sizeof(BITMAPFILEHEADER));//写入位图文件头 
	ofs.write((const   char*)pBih, nBmpInfSize);//写入位图信息数据 
	ofs.write((const   char*)pData, nDataSize);//写入位图数据 
	return   TRUE;
}

//转化BMP文件到JPG
BOOL CMainDlg::BmpToJpg(LPCTSTR fileName)
{
	//设置编码方式
	CLSID clsid;
	GetEncoderClsid(L"image/jpeg", &clsid);
	
	//设置编码参数
	ULONG encoderQuality = 50;                                //压缩比例
	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &encoderQuality;
	
	    //进行图片格式转换
	CString strFN = fileName;
	Bitmap bmBitmap(fileName);        //根据源图片文件构建一个GDI+位图对象
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



// 生成GUID字符串
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


// //上传文件至七牛云
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
	//设置机房域名
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
		/*200, 正确返回了, 你可以通过statRet变量查询一些关于这个文件的信息*/
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


// 将文本内容复制到剪切板中
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
		::SetClipboardData(CF_UNICODETEXT, hData); //多字符集
		::CloseClipboard();
	}
	//HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);
	////  获取字符串    
	//LPWSTR lpStr = (LPWSTR)GlobalLock(hMem);
	//if (lpStr)
	//{
	//	//printf(lpStr);
	//	// 释放内存锁    
	//	GlobalUnlock(hMem);
	//}
	//CloseClipboard();
	return true;
}


LRESULT CMainDlg::OnBnClickedBtnUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString ak, sk, bucket, zone, fileName, qiniuUrl;
	m_edit_filename.GetWindowTextW(fileName);
	//判断有无要上传的文件
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
		//cout << FILENAME << "没有被创建";
		MessageBox(L"没有找到上传文件");
		return 0;
	}
	file.close();
	//读取配置文件
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
		//自动生成文件链接地址
		qiniuUrl.Format(L"%s%s", qiniuUrl, fileName);
		m_edit_adr.SetWindowTextW(qiniuUrl);
		CString markDownUrl;
		markDownUrl.Format(L"![image](%s)", qiniuUrl);
		m_edit_markdown.SetWindowTextW(markDownUrl);
	}
	else
	{
		MessageBox(L"上传失败");
	}

	return 0;
}


LRESULT CMainDlg::OnBnClickedBtnClp2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_edit_markdown.GetWindowTextW(str);
	PasteClp(&str);
	return 0;
}



LRESULT CMainDlg::OnBnClickedBtnClp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_edit_adr.GetWindowTextW(str);
	PasteClp(&str);
	return 0;
}


