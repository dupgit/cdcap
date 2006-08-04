/* -*- indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */
/*
  log.c

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

#define _GNU_SOURCE

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "log.h"


void init_log_system(char *filename, char *timezone)
{
  tv = (struct timeval *) malloc (sizeof(struct timeval));
  tz = (struct timezone *) malloc (sizeof(struct timezone));
  
  setlocale(LC_TIME, timezone);

  if ((logfile = fopen(filename, "a+")) != NULL)
    {
      log_message("Démarrage du fichier journal");
    }
}


void log_message(const char *message)
{

  gettimeofday(tv, tz);
  bt = localtime(&tv->tv_sec);
  strftime(log_time, MAX_TIME_LENGTH-1, "%A %d %B %Y %H:%M:%S", bt); 

  if (logfile)
    {
		fprintf(logfile, "%s : %s\n", log_time, message);
		fflush(logfile);
    }

}


void end_log_system()
{
  log_message("Terminaison du fichier journal");

  if (logfile)
    fclose(logfile);

  free(tv);
  free(tz);
}
