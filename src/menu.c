#include "menu.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CANTIDAD_BASE_COMANDOS 15

struct comando {
	void *nombre;
	void *documentacion;
	bool es_alias;
	bool (*ejecutar)(void *, void *);
};

struct menu {
	hash_t *comandos;

};

/**
 * 
*/
bool mostrar_cada_comando(const char* clave, void *valor, void* aux)
{
	comando_t *comando = (comando_t*)valor;

	if (!comando->es_alias)
            printf("*  %s : %-*s  *\n", (char*)comando->nombre, 35, (char*)comando->documentacion);

	return true;
}

void menu_mostrar(menu_t *menu, const char *nombre_menu)
{	
	if(!menu || !nombre_menu)
		return;

	printf("\n");
	printf("*********************************************\n");
	printf("*         MENU DE %s        *\n", nombre_menu);
	printf("*********************************************\n");
	hash_con_cada_clave(menu->comandos, mostrar_cada_comando, NULL);
	printf("*********************************************\n");
}

/*
*
*/
char *copiar_texto(const char *texto)
{
	char *copia_text = malloc(strlen(texto) + 1);
	if (!copia_text)
		return NULL;
	return strcpy(copia_text, texto);
}

comando_t *comando_crear(const char *cmd, const char *doc,
			 bool (*ejecutar)(void *, void *), bool es_alias)
{
	if (!cmd || !doc || !ejecutar)
		return NULL;

	comando_t *comando = calloc(1, sizeof(comando_t));
	comando->nombre = copiar_texto(cmd);
	comando->documentacion = copiar_texto(doc);
	comando->es_alias = es_alias;
	comando->ejecutar = ejecutar;
	return comando;
}

menu_t *menu_crear()
{
	menu_t *menu = calloc(1, sizeof(menu_t));
	if (!menu)
		return NULL;

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

comando_t *menu_contiene_comando(menu_t *menu, const char *cmd)
{
	return (comando_t*)hash_obtener(menu->comandos, cmd);
}

bool menu_ejecutar_comando(menu_t *menu, const char *cmd)
{
	if(!menu || !cmd)
		return NULL;

	comando_t *comando = menu_contiene_comando(menu, cmd);

	return comando ? comando->ejecutar : NULL;
}

void menu_destruir(menu_t *menu)
{
	menu_destruir_todo(menu, NULL);
}

void menu_destruir_todo(menu_t *menu, void (*destructor)(void *))
{
	hash_destruir_todo(menu->comandos, destructor);
	free(menu);
}