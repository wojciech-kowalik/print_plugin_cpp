// EAN13.h: interface for the CEAN13 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EAN13_H__B9A50141_4220_4B6C_9495_AE8F312AAA42__INCLUDED_)
#define AFX_EAN13_H__B9A50141_4220_4B6C_9495_AE8F312AAA42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Barcode.h"

class CEAN13 : public CBarcode  
{
public:
	CEAN13();
	virtual ~CEAN13();
	void LoadDatas(CString csMessage, double dNarrowBar, double dFinalHeight, long nGuardbarHeight, HDC hDC, int nStartingXPixel, int nStartingYPixel, double dRatio);
	void DrawBitmap();
	void BitmapToClipboard();
	long CalculateCheckSumDigit();
	void test();

private:
	long  m_nGuardbarHeight;
	CString RetrieveLeftOddParityPattern(int iNumber);
	CString RetrieveLeftEvenParityPattern(int iNumber);
	CString RetrieveRightPattern(int iNumber);
	CString RetrieveCountryCodePattern(int iNumber);

	void DrawPattern(CString csPattern);

};

#endif // !defined(AFX_EAN13_H__B9A50141_4220_4B6C_9495_AE8F312AAA42__INCLUDED_)
