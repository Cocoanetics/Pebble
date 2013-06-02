#include "global.h"

#include "mini-printf.h"
#include "time.h"

/**
 Global Variables
 */

// UI Elements
Window			window;
BmpContainer	imageContainer;
TextLayer		countdownDaysTextLayer;
TextLayer		countdownHMSTextLayer;

// UTC Handling
int current_UTC_offset = UTC_OFFSET_INITIAL;
int show_UTC_counter;

// needs to be global because of delayed drawing
char dayLineBuffer[30];
char timeLineBuffer[30];


PblTm keynoteTime()
{
	PblTm keynote_time = { .tm_mday = 10, .tm_mon = 6, .tm_year = 2013, .tm_hour = 10, .tm_min = 0, .tm_sec = 0 };

	// adjust keynote time to current UTC offset
	keynote_time.tm_hour += (MAX(current_UTC_offset, UTC_OFFSET_SFO) - MIN(current_UTC_offset, UTC_OFFSET_SFO));
	
	return keynote_time;
}


void update_time(PblTm time)
{
	PblTm keynote_time = keynoteTime();
	
	int conference_day = time.tm_mday-keynote_time.tm_mday + 1;
	
	if (conference_day<=5)
	{
		snprintf(dayLineBuffer, sizeof(dayLineBuffer), "Day %d", conference_day);
	}
	else
	{
		string_format_time(dayLineBuffer, sizeof(dayLineBuffer), "%B %e", &time);
	}
	
	text_layer_set_text(&countdownDaysTextLayer, dayLineBuffer);
	
	char *time_format;
	
	if (clock_is_24h_style())
	{
		time_format = "%R:%S";
	}
	else
	{
		time_format = "%I:%M:%S";
	}
	
	string_format_time(timeLineBuffer, sizeof(timeLineBuffer), time_format, &time);
	
	// Kludge to handle lack of non-padded hour format string
	// for twelve hour clock.
	if (!clock_is_24h_style() && (timeLineBuffer[0] == '0'))
	{
		memmove(timeLineBuffer, &timeLineBuffer[1], sizeof(timeLineBuffer) - 1);
	}
	
	text_layer_set_text(&countdownHMSTextLayer, timeLineBuffer);
}


void update_countdown()
{
	
	PblTm now;
	get_time(&now);
	
	PblTm keynote_time = keynoteTime();
	
	long seconds_til_keynote = timeintervalBetweenTimes(keynote_time, now);
	long days = seconds_til_keynote / (24*60*60);
	
	if (seconds_til_keynote<0)
	{
		update_time(now);
		return;
	}
	else if (seconds_til_keynote==0)
	{
		vibes_long_pulse();
	}
	
	if (show_UTC_counter>0)
	{
		if (current_UTC_offset>=0)
		{
			snprintf(dayLineBuffer, sizeof(dayLineBuffer), "UTC + %d", current_UTC_offset);
		}
		else
		{
			snprintf(dayLineBuffer, sizeof(dayLineBuffer), "UTC - %d", -current_UTC_offset);
		}
	}
	else
	{
		if (days==1)
		{
			strcpy(dayLineBuffer, "1 day");
		}
		else
		{
			snprintf(dayLineBuffer, sizeof(dayLineBuffer), "%d days", days);
		}
	}
	
	text_layer_set_text(&countdownDaysTextLayer, dayLineBuffer);

	seconds_til_keynote -= days * (24*60*60);
	
	snprintf(timeLineBuffer, sizeof(timeLineBuffer), "%02d:%02d:%02d", (seconds_til_keynote/3600), (seconds_til_keynote/60)%60, seconds_til_keynote%60);
	
	text_layer_set_text(&countdownHMSTextLayer, timeLineBuffer);
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window)
{
	(void)recognizer;
	(void)window;
	
	current_UTC_offset = current_UTC_offset + 1;
	
	if (current_UTC_offset>12)
	{
		current_UTC_offset=-12;
	}
	
	show_UTC_counter = 3;
	update_countdown();
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window)
{
	(void)recognizer;
	(void)window;
	
	current_UTC_offset = current_UTC_offset - 1;
	
	if (current_UTC_offset<-12)
	{
		current_UTC_offset=12;
	}

	show_UTC_counter = 3;
	update_countdown();
}

void config_provider(ClickConfig **config, Window *window)
{
	(void)window;
	
	config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
	config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
	
	config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
	config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

void handle_init(AppContextRef ctx)
{
	(void)ctx;

	window_init(&window, "WWDC 2013 Countdown");
	window_set_fullscreen(&window, true); // standard app without status bar
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
	
	// Attach our desired button functionality
	window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t)
{
	(void)ctx;

	update_countdown();
	
	if (show_UTC_counter>0)
	{
		show_UTC_counter--;
	}
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
