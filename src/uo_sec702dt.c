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
#include "uotime.h"

static int mlen[]={31,28,31,30,31,30,31,31,30,31,30,31};

/* 10 times faster than gmtime() */
int
uo_sec702dt(uo_datetime_t *dt,uo_sec70_t *t)
{
	/* this code is certainly not beautiful, but correct and fast. */
	int restsecs;
	int days;
	int schalt;
	int y;

	days=*t/86400;
	restsecs=*t%86400;

	dt->hour=restsecs/3600;restsecs%=3600;
	dt->min=restsecs/60;
	dt->sec=restsecs%60;
	y=1970+days/365; days%=365;
	days-=(y-1969)/4;
	{
		int x;
		x=(y-2000)/100;
		days+=x; /* (y-2000)/100; */
		days-=x/4; /* (y-2000)/400; */
	}
	/* gcc generates better code without jumps */
	schalt=(y%4==0 && !(y%400 !=0 && y%100==0));

	if (days<31) {dt->day=days+1; dt->mon=0;}
	else if (days<59+schalt) {dt->day=days-30; dt->mon=1;}
	else {
		int x;
		days=days-59-schalt; 
		dt->mon=2+(days*10+5)/306;
		x=306*(dt->mon-2)-5;
		if (x<0) {
			x+=5;
			days++;
			dt->day=days+(x/10);
		} else {
			dt->day=days-(x/10);
		}
	}
	if (dt->day<0) {
		int x=-dt->day;
		dt->mon--;
		if (dt->mon<0) {dt->mon=11;y--;}
		dt->day=mlen[dt->mon]-x;
	}
	if (dt->day==0) {
		dt->mon--;
		if (dt->mon<0) {dt->mon=11;y--;}
		dt->day=mlen[dt->mon];
	}
	dt->year=y;
	return 0;
}

#ifdef TEST

#include <time.h>

int main(void)
{
	uo_sec70_t x;
	unsigned long d;
	gmtime(&x);

#define DAY 86400
#define YEAR (DAY*365)

	for (x=0;x<YEAR*40;x+=DAY) {
		uo_datetime_t dt;
		struct tm *tm=gmtime((time_t *)&x);
		uo_sec702dt(&dt,&x);
		if (tm->tm_year+1900!=dt.year || tm->tm_mday!=dt.day || tm->tm_mon !=dt.mon) {
			printf("difference at %lx\n",x);
		}
	}

	start();
	for (x=0;x<YEAR*40;x+=DAY) {
		uo_datetime_t dt;
		uo_sec702dt(&dt,&x);
	}
	d=stop(); printf("took %lu\n",d);
	start();
	for (x=0;x<YEAR*40;x+=DAY) {
		gmtime((time_t *)&x);
	}
	d=stop(); printf("took %lu\n",d);
	exit(0);
}
#endif
