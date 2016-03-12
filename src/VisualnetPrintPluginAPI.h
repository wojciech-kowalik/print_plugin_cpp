
#include <string>
#include <exception>
#include <sstream>
#include <atlstr.h>
#include <windows.h>
#include <map>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "json/json.h"
#include "fbjson.h"

#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>

#include "VisualnetPrintPlugin.h"
#include "VisualnetEAN13.h"
#include "VisualnetPrintUtilities.h"

#define VISUALSTRING_ELEMENT_DYNAMICTYPE "dynamic"
#define VISUALSTRING_ELEMENT_BARCODETYPE "barcode"
#define VISUALSTRING_ELEMENT_STATICTYPE "static"

#define VISUALSTRING_ELEMENT_DATECATEGORY "date"
#define VISUALSTRING_ELEMENT_REPLACECATEGORY "replace"
#define VISUALSTRING_ELEMENT_CHOICECATEGORY "choice"
 
#define VISUALSTRING_CATEGORY_INVITATIONCONDITION "invitation"
#define VISUALSTRING_CATEGORY_PLACECONDITION "place"

#define VISUALSTRING_ELEMENT_SECTIONPLACE "section"
#define VISUALSTRING_ELEMENT_ROWPLACE "row"
#define VISUALSTRING_ELEMENT_NUMBERPLACE "number"

#define VISUALSTRING_ORIENT_LANDSCAPE "landscape"
#define VISUALSTRING_ORIENT_PORTRAIT "portrait"

#define VISUALSTRING_TEXT_STYLE_BOLD "bold"
#define VISUALSTRING_TEXT_STYLE_NORMAL "normal"
#define VISUALSTRING_TEXT_STYLE_ITALIC "italic"
#define VISUALSTRING_TEXT_STYLE_UNDERLINE "underline"
#define VISUALSTRING_TEXT_STYLE_STRIKEOUT "strike"

#define VISUALSTRING_CONTAINER_WIDTH 0
#define VISUALSTRING_CONTAINER_HEIGHT 0
#define VISUALSTRING_CONTAINER_ALIGN 0

#define VISUALSTRING_GLOBAL_TEXT_FAMILY "Arial"
#define VISUALNUMBER_GLOBAL_TEXT_SIZE 12
#define VISUALSTRING_GLOBAL_TEXT_STYLE "normal"

#define VISUALSTRING_ALIGN_CENTER "center"
#define VISUALSTRING_ALIGN_RIGHT "right"
#define VISUALSTRING_ALIGN_LEFT "left"

#define VISUALNUMBER_GLOBAL_TEXT_ORIENTATION 0

#ifndef H_VisualnetPrintPluginAPI
#define H_VisualnetPrintPluginAPI

#define foreach BOOST_FOREACH

struct Elements{
	int x, y, textSize, textOrientation, containerWidth, containerHeight;
	std::string type, category, pattern, content, bar, textFamily, textStyle, containerAlign, root, condition;
	bool isContainer, isCategory;
	Json::Value fields;
};

class VisualnetPrintPluginAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn VisualnetPrintPluginAPI::VisualnetPrintPluginAPI(const VisualnetPrintPluginPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    VisualnetPrintPluginAPI(const VisualnetPrintPluginPtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
    {
        registerMethod("print", make_method(this, &VisualnetPrintPluginAPI::print));

        
        // Read-only property
        registerProperty("version",
                         make_property(this,
                                       &VisualnetPrintPluginAPI::get_version));
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn VisualnetPrintPluginAPI::~VisualnetPrintPluginAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~VisualnetPrintPluginAPI() {};

    VisualnetPrintPluginPtr getPlugin();

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

	// Print data
	bool print(std::string dataJson, std::string templateJson, std::string printerName, bool logData);

	// Send message to console output
	void console(std::string message, std::string type);

private:

	std::string textFamily, textStyle, printerName;
	int textSize, textOrientation, type;
	bool logData;

    VisualnetPrintPluginWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

	// Make printer job
	bool job(Json::Value dataJson, Json::Value templateJson);

	// Get document orientation
	std::string getOrientation(std::string orientation);

	// Get json object
	Json::Value getJson(std::string json);

	// Get text style
	std::string getTextStyle(std::string style);

	// Set text
	void setText(HDC &hdc, std::string textFamily, int textSize, std::string textStyle, int textDirection);

	// Print method
	bool generate(HDC &hdc, Json::Value valuesJson, std::map<std::string, Elements> elements, int hdps, int vdps);

	// Get text into container
	bool textInContainer(HDC &hdc, Elements element, LPCTSTR wideStr, int hdps, int vdps, int angle);

	// Get text style
	bool getImage(HDC &hdc, const char *img);

};

#endif // H_VisualnetPrintPluginAPI

