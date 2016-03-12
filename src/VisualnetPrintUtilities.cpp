
#include "VisualnetPrintPluginAPI.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::gregorian;
using namespace boost::posix_time;

///////////////////////////////////////////////////////////////////////////////
/// @fn VisualnetPrintUtilities::VisualnetPrintUtilities()
///
/// @brief Default constructor
///////////////////////////////////////////////////////////////////////////////
VisualnetPrintUtilities::VisualnetPrintUtilities(){}

///////////////////////////////////////////////////////////////////////////////
/// @fn VisualnetPrintUtilities::~VisualnetPrintUtilities()
///
/// @brief Default destructor
///////////////////////////////////////////////////////////////////////////////
VisualnetPrintUtilities::~VisualnetPrintUtilities(){}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintUtilities::prepareData(Json::Value dataJsonApproved, Json::Value templateJsonApproved)
///
/// @brief Prepare data for print
///////////////////////////////////////////////////////////////////////////////
std::map<std::string, Json::Value> VisualnetPrintUtilities::groupData(Json::Value dataJsonApproved, Json::Value templateJsonApproved){

	int i, j;
	std::string templateName;
	std::map<std::string, Json::Value> templateGroup;

	for ( i = 0; i < templateJsonApproved.size(); ++i ){

		templateName = templateJsonApproved[i].get("name", "default").asString();

		// set new root element
		Json::Value root;
		Json::Value values = root["values"];

		for (j = 0; j < dataJsonApproved.size(); ++j) {

			if(templateName == dataJsonApproved[j].get("template","").asString()){
				// append equals values
				values.append(dataJsonApproved[j]);

			}

		}
		// make groups of templates
		templateGroup.insert(std::make_pair(templateName, values));

	}

	return templateGroup;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn Json::Value VisualnetPrintUtilities::getJsonObject(std::string name, Json::Value templates)
///
/// @brief Get template by name
///////////////////////////////////////////////////////////////////////////////
Json::Value VisualnetPrintUtilities::getJsonObject(std::string name, Json::Value values){

	int i;
	Json::Value object;

	for (i = 0; i < values.size(); i++) {

		if(values[i].get("name","").asString() == name) {

			object = values[i];
			break;
		}
	}

	return object;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintUtilities::datePattern(std::string pattern, std::string value)
///
/// @brief Date pattern
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintUtilities::datePattern(std::string pattern, Json::Value value){

	std::stringstream ss;

	try{

		// set up the collections of custom strings.
		// only the full names are altered for the sake of brevity
		std::string month_names[12] = { "stycznia", "lutego", "marca", 
			"kwietnia", "maja", "czerwca", 
			"lipca", "sierpnia", "wrze쐍ia", 
			"pa웓ziernika", "listopada", "grudnia" };

		std::string day_names[7] = { "niedziela", "poniedzia쿮k", "wtorek", "쐒oda", 
			"czwartek", "pi퉡ek", "sobota" };

		std::vector<std::string> long_days(&day_names[0], &day_names[7]);
		std::vector<std::string> long_months(&month_names[0], &month_names[12]);

		//  create date_facet and date_input_facet using all defaults
		time_facet* date_output = new time_facet();

		// replace names in the output facet
		date_output->long_month_names(long_months);
		date_output->long_weekday_names(long_days);

		// format date
		date_output->format(pattern.c_str ());

		ptime d(time_from_string(value.get("date","").asString()));

		ss.imbue(std::locale(ss.getloc(), date_output));
		ss.exceptions(std::ios_base::failbit);

		ss << d;

	}catch(std::exception& e){

	}


	return ss.str();

}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintUtilities::choicePattern(std::string pattern, Json::Value fields, Json::Value condition, Json::Value value)
///
/// @brief Replace pattern
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintUtilities::choicePattern(std::string pattern, Json::Value fields, std::string condition, Json::Value value){

	std::string staticString = VisualnetPrintUtilities::explode(pattern, '|', 0);
	std::string dynamicString = VisualnetPrintUtilities::explode(pattern, '|', 1);
	std::string out;

	VisualnetPrintUtilities::trim(staticString);
	VisualnetPrintUtilities::trim(dynamicString);

	// check if condition is invitation
	if(condition == VISUALSTRING_CATEGORY_INVITATIONCONDITION){
		out = VisualnetPrintUtilities::invitationCondition(staticString, dynamicString, fields, value);
	}

	// check if condition is place
	if(condition == VISUALSTRING_CATEGORY_PLACECONDITION){
		out = VisualnetPrintUtilities::placeCondition(staticString, dynamicString, fields, value);
	}

	return out;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintUtilities::placeCondition(std::string staticString, std::string dynamicString, Json::Value fields, Json::Value value)
///
/// @brief Place condtition
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintUtilities::placeCondition(std::string staticString, std::string dynamicString, Json::Value fields, Json::Value value){

	std::string out, section, row, number;
	Json::Value fieldSection, fieldRow, fieldNumber;

	// manipulate output string depends on section, row, number

	// get json ticket values
	section = value.get(VISUALSTRING_ELEMENT_SECTIONPLACE,"").asString();
	row = value.get(VISUALSTRING_ELEMENT_ROWPLACE,"").asString();
	number = value.get(VISUALSTRING_ELEMENT_NUMBERPLACE,"").asString();

	// get json template values
	fieldSection = VisualnetPrintUtilities::getJsonObject(VISUALSTRING_ELEMENT_SECTIONPLACE, fields);
	fieldRow = VisualnetPrintUtilities::getJsonObject(VISUALSTRING_ELEMENT_ROWPLACE, fields);
	fieldNumber = VisualnetPrintUtilities::getJsonObject(VISUALSTRING_ELEMENT_NUMBERPLACE, fields);

	// CASES

	// Sektor        Rz퉐        Miejsce    Case       Warto쒏 domy쐋na
	// pusty        pusty        pusty       0         Miejsca nienumerowane
	// pusty        pusty          1         1         Miejsce: 1
	// pusty          2          pusty       2         Rz퉐: 2  Miejsca nienumerowane
	// pusty          2            1         3         Rz퉐: 2  Miejsce: 1
	//	  3         pusty        pusty       4         Sektor: 3  Miejsca nienumerowane
	//	  3         pusty          1         5         Sektor: 3  Miejsce: 1
	//	  3           2          pusty       6         Sektor: 3  Rz퉐: 2  Miejsca nienumerowane
	//	  3           2            1         7         Sektor: 3  Rz퉐: 2  Miejsce: 1 

	// 0 case
	if(section == "" && row == "" && number == ""){

		// get static text only
		out = staticString;

    // 1 case
	}else if(section == "" && row == "" && number != ""){

		// get number value
		out = fieldNumber.get("text","").asString()+" "+number;

    // 2 case
	}else if(section == "" && row != "" && number == ""){

		// get row and static text values
		out = fieldRow.get("text","").asString()+" "+row+" "+staticString;

    // 3 case
	}else if(section == "" && row != "" && number != ""){

		// get row and number values
		out = fieldRow.get("text","").asString()+" "+row+" "+fieldNumber.get("text","").asString()+" "+number;

    // 4 case
	}else if(section != "" && row == "" && number == ""){

		// get section and static text values
		out = fieldSection.get("text","").asString()+" "+section+" "+staticString;

    // 5 case
	}else if(section != "" && row == "" && number != ""){

		// get section and number values
		out = fieldSection.get("text","").asString()+" "+section+" "+fieldNumber.get("text","").asString()+" "+number;

    // 6 case
	}else if(section != "" && row != "" && number == ""){

		// get section and number values
		out = fieldSection.get("text","").asString()+" "+section+" "+fieldRow.get("text","").asString()+" "+row+" "+staticString;

    // 7 case 
	}else if(section != "" && row != "" && number != ""){

		// get section and number values
		out = fieldSection.get("text","").asString()+" "+section+" "+fieldRow.get("text","").asString()+" "+row+" "+fieldNumber.get("text","").asString()+" "+number;

	}

	return out;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintUtilities::invitationCondition(std::string staticString, std::string dynamicString, Json::Value fields, Json::Value value)
///
/// @brief Invitation condtition
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintUtilities::invitationCondition(std::string staticString, std::string dynamicString, Json::Value fields, Json::Value value){

	bool invitationValue;
	std::string out;

	// get invitation value from ticket data json
	invitationValue = value.get(VISUALSTRING_CATEGORY_INVITATIONCONDITION,"").asBool();

	if(invitationValue == TRUE){ // if TRUE get static text
		out = staticString;
	}else{ // else manipulate dynamic text value
		out = VisualnetPrintUtilities::replacePattern(dynamicString, fields, "", value);
	}

	return out;

}

///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintUtilities::replacePattern(std::string pattern, Json::Value fields, std::string root, Json::Value value)
///
/// @brief Replace pattern
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintUtilities::replacePattern(std::string pattern, Json::Value fields, std::string root, Json::Value value){

	std::string target;

	for ( int i = 0; i < fields.size(); ++i ){

		std::string temp("{"+boost::lexical_cast<std::string>(i)+"}");

		// check if root element exists
		if(root == ""){ // get value from main
			target = value.get(fields[i]["name"].asString(),"").asString();
		}else{ // if json element is object get value from it
			target = value[root].get(fields[i]["name"].asString(),"").asString();
		}

		VisualnetPrintUtilities::replace(pattern, temp, target); 
	}

	return pattern;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn std::string VisualnetPrintUtilities::explode( const std::string & str, const char ch, const unsigned int v )
///
/// @brief Explode string by sign
///////////////////////////////////////////////////////////////////////////////
std::string VisualnetPrintUtilities::explode( const std::string & str, const char ch, const unsigned int v )
{
    std::string ret = "";
    for( size_t i = 0, tmp = 0; i < str.size(); ++i ) {
        if( str[ i ] == ch ) {
            if( tmp > v ) break;
            else ++tmp;
            
        }
        else if( tmp == v ) ret += str[ i ];
        
    }
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintUtilities::replace(std::string& str, const std::string& from, const std::string& to)
///
/// @brief Replace string in string
///////////////////////////////////////////////////////////////////////////////
bool VisualnetPrintUtilities::replace(std::string& str, const std::string& from, const std::string& to) {

    size_t start_pos = str.find(from);

    if(start_pos == std::string::npos){
        return false;
	}

    str.replace(start_pos, from.length(), to);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool VisualnetPrintUtilities::replace(std::string& str)
///
/// @brief Replace string in string
///////////////////////////////////////////////////////////////////////////////
bool VisualnetPrintUtilities::trim(std::string& str) {

	std::string whitespaces (" \t\f\v\n\r");
    size_t found;

    found = str.find_last_not_of(whitespaces);

    if (found != std::string::npos) {
        str.erase(found+1);
	}else{
        str.clear();
	}

	return true;

}