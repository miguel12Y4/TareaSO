#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/stat.h>


#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdint.h>
#include <regex.h>

#include <sys/time.h>


char* leer();
int match(char * text, char * exp );
int match(char * text, char * exp );
int matchA(char * text, char * exp );
int procesoImg(char * text);
int procesoScript(char * text);
int procesoA(char * text);



int main(void){

    char * text = leer(&text);
    
    //contadores de etiquetas
    int script;
    int img;
    int a;

    //pipe para la cantidad de tags del procesoImg
    int fdImg[2];
    pipe(fdImg);

    //pipe para la cantidad de tags del procesoScript
    int fdScript[2];
    pipe(fdScript);

    //pipe para la cantidad de tags del procesoA
    int fdA[2];
    pipe(fdA);

    //Variable para los tiempos

    //pipe para el tiempo del procesoImg
    int fdImgT[2];
    pipe(fdImgT);

    //pipe para el tiempo del procesoScript
    int fdScriptT[2];
    pipe(fdScriptT);

    //pipe para el tiempo del proceso A
    int fdAT[2];
    pipe(fdAT);

    int status;

    int f = fork();
    if(f==0){
        //proceso hijo

        //definir variable para calcular el tiempo
        struct timeval begin, end;
        //calcular tiempo de inicio
        gettimeofday(&begin, 0);

        //llamar a la funcion encargada de contar la cantidad de tag IMG
        int contImg = procesoImg(text);

        for (int i = 0; i < 100000000; i++)
                {
                }

        //calcular tiempo de fin
        gettimeofday(&end, 0);

        //calcular tiempo (está en microsegundos, se debe transformar a milisegundos)
        printf("1..... %d  - %d \n", end.tv_usec, begin.tv_usec);
        int seconds = (int)(end.tv_usec - begin.tv_usec)/1000;


        //usar pipe para escribir resultados y luego pasarlo al proceso padre
        close(fdImgT[0]);
        write(fdImgT[1], &seconds, sizeof(int));
        close(fdImgT[1]);

        //usar pipe para escribir resultados y luego pasarlo al proceso padre
        close(fdImg[0]);
        write(fdImg[1], &contImg, sizeof(int));
        close(fdImg[1]);
        
    }else{
        int s = fork();
        if(s==0){

            //definir variable para calcular el tiempo
            struct timeval begin, end;
            //calcular tiempo de inicio
            gettimeofday(&begin, 0);

            //llamar a la funcion encargada de contar la cantidad de tag SCRIPT
            int contScript = procesoScript(text);
            
            for (int i = 0; i < 100000000; i++)
                {
                }

            //calcular tiempo de fin
            gettimeofday(&end, 0);

            //calcular tiempo (está en microsegundos, se debe transformar a milisegundos)
            printf("2..... %d  - %d", end.tv_usec, begin.tv_usec);
            int seconds = (int) (end.tv_usec - begin.tv_usec)/1000;


            //usar pipe para escribir resultados y luego pasarlo al proceso padre
            close(fdScriptT[0]);
            write(fdScriptT[1], &seconds, sizeof(int));
            close(fdScriptT[1]);

            //usar pipe para escribir resultados y luego pasarlo al proceso padre
            close(fdScript[0]);
            write(fdScript[1], &contScript, sizeof(int));
            close(fdScript[1]);

        }else{
            int g = fork();
            if(g==0){

                //definir variable para calcular el tiempo
                struct timeval begin, end;
                //calcular tiempo de inicio
                gettimeofday(&begin, 0);

                //llamar a la funcion encargada de contar la cantidad de tag A
                int contA = procesoA(text);

                for (int i = 0; i < 100000000; i++)
                {
                }
                
                //calcular tiempo de fin
                gettimeofday(&end, 0);

                

                //calcular tiempo (está en microsegundos, se debe transformar a milisegundos)
                printf("%d  - %d", end.tv_usec, begin.tv_usec);
                int seconds = (int) (end.tv_usec - begin.tv_usec)/1000;


                //usar pipe para escribir resultados y luego pasarlo al proceso padre
                close(fdAT[0]);
                write(fdAT[1], &seconds, sizeof(int));
                close(fdAT[1]);

                //usar pipe para escribir resultados y luego pasarlo al proceso padre
                close(fdA[0]);
                write(fdA[1], &contA, sizeof(int));
                close(fdA[1]);

            }else{

                //esperar a que terminen los procesos hijos
                pid_t w = waitpid(f, &status, WUNTRACED);
                pid_t x = waitpid(s, &status, WUNTRACED);
                pid_t y = waitpid(g, &status, WUNTRACED);

                //pipe para obtener la cantidad de tags A
                close(fdA[1]);
                read(fdA[0],&a,sizeof(int));
                close(fdA[0]);
                
                //pipe para obtener la cantidad de tags IMG
                close(fdImg[1]);
                read(fdImg[0],&img,sizeof(int));
                close(fdImg[0]);

                //pipe para obtener la cantidad de tags SCRIPT
                close(fdScript[1]);
                read(fdScript[0],&script,sizeof(int));
                close(fdScript[0]);


                //pipes para leer los tiempos de

                //pipe para leer el tiempo de img
                int timeImg;
                close(fdImgT[1]);
                read(fdImgT[0],&timeImg,sizeof(int));
                close(fdImgT[0]);

                //pipe para leer el tiempo de script
                int timeScript;
                close(fdScriptT[1]);
                read(fdScriptT[0],&timeScript,sizeof(int));
                close(fdScriptT[0]);

                //pipe para leer el tiempo de a
                int timeA;
                close(fdAT[1]);
                read(fdAT[0],&timeA,sizeof(int));
                close(fdAT[0]);



                //mostrar los resultados
                printf("Estadisticas: \n");
                printf("las coincidencias de IMG son %d\n", img);
                printf("las coincidencias de Script son %d\n", script);
                printf("las coincidencias de A son %d\n\n\n", a);

                printf("Estadisticas del tiempo de ejecucion: \n");
                printf("los tiempos de IMG son %d\n", timeImg);
                printf("los tiempos de Script son %d\n", timeScript);
                printf("los tiempos de A son %d\n", timeA);
            }
        }

    }

}

int procesoImg(char * text){
    //expresion regular para obtener tags IMG
    char * exp = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";


    
    int cantidad = match(text, exp);
    

    return cantidad;
}

int procesoScript(char * text){

    //encontrar expresion regular
    char * exp = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";


    //realizar funcionalidad del proceso
    int cantidad = match(text, exp);
    
    
    return cantidad;
}

int procesoA(char * text){

    //encontrar expresion regular
    char * exp = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";

    

    //realizar funcionalidad del proceso
    int cantidad = match(text, exp);
    
    

    return cantidad;
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

    //printf("Coincidenci1as:\n");

    int i;
    for (i = 0;; i++)
    {
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        off = pmatch[0].rm_so + (s - text);
        len = pmatch[0].rm_eo - pmatch[0].rm_so;
        //printf("subcadena = \"%.*s\"\n", len, s + pmatch[0].rm_so);
        //printf("\n\n");

        s += pmatch[0].rm_eo;
    }
    return i;
}

//buscar comando grep 
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

    regex_t regex;
    regmatch_t pmatch[1];
    regoff_t off, len;

    if (regcomp(&regex, exp, REG_EXTENDED))
        return -1;


    int i;
    for (i = 0;; i++)
    {
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        off = pmatch[0].rm_so + (s - text);
        len = pmatch[0].rm_eo - pmatch[0].rm_so;

        //encontrar subcadena para obtener los links
        
        fprintf(input_file, "%.*s\n", len, s + pmatch[0].rm_so);

        s += pmatch[0].rm_eo;
    }
    fclose(input_file);
    return i;
}