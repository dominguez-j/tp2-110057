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
comando_t *comando_crear(void *cmd, void *doc, void *doc_aux,
			 bool (*ejecutar)(void *, void *, void *), hash_t *alias);

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
 * Agrega un comando al menú.
 * Si se pudo agregar, devuelve el puntero a menú sino NULL.
*/
menu_t *menu_agregar_comando(menu_t *menu, comando_t *comando);

/**
 * Devuelve el comando si se encuentra el nombre de comando pasado por parámetro en el menú.
 * Caso contrario o de error, se retorna NULL.
*/
comando_t *menu_contiene_comando(menu_t *menu, void *cmd);

/**
 * Se ejecuta un comando y retorna true.
 * Caso contrario o de error, se retorna false.
*/
bool menu_ejecutar_comando(menu_t *menu, void *cmd, void *aux, void *aux2);

/**
 * Destruye el menu liberando toda la memoria ocupada por el mismo.
*/
void menu_destruir(menu_t *menu);

#endif // MENU_H_