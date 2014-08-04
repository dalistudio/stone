#ifndef _UNICODE_H
#define _UNICODE_H
/* gcc -o unicode unicode.c -D_DEBUG_UNICODE */
/* convert unicode to UTF-8 */
unsigned char *UnicodetoUTF8(int unicode, unsigned char *p);
#define UNICODE_TO_UTF8(unicode, e)                                             \
do                                                                              \
{                                                                               \
    if(e && unicode != 0)                                                       \
    {                                                                           \
        if(unicode < 0x80) *e++ = unicode;                                      \
        else if(unicode < 0x800)                                                \
        {                                                                       \
            /*<11011111> < 000 0000 0000>*/                                     \
            *e++ = ((unicode >> 6) & 0x1f)|0xc0;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else if(unicode < 0x10000)                                              \
        {                                                                       \
            /*<11101111> <0000 0000 0000 0000>*/                                \
            *e++ = ((unicode >> 12) & 0x0f)|0xe0;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else if(unicode < 0x200000)                                             \
        {                                                                       \
            /*<11110111> <0 0000 0000 0000 0000 0000>*/                         \
            *e++ = ((unicode >> 18) & 0x07)|0xf0;                               \
            *e++ = ((unicode >> 12) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else if(unicode < 0x4000000)                                            \
        {                                                                       \
            /*<11111011> <00 0000 0000 0000 0000 0000 0000>*/                   \
            *e++ = ((unicode >> 24) & 0x03)|0xf8 ;                              \
            *e++ = ((unicode >> 18) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 12) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            /*<11111101> <0000 0000 0000 0000 0000 0000 0000 0000>*/            \
            *e++ = ((unicode >> 30) & 0x01)|0xfc;                               \
            *e++ = ((unicode >> 24) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 18) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 12) & 0x3f)|0x80;                               \
            *e++ = ((unicode >> 6) & 0x3f)|0x80;                                \
            *e++ = (unicode & 0x3f)|0x80;                                       \
        }                                                                       \
    }                                                                           \
}while(0)

/* convert UTF-8 to unicode */
int UTF8toUnicode(unsigned char *ch, int *unicode);
#define UTF8_TO_UNICODE(p, e, n)                                                \
do                                                                              \
{                                                                               \
    if(p)                                                                       \
    {                                                                           \
        if(*p >= 0xfc)                                                          \
        {                                                                       \
            /*6:<11111100>*/                                                    \
            e = (p[0] & 0x01) << 30;                                            \
            e |= (p[1] & 0x3f) << 24;                                           \
            e |= (p[2] & 0x3f) << 18;                                           \
            e |= (p[3] & 0x3f) << 12;                                           \
            e |= (p[4] & 0x3f) << 6;                                            \
            e |= (p[5] & 0x3f);                                                 \
            n = 6;                                                              \
        }                                                                       \
        else if(*p >= 0xf8)                                                     \
        {                                                                       \
            /*5:<11111000>*/                                                    \
            e = (p[0] & 0x03) << 24;                                            \
            e |= (p[1] & 0x3f) << 18;                                           \
            e |= (p[2] & 0x3f) << 12;                                           \
            e |= (p[3] & 0x3f) << 6;                                            \
            e |= (p[4] & 0x3f);                                                 \
            n = 5;                                                              \
        }                                                                       \
        else if(*p >= 0xf0)                                                     \
        {                                                                       \
            /*4:<11110000>*/                                                    \
            e = (p[0] & 0x07) << 18;                                            \
            e |= (p[1] & 0x3f) << 12;                                           \
            e |= (p[2] & 0x3f) << 6;                                            \
            e |= (p[3] & 0x3f);                                                 \
            n = 4;                                                              \
        }                                                                       \
        else if(*p >= 0xe0)                                                     \
        {                                                                       \
            /*3:<11100000>*/                                                    \
            e = (p[0] & 0x0f) << 12;                                            \
            e |= (p[1] & 0x3f) << 6;                                            \
            e |= (p[2] & 0x3f);                                                 \
            n = 3;                                                              \
        }                                                                       \
        else if(*p >= 0xc0)                                                     \
        {                                                                       \
            /*2:<11000000>*/                                                    \
            e = (p[0] & 0x1f) << 6;                                             \
            e |= (p[1] & 0x3f);                                                 \
            n = 2;                                                              \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            e = p[0];                                                           \
            n = 1;                                                              \
        }                                                                       \
    }                                                                           \
}while(0)
#endif