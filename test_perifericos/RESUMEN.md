# Resumen del Proyecto de Prueba

## ✅ Archivos Creados

### Drivers
- ✅ `Core/Inc/ws2812b.h` - Header del driver WS2812B
- ✅ `Core/Src/ws2812b.c` - Implementación del driver WS2812B
- ✅ `Core/Inc/keyboard.h` - Header del driver de matriz de pulsadores 4x4
- ✅ `Core/Src/keyboard.c` - Implementación del driver de matriz de pulsadores

### Aplicación Principal
- ✅ `Core/Inc/main.h` - Header principal
- ✅ `Core/Src/main.c` - Programa principal de prueba
- ✅ `Core/Inc/stm32f4xx_it.h` - Header de interrupciones
- ✅ `Core/Src/stm32f4xx_it.c` - Handlers de interrupciones

### Documentación
- ✅ `README.md` - Guía de uso del proyecto
- ✅ `CONFIGURACION_CUBEMX.md` - Paso a paso para configurar en CubeMX
- ✅ `NOTAS_TECNICAS.md` - Detalles técnicos y troubleshooting
- ✅ `RESUMEN.md` - Este archivo

## 📋 Checklist para Empezar

### Antes de Compilar
- [ ] STM32CubeIDE instalado
- [ ] Drivers ST-Link instalados
- [ ] Placa Nucleo-144 conectada por USB
- [ ] Hardware conectado según esquema en README.md

### Configuración del Proyecto
**Opción A - Configurar en CubeMX (recomendado para aprender)**:
- [ ] Seguir `CONFIGURACION_CUBEMX.md` paso a paso
- [ ] Generar código base
- [ ] Copiar drivers personalizados
- [ ] Compilar

**Opción B - Usar código directo**:
- [ ] Crear proyecto nuevo en STM32CubeIDE
- [ ] Copiar todos los archivos de `Core/`
- [ ] Configurar includes y paths
- [ ] Agregar archivos HAL necesarios
- [ ] Compilar

### Verificación de Hardware
- [ ] Teclado: 4 filas a PC0-PC3, 4 columnas a PA0,PA1,PA4,PA5
- [ ] Teclado: Resistencias pull-down 10kΩ en columnas
- [ ] WS2812B: Señal de datos a PB6 (via level shifter)
- [ ] WS2812B: Alimentación 5V desde fuente externa
- [ ] WS2812B: GND común con Nucleo
- [ ] Level shifter: LV (3.3V) conectado a Nucleo, HV (5V) a fuente externa

### Primera Prueba
- [ ] Compilar sin errores
- [ ] Flashear a la placa
- [ ] LED verde onboard parpadea cada 500ms → ✓ Programa corriendo
- [ ] Presionar tecla '1' → LED superior izquierdo se enciende en rojo
- [ ] Presionar otras teclas → LEDs correspondientes se encienden

## 🎯 Funcionalidad del Proyecto

### Qué Hace
Al presionar un pulsador de la matriz 4x4, se enciende el LED correspondiente en la matriz WS2812B con un color específico. Solo un LED está encendido a la vez.

### Mapeo Pulsador → LED (Columnas Invertidas)
```
Matriz de Pulsadores:       Matriz de LEDs:
[P0] [P1] [P2] [P3]    →   [3]  [2]  [1]  [0]
[P4] [P5] [P6] [P7]    →   [7]  [6]  [5]  [4]
[P8] [P9] [P10][P11]   →   [11] [10] [9]  [8]
[P12][P13][P14][P15]   →   [15] [14] [13] [12]

Colores:
P0(KEY_1)  → LED 3  (Rojo)
P1(KEY_2)  → LED 2  (Naranja)
P2(KEY_3)  → LED 1  (Amarillo)
P3(KEY_A)  → LED 0  (Rojo)
P4(KEY_4)  → LED 7  (Verde)
P5(KEY_5)  → LED 6  (Cyan)
P6(KEY_6)  → LED 5  (Azul)
P7(KEY_B)  → LED 4  (Azul)
P8(KEY_7)  → LED 11 (Violeta)
P9(KEY_8)  → LED 10 (Magenta)
P10(KEY_9) → LED 9  (Blanco)
P11(KEY_C) → LED 8  (Verde)
P12(KEY_*) → LED 15 (Blanco)
P13(KEY_0) → LED 14 (Gris)
P14(KEY_#) → LED 13 (Amarillo oscuro)
P15(KEY_D) → LED 12 (Amarillo)
```

## 🔧 Características Técnicas

### WS2812B Driver
- Protocolo: PWM + DMA
- Frecuencia: 800 kHz
- Buffer: 444 valores × 2 bytes = 888 bytes RAM
- Timing preciso mediante hardware

### Teclado Driver
- Escaneo: Por filas (activa una fila a la vez)
- Anti-rebote: 30ms por software
- Frecuencia de actualización: 1 kHz (TIM6)
- Detección: Solo al presionar (no al soltar)

### Recursos Utilizados
- **TIM4**: PWM Channel 1 para WS2812B (84 MHz timer clock, Period=104 → 800 kHz)
- **TIM6**: Base timer para teclado (84 MHz / 8400 / 10 = 1 kHz → 1 ms interrupt)
- **DMA1 Stream 0**: Transferencia PWM buffer (Channel 2)
- **SYSCLK**: 168 MHz
- **APB1 Timer Clock**: 84 MHz (APB1=42 MHz × 2)
- **APB2 Timer Clock**: 168 MHz (APB2=84 MHz × 2)
- **GPIO**: 
  - PE13,PF15,PG14,PG9: Filas teclado/matriz pulsadores (Output)
  - PF14,PE11,PE9,PF13: Columnas teclado/matriz pulsadores (Input, pull-down 10kΩ)
  - PD12: Señal WS2812B (TIM4_CH1)
  - PB7: LED onboard (heartbeat)

## 🚀 Próximos Pasos

Una vez que este proyecto funcione correctamente:

1. **Validación completa del hardware** ✓
   - Todas las teclas responden
   - Todos los LEDs funcionan
   - Sin rebotes en las teclas

2. **Implementar lógica del juego**
   - Módulo `game_logic.c` con reglas del tateti
   - Detección de victoria (3 en línea)
   - Detección de empate

3. **Diseñar statechart**
   - Usar ITEMIS CREATE
   - Estados: IDLE → PLAYING → MATCH_END → GAME_OVER
   - Eventos: key_press, win, draw, reset

4. **Módulo de display**
   - Mapear estado del juego a la matriz
   - Mostrar tablero 3×3
   - Mostrar contadores de partidas
   - Mostrar indicador de turno

5. **Integración completa**
   - Cambio de colores durante partida (teclas A y B)
   - Animaciones de victoria
   - Sistema de puntuación

## 📚 Archivos de Referencia

- **README.md**: Guía principal de uso
- **CONFIGURACION_CUBEMX.md**: Configuración paso a paso
- **NOTAS_TECNICAS.md**: Detalles de implementación y troubleshooting
- **PROYECTO_TATETI.md** (en carpeta raíz): Especificación completa del proyecto final

## ⚠️ Troubleshooting Rápido

| Problema | Solución |
|----------|----------|
| No compila | Verificar que HAL drivers están incluidos |
| LED onboard no parpadea | Error en inicialización, ver Error_Handler() |
| Teclado no responde | Verificar conexiones y pull-downs |
| LEDs WS2812B no encienden | Verificar level shifter y alimentación 5V |
| Colores incorrectos | Ajustar WS2812B_LED_MAP[] según tu hardware |
| Lecturas múltiples | Aumentar KEYBOARD_DEBOUNCE_MS |

## 💡 Tips

1. **Prueba el LED onboard primero**: Si parpadea, el programa está corriendo
2. **Prueba el teclado sin LEDs**: Usa debug/printf para verificar lecturas
3. **Prueba los LEDs sin teclado**: Enciende LEDs en secuencia automática
4. **Mide voltajes**: Verifica 3.3V en pines del micro y 5V en LEDs
5. **Usa osciloscopio**: Si tienes, verifica señal PWM en PB6

## 📧 Contacto

Para dudas o problemas:
- Revisar `NOTAS_TECNICAS.md` para problemas comunes
- Verificar conexiones según esquema en `README.md`
- Medir señales con multímetro/osciloscopio
- Revisar configuración de CubeMX

---

**¡Buena suerte con las pruebas!** 🎮

Este proyecto es la base para el juego completo de tateti. Una vez funcionando, tendrás validado todo el hardware necesario.
