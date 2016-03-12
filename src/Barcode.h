// Barcode.h: interface for the CBarcode class.
//
//	Copyright 2002 Neil Van Eps
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BARCODE_H__C5D7FCDA_5C8F_4244_AF95_33D6FA93F8EB__INCLUDED_)
#define AFX_BARCODE_H__C5D7FCDA_5C8F_4244_AF95_33D6FA93F8EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum Symbology
{
   RATIONALCODABAR,
   TRADITIONALCODABAR,
   I2OF5,
   CODE39,
   COD128,
   UPCEAN,
   CODE93,
   EAN8,	// new symbology
   EAN13	// new symbology
};

#define		COLORWHITE	0x00FFFFFF
#define		COLORBLACK	0x00000000

class CBarcode  
{
public:
	CBarcode();
	void	LoadData(CString csMessage, double dNarrowBar, double dFinalHeight, HDC pDC, int nStartingXPixel, int nStartingYPixel, double dRatio = 1.0);
	virtual void DrawBitmap() = 0;
	virtual void BitmapToClipboard() = 0;
	virtual ~CBarcode();
	long	GetBarcodePixelWidth();
	long	GetBarcodePixelHeight();

protected:
	CString m_csMessage;
	HDC		m_hDC;
	long	m_nFinalBarcodePixelWidth;
	long	m_nNarrowBarPixelWidth;
	long	m_nPixelHeight;
	long	m_nStartingXPixel;
	long	m_nStartingYPixel;
	long	m_nSymbology;
	long	m_nWideBarPixelWidth;
	virtual void DrawPattern(CString csPattern) = 0;
};

#endif // !defined(AFX_BARCODE_H__C5D7FCDA_5C8F_4244_AF95_33D6FA93F8EB__INCLUDED_)
