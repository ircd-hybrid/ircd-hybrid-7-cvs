/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  csvlib.c - set of functions to deal with csv type of conf files
 *
 *  Copyright (C) 2003 by Diane Bruce, Stuart Walsh
 *  If you use it anywhere you like, if you like it buy us a beer.
 *  If its broken, don't bother us with the lawyers.
 *
 *  $Id: csvlib.c,v 7.2 2003/05/14 18:26:54 db Exp $
 */

#include "stdinc.h"
#include "tools.h"
#include "s_log.h"
#include "s_conf.h"
#include "hostmask.h"
#include "client.h"
#include "irc_string.h"
#include "memory.h"
#include "send.h"

static void parse_csv_line(char *line, ...);
static int write_csv_line(FBFILE *out, const char *format, ...);
static int flush_write(struct Client *source_p, FBFILE *in, FBFILE* out, 
		       char *buf, char *temppath);

/*
 * parse_csv_file
 *
 * inputs	- FILE pointer
 * 		- type of conf to parse
 * output	- none
 * side effects	-
 */

void
parse_csv_file(FBFILE *file, int conf_type)
{
  struct ConfItem *aconf;
  char* user_field=NULL;
  char* reason_field=NULL;
  char* host_field=NULL;
  char  line[BUFSIZE];
  char* p;

  while (fbgets(line, sizeof(line), file) != NULL)
  {
    if ((p = strchr(line, '\n')) != NULL)
      *p = '\0';

    if ((*line == '\0') || (*line == '#'))
      continue;

    switch(conf_type)
    {
    case CONF_KILL:
      parse_csv_line(line, &user_field, &host_field, &reason_field, NULL);
      aconf = make_conf(conf_type);
      conf_add_fields(aconf, host_field, reason_field, user_field, NULL, NULL);
      if (aconf->host != NULL)
	add_conf_by_address(aconf->host, CONF_KILL, aconf->user, aconf);
      break;

    case CONF_DLINE:
      parse_csv_line(line, &host_field, &reason_field, NULL);
      aconf = make_conf(CONF_DLINE);
      conf_add_fields(aconf, host_field, reason_field, NULL, NULL, NULL);
      conf_add_d_conf(aconf);
      break;
    }
  }
}

/*
 * parse_csv_line()
 *
 * inputs	- pointer to line to parse
 * output	-
 * side effects -
 */

static void
parse_csv_line(char *line, ...)
{
  va_list args;
  char **dest;
  char *field;

  va_start(args, line);
  dest = va_arg(args, char **);
  if ((dest == NULL) || ((field = getfield(line)) == NULL))
  {
    va_end(args);
    return;
  }

  *dest = field;
    
  for(;;)
  {
    dest = va_arg(args, char **);
    if ((dest == NULL) || ((field = getfield(NULL)) == NULL))
    {
      va_end(args);
      return;
    }
    *dest = field;
  }
  /* NOT REACHED */
  va_end(args);
}


/* write_conf_line()
 *
 * inputs       - type of conf i.e. kline, dline etc. as needed
 *              - client pointer to report to
 *              - user name of target
 *              - host name of target
 *              - reason for target
 *              - time_t cur_time
 * output       - NONE
 * side effects - This function takes care of
 *                finding right conf file, writing
 *                the right lines to this file, 
 *                notifying the oper that their kline/dline etc. is in place
 *                notifying the opers on the server about the k/d etc. line
 *                
 */
void 
write_conf_line(int type, struct Client *source_p, char *user, char *host,
		const char *reason, const char *oper_reason,
		const char *current_date, time_t cur_time)
{
  FBFILE *out;
  const char *filename;

  filename = get_conf_name(type);
  if ((out = fbopen(filename, "a")) == NULL)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL,
                         "*** Problem opening %s ", filename);
    return;
  }

  switch(type)
  {
  case CONF_KILL:
    sendto_realops_flags(UMODE_ALL, L_ALL,
                         "%s added K-Line for [%s@%s] [%s]",
                         get_oper_name(source_p), user, host, reason);
    sendto_one(source_p, ":%s NOTICE %s :Added K-Line [%s@%s]",
               me.name, source_p->name, user, host);
    ilog(L_TRACE, "%s added K-Line for [%s@%s] [%s]",
         source_p->name, user, host, reason);
    write_csv_line(out, "%s%s%s%s%s%s%ld",
		   user, host, reason, oper_reason, current_date,
		   get_oper_name(source_p), (long)cur_time);
    break;

  case CONF_DLINE:
    sendto_realops_flags(UMODE_ALL, L_ALL,
                         "%s added D-Line for [%s] [%s]",
                         get_oper_name(source_p), host, reason);
    sendto_one(source_p, ":%s NOTICE %s :Added D-Line [%s] to %s",
               me.name, source_p->name, host, filename);
    ilog(L_TRACE, "%s added D-Line for [%s] [%s]",
         get_oper_name(source_p), host, reason);
    write_csv_line(out, "%s%s%s%s%s%ld",
		   host, reason, oper_reason, current_date,
		   get_oper_name(source_p), (long)cur_time);
    break;

  default:
    fbclose(out);
    return;
  }

  /* XXX could add this to write_csv_line */
#if 0
    sendto_realops_flags(UMODE_ALL, L_ALL,
                         "*** Problem writing to %s",filename);
#endif

  fbclose(out);
}

/*
 * write_csv_line()
 *
 * inputs	- pointer to FBFILE *
 *		- formatted string
 * output	-
 * side effects - single line is written to csv conf file
 */

static int
write_csv_line(FBFILE *out, const char *format, ...)
{
  char c;
  int bytes=0;
  va_list args;
  char tmp[1024];
  char *str = tmp;
  char *null_string = "";

  if (out == NULL)
    return(0);

  va_start(args, format);

  while ((c = *format++))
  {
    if (c == '%')
    {
      c = *format++;
      if (c == 's')
      {
	const char *p1 = va_arg(args, const char *);
	if (p1 == NULL)
	  p1 = null_string;
	*str++ = '\"';
	++bytes;
	++bytes;
	while (*p1 != '\0')
	{
	  *str++ = *p1++;
	  ++bytes;
        }
	*str++ = '\"';
	*str++ = ',';
	*str = '\0';
	  
	bytes += 3;
	continue;
      }
      if (c == 'c')
      {
	*str++ = '\"';
	++bytes;
	*str++ = (char) va_arg(args, int);
	++bytes;
	*str++ = '\"';
	*str++ = ',';
	++bytes;
	++bytes;
	continue;
      }
      if (c != '%')
      {
	int ret;
	
	format -= 2;
	ret = vsprintf(str, format, args);
	str += ret;
	bytes += ret;
	
	break;
      }
    }
    *str++ = c;
    ++bytes;
  }
  if(*(str-1) == ',')
  {
    *(str-1) = '\n';
    *str = '\0';
  }
  else
  {
    *str++ = '\n';
    *str = '\0';
  }

  va_end(args);
  str = tmp;
  fbputs(str, out);
  return bytes;
}

/*
 * getfield
 *
 * inputs	- input buffer
 * output	- next field
 * side effects	- field breakup for ircd.conf file.
 */
char *
getfield(char *newline)
{
  static char *line = NULL;
  char  *end, *field;
        
  if (newline != NULL)
    line = newline;

  if (line == NULL)
    return(NULL);

  field = line;

  /* XXX make this skip to first " if present */
  if(*field == '"')
  {
    field++;
    end = field;
  }
  else
    return(NULL);		/* mal-formed field */

  for (;;)
  {
    /* At end of string, mark it as end and return */
    if (*end == '\0')
    {
      line = NULL;
      return(NULL);
    }
    else if (*end == '\\')      /* found escape character ? */
    {
      end++;
    }
    else if(*end == '"')	/* found terminating " */
    {
      *end++ = '\0';
      while (IsSpace(*end))	/* skip to start of next " (or '\0') */
	end++;
      while (*end == ',')
	end++;
      while (IsSpace(*end))
	end++;
      line = end;
      return(field);
    }
    end++;
  }
  return (NULL);
}

/*
 * remove_conf_line
 *
 * inputs	- type of kline to remove
 *		- pointer to oper removing
 *		- pat1 pat2 patterns to match
 * output	- -1 if unsuccessful 0 if no change 1 if change
 * side effects	-
 */
int
remove_conf_line(int type, struct Client *source_p, char *pat1, char *pat2)
{
  const char *filename;
  FBFILE *in, *out;
  int pairme=0;
  char buf[BUFSIZE], buff[BUFSIZE], temppath[BUFSIZE], *p;
  char *found1;
  char *found2;
  mode_t oldumask;

  filename = get_conf_name(type);

  if ((in = fbopen(filename, "r")) == NULL)
  {
    sendto_one(source_p, ":%s NOTICE %s :Cannot open %s", me.name,
	       source_p->name, filename);
    return(-1);
  }

  ircsprintf(temppath, "%s.tmp", filename);
  oldumask = umask(0);
  if ((out = fbopen(temppath, "w")) == NULL)
  {
    sendto_one(source_p, ":%s NOTICE %s :Cannot open %s", me.name,
	       source_p->name, temppath);
    fbclose(in);
    umask(oldumask);
    return(-1);
  }
  umask(oldumask);
  oldumask = umask(0);

  while (fbgets(buf, sizeof(buf), in) != NULL) 
  {
    if ((p = strchr(buff,'\n')) != NULL)
      *p = '\0';

    if ((*buff == '\0') || (*buff == '#'))
    {
      if(flush_write(source_p, in, out, buf, temppath) < 0)
	return(-1);
    }
    
    /* Keep copy of original line, getfield trashes line as it goes */
    strlcpy(buff, buf, sizeof(buff));
    
    if ((found1 = getfield(buff)) == NULL)
    {
      if(flush_write(source_p, in, out, buf, temppath) < 0)
	return(-1);
      continue;
    }

    if (pat2 != NULL)
    {
      if ((found2 = getfield(NULL)) == NULL)
      {
	if(flush_write(source_p, in, out, buf, temppath) < 0)
	  return(-1);
	continue;
      }
      
      if ((irccmp(pat1, found1) == 0) && (irccmp(pat2, found2) == 0))
      {
	pairme = 1;
	continue;
      }
      else
      {
	if(flush_write(source_p, in, out, buf, temppath) < 0)
	  return(-1);
	continue;
      }
    }
    else
    {
      if (irccmp(pat1, found1) == 0)
      {
	pairme = 1;
	continue;
      }
      else
      {
	if(flush_write(source_p, in, out, buf, temppath) < 0)
	  return(-1);
	continue;
      }
    }
  }

  fbclose(in);
  fbclose(out);

/* The result of the rename should be checked too... oh well */
/* If there was an error on a write above, then its been reported
 * and I am not going to trash the original kline /conf file
 */

  if (pairme == 0)
  {
    if(temppath != NULL)
      (void)unlink(temppath);
    return(0);
  }
  else
  {
    (void)rename(temppath, filename);
    rehash(0);
    return(1);
  }
}

/*
 * flush_write()
 *
 * inputs       - pointer to client structure of oper requesting unkline
 *		- in is the input file descriptor
 *              - out is the output file descriptor
 *              - buf is the buffer to write
 *              - ntowrite is the expected number of character to be written
 *              - temppath is the temporary file name to be written
 * output       - -1 for error on write
 *              - 0 for ok
 * side effects - if successful, the buf is written to output file
 *                if a write failure happesn, and the file pointed to
 *                by temppath, if its non NULL, is removed.
 *
 * The idea here is, to be as robust as possible when writing to the 
 * kline file.
 *
 * -Dianora
 */

static int
flush_write(struct Client *source_p, FBFILE *in, FBFILE* out, 
	    char *buf, char *temppath)
{
  int error_on_write = (fbputs(buf, out) < 0) ? (-1) : (0);

  if (error_on_write)
  {
    sendto_one(source_p,":%s NOTICE %s :Unable to write to %s aborting",
	       me.name, source_p->name, temppath );
    if(temppath != NULL)
      (void)unlink(temppath);
    fbclose(in);
    fbclose(out);
  }
  return(error_on_write);
}