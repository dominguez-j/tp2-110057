#include "menu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CANTIDAD_BASE_COMANDOS 15
#define AZUL "\x1b[34;1m"
#define AMARILLO "\x1b[33;1m"
#define VERDE "\x1b[38;5;43m"
#define ROJO "\x1b[38;5;203m"
#define RESET "\033[0m"

typedef struct comando_aux {
	comando_t *comando;
	char *nombre_comando;
} comando_aux_t;

struct comando {
	char *nombre;
	void *documentacion;
	void *documentacion_completa;
	hash_t *alias;
	bool (*ejecutar)(void *, void *);
};

struct menu {
	char *nombre_menu;
	hash_t *comandos;
};

/**
 * Imprime por pantalla una linea de *.
*/
void imprimir_linea(int longitud)
{
	for (int i = 0; i < longitud; i++)
		printf(AZUL "*");
	printf("\n");
}

/**
 * Muestra la clave de cada alias.
*/
bool mostrar_cada_alias(const char *clave, void *elemento, void *aux)
{
	if (!clave)
		return false;

	bool *primer_alias = (bool *)aux;

	if (*primer_alias) {
		printf("%s", clave);
		*primer_alias = false;
	} else
		printf(" / %s", clave);
	return true;
}

/**
 * Muestra por pantalla toda la información completa de todos los comandos.
 * Si se pueden mostrar retorna true, sino false.
*/
bool mostrar_cada_comando_completo(const char *clave, void *cmd, void *aux)
{
	if (!cmd)
		return false;

	comando_t *comando = cmd;
	bool primer_alias = true;
	int longitud_menu = *(int *)aux;

	printf(VERDE "   %s (", (char *)comando->nombre);
	hash_con_cada_clave(comando->alias, mostrar_cada_alias, &primer_alias);
	printf(")" AMARILLO" : %-.*s \n", longitud_menu - 16,
	       (char *)comando->documentacion_completa);
	printf(AZUL "* %-*s *\n", longitud_menu - 4, "");

	return true;
}

void menu_ayuda_mostrar(menu_t *menu)
{
	if (!menu)
		return;

	int longitud_menu = 111;

	printf("\n");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	printf("*" ROJO "         %-*c MENU DE %-*s  " AZUL "*\n", 35, ' ',
	       longitud_menu - 57, "AYUDA");
	printf("* %-*s *\n", longitud_menu - 4, "");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	hash_con_cada_clave(menu->comandos, mostrar_cada_comando_completo,
			    &longitud_menu);
	imprimir_linea(longitud_menu);
}

/**
 * 
*/
bool mostrar_cada_comando(const char *clave, void *cmd, void *aux)
{
	if (!cmd)
		return false;

	comando_t *comando = cmd;
	int longitud_menu = *(int *)aux;

	printf(VERDE "     %s " AMARILLO": %-*s  \n", (char *)comando->nombre, 35,
	       (char *)comando->documentacion);
	printf(AZUL "* %-*s *\n", longitud_menu - 4, "");

	return true;
}

void menu_mostrar(menu_t *menu)
{
	if (!menu)
		return;

	int longitud_menu = 50;

	printf("\n");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	printf("*" ROJO "           MENU DE %-*s  " AZUL "*\n",
	       longitud_menu - 23, menu->nombre_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	hash_con_cada_clave(menu->comandos, mostrar_cada_comando,
			    &longitud_menu);
	imprimir_linea(longitud_menu);
}

comando_t *comando_crear(void *cmd, void *doc, void *doc_aux,
			 bool (*ejecutar)(void *, void *), hash_t *alias)
{
	if (!cmd || !doc || !ejecutar)
		return NULL;

	comando_t *comando = malloc(sizeof(comando_t));

	if (!comando)
		return NULL;

	comando->nombre = cmd;
	comando->documentacion = doc;
	comando->documentacion_completa = doc_aux;
	comando->alias = alias;
	comando->ejecutar = ejecutar;

	return comando;
}

/**
 * Libera la memoria ocupada por el comando.
*/
void comando_destruir(comando_t *comando)
{
	if (comando)
		free(comando);
}

menu_t *menu_crear(void *nombre_menu)
{
	if (!nombre_menu)
		return NULL;

	menu_t *menu = malloc(sizeof(menu_t));
	if (!menu)
		return NULL;

	menu->nombre_menu = nombre_menu;

	if (!menu->nombre_menu) {
		free(menu);
		return NULL;
	}

	menu->comandos = hash_crear(CANTIDAD_BASE_COMANDOS);
	if (!menu->comandos) {
		free(menu);
		return NULL;
	}

	return menu;
}

menu_t *menu_agregar_comando(menu_t *menu, comando_t *comando)
{
	if (!menu || !comando)
		return NULL;

	menu->comandos =
		hash_insertar(menu->comandos, comando->nombre, comando, NULL);

	if (!menu->comandos)
		return NULL;

	return menu;
}

/**
 * Devuelve false cuando encuentra el comando y modifica el puntero de aux.
 * Caso contrario, devuelve true.
*/
bool buscar_comando(const char *clave, void *elemento, void *aux)
{
	if (!aux || !elemento)
		return true;

	comando_aux_t *comando_buscado = aux;
	comando_t *comando = elemento;
	bool existe =
		hash_contiene(comando->alias, comando_buscado->nombre_comando);

	if (existe) {
		comando_buscado->comando = comando;
		return false;
	}

	return true;
}

comando_t *menu_contiene_comando(menu_t *menu, void *cmd)
{
	if (!menu || !cmd)
		return NULL;

	comando_aux_t aux = { .nombre_comando = cmd,
			      .comando = hash_obtener(menu->comandos, cmd) };

	if (!aux.comando)
		hash_con_cada_clave(menu->comandos, buscar_comando, &aux);

	return aux.comando;
}

bool menu_ejecutar_comando(menu_t *menu, void *cmd, void *aux)
{
	if (!menu || !cmd)
		return NULL;

	comando_t *comando = menu_contiene_comando(menu, cmd);

	if (comando)
		return comando->ejecutar(menu, aux);

	printf(AMARILLO
	       "Comando inexistente, por favor use el comando de ayuda\n" RESET);
	return true;
}

/**
 * Destruye toda la memoria ocupada por un comando, incluyendo su hash alias.
*/
bool menu_destruir_comandos(const char *clave, void *elemento, void *aux)
{
	if (!clave || !elemento)
		return false;

	comando_t *comando = elemento;
	hash_destruir(comando->alias);
	comando_destruir(comando);
	return true;
}

void menu_destruir(menu_t *menu)
{
	if (!menu)
		return;

	if (!menu->comandos) {
		free(menu);
		return;
	}

	hash_con_cada_clave(menu->comandos, menu_destruir_comandos, NULL);
	hash_destruir(menu->comandos);
	free(menu);
}