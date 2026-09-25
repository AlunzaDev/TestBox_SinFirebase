# Test Box

## Descripción

La Test Box está basada en un ESP32 y permite ejecutar pruebas de barreras mediante salidas de relé. Incluye un panel web para seleccionar el modo, iniciar pruebas con duración configurable, ajustar tiempos, consultar el contador y revisar el registro de eventos. El contador se conserva en la memoria interna del ESP32.

Dispone de tres modos de operación:

- **Barrera simple:** ejecuta ciclos de cierre y apertura con tiempos configurables.
- **Barrera con lazo:** ejecuta la secuencia de barrera y la activación del lazo.
- **Eventos L0-L2:** permite seleccionar entre tres secuencias de eventos.

El botón físico de pausa detiene o reanuda el proceso. Los LED indican estados de operación y conexión.

## Requisitos

### Hardware

- Placa ESP32 compatible con **ESP32 Dev Module** y memoria flash de 4 MB.
- Cable USB con datos para programar la placa.
- Módulo de relés.
- Alimentación adecuada para el ESP32, el módulo de relés y los equipos bajo prueba.
- Pulsador de pausa y LED de estado, si no forman parte de la placa/caja montada.
- Cableado y protecciones apropiados para las cargas conectadas.

## Pines utilizados

| Función | GPIO |
| --- | ---: |
| Relé 1 | 19 |
| Relé 2 | 21 |
| Relé 3 | 22 |
| Relé 4 | 23 |
| LED de pausa | 4 |
| LED de ejecución | 15 |
| Pulsador de pausa | 5 |
| Indicador Wi-Fi | 2 |
| LED de encendido | 13 |

Las salidas de relé se controlan con lógica activa en bajo en las secuencias principales. Verifique la lógica y el cableado de la tarjeta de relés antes de conectar los equipos.

## Carga del programa

### Arduino IDE

Si se carga desde Arduino IDE, seleccione la placa **ESP32 Dev Module** y, en **Tools > Partition Scheme**, elija **No FS 4MB** antes de compilar y cargar. Seleccione también el puerto serie correspondiente.

## Puesta en marcha

Al arrancar, el ESP32 crea una red Wi-Fi de punto de acceso:

| Parámetro | Valor inicial |
| --- | --- |
| SSID | `TestBox` |
| Contraseña Wi-Fi | `TestBox2026` |
| Dirección del panel | `http://192.168.4.1` |

Conéctese a esa red desde un teléfono u ordenador y abra la dirección del panel. Las funciones protegidas de reinicio del contador y actualización de firmware solicitan autenticación; las credenciales definidas actualmente son usuario admin y contraseña 1234.
