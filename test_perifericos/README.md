# Test Básico - Matriz de Pulsadores 4x4 + Matriz LEDs WS2812B

## Descripción
Proyecto de prueba para validar las interfaces de hardware del juego de Tateti:
- **Matriz de pulsadores 4x4** con anti-rebote por software
- **Matriz de LEDs WS2812B 4x4** controlada por PWM+DMA

**Funcionalidad**: Al presionar un pulsador de la matriz, se enciende el LED correspondiente en la matriz WS2812B con un color específico.

## Hardware Requerido
- STM32 Nucleo-144 (STM32F439ZIT6)
- Matriz de 16 pulsadores (disposición 4x4) con resistencias pull-down de 10kΩ en columnas
- Matriz de LEDs WS2812B 4x4 (16 LEDs)
- Level shifter 3.3V → 5V para señal de datos del WS2812B
- Fuente externa 5V para alimentar la matriz de LEDs

## Conexiones de Hardware

### Matriz de Pulsadores 4x4 (Todos en CN10, consecutivos)
| Cable | Función | Pin STM32 | Conector | Resistencia |
|-------|---------|-----------|----------|-------------|
| Cable 1 | C4 (COL3) | PF13    | CN10     | 10kΩ pull-down |
| Cable 2 | C3 (COL2) | PE9     | CN10     | 10kΩ pull-down |
| Cable 3 | C2 (COL1) | PE11    | CN10     | 10kΩ pull-down |
| Cable 4 | C1 (COL0) | PF14    | CN10     | 10kΩ pull-down |
| Cable 5 | F1 (ROW0) | PE13    | CN10     | -           |
| Cable 6 | F2 (ROW1) | PF15    | CN10     | -           |
| Cable 7 | F3 (ROW2) | PG14    | CN10     | -           |
| Cable 8 | F4 (ROW3) | PG9     | CN10     | -           |

**Orden físico en CN10**: PF13 → PE9 → PE11 → PF14 → PE13 → PF15 → PG14 → PG9 (consecutivos)
**Orden cables matriz**: C4 → C3 → C2 → C1 → F1 → F2 → F3 → F4

**Distribución física** (vista desde arriba, con columnas invertidas por hardware):
```
Matriz de Pulsadores:       Mapeo interno (KEY_x):
[P0] [P1] [P2] [P3]    →   [1] [2] [3] [A]
[P4] [P5] [P6] [P7]    →   [4] [5] [6] [B]
[P8] [P9] [P10][P11]   →   [7] [8] [9] [C]
[P12][P13][P14][P15]   →   [*] [0] [#] [D]
```

### Matriz WS2812B 4x4
| Función     | Pin STM32 | Conector Nucleo | Notas                    |
|-------------|-----------|-----------------|--------------------------|
| Data        | PD15      | CN7-18          | Via level shifter 3.3→5V |
| VCC (5V)    | -         | Fuente externa  | NO conectar a Nucleo     |
| GND         | GND       | CN7/CN10 GND    | Común con Nucleo         |

## Configuración del Proyecto en STM32CubeIDE

### Opción 1: Importar Proyecto Existente (Recomendado)

1. **Abrir STM32CubeIDE**

2. **Importar proyecto**:
   - `File` → `Import...`
   - `General` → `Existing Projects into Workspace`
   - `Next`
   - `Select root directory`: Navegar a `tateti-1/test_basico`
   - Marcar el proyecto `test_basico`
   - `Finish`

3. **Configurar HAL Drivers**:
   - Click derecho en el proyecto → `Properties`
   - `C/C++ Build` → `Settings`
   - `MCU GCC Compiler` → `Include paths`
   - Agregar (si no están):
     - `../Drivers/STM32F4xx_HAL_Driver/Inc`
     - `../Drivers/CMSIS/Device/ST/STM32F4xx/Include`
     - `../Drivers/CMSIS/Include`

4. **Agregar archivos HAL** (si es necesario):
   - Copiar la carpeta `Drivers/` de otro proyecto STM32F439ZI
   - O generar desde STM32CubeMX (ver Opción 2)

5. **Build y Flash**:
   - `Project` → `Build All` (Ctrl+B)
   - `Run` → `Debug` (F11) o `Run` (Ctrl+F11)

### Opción 2: Crear Proyecto desde Cero con CubeMX

Si prefieres configurar manualmente:

1. **Crear nuevo proyecto**:
   - `File` → `New` → `STM32 Project`
   - Buscar `STM32F439ZIT6`
   - Board: `NUCLEO-F439ZI`
   - `Next` → Nombre: `test_basico` → `Finish`

2. **Configurar Clock**:
   - En CubeMX: `Clock Configuration`
   - Input frequency: 8 MHz (HSE)
   - HCLK (SYSCLK): 168 MHz
   - APB1: 42 MHz (Timer Clock: 84 MHz con multiplicador ×2)
   - APB2: 84 MHz (Timer Clock: 168 MHz con multiplicador ×2)

3. **Configurar GPIO**:
   - **PC0-PC3**: GPIO_Output (Filas teclado)
   - **PA0, PA1, PA4, PA5**: GPIO_Input (Columnas teclado)
   - **PB7**: GPIO_Output (LED onboard)

4. **Configurar TIM4** (para WS2812B):
   - Mode: PWM Generation CH1
   - Pin: PD12 (TIM4_CH1)
   - Clock Source: Internal Clock
   - Prescaler: 0
   - Counter Period: 104 (ARR - AutoReload Register)
   - CH1 Mode: PWM mode 1
   - CH1 Pulse: 0
   - **DMA Settings**:
     - Add DMA Request: TIM4_CH1
     - DMA Request: DMA1 Stream 0 Channel 2
     - Mode: Normal
     - Priority: High
     - Data Width: Half Word to Half Word

5. **Configurar TIM6** (para keyboard):
   - Mode: Activated
   - Clock Source: Internal Clock
   - Prescaler: 8399 (84 MHz / 8400 = 10 kHz)
   - Counter Period: 9 (10 kHz / 10 = 1 kHz → 1 ms interrupt)
   - **NVIC Settings**: Marcar `TIM6 global interrupt`

6. **Generar código**:
   - `Project` → `Generate Code`

7. **Copiar archivos del driver**:
   - Copiar `ws2812b.c/h` y `keyboard.c/h` a las carpetas correspondientes
   - Reemplazar `main.c` con el proporcionado

8. **Build y Flash**

## Mapeo de Teclas a LEDs

El programa mapea las teclas a LEDs específicos con colores diferentes:

```
Teclado:              LEDs (índice lógico):
┌───┬───┬───┬───┐    ┌────────────────────────┐
│ 1 │ 2 │ 3 │ A │    │ LED 0  │ LED 1  │ LED 2  │ LED 3  │
├───┼───┼───┼───┤    │ Rojo   │ Naranja│ Amaril │ Rojo   │
│ 4 │ 5 │ 6 │ B │    ├────────┼────────┼────────┼────────┤
├───┼───┼───┼───┤    │ LED 4  │ LED 5  │ LED 6  │ LED 7  │
│ 7 │ 8 │ 9 │ C │    │ Verde  │ Cyan   │ Azul   │ Azul   │
├───┼───┼───┼───┤    ├────────┼────────┼────────┼────────┤
│ * │ 0 │ # │ D │    │ LED 8  │ LED 9  │ LED 10 │ LED 11 │
└───┴───┴───┴───┘    │ Violeta│ Magenta│ Blanco │ Verde  │
                     ├────────┼────────┼────────┼────────┤
                     │ LED 12 │ LED 13 │ LED 14 │ LED 15 │
                     │ Blanco │ Gris   │ Amaril │ Amaril │
                     └────────┴────────┴────────┴────────┘
```

## Funcionamiento

1. Al presionar una tecla, el LED correspondiente se enciende con su color asignado
2. Al presionar otra tecla, el LED anterior se apaga y se enciende el nuevo
3. El LED verde onboard (PB7) parpadea cada 500ms indicando que el programa está corriendo
4. El anti-rebote de 30ms elimina lecturas múltiples de una misma pulsación

## Verificación del Funcionamiento

### Test del Teclado
1. Presionar cada tecla secuencialmente (1-9, 0, A-D, *, #)
2. Verificar que solo se enciende un LED a la vez
3. Verificar que no hay lecturas múltiples (el LED no parpadea al presionar)

### Test de la Matriz WS2812B
1. Cada LED debe encenderse con el color correcto según la tabla
2. Los colores deben ser uniformes y sin parpadeos
3. Al soltar la tecla, el LED debe mantener su estado

### Indicadores de Error
- Si el LED onboard parpadea muy rápido: Error en la inicialización
- Si no enciende ningún LED WS2812B: 
  - Verificar conexión del level shifter
  - Verificar alimentación 5V de la matriz
  - Verificar que GND es común
- Si el teclado no responde:
  - Verificar conexiones de filas y columnas
  - Verificar resistencias pull-down en columnas

## Archivos del Proyecto

```
test_basico/
├── Core/
│   ├── Inc/
│   │   ├── main.h              # Header principal
│   │   ├── stm32f4xx_it.h      # Handlers de interrupciones
│   │   ├── ws2812b.h           # Driver WS2812B
│   │   └── keyboard.h          # Driver teclado
│   └── Src/
│       ├── main.c              # Programa principal
│       ├── stm32f4xx_it.c      # ISRs
│       ├── ws2812b.c           # Implementación WS2812B
│       └── keyboard.c          # Implementación teclado
├── Drivers/                    # HAL y CMSIS (generados por CubeMX)
└── README.md                   # Este archivo
```

## Características Técnicas

### Driver WS2812B
- **Protocolo**: PWM + DMA para timing preciso
- **Timing**: 800kHz (1.25µs por bit)
  - Bit 0: 0.4µs HIGH, 0.85µs LOW
  - Bit 1: 0.8µs HIGH, 0.45µs LOW
- **Buffer**: 16 LEDs × 24 bits + 60 bits de reset
- **Formato**: GRB (Green-Red-Blue)

### Driver Teclado
- **Escaneo**: Por filas, activando una a la vez
- **Anti-rebote**: 30ms por software
- **Frecuencia de actualización**: 1 kHz (cada 1ms)
- **Detección**: Solo genera evento al presionar (no al soltar)

