
class VisualnetPrintUtilities{

public:

	// Default constructor
	VisualnetPrintUtilities();

	// Default destructor
	virtual ~VisualnetPrintUtilities();

	// Prepare data for print
	static std::map<std::string, Json::Value> VisualnetPrintUtilities::groupData(Json::Value dataJsonApproved, Json::Value templateJsonApproved);

	// Get tjson object by name
	static Json::Value VisualnetPrintUtilities::getJsonObject(std::string name, Json::Value values);

	// Explode string
	static std::string explode(const std::string& str, const char ch, const unsigned int v );

	// Replace string
	static bool replace(std::string& str, const std::string& from, const std::string& to);

	// Trim string
	static bool trim(std::string& str);

	// PATTERNS

	// date pattern
	static std::string datePattern(std::string pattern, Json::Value value);

	// replace pattern
	static std::string replacePattern(std::string pattern, Json::Value fields, std::string root, Json::Value value);

	// choice pattern
	static std::string choicePattern(std::string pattern, Json::Value fields, std::string condition, Json::Value value);

	// CONDITIONS

	// invitation condition
	static std::string invitationCondition(std::string staticString, std::string dynamicString, Json::Value fields, Json::Value value);

	// place condition
	static std::string placeCondition(std::string staticString, std::string dynamicString, Json::Value fields, Json::Value value);


private:


};
