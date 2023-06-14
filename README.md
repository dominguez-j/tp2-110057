<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP2

## Repositorio de Jonathan Dominguez - 110057 - chondominguez55@gmail.com

- Para compilar:

```bash
gcc -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2 -g src/*.c pruebas_chanutron.c -o pruebas_chanutron
```

- Para ejecutar:

```bash
./pruebas_chanutron
```

- Para ejecutar con valgrind:
```bash
valgrind --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes ./pruebas_chanutron
```
---
##  Funcionamiento



## Respuestas a las preguntas teóricas

***¿Que TDA conviene utilizar?  ¿Que problema habría la reemplazar dicha estructura?  ¿Habría algún problema con las pruebas?***

En mi opinión, me parece mucho más útil el uso de un ABB. Ya que puedo armar un árbol de pokemones, donde este organizado en base a la salud de los mismos. Como es un hospital lo que busco mayormente es el pokemon con menor salud. Aparte porque puedo recorrer el árbol por INORDEN  y voy a tener los pokemones que tengan más prioridad en tema de salud.

El problema de reemplazar dicha estructura, implica que tenga que modificar algunas funciones para el óptimo uso del ABB. Por ejemplo en vez de usar una función para ordenar un vector, puedo usar como mencioné el recorrido INORDEN que me permite tener el vector ordenado por salud y con menor complejidad.

Si no modifico ninguna función evidentemente voy a tener problemas con las pruebas. Ahora, si las funciones las adapto al uso de un ABB lo más seguro es que no haya ningún problema con las mismas. 

</br>Tabla de complejidades entre ABB y Lista enlazada

|          Operación           | Lista Enlazada | ABB      |
|------------------------------|----------------|----------|
| hospital_crear_desde_archivo | O(n)           | O(log n) |
| hospital_a_cada_pokemon      | O(n)           | O(log n) |
| hospital_aceptar_emergencias | O(n)           | O(log n) |
| hospital_obtener_pokemon     | O(n)           | O(log n) |
| hospital_destruir            | O(n)           | O(log n) |


