#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>
#include <stddef.h>
#include "hash.h"

typedef struct comando comando_t;
typedef struct menu menu_t;

/**
 * Crea un comando reservando la memoria necesaria para él.
 * Devuelve el puntero a comando o NULL en caso de error.
*/
comando_t *comando_crear(const char *cmd, const char *doc, const char *doc_aux,
			 bool (*ejecutar)(void *, void *), hash_t *alias);

/**
 * 
*/
void comando_destruir(comando_t *comando);

/**
 * 
*/
void comando_destruir_todo(comando_t *comando, void (*destructor)(void *));

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
menu_t *menu_crear(const char *nombre_menu);

/**
 * Agrega un comando al menú.
 * Si se pudo agregar, devuelve el puntero a menú sino NULL.
*/
menu_t *menu_agregar_comando(menu_t *menu, comando_t *comando);

/**
 * Elimina un comando del menú.
 * Si se pudo eliminar, devuelve el puntero a menú sino NULL.
*/
menu_t *menu_eliminar_comando(menu_t *menu, const char *cmd);

/**
 * Devuelve la cantidad de comandos que tiene el menú.
 * En caso de error, retorna 0.
*/
size_t menu_cantidad_comandos(menu_t *menu);

/**
 * Devuelve el comando si se encuentra el nombre de comando pasado por parámetro en el menú.
 * Caso contrario o de error, se retorna NULL.
*/
comando_t *menu_contiene_comando(menu_t *menu, const char *cmd);

/**
 * 
*/
bool menu_ejecutar_comando(menu_t *menu, const char *cmd, void *aux);

/**
 * Destruye el menu liberando la memoria reservada.
*/
void menu_destruir(menu_t *menu);

/**
 * Destruye el menu liberando la memoria reservada y asegurandose de
 * invocar la funcion destructora con cada elemento almacenado en el
 * menu.
*/
void menu_destruir_todo(menu_t *menu, void (*destructor)(void *));

#endif // MENU_H_