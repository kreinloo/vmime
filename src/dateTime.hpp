//
// VMime library (http://vmime.sourceforge.net)
// Copyright (C) 2002-2004 Vincent Richard <vincent@vincent-richard.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef VMIME_DATETIME_HPP_INCLUDED
#define VMIME_DATETIME_HPP_INCLUDED


#include "base.hpp"
#include "component.hpp"


namespace vmime
{


/** Date and time (basic type).
  */

class datetime : public component
{
public:

	// Constructors
	datetime();
	datetime(const int year, const int month, const int day);
	datetime(const int year, const int month, const int day, const int hour, const int minute, const int second, const int zone = GMT);
	datetime(const datetime& d);
	datetime(const string& date);

	// Destructor
	~datetime();

	// Some time zones (in minutes)
	enum TimeZones
	{
		GMT_12 = -720,   // GMT-12h
		GMT_11 = -660,   // GMT-11h
		GMT_10 = -600,   // GMT-10h
		GMT_9 = -540,    // GMT-9h
		GMT_8 = -480,    // GMT-8h
		GMT_7 = -420,    // GMT-7h
		GMT_6 = -360,    // GMT-6h
		GMT_5 = -300,    // GMT-5h
		GMT_4 = -240,    // GMT-4h
		GMT_3 = -180,    // GMT-3h
		GMT_2 = -120,    // GMT-2h
		GMT_1 = -60,     // GMT-1h
		GMT = 0,         // GMT
		GMT1 = 60,       // GMT+1h
		GMT2 = 120,      // GMT+2h
		GMT3 = 180,      // GMT+3h
		GMT4 = 240,      // GMT+4h
		GMT5 = 300,      // GMT+5h
		GMT6 = 360,      // GMT+6h
		GMT7 = 420,      // GMT+7h
		GMT8 = 480,      // GMT+8h
		GMT9 = 540,      // GMT+9h
		GMT10 = 600,     // GMT+10h
		GMT11 = 660,     // GMT+11h
		GMT12 = 720,     // GMT+12h

		UT = GMT,        // Universal Time

		EST = GMT_5,     // Eastern
		EDT = GMT_4,
		CST = GMT_6,     // Central
		CDT = GMT_5,
		MST = GMT_7,     // Mountain
		MDT = GMT_6,
		PST = GMT_8,     // Pacific
		PDT = GMT_7,

		// Military time zones
		A = GMT_1,
		B = GMT_2,
		C = GMT_3,
		D = GMT_4,
		E = GMT_5,
		F = GMT_6,
		G = GMT_7,
		H = GMT_8,
		I = GMT_9,       // J not used
		K = GMT_10,
		L = GMT_11,
		M = GMT_12,

		N = GMT1,
		O = GMT2,
		P = GMT3,
		Q = GMT4,
		R = GMT5,
		S = GMT6,
		T = GMT7,
		U = GMT8,
		V = GMT9,
		W = GMT10,
		X = GMT11,
		Y = GMT12,

		Z = GMT
	};

	// Months list
	enum Months
	{
		// Long
		JANUARY = 1,
		FEBRUARY = 2,
		MARCH = 3,
		APRIL = 4,
		MAY = 5,
		JUNE = 6,
		JULY = 7,
		AUGUST = 8,
		SEPTEMBER = 9,
		OCTOBER = 10,
		NOVEMBER = 11,
		DECEMBER = 12,

		// Short
		JAN = 1,
		FEB = 2,
		MAR = 3,
		APR = 4,
		JUN = 6,
		JUL = 7,
		AUG = 8,
		SEP = 9,
		OCT = 10,
		NOV = 11,
		DEC = 12
	};

	// Days of week list
	enum DaysOfWeek
	{
		// Long
		SUNDAY = 0,
		MONDAY = 1,
		TUESDAY = 2,
		WEDNESDAY = 3,
		THURSDAY = 4,
		FRIDAY = 5,
		SATURDAY = 6,

		// Short
		SUN = 0,
		MON = 1,
		TUE = 2,
		WED = 3,
		THU = 4,
		FRI = 5,
		SAT = 6
	};

private:

	// Date components
	int m_year;
	int m_month;
	int m_day;

	// Time components
	int m_hour;
	int m_minute;
	int m_second;
	int m_zone;

public:

	// Get
	const int getYear() const;
	const int getMonth() const;
	const int getDay() const;
	const int getHour() const;
	const int getMinute() const;
	const int getSecond() const;
	const int getZone() const;

	void getTime(int& hour, int& minute, int& second, int& zone) const;
	void getTime(int& hour, int& minute, int& second) const;
	void getDate(int& year, int& month, int& day) const;

	// Set
	void setYear(const int year);
	void setMonth(const int month);
	void setDay(const int day);
	void setHour(const int hour);
	void setMinute(const int minute);
	void setSecond(const int second);
	void setZone(const int zone);

	void setTime(const int hour = 0, const int minute = 0, const int second = 0, const int zone = GMT);
	void setDate(const int year, const int month, const int day);

	// Assignment
	datetime& operator=(const datetime& other);
	datetime& operator=(const string& s);

	void copyFrom(const component& other);

	datetime* clone() const;

	// Current date and time
	static const datetime now();

private:

	static const int dayOfWeek(const int year, const int month, const int day);

public:

	using component::parse;
	using component::generate;

	// Component parsing & assembling
	void parse(const string& buffer, const string::size_type position, const string::size_type end, string::size_type* newPosition = NULL);
	void generate(utility::outputStream& os, const string::size_type maxLineLength = lineLengthLimits::infinite, const string::size_type curLinePos = 0, string::size_type* newLinePos = NULL) const;
};


} // vmime


#endif // VMIME_DATETIME_HPP_INCLUDED