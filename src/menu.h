#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct comando comando_t;
typedef struct menu menu_t;

/**
 * Crea un comando reservando la memoria necesaria para él.
 * Pide un nombre de comando, la informacion del mismo y una información más completa.
 * A su vez, recibe una función que va a ejecutar lo que hace el comando.
 * Y tambien recibe un hash de alias.
 * Devuelve el puntero a comando o NULL en caso de error.
*/
comando_t *comando_crear(void *cmd, void *doc, void *doc_aux,
			 bool (*ejecutar)(void *, void *, void *));

/**
 * Devuelve el nombre del comando. En caso de error, NULL.
*/
char *comando_nombre(comando_t *cmd);

/**
 * Devuelve la informacion del comando. En caso de error, NULL.
*/
char *comando_informacion(comando_t *cmd);

/**
 * Devuelve la informacion completa del comando. En caso de error, NULL.
*/
char *comando_informacion_completa(comando_t *cmd);

/**
 * Agrega un alias para un comando. Si se pudo agregar se devuelve un puntero al comando.
 * Sino se devuelve NULL.
*/
comando_t *comando_agregar_alias(comando_t *cmd, const char *alias);

/**
 * Libera la memoria ocupada por el comando.
*/
void comando_destruir(comando_t *comando);

/*
* Muestra el menú de ayuda con sus comandos y alias.
*/
void menu_ayuda_mostrar(menu_t *menu);

/*
* Muestra el menú con sus comandos.
*/
void menu_mostrar(menu_t *menu);

/**
 * Crea el menú reservando la memoria necesaria para el.
*/
menu_t *menu_crear(void *nombre_menu);

/**
 * Devuelve el nombre del menu. En caso de error, NULL.
*/
char *menu_nombre(menu_t *menu);

/**
 * Agrega un comando al menú.
 * Si se pudo agregar, devuelve el puntero a menú sino NULL.
*/
menu_t *menu_agregar_comando(menu_t *menu, comando_t *cmd);

/**
 * Devuelve el comando si se encuentra el nombre de comando pasado por parámetro en el menú.
 * Caso contrario o de error, se retorna NULL.
*/
comando_t *menu_contiene_comando(menu_t *menu, void *cmd);

/**
 * Devuelve la cantidad de comandos.
*/
size_t menu_cantidad(menu_t *menu);

/**
 * Se ejecuta un comando y retorna true.
 * Caso contrario o de error, se retorna false.
*/
bool menu_ejecutar_comando(menu_t *menu, void *cmd, void *aux, void *aux2);

/**
 * Destruye el menu liberando toda la memoria ocupada por el mismo.
*/
void menu_destruir(menu_t *menu);

/*
 * Recorre cada comando almacenador en el menu e invoca a la
 * función f, pasandole como parámetros el nombre del comando, el comando y
 * el puntero auxiliar.
 *
 * Mientras que queden mas claves o la funcion retorne true, la
 * iteración continúa. Cuando no quedan mas claves o la función
 * devuelve false, la iteración se corta y la función principal
 * retorna.
 *
 * Devuelve la cantidad de claves totales iteradas (la cantidad de
 * veces que fue invocada la función) o 0 en caso de error.
 *
 */
size_t menu_con_cada_comando(menu_t *menu,
			     bool (*f)(const char *nombre_comando,
				       void *comando, void *aux),
			     void *aux);

#endif // MENU_H_