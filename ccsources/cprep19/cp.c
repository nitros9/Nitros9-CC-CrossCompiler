#define MAIN 1
#include "cp.h"
#include <errno.h>
#include <stdarg.h>

#define ishex(x) ((x)>='0' && (x)<='9') || ((x)>='a' && (x)<='f') || ((x)>='A' && (x)<='F')
#define isoctal(x) (x)>='0' && (x)<='7'

time_t mytime;

int
main (int argc, char *argv[])
{
    int a, b, i, namptr, triflag;
    char edition[5];

    mytime = time ((time_t *) 0);
    timdat = ctime (&mytime);    /* produce time/date string */
    strncpy (_date_, &timdat[4], 7);
    strncpy (&_date_[7], &timdat[20], 4);
    _date_[11] = 0;             /* copy date string into _date_ */
    strncpy (_time_, &timdat[11], 8);
    _time_[8] = 0;              /* copy time string into _time_ */
    initstdefs ();              /* Initialize standard definitions */
    ifstini();                  /* Initialize ifstat[0] */
    edition[0] = '0';
    edition[1] = 0;
    dptr = dstrtbl;             /* Init $trng tbl ptr */
    namptr = 0;                 /* Init filename ptr to none */
    triflag = FALSE;            /* DEFAULT: no trigraph expansion */

    for (i = 1; i < argc; ++i)  /* Loop for each arg */
    {
        if (*argv[i] == '-')    /* If arg is a switch... */
        {
            switch (*(argv[i] + 1))
            {
            case 't':          /* Trigraph switch */
            case 'T':
                triflag = TRUE; /* Turn on trigraph expansion */
                break;
            case 'd':          /* Define switch */
            case 'D':          /* Format: -d<name>[=<token sequence>] */
                strcpy (line, argv[i] + 2);     /* put def in line[] */
                if (a = findchar (line, '='))
                    line[a - 1] = ' ';       /* change = to space */
                else
                    strcat (line, " 1");     /* if no tok-seq then set to 1 */
                dodefine (strlen (line), 0);    /* Install def in tbl */
                break;
            case 'c':          /* Compiler select */
            case 'C':
                cflag = TRUE;   /* select Microware C compatible */
                defnam[4] = "__STDC__";
                break;
            case 'e':          /* Edition # */
            case 'E':          /* Format: -e[=<decimal number>] */
                if (*(argv[i] + 2) == '=')
                {
                    strncpy (edition, argv[i] + 3, 4);
                    edition[4] = 0;
                }
                else
                {
                    edition[0] = '0';
                    edition[1] = 0;
                }
                break;
            case 'l':          /* Copy source to ASM output (Microware only) */
            case 'L':
                lflag = TRUE;   /* select Microware C compatible */
                break;
            default:           /* Illegal switch */
                usage ();
                break;
            }
        }
        else if ( ! namptr)
            namptr = i;
        else
            usage ();           /* Already have filename and not a switch */
    }

    if ( ! namptr)
    {
        fpath[fptr] = stdin;    /* if no filename given use stdin */
        strcpy (ifnbuf[fptr], "stdin.c");       /* set default name */
    }
    else if ((fpath[fptr] = fopen (argv[namptr], "r")))
        strcpy (ifnbuf[fptr], argv[namptr]);    /* store root file name */
    else
        exit (_errmsg (errno, "Cannot open source file: %s\n", argv[namptr]));

    cflag = ! cflag;             /* INVERT CFLAG TO DEFAULT TO MICROWARE MODE */

    lnprint (3, edition);       /* print root file #line info */

    for (;;)                    /* Loop to process each file in IFNBUF[] */
    {
        eflag = FALSE;          /* set exit flag to no exit */

        for (;;)                /* Loop to process each line in current file */
        {
            if (eflag)          /* break if EOF */
            {
                break;
            }

            if ( ! (a = getln (0)))       /* Fill LINE[] up to CR or EOF */
            {
                lnflag[1] = -1; /* cancel #line after current line */
                lnflag[0] = _line_;     /* print #line before next line */
                continue;       /* empty line */
            }

            if (lflag)          /* generate Microware codes to copy source lines */
            {
                if (cflag)
                    puts ("#6");        /* for ASM output */
                else
                    printf ("#pragma src ");

                printf ("%s\n", line);
            }
            do
            {
                if (triflag)    /* include trigraph expansion */
                    b = a = trigraph (a);
                else
                    b = a;      /* exclude trigraph expansion */
                
                a = xtndln (a);
            }
            while (b != a);
            
            a = wspace (a);

            if (*line == '#' || line[1] == '#')
                a = prep ();    /* Parse preprocessor commands */
            
            if (a && procsw && tstline (a))
            {
                splittok (line, 0);     /* Split non # line into tokens */
                expand (line, NULL, NULL);
                cncatstr (line);        /* Concatenate adjacent strings */
                convert (VTAB, SPACE);
                lnprint (0, &lnflag[0]);
                printf ("%s\n", line + 1);
                lnprint (0, &lnflag[1]);
            }
            else
            {
                lnflag[1] = -1; /* cancel #line after current line */
                lnflag[0] = _line_;     /* print #line before next line */
            }
        }
        if (!fptr)              /* if root file end cprep */
            break;
        fclose (fpath[fptr]); fptr--;/* close include file */
        lnprint (2, NULL);      /* print #line code to reallign to old file */
    }
}

/* Get next line from input path up to CR or EOF.  If EOF then eflag
   is TRUE.  'a' is the position in line[] to start reading in new input.
   Upon exit 'a' = position of null terminator in line[].  All lines end with
   a NULL, even if EOF occurs line is  NULL terminated.  CRs are stripped.
   The only exception is when an EOF occurs when a=0 then since no chars are
   waiting to be processed getln() exits.  This is only rtn that sets eflag.
   If line exceeds line buffer length then fatal error occurs. */

    /* gets next line from input path, strips CR, and NULL */
    /* terminates */
int
getln (int a)
{
    register char *lnptr, *strgot, *crpos;

    lnptr = &line[a];
/*    if ((a=lnread(fpath[fptr],lnptr,LINEMAX-a))==ERROR) */

    if (strgot = fgets (lnptr, LINEMAX-a, fpath[fptr]))
    {
	    /* My version of MinGW doesn't seem to have index()
	     * Some other test may be needed rather than _WIN32
	     */
#ifndef _WIN32
        if (crpos = index (strgot, '\n'))
#else
        if (crpos = strchr (strgot, '\n'))
#endif
        {
            *crpos = '\0';
        }
        
        ++_line_;
/*  fprintf(stderr,"a=%d line=|%s|\n",a,line);  */
        return strlen (strgot);
        /*return a; *//* returns number of bytes read */
    }
    else
    {
        /*fprintf(stderr,"GETLN:fpath=%d errno=%d\n",fpath[fptr],errno);*/
        eflag = TRUE;           /* exit at next opportunity */
        return 0;            /* EOF on blank line */
    }
}

int
trigraph (int a)
{
    int c, d;
    register int b;
    c = 1;

    while (b = findstr (c, line, "??"))
    {
        if (d = gettri (b))     /* Get replacement char or zero if invalid */
        {
            line[b - 1] = d;    /* Insert substitute char */
            strcpy (&line[b], &line[b + 2]);   /* Append end of line */
            a -= 2;             /* Adjust end of line ptr */
        }
        c = b + 1;              /* set new search pos */
    }

    return a;
}

int
gettri (                        /* Given b=pos of first ? (1-...) in trigraph */
           int b                /* Returns substitution char or 0 if invalid */
    )
{
    register int i;

    i = 0;
    switch (line[b + 1])
    {
    case '=':
        i = '#';
        break;
    case '/':
        i = '\\';
        break;
    case '\'':
        i = '^';
        break;
    case '(':
        i = '[';
        break;
    case ')':
        i = ']';
        break;
    case '!':
        i = '|';
        break;
    case '<':
        i = '{';
        break;
    case '>':
        i = '}';
        break;
    case '-':
        i = '~';
        break;
    }
    return i;
}

int
xtndln (                        /* concatenate lines ending in \ */
           register int a)
{
    if (line[a - 1] == '\\')
    {
        if (eflag)              /* if EOF */
            line[--a] = 0;      /* Drop \ */
        else
        {
            a = getln (--a);    /* Drop \ and append next line */
            lnflag[1] = _line_; /* set to print #line after line */
        }
    }

    return a;
}

int
wspace (                        /* Shrink whitespace chars to single space */
           int a)
{
    register char *b;
    char *c;

    b = line - 1;
    c = &line[a];

    while (++b != c)
    {
        if (*b == LINEFEED || *b == FORMFEED || *b == VTAB || *b == HTAB)
            *b = ' ';
    }

    a = cmnt (a);               /* strip comments */
    return a;
}

        /* convert all occurrences of x to y */
        /* Routine will fail to operate properly if x=y */
int
convert (int x, int y)
{
    register char *b;

    b = line - 1;
    
    while (*++b)
    {
        if (*b == x)
            *b = y;
    }
}

int
cmnt (                          /* strip comments from line */
         int a)
{
    int b;

    b = 0;                      /* point to start of line */
    
    while (line[b])             /* read until NULL terminator */
    {
        if (line[b] == '/')
            a = comm (a, &b);   /* Process comments */
        else if (line[b] == '"')
            b = qa (line, '"', b);      /* Process quotes */
        else if (line[b] == '\'')
            b = qa (line, '\'', b);     /* Process apostrophes */
        else if (line[b] == ' ')
            a = space (a, b);   /* Reduce multiple spaces to one */
        ++b;
    }
    return a;
}

int
comm (int a, int *b)
{
    int c, d;

    if (line[*b + 1] == '*')    /* found start of comment */
    {
        line[*b] = ' ';         /* Found comment line(s) replace with space */
        
            /* Cmnt ends on same line */
        if (c = findstr (*b + 3, line, "*/"))
        {
            strcpy (&line[*b + 1], &line[c + 1]);
            a -= (c - *b);
            return a;
        }

        if (eflag)              /* File ends with no end comment operator */
        {
            line[a = *b--] = 0;
            doerr (10, *b);      /* No end comment terminator */
            return a;
        }
        do                      /* comment ends on another line */
        {
            if (eflag)          /* EOF occurred */
                break;
            a = getln (*b + 1); /* get nxt line */
        }

        while (!(c = findstr (1, line, "*/")));
        
        lnflag[1] = -1;         /* cancel #line after current line */
        lnflag[0] = _line_;     /* print #line before next line */
        
        if (c)
        {
            strcpy (&line[*b + 1], &line[c + 1]);
        }
        else
            line[*b] = 0;
        do
        {
            d = a = trigraph (strlen (line));
            a = xtndln (a);
        }
        
        while (d != a);
        *b -= 2;                /* Backup to catch double space situations */
    }
    return a;
}

int
qa (                            /* Searches ln for end " or ' depending on x */
       char *ln, int x,         /* b = first " or ' */
       int b)
{
    register char *lnptr;
    lnptr = &ln[b];
    
    while (*(++lnptr) != '\\' && *lnptr != x && *lnptr);
    
    b = lnptr - ln;
    
    if (*lnptr == '\\')
    {
        lnptr = escseq (ln, lnptr);
        b = qa (ln, x, lnptr - ln);     /* skip to char after \ */
    }
    else if (*lnptr != x)
    {
        doerr (19, b);          /* No end " or ' */
        return b - 1;
    }
    
    return b;
}

int
qa2 (                           /* Searches ln for end " or ' depending on x */
        char *ln, int x,        /* b = first " or '  Swaps VTAB for spaces */
        int b)
{
    register char *lnptr;
    
    lnptr = &ln[b];
    
    while (*(++lnptr) != '\\' && *lnptr != x && *lnptr)
    {
        if (*lnptr == ' ')
        {
            *lnptr = VTAB;
        }
    }

    b = lnptr - ln;
    
    if (*lnptr == '\\')
    {
        lnptr = escseq (ln, lnptr);     /* bypass any escape sequences */
        b = qa (ln, x, lnptr - ln);     /* skip to char after \ */
    }
    else if (*lnptr != x)
    {
        doerr (19, b);
        return b - 1;
    }

    return b;
}

int
space (                         /* shrink spaces to one */
          int a, int b)
{
    register char *cptr;
    char *bptr;
    
    bptr = cptr = &line[b];
    
    while (*(++cptr) == ' ');

    if (bptr != cptr - 1)
    {
        strcpy (bptr + 1, cptr);
        a -= (cptr - bptr - 1);
    }

    return a;
}

int
tstline (int a)
{
    return (skpbl (line, 0) != a);
}

int
usage (void)
{
    fputs ("Usage:   c_prep [-opt] <source file>\n\n", stderr);
    fputs ("Options: -t Turn on trigraph expansion\n", stderr);
    fputs ("         -d<name>[=<token-sequence>] defines name\n", stderr);
    fputs ("         -c Switch to full ANSI mode\n", stderr);
    fputs ("         -h Prints this help message\n", stderr);
    fputs ("         -e[=<decimal>] edition #\n", stderr);
    fputs ("         -l Copies source lines for ASM output\n", stderr);
    fputs ("\nc_prep operates in Microware compatible mode unless", stderr);
    fputs ("the -c switch is invoked.", stderr);
    exit (0);
}

    /* Given B: pos. of first char. in identifier */
    /* Returns B: ERROR = invalid identifier or ## = pos of end */
    /*  character in identifier */

int
getident (char *ln, register int b)
{
    if (!(IDNT_INIT (ln[b])))
        return ERROR;

    do
    {
        ++b;
    }
    
    while (IDNT_TYPE (ln[b]));
    
    return --b;
}

/* My version of MinGW doesn't seem have isblank.
 * Some versions may have it.  If so, this can be removed,
 * or some other test than _WIN32 may be needed
 */

#ifdef _WIN32
int
isblank (int pos)
{
	return ((pos == ' ') || (pos == '\t'));
}
#endif

/*
skpbl(ln,b)       Given B: start pos. of blank search
char *ln;
int b;     Returns B: pos. of first non-blank char. after spaces
*/

int
skpbl (char *ln, int b)
{
    char *pos = &ln[b];

    while (isblank (*(pos++)));

    return (pos - ln - 1);
}

/*#asm
skpbl:
 ldd 2,s   (ln)
 addd 4,s  (b)
 tfr d,x   (ln[b])
SKPL0 ldb ,x+
 cmpb #32
 lbeq SKPL0
 tfr x,d
 subd 2,s
 subd #1
 puls pc
#endasm*/

int
rskpbl (                        /* Given A: end pos. to start search */
           char *ln, register int a     /* Returns A: pos. of first non-blank char. after spaces */
    )
{
    while (ln[a] == ' ')
        --a;
    return a;
}

char *
escseq (                        /* returns char * to last char in escape sequence */
           char *l, char *ln    /* ln must point to first \ in escape sequence */
    )
{
    switch (*++ln)
    {
    case '\"':
    case '\'':
    case '\?':
    case '\\':
    case 'a':
    case 'b':
    case 'f':
    case 'l':                  /* Microware addition for COCO compatibility */
    case 'n':
    case 'r':
    case 't':
    case 'v':
        return ln;
        break;
    case 'x':                  /* Hexadecimal escape sequence */
        do
        {
            ++ln;
        }
        while (ishex (*ln));
        return --ln;
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        ++ln;
        if (isoctal (*ln))
        {
            ++ln;
            if (isoctal (*ln))
                return ln;
        }
        return --ln;
        break;
    default:                   /* ERROR */
        doerr (20, ln - l);     /* Illegal escape sequence */
        return ln;
        break;
    }
}

_errmsg (int ernum, char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);
    vfprintf (stderr, fmt, ap);
    va_end (ap);

    return ernum;
}

/* Linux does not allow overlapping strcpy so provide a safe alternative */

char *safestrcpy(char *d, char *s)
{
    char *tmp = d;
    while(*d++ = *s++);
    return d;
}