#include "src/tp1.h"
#include "src/menu.h"
#include "src/hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUFFER 100
#define MIN_ALIAS 5
#define MIN_HOSPITALES 3
#define MAX_ID 3
#define AMARILLO "\x1b[33;1m"
#define VERDE "\x1b[32;1m"
#define ROJO "\x1b[31;1m"
#define NARANJA "\x1b[38;5;208m"
#define RESET "\033[0m"

typedef struct centro {
	hospital_t *hospital;
	char *nombre_hospital;
	bool activo;
	char id[MAX_ID];
} centro_t;

typedef struct sistema_hospitales {
	hash_t *hospitales;
	centro_t *hospital_activo;
	char id_hospital_activo[MAX_ID];
	size_t contador_ids;
} sistema_hospitales_t;

/**
 * Pide un ingreso al usuario. Retorna la respuesta del usuario.
 * En caso de error, retorna NULL.
*/
char *leer_interaccion(char *buffer, char *texto)
{
	printf(AMARILLO "%s \n" RESET, texto);
	char *respuesta = fgets(buffer, MAX_BUFFER, stdin);
	if (respuesta)
		respuesta[strlen(respuesta) - 1] = 0;

	return respuesta;
}

/**
 * Libera toda la memoria ocupada por un centro.
 * Si no se pudo liberar, retorna false. Caso contrario true.
*/
bool centro_destruir_todo(const char *clave, void *elemento, void *aux)
{
	if (!elemento)
		return false;

	centro_t *centro = elemento;

	hospital_destruir(centro->hospital);
	free(centro->nombre_hospital);
	free(centro);

	return true;
}

/**
* Libera toda la memoria ocupada por el programa.
* La del menu y el centro. Y retorna false.
*/
bool ejecutar_salir(void *menu, void *centros, void *buffer)
{
	sistema_hospitales_t *sistema = centros;
	hash_con_cada_clave(sistema->hospitales, centro_destruir_todo, NULL);
	hash_destruir(sistema->hospitales);
	free(sistema);
	menu_destruir(menu);
	return false;
}

/**
* Invoca el menu de ayuda y pide un ingreso.
* Retorna la funcion que ejecuta ese ingreso.
*/
bool ejecutar_ayuda(void *menu, void *aux, void *buffer)
{
	menu_ayuda_mostrar(menu);
	char *comando = leer_interaccion(buffer, "Ingrese una opción: ");
	return menu_ejecutar_comando(menu, comando, aux, buffer);
}

/**
 * Se crea una copia del nombre y se la devuelve. Caso contrario, se retorna NULL.
*/
char *copiar_nombre(char *nombre)
{
	char *copia_nombre = malloc(strlen(nombre) + 1);
	if (!copia_nombre)
		return NULL;
	return strcpy(copia_nombre, nombre);
}

/**
* Se encarga de cargar la direccion del archivo en un centro.
* Retorna true, se haya cargado bien o no.
*/
bool ejecutar_cargar(void *menu, void *centros, void *buffer)
{
	if (!centros) {
		printf(ROJO "\nHubo un error al crear el hospital.\n" RESET);
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	char *archivo = leer_interaccion(
		buffer, "\nIngrese la dirección del archivo: ");

	hospital_t *hospital_aux = hospital_crear_desde_archivo(archivo);
	if (!hospital_aux) {
		printf(ROJO "\nHubo un error al crear el hospital.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	centro_t *centro = calloc(1, sizeof(centro_t));
	if (!centro) {
		printf(ROJO "\nHubo un error al crear el hospital\n");
		printf("Volviendo al menu principal...\n" RESET);
		hospital_destruir(hospital_aux);
		return true;
	}
	centro->nombre_hospital = copiar_nombre(archivo);
	centro->activo = false;
	centro->hospital = hospital_aux;

	sistema_hospitales_t *sistema = centros;
	sistema->contador_ids++;
	snprintf(centro->id, sizeof(centro->id), "%zu", sistema->contador_ids);

	hash_insertar(sistema->hospitales, centro->id, centro, NULL);
	printf(VERDE "\nHospital cargado con éxito.\n" AMARILLO "ID: " NARANJA
		     "%s" AMARILLO " Nombre: " NARANJA "%s\n" RESET,
	       centro->id, centro->nombre_hospital);

	return true;
}

/**
 *  Muestra el estado de los hospitales. Retorna true. En caso de error, false.
*/
bool estado_hospitales(const char *clave, void *hospital, void *aux)
{
	if (!hospital)
		return false;

	centro_t *centro = hospital;
	char estado[] = "INACTIVO";
	if (centro->activo)
		strcpy(estado, "ACTIVO");

	printf(AMARILLO "\nNombre hospital: " NARANJA "%s" AMARILLO
			" ID: " NARANJA "%s" AMARILLO " Estado: " NARANJA "%s",
	       centro->nombre_hospital, centro->id, estado);
	return true;
}

/**
 * Muestra todos los hospitales y su estado
 * Retorna true.
*/
bool ejecutar_estado(void *menu, void *centros, void *buffer)
{
	if (!centros) {
		printf(ROJO
		       "\nHubo un error al mostrar el estado de los hospitales.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	sistema_hospitales_t *sistema = centros;
	if (hash_cantidad(sistema->hospitales) == 0) {
		printf(ROJO "\nNo hay hospitales cargados.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	hash_con_cada_clave(sistema->hospitales, estado_hospitales, NULL);
	printf("\n");
	return true;
}

/**
 * Activa un hospital con la id que se ingresa.
 * Retorna true.
*/
bool ejecutar_activar(void *menu, void *centros, void *buffer)
{
	if (!centros) {
		printf(ROJO "\nHubo un error al activar un hospital.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}
	sistema_hospitales_t *sistema = centros;

	if (hash_cantidad(sistema->hospitales) == 0) {
		printf(ROJO "\nNo hay ningún hospital cargado para activar.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	char *id = leer_interaccion(buffer, "Ingrese la id del hospital: ");
	if (!id) {
		printf(ROJO "\nHubo un error cargar la id.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	for (int i = 0; id[i] != '\0'; i++) {
		if (!isdigit(id[i])) {
			printf(ROJO "\nLa id es incorrecta.\n");
			printf("Volviendo al menu principal...\n" RESET);
			return true;
		}
	}

	if (atoi(id) <= 0 || !hash_contiene(sistema->hospitales, id)) {
		printf(ROJO "\nNo existe un hospital con esa id.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	if (strcmp(sistema->id_hospital_activo, id) == 0) {
		printf(AMARILLO "\nEse hospital ya está activo.\n" RESET);
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	if (sistema->hospital_activo) {
		printf(AMARILLO
		       "\nYa había un hospital activo. Se va desactivar, para activar el nuevo.\n" RESET);
		printf("Volviendo al menu principal...\n" RESET);
		sistema->hospital_activo->activo = false;
	}

	snprintf(sistema->id_hospital_activo,
		 sizeof(sistema->id_hospital_activo), "%s", id);
	sistema->hospital_activo =
		hash_obtener(sistema->hospitales, sistema->id_hospital_activo);

	if (!sistema->hospital_activo) {
		printf(ROJO "\nNo existe un hospital con esa id.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}
	sistema->hospital_activo->activo = true;

	printf(VERDE "\nHospital activado con éxito.\n" AMARILLO "ID: " NARANJA
		     "%s" AMARILLO " Nombre: " NARANJA "%s\n" RESET,
	       sistema->id_hospital_activo,
	       sistema->hospital_activo->nombre_hospital);

	return true;
}

/**
 * Muestra por pantalla los pokemones con su información.
 * En caso de error retorna false. Sino, true.
*/
bool imprimir_pokemones(pokemon_t *poke, void *aux)
{
	if (!poke)
		return false;

	bool completo = *(bool *)aux;

	if (completo)
		printf(AMARILLO "Nombre: " NARANJA "%s" AMARILLO " ID: " NARANJA
				"%zu" AMARILLO " Salud: " NARANJA "%zu" AMARILLO
				" Nombre entrenador: " NARANJA "%s\n",
		       pokemon_nombre(poke), pokemon_id(poke),
		       pokemon_salud(poke), pokemon_entrenador(poke));
	else
		printf(AMARILLO "Nombre: " NARANJA "%s\n",
		       pokemon_nombre(poke));

	return true;
}

/**
 * Muestra los nombres de los pokemones del hospital.
 * Retorna true.
*/
bool ejecutar_mostrar(void *menu, void *centros, void *buffer)
{
	if (!centros) {
		printf(ROJO "\nHubo un error al mostrar el hospital.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	sistema_hospitales_t *sistema = centros;
	if (!sistema->hospital_activo || !sistema ||
	    !sistema->hospital_activo->hospital) {
		printf(ROJO "\nNo hay ningun hospital activo para mostrar.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}
	bool completo = false;
	printf("\n");
	hospital_a_cada_pokemon(sistema->hospital_activo->hospital,
				imprimir_pokemones, &completo);

	return true;
}

/**
 * Muestra todos los datos de los pokemones del hospital
 * Retorna true.
*/
bool ejecutar_listar(void *menu, void *centros, void *buffer)
{
	if (!centros) {
		printf(ROJO "\nHubo un error al listar el hospital.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	sistema_hospitales_t *sistema = centros;
	if (!sistema->hospital_activo || !sistema ||
	    !sistema->hospital_activo->hospital) {
		printf(ROJO "\nNo hay ningun hospital activo para listar.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	bool completo = true;
	printf("\n");
	hospital_a_cada_pokemon(sistema->hospital_activo->hospital,
				imprimir_pokemones, &completo);

	return true;
}

/**
 * Libera toda la memoria ocupada por el hospital activo.
 * Retorna true.
*/
bool ejecutar_destruir(void *menu, void *centros, void *buffer)
{
	if (!centros) {
		printf(ROJO "\nHubo un error al destruir el hospital.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	sistema_hospitales_t *sistema = centros;

	if (!sistema->hospital_activo) {
		printf(ROJO "\nNo hay ningun hospital activo para destruir.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	hash_quitar(sistema->hospitales, sistema->id_hospital_activo);

	printf(VERDE "\nHospital destruido con éxito.\n" AMARILLO
		     "Nombre: " NARANJA "%s" AMARILLO " ID: " NARANJA
		     "%s\n" RESET,
	       sistema->hospital_activo->nombre_hospital,
	       sistema->id_hospital_activo);

	centro_destruir_todo(sistema->id_hospital_activo,
			     sistema->hospital_activo, NULL);
	sistema->hospital_activo = NULL;
	snprintf(sistema->id_hospital_activo,
		 sizeof(sistema->id_hospital_activo), "%i", 0);

	return true;
}

/**
 * Agrega los comando necesarios para el programa. En caso de error, retorna false.
 * Sino true.
*/
bool agregar_comandos(menu_t *menu)
{
	comando_t *salir = comando_crear(
		"S", "Salir del programa",
		"Sale del programa, liberando toda la memoria ocupada por el mismo",
		ejecutar_salir);

	if (!menu_agregar_comando(menu, salir) ||
	    !comando_agregar_alias(salir, "SALIR") ||
	    !comando_agregar_alias(salir, "salir") ||
	    !comando_agregar_alias(salir, "s") ||
	    !comando_agregar_alias(salir, "exit"))
		return false;

	comando_t *ayuda = comando_crear(
		"H", "Mostrar menu de ayuda",
		"Muestra el menu de ayuda, con todos los comandos y explicaciones ",
		ejecutar_ayuda);

	if (!menu_agregar_comando(menu, ayuda) ||
	    !comando_agregar_alias(ayuda, "help") ||
	    !comando_agregar_alias(ayuda, "h") ||
	    !comando_agregar_alias(ayuda, "ayuda") ||
	    !comando_agregar_alias(ayuda, "AYUDA"))
		return false;

	comando_t *cargar = comando_crear(
		"C", "Cargar hospital",
		"Pide un nombre de archivo e intenta cargarlo creando un hospital",
		ejecutar_cargar);

	if (!menu_agregar_comando(menu, cargar) ||
	    !comando_agregar_alias(cargar, "c") ||
	    !comando_agregar_alias(cargar, "cargar") ||
	    !comando_agregar_alias(cargar, "CARGAR"))
		return false;

	comando_t *estado = comando_crear(
		"E", "Mostrar estado de los hospitales",
		"Muestra un listado con los hospitales cargados (y el activo, si hay alguno)",
		ejecutar_estado);

	if (!menu_agregar_comando(menu, estado) ||
	    !comando_agregar_alias(estado, "e") ||
	    !comando_agregar_alias(estado, "estado") ||
	    !comando_agregar_alias(estado, "ESTADO"))
		return false;

	comando_t *activar = comando_crear(
		"A", "Activar hospital",
		"Pide un número de identificación y activa el hospital",
		ejecutar_activar);

	if (!menu_agregar_comando(menu, activar) ||
	    !comando_agregar_alias(activar, "a") ||
	    !comando_agregar_alias(activar, "activar") ||
	    !comando_agregar_alias(activar, "ACTIVAR"))
		return false;

	comando_t *mostrar = comando_crear(
		"M", "Mostrar hospital activo",
		"Muestra un listado con los nombres de todos los pokemones en el hospital activo",
		ejecutar_mostrar);

	if (!menu_agregar_comando(menu, mostrar) ||
	    !comando_agregar_alias(mostrar, "m") ||
	    !comando_agregar_alias(mostrar, "mostrar") ||
	    !comando_agregar_alias(mostrar, "MOSTRAR"))
		return false;

	comando_t *listar = comando_crear(
		"L", "Listar hospital activo",
		"Muestra un listado detallado de todos los pokemones en el hospital activo",
		ejecutar_listar);
	if (!menu_agregar_comando(menu, listar) ||
	    !comando_agregar_alias(listar, "l") ||
	    !comando_agregar_alias(listar, "listar") ||
	    !comando_agregar_alias(listar, "LISTAR"))
		return false;

	comando_t *destruir = comando_crear(
		"D", "Destruir hospital activo",
		"Destruye el hospital activo, liberando toda la memoria ocupada por el mismo",
		ejecutar_destruir);

	if (!menu_agregar_comando(menu, destruir) ||
	    !comando_agregar_alias(destruir, "d") ||
	    !comando_agregar_alias(destruir, "destruir") ||
	    !comando_agregar_alias(destruir, "DESTRUIR"))
		return false;

	return true;
}

int main()
{
	bool seguir = true;
	char *comando;
	char buffer[MAX_BUFFER];
	buffer[0] = 0;

	sistema_hospitales_t *sistema = calloc(1, sizeof(sistema_hospitales_t));
	if (!sistema)
		return -1;

	sistema->hospitales = hash_crear(MIN_HOSPITALES);
	if (!sistema->hospitales) {
		free(sistema);
		return -1;
	}

	menu_t *menu = menu_crear("HOSPITAL POKEMONES");
	if (!menu) {
		free(sistema->hospitales);
		free(sistema);
		return -1;
	}

	if (!agregar_comandos(menu)) {
		free(sistema->hospitales);
		free(sistema);
		free(menu);
		return -1;
	}

	while (seguir) {
		menu_mostrar(menu);
		comando = leer_interaccion(buffer, "Ingrese una opción: ");
		seguir = menu_ejecutar_comando(menu, comando, sistema, buffer);
	}
	return 0;
}
