#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "UART/UART.h"
#include "I2C/I2C.h"

#define LM75_ADDR 0x48  // Dirección del sensor 

uint8_t MSB, LSB;
int16_t raw_temp;
float temperatura;

int main(void) {
	I2C_Master_Init(100000, 1);  // Inicializa I2C a 100kHz
	UART_init();
	

	while(1) {
		I2C_Master_Start();
		if (I2C_Master_Write((LM75_ADDR << 1) | 0) != 1) {  // Escribir dirección con W
			I2C_Master_Stop();
		}

		I2C_Master_Write(0x00);  // Registro de temperatura
		I2C_Master_Stop();
		_delay_ms(10);

		I2C_Master_Start();
		if (I2C_Master_Write((LM75_ADDR << 1) | 1) != 1) {  // Leer datos
			I2C_Master_Stop();
		}

		I2C_Master_Read(&MSB, 1);  // Leer MSB con ACK
		I2C_Master_Read(&LSB, 0);  // Leer LSB con NACK
		I2C_Master_Stop();

		// Convertir datos a temperatura real
		raw_temp = (MSB << 8) | LSB;
		raw_temp >>= 5;
		temperatura = raw_temp * 0.125;
		
		UART_Var(temperatura);
		UART_Write("°C");
		UART_Char('\n');
		
		// Ahora puedes imprimir temperatura en una pantalla o enviar por UART
		_delay_ms(1000);
	}
}
