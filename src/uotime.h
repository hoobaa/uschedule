/*
 * Copyright (C) 1999 Uwe Ohse
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * As a special exception this source may be used as part of the
 * SRS project by CORE/Computer Service Langenbach
 * regardless of the copyright they choose.
 * 
 * Contact: uwe@ohse.de
 */
#ifndef UOTIME_H
#define UOTIME_H

/* this is frustration: why can't vendors provide usable time header files, damned? */

typedef unsigned long uo_sec70_t; /* secs since 1970 */
typedef struct {
	long year;
	short mon; /* 0..11 */
	short day; /* 1..31 */
	short hour;
	short min;
	short sec;
} uo_datetime_t;

uo_sec70_t uo_now(void);
uo_sec70_t uo_dt2sec70 (uo_datetime_t *tm);
int uo_sec702dt(uo_datetime_t *dt, uo_sec70_t *);
const char * uo_monabbrev(int mon);
#define YYYY_MM_DD0 11
char *yyyy_mm_dd(char *buf, uo_datetime_t *dt, char dash);
#define YYYY_MM_DD_HH_MM_SS0 20
char *yyyy_mm_dd_hh_mm_ss(char *buf, uo_datetime_t *dt, char dash);
int uo_scandate(uo_datetime_t *dt, const char *format, const char *data);


#endif
