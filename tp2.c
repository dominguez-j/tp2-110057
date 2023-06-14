#include "src/tp1.h"
#include "src/menu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *leer_interaccion(char *buffer, size_t tam)
{
        printf("Ingrese una opción: \n");
	char *respuesta = fgets(buffer, tam, stdin);
	if (respuesta)
		respuesta[strlen(respuesta) - 1] = 0;

	return respuesta;
}

bool ejecutar_salir(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_ayuda(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_cargar(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_estado(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_activar(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_mostrar(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_listar(void *aux, void *aux2)
{
        return true;
}

bool ejecutar_destruir(void *aux, void *aux2)
{
        return true;
}

void agregar_comandos(menu_t *menu)
{
        menu = menu_agregar_comando(menu, comando_crear("S", "Sale del programa", ejecutar_salir, false));
        menu = menu_agregar_comando(menu, comando_crear("SALIR", "Sale del programa", ejecutar_salir, true));
        menu = menu_agregar_comando(menu, comando_crear("s", "Sale del programa", ejecutar_salir, true));
        menu = menu_agregar_comando(menu, comando_crear("salir", "Sale del programa", ejecutar_salir, true));
        menu = menu_agregar_comando(menu, comando_crear("exit", "Sale del programa", ejecutar_salir, true));

        menu = menu_agregar_comando(menu, comando_crear("H", "Muestra el menu de ayuda", ejecutar_ayuda, false));
        menu = menu_agregar_comando(menu, comando_crear("h", "Muestra el menu de ayuda", ejecutar_ayuda, true));
        menu = menu_agregar_comando(menu, comando_crear("help", "Muestra el menu de ayuda", ejecutar_ayuda, true));
        menu = menu_agregar_comando(menu, comando_crear("ayuda", "Muestra el menu de ayuda", ejecutar_ayuda, true));
        menu = menu_agregar_comando(menu, comando_crear("AYUDA", "Muestra el menu de ayuda", ejecutar_ayuda, true));
        menu = menu_agregar_comando(menu, comando_crear("HELP", "Muestra el menu de ayuda", ejecutar_ayuda, true));

        menu = menu_agregar_comando(menu, comando_crear("C", "Cargar hospital", ejecutar_cargar, false));
        menu = menu_agregar_comando(menu, comando_crear("c", "Cargar hospital", ejecutar_cargar, true));
        menu = menu_agregar_comando(menu, comando_crear("cargar", "Cargar hospital", ejecutar_cargar, true));
        menu = menu_agregar_comando(menu, comando_crear("CARGAR", "Cargar hospital", ejecutar_cargar, true));

        menu = menu_agregar_comando(menu, comando_crear("E", "Muestra el estado de los hospitales", ejecutar_estado, false));
        menu = menu_agregar_comando(menu, comando_crear("e", "Muestra el estado de los hospitales", ejecutar_estado, true));
        menu = menu_agregar_comando(menu, comando_crear("estado", "Muestra el estado de los hospitales", ejecutar_estado, true));
        menu = menu_agregar_comando(menu, comando_crear("ESTADO", "Muestra el estado de los hospitales", ejecutar_estado, true));

        menu = menu_agregar_comando(menu, comando_crear("A", "Activa un hospital", ejecutar_activar, false));
        menu = menu_agregar_comando(menu, comando_crear("a", "Activa un hospital", ejecutar_activar, true));
        menu = menu_agregar_comando(menu, comando_crear("activar", "Activa un hospital", ejecutar_activar, true));
        menu = menu_agregar_comando(menu, comando_crear("ACTIVAR", "Activa un hospital", ejecutar_activar, true));

        menu = menu_agregar_comando(menu, comando_crear("M", "Muestra los hospitales", ejecutar_mostrar, false));
        menu = menu_agregar_comando(menu, comando_crear("m", "Muestra los hospitales", ejecutar_mostrar, true));
        menu = menu_agregar_comando(menu, comando_crear("mostrar", "Muestra los hospitales", ejecutar_mostrar, true));
        menu = menu_agregar_comando(menu, comando_crear("MOSTRAR", "Muestra los hospitales", ejecutar_mostrar, true));

        menu = menu_agregar_comando(menu, comando_crear("L", "Lista los hospitales", ejecutar_listar, false));
        menu = menu_agregar_comando(menu, comando_crear("l", "Lista los hospitales", ejecutar_listar, true));
        menu = menu_agregar_comando(menu, comando_crear("LISTAR", "Lista los hospitales", ejecutar_listar, true));
        menu = menu_agregar_comando(menu, comando_crear("listar", "Lista los hospitales", ejecutar_listar, true));

        menu = menu_agregar_comando(menu, comando_crear("D", "Destruye un hospital", ejecutar_destruir, false));
        menu = menu_agregar_comando(menu, comando_crear("d", "Destruye un hospital", ejecutar_destruir, true));
        menu = menu_agregar_comando(menu, comando_crear("DESTRUIR", "Destruye un hospital", ejecutar_destruir, true));
        menu = menu_agregar_comando(menu, comando_crear("destruir", "Destruye un hospital", ejecutar_destruir, true));
}

int main()
{
        char buffer[512];
        buffer[0] = 0;
        char *comando;
        bool salir = false;
        menu_t *menu = menu_crear();
        agregar_comandos(menu);
        while(!salir){
                menu_mostrar(menu, "HOSPITAL POKEMONES");
                comando = leer_interaccion(buffer, 512);
                salir = menu_ejecutar_comando(menu, comando);
                free(comando);
        }

        return 0;
}
