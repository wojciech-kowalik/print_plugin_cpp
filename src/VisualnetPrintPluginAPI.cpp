
#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "DOM/Window.h"
#include "global/config.h"
#include <atlstr.h>
#include <windows.h>
#include <math.h>
#include <map>

#include "VisualnetPrintPluginAPI.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintPluginAPI::print(std::string dataJson, std::string templateJson, bool logData)
///
/// @brief Print data
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool VisualnetPrintPluginAPI::print(std::string dataJson, std::string templateJson, std::string printerName,  bool logData){

	std::map<std::string, Json::Value> groups; 
	std::map<std::string, Json::Value>::iterator curr,end;

	// flag to set if logging has to be shown
	VisualnetPrintPluginAPI::logData = logData;

	// set print type - should be deprecated ?
	//VisualnetPrintPluginAPI::type = type;

	// get printer name
	VisualnetPrintPluginAPI::printerName = printerName;

	// approved data and templates values
	Json::Value dataJsonApproved = VisualnetPrintPluginAPI::getJson(dataJson);
	Json::Value templateJsonApproved = VisualnetPrintPluginAPI::getJson(templateJson);

	// data after grouping
	groups = VisualnetPrintUtilities::groupData(dataJsonApproved, templateJsonApproved);


	// log plugin version
	console("******************************************************************", "log");
	console("VisualnetPrintPlugin: "+get_version(), "log");

	// iterate over group templates
	for( curr = groups.begin(), end = groups.end();  curr != end;  curr++ ){

		console("-------------------------------------------------------------------", "log");
		console("Prepare for "+curr->first+" template - "+boost::lexical_cast<std::string>(curr->second.size())+" print(s)", "log");
		console("-------------------------------------------------------------------", "log");

		// check if elements values objects exists
		if(curr->second.isNull()){

			FBLOG_INFO("job", "[ NOTICE ] No data values for template data");
			console("[ NOTICE ] No data values for template data", "info");

			continue;
		}

		// make print jobs depends on template values
		VisualnetPrintPluginAPI::job(curr->second, VisualnetPrintUtilities::getJsonObject(curr->first, templateJsonApproved));

		console("Print job for "+curr->first+" template has been sent to: "+printerName, "log");
	}


	console("******************************************************************", "log"); 

	return true;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintPluginAPI::job(Json::Value dataJson, Json::Value templateJson, int typ)
///
/// @brief make printer job
///////////////////////////////////////////////////////////////////////////////
bool VisualnetPrintPluginAPI::job(Json::Value dataJson, Json::Value templateJson){

	std::map<std::string, Elements> elements;
	std::map<std::string, std::string> values;

	// default global text settings
	textFamily = VISUALSTRING_GLOBAL_TEXT_FAMILY;
	textSize = VISUALNUMBER_GLOBAL_TEXT_SIZE;
	textStyle = VISUALSTRING_GLOBAL_TEXT_STYLE;
	textOrientation = VISUALNUMBER_GLOBAL_TEXT_ORIENTATION;

	// check if initial object exists
	if(templateJson["init"].isNull()){
		FBLOG_ERROR("job", "[ ERROR ] No init data");
		throw FB::script_error("[ ERROR ] No init data");
	}

	// set default text
	const Json::Value font = templateJson["init"]["defaultText"];

	// get text settings from config
	if(font.isObject()){
		textFamily = font.get("family",textFamily).asString();
		textSize = font.get("size",textSize).asInt();
		textStyle = font.get("style",textStyle).asString();
		textOrientation = font.get("orientation",textOrientation).asInt();
	}

	// check if object with elements descriptions
	if(templateJson["elements"].isNull()){
		FBLOG_ERROR("job", "[ ERROR ] No elements data");
		throw FB::script_error("[ ERROR ] No elements data");
	}

	double widthArea = templateJson["init"].get("widthArea",0).asDouble()*10; // value in mm
	double heightArea = templateJson["init"].get("heightArea",0).asDouble()*10; // value in mm

	const Json::Value elementsJson = templateJson["elements"];
	const Json::Value valuesJson = dataJson;

	// get document orientation - default landscape
	std::string orientation = VisualnetPrintPluginAPI::getOrientation(templateJson["init"].get("printerOrientation","landscape").asString());

	Elements e;

	// get configuration elements data
	for ( int i = 0; i < elementsJson.size(); ++i ){

		Json::Value text = elementsJson[i]["text"];
		Json::Value container = elementsJson[i]["container"];
		Json::Value category = elementsJson[i]["category"];

		// check if container exists
		if(!container.isNull()){

			e.isContainer = true;
			e.containerWidth = container.get("width",VISUALSTRING_CONTAINER_WIDTH).asInt();
			e.containerHeight = container.get("height",VISUALSTRING_CONTAINER_WIDTH).asInt();
			e.containerAlign = container.get("align",VISUALSTRING_CONTAINER_ALIGN).asString();

		}else{
			e.isContainer = false;
		}

		// check if element has got category
		if(!category.isNull()){

			e.isCategory = true;
			e.category = category.get("name","").asString();
			e.pattern = category.get("pattern","").asString();
			e.fields = category.get("fields","");
			e.condition = category.get("condition","").asString();
			e.root = category.get("root","").asString();

		}else{

			e.isCategory = false;
		}

		e.x = elementsJson[i].get("x","0").asInt();
		e.y = elementsJson[i].get("y","0").asInt();
		e.type = elementsJson[i].get("type","").asString();
		e.content = elementsJson[i].get("content","").asString();

		e.textFamily = text.get("family", textFamily).asString();
		e.textSize = text.get("size", textSize).asInt();
		e.textStyle = text.get("style", textStyle).asString();
		e.textOrientation = text.get("orientation", textOrientation).asInt();

		elements.insert(std::make_pair(elementsJson[i].get("field","none").asString(), e));
	}

	// get printer system from initial object
	std::string s = VisualnetPrintPluginAPI::printerName;
	CString str(s.c_str());	
	LPTSTR printerName = str.GetBuffer(0);
	str.ReleaseBuffer();

	HDC hdc = NULL;
	HANDLE hPrinter;
	LPDEVMODE  pDevMode = NULL;
	PRINTER_DEFAULTS pd = { NULL, NULL, PRINTER_ALL_ACCESS };

	DWORD dwNeeded, dwRet;

	//start print job
	if (OpenPrinter(printerName, &hPrinter, &pd))
	{
		dwNeeded = DocumentProperties(NULL,
			hPrinter,       // handle to our printer
			printerName,    // name of the printer
			NULL,           
			NULL,           
			0);             // zero returns buffer size 

		pDevMode = (LPDEVMODE)malloc(dwNeeded);

		dwRet = DocumentProperties(NULL,
			hPrinter,
			printerName,
			pDevMode,       // the address of the buffer to fill
			NULL,           // not using the input buffer 
			DM_OUT_BUFFER); // have the output buffer filled

		if (dwRet != IDOK)
		{
			// if failure, cleanup and return failure
			free(pDevMode);
			ClosePrinter(hPrinter);

			FBLOG_ERROR("job", "[ ERROR ] Printer failure");
			throw FB::script_error("[ ERROR ] Printer failure");
		}

		// if the printer supports paper orientation, set it
		if (pDevMode->dmFields & DM_ORIENTATION)
		{
			if(orientation == VISUALSTRING_ORIENT_LANDSCAPE){
				pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			}

			if(orientation == VISUALSTRING_ORIENT_PORTRAIT){
				pDevMode->dmOrientation = DMORIENT_PORTRAIT;
			}
		}

		// set print quality
		pDevMode->dmPrintQuality = DMRES_HIGH;

		// set size of print area
		pDevMode->dmPaperSize = DMPAPER_USER;
		pDevMode->dmPaperLength = (short) heightArea;
		pDevMode->dmPaperWidth = (short) widthArea;

		dwRet = DocumentProperties(NULL,
			hPrinter,
			printerName,
			pDevMode,       // reuse our buffer for output
			pDevMode,       // pass the driver our changes
			DM_IN_BUFFER |  // commands to merge our changes and write the result
			DM_OUT_BUFFER); 

	}else{

		FBLOG_ERROR("job", "[ ERROR ] No printer "+s+" available");
		throw FB::script_error("[ ERROR ] No printer "+s+" available");
	}

	hdc = CreateDC(TEXT("WINSPOOL"), printerName, NULL, pDevMode);

	// get device capabilities
	int xDPI = GetDeviceCaps(hdc, LOGPIXELSX);
	std::string sxDPI = boost::lexical_cast<std::string>( xDPI );
	console("Printer dpi: "+sxDPI, "log");

	int horzsize = GetDeviceCaps(hdc,HORZSIZE); // gives the width of the physical display in millimeters
	int vertsize = GetDeviceCaps(hdc, VERTSIZE); // gives the height of the physical display in millimeters
	int horzres = GetDeviceCaps(hdc, HORZRES); // gives the height of the physical display in pixels
	int vertres = GetDeviceCaps(hdc, VERTRES); // gives the width of the physical display in pixels

	int hdps = horzres/horzsize; // calculate the horizontal pixels per millimeter
	int vdps = vertres/vertsize; // calculate the vertical pixels per millimeter

	console("Physical paper width: "+boost::lexical_cast<std::string>( horzsize )+" mm", "log"); 
	console("Physical paper height: "+boost::lexical_cast<std::string>( vertsize )+" mm", "log"); 

	// set default text
	setText(hdc, textFamily, textSize, textStyle, textOrientation);

	// get text style
	console("Global text has been set to ( family: "+textFamily+", size: "+boost::lexical_cast<std::string>( textSize )+", style: "+textStyle+")", "log");

	// generate print
	VisualnetPrintPluginAPI::generate(hdc, valuesJson, elements, hdps, vdps);

	// clean handlers
	DeleteDC(hdc);
	ClosePrinter(hPrinter);

	return true;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintPluginAPI::generate(HDC &hdc, Json::Value valuesJson, std::map<std::string, Elements> elements)
///
/// @brief Generate print
///////////////////////////////////////////////////////////////////////////////
bool VisualnetPrintPluginAPI::generate(HDC &hdc, Json::Value valuesJson, std::map<std::string, Elements> elements, int hdps, int vdps){

	DOCINFO di = { 0 };
	di.cbSize      = sizeof(DOCINFO); 
	di.lpszDocName = TEXT("print"); 
	di.fwType      = 0;
	TEXTMETRIC tm = { 0 };
	std::map<std::string, Elements>::iterator curr,end;

	StartDoc(hdc, &di);

	for ( int i = 0; i < valuesJson.size(); ++i ){

		StartPage(hdc);

		MoveToEx(hdc, 0, 0, NULL);

		//VisualnetPrintPluginAPI::getImage(hdc, "C:\\Users\\user\\Desktop\\test.png");

		// set the alignment mode so we can output text and it will automatically be horizontally positioned
		SetTextAlign(hdc, GetTextAlign(hdc));

		// get the height of text
		GetTextMetrics(hdc, &tm);

		for( curr = elements.begin(), end = elements.end();  curr != end;  curr++ ){

			std::string value;

			// if element is dynamic
			if( curr->second.type == VISUALSTRING_ELEMENT_DYNAMICTYPE ){

				// get category and pattern
				if(curr->second.isCategory == true) {

					// date category
					if(curr->second.category == VISUALSTRING_ELEMENT_DATECATEGORY){
						value = VisualnetPrintUtilities::datePattern(curr->second.pattern, valuesJson[i]["eventdate"]);
					}

					// replace category
					if(curr->second.category == VISUALSTRING_ELEMENT_REPLACECATEGORY){

						// check if pattern fields exists
						if(curr->second.fields.isNull()){

							FBLOG_INFO("generate", "[ NOTICE ] No fields for category "+boost::lexical_cast<std::string>(VISUALSTRING_ELEMENT_REPLACECATEGORY)+" pattern "+curr->second.pattern);
							console("[ NOTICE ] No fields for category "+boost::lexical_cast<std::string>(VISUALSTRING_ELEMENT_REPLACECATEGORY)+" pattern "+curr->second.pattern, "info");

						}else{

							value = VisualnetPrintUtilities::replacePattern(curr->second.pattern, curr->second.fields, curr->second.root, valuesJson[i]);
						}

					}

					// choice category
					if(curr->second.category == VISUALSTRING_ELEMENT_CHOICECATEGORY){

						// check if pattern fields exists
						if(curr->second.fields.isNull()){

							FBLOG_INFO("generate", "[ NOTICE ] No fields for category "+boost::lexical_cast<std::string>(VISUALSTRING_ELEMENT_CHOICECATEGORY)+" pattern "+curr->second.pattern);
							console("[ NOTICE ] No fields for category "+boost::lexical_cast<std::string>(VISUALSTRING_ELEMENT_CHOICECATEGORY)+" pattern "+curr->second.pattern, "info");

						}else{

							value = VisualnetPrintUtilities::choicePattern(curr->second.pattern, curr->second.fields, curr->second.condition, valuesJson[i]);
						}
					}

				}else{

					// get dynamic value only
					value = valuesJson[i].get(curr->first,"").asString();

				}

			}

			// if element is static
			if(curr->second.type == VISUALSTRING_ELEMENT_STATICTYPE){
				value = curr->second.content;
			} 

			// convert string to wstring with utf8 support                                                                                                                                                       
			CA2W wideStr(value.c_str(), CP_UTF8);

			// set text from element settings
			VisualnetPrintPluginAPI::setText(hdc, curr->second.textFamily, curr->second.textSize, curr->second.textStyle, curr->second.textOrientation);

			// check if container exists
			if(curr->second.isContainer == true){
				VisualnetPrintPluginAPI::textInContainer(hdc, curr->second, wideStr, hdps, vdps, curr->second.textOrientation); // set tex into container
			}else{
				TextOutW(hdc, curr->second.x*hdps, curr->second.y*vdps, wideStr, lstrlen(wideStr)); // simply text out
			}

			MoveToEx(hdc, 0, 0, NULL);

			// if barcode exists, generate it
			if(curr->second.type == VISUALSTRING_ELEMENT_BARCODETYPE){

				try{
					VisualnetEAN13 ean13;
					ean13.make(valuesJson[i].get(curr->first,"").asString(), curr->second.x*hdps, curr->second.y*vdps, true, hdc);
				}catch (std::exception& e){

					FBLOG_ERROR("generate", "[ ERROR ] Error during barcode generating: "+(std::string)e.what());
					throw FB::script_error("[ ERROR ] Error during barcode generating: "+(std::string)e.what());
				}

			}

		}

		EndPage(hdc);

	}

	EndDoc(hdc);

	return true;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintPluginAPI::textInContainer(HDC &hdc, Elements element, LPCTSTR wideStr, int hdps, int vdps, int angle))
///
/// @brief set text in container
///////////////////////////////////////////////////////////////////////////////
bool VisualnetPrintPluginAPI::textInContainer(HDC &hdc, Elements element, LPCTSTR wideStr, int hdps, int vdps, int angle){

	int width = element.containerWidth*10; // mm
	int height = element.containerHeight*10; // mm
	UINT align;

	if(element.containerAlign == VISUALSTRING_ALIGN_CENTER){
		align = DT_CENTER;
	}else if(element.containerAlign == VISUALSTRING_ALIGN_RIGHT){
		align = DT_RIGHT;
	}else if(element.containerAlign == VISUALSTRING_ALIGN_LEFT){
		align = DT_LEFT;
	}else{
		align = DT_LEFT; // default left align
	}

   // NEW SOLUTION

	double pi = 3.141592654;
	double radian = (pi * angle)/180;

   SIZE TextSize;

   GetTextExtentPoint32(hdc, wideStr, lstrlen(wideStr), &TextSize);

   POINT center;
   center.x = TextSize.cx / 2;
   center.y = TextSize.cy / 2;

	POINT rcenter;
	rcenter.x = long(cos(radian) * TextSize.cx - sin(radian) * TextSize.cy);
	rcenter.y = long(sin(radian) * TextSize.cx + cos(radian) * TextSize.cy);


	long xTopLeft = (long) (cos(radian) * element.x*hdps - sin(radian) * element.y*vdps);
	long yTopLeft = (long) (sin(radian) * element.x*hdps + cos(radian) * element.y*vdps);

	long xBottomRight = (long) (cos(radian) * element.x+width - sin(radian) * element.y+height);
	long yBottomRight = (long) (sin(radian) * element.x+width + cos(radian) * element.y+height);


	// get text into container
	//RECT rc = {element.x*hdps, element.y*vdps, xBottomRight, yBottomRight };



	// END NEW SOLUTION

	RECT rc = {element.x*hdps, element.y*vdps, element.x+width /* width */, element.y+height /* height */ };

	//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
	//FillRect(hdc, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH));



	DrawText(hdc, wideStr, lstrlen(wideStr), &rc, align | DT_WORDBREAK | DT_VCENTER);

	return true;

}

bool VisualnetPrintPluginAPI::getImage(HDC &hdc, const char *img){

	//HBITMAP hbmObraz;
	//hbmObraz = ( HBITMAP ) LoadImage( NULL, TEXT("C:\\Users\\user\\Desktop\\test.bmp"), IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE );

	//HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbmObraz);
	//SelectObject( hdc, hbmObraz );

	//BITMAP bmInfo;
	//GetObject( hbmObraz, sizeof( bmInfo ), & bmInfo );
	//BitBlt( hdc, 150, 100, 200, 250, hdc, 0, 0, SRCCOPY );
	//SelectObject(hdc, hbmOld);

	FILE * pFile;
	long lSize;
	char * buffer;
	size_t result;

	pFile = fopen ( img , "rb" );
	if (pFile==NULL) {
		throw FB::script_error("File dosen`t exists");
	}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	buffer = (char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL) {
		throw FB::script_error("Memory error");
	}

	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize) {
		throw FB::script_error("Reading error");
	}

	// terminate
	fclose (pFile);
	free (buffer);


	//int ul = CHECKPNGFORMAT;

	//if ((ExtEscape(hdc, QUERYESCSUPPORT,sizeof(ul), &ul, 0, 0) > 0) &&
	//		(ExtEscape(hdc, CHECKPNGFORMAT,lSize, buffer, sizeof(ul), &ul) > 0) &&
	//			(ul == 1)){}


	//SetStretchBltMode(hdc, HALFTONE);

	console("Last error: "+ boost::lexical_cast<std::string>(GetLastError()), "log");

	return true; 

}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintPluginAPI::getOrientation(std::string orientation)
///
/// @brief get document orientation
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintPluginAPI::getOrientation(std::string orientation){

	bool state = false;
	std::string values[] = {VISUALSTRING_ORIENT_LANDSCAPE, VISUALSTRING_ORIENT_PORTRAIT};

	// iterate over values
	foreach(std::string str, values)
	{
		if(orientation == str){
			state = true;
			break;
		}
	}

	// raise exception when type is not supported
	if(state == false){

		FBLOG_ERROR("getOrientation", "[ ERROR ] Not supported document orientation, wrong value ("+orientation+") - supported ["+VISUALSTRING_ORIENT_LANDSCAPE+", "+VISUALSTRING_ORIENT_PORTRAIT+"]");
		throw FB::script_error("[ ERROR ] Not supported document orientation, wrong value ("+orientation+") - supported ["+VISUALSTRING_ORIENT_LANDSCAPE+", "+VISUALSTRING_ORIENT_PORTRAIT+"]");
	}

	return orientation;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintPluginAPI::getTextStyle(std::string style)
///
/// @brief get text style
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintPluginAPI::getTextStyle(std::string style){

	bool state = false;
	std::string message;
	int length = 5;
	int i = 1;

	std::string values[] = {
		VISUALSTRING_TEXT_STYLE_BOLD, 
		VISUALSTRING_TEXT_STYLE_NORMAL, 
		VISUALSTRING_TEXT_STYLE_ITALIC,
		VISUALSTRING_TEXT_STYLE_UNDERLINE,
		VISUALSTRING_TEXT_STYLE_STRIKEOUT
	};

	// iterate over values
	foreach(std::string str, values)
	{
		if(i == length){
			message += str;
		}else{
			message += str+", ";
		}

		if(style == str){
			state = true;
		}

		i++;
	}

	// raise exception when type is not supported
	if(state == false){
		FBLOG_ERROR("getTextStyle", "[ ERROR ] Not supported text style, wrong value ("+style+") - supported ["+message+"]");
		throw FB::script_error("[ ERROR ] Not supported text style, wrong value ("+style+") - supported ["+message+"]");
	}

	return style;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Json::Value VisualnetPrintPluginAPI::isJson(std::string json)
///
/// @brief Check if string is json object
///////////////////////////////////////////////////////////////////////////////
Json::Value VisualnetPrintPluginAPI::getJson(std::string json){

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	bool parsingSuccessful = reader.parse( json, root );

	// check if string representation is JSON object
	if ( !parsingSuccessful ){
		FBLOG_ERROR("getJson", "[ ERROR ] No JSON data format, wrong value("+json+")");
		throw FB::script_error("[ ERROR ] No JSON data format, wrong value("+json+")");
	}

	return root;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn void VisualnetPrintPluginAPI::setText(HDC &hdc, std::string textFamily, int textSize, std::string textStyle, int textOrientation)
///
/// @brief Set text on document
///////////////////////////////////////////////////////////////////////////////
void VisualnetPrintPluginAPI::setText(
	HDC &hdc, 
	std::string textFamily = VISUALSTRING_GLOBAL_TEXT_FAMILY, 
	int textSize = VISUALNUMBER_GLOBAL_TEXT_SIZE, 
	std::string textStyle = VISUALSTRING_GLOBAL_TEXT_STYLE,
	int textOrientation = VISUALNUMBER_GLOBAL_TEXT_ORIENTATION){

		VisualnetPrintPluginAPI::getTextStyle(textStyle);

		HFONT hFont;
		int weight = 400; //FW_NORMAL
		bool italic = false;
		bool underline = false;
		bool strike = false;

		if(textStyle == VISUALSTRING_TEXT_STYLE_BOLD){
			weight = FW_BOLD;
		}else if(textStyle == VISUALSTRING_TEXT_STYLE_NORMAL){
			weight = FW_NORMAL;
		}else if(textStyle == VISUALSTRING_TEXT_STYLE_ITALIC){
			italic = true;
		}else if(textStyle == VISUALSTRING_TEXT_STYLE_UNDERLINE){
			underline = true;
		}else if(textStyle == VISUALSTRING_TEXT_STYLE_STRIKEOUT){
			strike = true;
		}

		CA2W wideStr(textFamily.c_str(), CP_UTF8);
		int nHeight = -MulDiv(textSize, GetDeviceCaps(hdc, LOGPIXELSX), 72);

		hFont = CreateFont(nHeight, 0, textOrientation*10, 0, weight, italic, underline, strike, OEM_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, wideStr);
		SelectObject(hdc, hFont);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn void TestAPI::console(std::string message, std::string type)
///
/// @brief Send message to console output
///////////////////////////////////////////////////////////////////////////////
void VisualnetPrintPluginAPI::console(std::string message, std::string type){

	// check if value has to be logged
	if(VisualnetPrintPluginAPI::logData == false){
		return;
	}

	// retrieve a reference to the DOM Window
	FB::DOM::WindowPtr window = m_host->getDOMWindow();

	// check if the DOM Window has an the property console
	if (window && window->getJSObject()->HasProperty("console")) {
		// create a reference to the browswer console object
		FB::JSObjectPtr obj = window->getProperty<FB::JSObjectPtr>("console");

		// invoke the "log" method on the console object
		obj->Invoke(type, FB::variant_list_of(message));
	}

	// log data into file
	FBLOG_INFO("log", message);

}

///////////////////////////////////////////////////////////////////////////////
/// @fn VisualnetPrintPluginPtr VisualnetPrintPluginAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
VisualnetPrintPluginPtr VisualnetPrintPluginAPI::getPlugin()
{
	VisualnetPrintPluginPtr plugin(m_plugin.lock());
	if (!plugin) {
		FBLOG_ERROR("getPlugin", "[ ERROR ] The plugin is invalid");
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintPluginAPI::get_version()
///
/// @brief Read-only property version
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintPluginAPI::get_version()
{
	return FBSTRING_PLUGIN_VERSION;
}
