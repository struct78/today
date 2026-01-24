// environment.h - Configurable environment settings
#pragma once

// =============================================================================
// TIMEZONE CONFIGURATION
// =============================================================================
// Use Olson timezone name (for reference)
// See: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones

const char TIMEZONE[] = "Australia/Melbourne";

// POSIX timezone string (avoids network lookup for timezone data)
// Format: STDoffset[DST[offset],start,end]
// Australia/Melbourne: AEST-10AEDT,M10.1.0,M4.1.0/3
// - AEST = standard time name, -10 = UTC+10 (note: POSIX uses inverted sign)
// - AEDT = daylight time name
// - M10.1.0 = DST starts first Sunday of October
// - M4.1.0/3 = DST ends first Sunday of April at 3am
const char TIMEZONE_POSIX[] = "AEST-11AEDT,M10.1.0,M4.1.0/3";

