/*
 * JTAG Core library
 * Copyright (c) 2008-2021 Viveris Technologies
 *
 * JTAG Core library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * JTAG Core library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with JTAG Core library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
* @file   script.c
* @brief  command line parser.
* @author Jean-François DEL NERO <Jean-Francois.DELNERO@viveris.fr>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#define MAX_PATH 256
#define DEFAULT_BUFLEN 1024
#define DIR_SEPARATOR "\\"
#define DIR_SEPARATOR_CHAR '\\'

#include "../drivers/drv_loader.hxx"
#include "jtag_core_internal.hxx"
#include "jtag_core.hxx"
#include "../bsdl_parser/bsdl_loader.hxx"

typedef int (* CMD_FUNC)( jtag_core * jc, char * line);

typedef struct filefoundinfo_
{
	int isdirectory;
	char filename[256];
	int size;
}filefoundinfo;

typedef struct cmd_list_
{
	char * command;
	CMD_FUNC func;
}cmd_list;

#define MAX_CFG_STRING_SIZE 1024

typedef struct envvar_entry_
{
	char * name;
	char * varvalue;
}envvar_entry;

int jtagcore_setEnvVar( jtag_core * jc, char * varname, char * varvalue )
{
	int i;
	envvar_entry * tmp_envvars;

	i = 0;

	tmp_envvars = (envvar_entry *)jc->envvar;

	if(!tmp_envvars)
	{
		tmp_envvars = malloc(sizeof(envvar_entry) );
		if(!tmp_envvars)
			return -1;

		memset( tmp_envvars,0,sizeof(envvar_entry));

		jc->envvar = (void*)tmp_envvars;
	}

	// is the variable already there
	while(tmp_envvars[i].name)
	{
		if(!strcmp(tmp_envvars[i].name,varname) )
		{
			break;
		}
		i++;
	}

	if(tmp_envvars[i].name)
	{
		// the variable already exist - update it.
		if(tmp_envvars[i].varvalue)
		{
			free(tmp_envvars[i].varvalue);
			tmp_envvars[i].varvalue = NULL;
		}

		if(varvalue)
		{
			tmp_envvars[i].varvalue = malloc(strlen(varvalue)+1);

			if(!tmp_envvars[i].varvalue)
				return -1;

			memset(tmp_envvars[i].varvalue,0,strlen(varvalue)+1);
			if(varvalue)
				strcpy(tmp_envvars[i].varvalue,varvalue);
		}
	}
	else
	{
		// No variable found, alloc an new entry
		if(strlen(varname))
		{
			tmp_envvars[i].name = malloc(strlen(varname)+1);
			if(!tmp_envvars[i].name)
				return -1;

			memset(tmp_envvars[i].name,0,strlen(varname)+1);
			strcpy(tmp_envvars[i].name,varname);

			if(varvalue)
			{
				tmp_envvars[i].varvalue = malloc(strlen(varvalue)+1);

				if(!tmp_envvars[i].varvalue)
					return -1;

				memset(tmp_envvars[i].varvalue,0,strlen(varvalue)+1);
				if(varvalue)
					strcpy(tmp_envvars[i].varvalue,varvalue);
			}

			tmp_envvars = realloc(tmp_envvars,sizeof(envvar_entry) * (i + 1 + 1));
			memset(&tmp_envvars[i + 1],0,sizeof(envvar_entry));
		}
	}

	jc->envvar = (void*)tmp_envvars;

	return 1;
}

char * jtagcore_getEnvVar( jtag_core * jc, char * varname, char * varvalue)
{
	int i;
	envvar_entry * tmp_envvars;

	i = 0;

	tmp_envvars = (envvar_entry *)jc->envvar;
	if(!tmp_envvars)
		return NULL;

	// search the variable...
	while(tmp_envvars[i].name)
	{
		if(!strcmp(tmp_envvars[i].name,varname) )
		{
			break;
		}
		i++;
	}

	if(tmp_envvars[i].name)
	{
		if(varvalue)
			strcpy(varvalue,tmp_envvars[i].varvalue);

		return tmp_envvars[i].varvalue;
	}
	else
	{
		return NULL;
	}
}

int jtagcore_getEnvVarValue( jtag_core * jc, char * varname)
{
	int value;
	char * str_return;

	value = 0;

	if(!varname)
		return 0;

	str_return = jtagcore_getEnvVar( jc, varname, NULL);

	if(str_return)
	{
		if( strlen(str_return) > 2 )
		{
			if( str_return[0]=='0' && ( str_return[0]=='x' || str_return[0]=='X'))
			{
				value = (int)strtol(str_return, NULL, 0);
			}
			else
			{
				value = atoi(str_return);
			}
		}
		else
		{
			value = atoi(str_return);
		}
	}

	return value;
}

static int is_end_line(char c)
{
	if( c == 0 || c == '#' || c == '\r' || c == '\n' )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int is_space(char c)
{
	if( c == ' ' || c == '\t' )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int get_next_word(char * line, int offset)
{
	while( !is_end_line(line[offset]) && ( line[offset] == ' ' ) )
	{
		offset++;
	}

	return offset;
}

static int copy_param(char * dest, char * line, int offs)
{
	int i,insidequote;

	i = 0;
	insidequote = 0;
	while( !is_end_line(line[offs]) && ( insidequote || !is_space(line[offs]) ) && (i < (DEFAULT_BUFLEN - 1)) )
	{
		if(line[offs] != '"')
		{
			if(dest)
				dest[i] = line[offs];

			i++;
		}
		else
		{
			if(insidequote)
				insidequote = 0;
			else
				insidequote = 1;
		}

		offs++;
	}

	if(dest)
		dest[i] = 0;

	return offs;
}

static int get_param_offset(char * line, int param)
{
	int param_cnt, offs;

	offs = 0;
	offs = get_next_word(line, offs);

	param_cnt = 0;
	do
	{
		offs = copy_param(NULL, line, offs);

		offs = get_next_word( line, offs );

		if(line[offs] == 0 || line[offs] == '#')
			return -1;

		param_cnt++;
	}while( param_cnt < param );

	return offs;
}

static int get_param(char * line, int param_offset,char * param)
{
	int offs;

	offs = get_param_offset(line, param_offset);

	if(offs>=0)
	{
		offs = copy_param(param, line, offs);

		return 1;
	}

	return -1;
}

static int cmd_autoinit( jtag_core * jc, char * line)
{
	int number_of_devices, dev_nb;
	int loaded_bsdl;
	char szExecPath[MAX_PATH + 1];
	char filename[MAX_PATH + 1];
	char entityname[DEFAULT_BUFLEN];
	char file[MAX_PATH + 1];

	filefoundinfo fileinfo;
	void* h_file_find;

	unsigned long chip_id;

	loaded_bsdl = 0;

	// BSDL Auto load : check which bsdl file match with the device
	// And load it.

	jtagcore_scan_and_init_chain(jc);

	number_of_devices = jtagcore_get_number_of_devices(jc);

	strcpy(filename,szExecPath);
	strcat(filename,DIR_SEPARATOR"bsdl_files"DIR_SEPARATOR);


	return loaded_bsdl;
}

static int cmd_print( jtag_core * jc, char * line)
{
	int i;

	i = get_param_offset(line, 1);
	if(i>=0)

	return JTAG_CORE_NO_ERROR;
}

static int cmd_pause( jtag_core * jc, char * line)
{
	int i;
	char delay_str[DEFAULT_BUFLEN];

	i = get_param(line, 1,delay_str);

	if(i>=0)
	{
		//genos_pause(atoi(delay_str));

		return JTAG_CORE_NO_ERROR;
	}


	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_init_and_scan( jtag_core * jc, char * line)
{
	int ret;

	ret = jtagcore_scan_and_init_chain(jc);

	if( ret == JTAG_CORE_NO_ERROR )
	{

		return JTAG_CORE_NO_ERROR;
	}
	else
	{
	}

	return ret;
}

static int cmd_print_nb_dev( jtag_core * jc, char * line)
{
	int i;

	i = jtagcore_get_number_of_devices(jc);


	return JTAG_CORE_NO_ERROR;
}

static int cmd_print_devs_list( jtag_core * jc, char * line)
{
	int i;

	i = jtagcore_get_number_of_devices(jc);
	if(i>0)
	{
	}

	return JTAG_CORE_NOT_FOUND;
}

static int cmd_print_probes_list( jtag_core * jc, char * line)
{
	int i,j;
	char probe_list[64];
	int nb_of_drivers,nb_of_probes;

	nb_of_drivers = jtagcore_get_number_of_probes_drv(jc);
	j = 0;
	while (j < nb_of_drivers)
	{
		nb_of_probes = jtagcore_get_number_of_probes(jc, j);
		i = 0;
		while( i < nb_of_probes )
		{
			jtagcore_get_probe_name(jc, PROBE_ID(j,i), probe_list);
			i++;
		}
		j++;
	}

	return JTAG_CORE_NO_ERROR;
}

static int cmd_open_probe( jtag_core * jc, char * line)
{
	int ret;
	char probe_id[64];

	if(get_param(line, 1,probe_id)>0)
	{
		ret = jtagcore_select_and_open_probe(jc, strtol(probe_id, NULL, 16));
		if(ret != JTAG_CORE_NO_ERROR)
		{
			return ret;
		}
		else
		{
			return JTAG_CORE_NO_ERROR;
		}
	}
	else
	{

		return JTAG_CORE_BAD_PARAMETER;
	}
}

static int cmd_load_bsdl( jtag_core * jc, char * line)
{
	int i,j;
	char dev_index[DEFAULT_BUFLEN];
	char filename[DEFAULT_BUFLEN];

	i = get_param(line, 1,filename);
	j = get_param(line, 2,dev_index);

	if(i>=0 && j>=0)
	{
		if (jtagcore_loadbsdlfile(jc, filename, atoi(dev_index)) >= 0)
		{
			return JTAG_CORE_NO_ERROR;
		}
		else
		{
			return JTAG_CORE_ACCESS_ERROR;
		}
	}


	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_set_scan_mode( jtag_core * jc, char * line)
{
	int i,j;
	char dev_index[DEFAULT_BUFLEN];
	char scan_mode[DEFAULT_BUFLEN];

	i = get_param(line, 1,dev_index);
	j = get_param(line, 2,scan_mode);

	if(i>=0 && j>=0)
	{
		if( !strcmp(scan_mode,"EXTEST") )
		{
			jtagcore_set_scan_mode(jc, atoi(dev_index),JTAG_CORE_EXTEST_SCANMODE);
		}
		else
		{
			if( !strcmp(scan_mode,"SAMPLE") )
			{
				jtagcore_set_scan_mode(jc, atoi(dev_index),JTAG_CORE_SAMPLE_SCANMODE);

			}
			else
			{
				return JTAG_CORE_BAD_PARAMETER;
			}
		}

		return JTAG_CORE_NO_ERROR;
	}


	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_push_and_pop( jtag_core * jc, char * line)
{
	int ret;

	ret = jtagcore_push_and_pop_chain(jc, JTAG_CORE_WRITE_READ);

	if(ret != JTAG_CORE_NO_ERROR)
	{
		return ret;
	}
	else
	{
	}

	return JTAG_CORE_NO_ERROR;
}

static int cmd_set_pin_mode( jtag_core * jc, char * line)
{
	int i,j,k,id;
	char dev_index[DEFAULT_BUFLEN];
	char pinname[DEFAULT_BUFLEN];
	char mode[DEFAULT_BUFLEN];

	i = get_param(line, 1,dev_index);
	j = get_param(line, 2,pinname);
	k = get_param(line, 3,mode);

	if(i>=0 && j>=0 && k>=0)
	{
		id = jtagcore_get_pin_id(jc, atoi(dev_index), pinname);

		if(id>=0)
		{
			jtagcore_set_pin_state(jc, atoi(dev_index), id, JTAG_CORE_OE, atoi(mode));


			return JTAG_CORE_NO_ERROR;
		}
		else
		{
			return JTAG_CORE_NOT_FOUND;
		}
	}


	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_set_pin_state( jtag_core * jc, char * line)
{
	int i,j,k,id;
	char dev_index[DEFAULT_BUFLEN];
	char pinname[DEFAULT_BUFLEN];
	char state[DEFAULT_BUFLEN];

	i = get_param(line, 1,dev_index);
	j = get_param(line, 2,pinname);
	k = get_param(line, 3,state);

	if(i>=0 && j>=0 && k>=0)
	{
		id = jtagcore_get_pin_id(jc, atoi(dev_index), pinname);

		if(id>=0)
		{
			jtagcore_set_pin_state(jc, atoi(dev_index), id, JTAG_CORE_OUTPUT, atoi(state));


			return JTAG_CORE_NO_ERROR;
		}
		else
		{
			return JTAG_CORE_NOT_FOUND;
		}
	}


	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_get_pin_state( jtag_core * jc, char * line)
{
	int i,j,k,ret,id;
	char dev_index[DEFAULT_BUFLEN];
	char pinname[DEFAULT_BUFLEN];
	char mode[DEFAULT_BUFLEN];

	i = get_param(line, 1,dev_index);
	j = get_param(line, 2,pinname);
	k = get_param(line, 3,mode);

	if(i>=0 && j>=0 && k>=0)
	{
		id = jtagcore_get_pin_id(jc, atoi(dev_index), pinname);

		if(id>=0)
		{
			ret = jtagcore_get_pin_state(jc, atoi(dev_index), id, JTAG_CORE_INPUT);


			return JTAG_CORE_NO_ERROR;
		}
		else
		{
			return JTAG_CORE_NOT_FOUND;
		}
	}


	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_get_pins_list( jtag_core * jc, char * line)
{
	int i,j,nb_of_pins;
	char dev_index[DEFAULT_BUFLEN];
	char pinname[DEFAULT_BUFLEN];
	int type;

	i = get_param(line, 1,dev_index);
	if(i>=0)
	{
		nb_of_pins = jtagcore_get_number_of_pins(jc,atoi(dev_index));
		if(nb_of_pins>=0)
		{
			for(j = 0;j < nb_of_pins;j++)
			{
				if(jtagcore_get_pin_properties(jc, atoi(dev_index), j, pinname, sizeof(pinname), &type) == JTAG_CORE_NO_ERROR)
				{
					if(type & JTAG_CORE_PIN_IS_INPUT)
					{
					}
					else
					{
					}

					if(type & JTAG_CORE_PIN_IS_OUTPUT)
					{
					}
					else
					{
					}

					if(type & JTAG_CORE_PIN_IS_TRISTATES)
					{
					}
					else
					{
					}


				}
			}
		}

		return JTAG_CORE_NO_ERROR;
	}

	return JTAG_CORE_BAD_PARAMETER;
}


static int cmd_help( jtag_core * jc, char * line);

static int cmd_version( jtag_core * jc, char * line)
{

	return JTAG_CORE_NO_ERROR;
}

static int cmd_set_env_var( jtag_core * jc, char * line )
{
	int i,j,ret;
	char varname[DEFAULT_BUFLEN];
	char varvalue[DEFAULT_BUFLEN];

	ret = JTAG_CORE_BAD_PARAMETER;

	i = get_param(line, 1,varname);
	j = get_param(line, 2,varvalue);

	if(i>=0 && j>=0)
	{
		ret = jtagcore_setEnvVar( jc, (char*)&varname, (char*)&varvalue );
	}

	return ret;
}

static int cmd_print_env_var( jtag_core * jc, char * line )
{
	int i;
	char varname[DEFAULT_BUFLEN];
	char varvalue[DEFAULT_BUFLEN];
	char * ptr;

	i = get_param(line, 1,varname);

	if(i>=0)
	{
		ptr = jtagcore_getEnvVar( jc, (char*)&varname, (char*)&varvalue );
		if(ptr)
		{

			return JTAG_CORE_NO_ERROR;
		}

		return JTAG_CORE_NOT_FOUND;
	}
	else
	{
		return JTAG_CORE_BAD_PARAMETER;
	}
}

static int cmd_call( jtag_core * jc, char * line )
{
	int offs;
	char path[DEFAULT_BUFLEN];

	get_param(line, 1,(char*)&path);

	offs = get_param_offset(line, 1);

	if(offs>=0)
	{
		return jtagcore_execScriptFile( jc, (char*)&path );
	}

	return JTAG_CORE_BAD_PARAMETER;
}

static int cmd_system( jtag_core * jc, char * line )
{
	int offs;
	int ret;

	offs = get_param_offset(line, 1);

	if(offs>=0)
	{
		ret = system(&line[offs]);

		if( ret != 1 )
			return JTAG_CORE_NO_ERROR;
		else
			return JTAG_CORE_NOT_FOUND;
	}

	return JTAG_CORE_BAD_PARAMETER;
}

cmd_list cmdlist[] =
{
	{"print",                   cmd_print},
	{"help",                    cmd_help},
	{"?",                       cmd_help},
	{"version",                 cmd_version},
	{"pause",                   cmd_pause},
	{"set",                     cmd_set_env_var},
	{"print_env_var",           cmd_print_env_var},
	{"call",                    cmd_call},
	{"system",                  cmd_system},

	{"jtag_get_probes_list",    cmd_print_probes_list},
	{"jtag_open_probe",         cmd_open_probe},
	{"jtag_autoinit",           cmd_autoinit},

	{"jtag_init_scan",          cmd_init_and_scan},
	{"jtag_get_nb_of_devices",  cmd_print_nb_dev},
	{"jtag_get_devices_list",   cmd_print_devs_list},
	{"jtag_load_bsdl",          cmd_load_bsdl},
	{"jtag_set_mode",           cmd_set_scan_mode},

	{"jtag_push_pop",           cmd_push_and_pop},

	{"jtag_get_pins_list",      cmd_get_pins_list},

	{"jtag_set_pin_dir",        cmd_set_pin_mode},
	{"jtag_set_pin_state",      cmd_set_pin_state},
	{"jtag_get_pin_state",      cmd_get_pin_state},

	{0 , 0}
};

static int extract_cmd(char * line, char * command)
{
	int offs,i;

	i = 0;
	offs = 0;

	offs = get_next_word(line, offs);

	if( !is_end_line(line[offs]) )
	{
		while( !is_end_line(line[offs]) && !is_space(line[offs]) && i < (DEFAULT_BUFLEN - 1) )
		{
			command[i] = line[offs];
			offs++;
			i++;
		}

		command[i] = 0;

		return i;
	}

	return 0;
}

int exec_cmd( jtag_core * jc, char * command,char * line)
{
	int i;

	i = 0;
	while(cmdlist[i].func)
	{
		if( !strcmp(cmdlist[i].command,command) )
		{
			cmdlist[i].func(jc,line);
			return 1;
		}

		i++;
	}

	return JTAG_CORE_CMD_NOT_FOUND;
}

static int cmd_help( jtag_core * jc, char * line)
{
	int i;


	i = 0;
	while(cmdlist[i].func)
	{
		i++;
	}

	return JTAG_CORE_NO_ERROR;
}

int jtagcore_execScriptLine( jtag_core * jc, char * line )
{
	char command[DEFAULT_BUFLEN];

	command[0] = 0;

	if( extract_cmd(line, command) )
	{
		if(strlen(command))
		{
			if(exec_cmd(jc,command,line) == JTAG_CORE_CMD_NOT_FOUND )
			{

				return JTAG_CORE_CMD_NOT_FOUND;
			}

			return JTAG_CORE_NO_ERROR;
		}

	}
	return JTAG_CORE_BAD_CMD;
}

int jtagcore_execScriptFile( jtag_core * jc, char * script_path )
{
	int err;
	FILE * f;
	char line[DEFAULT_BUFLEN];

	err = JTAG_CORE_INTERNAL_ERROR;

	f = fopen(script_path,"r");
	if(f)
	{
		do
		{
			if(!fgets(line,sizeof(line),f))
				break;

			if(feof(f))
				break;

			jtagcore_execScriptLine(jc, line);
		}while(1);

		fclose(f);

		err = JTAG_CORE_NO_ERROR;
	}
	else
	{

		err = JTAG_CORE_ACCESS_ERROR;
	}

	return err;
}

int jtagcore_execScriptRam( jtag_core * jc, unsigned char * script_buffer, int buffersize )
{
	int err = 0;
	int buffer_offset,line_offset;
	char line[DEFAULT_BUFLEN];

	buffer_offset = 0;
	line_offset = 0;

	do
	{
		memset(line,0,DEFAULT_BUFLEN);
		line_offset = 0;
		while( (buffer_offset < buffersize) && script_buffer[buffer_offset] && script_buffer[buffer_offset]!='\n' && script_buffer[buffer_offset]!='\r' && (line_offset < DEFAULT_BUFLEN - 1))
		{
			line[line_offset++] = script_buffer[buffer_offset++];
		}

		while( (buffer_offset < buffersize) && script_buffer[buffer_offset] && (script_buffer[buffer_offset]=='\n' || script_buffer[buffer_offset]=='\r') )
		{
			buffer_offset++;
		}

		jtagcore_execScriptLine(jc, line);

		if( (buffer_offset >= buffersize) || !script_buffer[buffer_offset])
			break;

	}while(buffer_offset < buffersize);

	return err;
}
