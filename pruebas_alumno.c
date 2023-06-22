#include "pa2m.h"
#include "src/menu.h"
#include <string.h>
#include <stdlib.h>

bool ejecutar_cuenta(void *menu, void *aux1, void *aux2)
{
	*(int *)aux2 = 2;
	return !aux1 ? true : false;
}

bool recorrer_todo(const char *clave, void *valor, void *aux)
{
	size_t *n = (size_t *)aux;
	(*n)++;
	return true;
}

bool recorrer_parcialemente(const char *clave, void *valor, void *aux)
{
	size_t *n = (size_t *)aux;
	if (*n < 4)
		(*n)++;
	return true;
}

bool recorrer_con_false(const char *clave, void *valor, void *aux)
{
	size_t *n = (size_t *)aux;
	(*n)++;
	return false;
}

void crear_menu_con_nombre_valido_no_es_error()
{
	pa2m_nuevo_grupo("Creando menu con nombre válido");
	menu_t *menu = menu_crear("Hospital Pokemon");
	pa2m_afirmar(menu != NULL, "Se puede crear un menu con nombre válido");
	pa2m_afirmar(menu_cantidad(menu) == 0, "El menu está vacío");
	pa2m_afirmar(strcmp(menu_nombre(menu), "Hospital Pokemon") == 0,
		     "El nombre del menu es Hospital Pokemon");

	menu_destruir(menu);
}

void pruebas_crear_menu()
{
	crear_menu_con_nombre_valido_no_es_error();
}

void crear_comando_con_datos_validos_no_es_error()
{
	pa2m_nuevo_grupo("Creando comando con datos válidos");
	comando_t *cmd = comando_crear("C", "Cuenta", "Cuenta hasta dos",
				       ejecutar_cuenta);
	pa2m_afirmar(cmd != NULL,
		     "Se puede crear un comando con datos válidos");
	pa2m_afirmar(strcmp(comando_nombre(cmd), "C") == 0,
		     "El nombre del comando es C");
	pa2m_afirmar(strcmp(comando_informacion(cmd), "Cuenta") == 0,
		     "La información es Cuenta");
	pa2m_afirmar(strcmp(comando_informacion_completa(cmd),
			    "Cuenta hasta dos") == 0,
		     "La información completa es Cuenta hasta dos");

	comando_destruir(cmd);
}

void pruebas_crear_comando()
{
	crear_comando_con_datos_validos_no_es_error();
}

void agregar_comando_valido_no_es_error()
{
	pa2m_nuevo_grupo("Agregando comandos válidos");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_t *cmd2 = comando_crear("E", "Enumerar", "Enumera hasta dos",
					ejecutar_cuenta);
	comando_t *cmd3 = comando_crear("N", "Numerar", "Numera hasta dos",
					ejecutar_cuenta);
	pa2m_afirmar(menu_agregar_comando(menu, cmd1) != NULL,
		     "Se puede agregar un comando valido");
	pa2m_afirmar(menu_cantidad(menu) == 1, "Hay 1 comando en el menu");
	pa2m_afirmar(menu_agregar_comando(menu, cmd2) != NULL,
		     "Se puede agregar otro comando valido");
	pa2m_afirmar(menu_cantidad(menu) == 2, "Hay 2 comando en el menu");
	pa2m_afirmar(menu_agregar_comando(menu, cmd3) != NULL,
		     "Se puede agregar un comando valido");
	pa2m_afirmar(menu_cantidad(menu) == 3, "Hay 3 comando en el menu");

	menu_destruir(menu);
}

void pruebas_agregar_comando_menu()
{
	agregar_comando_valido_no_es_error();
}

void buscar_comando_con_nombre_o_alias_valido()
{
	pa2m_nuevo_grupo("Buscar comando con nombre o alias existente");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);
	comando_agregar_alias(cmd1, "Contar");
	comando_t *cmd_aux = menu_contiene_comando(menu, "C");
	pa2m_afirmar(cmd_aux != NULL, "Se busca el comando C y se encuentra");
	pa2m_afirmar(strcmp(comando_nombre(cmd_aux), "C") == 0,
		     "El nombre del comando es C");
	cmd_aux = menu_contiene_comando(menu, "Contar");
	pa2m_afirmar(
		cmd_aux != NULL,
		"Se busca el comando C mediante el alias Contar y se encuentra");
	pa2m_afirmar(strcmp(comando_nombre(cmd_aux), "C") == 0,
		     "El nombre del comando es C");

	menu_destruir(menu);
}

void buscar_comando_con_nombre_o_alias_inexistente_es_error()
{
	pa2m_nuevo_grupo("Buscar comando con nombre o alias inexistente");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);

	comando_t *cmd_aux = menu_contiene_comando(menu, "Enumerar");
	pa2m_afirmar(!cmd_aux,
		     "Se busca el comando Enumerar y no se encuentra");

	menu_destruir(menu);
}

void pruebas_buscar_comando_menu()
{
	buscar_comando_con_nombre_o_alias_valido();
	buscar_comando_con_nombre_o_alias_inexistente_es_error();
}

void ejecutar_comando_valido_no_es_error()
{
	pa2m_nuevo_grupo("Ejecutar comando con función válida");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);

	size_t n = 0;
	pa2m_afirmar(menu_ejecutar_comando(menu, "C", NULL, &n),
		     "Se ejecutó la función del comando C y devolvió true");
	pa2m_afirmar(n == 2, "Se contó hasta dos");

	pa2m_afirmar(!menu_ejecutar_comando(menu, "C", &n, &n),
		     "Se ejecutó la función del comando C y devolvió false");
	pa2m_afirmar(n == 2, "Se contó hasta dos");

	menu_destruir(menu);
}

void ejecutar_comando_valido_con_valores_inexistentes()
{
	pa2m_nuevo_grupo(
		"Ejecutar comando con función válida y valores inexistentes");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);

	size_t n = 0;

	pa2m_afirmar(
		menu_ejecutar_comando(menu, "A", NULL, &n),
		"No se ejecutó la función de un comando inexistente y devolvió true");
	pa2m_afirmar(n != 2, "No se contó hasta dos");

	menu_destruir(menu);
}

void pruebas_ejecutar_comando_menu()
{
	ejecutar_comando_valido_no_es_error();
	ejecutar_comando_valido_con_valores_inexistentes();
}

void recorrer_todo_el_menu_con_funcion_valida()
{
	pa2m_nuevo_grupo("Iterar menu completo");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_t *cmd2 =
		comando_crear("R", "Resta", "Resta hasta dos", ejecutar_cuenta);
	comando_t *cmd3 =
		comando_crear("S", "Suma", "Suma hasta dos", ejecutar_cuenta);
	comando_t *cmd4 = comando_crear(
		"M", "Multiplica", "Multiplica hasta dos", ejecutar_cuenta);
	comando_t *cmd5 = comando_crear("D", "Divide", "Divide hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);
	menu_agregar_comando(menu, cmd2);
	menu_agregar_comando(menu, cmd3);
	menu_agregar_comando(menu, cmd4);
	menu_agregar_comando(menu, cmd5);

	size_t n = 0;
	size_t contados = menu_con_cada_comando(menu, recorrer_todo, &n);

	pa2m_afirmar(menu_cantidad(menu) == 5,
		     "Se crea un hash con cantidad 5");
	pa2m_afirmar(n == contados, "Se invocó 5 veces la función");

	menu_destruir(menu);
}

void recorrer_parcialmente_el_menu_con_funcion_valida()
{
	pa2m_nuevo_grupo("Iterar menu parcialmente");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_t *cmd2 =
		comando_crear("R", "Resta", "Resta hasta dos", ejecutar_cuenta);
	comando_t *cmd3 =
		comando_crear("S", "Suma", "Suma hasta dos", ejecutar_cuenta);
	comando_t *cmd4 = comando_crear(
		"M", "Multiplica", "Multiplica hasta dos", ejecutar_cuenta);
	comando_t *cmd5 = comando_crear("D", "Divide", "Divide hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);
	menu_agregar_comando(menu, cmd2);
	menu_agregar_comando(menu, cmd3);
	menu_agregar_comando(menu, cmd4);
	menu_agregar_comando(menu, cmd5);

	size_t n = 0;
	size_t contados = menu_con_cada_comando(menu, recorrer_todo, &n);

	pa2m_afirmar(menu_cantidad(menu) == 5,
		     "Se crea un hash con cantidad 5");
	pa2m_afirmar(n == contados, "Se invocó 4 veces la función");

	menu_destruir(menu);
}

void recorrer_el_menu_con_funcion_false()
{
	pa2m_nuevo_grupo("Iterar menu con función false");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_t *cmd2 =
		comando_crear("R", "Resta", "Resta hasta dos", ejecutar_cuenta);
	comando_t *cmd3 =
		comando_crear("S", "Suma", "Suma hasta dos", ejecutar_cuenta);
	comando_t *cmd4 = comando_crear(
		"M", "Multiplica", "Multiplica hasta dos", ejecutar_cuenta);
	comando_t *cmd5 = comando_crear("D", "Divide", "Divide hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);
	menu_agregar_comando(menu, cmd2);
	menu_agregar_comando(menu, cmd3);
	menu_agregar_comando(menu, cmd4);
	menu_agregar_comando(menu, cmd5);
	size_t n = 0;
	size_t contados = menu_con_cada_comando(menu, recorrer_con_false, &n);

	pa2m_afirmar(menu_cantidad(menu) == 5,
		     "Se crea un hash con cantidad 5");
	pa2m_afirmar(n == contados, "Se invocó 1 vez la función");

	menu_destruir(menu);
}

void pruebas_iterador_menu()
{
	recorrer_todo_el_menu_con_funcion_valida();
	recorrer_parcialmente_el_menu_con_funcion_valida();
	recorrer_el_menu_con_funcion_false();
}

void destruir_menu_con_comandos_y_alias_no_es_error()
{
	pa2m_nuevo_grupo(
		"Creando menu con 5 comandos y alias, y destruyendo el menu");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_agregar_alias(cmd1, "Contar");
	comando_t *cmd2 =
		comando_crear("R", "Resta", "Resta hasta dos", ejecutar_cuenta);
	comando_agregar_alias(cmd2, "Restar");
	comando_t *cmd3 =
		comando_crear("S", "Suma", "Suma hasta dos", ejecutar_cuenta);
	comando_agregar_alias(cmd3, "Sumar");
	comando_t *cmd4 = comando_crear(
		"M", "Multiplica", "Multiplica hasta dos", ejecutar_cuenta);
	comando_agregar_alias(cmd4, "Multiplicar");
	comando_t *cmd5 = comando_crear("D", "Divide", "Divide hasta dos",
					ejecutar_cuenta);
	comando_agregar_alias(cmd5, "Dividir");

	menu_agregar_comando(menu, cmd1);
	pa2m_afirmar(menu_cantidad(menu) == 1, "La cantidad de comandos es 1");
	menu_agregar_comando(menu, cmd2);
	pa2m_afirmar(menu_cantidad(menu) == 2, "La cantidad de comandos es 2");
	menu_agregar_comando(menu, cmd3);
	pa2m_afirmar(menu_cantidad(menu) == 3, "La cantidad de comandos es 3");
	menu_agregar_comando(menu, cmd4);
	pa2m_afirmar(menu_cantidad(menu) == 4, "La cantidad de comandos es 4");
	menu_agregar_comando(menu, cmd5);
	pa2m_afirmar(menu_cantidad(menu) == 5, "La cantidad de comandos es 5");

	menu_destruir(menu);
}

void pruebas_creación_y_destruccion()
{
	destruir_menu_con_comandos_y_alias_no_es_error();
}

void crear_menu_con_nombre_null_es_error()
{
	pa2m_nuevo_grupo("Creando menu con nombre NULL");
	menu_t *menu = menu_crear(NULL);
	pa2m_afirmar(!menu, "No se puede crear un menu con nombre NULL");
}

void crear_comando_con_datos_null_es_error()
{
	pa2m_nuevo_grupo("Creando comando con datos NULL");
	comando_t *cmd = comando_crear(NULL, "Cuenta", "Cuenta hasta dos",
				       ejecutar_cuenta);
	pa2m_afirmar(!cmd, "No se puede crear un comando con nombre NULL");
	cmd = comando_crear("C", NULL, "Cuenta hasta dos", ejecutar_cuenta);
	pa2m_afirmar(!cmd, "No se puede crear un comando con información NULL");
	cmd = comando_crear("C", "Cuenta", NULL, ejecutar_cuenta);
	pa2m_afirmar(
		!cmd,
		"No se puede crear un comando con información completa NULL");
	cmd = comando_crear("C", "Cuenta", "Cuenta hasta dos", NULL);
	pa2m_afirmar(!cmd, "No se puede crear un comando con una funcion NULL");
}

void agregar_comando_null_es_error()
{
	pa2m_nuevo_grupo("Agregando comandos null");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear(NULL, "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_t *cmd2 =
		comando_crear("C", NULL, "Cuenta hasta dos", ejecutar_cuenta);
	comando_t *cmd3 = comando_crear("C", "Cuenta", NULL, ejecutar_cuenta);
	comando_t *cmd4 =
		comando_crear("C", "Cuenta", "Cuenta hasta dos", NULL);
	pa2m_afirmar(!menu_agregar_comando(menu, cmd1),
		     "No se puede agregar un comando 1 null");
	pa2m_afirmar(!menu_agregar_comando(menu, cmd2),
		     "No se puede agregar un comando 2 null");
	pa2m_afirmar(!menu_agregar_comando(menu, cmd3),
		     "No se puede agregar un comando 3 null");
	pa2m_afirmar(!menu_agregar_comando(menu, cmd4),
		     "No se puede agregar un comando 4 null");

	menu_destruir(menu);
}

void buscar_comando_con_nombre_o_alias_null_es_error()
{
	pa2m_nuevo_grupo("Buscar comando con nombre o alias null");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);

	comando_t *cmd_aux = menu_contiene_comando(menu, NULL);
	pa2m_afirmar(!cmd_aux, "Se busca comando NULL y no se encuentra");

	menu_destruir(menu);
}

void ejecutar_comando_valido_con_valores_null()
{
	pa2m_nuevo_grupo("Ejecutar comando con función válida y valores null");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	menu_agregar_comando(menu, cmd1);

	size_t n = 0;
	pa2m_afirmar(
		menu_ejecutar_comando(menu, NULL, NULL, &n),
		"No se ejecutó la función de un comando NULL y devolvió true");
	pa2m_afirmar(n != 2, "No se contó hasta dos");

	menu_destruir(menu);
}

void destruir_menu_con_comandos_y_alias_null_no_es_error()
{
	pa2m_nuevo_grupo(
		"Creando menu con 5 comandos y alias null, y destruyendo el menu");
	menu_t *menu = menu_crear("Hospital Pokemon");
	comando_t *cmd1 = comando_crear("C", "Cuenta", "Cuenta hasta dos",
					ejecutar_cuenta);
	comando_t *cmd2 =
		comando_crear("R", "Resta", "Resta hasta dos", ejecutar_cuenta);
	comando_t *cmd3 =
		comando_crear("S", "Suma", "Suma hasta dos", ejecutar_cuenta);
	comando_t *cmd4 = comando_crear(
		"M", "Multiplica", "Multiplica hasta dos", ejecutar_cuenta);
	comando_t *cmd5 = comando_crear("D", "Divide", "Divide hasta dos",
					ejecutar_cuenta);

	menu_agregar_comando(menu, cmd1);
	pa2m_afirmar(menu_cantidad(menu) == 1, "La cantidad de comandos es 1");
	menu_agregar_comando(menu, cmd2);
	pa2m_afirmar(menu_cantidad(menu) == 2, "La cantidad de comandos es 2");
	menu_agregar_comando(menu, cmd3);
	pa2m_afirmar(menu_cantidad(menu) == 3, "La cantidad de comandos es 3");
	menu_agregar_comando(menu, cmd4);
	pa2m_afirmar(menu_cantidad(menu) == 4, "La cantidad de comandos es 4");
	menu_agregar_comando(menu, cmd5);
	pa2m_afirmar(menu_cantidad(menu) == 5, "La cantidad de comandos es 5");

	menu_destruir(menu);
}

void pruebas_con_null()
{
	crear_menu_con_nombre_null_es_error();
	crear_comando_con_datos_null_es_error();
	agregar_comando_null_es_error();
	buscar_comando_con_nombre_o_alias_null_es_error();
	ejecutar_comando_valido_con_valores_null();
	destruir_menu_con_comandos_y_alias_null_no_es_error();
}

int main()
{
	pa2m_nuevo_grupo(
		"\n========================  PRUEBAS DEL TDA - MENU ========================");

	pa2m_nuevo_grupo("PRUEBAS DE CREACIÓN MENU");
	pruebas_crear_menu();

	pa2m_nuevo_grupo("PRUEBAS DE CREACIÓN COMANDO");
	pruebas_crear_comando();

	pa2m_nuevo_grupo("PRUEBAS DE INSERTAR");
	pruebas_agregar_comando_menu();

	pa2m_nuevo_grupo("PRUEBAS DE BUSQUEDA");
	pruebas_buscar_comando_menu();

	pa2m_nuevo_grupo("PRUEBAS DE EJECUTAR COMANDO");
	pruebas_ejecutar_comando_menu();

	pa2m_nuevo_grupo("PRUEBAS DE ITERADOR");
	pruebas_iterador_menu();

	pa2m_nuevo_grupo("PRUEBAS DE CREACIÓN Y DESTRUCCIÓN");
	pruebas_creación_y_destruccion();

	pa2m_nuevo_grupo("PRUEBAS CON NULL");
	pruebas_con_null();

	return pa2m_mostrar_reporte();
}
