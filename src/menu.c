#include "menu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CANTIDAD_BASE_COMANDOS 15
#define AZUL "\x1b[34;1m"
#define AMARILLO "\x1b[33;1m"

typedef struct comando_aux {
	comando_t *comando;
	const char *nombre_comando;
} comando_aux_t;

struct comando {
	void *nombre;
	void *documentacion;
	void *documentacion_completa;
	hash_t *alias;
	bool (*ejecutar)(void *, void *);
};

struct menu {
	hash_t *comandos;
	const char *nombre_menu;
};

void imprimir_linea(int longitud)
{
	for (int i = 0; i < longitud; i++)
		printf(AZUL "*");
	printf("\n");
}

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
 * 
*/
bool mostrar_cada_comando_completo(const char *clave, void *cmd, void *aux)
{
	if (!cmd)
		return false;

	comando_t *comando = cmd;
	bool primer_alias = true;
	int longitud_menu = *(int *)aux;

	printf(AMARILLO "   %s (", (char *)comando->nombre);
	hash_con_cada_clave(comando->alias, mostrar_cada_alias, &primer_alias);
	printf(") : %-.*s \n", longitud_menu - 16,
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
	printf("*" AMARILLO "         %-*c MENU DE %-*s  " AZUL "*\n", 35, ' ',
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

	printf(AMARILLO "     %s : %-*s  \n", (char *)comando->nombre, 35,
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
	printf("*" AMARILLO "           MENU DE %-*s  " AZUL "*\n",
	       longitud_menu - 23, menu->nombre_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	imprimir_linea(longitud_menu);
	printf("* %-*s *\n", longitud_menu - 4, "");
	hash_con_cada_clave(menu->comandos, mostrar_cada_comando,
			    &longitud_menu);
	imprimir_linea(longitud_menu);
}

/*
*
*/
char *copiar_texto(const char *texto)
{
	char *copia_texto = malloc(strlen(texto) + 1);
	if (!copia_texto)
		return NULL;
	return strcpy(copia_texto, texto);
}

comando_t *comando_crear(const char *cmd, const char *doc, const char *doc_aux,
			 bool (*ejecutar)(void *, void *), hash_t *alias)
{
	if (!cmd || !doc || !ejecutar)
		return NULL;

	comando_t *comando = calloc(1, sizeof(comando_t));
	comando->nombre = copiar_texto(cmd);
	comando->documentacion = copiar_texto(doc);
	comando->documentacion_completa = copiar_texto(doc_aux);
	comando->alias = alias;
	comando->ejecutar = ejecutar;
	return comando;
}

void comando_destruir(comando_t *comando)
{
	comando_destruir_todo(comando, NULL);
}

void comando_destruir_todo(comando_t *comando, void (*destructor)(void *))
{
	if (!comando)
		return;

	if (destructor) {
		destructor(comando->nombre);
		destructor(comando->documentacion);
		destructor(comando->documentacion_completa);
	}
	free(comando);
}

menu_t *menu_crear(const char *nombre_menu)
{
	if (!nombre_menu)
		return NULL;

	menu_t *menu = calloc(1, sizeof(menu_t));
	if (!menu)
		return NULL;

	menu->nombre_menu = copiar_texto(nombre_menu);

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

menu_t *menu_eliminar_comando(menu_t *menu, const char *cmd)
{
	if (!menu || !cmd)
		return NULL;

	menu->comandos = hash_quitar(menu->comandos, cmd);

	if (!menu->comandos)
		return NULL;

	return menu;
}

size_t menu_cantidad_comandos(menu_t *menu)
{
	return !menu ? 0 : hash_cantidad(menu->comandos);
}

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

comando_t *menu_contiene_comando(menu_t *menu, const char *cmd)
{
	if (!menu || !cmd)
		return NULL;

	comando_aux_t aux = { .nombre_comando = cmd,
			      .comando = hash_obtener(menu->comandos, cmd) };

	if (!aux.comando)
		hash_con_cada_clave(menu->comandos, buscar_comando, &aux);

	return aux.comando;
}

bool menu_ejecutar_comando(menu_t *menu, const char *cmd, void *aux)
{
	if (!menu || !cmd)
		return NULL;

	comando_t *comando = menu_contiene_comando(menu, cmd);

	return comando ? comando->ejecutar(menu, aux) : false;
}

void menu_destruir(menu_t *menu)
{
	menu_destruir_todo(menu, NULL);
}

/**
 * 
*/
bool menu_destruir_alias(const char *clave, void *elemento, void *aux)
{
	if (!clave || !elemento)
		return false;

	comando_t *comando = elemento;
	void (*destructor)(void *) = (void (*)(void *))(intptr_t)aux;
	comando_destruir_todo(comando, destructor);
	return true;
}

/**
 * 
*/
bool menu_destruir_comandos(const char *clave, void *elemento, void *aux)
{
	if (!clave || !elemento)
		return false;

	comando_t *comando = elemento;
	hash_con_cada_clave(comando->alias, menu_destruir_alias, aux);
	void (*destructor)(void *) = (void (*)(void *))(intptr_t)aux;
	comando_destruir_todo(comando, destructor);
	return true;
}

void menu_destruir_todo(menu_t *menu, void (*destructor)(void *))
{
	if (!menu)
		return;

	intptr_t destructor_int = (intptr_t)destructor;
	hash_con_cada_clave(menu->comandos, menu_destruir_comandos,
			    (void *)destructor_int);
	free(menu);
}