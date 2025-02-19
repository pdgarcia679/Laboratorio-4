// ESCLAVO EN LA COMUNICACI�N I2C

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

//DECLARACI�N DE FUNCIONES
void configurar(void);
void correr_funciones_I2C(void);

// DECLARACI�N DE FUNCIONES PARA EL I2C
void I2C_iniciar(void);
void I2C_si_llego_el_dato(void);
void I2C_lectura(void);
void I2C_si_se_envio_el_dato(void);
void I2C_escritura(void);

unsigned char write_data,recv_data, dato_recibido;
unsigned char comando_maestro=0;

int main(void)
{	
	configurar();
	I2C_iniciar(); // Inicializar al esclavo
	
	while(1)
	{	
		PORTD=~recv_data; // Leo lo que llego por I2C	
		correr_funciones_I2C();
		write_data=PINB;  //Escribo el dato para Tx por I2C
	}
}

void configurar(void)
{
	DDRD=0xff;
	DDRB=0;
	PORTB=255;
	MCUCR&= ~(1<<PUD);
}

void correr_funciones_I2C(void)
{
	// Funci�n para hacer coincidir la direcci�n del esclavo y el bit de direcci�n del esclavo (lectura)
	I2C_si_llego_el_dato();
	
	// Funci�n para leer el dato
	I2C_lectura();
			
	// Funci�n para hacer coincidir la direcci�n del esclavo y el bit de direcci�n del esclavo (escribir)
	I2C_si_se_envio_el_dato();
	
	// Funci�n para q el esclavo escriba algo q le llegar� al maestro
	I2C_escritura();
}

void I2C_iniciar(void)
{
	// Colocar la direcci�n del esclavo
	TWAR=0x10;
}

void I2C_escritura(void) // Function to write data
{
	// Aqu� se coloca el dato a enviar al maestro.
	TWDR= write_data;
	// Habilitar el I2C, Borrar la bandera de interrupci�n TWI
	TWCR= (1<<TWEN)|(1<<TWINT);
	// Esperar al reconocimiento
	while((TWSR & 0xF8) != 0xC0);
}

// Function to match the slave address and slave
void I2C_si_se_envio_el_dato(void)
{
	// Bucle hasta que se haya recibido el acuse de recibo correcto
	while((TWSR & 0xF8)!= 0xA8)
	{
		// Obtener el reconocimiento, habilitar el TWI, bajar la bandera de interrupci�n TWI
		TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
		while (!(TWCR & (1<<TWINT)));  // Esperar a la bandera TWINT
	}
}

void I2C_lectura(void)
{
	// Borrar la bandera de interrupci�n TWI, obtener el reconocimiento, Habilitar el TWI
	TWCR= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT))); // Esperar a la bandera TWINT
	while((TWSR & 0xF8)!=0x80); // Esperar el reconocimiento
	recv_data=TWDR; // Tomar el dato q env�a el maestro
}

void I2C_si_llego_el_dato(void)
{	
	while((TWSR & 0xF8)!= 0x60) // Bucle hasta que se haya recibido el acuse de recibo correcto
	{
		// Obtener el reconocimiento, habilitar el TWI, bajar la bandera de interrupci�n TWI
		TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
		while (!(TWCR & (1<<TWINT))); // Esperar a la bandera TWINT
	}
}











