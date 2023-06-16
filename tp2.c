#include "src/tp1.h"
#include "src/menu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN_ALIAS 5
#define AMARILLO "\x1b[33;1m"
#define RESET "\033[0m"

char *leer_interaccion(char *buffer, int tam)
{
	printf(AMARILLO"Ingrese una opción: \n"RESET);
	char *respuesta = fgets(buffer, tam, stdin);
	if (respuesta)
		respuesta[strlen(respuesta) - 1] = 0;

	return respuesta;
}

/**
* 
*/
bool ejecutar_salir(void *menu, void *hospitales)
{
	menu_destruir((menu_t*)menu);
	return false;
}

/**
*
*/
bool ejecutar_ayuda(void *menu, void *hospitales)
{
	menu_ayuda_mostrar(menu);
	return true;
}

/**
*
*/
bool ejecutar_cargar(void *menu, void *hospitales)
{
	return true;
}

/**
 * 
*/
bool ejecutar_estado(void *menu, void *hospitales)
{
	return true;
}

/**
 * 
*/
bool ejecutar_activar(void *menu, void *hospitales)
{
	return true;
}

/**
 * 
*/
bool ejecutar_mostrar(void *menu, void *hospitales)
{
	return true;
}

/**
 * 
*/
bool ejecutar_listar(void *menu, void *hospitales)
{
	return true;
}

/**
 * 
*/
bool ejecutar_destruir(void *menu, void *hospitales)
{
	return true;
}

void agregar_comandos(menu_t *menu)
{
	hash_t *alias_salir = hash_crear(MIN_ALIAS);
	hash_insertar(alias_salir, "SALIR", NULL, NULL);
	hash_insertar(alias_salir, "salir", NULL, NULL);
	hash_insertar(alias_salir, "s", NULL, NULL);
	hash_insertar(alias_salir, "exit", NULL, NULL);
	menu_agregar_comando(
		menu,
		comando_crear(
			"S", "Sale del programa",
			"Sale del programa, liberando toda la memoria ocupada por el programa",
			ejecutar_salir, alias_salir));

	hash_t *alias_ayuda = hash_crear(MIN_ALIAS);
	hash_insertar(alias_ayuda, "h", "h", NULL);
	hash_insertar(alias_ayuda, "help", NULL, NULL);
	hash_insertar(alias_ayuda, "ayuda", NULL, NULL);
	hash_insertar(alias_ayuda, "AYUDA", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"H", "Muestra el menu de ayuda",
			"Muestra el menu de ayuda, con todos los comandos y explicaciones ",
			ejecutar_ayuda, alias_ayuda));

	hash_t *alias_cargar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_cargar, "c", NULL, NULL);
	hash_insertar(alias_cargar, "cargar", NULL, NULL);
	hash_insertar(alias_cargar, "CARGAR", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"C", "Cargar hospital",
			"Pide un nombre de archivo e intenta cargarlo creando un hospital",
			ejecutar_cargar, alias_cargar));

	hash_t *alias_estado = hash_crear(MIN_ALIAS);
	hash_insertar(alias_estado, "e", NULL, NULL);
	hash_insertar(alias_estado, "estado", NULL, NULL);
	hash_insertar(alias_estado, "ESTADO", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"E", "Muestra el estado de los hospitales",
			"Muestra un listado con los hospitales cargados (y el activo, si hay alguno)",
			ejecutar_estado, alias_estado));

	hash_t *alias_activar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_activar, "a", NULL, NULL);
	hash_insertar(alias_activar, "activar", NULL, NULL);
	hash_insertar(alias_activar, "ACTIVAR", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"A", "Activa un hospital",
			"Pide un número de identificación y activa el hospital",
			ejecutar_activar, alias_activar));

	hash_t *alias_mostrar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_mostrar, "m", NULL, NULL);
	hash_insertar(alias_mostrar, "mostrar", NULL, NULL);
	hash_insertar(alias_mostrar, "MOSTRAR", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"M", "Muestra los hospitales",
			"Muestra un listado con los nombres de todos los pokemones en el hospital ",
			ejecutar_mostrar, alias_mostrar));

	hash_t *alias_listar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_listar, "l", NULL, NULL);
	hash_insertar(alias_listar, "listar", NULL, NULL);
	hash_insertar(alias_listar, "LISTAR", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"L", "Lista los hospitales",
			"Muestra un listado detallado de todos los pokemones en el hospital",
			ejecutar_listar, alias_listar));

	hash_t *alias_destruir = hash_crear(MIN_ALIAS);
	hash_insertar(alias_destruir, "d", NULL, NULL);
	hash_insertar(alias_destruir, "destruir", NULL, NULL);
	hash_insertar(alias_destruir, "DESTRUIR", NULL, NULL);

	menu_agregar_comando(
		menu,
		comando_crear(
			"D", "Destruye el hospital activo",
			"Destruye el hospital activo, liberando toda la memoria ocupada por el mismo",
			ejecutar_destruir, alias_destruir));

	/*hash_destruir(alias_activar);
	hash_destruir(alias_listar);
	hash_destruir(alias_destruir);
	hash_destruir(alias_ayuda);
	hash_destruir(alias_estado);
	hash_destruir(alias_mostrar);*/
}

int main()
{
	char buffer[512];
	buffer[0] = 0;
	char *comando;
	bool seguir = true;
	menu_t *menu = menu_crear("HOSPITAL POKEMONES");
	hospital_t **hospitales = NULL;
	agregar_comandos(menu);
	while (seguir) {
		menu_mostrar(menu);
		comando = leer_interaccion(buffer, 512);
		seguir = menu_ejecutar_comando(menu, comando, hospitales);
	}

	//menu_destruir_todo(menu, free);
	return 0;
}
