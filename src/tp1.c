#include "tp1.h"

#include "pokemon.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const int MAX_LINEA = 30;

struct _hospital_pkm_t {
	pokemon_t **pokemones;
	size_t cantidad_pokemon;
	size_t cantidad_entrenadores;
};

/**
 * Ordena el vector de pokemones por salud. De menor a mayor salud.
 */
void ordenar_pokemones_por_salud(pokemon_t **pokemones, size_t cantidad_pokemon)
{
	if (!pokemones || cantidad_pokemon == 0)
		return;

	pokemon_t *aux;

	for (size_t i = 1; i < cantidad_pokemon; i++) {
		for (size_t j = 0; j < cantidad_pokemon - i; j++) {
			if (pokemon_salud(pokemones[j]) >
			    pokemon_salud(pokemones[j + 1])) {
				aux = pokemones[j];
				pokemones[j] = pokemones[j + 1];
				pokemones[j + 1] = aux;
			}
		}
	}
}

hospital_t *hospital_crear_desde_archivo(const char *nombre_archivo)
{
	if (!nombre_archivo)
		return NULL;

	FILE *archivo_pokemones = fopen(nombre_archivo, "r");

	if (!archivo_pokemones)
		return NULL;

	char linea[MAX_LINEA];
	char *fila = fgets(linea, MAX_LINEA, archivo_pokemones);

	if (!fila) {
		fclose(archivo_pokemones);
		return NULL;
	}

	hospital_t *hospital = malloc(sizeof(hospital_t));

	if (!hospital) {
		fclose(archivo_pokemones);
		return NULL;
	}

	size_t tamanio = 0;
	pokemon_t **poke_aux = NULL;

	while (fila != NULL) {
		poke_aux =
			realloc(poke_aux, (tamanio + 1) * sizeof(pokemon_t *));
		if (!poke_aux) {
			hospital_destruir(hospital);
			fclose(archivo_pokemones);
			return NULL;
		}
		poke_aux[tamanio] = pokemon_crear_desde_string(linea);
		tamanio++;
		fila = fgets(linea, MAX_LINEA, archivo_pokemones);
	}

	fclose(archivo_pokemones);

	hospital->cantidad_entrenadores = tamanio;
	hospital->cantidad_pokemon = tamanio;
	hospital->pokemones = poke_aux;

	ordenar_pokemones_por_salud(hospital->pokemones,
				    hospital->cantidad_pokemon);

	return hospital;
}

size_t hospital_cantidad_pokemones(hospital_t *hospital)
{
	return !hospital ? 0 : hospital->cantidad_pokemon;
}

size_t hospital_a_cada_pokemon(hospital_t *hospital,
			       bool (*funcion)(pokemon_t *p, void *aux),
			       void *aux)
{
	if (!funcion || !hospital)
		return 0;

	size_t i = 0;
	bool seguir = true;

	while (seguir && i < hospital->cantidad_pokemon) {
		seguir = funcion(hospital->pokemones[i], aux);
		i++;
	}

	return i;
}

int hospital_aceptar_emergencias(hospital_t *hospital,
				 pokemon_t **pokemones_ambulancia,
				 size_t cant_pokes_ambulancia)
{
	if (!hospital || !pokemones_ambulancia)
		return ERROR;

	if (cant_pokes_ambulancia == 0)
		return EXITO;

	size_t total = cant_pokes_ambulancia + hospital->cantidad_pokemon;

	pokemon_t **poke_aux =
		realloc(hospital->pokemones, total * sizeof(pokemon_t *));

	if (!poke_aux)
		return ERROR;

	hospital->pokemones = poke_aux;

	size_t j = 0;
	for (size_t i = hospital->cantidad_pokemon; i < total; i++) {
		hospital->pokemones[i] = pokemones_ambulancia[j];
		j++;
	}

	hospital->cantidad_pokemon += cant_pokes_ambulancia;

	ordenar_pokemones_por_salud(hospital->pokemones,
				    hospital->cantidad_pokemon);

	return EXITO;
}

pokemon_t *hospital_obtener_pokemon(hospital_t *hospital, size_t prioridad)
{
	if (!hospital || prioridad > hospital->cantidad_pokemon)
		return NULL;

	return hospital->pokemones[prioridad];
}

void hospital_destruir(hospital_t *hospital)
{
	for (size_t i = 0; i < hospital->cantidad_pokemon; i++)
		pokemon_destruir(hospital->pokemones[i]);

	free(hospital->pokemones);
	free(hospital);
}
