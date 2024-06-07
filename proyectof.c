#include <stdint.h>
// INCLUYE LA LIBRERIA DONDE SE ALMACENA LOS REGISTISTORS DEL STM32L053
#include "stm32l053xx.h"

//DIGITOS EN CATODO COMUN

#define digit0 0x3F
#define digit1 0X06
#define digit2 0X5B
#define digit3 0X4F
#define digit4 0X66
#define digit5 0X6D
#define digit6 0X7D
#define digit7 0X07
#define digit8 0X7F
#define digit9 0X6F
#define A 0X77
#define B 0X7C
#define C 0X39
#define D 0X5E

// DEFINIENDO FUNCIONES Y METODOS

uint8_t decoder(uint8_t value_to_decode); // DECODIFICADOR DE ENTERO A HEXADECIMAL
void delay_ms(uint16_t n); // DELAY INTERRUPCION DEL PROGRAMA
void UART_SendChar(char c); // ENVIA DATOS CARACTER
void UART_SendString(const char *str); //ENVIA DATOS CADENA

char UART_ReceiveChar(); // RECIBE DATOS CADENA


// ESTRUCTURA PARA EL DISPLAY

struct Pantalla {

	int8_t display_4 ;
	int8_t display_3 ;
	int8_t display_2 ;
	int8_t display_1 ;


	};



// VARIABLES DEL DISPLAY

uint8_t myfsm = 0x00; //CREANDO VARIABLE QUE CONTROLA LA SECUENCIA DE ENCEDIDO DE LOS DISPLAY

// VARIABLES DE EL KEYPAD

uint8_t myfsm1 = 0x00; //CREANDO VARIABLE QUE CONTROLA LA SECUENCIA DE ENCEDIDO DEL KEPAD
uint16_t mascara = 0x00; //MASCARA PARA SOLO USAR LOS ULTIMOS 4 BITS DEL GPIOB
int numero = 0; //VARIABLE ALMACENA EL NUMERO PRESIONADO
int opcion = 0; // VERIFICA QUE OPCION SE PRESIONA

//VARIABLES DEL LOCKER
int digitos1[4]; // DECLARACION DE LA CONTRASENA BASE
int digitos2[4];  // DECLARACION DE LA CONTRASENA A INGRESAR
int comparador =0; //VERIFICA SI LOS DOS ARREGLOS DE NUMEROS SON IGUALES
int resultado =0; // EL RESULTADO DEPENDE SI ABRE O NO LA PUERTA

// VARIABLE DE CAMBIO SERIAL/ DISPLAY
char serial_display ='0';

// VARIABLE SERIAL


//VARIABLES QUE VERIFICAN LA CONTRAENA EN SERIAL
int num;
int verifica;

// VARIABLES QUE CAMBIAN CONTRASENA
int cambio = 0;
char mostrar;






int main(void)

{
	struct Pantalla watch;
	watch.display_1 = 0x00;
	watch.display_2 = 0x00;
	watch.display_3 = 0x00;
	watch.display_4 = 0x00;

	// RELOJ DEL IOPENR

	RCC ->IOPENR |= 1 << 0; //HABILITAR PUERTO A RELOJ
	RCC ->IOPENR |= 1 << 1; //HABILITAR PUERTO B RELOJ
	RCC ->IOPENR |= 1 << 2; //HABILITAR PUERTO C RELOJ

	// PINES PUERTOS A
	// PINES CONFIGURADOS COMO SALIDA PUERTO A

	GPIOA->MODER |= 1<<10; // PIN A5 //LED VERDE
	GPIOA->MODER &= ~(1 << 11);

	GPIOA->PUPDR |= 1<< 10; //RESISTENCIA DE PULL DOWN
	GPIOA->PUPDR &= ~(1 << 11);

	GPIOA->MODER |= 1<<12; // PIN A6 // LED ROJO
	GPIOA->MODER &= ~(1 << 13);

	GPIOA->MODER |= 1<<14; // PIN A7 // ALARMA
	GPIOA->MODER &= ~(1 << 15);

	GPIOA->PUPDR |= 1<< 14; //RESISTENCIA DE PULL DOWN
	GPIOA->PUPDR &= ~(1 << 15);

	// PINES DE USART2

	//RCC DEL AP1 DEL USART2
	RCC->APB1ENR |= 1<<17;

    // CONFIGURACION USART 2
	GPIOA->MODER &= ~(1 << 4);  // PINES PA2 Y PA3 ALTERNATIVO
    GPIOA->MODER &= ~(1 << 6);

    // SELECCIONAR MODO AF4
    GPIOA->AFR[0] |= 1 << 10;  // MAPEAR PA2 A AF4
    GPIOA->AFR[0] |= 1 << 14;  // MAPEAR PA3 A AF4

    USART2->BRR =218 ; //TASA DE PUSHEO
    // HABILITAR RX Y TX
    USART2->CR1 |= (1 << 2) | (1 << 3);
    // HABILITAR USAR2
    USART2->CR1 |= 1 << 0;

	// PINES PUERTOS B
	//PINES CONFIGURADOS PARA DISPLAY

	// PINES CONFIGURADOS COMO SALIDA DEL PUERTO B, DISPLAY DE 7 SEGMENTOS MULTIPLEXADO

	GPIOB ->MODER |= 1<<0;       //PIN B0
	GPIOB ->MODER &= ~(1 << 1);

	GPIOB ->MODER |= 1<< 2;	     //PIN B1
	GPIOB ->MODER &= ~(1 << 3);

	GPIOB ->MODER |= 1 << 4;	//PIN B2
	GPIOB ->MODER &= ~(1 << 5);

	GPIOB ->MODER |= 1 << 6;	//PIN B3
	GPIOB ->MODER &= ~(1 << 7);

	GPIOB ->MODER |= 1 << 8;	//PIN B4
	GPIOB ->MODER &= ~(1 << 9);

	GPIOB ->MODER |= 1 << 10;	//PIN B5
	GPIOB ->MODER &= ~(1 << 11);

	GPIOB ->MODER |= 1 << 12;	//PIN B6
	GPIOB ->MODER &= ~(1 << 13);

	GPIOB ->MODER |= 1 << 14;	//PIN B7
	GPIOB ->MODER &= ~(1 << 15);



	// PINES CONFIGURADOS COMO ENTRADA DEL PUERTO B DEL KPAD

	GPIOB ->MODER &= ~(1 << 16); // PIN PB8
	GPIOB ->MODER &= ~(1 << 17);

	GPIOB ->PUPDR |= 1<<16; //RESISTENCIA DE PULL-UP PB8
	GPIOB ->PUPDR &= ~(1 << 17);

	GPIOB ->MODER &= ~(1 << 18); //PIN PB9
	GPIOB ->MODER &= ~(1 << 19);

	GPIOB ->PUPDR |= 1<<18; //RESISTENCIA DE PULL-UP PB9
	GPIOB ->PUPDR &= ~(1 << 19);

	GPIOB ->MODER &= ~(1 << 20); //PIN PB10
	GPIOB ->MODER &= ~(1 << 21);

	GPIOB ->PUPDR |= 1<<20; //RESISTENCIA DE PULL-UP PB10
	GPIOB ->PUPDR &= ~(1 << 21);

	GPIOB ->MODER &= ~(1 << 22); // PB11
	GPIOB ->MODER &= ~(1 << 23);

	GPIOB ->PUPDR |= 1<<22; //RESISTENCIA DE PULL-UP PB11
	GPIOB ->PUPDR &= ~(1 << 23);

	// PINES CONFIGURADOS COMO SALIDA DEL PUERTO B DEL KPAD

	GPIOB ->MODER |= 1 << 24;	//PIN B12
	GPIOB ->MODER &= ~(1 << 25);

	GPIOB ->MODER |= 1 << 26;	//PIN B13
	GPIOB ->MODER &= ~(1 << 27);

	GPIOB ->MODER |= 1 << 28;	//PIN B14
	GPIOB ->MODER &= ~(1 << 29);

	GPIOB ->MODER |= 1 << 30;	//PIN B15
	GPIOB ->MODER &= ~(1 << 31);



	// PINES PUERTO C
	// PINES DEL CONTROL DEL DISPLAY

	GPIOC->MODER |= 1<<10; //PIN C5
	GPIOC->MODER &= ~(1 << 11);

	GPIOC->MODER |= 1<<12; //PIN C6
	GPIOC->MODER &= ~(1 << 13);

	GPIOC->MODER |= 1<<16; //PIN C8
	GPIOC->MODER &= ~(1 << 17);

	GPIOC->MODER |= 1<<18; //PIN C9
	GPIOC->MODER &= ~(1 << 19);

	// HABILITAR RELOJ DE 16 MHZ
	//RCC->CR |= (1<<0); //HABILITA EL RELOJ DE 16MHZ
	//RCC->CFGR |= (1<<0); // ESTABLECIENDO SISTEM CLOCK


	// TIMER 2
	// RELOJ DEL APB1 DONDE SE ENCUENTRA EL TIMER2

	RCC->APB1ENR |= (1<<0); //HABILITA EL RELOJ EL APB1 TANTO PARA EL TIMER COMO PARA EL SERIAL
	TIM2->PSC = 1600-1; // HABILITA EL PRESCALADOR
	TIM2->ARR = 2-1; // VALOR DE AUTORECARGO CONTADOR 1ms ARR=10 16MHZ / ARR =1.31 2.097MHZ
	TIM2->CR1 = (1<<4); // CONTADOR HACIA ABAJO
	TIM2->CR1 |= (1<<0); // HABILITA EL TIMER COMO TAL





	//INGRESAR CONTRASENA
	digitos2[0] = 0;
    digitos2[1] = 0;
    digitos2[2] = 0;
    digitos2[3] = 0;

    //CINTRASENA BASE
    digitos1[0] = 1;
    digitos1[1] = 2;
    digitos1[2] = 3;
    digitos1[3] = 4;

    opcion = 0;
    numero = 0;
    comparador = 0; //INDICA QUE NO SON IGUALES

	GPIOA->ODR |= (1<<6);  //ACTIVA PA6
	GPIOA->ODR &= ~(1<<5); //DESACTIVA PA5



while(1)
{
	// UART
	if((serial_display == '1'))
	{
		GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
		UART_SendString_E("INGRESE CONTRASENA BASE: ");
		for(int i=0; i<4; i++) //INGRESA CONTRASENA PARA VERIFICAR SI ES LA MISMA QUE LA BASE
		{
			UART_SendString("EL NUMERO INGRESADO ES: ");
			mostrar= UART_ReceiveChar();
			num = mostrar -'0'; //CONVIERTE CARACTER A NUMERO
			UART_SendChar(mostrar);
			UART_SendChar(0X0D);
		    UART_SendChar(0X0A);
			digitos2[i] = num;
		}
		for(int i=0; i<4; i++) //VERIFICA QUE LA CONTRASENA INGRESADA ES IGUAL A LA BASE
		{
			if (digitos1[i] != digitos2[i])
			{
				UART_SendString_E("CONTRASENA INCORRECTA INTENTELO DE NUEVO ");
				verifica = 0;
				break;
			}
			else{
				verifica = 1;
			}
		}
		if(verifica ==1)
		{
			verifica =0;
			UART_SendString_E("ELEGIR OPCION: ");
			UART_SendString_E("1: CAMBIAR CONTRASENA ");
			UART_SendString_E("2: SALIR DEL MODO SERIAL ");

			serial_display =  UART_ReceiveChar();
			if(serial_display =='1')
			{
			for(int i=0; i<4; i++)
			{
				UART_SendString("EL NUMERO INGRESADO ES: ");
				mostrar= UART_ReceiveChar();
				cambio = mostrar -'0';
				UART_SendChar(mostrar);
				UART_SendChar(0X0D);
			    UART_SendChar(0X0A);
				digitos1[i] = cambio;
			}
			UART_SendString_E("CONTRASENA CAMBIADA CON EXITO");

			} // TERMINA CAMBIO DE CONTRASENA
		}





	 }// TERMINA ELEGIR MODO SERIAL ACTIVO









	// CONTROL DE ACCESOS
	 switch(opcion) {
	 		case 0:

	 			watch.display_1 = 0; // LIMPIA TODOS LOS DISPLAY
	 			watch.display_2 = 0;
	 			watch.display_3 = 0;
	 			watch.display_4 = 0;
	 			digitos2[0] =0;
	 			digitos2[1] =0;
	 			digitos2[2] =0;
	 			digitos2[3] =0;
	 			break;


	        case 1:

	            digitos2[0] = numero;  //ALMACENA NUMERO 1
	            watch.display_1 = numero; //MANDA A PINTAR DISPLAY 1
	            break;

	        case 2:

	            digitos2[1] = numero; // ALMACENA NUMERO 2
	            watch.display_2 = numero; //MANDA A PINTAR DISPLAY 2
	            break;

	        case 3:

	            digitos2[2] = numero; // ALMACENA NUMERO 3
	            watch.display_3 = numero; //MANDA A PINTAR DISPLAY 3
	            break;

	        case 4:

	            digitos2[3] = numero; //ALMACENA NUMERO 4
	            watch.display_4 = numero;

	            break;

	        case 5: // SOLO SI SE PRESIONA LA LETRA D
	        	//COMPARADOR DE CONTRASENAS
	        	if(resultado==1){

	        		resultado =0;
	        		for (int i = 0; i < 4; i++) {
	        			if (digitos1[i] != digitos2[i]) {
	            		comparador = 0;  // SI ES 0 SON DIFERENTES
	            		break;
	        			} //TERMINA IF
	        			else{
	        			comparador = 1; //SI ES 1 INDICA QUE SON IGUALES
	        		}

	        		} // TERMINA FOR
	        		if(comparador == 1){
	   			 	//ABRE PUERTA


					GPIOA->ODR |= (1<<5); //ACTIVA PA5
					GPIOA->ODR &= ~(1<<6); //DESACTIVA PA6
					//GPIOA->ODR |= (1<<7); //ACTIVA ALARMA PA7
					//delay_ms(500);
					//GPIOA->ODR &= ~(1<<7); //DESACTIVA ALARMA PA7


	        		}
	        		else if(comparador == 0){
	   			 	//ACCESO DENEGADO
	   			 	GPIOA->ODR |= (1<<6);  //ACTIVA PA6
					GPIOA->ODR &= ~(1<<5); //DESACTIVA PA5

	        		}
	        		}
	        	else if(resultado ==0){
	        		opcion =0;
	        	}

	   			 break;



	        default:
	            break;
	    }




	if(TIM2->SR & 0x00000001){
		TIM2->SR &= ~1; //LIMPIAR BANDERA UIF


		// DISPLAY MULTIPLEXASION
		//MAQUINA DE ESTADOS FINITOS DISPLAY
		switch(myfsm)
		{
		case 0:
		{
			//PARA EL DISPLAY 1
		myfsm++;
		GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
		GPIOC ->BSRR |= 0X01 << 25; // LIMPIAR D3/C9
		GPIOC ->BSRR |= 0X01 << 5; // HABILITA D0/C5
		GPIOB ->BSRR |= decoder(watch.display_1) << 0; // MANDA A PINTAR DISPLAY 1

		break;

		}// TERMINA CASO 0
		case 1:
		{
			//PARA EL DISPLAY 2
		myfsm++;
		GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
		GPIOC ->BSRR |= 0X01 << 21; // LIMPIAR D0/C5
		GPIOC ->BSRR |= 0X01 << 6 ; // HABILITA D1/C6
		GPIOB ->BSRR |= decoder(watch.display_2) << 0; //MANDA A PINTAR DISPLAY 2

		break;


		} //TERMINA CASO 1

		case 2:{
			//PARA EL DISPLAY 3
		myfsm++;
		GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
		GPIOC ->BSRR |= 0X01 << 22; // LIMPIAR D1/C6
		GPIOC ->BSRR |= 0X01 << 8; // HABILITA D2/C8
		GPIOB ->BSRR |= decoder(watch.display_3) << 0; //MANDA A PINTAR DISPLAY 3

		break;
		}//TERMINA CASO 2

		case 3:{
			//PARA EL DISPLAY 4
			myfsm =0;
			GPIOB ->BSRR |= 0XFF << 16; //LIMPIAR DISPLAY
			GPIOC ->BSRR |= 0X01 << 24; // LIMPIAR D2/C8
			GPIOC ->BSRR |= 0X01 << 9; // HABILITA D3/C9
			GPIOB ->BSRR |= decoder(watch.display_4) << 0; //MANDA A PINTAR DISPLAY 4
			break;

		} //TERMINA CASO 3
		default:{
			//CASO NO TOME NINGUNO DE LOS OTROS
			myfsm =0;
			break;
		} // TERMINA DEFAULT

		}//TERMINA SWITCH DE DISPLAY MAQUINA DE ESTADOS FINITOS

		//KEYPAD
		//KEYPAD MULTIPLEXACION



		} //TERMINA TIMER DE 1MS

		switch(myfsm1)
					{
					case 0:{
						//PARA LA COLUMAN 0
					myfsm1++;

					GPIOB->ODR |= 1<<15; // APAGANDO PB15
					GPIOB->ODR &= ~(1 << 12); //HABILITANDO PB12
					mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
					if (mascara == 0xE00){ //1110
						 //LETRA A
						serial_display ='1';
						delay_ms(500);
					}
					else if (mascara == 0xD00 ){ //1101
						 //LETRA B
						delay_ms(500);
					}
					else if (mascara == 0xB00){ //1011
						 //LETRA C
						delay_ms(500);
					}
					else if (mascara ==0X700){ //0111
						 //LETRA D
						opcion =5;
						resultado =1;
						delay_ms(500);
					}

					break;

					}
					case 1:{
						//PARA COLUMNA 1
					myfsm1++;
					GPIOB->ODR |= 1<<12; // APAGANDO PB12
					GPIOB->ODR &= ~(1 << 13); //HABILITANDO PB13
					mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
					if (mascara == 0xE00){ //1110
						delay_ms(500);
						//NUMERO 3
						numero = 3;
						opcion ++;
						resultado =0;
					}
					else if (mascara == 0xD00 ){ //1101
						 //NUMERO 6
						delay_ms(500);
						numero = 6;
						opcion ++;
						resultado =0;
					}
					else if (mascara == 0xB00){ //1011
						// NUMERO 9
						delay_ms(500);
						numero = 9;
						opcion ++;
						resultado =0;
					}
					else if (mascara ==0X700){ //0111
						delay_ms(300);
						 //NUM


					}

					break;
					}
					case 2:{
						//PARA COLUMNA 2
						myfsm1++;
						GPIOB->ODR |= 1<<13; // APAGANDO PB13
						GPIOB->ODR &= ~(1 << 14); //HABILITANDO PB14
						mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
						if (mascara == 0xE00){ //1110
							 //NUMERO 2
							delay_ms(500);
							numero = 2;
							opcion ++;
							resultado =0;
						}
						else if (mascara == 0xD00 ){ //1101
							 //NUMERO 5
							delay_ms(500);
							numero = 5;
							opcion ++;
							resultado =0;
						}
						else if (mascara == 0xB00){ //1011
							 //NUMERO 8
							delay_ms(500);
							numero = 8;
							opcion ++;
							resultado =0;
						}
						else if (mascara ==0X700){ //0111
							 //NUMERO 0
							delay_ms(500);
							numero = 0;
							opcion ++;
							resultado =0;
						}

						break;
					}

					case 3:{
						//PARA COLUMNA 3
						myfsm1 = 0;

						GPIOB->ODR |= 1<<14; // APAGANDO PB14
						GPIOB->ODR &= ~(1 << 15); //HABILITANDO PB15
						mascara = GPIOB->IDR & 0XF00; //CREA MASCARA DE 1111-0000-0000
						if (mascara == 0xE00){ //1110
							 //NUMERO 1
							delay_ms(500);
							numero = 1;
							opcion ++;
							resultado =0;
						}
						else if (mascara == 0xD00 ){ //1101
							 //NUMERO 4
							delay_ms(500);
							numero = 4;
							opcion ++;
							resultado =0;

						}
						else if (mascara == 0xB00){ //1011
							 //NUMERO 7
							delay_ms(500);
							numero = 7;
							opcion ++;
							resultado =0;
						}
						else if (mascara ==0X700){ //0111
							 //NUMERO *
							delay_ms(500);
						}


						break;
					}
					default:{
						myfsm1 = 0;

						break;
					}



					} //TERMINA MAQUINA DE ESTADOS FINITOS KPAD










	}//TERMINA WHILE



} //TERMINA EL MAIN










uint8_t decoder(uint8_t value_to_decode)
{
	switch(value_to_decode)
	{
	case 0:
	{
		return(digit0);

	}
	case 1:{

		return(digit1);
	}
	case 2:{

		return(digit2);
	}
	case 3:{

		return(digit3);
	}
	case 4:{

		return(digit4);
	}
	case 5:{

		return(digit5);
	}

	case 6:{

		return(digit6);
	}

	case 7:{

		return(digit7);
	}

	case 8:{

		return(digit8);
	}
	case 9:{

		return(digit9);
	}
	case 10:{
		return(A);
	}
	case 11:{
		return(B);
	}
	case 12:{
		return(C);
	}
	case 13:{
		return(D);
	}
	default:
	{

	}
	return 0x00;


	}
} // TERMINA EL DECODER

void delay_ms(uint16_t n){

	uint32_t i;
	for(; n>0; n--)
		for(i=0; i<140; i++);
} // TERMINA DELAY

void UART_SendChar(char c) { // MANDA UN DATOS CARACTER
    while (!(USART2->ISR & 0X0080)){} // ESPERAR A QUE ESTE VACIO
    USART2->TDR = c; // ENVIAR UN CARACTER
}

void UART_SendString(const char *str) { // MANDA DATOS CADENA
    while (*str != '\0') {
        UART_SendChar(*str++);
    }
}

void UART_SendString_E(const char *str) { // MANDA DATOS CADENA
    while (*str != '\0') {

        UART_SendChar(*str++);


    }
    UART_SendChar(0X0D);
    UART_SendChar(0X0A);
}

char UART_ReceiveChar() { //RECIBE DATOS
    while (!(USART2->ISR & 0X0020)){} // ESPERAR A QUE LLEGUE UN BYTE
    return USART2->RDR; // DEVOLVER BYTE
}







