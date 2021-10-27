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

//en la especificacion de la tarea aparece que se debe tomar en cuenta 
//la etiqueta a como <a></a> pero en la imagne subrrayada aparece <a ...>
//nosotros consideramos ambas posibilidades



//declaración de cabezera de funciones
//leer archivos
char* leer();

//encontrar cantidad de coincidencias dada una expresion regular (IMG o A)
int match(char * text, char * exp );

//encontrar expresion regular
int matchA(char * text, char * exp );

//procesos de buscar cada etiqueta
int procesoImg(char * text);
int procesoScript(char * text);
int procesoA(char * text);



int main(void){

    char * text = leer(&text);
    
    //contadores de etiquetas
    int script;
    int img;
    int a;

    //pipe para compartir la cantidad de tags del procesoImg con el padre
    int fdImg[2];
    pipe(fdImg);

    //pipe para compartir la cantidad de tags del procesoScript con el padre
    int fdScript[2];
    pipe(fdScript);

    //pipe para compartir la cantidad de tags del procesoA con el padre
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
        //proceso hijo encargado de buscar una etiqueta IMG

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
        int seconds = (int)(end.tv_usec - begin.tv_usec)/1000;


        //usar pipe para escribir resultados del tiempo tarnscurrido y luego pasarlo al proceso padre
        close(fdImgT[0]);
        write(fdImgT[1], &seconds, sizeof(int));
        close(fdImgT[1]);

        //usar pipe para escribir resultados de la cantidad de tag IMG y luego pasarlo al proceso padre
        close(fdImg[0]);
        write(fdImg[1], &contImg, sizeof(int));
        close(fdImg[1]);
        
    }else{
        int s = fork();
        if(s==0){
            //proceso hijo encargado de encontrar una etiqueta Script

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
                //proceso hijo encargado de encontrar etiquetas a

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
                printf("**** ESTADÍSTICAS DEL SITIO WEB ANALIZADO ****\n");
                printf("ENLACES ENCONTRADOS: %d\n", a);
                printf("IMÁGENES ENCONTRADAS: %d\n", img);
                printf("SCRIPT ANCLADOS AL SITIO: %d\n\n\n", script);

                printf("**** ESTADÍSTICAS DE EJECUCIÓN ***\n");
                printf("Hijo analizador de links PID: %d, demoró: %d Milisegundos\n", y ,timeA);
                printf("Hijo analizador de imágenes PID: %d, demoró: %d Milisegundos\n", w, timeImg);
                printf("Hijo analizador de scripts PID: %d, demoró: %d Milisegundos\n", x, timeScript);
            }
        }

    }

}

int procesoImg(char * text){
    //expresion regular para obtener tags IMG
    char * exp = "<img\\s+((\\s*[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>";


    //buscar la cantidad de etiquetas
    int cantidad = match(text, exp);
    
    //retornar la cantidad previamente calculada
    return cantidad;
}

int procesoScript(char * text){

    //expresion regulat para obtener todas las etiquetas Script
    char * exp = "<script\\s*((\\s+[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>[^<>]*<\\s*/\\s*script\\s*>";


    //Buscar la cantidad de etiquetas
    int cantidad = match(text, exp);
    
    //retornar la cantidad
    return cantidad;
}

int procesoA(char * text){

    //expresion regulat para obtener todas las etiquetas A
    char * exp = "<a\\s*((\\s+[^\"<>]+)\\s*=\\s*(\"[^\"<>]*\"\\s*))*\\s*>([^<>]*<\\s*/\\s*a\\s*>)?";

    

    //realizar funcionalidad del proceso
    int cantidad = matchA(text, exp);
    
    
    //retornar cantidad de etiquetas encontradas
    return cantidad;
}  

//leer un archivo de texto y retornar su contenido en un arreglo de char
char* leer() {
    
    //leer archivo .txt
    const char* filename = "input.html";

    FILE* input_file = fopen(filename, "r");
    if (!input_file)
        return "";

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return "";
    }

    //reservo espacio para guardar el contenido del txt
    char* file_contents = (char *)malloc(sb.st_size);

    //leo el contenido desde el archivo a file_contents
    fread(file_contents, sb.st_size, 1, input_file);

    //printf("el texto s %s\n", file_contents);

    //cierro el archuvo
    fclose(input_file);

    //retorno el contenido
    return file_contents;
}


#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))


int match(char * text, char * exp){

    const char *s = text;

    //variables para buscar coincidencias con una expresion regular dada
    regex_t regex;
    //datos de la posicion de las coincidencias
    regmatch_t pmatch[1];
    regoff_t off, len;

    //compilar expresion regular
    if (regcomp(&regex, exp, REG_EXTENDED))
        return -1;

    //comenzar a buscar coincidencias con una expresion regular
    int i;
    for (i = 0;; i++)
    {
        //buscar coincidencia
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        //detalles de la posicion de la coincidencia
        off = pmatch[0].rm_so;
        len = pmatch[0].rm_eo - pmatch[0].rm_so;

        //contendrá la subcadena que haya hecho match con la expresion regular
        char tag [len];
        strncpy(tag,  s+off, len);
    

        s += pmatch[0].rm_eo;
    }
    //retorna la cantidad de coincidencias
    return i;
}


int matchA(char * text, char * exp){
    
    //abrir archivo en el cual se insertarán los links
    const char* filename = "links.txt";
    FILE* input_file = fopen(filename, "w");

    //verificar que se halla abierto de arhivo .txt
    if (!input_file)
        return -1;

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return -1;
    }
    
    //texto que se quiere analizar
    const char *s = text;

    //variables para que se compilen las expresiones regulares que se quieren analizar
    regex_t regex, regexhref, regexlink;

    //contendrá los datos de la  subcadena que vaya haciendo match con la expresion regular dada
    regmatch_t pmatch[1], hrefmatch[1], linkmatch[1];

    //dirección y tamaño de la subcadena que hizo match con respecto al texto analizado
    regoff_t off, len;

    //compilar expresion regular para encontrar la etiqueta a
    if (regcomp(&regex, exp, REG_EXTENDED))
        return -1;

    
    //compilar expresion regular que se encargará que encontrar los href="link" de las etiquetas a
    char * exphref = "href\\s*=\\s*\".[^\"]*\"";
    if (regcomp(&regexhref, exphref, REG_EXTENDED))
        return -1;

    //compilar expresion regular que se encargará que encontrar los "link" de los href="link" de las etiquetas a
    char * explink = "\".[^\"]*\"";
    if(regcomp(&regexlink, explink, REG_EXTENDED))
        return -1;

    //comenzar a analizar la cadena
    int i;
    for (i = 0;; i++)
    {
        //buscar una etiqueta a en el texto
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0))
            break;

        //asignar valores de las posiciones de la coincidencia del tag a
        off = pmatch[0].rm_so;
        len = pmatch[0].rm_eo - pmatch[0].rm_so;

        //contendrá un string con la cadena encontrada <a ...>
        char tag [len];
        strncpy(tag,  s+off, len);
    

        //en la cadena encontrada (guardada en tag), se buscará el href="link", con una expresion regular
        if (!regexec(&regexhref, tag, ARRAY_SIZE(hrefmatch), hrefmatch, 0)){
            //posicion de la coincidencia
            int hoff = hrefmatch[0].rm_so;
            int hlen = hrefmatch[0].rm_eo - hrefmatch[0].rm_so;

            //contendrá un string con la cadena encontrada href="link"
            char datahref [hlen];
            strncpy(datahref,  tag+hoff, hlen);

            //en la cadena encontrada href="link", se buscará lo que hay en "link"
            if (!regexec(&regexlink, datahref, ARRAY_SIZE(linkmatch), linkmatch, 0)){
                //posicion de la coincidencia
                int loff = linkmatch[0].rm_so;
                int llen = linkmatch[0].rm_eo - linkmatch[0].rm_so;
                
                //contendrá la cadena "link"
                char link [hlen];
                strncpy(link,  datahref + loff, llen);

                //escribir en el archivo, pero sin las ""
                fprintf(input_file,"%.*s\n", llen-2, link+1);
            }
        }
        

        s += pmatch[0].rm_eo;
    }

    //se cierra el archivo de texto
    fclose(input_file);

    //se retorna la cantidad de coincidencias
    return i;
}
