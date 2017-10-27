/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

//#include "program.h"   // <= su propio archivo de cabecera (opcional)
#include "sapi.h"        // <= Biblioteca sAPI

//#include "c_i18n_es.h" // <= para traducir el codigo C al espaï¿½ol (opcional)
//#include "c_i18n_es.h" // <= para traducir la sAPI al espaï¿½ol (opcional)

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

CONSOLE_PRINT_ENABLE

void CargarTiempos            (void);
uint32_t ObtenerTiempoMaximo   (void);

uint32_t TiempoLedRgb, TiempoLed1, TiempoLed2, TiempoLed3, TiempoMaximo;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

uint32_t ObtenerTiempoMaximo   (void){
   uint8_t tiempoMaximoDetectado = FALSE;
   uint32_t tiempoMax;
   
   tiempoMax = TiempoLedRgb;
   
   if (TiempoLed1 > tiempoMax){
      tiempoMax = TiempoLed1;
   }
   if (TiempoLed2 > tiempoMax){
      tiempoMax = TiempoLed2;
   }
   if (TiempoLed3 > tiempoMax){
      tiempoMax = TiempoLed3;
   }
   
   consolePrintString("n\rTiempo maximo detectado: ");
   consolePrintlnInt(tiempoMax);
   
   return tiempoMax;   
}

void CargarTiempos (){
   uint8_t dataUart;
   uint32_t counter;
   uint32_t todosLosValoresCargados = FALSE;
   uint32_t led, timeAux;
   uint8_t timeArray [4] = {0,0,0,0};
   
   // Inicializa los tiempos de los LEDs
   TiempoLedRgb = 0;
   TiempoLed1 = 0;
   TiempoLed2 = 0;
   TiempoLed3 = 0;
   
   consolePrintlnString("\n\n\rPresionar a (LED RGB), 1 (LED1), 2 (LED2), 3(LED3) para cada LED");
   
   //Esperar a que esten cargados los tiempos correspondientes al LEDB, LED1, LED2 y LED3.
   while (todosLosValoresCargados == FALSE){
      //Esperar que lleguen datos por consola      
      if(  uartReadByte( UART_USB, &dataUart ) ){
		// Chequea que el valor ingresado por el usuario sea valido.
         if (dataUart == 'a' || dataUart == '1' || dataUart == '2' || dataUart == '3'){
			//Asigna el dato ingresado al led correspondiente.
            led = dataUart;
            consolePrintlnString("\n\rIngresar el digito en MS en un formato de 4 digitos. Ej: tiempo de 75 ms = 0075");
            //Inicializa el contador. Recordar que el formato a ingresar debe ser obligatoriamente 4 digitos.
            counter = 0;
            while (counter < 4){
				//Espera que lleguen los datos ingresados por el usuario.
               if(  uartReadByte( UART_USB, &dataUart ) ){
					// Chequea que lo ingresado por el usuario sea un numero.
					if (dataUart >= '0' && dataUart <= '9'){
						//Convierte el caracter ASCII ingresado en un numero entero.
						timeArray[counter] = dataUart - '0';	// es lo mismo que restarle 0x30 o 48;  
						consolePrintInt(timeArray[counter]);	// Hace ECO en la terminal.
						counter++;
					}                 
               }
            }
			//Asignacion:   Unidades de mil       Centenas               Decenas               Unidades 
            timeAux = (timeArray[0] * 1000) + (timeArray[1] * 100) + (timeArray[2] * 10) + (timeArray[3] * 1);
            
			//Asigna el delay al LED seleccionado
            if       (led == 'a')   TiempoLedRgb = timeAux;
            else if  (led == '1')   TiempoLed1 = timeAux;
            else if  (led == '2')   TiempoLed2 = timeAux;
            else if  (led == '3')   TiempoLed3 = timeAux;
            
			// Imprime en pantalla el valor del tiempo.
            consolePrintString("\n\rValor ingresado: ");
            consolePrintlnInt(timeAux);
         } else {
            consolePrintlnString("LED invalido. Presionar A, 1, 2 o 3.");
         }
         
      }
      // Si se cargaron correctamente todos los tiempos de los LEDs se sale del while.
      if (TiempoLedRgb != 0 && TiempoLed1 != 0 && TiempoLed2 != 0 && TiempoLed3 != 0){
         todosLosValoresCargados = TRUE;
      }
   }
   consolePrintlnString("\n\n\rTodos los valores cargados:");
   consolePrintString("   Valor LEDB: ");
   consolePrintlnInt(TiempoLedRgb);
   consolePrintString("   Valor LED1: ");
   consolePrintlnInt(TiempoLed1);
   consolePrintString("   Valor LED2: ");
   consolePrintlnInt(TiempoLed2);
   consolePrintString("   Valor LED3: ");
   consolePrintlnInt(TiempoLed3);
   
   TiempoMaximo = ObtenerTiempoMaximo();
   
   consolePrintlnString("\n\n\n\r¡¡¡ PRESIONAR LA TECLA # PARA INGRESAR NUEVOS TIEMPOS !!!\n\n\n\r");
   
   
}

/* FUNCION que se ejecuta cada vezque ocurre un Tick. */
bool_t myTickHookPwm( void *ptr ){

   static uint32_t counter;
   
   if (TiempoLedRgb != 0 && TiempoLed1 != 0 && TiempoLed2 != 0 && TiempoLed3 != 0){
      if (counter == TiempoLedRgb){
        gpioToggle (LEDB);
      }
      if (counter == TiempoLed1){
        gpioToggle (LED1);
      }
      if (counter == TiempoLed2){
        gpioToggle (LED2);
      }
      if (counter == TiempoLed3){
        gpioToggle (LED3);
      }
      if (counter == TiempoMaximo){
        counter = 0;
      }
      counter++;         
   }
   return 1; 
}

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){
uint8_t dataUart;
   /* ------------- INICIALIZACIONES ------------- */

   // Inicializar y configurar la plataforma
	boardConfig();
	
	// Inicializar UART_USB como salida de consola
	consolePrintConfigUart( UART_USB, 115200 );

   consolePrintlnString("\n\rIngresar los tiempos de blinkeo en MS para cada LED.");
   consolePrintlnString("El blinkeo comienza cuando todos los valores de los LEDs estan cargados");
   
   CargarTiempos ();

   /* Inicializar el conteo de Ticks con resolucion de 50ms (se ejecuta
      periódicamente una interrupcón cada 50ms que incrementa un contador de
      Ticks obteniendose una base de tiempos). Se agrega además un "tick hook"
      nombrado myTickHook. El tick hook es simplemente una función que se
      ejecutará períodicamente con cada interrupción de Tick, este nombre se
      refiere a una función "enganchada" a una interrupción */
   tickConfig( 1, myTickHookPwm );
   

   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1) {
		if(  uartReadByte( UART_USB, &dataUart ) ){
			if (dataUart == '#'){
				CargarTiempos ();
			}
		}
   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}


/*==================[fin del archivo]========================================*/