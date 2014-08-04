#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "unicode.h"
#define CHINA_SET_MAX 8
int outbin(FILE *fp, char ch)
{
    int i = 7;
    while(i >= 0)
    {
        fprintf(fp, "%d", ((ch >> i) & 1));
        i--;
    }
}

/* convert unicode to UTF-8 */
unsigned char *UnicodetoUTF8(int unicode, unsigned char *p)
{
    unsigned char *e = NULL;

    if((e = p))
    {
        if(unicode < 0x80) *e++ = unicode;
        else if(unicode < 0x800)
        {
            /*<11011111> < 000 0000 0000>*/
            *e++ = ((unicode >> 6) & 0x1f)|0xc0;
            *e++ = (unicode & 0x3f)|0x80; 
        }
        else if(unicode < 0x10000)
        {
            /*<11101111> <0000 0000 0000 0000>*/
            *e++ = ((unicode >> 12) & 0x0f)|0xe0; 
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80; 
        }
        else if(unicode < 0x200000)
        {
            /*<11110111> <0 0000 0000 0000 0000 0000>*/
            *e++ = ((unicode >> 18) & 0x07)|0xf0; 
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80; 
        }
        else if(unicode < 0x4000000)
        {
            /*<11111011> <00 0000 0000 0000 0000 0000 0000>*/
            *e++ = ((unicode >> 24) & 0x03)|0xf8 ; 
            *e++ = ((unicode >> 18) & 0x3f)|0x80;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80; 
        }
        else
        {
            /*<11111101> <0000 0000 0000 0000 0000 0000 0000 0000>*/
            *e++ = ((unicode >> 30) & 0x01)|0xfc; 
            *e++ = ((unicode >> 24) & 0x3f)|0x80;
            *e++ = ((unicode >> 18) & 0x3f)|0x80;
            *e++ = ((unicode >> 12) & 0x3f)|0x80;
            *e++ = ((unicode >> 6) & 0x3f)|0x80;
            *e++ = (unicode & 0x3f)|0x80; 
        }
    }
    return e;
}

/* convert UTF-8 to unicode */
int UTF8toUnicode(unsigned char *ch, int *unicode)
{
    unsigned char *p = NULL;
    int e = 0, n = 0;

    if((p = ch) && unicode)
    {
        if(*p >= 0xfc)
        {
            /*6:<11111100>*/
            e = (p[0] & 0x01) << 30;
            e |= (p[1] & 0x3f) << 24;
            e |= (p[2] & 0x3f) << 18;
            e |= (p[3] & 0x3f) << 12;
            e |= (p[4] & 0x3f) << 6;
            e |= (p[5] & 0x3f);
            n = 6;
        }
        else if(*p >= 0xf8) 
        {
            /*5:<11111000>*/
            e = (p[0] & 0x03) << 24;
            e |= (p[1] & 0x3f) << 18;
            e |= (p[2] & 0x3f) << 12;
            e |= (p[3] & 0x3f) << 6;
            e |= (p[4] & 0x3f);
            n = 5;
        }
        else if(*p >= 0xf0)
        {
            /*4:<11110000>*/
            e = (p[0] & 0x07) << 18;
            e |= (p[1] & 0x3f) << 12;
            e |= (p[2] & 0x3f) << 6;
            e |= (p[3] & 0x3f);
            n = 4;
        }
        else if(*p >= 0xe0)
        {
            /*3:<11100000>*/
            e = (p[0] & 0x0f) << 12;
            e |= (p[1] & 0x3f) << 6;
            e |= (p[2] & 0x3f);
            n = 3;
        }
        else if(*p >= 0xc0) 
        {
            /*2:<11000000>*/
            e = (p[0] & 0x1f) << 6;
            e |= (p[1] & 0x3f);
            n = 2;
        }
        else 
        {
            e = p[0];
            n = 1;
        }
        *unicode = e;
    }
    return n;
}
#ifdef _DEBUG_UNICODE
int main()
{
    unsigned char ch[CHINA_SET_MAX], *p = NULL, *e = NULL, *s = NULL;
    int i = 0, n = 0, unicode = 0;

#ifdef _OUT_UNICODE_
       for(i = 1; i < 65536; i++)
       {
           p = (unsigned char *)&i;
           outbin(stdout, p[0]);
           fprintf(stdout, " ");
           outbin(stdout, p[1]);
           fprintf(stdout, " ");
           outbin(stdout, p[2]);
           fprintf(stdout, " ");
           outbin(stdout, p[3]);
           e = ch;
           s = unicodetoUTF8(i, e);
           *s = '\0';
           while(*e != 0)
           {
               fprintf(stdout, "[");
               outbin(stdout, *e++);
               fprintf(stdout, "]");
           }
        }
#endif
    //macro functions 
    sprintf((char *)ch, "%s", "Äã");
    e = ch;
    unicode = 0;
    UTF8_TO_UNICODE(e, unicode, n);
    fprintf(stdout, "convert UTF8_TO_UNICODE(%s, %08x, %d)\n", ch, unicode, n);
    memset((char *)ch, 0, CHINA_SET_MAX);
    e = ch;
    UNICODE_TO_UTF8(unicode, e);
    e = '\0';
    fprintf(stdout, "convert UNICODE_TO_UTF8(%08x, %s)\n", unicode, ch);
    //C functions
    unicode = 0;
    if((n = UTF8toUnicode(ch, &unicode)) > 0)
    {
        fprintf(stdout, "convert %d = UTF8toUnicode(%s, %08x)\n", n, ch, unicode);
        memset((char *)ch, 0, CHINA_SET_MAX);
        if((e = UnicodetoUTF8(unicode, ch)) > ch)
        {
            e = '\0';
            fprintf(stdout, "convert UnicodetoUTF8(%08x, %s)\n", unicode, ch);
        }
    }
    return 0;
}
#endif