#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

uint32_t str2uint(const char *s, uint8_t base)
{
    uint32_t sum = 0, num;
    while(*s != ' ' && *s != '\0')
    {
        if(*s >= 'a' && *s <= 'f')
            num = *s - 'a' + 10;
        else
            num = *s - '0';
        sum = sum*base + num;
        s++;
    }
    return sum;
}

const char *skip_space(const char *p)
{
    while(isspace(*p))
        p++;
    return p;
}

static uint8_t tsscanf(const char *str, const char *format, va_list ap)
{
    uint8_t cnt = 0;
	const char *s = str, *f = format;

	for (; *f; ++f)
	{
        s = skip_space(s);
        f = skip_space(f);

		if (*f == '%')
		{
			switch (*++f)
			{
				case 'd':
                {
                    *va_arg(ap, uint32_t *) = str2uint(s, 10);
                    cnt++;
                    break;
                }
                case 'x':
				{
                    *va_arg(ap, uint32_t *) = str2uint(s, 16);
                    cnt++;
                    break;
				}

				case 's':
				{
					char *str_arg = va_arg(ap, char *);
                    while (!isspace(*s))
                        *str_arg++ = *s++;
                    *str_arg = '\0';
                    cnt++;
					break;
				}

				default:
					return cnt;
			}
		}
		else
		{
			if (*s++ != *f)
				return cnt;
		}
	}

	return cnt;
}

uint8_t simple_sscanf(const char *str, const char *format, ...)
{
    uint8_t rc;
    va_list ap;
    va_start(ap, format);
    rc = tsscanf(str, format, ap);
    va_end(ap);
    return rc;
}