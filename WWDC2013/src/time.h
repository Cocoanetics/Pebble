//
//  time.h
//  WWDC2013
//
//  Created by Oliver Drobnik on 6/2/13.
//
//

#ifndef WWDC2013_time_h
#define WWDC2013_time_h

// get a time stamp from a pebble time struct
long long timestampFromTime(PblTm time);

// calculate number of seconds between two time stamps
long long timeintervalBetweenTimes(PblTm time1, PblTm time0);

#endif
