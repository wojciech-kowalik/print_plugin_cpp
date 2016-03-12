// EAN13.cpp: implementation of the CEAN13 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EAN13.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEAN13::CEAN13()
{
	m_nSymbology = EAN13;
}

CEAN13::~CEAN13()
{

}


void CEAN13::LoadDatas(CString csMessage, double dNarrowBar, double dFinalHeight, long nGuardbarHeight, HDC hDC, int nStartingXPixel, int nStartingYPixel, double dRatio)
{
	m_nGuardbarHeight = nGuardbarHeight;
	CBarcode::LoadData(csMessage,dNarrowBar,dFinalHeight,hDC,nStartingXPixel,nStartingYPixel,dRatio);
}

long CEAN13::CalculateCheckSumDigit()
{
	int i, nLen, nSum = 0, nItem;
	nLen = m_csMessage.GetLength();

	for (i = nLen ; i >= 1 ; i--){
		nItem = i%2 ? ((int)m_csMessage.GetAt(i-1)-48)*1 : ((int)m_csMessage.GetAt(i-1)-48)*3;
		nSum += nItem;
	}

	nSum %= 10;
	return (10-nSum)%10;
}


CString CEAN13::RetrieveLeftOddParityPattern(int iNumber)
{
	CString strCharPattern;

	switch(iNumber){
		case 0: 
			strCharPattern = "sssbbsb"; 
			break;
		case 1: 
			strCharPattern = "ssbbssb";
			break;
		case 2: 
			strCharPattern = "ssbssbb"; 
			break;
		case 3: 
			strCharPattern = "sbbbbsb"; 
			break; 
		case 4: 
			strCharPattern = "sbsssbb"; 
			break; 
		case 5: 
			strCharPattern = "sbbsssb"; 
			break;
		case 6:
			strCharPattern = "sbsbbbb";
			break;
		case 7:
			strCharPattern = "sbbbsbb"; 
			break;
		case 8:
			strCharPattern = "sbbsbbb";
			break; 
		case 9:
			strCharPattern = "sssbsbb"; 
			break;
	}
	return strCharPattern;
}


CString CEAN13::RetrieveLeftEvenParityPattern(int iNumber)
{
	CString strCharPattern;
	
	switch(iNumber){
		case 0: 
			strCharPattern = "sbssbbb";
			break;
		case 1: 
			strCharPattern = "sbbssbb";
			break; 
		case 2:
			strCharPattern = "ssbbsbb";
			break; 
		case 3: 
			strCharPattern = "sbssssb";
			break;
		case 4:
			strCharPattern = "ssbbbsb";
			break;
		case 5:
			strCharPattern = "sbbbssb";
			break;
		case 6: 
			strCharPattern = "ssssbsb";
			break; 
		case 7: 
			strCharPattern = "ssbsssb";
			break;
		case 8: 
			strCharPattern = "sssbssb"; 
			break;
		case 9:
			strCharPattern = "ssbsbbb";
			break;
	}
	return strCharPattern;
}


CString CEAN13::RetrieveRightPattern(int iNumber)
{
	CString strCharPattern;
	
	switch(iNumber){
		case 0: 
			strCharPattern = "bbbssbs";
			break; 
		case 1:
			strCharPattern = "bbssbbs";
			break; 
		case 2: 
			strCharPattern = "bbsbbss";
			break; 
		case 3:
			strCharPattern = "bssssbs"; 
			break; 
		case 4:
			strCharPattern = "bsbbbss"; 
			break; 
		case 5:
			strCharPattern = "bssbbbs";
			break; 
		case 6: 
			strCharPattern = "bsbssss";
			break; 
		case 7: 
			strCharPattern = "bsssbss";
			break; 
		case 8:
			strCharPattern = "bssbsss";
			break; 
		case 9: 
			strCharPattern = "bbbsbss"; 
			break;
	}

	return strCharPattern;
}


CString CEAN13::RetrieveCountryCodePattern(int iNumber)
{
	CString strCharPattern;
	
	switch(iNumber){
		case 0: 
			strCharPattern = "OOOOO";
			break;  
		case 1:
			strCharPattern = "OEOEE";
			break;  
		case 2: 
			strCharPattern = "OEEOE";
			break;  
		case 3:
			strCharPattern = "OEEEO";
			break;  
		case 4: 
			strCharPattern = "EOOEE"; 
			break;  
		case 5: 
			strCharPattern = "EEOOE";
			break;  
		case 6:
			strCharPattern = "EEEOO";
			break;  
		case 7: 
			strCharPattern = "EOEOE";
			break;  
		case 8: 
			strCharPattern = "EOEEO";
			break;  
		case 9:
			strCharPattern = "EEOEO";
			break;  
	}

	return strCharPattern;
}

void CEAN13::DrawPattern(CString csPattern)
{
	int			i,nXPixel,nYPixel;
	//CDC			oDC;

	// attach to the device context
	//oDC.Attach(m_hDC);

	// initialize X pixel value
	nXPixel = m_nStartingXPixel;
	
	for (i=0;i<csPattern.GetLength();i++)
	{
		// X value for loop
		for (nXPixel=m_nStartingXPixel;nXPixel<m_nStartingXPixel+m_nNarrowBarPixelWidth;nXPixel++)
		{
			// Y value for loop
			for (nYPixel=m_nStartingYPixel;nYPixel<m_nStartingYPixel+m_nPixelHeight+m_nGuardbarHeight;nYPixel++)
			{
				// if this is a bar
				if (csPattern.GetAt(i)=='b')
					SetPixelV(m_hDC, nXPixel,nYPixel,COLORBLACK);
				else
					SetPixelV(m_hDC,nXPixel,nYPixel,COLORWHITE);
			}
		}

		// advance the starting position
		m_nStartingXPixel+= m_nNarrowBarPixelWidth;
	}

	
	return;

}

void CEAN13::DrawBitmap()
{
	int i, tmpGuardBarHeight;
	
	DrawPattern("sssssssss"); // draw quite zone
	
	DrawPattern("bsb"); // draw lead

	CString strCountryCodePattern;
	strCountryCodePattern = RetrieveCountryCodePattern((int)m_csMessage.GetAt(0)-48);

	tmpGuardBarHeight = m_nGuardbarHeight;
	m_nGuardbarHeight = 0;

	
	DrawPattern(RetrieveLeftOddParityPattern((int)m_csMessage.GetAt(1)-48));

	
	for (i = 2 ; i < 7 ; i ++){
		if (strCountryCodePattern[i-2] == 'O')
			DrawPattern(RetrieveLeftOddParityPattern((int)m_csMessage.GetAt(i)-48));

		if (strCountryCodePattern[i-2] == 'E')
			DrawPattern(RetrieveLeftEvenParityPattern((int)m_csMessage.GetAt(i)-48));
	}

	m_nGuardbarHeight = tmpGuardBarHeight;

	DrawPattern("sbsbs"); // draw separator bars

	tmpGuardBarHeight = m_nGuardbarHeight;
	m_nGuardbarHeight = 0;
	for (i = 7 ; i < 12 ; i ++)
		DrawPattern(RetrieveRightPattern((int)m_csMessage.GetAt(i)-48));


	DrawPattern(RetrieveRightPattern(CalculateCheckSumDigit()));
	m_nGuardbarHeight = tmpGuardBarHeight;
	
	DrawPattern("bsb"); // draw trailer bars
	DrawPattern("sssssssss"); // draw quite zone 

}

void CEAN13::BitmapToClipboard()
{
}

void CEAN13::test()
{
}
