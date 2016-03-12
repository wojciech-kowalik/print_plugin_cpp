
#include <exception>
#include <string>
#include <atlstr.h>

class VisualnetEAN13{

public:

	// Default constructor
	VisualnetEAN13();

	// Default destructor
	virtual ~VisualnetEAN13();

	// Make EAN13 barcode
	bool make(std::string code, int x, int y, bool sentinel, HDC &hdc);

private:
	// Check if EAN13 is right value
	bool check();

	// EAN13 values
	char EAN13[13];

	// bars array
	char bars[95];

	static char parityTable[10][6];
	static char right[10][7];
	static char left[2][10][7];

};
