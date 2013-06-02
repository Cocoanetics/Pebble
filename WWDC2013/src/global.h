#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

/**
 As watchface you cannot modify the UTC offset. As standard app you can use the up and down buttons to adjust UTC offset.
 */
#define BUILD_MODE APP_INFO_STANDARD_APP

/**
 The app's UUID
 */
#define MY_UUID { 0x6B, 0xF0, 0x43, 0xB0, 0xFD, 0x06, 0x48, 0x3D, 0x84, 0x7E, 0xBC, 0x9B, 0x08, 0xAD, 0x58, 0xA0 }


/**
 Useful Macros
 */
#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a<b)?a:b)


/**
 Time Zones
 */
#define UTC_OFFSET_SFO -7
#define UTC_OFFSET_INITIAL UTC_OFFSET_SFO


/**
 Macro to setup app
 */
PBL_APP_INFO(MY_UUID,
             "WWDC 2013", "Cocoanetics.com",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             BUILD_MODE);
