#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

static const char *const str = "<img height=\"\" alt=\"1MDN\" srcset=\"mdn-logo-HD.png 2x\"><img src=\"\" alt=\"2MDN\" srcset=\"mdn-logo-HD.png 2x\"><img src=\"\" alt=\"3MDN\" srcset=\"mdn-logo-HD.png 2x\"><img src=\"\" data-deferred=\"1\" class=\"rg_i Q4LuWd\" jsname=\"Q4LuWd\" width=\"169\" height=\"200\" alt=\"Holanda - Wikipedia, la enciclopedia libre\" data-iml=\"1468.5\" data-atf=\"true\"><img src=\"dsgfdhgf\" alt=\"DA - Home | Facebook\" jsname=\"HiaYvf\" jsaction=\"load:XAeZkd;\" class=\"n3VNCb\" data-noaft=\"1\" style=\"width: 385px; height: 385.664px; margin: 0px;\">";
static const char *const re = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";

int main(void)
{

    const char *s = str;
    regex_t regex;
    regmatch_t pmatch[1];
    regoff_t off, len;

    if (regcomp(&regex, re, REG_EXTENDED))
        exit(EXIT_FAILURE);

    printf("Cadena original = \"%s\"\n", str);
    printf("Coincidencias:\n");
    int i;
    for (i = 0;; i++)
    {
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        off = pmatch[0].rm_so + (s - str);
        len = pmatch[0].rm_eo - pmatch[0].rm_so;
        printf("subcadena = \"%.*s\"\n", len, s + pmatch[0].rm_so);

        s += pmatch[0].rm_eo;
    }
    printf("#%d:\n", i);

    exit(EXIT_SUCCESS);
}