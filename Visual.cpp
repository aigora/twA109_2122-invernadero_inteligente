#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <conio.h>
#include "SerialClass/SerialClass.h"

#define MAX_BUFFER 200
#define PAUSA_MS 200 
#define ON 1
#define OFF 0
#define MAXCAD 20
#define FILENAME "data.txt"

typedef struct
{
	int sensorluz;
	int sensorhumedad;
	int sensortemp;
} sensores;

int menu_ppal(void);
void act_led(Serial*, int);
void act_bomba(Serial*, int);
void act_vent(Serial*, int);
void comprobar_mensajes(Serial*);
float leer_sensor_temperatura(Serial*);
float leer_sensor_humedad(Serial*);
float leer_sensor_luz(Serial*);
int Enviar_y_Recibir(Serial*, const char*, char*);
float float_from_cadena(char* cadena);
void Send_to_hw(Serial*, char*);
void escribir_fichero_datos(sensores*);
int preguntar_opcion();
void leer_fichero_datos();

int main(void) {
	Serial* Arduino;
	char puerto[] = "COM6"; // Puerto serie al que está conectado Arduino
	int opcion_menu;
	int umbraluz = 0, luces;
	int umbralhumedad = 0, bomba;
	int umbraltemp = 0, ventilador;
	int opcion = 0, n, i;
	sensores datsen;
	sensores* datememoria;



	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);

	comprobar_mensajes(Arduino);

	while (opcion != 6) {
		opcion = menu_ppal();

		switch (opcion)
		{
		case 1:
			do {
				luces = preguntar_opcion();
				act_led(Arduino, luces);
			} while (luces != 2);
			break;

		case 2:
			do {
				bomba = preguntar_opcion();
				act_bomba(Arduino, bomba);
				
			} while (bomba != 2);
			break;

		case 3:
			do {
				ventilador = preguntar_opcion();
				act_vent(Arduino, ventilador);
			} while (ventilador != 2);
			break;

		case 4:
			int mill;
			printf("Introduzca las veces que quieres leer los datos: ");
			scanf_s("%d", &n);
			printf("Introduzca los milisegundos entre lecturas: ");
			scanf_s("%d", &mill);
			datememoria = (sensores*)malloc(sizeof(sensores) * n);
			if (datememoria == NULL)
				printf("Memoria insuficiente");
			else {
				for (i = 0; i < n; i++) {
					datsen.sensorhumedad = leer_sensor_humedad(Arduino);
					Sleep(100);
					datsen.sensorluz = leer_sensor_luz(Arduino);
					Sleep(100);
					datsen.sensortemp = leer_sensor_temperatura(Arduino);
					datememoria->sensorhumedad = datsen.sensorhumedad;
					datememoria->sensorluz = datsen.sensorluz;
					datememoria->sensortemp = datsen.sensortemp;
					printf("\n Sensor luz: %d", datememoria->sensorluz);
					printf("\n Sensor humedad: %d", datememoria->sensorhumedad);
					printf("\n Sensor temperatura: %d", datememoria->sensortemp);
					escribir_fichero_datos(datememoria);
					Sleep(mill);
				}
			}

			break;
		case 5:

			printf("\nIntroduce los valores umbrales de los sensores\n");
			printf("Humedad ");
			scanf_s("%d", &umbraluz);
			printf("\nLuz ");
			scanf_s("%d", &umbralhumedad);
			printf("\n Temparatura ");
			scanf_s("%d", &umbraltemp);

			if (datsen.sensorluz <= umbraluz)
				act_led(Arduino, ON);
			else
				act_led(Arduino, OFF);
			if (datsen.sensorhumedad <= umbralhumedad)
				act_bomba(Arduino, ON);
			else
				act_bomba(Arduino, OFF);
			if (datsen.sensortemp <= umbraltemp)
				act_vent(Arduino, ON);
			else
				act_vent(Arduino, OFF);
			break;
		case 6:
			printf("\n¡Hasta otra!\n\n");
			break;
		default: printf("\nERRROR: ¡Opción incorrecta!\n");

		}
	}

	return 0;
}


int menu_ppal(void)
{
	int opcion = 0;
	do
	{
		system("cls");
		printf("Selecciona la opcion deseada:");
		printf("\n 1) Controlar los LED manualmente");
		printf("\n 2) Controlar el riego manualmente");
		printf("\n 3) Controlar el ventilador manualmente");
		printf("\n 4) Mandar los datos de los sensores a un archivo");
		printf("\n 5) Actuadores automaticos");
		printf("\n 6) Cerrar aplicacion");
		printf("\n Opción: ");

		scanf_s("%d", &opcion);
		if (opcion < 1 || opcion > 6)
			printf("ERROR\n Introduzca otra opcion");
		else
			printf("Ha seleccionado la opcion %d\n", opcion);
	} while (opcion < 1 || opcion > 6);

	return opcion;
}

void comprobar_mensajes(Serial* Arduino)
{
	int bytesRecibidos, total = 0;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Arduino->ReadData(mensaje_recibido, sizeof(char) * MAX_BUFFER - 1);
	while (bytesRecibidos > 0)
	{
		Sleep(PAUSA_MS);
		total += bytesRecibidos;
		bytesRecibidos = Arduino->ReadData(mensaje_recibido + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0)
	{
		mensaje_recibido[total - 1] = '\0';
		printf("\nMensaje recibido: %s\n", mensaje_recibido);
	}
}

int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir)
{
	int bytes_recibidos = 0, total = 0;
	int intentos = 0, fin_linea = 0;


	Arduino->WriteData((char*)mensaje_enviar, strlen(mensaje_enviar));
	Sleep(PAUSA_MS);

	bytes_recibidos = Arduino->ReadData(mensaje_recibir, sizeof(char) * MAX_BUFFER - 1);

	while ((bytes_recibidos > 0 || intentos < 5) && fin_linea == 0)
	{
		if (bytes_recibidos > 0)
		{
			total += bytes_recibidos;
			if (mensaje_recibir[total - 1] == 13 || mensaje_recibir[total - 1] == 10)
				fin_linea = 1;
		}
		else
			intentos++;
		Sleep(PAUSA_MS);
		bytes_recibidos = Arduino->ReadData(mensaje_recibir + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0)
		mensaje_recibir[total - 1] = '\0';

	return total;
}

float leer_sensor_temperatura(Serial* Arduino) {
	float temperatura;
	char mensaje_recibido[MAX_BUFFER];

	if (Enviar_y_Recibir(Arduino, "GET_TEMP\n", mensaje_recibido) <= 0)
		temperatura = -1;
	else
		temperatura = float_from_cadena(mensaje_recibido);
	return temperatura;
}

float leer_sensor_luz(Serial* Arduino) {
	float luz;
	char mensaje_recibido[MAX_BUFFER];

	if (Enviar_y_Recibir(Arduino, "GET_LUZ\n", mensaje_recibido) <= 0)
		luz = -1;
	else
		luz = float_from_cadena(mensaje_recibido);
	return luz;
}

float leer_sensor_humedad(Serial* Arduino) {
	float humedad;
	char mensaje_recibido[MAX_BUFFER];

	if (Enviar_y_Recibir(Arduino, "GET_HUM\n", mensaje_recibido) <= 0)
		humedad = -1;
	else
		humedad = float_from_cadena(mensaje_recibido);
	return humedad;
}

float float_from_cadena(char* cadena) {
	float numero = 0;
	int i, divisor = 10, estado = 0;

	for (i = 0; cadena[i] != '\0' && estado != 3 && i < MAX_BUFFER; i++)
		switch (estado)
		{
		case 0:// Antes del número
			if (cadena[i] >= '0' && cadena[i] <= '9')
			{
				numero = cadena[i] - '0';
				estado = 1;
			}
			break;
		case 1:// Durante el número
			if (cadena[i] >= '0' && cadena[i] <= '9')
				numero = numero * 10 + cadena[i] - '0';
			else
				if (cadena[i] == '.' || cadena[i] == ',')
					estado = 2;
				else
					estado = 3;
			break;
		case 2: // Parte decimal
			if (cadena[i] >= '0' && cadena[i] <= '9')
			{
				numero = numero + (float)(cadena[i] - '0') / divisor;
				divisor *= 10;
			}
			else
				estado = 3;
			break;
		}
	return numero;
}

void act_bomba(Serial* Arduino, int on_off)
{
	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected())//FALTA && DATOS RECIBIDOS<VALORES UMBRALES!! Si hay conexión entre ambos sistemas. Requisito 1
	{
		if (on_off == 1) {
			sprintf_s(BufferSalida, "BOMBA1\n");
			Send_to_hw(Arduino, BufferSalida);
			printf("Se ha activado el riego manual\n");
		}
		if (on_off == 0) {
			sprintf_s(BufferSalida, "BOMBA0\n");
			Send_to_hw(Arduino, BufferSalida);
			printf("Se ha desactivado el riego manual\n");
		}
	}
}

void act_led(Serial* Arduino, int on_off)
{
	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected())//FALTA && DATOS RECIBIDOS<VALORES UMBRALES!! Si hay conexión entre ambos sistemas. Requisito 1
	{
		if (on_off == 1) {
			sprintf_s(BufferSalida, "LED1\n");
			Send_to_hw(Arduino, BufferSalida);
			printf("Se han activado los leds\n");
		}
		if (on_off == 0) {
			sprintf_s(BufferSalida, "LED0\n");
			Send_to_hw(Arduino, BufferSalida);
			printf("Se han desactivado los leds\n");
		}
	}
}

void act_vent(Serial* Arduino, int on_off)
{
	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected())//FALTA && DATOS RECIBIDOS<VALORES UMBRALES!! Si hay conexión entre ambos sistemas. Requisito 1
	{
		if (on_off == 1) {
			sprintf_s(BufferSalida, "VENT1\n");
			Send_to_hw(Arduino, BufferSalida);
			printf("Se ha activado el ventilador");
		}
		if (on_off == 0) {
			sprintf_s(BufferSalida, "VENT0\n");
			Send_to_hw(Arduino, BufferSalida);
			printf("Se ha desactivado el ventilador");
		}
	}
}

void Send_to_hw(Serial* Arduino, char* BufferSalida) {
	Arduino->WriteData(BufferSalida, strlen(BufferSalida));
}

void escribir_fichero_datos(sensores* data) {
	// Creo la variable y abro el fichero
	FILE* file;
	errno_t err;

	err = fopen_s(&file, FILENAME, "a+");
	//fseek(file, 0, SEEK_END);// Movemos el puntero del archivo al final del mismo
	if (err == 0) // Si el fichero data.txt se ha abierto correctamente
	{
		fprintf(file, "Luz %i\n", data->sensorluz);
		fprintf(file, "Humedad %i\n", data->sensorhumedad);
		fprintf(file, "Temperatura %i\n\n", data->sensortemp);


		fclose(file);
	}
	else printf("Se ha producido un error al abrir el fichero de datos.\n");

	//while (!feof(file))
}

void leer_fichero_datos() {
	FILE* file;
	errno_t err;
	char luz[MAX_BUFFER], humedad[MAX_BUFFER];
	char temp[MAX_BUFFER], trash[MAX_BUFFER];

	err = fopen_s(&file, FILENAME, "a+");
	if (err == 0) // Si el fichero data.txt se ha abierto correctamente
	{
		while (!feof(file)) {
			fgets(trash, MAXCAD, file);
			fgets(luz, MAXCAD, file);
			fgets(trash, MAXCAD, file);
			fgets(humedad, MAXCAD, file);
			fgets(trash, MAXCAD, file);
			fgets(temp, MAXCAD, file);

			printf("\nLuz: %s\nHumedad: %s %% \nTemperatura: %s ºC\n\n", luz, humedad, temp);
		}
	}
}

int preguntar_opcion() {
	int opc = 0;

	do {
		printf("\nApagar = 0 / Encender = 1 / Salir = 2: ");
		scanf_s("%d", &opc);

		if (opc < 0 && opc > 2) printf("\n¡Opción incorrecta!");
	} while (opc < 0 && opc > 2);

	return opc;
}