/*********************************************************************************************************
*  dsc_IT100.h                                                                                           *
*                                                                                                        *
*  Libreria para el control serie de alarmas DSC a traves del módulo IT100 de DSC.                       *
*  Autor - Juanjo.                                                                                       *
*  Empresa - Prodoelec.                                                                                  *
*  Licencia - Libre.                                                                                     *
*                                                                                                        *
*  Constructor de la clase                                                                               *
*  =======================                                                                               *
*                                                                                                        *
*  dsc_IT100()                                                                                           *
*                                                                                                        *
*                                                                                                        *
*  Metodos publicos de la clase                                                                          *
*  ============================                                                                          *
*                                                                                                        *
*  void begin()           -> Inicializa el puerto serie                                                  *
*                                                                                                        * 
*  bool link()            -> Devuelve TRUE si hay respuesta del módulo IT100 y FALSE si no es así        *
*                                                                                                        *   
*  String command(String) -> Recibe el comando a ejecutar y devuelve el estado de la alarma              *
*                                                                                                        *
*  Comandos: - state   -> Devuelve estado alarma                Estados: - Alarma disparada              *
*            - away    -> Activa la alarma en ausente                    - Demora de salida              *
*            - stay    -> Activa la alarma en presente                   - Demora de entrada             *
*            - disarm  -> Desactiva alarma                               - Desarmada                     *     
*            - out1    -> Activa salida 1                                - Armada en Ausente             *
*            - out2    -> Activa salida 2                                - Armada en Presente            *  
*                                                                        - Salida ejecutandose           *
*                                                                        - Sistema preparado             *
*                                                                                                        *
*                                                                                                        *
**********************************************************************************************************/

#include <Arduino.h>
#include "dsc_IT100.h"

/*********************************************************************************************************
 * Definiciones                                                                                          *
 * *******************************************************************************************************/

#define LINK "00090"     // Código para control comunicación módulo IT100
#define DEBUG_OFF        // Activa "DEBUG_ON" desactiva "DEBUG_OFF" la depuración serie
#define NUM_STATE   9    // Número de estados que puede tomar la alarma
#define NUM_COMMAND 6    // Número de comandos a enviar a la alarma
#define TIME_OVER 100    // Tiempo maximo de espera puerto serie en milisegundos

/*********************************************************************************************************
 * Coódigos estados que puede tomar la alarma y su correspodiente mensaje                                *
 * *******************************************************************************************************/

const char *status_dsc_IT100[NUM_STATE][NUM_STATE] = {{"6541D0", 
                                                     "6561D2", 
                                                     "6571D3",
                                                     "6551D1", 
                                                     "65210FE",
                                                     "65211FF",
                                                     "6601CD", 
                                                     "6501CC",
                                                     "50000025"
                                                     },

                                                    {"Alarma disparada",
                                                    "Demora de salida", 
                                                    "Demora de entrada",
                                                    "Desactivada", 
                                                    "Activada en Ausente",
                                                    "Activada en Presente",
                                                    "Salida ejecutandose", 
                                                    "Sistema preparado",
                                                    "ACK"
                                                    }};  // Mensajes de estado                                                  

/*********************************************************************************************************
 * Comandos que puede recibir la clase y el codigo enviado al sistema                                    *
 * *******************************************************************************************************/

const char *command_dsc_IT100[NUM_COMMAND][NUM_COMMAND] = {{"state",    
                                                            "away",
                                                            "stay",
                                                            "disarm",
                                                            "out1",
                                                            "out2",
                                                              },

                                                            {"00191",
                                                            "0301C4",
                                                            "0311C5",
                                                            "0401031100EA",
                                                            "02011F4",
                                                            "02013F6",
                                                              }}; // Comando a enviar al módulo 

/*********************************************************************************************************
 * Constructor de la clase                                                                               *
 * *******************************************************************************************************/

dsc_IT100::dsc_IT100()
{
}

/*********************************************************************************************************
 * Metodo que inicializa el puerto serie                                                                 *
 ********************************************************************************************************/

void dsc_IT100::begin()
{
  Serial.begin(9600);   // Incializa el puerto serie
}

/*********************************************************************************************************
 * Metodo que comprueba la comunicación con el módulo de alarma IT100                                    *
 ********************************************************************************************************/

bool dsc_IT100::link()
{
    Serial.println(LINK);         // Envía comando de chequeo módulo IT100
    _reply_status = status();     // Obten respuesta de este
    if (_reply_status == "ACK")   // Si es igual "ACK" devuelve TRUE
    {
        return true;          
    }
    return false;                 // Si no devuelve FALSE
}

/*********************************************************************************************************
 * Metodo que recibe el comando a envíar al módulo y devuelve el estado de la alarma                     *
 ********************************************************************************************************/

String dsc_IT100::command(String code)
{
   _code = code;                                      // Iguala variable interna al codigo recivido
   _reply_command = "NACK";                           // Inicializa respuesta del metodo
  
   for (int i = 0; i < NUM_COMMAND; i++)              // Busca en el array el comando recibido
   {
       if (_code == command_dsc_IT100[0][i])          // Si lo encuentras
       {
           Serial.println(command_dsc_IT100[1][i]);   // Envialo al módulo
           _reply_command = status();                 // y obten la respuesta de este
           return _reply_command;                     // retorna respuesta
       }
   }
   return _reply_command;                             // Retorna NACK si no se reconoce el comando
}

/*********************************************************************************************************
 * Metodo que comprueba el estado de la alarma y lo delvuelve                                            *
 ********************************************************************************************************/

String dsc_IT100::status()
{
    _passtime = millis();
    _i = 0;                                    // Resetea contador de tramas
    _state = "NACK";                           // Estado de la alarma inicial

    while (!Serial.available())                // Espera que lleguen _datas al puerto serie
    {
      if (millis()-_passtime >= TIME_OVER )   // Si se supero el tiempo de espera 
      {
        return _state;                         // retorna
      }
    }               

    do                                          // Hacer mientras sigan llegando _datas
      {
      delayMicroseconds(1100);                  // !! IMPORTANTE !! espera para que no salga  
                                                // del while mientras llega el siguiente dato
      char _data = Serial.read();               // Lee el caracter recibido

      if (_data != '\n')                        // Mientras no llegue el salto de carro
        {
            _trama += _data;                    // añade el caracter a la cadena
        }    
      else
        {                                       // Si llego el final de la trama
        _trama.remove((_trama.length()-1),1);   // quitale el salto de linea
        for (int x = 0; x < NUM_STATE; x++)     // Mira en el array si coincide 
          {                                     // con los estados guardados
          if (_trama == status_dsc_IT100[0][x]) // Si coincide, entonces
            {
              _state = status_dsc_IT100[1][x];  // estado es igual al valor que corresponda
              return _state;
            }
          }
        _trama = "";                            // Vacía la variable para la siguiente    _trama
        _i++;                                   // Cuenta número de tramas (Solo para debug)
        }
      Serial.flush();                           // Vacía buffer puerto serie
      } while (Serial.available());             // Mientras que haya _datas sigue leyendo

      #ifdef DEBUG_ON                           // Depuración por puerto serie para control 
      Serial.print("Tramas: ");                 // Control del metodo
      Serial.println(_i);
      Serial.println(_state);
      #endif

      return _state;                            // Retorna estado de la alarma 
}



