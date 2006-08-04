/* -*- indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */
/*
  log.h

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

#ifndef _LOG_SYSTEM_
#define _LOG_SYSTEM_

#define MAX_TIME_LENGTH 64

/* Global vars */
FILE *logfile;       /* Filename                     */
struct timeval *tv;  /* Date                         */
struct timezone *tz; /* Timezone                     */
struct tm *bt;       /* "broken time" (for humans)   */
char log_time[MAX_TIME_LENGTH];


extern void init_log_system(char *filename, char *timezone);
extern void log_message(const char *message);
extern void end_log_system();


#endif /* _LOG_SYSTEM_ */
