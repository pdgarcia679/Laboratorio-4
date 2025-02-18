// MAESTRO EN COMUNICACI�N I2C

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

//DECLARACI�N DE FUNCIONES
void config_inicial(void); 
void correr_funciones_I2C(void);

// DECLARACI�N DE FUNCIONES PARA EL I2C
void I2C_config_clock (void);
void I2C_repeated_start (void);
void I2C_Iniciar (void);
void I2C_escribir_direccion (unsigned char);
void I2C_leer_direccion (unsigned char);
void I2C_escribir_dato (unsigned char);
void I2C_leer_dato (void);
void I2C_stop (void);

//DECLARO VARIABLES
int tempo=0;
unsigned char address= 0x10, read=1, write=0;        //Para I2C
unsigned char write_data = 0, recv_data,datos;       //Para I2C

int main (void)
{	
	config_inicial();
	I2C_config_clock ();  // Aqu� se configura la velocidad del clock para I2C
	
	while (1)
	{	
		write_data=PINB; //Escribo el dato que deseo transmitir por I2C
		correr_funciones_I2C();	
		PORTD= ~recv_data; // Leo dato que lleg� (recepci�n) por I2C
	}	
}


void config_inicial(void){
	DDRA=0;
	DDRB=0;
	DDRC=0b11111100; // Declaro como entradas a los pines del I2C
	DDRD=255;

	PORTA=255;
	PORTB=255;
	PORTC=0b00000011;
	PORTD=0b00001111;
	
	tempo=MCUCR & 0b11101111;
	MCUCR=tempo;
}

void correr_funciones_I2C(void)
{	
	I2C_Iniciar (); // Funci�n para enviar condici�n de inicio
	I2C_escribir_direccion (address+write); // Funci�n para escribir la direcci�n y el bit de direcci�n de datos (escribir) en SDA
	I2C_escribir_dato (write_data);       // Funci�n para escribir datos en esclavo
	I2C_stop ();  // Funci�n para enviar condici�n de parada
			
	_delay_ms (10);  // Retraso de 10 mili segundos
			
	I2C_Iniciar ();
	I2C_leer_direccion (address+read);  // Funci�n para escribir direcci�n y bit de direcci�n de datos (lectura) en SDA
	I2C_leer_dato ();  // Funci�n para leer datos del esclavo
	I2C_stop ();
	
	_delay_ms (100);
}


void I2C_config_clock (void) // En las dos l�neas siguientes coloco la velocidad del clock, hay una ecuaci�n
{							// en el manual, estos dos par�metros son requeridos para esa ecuaci�n.
	TWBR = 0x01;  // Tasa de bits
	TWSR = (0 << TWPS1) | (0 << TWPS0);  // Configuraci�n de bits prescalares
}

void I2C_Iniciar (void) // En manual est�n estas 3 l�neas, as� lo recomienda*
{
	// Borrar el indicador de interrupci�n TWI, poner la condici�n de inicio en SDA, habilitar TWI
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Espere hasta que se transmita la condici�n de inicio
	while ((TWSR & 0xF8)!= 0x08);   // Verifique el reconocimiento -->Tabla 23-3
}

void I2C_repeated_start (void) // Similar a la condici�n de inicio, pero se debe indicar el reconocimiento
{
	// Borrar el indicador de interrupci�n TWI, poner la condici�n de inicio en SDA, habilitar TWI*
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Esperar hasta que se transmita la condici�n de reinicio*
	while ((TWSR & 0xF8)!= 0x10);  // Verificar el reconocimiento --> Tabla 23-3
}

void I2C_escribir_direccion (unsigned char datos) //Tabla 23-2 dicta este procedimiento
{
	TWDR = datos; // Direcci�n y escritura de instruc --> se env�a al esclavo para q se entere donde va el dato*
	TWCR = (1 << TWINT) | (1 << TWEN); // Borrar el indicador de interrupci�n TWI, habilitar TWI*
	while (!(TWCR & (1 << TWINT)));    // Espere hasta que se transmita el byte TWDR completo*
	while ((TWSR & 0xF8)!= 0x18);      // Verificar el reconocimiento --> Tabla 23-3
}

void I2C_leer_direccion (unsigned char datos) // Igual a escritura, solo cambia el reconocimiento (esto si es l�gico)
{
	TWDR = datos;  // Direc y lectura de instruc --> se env�a al esclavo para q se entere d donde se toma el dato*
	TWCR = (1 << TWINT) | (1 << TWEN); // Borrar el indicador de interrupci�n TWI, habilitar TWI
	while (!(TWCR & (1 << TWINT))); // Espere hasta que se reciba el byte TWDR completo
	while ((TWSR & 0xF8)!= 0x40);  // Verificar el reconocimiento --> Tabla 23-4
}

void I2C_escribir_dato (unsigned char datos) // Notar q es lo mismo pero aqu� se env�a ya un dato y no un comando.
{										  // El dato q se env�a es el de inter�s, se puede mandar m�s datos...
	TWDR = datos;  // poner datos en TWDR
	TWCR = (1 << TWINT) | (1 << TWEN); // Borrar el indicador de interrupci�n TWI, habilitar TWI
	while (!(TWCR & (1 << TWINT))); // Espere hasta que se transmita el byte TWDR completo
	while ((TWSR & 0xF8)!= 0x28); // Verificar el reconocimiento
}

void I2C_leer_dato (void)
{
	TWCR = (1 << TWINT) | (1 << TWEN); // Borrar el indicador de interrupci�n TWI, habilitar TWI
	while (!(TWCR & (1 << TWINT))); // Espere hasta que se transmita el byte TWDR completo
	while ((TWSR & 0xF8)!= 0x58); // Verificar el reconocimiento
	recv_data = TWDR;
}

void I2C_stop (void) // En tabla 23-2 Recomienda esta l�nea para la condici�n de parada
{
	// Borrar el indicador de interrupci�n TWI, poner la condici�n de parada en SDA, habilitar TWI
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}