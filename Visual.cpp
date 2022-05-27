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

typedef struct {
	int sensorluz;
	int sensorhumedad;
	int sensortemp;
} sensores;

int menu_ppal(void);
void act_led(Serial*, int, bool*);
void act_bomba(Serial*, int, bool*);
void act_vent(Serial*, int, bool*);
void act_autom(Serial*, int, bool*);
void set_umbrales(Serial*);
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
	int luces, bomba, ventilador, opcion = 0;
	bool autom = false;
	sensores datsen;
	sensores* datememoria;

	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);
	comprobar_mensajes(Arduino);

	while (opcion != 8) {
		opcion = menu_ppal();

		switch (opcion) {
		case 1:
			do {
				luces = preguntar_opcion();
				act_led(Arduino, luces, &autom);
			} while (luces != 2);
			break;

		case 2:
			do {
				bomba = preguntar_opcion();
				act_bomba(Arduino, bomba, &autom);
			} while (bomba != 2);
			break;

		case 3:
			do {
				ventilador = preguntar_opcion();
				act_vent(Arduino, ventilador,& autom);
			} while (ventilador != 2);
			break;

		case 4:
			datememoria = (sensores*)malloc(sizeof(sensores));

			if (datememoria == NULL)
				printf("Memoria insuficiente");
			else {
				datememoria->sensorhumedad = leer_sensor_humedad(Arduino);
				datememoria->sensorluz = leer_sensor_luz(Arduino);
				datememoria->sensortemp = leer_sensor_temperatura(Arduino);
				printf("\n Luz: %d", datememoria->sensorluz);
				printf("\n Humedad: %d", datememoria->sensorhumedad);
				printf("\n Temperatura: %d ºC\n", datememoria->sensortemp);
				escribir_fichero_datos(datememoria);
				system("PAUSE");
			}
			break;

		case 5: 
			leer_fichero_datos();
			system("PAUSE");
			break;

		case 6:
			set_umbrales(Arduino);
			break;

		case 7:
			act_autom(Arduino, ON, &autom);
			break;

		case 8:
			printf("\n¡Hasta otra!\n\n");
			break;

		default: printf("\nERRROR: ¡Opción incorrecta!\n");
		}
	}

	return 0;
}


int menu_ppal(void) {
	int opcion = 0;
	do {
		system("cls");
		printf("Selecciona la opción deseada:");
		printf("\n 1) Controlar los LED manualmente");
		printf("\n 2) Controlar el riego manualmente");
		printf("\n 3) Controlar el ventilador manualmente");
		printf("\n 4) Mandar los datos de los sensores a un archivo");
		printf("\n 5) Mostrar fichero de datos");
		printf("\n 6) Modificar parámetros para el automatismo");
		printf("\n 7) Activar modo automático");
		printf("\n 8) Cerrar aplicación");
		printf("\n Opción: ");

		scanf_s("%d", &opcion);
		if (opcion < 1 || opcion > 8)
			printf("ERROR\n Introduzca otra opcion");
	} while (opcion < 1 || opcion > 8);

	return opcion;
}

void comprobar_mensajes(Serial* Arduino)
{
	int bytesRecibidos, total = 0;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Arduino->ReadData(mensaje_recibido, sizeof(char) * MAX_BUFFER - 1);
	while (bytesRecibidos > 0) {
		Sleep(PAUSA_MS);
		total += bytesRecibidos;
		bytesRecibidos = Arduino->ReadData(mensaje_recibido + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0) {
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

	while ((bytes_recibidos > 0 || intentos < 5) && fin_linea == 0)	{
		if (bytes_recibidos > 0) {
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
		switch (estado) {
		case 0:// Antes del número
			if (cadena[i] >= '0' && cadena[i] <= '9') {
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
			if (cadena[i] >= '0' && cadena[i] <= '9') {
				numero = numero + (float)(cadena[i] - '0') / divisor;
				divisor *= 10;
			}
			else
				estado = 3;
			break;
		}
	return numero;
}

void act_bomba(Serial* Arduino, int on_off, bool* autom) {
	if (on_off == 2) return;

	act_autom(Arduino, OFF, autom);
	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "BOMBA_%s\n", (on_off ? "1" : "0"));
		Send_to_hw(Arduino, BufferSalida);
		printf("Se ha %s el riego manual\n", (on_off ? "activado" : "desactivado"));
	}
}

void act_led(Serial* Arduino, int on_off, bool* autom) {
	if (on_off == 2) return;

	act_autom(Arduino, OFF, autom);
	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "LED_%s\n", (on_off ? "1" : "0"));
		Send_to_hw(Arduino, BufferSalida);
		printf("Se han %s los LEDS\n", (on_off ? "activado" : "desactivado"));
	}
}

void act_vent(Serial* Arduino, int on_off, bool* autom) {
	if (on_off == 2) return;

    act_autom(Arduino, OFF, autom);
	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "VENT_%s\n", (on_off ? "1" : "0"));
		Send_to_hw(Arduino, BufferSalida);
		printf("Se ha %s el ventilador\n", (on_off ? "activado" : "desactivado"));
	}
}

void act_autom(Serial* Arduino, int on_off, bool* autom) {
	if ((on_off == 1 && *autom) || (on_off == 0 && !*autom)) return;

	char BufferSalida[MAX_BUFFER];
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "AUTO_%s\n", (on_off ? "1" : "0"));
		Send_to_hw(Arduino, BufferSalida);
		printf("Se ha %s el modo automático\n", (on_off ? "activado" : "desactivado"));
	}
	*autom = (on_off != 0);
}

void set_umbrales(Serial* Arduino) {
	int umbT = 27, umbH = 30, umbL = 1000;
	char BufferSalida[MAX_BUFFER];

	printf("\nIntroduce los valores umbrales de los sensores para el modo automático\n");
	printf("Humedad: ");
	scanf_s("%d", &umbH);
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "SET_HUM\n %i", umbH);
		Send_to_hw(Arduino, BufferSalida);
	}

	printf("Luz: ");
	scanf_s("%d", &umbL);
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "SET_LUZ\n %i", umbL);
		Send_to_hw(Arduino, BufferSalida);
	}

	printf("Temperatura: ");
	scanf_s("%d", &umbT);
	if (Arduino->IsConnected()) {
		sprintf_s(BufferSalida, "SET_TEMP\n %i", umbT);
		Send_to_hw(Arduino, BufferSalida);
	}
}

void Send_to_hw(Serial* Arduino, char* BufferSalida) {
	Arduino->WriteData(BufferSalida, strlen(BufferSalida));
	Sleep(PAUSA_MS);
}

void escribir_fichero_datos(sensores* data) {
	// Creo la variable y abro el fichero
	FILE* file;
	errno_t err;

	err = fopen_s(&file, FILENAME, "a+");
	if (err == 0) { // Si el fichero data.txt se ha abierto correctamente
		fprintf(file, "Luz: %i\n", data->sensorluz);
		fprintf(file, "Humedad: %i\n", data->sensorhumedad);
		fprintf(file, "Temperatura: %i ºC\n\n", data->sensortemp);

		fclose(file);
	}
	else printf("Se ha producido un error al abrir el fichero de datos.\n");
}

void leer_fichero_datos() {
	FILE* file;
	errno_t err;
	char luz[MAX_BUFFER], humedad[MAX_BUFFER], temp[MAX_BUFFER];

	err = fopen_s(&file, FILENAME, "r");
	if (err == 0) { // Si el fichero data.txt se ha abierto correctamente
		fgets(luz, MAXCAD, file);
		while (!feof(file)) {
			fgets(humedad, MAXCAD, file);
			fgets(temp, MAXCAD, file);
			printf("%s%s%s\n", luz, humedad, temp);

			fgets(luz, MAXCAD, file);
			fgets(luz, MAXCAD, file);
		}
		fclose(file);
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
