/* -*- indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */
/*
  cdcap.h 
  cdcap - Which device is capable of writing cd, dvd, ... ?

  (C) Copyright 2006 Olivier Delhomme
  e-mail : olivier.delhomme@free.fr
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or  (at your option) 
  any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY  or  FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
*/ 

#ifndef _CDCAP_
#define _CDCAP_

#define ProgName "cdcap"
#define ProgVersion "0.0.1"
#define ProgDate "19.07.2006"
#define ProgAuthor "Olivier Delhomme (DCPJ/SDPTS/SITT/LATS)"

#define SHOW_VERSION -2

#define TRUE 0
#define FALSE 1

#define MAX_DEVICE_NAME_LENGTH 8

static struct option const long_options[] =
{
	{"quiet", no_argument, NULL, 'q'},
	{"debug", no_argument, NULL, 'd'},
	{"version", no_argument, NULL, 'v'},
	{"help", no_argument, NULL, 'h'},
	{"cap", required_argument, NULL, 'c'},
	{"list", no_argument, NULL, 'l'},
	{NULL, 0, NULL, 0}
};


typedef struct 
{
	uint debug;
	uint quiet;
	uint version;
	uint list;
} prog_options_t;



typedef struct
{
	char name[MAX_DEVICE_NAME_LENGTH]; /* device name */
	unsigned int speed;    /* drive speed (eg 24x) */
    unsigned int n_slots;  /* number of slot(s)    */
	unsigned short close;
    unsigned short open;
	unsigned short lock; 
	unsigned short chg_speed;
	unsigned short sel_disk;
	unsigned short read_multi;
	unsigned short read_MCN;
	unsigned short media_chg;
	unsigned short play_audio;
	unsigned short cdr;
	unsigned short cdrw;
	unsigned short read_dvd;
	unsigned short dvdr;
	unsigned short dvdram;
	unsigned short read_MRW;
	unsigned short writ_MRW;
	unsigned short writ_RAM;
} device_cap_t;


typedef struct device_list_t
{
	device_cap_t cap;
	struct device_list_t *next;
} device_list_t;


#endif /* _CDCAP_ */
