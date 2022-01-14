#include "uart.h"
#define MAX_STR 50
#define BAUD 9600	//Funciona para frecuencias mayores o iguales a 1MHz
/******************************************************************************************************************************/
/*                                                Funciones privadas o internas de la libreria						*/
/******************************************************************************************************************************/
volatile unsigned char rx_buffer[MAX_STR] = {0};
volatile unsigned char current_size = 0;
bool isReady = false;

ISR(USART_RX_vect){
   unsigned char ch = UDR0;
   if( ch == '\r' || ch == '\n'){
     rx_buffer[current_size] = 0;
     isReady = true;
   }
   else if( ch == '\b' && current_size>0){
     rx_buffer[--current_size] = 0;
   }
   else{
     rx_buffer[current_size++] = ch;
   }
}



/******************************************************************************************************************************/
/*                                                               Funciones p�blicas de la libreria						*/
/******************************************************************************************************************************/
/////////////////////////////////////////////
//inicializaci�n del m�dulo USART AVR modo as�ncrono
//en una funci�n, a 8bits,a 9600 baudios, sin bit de paridad
//1 bit de parada
/////////////////////////////////////////////////////
void serial_begin(){
	cli();
	float valor_UBBR0 = 0;
	UCSR0A=0b00000010;	//el bit 1 (U2X0) se pone en uno para duplicar la velocidad y poder utilizar frecuencias desde 1MHz
	UCSR0B=0b10011000;	//habilitar interrupcion por recepcion / transmisi�n y recepci�n habilitados a 8 bits
	UCSR0C=0b00000110;	//as�ncrono, sin bit de paridad, 1 bit de parada a 8 bits
	valor_UBBR0 = F_CPU/16.0/BAUD;	//Definir la constante BAUD al inicio del c�digo
        if(UCSR0A & (1<<U2X0)) valor_UBBR0 *= 2;
	UBRR0=valor_UBBR0 - 1;
        sei();
}
///////////////////////////////////////////////
//recepci�n de datos del m�dulo USART AVR modo as�ncrono
////////////////////////////////////////////////////////////
unsigned char serial_read_char(){
	if(UCSR0A&(1<<7)){  //si el bit7 del registro UCSR0A se ha puesto a 1
		return UDR0;    //devuelve el dato almacenado en el registro UDR0
	}
	else//sino
	return 0;//retorna 0
}
///////////////////////////////////////////////
//transmisi�n de datos del m�dulo USART AVR modo as�ncrono
///////////////////////////////////////////////
void serial_print_char(unsigned char caracter){
        if(caracter==0) return;
	while(!(UCSR0A&(1<<5)));    // mientras el registro UDR0 est� lleno espera
	UDR0 = caracter;            //cuando el el registro UDR0 est� vacio se envia el //caracter
}
//////////////////////////////////////////////
//transmisi�n de cadenas de caracteres con el m�dulo USART AVR modo as�ncrono
///////////////////////////////////////////////
void serial_print_str(char *cadena){	//cadena de caracteres de tipo char
	while(*cadena !=0x00){			//mientras el �ltimo valor de la cadena sea diferente
							        //al caracter nulo
		serial_print_char(*cadena);	//transmite los caracteres de cadena
		cadena++;				//incrementa la ubicaci�n de los caracteres en cadena
								//para enviar el siguiente caracter de cadena
	}
}

void serial_println_str(char *cadena){
	serial_print_str(cadena);
	serial_print_char('\r');
	serial_print_char('\n');
}

char* get_RX_buffer(){
   current_size = 0;
   isReady = false;
   return (char*)rx_buffer;
}

bool is_data_ready(){
   return isReady;
}