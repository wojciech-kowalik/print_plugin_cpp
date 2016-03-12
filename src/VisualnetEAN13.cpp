#include "VisualnetEAN13.h"

///////////////////////////////////////////////////////////////////////////////
/// @brief Populate parity table
///////////////////////////////////////////////////////////////////////////////
char VisualnetEAN13::parityTable[10][6] = {

	{0, 0, 0, 0, 0, 0},
	{0, 0, 1, 0, 1, 1},
	{0, 0, 1, 1, 0, 1},
	{0, 0, 1, 1, 1, 0},
	{0, 1, 0, 0, 1, 1},
	{0, 1, 1, 0, 0, 1},
	{0, 1, 1, 1, 0, 0},
	{0, 1, 0, 1, 0, 1},
	{0, 1, 0, 1, 1, 0},
	{0, 1, 1, 0, 1, 0}

};

///////////////////////////////////////////////////////////////////////////////
/// @brief Populate right side data
///////////////////////////////////////////////////////////////////////////////
char VisualnetEAN13::right[10][7] = {

	{1, 1, 1, 0, 0, 1, 0},
	{1, 1, 0, 0, 1, 1, 0},
	{1, 1, 0, 1, 1, 0, 0},
	{1, 0, 0, 0, 0, 1, 0},
	{1, 0, 1, 1, 1, 0, 0},
	{1, 0, 0, 1, 1, 1, 0},
	{1, 0, 1, 0, 0, 0, 0},
	{1, 0, 0, 0, 1, 0, 0},
	{1, 0, 0, 1, 0, 0, 0},
	{1, 1, 1, 0, 1, 0, 0}

};

///////////////////////////////////////////////////////////////////////////////
/// @brief Populate left side data
///////////////////////////////////////////////////////////////////////////////
char VisualnetEAN13::left[2][10][7] = {

	{
		{0, 0, 0, 1, 1, 0, 1},
		{0, 0, 1, 1, 0, 0, 1},
		{0, 0, 1, 0, 0, 1, 1},
		{0, 1, 1, 1, 1, 0, 1},
		{0, 1, 0, 0, 0, 1, 1},
		{0, 1, 1, 0, 0, 0, 1},
		{0, 1, 0, 1, 1, 1, 1},
		{0, 1, 1, 1, 0, 1, 1},
		{0, 1, 1, 0, 1, 1, 1},
		{0, 0, 0, 1, 0, 1, 1}
	},

	{
		{0, 1, 0, 0, 1, 1, 1},
		{0, 1, 1, 0, 0, 1, 1},
		{0, 0, 1, 1, 0, 1, 1},
		{0, 1, 0, 0, 0, 0, 1},
		{0, 0, 1, 1, 1, 0, 1},
		{0, 1, 1, 1, 0, 0 ,1},
		{0, 0, 0, 0, 1, 0, 1},
		{0, 0, 1, 0, 0, 0 ,1},
		{0, 0, 0, 1, 0, 0, 1},
		{0, 0, 1, 0, 1, 1, 1}
	}
};

///////////////////////////////////////////////////////////////////////////////
/// @fn VisualnetEAN13::VisualnetEAN13()
///
/// @brief Default constructor
///////////////////////////////////////////////////////////////////////////////
VisualnetEAN13::VisualnetEAN13(){}

///////////////////////////////////////////////////////////////////////////////
/// @fn VisualnetEAN13::~VisualnetEAN13()
///
/// @brief Default destructor
///////////////////////////////////////////////////////////////////////////////
VisualnetEAN13::~VisualnetEAN13(){}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetEAN13::check()
///
/// @brief Check if EAN13 is right value
///////////////////////////////////////////////////////////////////////////////
bool VisualnetEAN13::check(){

	int sum = 0;
	int sumEven = 0;
	int sumUneven = 0;
	int i;

	for (i=1; i<12; i+=2) {
		sumUneven += EAN13[i];
	}
	for (i=0; i<12; i+=2) {
		sumEven += EAN13[i];
	}
	sum = sumUneven*3 + sumEven;
	sum %= 10;
	sum = 10 - sum;
	sum %= 10;

	if (EAN13[12] == sum) {
		return true;
	}
	else {
		return false;
	}

}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetEAN13::make(std::string code, int x, int y, bool sentinel, HDC &hdc)
///
/// @brief Make EAN13 barcode
///////////////////////////////////////////////////////////////////////////////
bool VisualnetEAN13::make(std::string code, int x, int y, bool sentinel, HDC &hdc){

	int i, j;

	// check if EAN13 has 13 characters
	if (code.size() != 13) {
		throw std::runtime_error("EAN13 must have 13 digits");
	}

	// convert simple string value to CString representation
	CString codeConverted(code.c_str());	

	// get characters from code
	for (i=0; i<13; i++) {
		EAN13[i] = (int)codeConverted.GetAt(i)-48;
	}

	// initialize bars array
	for (i=0; i<95; i++) {
		bars[i] = 0;
	}

	if(check() == false){
		throw std::runtime_error("Wrong EAN13 number");
	}

	bars[0] = 1;
	bars[1] = 0;
	bars[2] = 1;

	for (i=1; i<7; i++) {
		for (j=0; j<7; j++) {
			bars[(i-1)*7 + 3 + j] = left[parityTable[EAN13[0]][i-1]][EAN13[i]][j];
		}
	}

	bars[45] = 0;
	bars[46] = 1;
	bars[47] = 0;
	bars[48] = 1;
	bars[49] = 0;

	for (i=7; i<13; i++) {
		for (j=0; j<7; j++) {
			bars[(i-7)*7 + 50 + j] = right[EAN13[i]][j];
		}
	}

	bars[92] = 1;
	bars[93] = 0;
	bars[94] = 1;

	int length;

	for (i=0; i<95; i++) {

		// check if sentinels have to be showed
		if(sentinel){

			if ((i >= 0 && i<= 2) ||
				(i >= 45 && i<= 49) ||
				(i >= 92 && i<= 94)) {
					// set start, central, end sentinel lenght
					length = y;
			} else {
				// set normal bar code length
				length = (y-30);
			}

		}else{
			length = y;
		}

		// if 1 set black bar
		if (bars[i] == 1) {

			RECT rc = {i*3 + 35, x, i*3 + 37, length };
			Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			FillRect(hdc, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH));

		}

	}

	//for (i=1; i<7; i++) {
	//	TextOutW(hdc, i*14 + 17, 100, TEXT("11"), lstrlen(TEXT("11")));
	//}

	//for (i=7; i<13; i++) {
	//	TextOutW(hdc, i*14 + 27, 100, TEXT("22"), lstrlen(TEXT("22")));
	//}

	//TextOutW(hdc, 7, 90, TEXT("33"), lstrlen(TEXT("33")));

	return true;

}
