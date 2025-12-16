# Tateti - Juego de Ta-Te-Ti con Statecharts

Implementación de un juego de Ta-Te-Ti para STM32F439ZI con control mediante statecharts de ITEMIS CREATE, visualización en matriz de LEDs WS2812B y modo de juego contra IA con 3 niveles de dificultad.

## 🎮 Características

- **Control por Statechart**: Lógica del juego implementada con ITEMIS CREATE
- **Visualización LED**: Matriz de 16 LEDs WS2812B (4x4) para tablero y marcadores
- **Modos de juego**: 
  - Jugador vs Jugador (PvP)
  - Jugador vs IA (PvIA) con 3 niveles de dificultad
- **Selección de colores**: Personalización de colores para cada jugador
- **Sistema de puntuación**: Partidas al mejor de 3 victorias
- **Entrada**: Teclado matricial 4x4

## 🔧 Hardware

- **Microcontrolador**: STM32F439ZI (Nucleo-144)
- **Display**: 16 LEDs WS2812B (matriz 4x4)
- **Entrada**: Teclado matricial 4x4
- **Comunicación LEDs**: TIM2_CH1 + DMA (PA0)

### Conexiones

| Periférico | Pin STM32 | Descripción |
|------------|-----------|-------------|
| WS2812B DIN | PA0 (TIM2_CH1) | Señal de datos para LEDs |
| Teclado Filas | PF15, PG14, PG9, PE13 | Salidas para escaneo |
| Teclado Columnas | PF14, PE11, PE9, PF13 | Entradas con pull-down |

## 🎯 Cómo Jugar

### Controles (Estado IDLE - Selección)

| Tecla | Función |
|-------|---------|
| **P11** | Cambiar modo: PvP ↔ PvIA |
| **P3** | Cambiar color Jugador 1 |
| **P7** | Cambiar color Jugador 2 |
| **P0** | Dificultad Fácil (solo modo IA) |
| **P1** | Dificultad Media (solo modo IA) |
| **P2** | Dificultad Difícil (solo modo IA) |
| **P15** | Comenzar partida |

### Controles (Durante el Juego)

**Tablero 3x3:**
```
[P4 ] [P5 ] [P6 ]
[P8 ] [P9 ] [P10]
[P12] [P13] [P14]
```

| Tecla | Función |
|-------|---------|
| **P15** | Reset del juego completo |

### Indicadores Visuales

- **LED de turno**: Indica qué jugador debe mover (se ilumina con el color del jugador activo)
- **Modo de juego**: LED de turno blanco tenue = modo IA activo
- **Nivel de dificultad**: Todo el tablero se ilumina al seleccionar:
  - Verde: Fácil (movimientos aleatorios)
  - Naranja: Medio (bloquea y busca ganar)
  - Rojo: Difícil (algoritmo Minimax, invencible)
- **Puntuación**: LEDs laterales muestran partidas ganadas (máximo 3)

## 🏗️ Arquitectura del Software

### Statechart (Yakindu)

El flujo del juego está controlado por un statechart con 6 estados principales:

1. **Entry**: Inicialización
2. **IDLE**: Selección de colores y configuración
3. **PLAYING**: Partida en curso, validación de movimientos
4. **Check_win**: Verificación automática de victoria/empate
5. **Match_end**: Animación de victoria de partida
6. **Game_over**: Fin del juego (alguien ganó 3 partidas)

**Archivo**: `tateti/tateti.ysc`

### Módulos Principales

```
Core/
├── Inc/
│   ├── tateti.h              # Statechart generado (API)
│   ├── game_logic.h          # Lógica del juego (validación, detección de victoria)
│   ├── display.h             # Control de LEDs WS2812B
│   ├── keyboard.h            # Driver teclado matricial
│   ├── ai.h                  # Inteligencia artificial (3 niveles)
│   ├── color_manager.h       # Gestión de paletas de colores
│   └── ws2812b.h             # Driver WS2812B (TIM2+DMA)
└── Src/
    ├── tateti.c              # Statechart generado (lógica)
    ├── tateti_glue.c         # Mapeo de operaciones del statechart a funciones C
    ├── main.c                # Loop principal, inyección de eventos de IA
    ├── game_logic.c          # Implementación de reglas del juego
    ├── display.c             # Renderizado de tablero, animaciones
    ├── keyboard.c            # Escaneo de teclado con anti-rebote
    ├── ai.c                  # Algoritmos de IA (aleatorio, heurístico, minimax)
    ├── color_manager.c       # Ciclo de colores para jugadores
    └── ws2812b.c             # Control de LEDs por PWM+DMA
```

## 🤖 Niveles de IA

### Fácil (Verde)
Elige posiciones aleatorias disponibles.

### Medio (Naranja) - Por defecto
Implementa estrategia heurística:
1. Intenta ganar si puede
2. Bloquea victorias del oponente
3. Prefiere el centro
4. Toma esquinas
5. Toma posiciones restantes

### Difícil (Rojo)
Algoritmo **Minimax** completo. Juega de forma óptima, imposible de vencer (solo empate si el oponente juega perfecto).

## 📝 Notas de Diseño

- **Separación de responsabilidades**: El statechart solo maneja el flujo, la lógica está en módulos independientes
- **Eventos vs Completion Transitions**: Se usa `tateti_trigger_without_event()` para procesar transiciones automáticas
- **IA externa al statechart**: La IA inyecta eventos como si fueran teclas del usuario
- **Anti-rebote por software**: 30ms de delay en escaneo de teclado
- **Empates no cuentan**: Solo las victorias suman puntos (reglas tradicionales de tateti)
