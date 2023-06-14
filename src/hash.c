#include <string.h>
#include <stdlib.h>

#include "hash.h"

#define CARGA_MAXIMA 0.7
#define CAPACIDAD_MINIMA 3

typedef struct par {
	void *clave;
	void *elemento;
	struct par *siguiente;
} par_t;

struct hash {
	par_t **vector;
	size_t capacidad;
	size_t cantidad;
};

/**
 * Se crea una copia de la clave y se la devuelve. Caso contrario, se retorna NULL.
*/
char *copiar_clave(const char *clave)
{
	char *copia_clave = malloc(strlen(clave) + 1);
	if (!copia_clave)
		return NULL;
	return strcpy(copia_clave, clave);
}

/**
 * Crea el par reservando la memoria necesaria para el.
 * Recibe una clave y un elemento. Y guarda una copia de la clave.
*/
par_t *par_crear(const char *clave, void *elemento)
{
	par_t *par = calloc(1, sizeof(par_t));
	if (!par)
		return NULL;

	par->clave = copiar_clave(clave);
	if (!par->clave) {
		free(par);
		return NULL;
	}
	par->elemento = elemento;
	return par;
}

/**
 * Libera toda la memoria reservada por el par.
 */
void par_destruir(par_t *par)
{
	free(par->clave);
	free(par);
}

hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;

	if (capacidad < CAPACIDAD_MINIMA)
		capacidad = CAPACIDAD_MINIMA;

	hash->vector = calloc(capacidad, sizeof(par_t *));
	if (!hash->vector) {
		free(hash);
		return NULL;
	}

	hash->capacidad = capacidad;
	return hash;
}

/**
 * Devuelve la posición.
*/
size_t obtener_posicion(size_t valor_hash, size_t capacidad)
{
	return valor_hash % capacidad;
}

/**
 * Devuelve el valor hash de la clave pasada por parámetro.
*/
size_t hashear(const char *clave)
{
	int valor_hash = 5381;
	int c = 0;

	while ((c = *(clave++)))
		valor_hash = ((valor_hash << 5) + valor_hash) + c;

	return (size_t)valor_hash;
}

/**
 * Inserta el par en la posicion del vector o en el final de la lista enlazada de esa posicion.
 * Si hay clave repetida, actualizo el valor del elemento con el pasado por parametro. Y anterior guarda el viejo elemento.
*/
hash_t *hash_insertar_par(hash_t *hash, const char *clave, void *elemento,
			  void **anterior)
{
	size_t posicion = obtener_posicion(hashear(clave), hash->capacidad);
	par_t **actual = &(hash->vector[posicion]);
	while (*actual) {
		if (strcmp((*actual)->clave, clave) == 0) {
			if (anterior)
				*anterior = (*actual)->elemento;
			(*actual)->elemento = elemento;
			return hash;
		}
		actual = &((*actual)->siguiente);
	}

	par_t *par = par_crear(clave, elemento);
	if (!par)
		return NULL;
	if (anterior)
		*anterior = NULL;

	*actual = par;
	hash->cantidad++;
	return hash;
}

/**
 * Crea un nuevo hash con el doble de capacidad que el anterior.
 * Se le insertan todos los pares del otro hash.
 * Se intercambian los hash y se libera el nuevo creado.
*/
hash_t *rehashear(hash_t *hash)
{
	hash_t *nuevo_hash = hash_crear(hash->capacidad * 2);

	if (!nuevo_hash)
		return NULL;

	size_t cant = 0;
	for (size_t i = 0; i < hash->capacidad && cant < hash_cantidad(hash);
	     i++) {
		par_t *actual = hash->vector[i];
		while (actual) {
			hash_insertar(nuevo_hash, actual->clave,
				      actual->elemento, NULL);
			actual = actual->siguiente;
			cant++;
		}
	}

	hash_t aux = *hash;
	*hash = *nuevo_hash;
	*nuevo_hash = aux;

	hash_destruir(nuevo_hash);
	return hash;
}

/*
*Devuelve el factor de carga.
*/
float factor_carga(size_t cantidad, size_t capacidad)
{
	return (float)cantidad / (float)capacidad;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	float carga = factor_carga(hash->cantidad, hash->capacidad);

	if (carga > CARGA_MAXIMA) {
		hash_t *nuevo_hash = rehashear(hash);
		if (!nuevo_hash)
			return NULL;
		hash = nuevo_hash;
	}
	return hash_insertar_par(hash, clave, elemento, anterior);
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = obtener_posicion(hashear(clave), hash->capacidad);

	par_t *actual = hash->vector[posicion];
	par_t *anterior = NULL;
	while (actual) {
		if (strcmp(actual->clave, clave) == 0) {
			if (anterior)
				anterior->siguiente = actual->siguiente;
			else
				hash->vector[posicion] = actual->siguiente;

			void *elemento_eliminado = actual->elemento;
			hash->cantidad--;
			par_destruir(actual);
			return elemento_eliminado;
		}
		anterior = actual;
		actual = actual->siguiente;
	}
	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = hashear(clave) % hash->capacidad;
	par_t *actual = hash->vector[posicion];
	while (actual) {
		if (strcmp(actual->clave, clave) == 0)
			return actual->elemento;
		actual = actual->siguiente;
	}
	return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;

	size_t posicion = obtener_posicion(hashear(clave), hash->capacidad);
	par_t *actual = hash->vector[posicion];
	while (actual) {
		if (strcmp(actual->clave, clave) == 0)
			return true;
		actual = actual->siguiente;
	}
	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	return hash ? hash->cantidad : 0;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (int i = 0; i < hash->capacidad; i++) {
		par_t *actual = hash->vector[i];
		while (actual) {
			par_t *siguiente = actual->siguiente;
			if (destructor)
				destructor(actual->elemento);
			par_destruir(actual);
			actual = siguiente;
		}
	}
	free(hash->vector);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *elemento,
				     void *aux),
			   void *aux)
{
	size_t n = 0;
	if (!hash || !f)
		return n;

	par_t *actual = NULL;
	bool seguir = true;

	for (size_t i = 0; i < hash->capacidad && n < hash_cantidad(hash);
	     i++) {
		actual = hash->vector[i];
		while (actual && seguir) {
			seguir = f(actual->clave, actual->elemento, aux);
			actual = actual->siguiente;
			n++;
		}
	}

	return n;
}
