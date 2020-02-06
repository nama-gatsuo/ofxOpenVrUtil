meta:
	ADDON_NAME = ofxOpenVrUtil
	ADDON_DESCRIPTION = Addon to access VR hardware using the OpenVR API from Valve Software
	ADDON_AUTHOR = Ayumu Nagamatsu
	ADDON_TAGS = "VR" "OpenVR" "SteamVR" "HTC Vive" "Oculus Rift"
	ADDON_URL = http://github.com/nama-gatsuo/ofxOpenVrUtil

common:

vs:
	ADDON_DLLS_TO_COPY += "openvr/bin/win64/openvr_api.dll"
	ADDON_LIBS += "openvr/lib/win64/openvr_api.lib"
	ADDON_INCLUDES += "openvr/headers/"
	
