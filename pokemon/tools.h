#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

int largestStr(char *arr[], int n);
int largestStr_bra(char *arr[], int n);
int largestOpt(char *choices[], int n_choice, const char *title);
unsigned long fileLines(const char *file, int opt);
char **readText(const char *file);
size_t u8_len(const char *s);
int replace_fmt(char **arr, int idx, const char *arg);

/**
 * @brief Recorre un arreglo de strings, y devuelve la cantidad del string mas largo
 * 
 * @param arr arreglo strings
 * @param n cantidad de strings
 * @return int -> cantidad del str mas largo
 */
int largestStr(char *arr[], int n)
{
    if(n <= 0)
    {
        perror("No array length");
        exit(1);
    }
    int max = 0, count = 0;
    for(int i = 0; i < n; i++)
    {
        count = (int)strlen(arr[i]);
        if(count > max)
            max = count;
    }
    return max;
}
/**
 * @brief Recorre un arreglo de strings en braile, y devuelve la cantidad del string mas largo
 * 
 * @param arr arreglo strings
 * @param n cantidad de strings
 * @return int -> cantidad del str mas largo
 */
int largestStr_bra(char *arr[], int n)
{
    if(n <= 0)
    {
        perror("No array length");
        exit(1);
    }
    int max = 0, count = 0;
    for(int i = 0; i < n; i++)
    {
        count = (int)u8_len(arr[i]);
        if(count > max)
            max = count;
    }
    return max;
}
/**
 * @brief Calcula el ancho optimo para la ventana de menu
 * 
 * @param m estructura menu
 * @return int -> Ancho optimo
 */
int largestOpt(char *choices[], int n_choice, const char *title)
{
    int length = largestStr(choices, n_choice);
    if(length < (int)strlen(title))
        return (int)strlen(title) + 4;
    else
        return length + 6;
}
/**
 * @brief Devuelve el numero de lineas o el maximo de caracteres de un string dentro de un archivo
 * 
 * @param file 
 * @param opt elegir: cantidad de lineas = 0 / maximo de caracteres = 1
 * @return unsigned long 
 */
unsigned long fileLines(const char *file, int opt)
{
    FILE *txt = fopen(file, "r");
    if(!txt)
    {
        printf("Error: file '%s' not found\n", file);
        return 0;
    }
    unsigned long lines_count = 0, char_count = 0, WMAX = 0;
    char t;
    while ((t = fgetc(txt)) != EOF)
	{

        if (t == '\n' || t == '\r')
		{
			if(char_count > WMAX)
			{
				WMAX = char_count;
			}
            lines_count++;
			char_count = 0;
		}
		else
			char_count++;
    }
	if(lines_count > 1)
		lines_count++; //Linea adicional porque no existe \n inicial
    fclose(txt);
    if (opt == 0)
        return lines_count;
    else
        return WMAX;
}
/**
 * @brief Lee un archivo y transforma el contenido en un arreglo de strings
 * 
 * @param file archivo
 * @return char** Devuelve el arreglo
 */
char **readText(const char *file)
{
    unsigned long lines = fileLines(file, 0);
    unsigned long chars = fileLines(file, 1);
    if (lines == 0 || chars == 0)
        return NULL;

    FILE *txt = fopen(file, "r");
    if (!txt)
        return NULL;

    char **arr = calloc(lines, sizeof(char *));
    if (!arr) {
        fclose(txt);
        return NULL;
    }

    for (unsigned long i = 0; i < lines; i++) {
        arr[i] = calloc(chars + 2, sizeof(char));
        if (!arr[i]) {
            fclose(txt);
            for (unsigned long j = 0; j < i; j++)
                free(arr[j]);
            free(arr);
            return NULL;
        }
        if (!fgets(arr[i], (int)(chars + 2), txt))
            arr[i][0] = '\0';
        arr[i][strcspn(arr[i], "\r\n")] = '\0';
    }
    fclose(txt);
    return arr;
}

size_t u8_len(const char *s)
{
    mbstate_t st = {0};
    size_t count = 0;
    const char *p = s;
    wchar_t wc;
    while (*p)
    {
        size_t r = mbrtowc(&wc, p, MB_CUR_MAX, &st);
        if (r == (size_t)-1 || r == (size_t)-2)
            break;
        if (r == 0)
            break;
        count++;
        p += r;
    }
    return count;
}
/**
 * @brief Formatea una entrada con %s y reemplaza el puntero en el arreglo.
 *
 * No libera el string anterior (pensado para literales).
 *
 * @param arr arreglo de strings
 * @param idx indice a formatear
 * @param arg argumento que reemplaza %s
 * @return 0 en exito, -1 en error
 */
int replace_fmt(char **arr, int idx, const char *arg)
{
    if (!arr || idx < 0 || !arg)
        return -1;

    const char *tmpl = arr[idx];
    if (!tmpl)
        return -1;

    size_t needed = (size_t)snprintf(NULL, 0, tmpl, arg);
    char *buf = malloc(needed + 1);
    if (!buf)
        return -1;

    snprintf(buf, needed + 1, tmpl, arg);
    arr[idx] = buf;
    return 0;
}
#endif /* TOOLS_H */

//damage