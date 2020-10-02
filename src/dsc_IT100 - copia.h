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
**********************************************************************************************************/

#ifndef dsc_IT100_h
#define dsc_IT100_h

#include <Arduino.h>

class dsc_IT100
{
private:
    String status(void);            // Metodo que devuelve el estado de la alarma
    String _code;                   // Código recibido
    String _state;                  // Estado de la alarma
    String _reply_command;          // Respuesta metodo command
    String _reply_status;           // Respuesta metodo status
    int _i;                         // Contador de tramas recibidas
    char _data;                     // Caracter recibido por el puerto serie
    String _trama;                  // Trama de caracteres recibidos por el puerto serie
    unsigned long _passtime;       // Tiempo para controlar tiempo de espera maximo puerto serie     

public:     
    dsc_IT100();                    // Constructor de la clase 
    void begin(void);               // Metodo de inicialización de la clase
    bool link(void);                // Metodo para comprobar comunicación módulo IT100    
    String command(String code);    // Metodo para chequear estado alarma
        
};

#endif