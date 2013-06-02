#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "mini-printf.h"
#include <time.h>

// the app's UUID
#define MY_UUID { 0x6B, 0xF0, 0x43, 0xB0, 0xFD, 0x06, 0x48, 0x3D, 0x84, 0x7E, 0xBC, 0x9B, 0x08, 0xAD, 0x58, 0xA0 }

PBL_APP_INFO(MY_UUID,
             "WWDC 2013", "Cocoanetics.com",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer countdownDaysTextLayer;
TextLayer countdownHMSTextLayer;

BmpContainer imageContainer;

// needs to be global because of delayed drawing
char dayLineBuffer[30];
char timeLineBuffer[30];

// get a time stamp from a pebble time struct
long long timestampFromTime(PblTm time)
{
	return (((time.tm_mday * 24) + time.tm_hour) * 60 + time.tm_min) * 60 + time.tm_sec;
}

long long timeintervalBetweenTimes(PblTm time1, PblTm time0)
{
	long long timestamp1 = timestampFromTime(time1);
	long long timestamp0 = timestampFromTime(time0);
	
	return timestamp1 - timestamp0;
}


void update_countdown()
{
	
	PblTm now;
	get_time(&now);
	
	PblTm keynote_time;
	keynote_time.tm_mday = 10;
	keynote_time.tm_hour = 19;
	
	long seconds_til_keynote = timeintervalBetweenTimes(keynote_time, now);
	
	long days = seconds_til_keynote / (24*60*60);
	
	if (days==1)
	{
		strcpy(dayLineBuffer, "1 day");
	}
	else
	{
		snprintf(dayLineBuffer, sizeof(dayLineBuffer), "%d days", days);
	}
	
	text_layer_set_text(&countdownDaysTextLayer, dayLineBuffer);

	seconds_til_keynote -= days * (24*60*60);
	
	snprintf(timeLineBuffer, sizeof(timeLineBuffer), "%02d:%02d:%02d", (seconds_til_keynote/3600), (seconds_til_keynote/60)%60, seconds_til_keynote%60);
	
	text_layer_set_text(&countdownHMSTextLayer, timeLineBuffer);
}

void handle_init(AppContextRef ctx)
{
	(void)ctx;

	window_init(&window, "WWDC 2013 Countdown");
	window_stack_push(&window, true /* Animated */);
	
	resource_init_current_app(&APP_RESOURCES);
	
	// WWDC Logo
	bmp_init_container(RESOURCE_ID_IMAGE_WWDC2013_LOGO, &imageContainer);
	
	imageContainer.layer.layer.frame.origin.x = 7;
	imageContainer.layer.layer.frame.origin.y = 30;

	layer_add_child(&window.layer, &imageContainer.layer.layer);

	// Days Display
	GRect frame;
	frame.origin.x = 0;
	frame.origin.y = 65;
	frame.size.w = window.layer.frame.size.w;
	frame.size.h = 40;
	
	text_layer_init(&countdownDaysTextLayer, frame);
	text_layer_set_background_color(&countdownDaysTextLayer, GColorClear);
	text_layer_set_text_alignment(&countdownDaysTextLayer, GTextAlignmentCenter);
	text_layer_set_font(&countdownDaysTextLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_APPLE_BOLD_30)));
	layer_add_child(&window.layer, &countdownDaysTextLayer.layer);

	
	// Hour Minutes Seconds Display
	frame.origin.x = 0;
	frame.origin.y = 110;
	frame.size.h = 30;
	
	text_layer_init(&countdownHMSTextLayer, frame);
	text_layer_set_background_color(&countdownHMSTextLayer, GColorClear);
	text_layer_set_text_alignment(&countdownHMSTextLayer, GTextAlignmentCenter);
	
	text_layer_set_font(&countdownHMSTextLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_APPLE_REGULAR_30)));
	layer_add_child(&window.layer, &countdownHMSTextLayer.layer);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t)
{
	(void)ctx;

	update_countdown();
}


void pbl_main(void *params)
{
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	  
	  .tick_info = {
		  .tick_handler = &handle_tick,
		  .tick_units = SECOND_UNIT
	  }
  };
  app_event_loop(params, &handlers);
}
