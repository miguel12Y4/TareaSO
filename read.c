#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/stat.h>


#include <sys/wait.h>

#include <sys/types.h>
#include <unistd.h>

#include <stdint.h>
#include <regex.h>

char* leer();
int match(char * text, char * exp );
int match(char * text, char * exp );
int matchA(char * text, char * exp );
int procesoImg(char * text);
int procesoScript(char * text);
int procesoA(char * text);

int main(void){

    char * text = leer(&text);
    
    int img = 0;

    //tuberia para el procesoImg
    int fd[2];
    pipe(fd);

    //tuberia para el procesoScript

    //tuberia para el proceso A


    int status;

    int f = fork();
    if(f==0){

        close(fd[0]);
        int n = procesoImg(text);
        write(fd[1], &n, sizeof(int));
        
    }else{
        int s = fork();
        if(s==0){
            int n = procesoScript(text);
            //codigo de la etiqueta script

        }else{
            int g = fork();
            if(g==0){
                int n = procesoA(text);
                //codigo de la etiqueta a

            }else{
                pid_t w = waitpid(f, &status, WUNTRACED);

                close(fd[1]);
                read(fd[0],&img,sizeof(int));

                printf("las coincidencias son %d\n", img);
            }
        }

    }

}

int procesoImg(char * text){

    char * exp = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";

    int numero = matchA(text, exp);

    printf("las coincidencias son %d\n", numero);

    return numero;
}

int procesoScript(char * text){

    char * exp = "falta expresion regular";

    match(text, exp);

}

int procesoA(char * text){
    char * exp = "falta expresion regular";
    
    match(text, exp);

}

char* leer() {
    
    const char* filename = "input.html";

    FILE* input_file = fopen(filename, "r");
    if (!input_file)
        return "";

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return "";
    }

    char* file_contents = (char *)malloc(sb.st_size);

    
    fread(file_contents, sb.st_size, 1, input_file);

    //printf("el texto s %s\n", file_contents);

    fclose(input_file);

    return file_contents;
}



#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

static const char *const str = "<img height=\"\" alt=\"1MDN\" srcset=\"mdn-logo-HD.png 2x\"><img src=\"\" alt=\"2MDN\" srcset=\"mdn-logo-HD.png 2x\"><img src=\"\" alt=\"3MDN\" srcset=\"mdn-logo-HD.png 2x\"><img src=\"\" data-deferred=\"1\" class=\"rg_i Q4LuWd\" jsname=\"Q4LuWd\" width=\"169\" height=\"200\" alt=\"Holanda - Wikipedia, la enciclopedia libre\" data-iml=\"1468.5\" data-atf=\"true\"><img src=\"dsgfdhgf\" alt=\"DA - Home | Facebook\" jsname=\"HiaYvf\" jsaction=\"load:XAeZkd;\" class=\"n3VNCb\" data-noaft=\"1\" style=\"width: 385px; height: 385.664px; margin: 0px;\">";
static const char *const re = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";

int match(char * text, char * exp){
    const char *s = text;
    //printf("la expresion es %s\n", s);
    regex_t regex;
    regmatch_t pmatch[1];
    regoff_t off, len;

    if (regcomp(&regex, exp, REG_EXTENDED))
        return -1;

    printf("Coincidencias:\n");

    int i;
    for (i = 0;; i++)
    {
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        off = pmatch[0].rm_so + (s - text);
        len = pmatch[0].rm_eo - pmatch[0].rm_so;
        printf("subcadena = \"%.*s\"\n", len, s + pmatch[0].rm_so);
        printf("\n\n");

        s += pmatch[0].rm_eo;
    }
    return i;
}

int matchA(char * text, char * exp){
    
    //abrir archivo en el cual se insertarán los links
    const char* filename = "links.txt";

    FILE* input_file = fopen(filename, "w");
    if (!input_file)
        return -1;

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return -1;
    }
    
    //busco los match
    const char *s = text;
    //printf("la expresion es %s\n", s);
    regex_t regex;
    regmatch_t pmatch[1];
    regoff_t off, len;

    if (regcomp(&regex, exp, REG_EXTENDED))
        return -1;

    //printf("Coincidencias:\n");

    int i;
    for (i = 0;; i++)
    {
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        off = pmatch[0].rm_so + (s - text);
        len = pmatch[0].rm_eo - pmatch[0].rm_so;
        fprintf(input_file, "%.*s\n", len, s + pmatch[0].rm_so);

        s += pmatch[0].rm_eo;
    }
    fclose(input_file);
    return i;
}