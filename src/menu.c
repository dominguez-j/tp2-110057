#include "menu.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CANTIDAD_BASE_COMANDOS 15
#define CANTIDAD_BASE_ALIAS 10
#define AZUL "\x1b[34;1m"
#define AMARILLO "\x1b[33;1m"
#define VERDE "\x1b[38;5;43m"
#define NARANJA "\x1b[38;5;208m"
#define ROJO "\x1b[31;1m"
#define RESET "\033[0m"

typedef struct comando_aux {
	comando_t *comando;
	char *nombre_comando;
} comando_aux_t;

struct comando {
	char *nombre;
	void *info;
	void *info_ayuda;
	hash_t *alias;
	bool (*ejecutar)(void *, void *, void *);
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
	printf(")" AMARILLO " : %-.*s \n", longitud_menu - 16,
	       (char *)comando->info_ayuda);
	printf(AZUL "* %-*s *\n", longitud_menu - 4, "");

	return true;
}

void menu_ayuda_mostrar(menu_t *menu)
{
	if (!menu)
		return;

	int longitud_menu = 113;

	printf("\n");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	printf("*" NARANJA "         %-*c MENU DE %-*s  " AZUL "*\n", 35, ' ',
	       longitud_menu - 57, "AYUDA");
	printf("* %-*s *\n", longitud_menu - 4, "");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	hash_con_cada_clave(menu->comandos, mostrar_cada_comando_completo,
			    &longitud_menu);
	imprimir_linea(longitud_menu);
}

/**
 * Se encarga de mostrar por pantalla cada comando que recibe.
 * Si no hay comando, retorna false. Caso contrario true.
*/
bool mostrar_cada_comando(const char *clave, void *cmd, void *aux)
{
	if (!cmd)
		return false;

	comando_t *comando = cmd;
	int longitud_menu = *(int *)aux;

	printf(VERDE "     %s " AMARILLO ": %-*s  \n", (char *)comando->nombre,
	       35, (char *)comando->info);
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
	printf("*" NARANJA "           MENU DE %-*s  " AZUL "*\n",
	       longitud_menu - 23, menu->nombre_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	hash_con_cada_clave(menu->comandos, mostrar_cada_comando,
			    &longitud_menu);
	imprimir_linea(longitud_menu);
}

comando_t *comando_crear(void *cmd, void *info, void *info_ayuda,
			 bool (*ejecutar)(void *, void *, void *))
{
	if (!cmd || !info || !info_ayuda || !ejecutar)
		return NULL;

	comando_t *comando = malloc(sizeof(comando_t));

	if (!comando)
		return NULL;

	comando->nombre = cmd;
	comando->info = info;
	comando->info_ayuda = info_ayuda;
	comando->alias = NULL;
	comando->ejecutar = ejecutar;

	return comando;
}

char *comando_nombre(comando_t *cmd)
{
	return !cmd ? NULL : cmd->nombre;
}

char *comando_informacion(comando_t *cmd)
{
	return !cmd ? NULL : cmd->info;
}

char *comando_informacion_completa(comando_t *cmd)
{
	return !cmd ? NULL : cmd->info_ayuda;
}

comando_t *comando_agregar_alias(comando_t *cmd, const char *alias)
{
	if (!cmd || !alias)
		return NULL;

	if (!cmd->alias) {
		cmd->alias = hash_crear(CANTIDAD_BASE_ALIAS);
		if (!cmd->alias)
			return NULL;
	}

	hash_t *aux = hash_insertar(cmd->alias, alias, NULL, NULL);

	return !aux ? NULL : cmd;
}

void comando_destruir(comando_t *comando)
{
	if (!comando)
		return;

	hash_destruir(comando->alias);
	free(comando);
}

menu_t *menu_crear(void *nombre_menu)
{
	if (!nombre_menu)
		return NULL;

	menu_t *menu = malloc(sizeof(menu_t));
	if (!menu)
		return NULL;

	menu->comandos = hash_crear(CANTIDAD_BASE_COMANDOS);
	if (!menu->comandos) {
		free(menu);
		return NULL;
	}

	menu->nombre_menu = nombre_menu;

	return menu;
}

menu_t *menu_agregar_comando(menu_t *menu, comando_t *cmd)
{
	if (!menu || !cmd)
		return NULL;

	hash_t *aux = hash_insertar(menu->comandos, cmd->nombre, cmd, NULL);

	return !aux ? NULL : menu;
}

char *menu_nombre(menu_t *menu)
{
	return !menu ? NULL : menu->nombre_menu;
}

size_t menu_cantidad(menu_t *menu)
{
	return !menu ? 0 : hash_cantidad(menu->comandos);
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

bool menu_ejecutar_comando(menu_t *menu, void *cmd, void *aux, void *aux2)
{
	if (!menu || !cmd)
		return true;

	comando_t *comando = menu_contiene_comando(menu, cmd);

	if (comando)
		return comando->ejecutar(menu, aux, aux2);

	printf(ROJO
	       "\nComando inexistente, por favor use el comando de ayuda\n" RESET);
	return true;
}

size_t menu_con_cada_comando(menu_t *menu,
			     bool (*f)(const char *nombre_comando,
				       void *comando, void *aux),
			     void *aux)
{
	return !menu || !f ? 0 : hash_con_cada_clave(menu->comandos, f, aux);
}

/**
 * Destruye toda la memoria ocupada por un comando, incluyendo su hash alias.
*/
bool menu_destruir_comandos(const char *clave, void *elemento, void *aux)
{
	if (!clave || !elemento)
		return false;

	comando_t *comando = elemento;
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

	menu_con_cada_comando(menu, menu_destruir_comandos, NULL);
	hash_destruir(menu->comandos);
	free(menu);
}