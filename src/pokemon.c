#include "pokemon.h"
#include <string.h>
#include <stdio.h>
#include "pokemon_privado.h"
#define LECTURA "%zu,%[^,],%zu,%[^\n]\n"
const int CANT_ARGS = 4;

pokemon_t *pokemon_crear_desde_string(const char *string)
{
	if (!string)
		return NULL;

	pokemon_t *poke_aux = malloc(sizeof(pokemon_t));

	if (!poke_aux)
		return NULL;

	int leidos = sscanf(string, LECTURA, &(poke_aux->id), poke_aux->nombre,
			    &(poke_aux->salud), poke_aux->nombre_entrenador);

	if (leidos != CANT_ARGS) {
		pokemon_destruir(poke_aux);
		return NULL;
	}

	return poke_aux;
}

pokemon_t *pokemon_copiar(pokemon_t *poke)
{
	if (!poke)
		return NULL;

	pokemon_t *poke_copia = malloc(sizeof(pokemon_t));

	if (!poke_copia)
		return NULL;

	*poke_copia = *poke;

	return poke_copia;
}

bool pokemon_son_iguales(pokemon_t *pokemon1, pokemon_t *pokemon2)
{
	if (!pokemon1 || !pokemon2)
		return false;

	return pokemon_id(pokemon1) == pokemon_id(pokemon2) &&
	       pokemon_salud(pokemon1) == pokemon_salud(pokemon2) &&
	       strcmp(pokemon1->nombre_entrenador,
		      pokemon2->nombre_entrenador) == 0 &&
	       strcmp(pokemon1->nombre, pokemon2->nombre) == 0;
}

char *pokemon_nombre(pokemon_t *pokemon)
{
	return !pokemon ? 0 : pokemon->nombre;
}

char *pokemon_entrenador(pokemon_t *pokemon)
{
	return !pokemon ? 0 : pokemon->nombre_entrenador;
}

size_t pokemon_salud(pokemon_t *pokemon)
{
	return !pokemon ? 0 : pokemon->salud;
}

size_t pokemon_id(pokemon_t *pokemon)
{
	return !pokemon ? 0 : pokemon->id;
}

void pokemon_destruir(pokemon_t *pkm)
{
	free(pkm);
}
