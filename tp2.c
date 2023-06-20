#include "src/tp1.h"
#include "src/menu.h"
#include "src/lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MIN_ALIAS 5
#define AMARILLO "\x1b[33;1m"
#define VERDE "\x1b[32;1m"
#define ROJO "\x1b[31;1m"
#define NARANJA "\x1b[38;5;208m"
#define RESET "\033[0m"

typedef struct buffer {
	char buffer[100];
	int tam;
} buffer_t;

typedef struct centro {
	hospital_t *hospital;
	char *nombre_hospital;
	bool activo;
} centro_t;

typedef struct sistema_hospitales {
	lista_t *hospitales;
	centro_t *hospital_activo;
	size_t id_hospital_activo;
} sistema_hospitales_t;

char *leer_interaccion(buffer_t *buffer, char *texto)
{
	printf(AMARILLO "%s \n" RESET, texto);
	char *respuesta = fgets(buffer->buffer, buffer->tam, stdin);
	if (respuesta)
		respuesta[strlen(respuesta) - 1] = 0;

	return respuesta;
}

/**
 * Libera toda la memoria ocupada por un centro.
 * Si no se pudo liberar, retorna false. Caso contrario true.
*/
bool centro_destruir_todo(void *elemento, void *aux)
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
bool ejecutar_salir(void *menu, void *centros, void *aux2)
{
	sistema_hospitales_t *sistema = centros;
	lista_con_cada_elemento(sistema->hospitales, centro_destruir_todo,
				NULL);
	lista_destruir(sistema->hospitales);
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
	if (!centros || !buffer) {
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
	lista_insertar(sistema->hospitales, centro);
	printf(VERDE "\nHospital cargado con éxito.\n" AMARILLO "ID: " NARANJA
		     "%zu" AMARILLO " Nombre: " NARANJA "%s\n" RESET,
	       lista_tamanio(sistema->hospitales), centro->nombre_hospital);

	return true;
}

/**
 *  Muestra el estado de los hospitales. Retorna true. En caso de error, false.
*/
bool estado_hospitales(void *hospital, void *aux)
{
	if (!hospital || !aux)
		return false;

	centro_t *centro = hospital;
	size_t *id = aux;
	char estado[] = "INACTIVO";
	if (centro->activo)
		strcpy(estado, "ACTIVO");

	printf(AMARILLO "\nNombre hospital: " NARANJA "%s" AMARILLO
			" ID: " NARANJA "%zu" AMARILLO " Estado: " NARANJA "%s",
	       centro->nombre_hospital, *id, estado);
	(*id)++;
	return true;
}

/**
 * Muestra todos los hospitales y su estado
 * Retorna true.
*/
bool ejecutar_estado(void *menu, void *centros, void *aux2)
{
	if (!centros) {
		printf(ROJO
		       "\nHubo un error al mostrar el estado de los hospitales.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	sistema_hospitales_t *sistema = centros;
	if (lista_tamanio(sistema->hospitales) == 0) {
		printf(ROJO "\nNo hay hospitales cargados.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}
	size_t id = 1;
	lista_con_cada_elemento(sistema->hospitales, estado_hospitales, &id);

	return true;
}

/**
 * Activa un hospital con la id que se ingresa.
 * Retorna true.
*/
bool ejecutar_activar(void *menu, void *centros, void *buffer)
{
	if (!centros || !buffer) {
		printf(ROJO "\nHubo un error al activar un hospital.\n");
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

	sistema_hospitales_t *sistema = centros;
	size_t id_hospital = (size_t)atoi(id);
	if (id_hospital <= 0 ||
	    id_hospital > lista_tamanio(sistema->hospitales)) {
		printf(ROJO
		       "\nLa id es incorrecta o no existe un hospital con esa id.\n");
		printf("Volviendo al menu principal...\n" RESET);
		return true;
	}

	if (sistema->id_hospital_activo == id_hospital) {
		printf(AMARILLO "\nEse hospital ya está activo.\n" RESET);
		return true;
	}

	if (sistema->hospital_activo) {
		printf(AMARILLO
		       "\nYa había un hospital activo. Se va desactivar, para activar el nuevo.\n" RESET);
		sistema->hospital_activo->activo = false;
	}

	sistema->id_hospital_activo = id_hospital;
	sistema->hospital_activo = lista_elemento_en_posicion(
		sistema->hospitales, sistema->id_hospital_activo - 1);
	sistema->hospital_activo->activo = true;

	printf(VERDE "\nHospital activado con éxito.\n" AMARILLO "ID: " NARANJA
		     "%zu" AMARILLO " Nombre: " NARANJA "%s\n" RESET,
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
bool ejecutar_mostrar(void *menu, void *centros, void *aux2)
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
	hospital_a_cada_pokemon(sistema->hospital_activo->hospital,
				imprimir_pokemones, &completo);

	return true;
}

/**
 * Muestra todos los datos de los pokemones del hospital
 * Retorna true.
*/
bool ejecutar_listar(void *menu, void *centros, void *aux2)
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
	hospital_a_cada_pokemon(sistema->hospital_activo->hospital,
				imprimir_pokemones, &completo);

	return true;
}

/**
 * Libera toda la memoria ocupada por el hospital activo.
 * Retorna true.
*/
bool ejecutar_destruir(void *menu, void *centros, void *aux2)
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

	lista_quitar_de_posicion(sistema->hospitales,
				 sistema->id_hospital_activo - 1);

	printf(VERDE "\nHospital destruido con éxito.\n" AMARILLO
		     "Nombre: " NARANJA "%s" AMARILLO " ID: " NARANJA
		     "%zu\n" RESET,
	       sistema->hospital_activo->nombre_hospital,
	       sistema->id_hospital_activo);

	centro_destruir_todo(sistema->hospital_activo, NULL);
	sistema->hospital_activo = NULL;
	sistema->id_hospital_activo = 0;

	printf(VERDE "\nSe actualizaron las ID de los hospitales:" RESET);
	ejecutar_estado(menu, centros, aux2);
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
			"S", "Salir del programa",
			"Sale del programa, liberando toda la memoria ocupada por el mismo",
			ejecutar_salir, alias_salir));

	hash_t *alias_ayuda = hash_crear(MIN_ALIAS);
	hash_insertar(alias_ayuda, "h", "h", NULL);
	hash_insertar(alias_ayuda, "help", NULL, NULL);
	hash_insertar(alias_ayuda, "ayuda", NULL, NULL);
	hash_insertar(alias_ayuda, "AYUDA", NULL, NULL);
	menu_agregar_comando(
		menu,
		comando_crear(
			"H", "Mostrar menu de ayuda",
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
			"E", "Mostrar estado de los hospitales",
			"Muestra un listado con los hospitales cargados (y el activo, si hay alguno)",
			ejecutar_estado, alias_estado));

	hash_t *alias_activar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_activar, "a", NULL, NULL);
	hash_insertar(alias_activar, "activar", NULL, NULL);
	hash_insertar(alias_activar, "ACTIVAR", NULL, NULL);
	menu_agregar_comando(
		menu,
		comando_crear(
			"A", "Activar hospital",
			"Pide un número de identificación y activa el hospital",
			ejecutar_activar, alias_activar));

	hash_t *alias_mostrar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_mostrar, "m", NULL, NULL);
	hash_insertar(alias_mostrar, "mostrar", NULL, NULL);
	hash_insertar(alias_mostrar, "MOSTRAR", NULL, NULL);
	menu_agregar_comando(
		menu,
		comando_crear(
			"M", "Mostrar hospital activo",
			"Muestra un listado con los nombres de todos los pokemones en el hospital activo",
			ejecutar_mostrar, alias_mostrar));

	hash_t *alias_listar = hash_crear(MIN_ALIAS);
	hash_insertar(alias_listar, "l", NULL, NULL);
	hash_insertar(alias_listar, "listar", NULL, NULL);
	hash_insertar(alias_listar, "LISTAR", NULL, NULL);
	menu_agregar_comando(
		menu,
		comando_crear(
			"L", "Listar hospital activo",
			"Muestra un listado detallado de todos los pokemones en el hospital activo",
			ejecutar_listar, alias_listar));

	hash_t *alias_destruir = hash_crear(MIN_ALIAS);
	hash_insertar(alias_destruir, "d", NULL, NULL);
	hash_insertar(alias_destruir, "destruir", NULL, NULL);
	hash_insertar(alias_destruir, "DESTRUIR", NULL, NULL);
	menu_agregar_comando(
		menu,
		comando_crear(
			"D", "Destruir hospital activo",
			"Destruye el hospital activo, liberando toda la memoria ocupada por el mismo",
			ejecutar_destruir, alias_destruir));
}

int main()
{
	buffer_t buffer = { .tam = 100, .buffer[0] = 0 };
	bool seguir = true;
	char *comando;

	sistema_hospitales_t *sistema = calloc(1, sizeof(sistema_hospitales_t));
	if (!sistema)
		return -1;

	sistema->hospitales = lista_crear();
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

	agregar_comandos(menu);
	while (seguir) {
		menu_mostrar(menu);
		comando = leer_interaccion(&buffer, "Ingrese una opción: ");
		seguir = menu_ejecutar_comando(menu, comando, sistema, &buffer);
	}
	return 0;
}
