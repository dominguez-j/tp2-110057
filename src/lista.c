#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

#define POSICION_INICIAL 0
const size_t CERO = 0;
const size_t UN_ELEMENTO = 1;

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	size_t cantidad;
	nodo_t *inicio;
	nodo_t *fin;
};

struct lista_iterador {
	nodo_t *actual;
};

/**
 * Devuelve true si el iterador no es nulo y si el nodo actual tampoco lo es.
 * Caso contrario, retorna false.
 */
bool lista_iterador_valido(lista_iterador_t *iterador)
{
	return iterador && iterador->actual;
}

/**
 * Devuelve true si la lista no es nula y si no esta vacia.
 * Caso contrario, retorna false.
 */
bool lista_valida(lista_t *lista)
{
	return lista && !lista_vacia(lista);
}

/**
 * Libera la memoria reservada por el nodo.
 */
void nodo_destruir(nodo_t *nodo)
{
	free(nodo);
}

lista_t *lista_crear()
{
	return calloc(1, sizeof(lista_t));
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nodo_nuevo = malloc(sizeof(nodo_t));

	if (!nodo_nuevo)
		return NULL;

	if (lista_vacia(lista))
		lista->inicio = nodo_nuevo;
	else
		lista->fin->siguiente = nodo_nuevo;

	nodo_nuevo->elemento = elemento;
	nodo_nuevo->siguiente = NULL;
	lista->fin = nodo_nuevo;
	lista->cantidad++;

	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista)
		return NULL;

	if (lista_vacia(lista) || posicion >= lista_tamanio(lista))
		return lista_insertar(lista, elemento);

	nodo_t *nodo_nuevo = malloc(sizeof(nodo_t));

	if (!nodo_nuevo)
		return NULL;

	if (posicion == POSICION_INICIAL) {
		nodo_nuevo->siguiente = lista->inicio;
		lista->inicio = nodo_nuevo;

	} else {
		nodo_t *actual = lista->inicio;
		size_t posicion_anterior = posicion - 1;

		for (int i = 0; i < posicion_anterior; i++)
			actual = actual->siguiente;

		nodo_nuevo->siguiente = actual->siguiente;
		actual->siguiente = nodo_nuevo;
	}

	nodo_nuevo->elemento = elemento;
	lista->cantidad++;

	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista_valida(lista))
		return NULL;

	nodo_t *actual = lista->inicio;

	if (lista_tamanio(lista) > UN_ELEMENTO) {
		nodo_t *anterior = NULL;

		while (actual->siguiente) {
			anterior = actual;
			actual = actual->siguiente;
		}

		lista->fin = anterior;
		anterior->siguiente = NULL;
	}

	void *elemento_eliminado = actual->elemento;
	lista->cantidad--;
	nodo_destruir(actual);

	return elemento_eliminado;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista_valida(lista))
		return NULL;

	size_t ultima_pos = lista_tamanio(lista) - 1;

	if (posicion >= ultima_pos || lista_tamanio(lista) == UN_ELEMENTO)
		return lista_quitar(lista);

	nodo_t *actual = lista->inicio;
	nodo_t *nodo_a_quitar = NULL;

	if (posicion == POSICION_INICIAL) {
		nodo_a_quitar = lista->inicio;
		lista->inicio = nodo_a_quitar->siguiente;

	} else {
		size_t posicion_anterior = posicion - 1;
		for (int i = 0; i < posicion_anterior; i++)
			actual = actual->siguiente;

		nodo_a_quitar = actual->siguiente;
		actual->siguiente = nodo_a_quitar->siguiente;
	}

	void *elemento_eliminado = nodo_a_quitar->elemento;
	lista->cantidad--;
	nodo_destruir(nodo_a_quitar);

	return elemento_eliminado;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista_valida(lista) || posicion >= lista_tamanio(lista))
		return NULL;

	size_t ultima_pos = lista_tamanio(lista) - 1;

	if (posicion == ultima_pos)
		return lista->fin->elemento;

	nodo_t *actual = lista->inicio;

	for (int i = 0; i < posicion; i++)
		actual = actual->siguiente;

	return actual->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista_valida(lista) || !comparador)
		return NULL;

	nodo_t *actual = lista->inicio;
	void *elemento_buscado = NULL;

	while (actual && !elemento_buscado) {
		if (comparador(actual->elemento, contexto) == 0)
			elemento_buscado = actual->elemento;
		else
			actual = actual->siguiente;
	}

	return elemento_buscado;
}

void *lista_primero(lista_t *lista)
{
	return lista_valida(lista) ? lista->inicio->elemento : NULL;
}

void *lista_ultimo(lista_t *lista)
{
	return lista_valida(lista) ? lista->fin->elemento : NULL;
}

bool lista_vacia(lista_t *lista)
{
	return !lista || !lista->inicio ? true : lista_tamanio(lista) == CERO;
}

size_t lista_tamanio(lista_t *lista)
{
	return lista ? lista->cantidad : CERO;
}

void lista_destruir(lista_t *lista)
{
	lista_destruir_todo(lista, NULL);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;

	if (lista_vacia(lista)) {
		free(lista);
		return;
	}

	nodo_t *actual = lista->inicio;
	nodo_t *siguiente = NULL;

	while (actual) {
		siguiente = actual->siguiente;
		if (funcion)
			funcion(actual->elemento);
		nodo_destruir(actual);
		actual = siguiente;
	}

	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;

	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));

	if (!iterador)
		return NULL;

	iterador->actual = lista->inicio;

	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	return !lista_iterador_valido(iterador) ? false : true;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!lista_iterador_tiene_siguiente(iterador))
		return false;

	iterador->actual = iterador->actual->siguiente;

	return iterador->actual;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	return lista_iterador_valido(iterador) ? iterador->actual->elemento :
						 NULL;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista_valida(lista) || !funcion)
		return 0;

	nodo_t *actual = lista->inicio;
	size_t contador = 1;

	while (funcion(actual->elemento, contexto) &&
	       contador < lista_tamanio(lista)) {
		contador++;
		actual = actual->siguiente;
	}

	return contador;
}
