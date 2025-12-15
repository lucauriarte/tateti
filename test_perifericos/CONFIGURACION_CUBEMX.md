# Guía Rápida de Configuración en STM32CubeMX

Esta guía te ayudará a configurar el proyecto desde cero usando STM32CubeMX para el test de matriz de pulsadores 4x4 + matriz LEDs WS2812B.

## Paso 1: Crear Nuevo Proyecto

1. Abrir STM32CubeMX
2. `File` → `New Project`
3. En el selector:
   - MCU/MPU Selector: Buscar `STM32F439ZIT6`
   - O Board Selector: `NUCLEO-F439ZI`
4. Click en `Start Project`

## Paso 2: Configuración del Reloj (Clock Configuration)

1. Ir a la pestaña `Clock Configuration`
2. **Usar la configuración predeterminada** (recomendado):
   ```
   HSE: 8 MHz (BYPASS mode - del ST-Link)
   PLL Source: HSE
   PLLM: /4
   PLLN: ×168
   PLLP: /2
   → HCLK: 168 MHz
   APB1 Prescaler: /4 → 42 MHz
   APB2 Prescaler: /2 → 84 MHz
   ```

**Nota**: El STM32F439 puede ir hasta 180 MHz pero requiere configuración adicional (over-drive). La configuración de 168 MHz funciona perfectamente y es más estable.

## Paso 3: Configuración de Pines (Pinout & Configuration)

### GPIO - Matriz de Pulsadores 4x4

**Nota**: Los pulsadores son genéricos (sin números ni caracteres), dispuestos en matriz 4×4.

**Orden de cables de la matriz física**: C4, C3, C2, C1, F1, F2, F3, F4
**Orden de pines en CN10**: PF13, PE9, PE11, PF14, PE13, PF15, PG14, PG9 (consecutivos)

**Filas (Outputs)**:
- PE13: `GPIO_Output`
  - User Label: `ROW0` (Cable F1)
  - GPIO mode: Output Push Pull
  - Pull-up/Pull-down: No pull
  - Initial Output Level: Low
  - Conector: CN10 (posición 5)
  
- PF15: `GPIO_Output` → User Label: `ROW1` (Cable F2) - CN10 (posición 6)
- PG14: `GPIO_Output` → User Label: `ROW2` (Cable F3) - CN10 (posición 7)
- PG9: `GPIO_Output` → User Label: `ROW3` (Cable F4) - CN10 (posición 8)

**Columnas (Inputs con pull-down externas 10kΩ)**:
- PF14: `GPIO_Input`
  - User Label: `COL0` (Cable C1)
  - GPIO mode: Input
  - Pull-up/Pull-down: No pull
  - Conector: CN10 (posición 4)
  
- PE11: `GPIO_Input` → User Label: `COL1` (Cable C2) - CN10 (posición 3)
- PE9: `GPIO_Input` → User Label: `COL2` (Cable C3) - CN10 (posición 2)
- PF13: `GPIO_Input` → User Label: `COL3` (Cable C4) - CN10 (posición 1)

**Mapeo Cable → Pin**:
- Cable 1 (C4) → PF13 → COL3
- Cable 2 (C3) → PE9 → COL2
- Cable 3 (C2) → PE11 → COL1
- Cable 4 (C1) → PF14 → COL0
- Cable 5 (F1) → PE13 → ROW0
- Cable 6 (F2) → PF15 → ROW1
- Cable 7 (F3) → PG14 → ROW2
- Cable 8 (F4) → PG9 → ROW3

**Ventaja**: Todos los pines consecutivos en CN10, conexión directa sin cruzar cables.

**LED Onboard**:
- PB0: `GPIO_Output`
  - User Label: `LED_GREEN`
  - Output Level: Low
  - Conector: CN10 pin 31

### TIM2 - PWM para WS2812B

**Nota**: Usaremos PA0 que es TIM2_CH1, disponible en CN10 pin 29.

1. En `Timers` → `TIM2`:
   - Clock Source: `Internal Clock`
   - Channel1: `PWM Generation CH1`

2. En `Parameter Settings`:
   ```
   Prescaler: 0
   Counter Mode: Up
   Counter Period (ARR): 104
   Internal Clock Division: No Division
   Auto-reload preload: Enable
   ```
   **Nota**: Con APB1 Timer Clock = 84 MHz (APB1=42 MHz ×2), ARR=104 da ~800 kHz (84M/105 = 800kHz)

3. En `PWM Generation Channel 1`:
   ```
   Mode: PWM mode 1
   Pulse (CCR1): 0
   Output compare preload: Enable
   Fast Mode: Disable
   CH Polarity: High
   ```

4. En `GPIO Settings`:
   - PA0 debe aparecer como `TIM2_CH1`
   - Mode: Alternate Function Push Pull
   - Speed: Very High
   - Conector: CN10 pin 29

5. **DMA Settings** (muy importante):
   - Click en `Add` para agregar DMA request
   - DMA Request: `TIM2_CH1`
   - Stream: `DMA1 Stream 5`
   - Channel: `3`
   - Direction: `Memory To Peripheral`
   - Priority: `High`
   - Mode: `Normal`
   - Data Width:
     - Peripheral: `Half Word`
     - Memory: `Half Word`
   - Increment Address:
     - Peripheral: ☐ (deshabilitado)
     - Memory: ☑ (habilitado)

6. **NVIC Settings**:
   - ☑ `DMA1 stream5 global interrupt`

### TIM6 - Timer Base para Teclado

1. En `Timers` → `TIM6`:
   - Activated: ☑

2. En `Parameter Settings`:
   ```
   Prescaler: 8399
   Counter Mode: Up
   Counter Period (ARR): 9
   Auto-reload preload: Disable
   ```
   Esto genera una interrupción cada 1ms (84MHz / 8400 / 10 = 1kHz)

3. **NVIC Settings**:
   - ☑ `TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts`

## Paso 4: Project Manager

1. Ir a pestaña `Project Manager`
2. Configurar:
   ```
   Project Name: test_basico
   Project Location: C:\Users\lucah\Documents\git\tateti-1\
   Toolchain / IDE: STM32CubeIDE
   ```

3. En `Code Generator`:
   - ☑ `Copy only the necessary library files`
   - ☑ `Generate peripheral initialization as a pair of '.c/.h' files per peripheral`

## Paso 5: Generar Código

1. Click en `GENERATE CODE` (o `Alt+K`)
2. Abrir proyecto en STM32CubeIDE cuando termine
3. El IDE se abrirá automáticamente

## Paso 6: Integrar Drivers Personalizados

1. Copiar archivos:
   ```
   ws2812b.h → Core/Inc/
   ws2812b.c → Core/Src/
   keyboard.h → Core/Inc/
   keyboard.c → Core/Src/
   ```

2. Reemplazar `main.c` con el proporcionado

3. Actualizar includes en `main.c`:
   ```c
   /* USER CODE BEGIN Includes */
   #include "ws2812b.h"
   #include "keyboard.h"
   /* USER CODE END Includes */
   ```

## Paso 7: Modificar stm32f4xx_it.c

Agregar estas funciones (o usar el archivo proporcionado):

```c
/* USER CODE BEGIN 1 */
extern TIM_HandleTypeDef htim6;
/* USER CODE END 1 */

void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}
```

## Paso 8: Build y Flash

1. Build: `Project` → `Build All` (Ctrl+B)
2. Verificar que compila sin errores
3. Connect Nucleo board via USB
4. Flash: `Run` → `Run` (Ctrl+F11)

## Verificación de Configuración

### Check List de Pines
- [ ] PE13, PF15, PG14, PG9 configurados como GPIO Output (filas matriz: F1, F2, F3, F4)
- [ ] PF14, PE11, PE9, PF13 configurados como GPIO Input (columnas matriz: C1, C2, C3, C4)
- [ ] PA0 configurado como TIM2_CH1 (Alternate Function, Very High Speed, CN10 pin 29)
- [ ] PB0 configurado como GPIO Output (LED onboard)
- [ ] USART6 deshabilitado (evita conflicto con PG14)

### Check List de Timers
- [ ] TIM2 en modo PWM CH1, PSC=0, ARR=104 (APB1 Timer @ 84 MHz)
- [ ] TIM2 con DMA habilitado (DMA1 Stream 5, Channel 3)
- [ ] TIM6 activado, PSC=8399, ARR=9 (interrupt cada 1 ms)
- [ ] TIM6 con NVIC interrupt habilitado

### Check List de Clock
- [ ] HCLK = 168 MHz
- [ ] APB1 = 42 MHz (timers × 2 = 84 MHz)
- [ ] APB2 = 84 MHz
