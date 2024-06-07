# LOCKER-DISPLAY-KEYPAD-SERIAL

STM32L053R8: Microcontrolador que gestionará todas las operaciones del sistema.
Keypad (4x4): Teclado matricial para la entrada de la contraseña.
Display de 4 Dígitos: Pantalla para mostrar la contraseña ingresada y mensajes.
Módulo Serial (USART2): Para comunicación serial y cambio de contraseña.
Sistema de Bloqueo : Controlado por el microcontrolador para abrir/cerrar el locker.

Funcionalidades
Entrada de Contraseña:

El usuario ingresa una contraseña de 4 dígitos utilizando el keypad.
Cada dígito ingresado se muestra en el display de 4 dígitos.
Verificación de Contraseña:

El microcontrolador verifica si la contraseña ingresada coincide con la almacenada en la memoria.
Si la contraseña es correcta, el sistema de bloqueo se activa para abrir el locker luz verde se activa.
Si la contraseña es incorrecta, una luz roja indicadora se activa.
Cambio de Contraseña Vía Serial:

Conexión serial  al STM32L053R8 para recibir comandos de cambio de contraseña.
Un comando específico permite ingresar una nueva contraseña que reemplaza a la anterior.

Aqui se encuentra una imagen de funcionamiento del keypad y del display de 4 digitos

VIDEO DE EXPLICACION DEL PROGRAMA

https://youtu.be/4VNrx10fvoU



![image](https://github.com/GIRONBR/LOCKER-DISPLAY-KEYPAD-SERIAL/assets/144930597/57a396b9-842d-462e-9391-4f057fab79ad)
