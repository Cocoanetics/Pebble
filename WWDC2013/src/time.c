//
//  time.c
//  WWDC2013
//
//  Created by Oliver Drobnik on 6/2/13.
//
//

#include "pebble_os.h"

// get a time stamp from a pebble time struct
long long timestampFromTime(PblTm time)
{
	return (((time.tm_mday * 24) + time.tm_hour) * 60 + time.tm_min) * 60 + time.tm_sec;
}

// calculate number of seconds between two time stamps
long long timeintervalBetweenTimes(PblTm time1, PblTm time0)
{
	long long timestamp1 = timestampFromTime(time1);
	long long timestamp0 = timestampFromTime(time0);
	
	return timestamp1 - timestamp0;
}