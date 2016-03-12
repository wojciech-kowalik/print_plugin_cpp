#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for VisualnetPrintPlugin
#
#\**********************************************************/

set(PLUGIN_NAME "VisualnetPrintPlugin")
set(PLUGIN_PREFIX "VPP")
set(COMPANY_NAME "Visualnet")

# ActiveX constants:
set(FBTYPELIB_NAME VisualnetPrintPluginLib)
set(FBTYPELIB_DESC "VisualnetPrintPlugin 1.6 Type Library")
set(IFBControl_DESC "VisualnetPrintPlugin Control Interface")
set(FBControl_DESC "VisualnetPrintPlugin Control Class")
set(IFBComJavascriptObject_DESC "VisualnetPrintPlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "VisualnetPrintPlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "VisualnetPrintPlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 22f77135-2290-51b1-b705-a682952e034c)
set(IFBControl_GUID 796040c5-2166-58c8-bb28-67f82033df3b)
set(FBControl_GUID b9545ee4-e97d-5943-bbc7-364a5ab5a8fb)
set(IFBComJavascriptObject_GUID a2c7dfed-2bda-5551-91d2-4121033725f4)
set(FBComJavascriptObject_GUID 7d4b348b-62ca-5416-9944-c61c962ff065)
set(IFBComEventSource_GUID cbbf120b-18ec-5a7b-a3f4-3963bd06d112)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "Visualnet.VisualnetPrintPlugin")
set(MOZILLA_PLUGINID "visuanet.pl/VisualnetPrintPlugin")

# strings
set(FBSTRING_CompanyName "visualnet.pl")
set(FBSTRING_FileDescription "print plugin developed by visualnet.pl")
set(FBSTRING_PLUGIN_VERSION "1.6")
set(FBSTRING_LegalCopyright "Copyright 2012 visualnet.pl")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "VisualnetPrintPlugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "VisualnetPrintPlugin")
set(FBSTRING_MIMEType "application/x-visualnet-printplugin")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

add_firebreath_library(jsoncpp)
add_firebreath_library(log4cplus)
add_boost_library(date_time)