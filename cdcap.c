/* -*- indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */
/*
  cdcap.c 
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

#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64

#define LINE_BUFFER_SIZE 256

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cdcap.h"
#include "log.h"

static device_list_t *cons_device_list(device_list_t *list);
static device_list_t *read_from_file(prog_options_t *opt, device_list_t *device_list);
static int ZeroMemory(char *buffer, unsigned int length);
static void print_device_value(unsigned short val);
static void print_device_list(device_list_t *device_list);

void usage(int status)
{
	if (status == SHOW_VERSION)
		{
			fprintf(stderr, "%s version %s (%s) by %s\n", ProgName, ProgVersion, ProgDate, ProgAuthor);
			exit(TRUE);
		}
	else
		{
			fprintf(stderr, "cdcap should return device names regarding there capabilities\n");
			fprintf(stderr, "Usage : %s [OPTIONS]\n", ProgName);
			fprintf(stderr, "Options :\n");
			fprintf(stderr, " -q, --quiet   : no output\n");
			fprintf(stderr, " -d, --debug   : verbose option usefull to debug\n");
			fprintf(stderr, " -v, --version : prints version info and exit\n");
			fprintf(stderr, " -h, --help    : prints this help and exit\n");
			fprintf(stderr, " -c, --cap=CAP : finds (if it exists) the device that has the\n");
			fprintf(stderr, "                 capabilities CAP wich can be one of :\n");
			fprintf(stderr, "                 cdr    : cd recordable capabilities\n");
			fprintf(stderr, "                 cdrw   : cd rewritable capabilities\n");
			fprintf(stderr, "                 dvdr   : dvd recordable capabilities\n");
		/*	fprintf(stderr, "                 dvdrw  : dvd rewritable capabilities\n"); */
			fprintf(stderr, "                 dvdram : dvd ram capabilities\n");
			exit(status);
		}
}



/* Kills the memory ! */
int ZeroMemory(char *buffer, unsigned int length)
{
	/* Memory begins at 0 so there is length-1 chars */
	if (buffer)
		{
			while(length>0)
				buffer[--length] = 0;
			return(TRUE);
		}
	else
		{
			return(FALSE);
		}
}


/* read a line from a file */
int read_a_line(FILE *info, char *buffer, unsigned int max_length)
{
	unsigned int i = 0;
	char c = 0;

	while (!feof(info) && i<max_length && c!=10)
		{
			fread( &c, 1, 1, info); 
			if (c!=10)
				{
					buffer[i] = c;
					i++;
				}
		}
	buffer[i] = 0;

	return(i);
}

void print_device_value(unsigned short val)
{
	if (val == TRUE)
		fprintf(stdout, " Yes  ");
	else
		fprintf(stdout, " No   ");
}


void print_device_list(device_list_t *device_list)
{
	device_list_t *list = device_list;

	fprintf(stdout, "device speed slots close open  lock  speed selct r_mul r_mcn m_chg r_ply cdr   cdrw  r_dvd dvdr  dvdrm r_mrw w_mrw w_ram\n");
	while (list)
		{
			
            fprintf(stdout, "  %s     %d    %d  ", list->cap.name, list->cap.speed, list->cap.n_slots);
			print_device_value(list->cap.close);
			print_device_value(list->cap.open);
			print_device_value(list->cap.lock);
			print_device_value(list->cap.chg_speed);
			print_device_value(list->cap.sel_disk);
			print_device_value(list->cap.read_multi);
			print_device_value(list->cap.read_MCN);
			print_device_value(list->cap.media_chg);
			print_device_value(list->cap.play_audio);
			print_device_value(list->cap.cdr);
			print_device_value(list->cap.cdrw);
			print_device_value(list->cap.read_dvd);
			print_device_value(list->cap.dvdr);
			print_device_value(list->cap.dvdram);
			print_device_value(list->cap.read_MRW);
			print_device_value(list->cap.writ_MRW);
			print_device_value(list->cap.writ_RAM);
			
			list = list->next;
			fprintf(stdout, "\n");
		}
}


void set_device_value(unsigned short *val, char c)
{

	if (c == '1')
		*val = TRUE;
	else 
		*val = FALSE;

}


device_list_t *split_line(char *line, unsigned int max_length, device_list_t *device_list)
{
	char capability[32];
	unsigned int i = 0;
	unsigned int j = 0;
	device_list_t *list = device_list;
	device_list_t *head = NULL;

	unsigned int devices = TRUE;
	char value[8];

	if (!list)
		list = cons_device_list(list);
	head = list;

	while (i<max_length && line[i]!=':')
		{
			capability[i] = line[i];
			i++;
		}
	capability[i] = 0;
	
	while (devices == TRUE)
		{
			while (i<max_length && (line[i]==9 || line[i]==':')) /* get rid of the [tab] character(s). */
				i++; 

			if (strcmp(capability, "drive name") == 0)
				{	
					j = 0;
					while(i<max_length && line[i]!=9 && line[i]!=0)
						list->cap.name[j++] = line[i++];
						
					list->cap.name[j] = 0;
				}
			else if (strcmp(capability, "drive speed") == 0)
				{
					j = 0;
					while(i<max_length && line[i]!=9 && line[i]!=0)
						value[j++] = line[i++];

					value[j] = 0;	
					if (sscanf(value, "%d", &list->cap.speed) < 1)
						list->cap.speed = 0;			
				}
			else if (strcmp(capability,"drive # of slots") == 0)
				{
					j = 0;
					while(i<max_length && line[i]!=9 && line[i]!=0)
						value[j++] = line[i++];

					value[j] = 0;	
					if (sscanf(value, "%d", &list->cap.n_slots) < 1)
						list->cap.n_slots = 0;
				}
			else if (strcmp(capability,"Can close tray") == 0)
				set_device_value(&list->cap.close, line[i++]);
			else if (strcmp(capability,"Can open tray") == 0)
				set_device_value(&list->cap.open, line[i++]);
			else if (strcmp(capability,"Can lock tray") == 0)
				set_device_value(&list->cap.lock, line[i++]);
			else if (strcmp(capability,"Can change speed") == 0)
				set_device_value(&list->cap.chg_speed, line[i++]);
			else if (strcmp(capability,"Can select disk") == 0)
				set_device_value(&list->cap.sel_disk, line[i++]);
			else if (strcmp(capability,"Can read multisession") == 0)
				set_device_value(&list->cap.read_multi, line[i++]);
			else if (strcmp(capability,"Can read MCN") == 0)
				set_device_value(&list->cap.read_MCN, line[i++]);
			else if (strcmp(capability,"Reports media changed") == 0)
				set_device_value(&list->cap.media_chg, line[i++]);
			else if (strcmp(capability,"Can play audio") == 0)
				set_device_value(&list->cap.play_audio, line[i++]);
			else if (strcmp(capability,"Can write CD-R") == 0)
				set_device_value(&list->cap.cdr, line[i++]);
			else if (strcmp(capability,"Can write CD-RW") == 0)
				set_device_value(&list->cap.cdrw, line[i++]);
			else if (strcmp(capability,"Can read DVD") == 0)
				set_device_value(&list->cap.read_dvd, line[i++]);
			else if (strcmp(capability,"Can write DVD-R") == 0)
				set_device_value(&list->cap.dvdr, line[i++]);
			else if (strcmp(capability,"Can write DVD-RAM") == 0)
				set_device_value(&list->cap.dvdram, line[i++]);
			else if (strcmp(capability,"Can read MRW") == 0)
				set_device_value(&list->cap.read_MRW, line[i++]);
			else if (strcmp(capability,"Can write MRW") == 0)
				set_device_value(&list->cap.writ_MRW, line[i++]);
			else if (strcmp(capability,"Can write RAM") == 0)
				set_device_value(&list->cap.writ_RAM, line[i++]);
			else
				devices = FALSE;

			if (i>=max_length || line[i] == 0)
				devices = FALSE;
			else
				{
					if (!list->next)
						list->next = cons_device_list(NULL);
					list = list->next;
				}
		}

	return(head);
}


/* read from the /proc/sys/dev/cdrom/info file */
device_list_t *read_from_file(prog_options_t *opt, device_list_t *device_list)
{
	FILE *info = NULL;
	char *buffer = NULL;
	unsigned int line_size = 0;

	info = fopen("/proc/sys/dev/cdrom/info", "r");
	if( info != NULL )
		{
			if (opt->debug == TRUE)
				log_message("/proc/sys/dev/cdrom/info opened");
			
			buffer = (char *) malloc(LINE_BUFFER_SIZE*sizeof(char));

			/* first two lines out of interest here */
			read_a_line(info, buffer, LINE_BUFFER_SIZE);
			log_message(buffer);
			read_a_line(info, buffer, LINE_BUFFER_SIZE);
			log_message(buffer);

			while (!feof(info))
				{
					ZeroMemory(buffer, LINE_BUFFER_SIZE);
					line_size = read_a_line(info, buffer, LINE_BUFFER_SIZE);
					log_message(buffer);
					device_list = split_line(buffer, line_size, device_list);	
				}

			return(device_list);
				
		}
	else
		{
			log_message("Error while openning /proc/sys/dev/cdrom/info");
			return(NULL);
		}
}


device_list_t * cons_device_list(device_list_t *list)
{
	device_list_t *new = NULL;
	
	new = (device_list_t *) malloc(sizeof(device_list_t));
	if (!new)
		log_message("Error : no more memory !!");
	else
		{
			sprintf(new->cap.name, "%c%c%c%c", '\0', '\0', '\0', '\0');
			new->cap.speed = 0;
			new->cap.n_slots = 0;
			new->cap.close = 0;
			new->cap.open = 0;
			new->cap.lock = 0;
			new->cap.chg_speed = 0;
			new->cap.sel_disk = 0;
			new->cap.read_multi = 0;
			new->cap.read_MCN = 0;
			new->cap.media_chg = 0;
			new->cap.play_audio = 0;
			new->cap.cdr = 0;
			new->cap.cdrw = 0;
			new->cap.read_dvd = 0;
			new->cap.dvdr = 0;
			new->cap.dvdram = 0;
			new->cap.read_MRW = 0;
			new->cap.writ_MRW = 0;
			new->cap.writ_RAM = 0;
			new->next = list;
		}
	
	return new;
}

void find_devices(device_list_t *device_list, char *what)
{
	device_list_t *list = device_list;
	
	while (list)
		{
			if (strcmp(what, "cdr") == 0)
				{
					if (list->cap.cdr == TRUE)
						fprintf(stdout, "%s ", list->cap.name);
				}
			else if (strcmp(what, "cdrw") == 0)
				{
					if (list->cap.cdrw == TRUE)
						fprintf(stdout, "%s ", list->cap.name);
				}
			else if (strcmp(what, "dvdr") == 0)
				{
					if (list->cap.dvdr == TRUE)
						fprintf(stdout, "%s ", list->cap.name);
				}
			else if (strcmp(what, "dvdram") == 0)
				{
					if (list->cap.dvdram == TRUE)
						fprintf(stdout, "%s ", list->cap.name);
				}
			list = list->next;
		}
	fprintf(stdout, "\n");
}


int main(int argc, char *argv[])
{
	prog_options_t *opt = NULL;
	device_list_t *device_list = NULL;
	int c = 0;
	char *what = NULL;

	init_log_system("./cdcap.log", "fr_FR@euro");
	/* Gestion des options du programme */
	opt = (prog_options_t *) malloc(sizeof(prog_options_t));
	if (!opt)
		log_message("Error : no more memory !!");

	opt->debug = TRUE;  /* We are in a debugging stage */
	opt->quiet = FALSE;
	opt->list = FALSE;

	while ((c = getopt_long(argc, argv, "qdvhlc:", long_options, NULL)) != -1)
		{
			switch (c)
				{
				case 0:			/* Long option. */
					break; 

				case 'd':
					opt->debug = TRUE;
					opt->quiet = FALSE;
					break;

				case 'q':
					opt->debug = FALSE;
					opt->quiet = TRUE;
					break;
			
				case 'l':
					opt->list = TRUE;
					break;

				case 'v':
					usage(SHOW_VERSION);
					break;

				case 'h':
					usage(TRUE);
					break;
				
				case 'c':
					if (optarg)
						{
							what = (char *) malloc(sizeof(char) * (strlen(optarg)+1));
							if (!what)
								log_message("Error : no more memory !!");
							if (sscanf(optarg, "%s", what) < 1)
								sprintf(what, "cdr%c", '\0');
						}
					else
						{
							usage(FALSE);
						}
					break;

				default:
					usage(FALSE); 
				}
		}

	device_list = read_from_file(opt, device_list);

	if (opt->list == TRUE)
		print_device_list(device_list);

	find_devices(device_list, what);

	end_log_system(); 

	return 0;
}
