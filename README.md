# TTK4155 – Ping Pong Game (Embedded Systems Term Project 2024)

**Course:** TTK4155 – Embedded and Industrial Computer Systems  
**Group:** #45, Bench #22  
**University:** NTNU

---

## Project Overview

This repository contains the firmware for a two-node embedded ping pong game built as the term project for TTK4155.  
A joystick and two sliders on **Node 1** (ATmega162) control a servo and a DC motor on **Node 2** (SAM3X8E / Arduino Due). A solenoid punch mechanism allows the player to hit the ball. The two nodes communicate over a **CAN bus**. An OLED display on Node 1 shows a navigable menu, and hit counts are transmitted back from Node 2 via CAN.

> **Note:** The PIC controller portion of the project specification was **not implemented**. All other subsystems described below were completed and verified.

---

## Hardware

| Component | Role |
|-----------|------|
| **ATmega162** (Node 1) | User interface controller |
| **MAX233** | RS-232 / UART level shifter on Node 1 |
| **MC7805** | 5 V voltage regulator on Node 1 |
| **MCP2515** | SPI-connected CAN controller on Node 1 |
| **External ADC (MAX156-compatible)** | Reads joystick (X/Y) and two sliders, mapped to address `0x1400` |
| **SSD1306 OLED display** | Displays menu on Node 1 via SPI |
| **SAM3X8E / Arduino Due** (Node 2) | Actuator controller |
| **Servo motor** | Controlled via PWM on PB13 (Node 2) |
| **DC motor** | Controlled via PWM on PB12 + direction pin PC23 (Node 2) |
| **Solenoid (punch mechanism)** | Triggered via PA10 (Node 2) |
| **Quadrature encoder** | Position feedback via TC2 / PC25–PC26 (Node 2) |

---

## Repository Structure

```
TTK4155-Ping-Pong-Game-LAB/
│
├── lab#1/
│   └── GccApplication1/           # Node 1 – ATmega162 main project
│       └── GccApplication1/
│           ├── main.c             # Top-level: UART, ADC, OLED, CAN, buttons
│           ├── oled.c / oled.h    # OLED low-level driver (SSD1306)
│           ├── fonts.c / fonts.h  # Font bitmaps for OLED
│           ├── User_Interface.c   # Menu display logic
│           ├── spi.c / spi.h      # SPI master driver (ATmega162)
│           ├── MCP2515_Driver.c   # MCP2515 register-level driver
│           ├── CAN_driver.c       # CAN send/receive abstraction
│           └── pins.h             # Pin definitions
│
├── lab#2/
│   ├── 4155_lab2/                 # Standalone external SRAM test
│   │   └── 4155_lab2/
│   │       └── main.c             # SRAM write/read verification
│   └── vortex_uart/               # UART echo test on Vortex (AVR XMEGA) board
│       └── vortex_uart/
│           └── main.c             # Interrupt-driven UART receive + echo
│
└── Node2Project/                  # Node 2 – SAM3X8E (Arduino Due) project
    └── Node2Project/
        ├── main.c                 # Top-level: CAN rx/tx, servo, motor, solenoid
        ├── can.c / can.h          # Native SAM3X8E CAN driver
        ├── pwm_driver.c           # Servo PWM (PB13, 20 ms period)
        ├── control.c / control.h  # DC motor PWM, encoder, solenoid
        ├── ADC.c / ADC.h          # SAM3X8E ADC (hit counter, CH0)
        ├── uart.c / uart.h        # Debug UART (9600 baud)
        └── time.c / time.h        # Timer utilities
```

---

## System Architecture & Communication Flow

```
 ┌─────────────────────────────────────────────┐
 │               NODE 1  (ATmega162)            │
 │                                              │
 │  Joystick X/Y ──► External ADC (0x1400)     │
 │  Sliders L/R  ──► External ADC (0x1400)     │
 │  Buttons INT0 / INT1 / INT2                  │
 │  OLED display ◄── User_Interface / oled.c   │
 │                                              │
 │  CAN_driver ──► MCP2515 ◄──► SPI bus        │
 └──────────────────┬───────────────────────────┘
                    │  CAN bus (125 kbps)
 ┌──────────────────▼───────────────────────────┐
 │               NODE 2  (SAM3X8E)              │
 │                                              │
 │  can.c ──► RX message                        │
 │    byte[0] → Joystick Y → Servo PWM (PB13)  │
 │    byte[2] → Joystick X → DC motor (PB12)   │
 │    byte[1] → Button     → Solenoid (PA10)   │
 │                                              │
 │  ADC CH0 (hit counter) ──► TX back to Node 1│
 └──────────────────────────────────────────────┘
```

1. **Node 1** reads the joystick (X and Y axes) and button state via the external ADC (memory-mapped at `0x1400`).
2. A CAN frame (`id=1, len=3`) is assembled with joystick Y, button state, and joystick X, then sent via the MCP2515 at 125 kbps.
3. **Node 2** receives the frame through its native SAM3X8E CAN peripheral and drives:
   - **Servo** (PWM channel 1, 20 ms period, PB13) proportional to joystick Y.
   - **DC motor** (PWM channel 0 + direction pin PC23, PB12) proportional to joystick X.
   - **Solenoid punch** (PA10 pulled low momentarily) when the joystick button is pressed.
4. Node 2 reads ADC channel 0 (used as a hit counter) and transmits the result back to Node 1 via CAN.
5. Node 1 can display information on the OLED screen; the joystick navigates a 5-item menu.

---

## Sub-projects

### lab#2 – SRAM Test (`4155_lab2`)

A standalone program that verifies the 2 KB external SRAM wired to the ATmega162's external memory bus (starting at `0x1800`). It:
1. Writes pseudo-random values and immediately reads them back (**write phase**).
2. Re-seeds the PRNG with the same seed and reads all values again (**retrieval phase**).
3. Reports the number of mismatches over UART.

### lab#2 – UART Echo (`vortex_uart`)

Runs on an AVR XMEGA (Vortex) evaluation board. It configures USART3 at 9600 baud with a receive-complete interrupt and echoes every received byte back to the sender. This was used to verify the serial communication link before integrating Node 1.

---

## Building & Flashing

Both ATmega162 projects (`lab#1`, `lab#2`) and the SAM3X8E project (`Node2Project`) use **Microchip Studio (Atmel Studio)** solution files (`.atsln`).

1. Open the desired `.atsln` file in Microchip Studio.
2. Select the correct target device and toolchain (AVR-GCC for ATmega162 / AVR XMEGA, ARM-GCC for SAM3X8E).
3. Build the project (**F7**).
4. Flash using the connected programmer/debugger (e.g., Atmel-ICE, J-Link, or the SAM-BA bootloader for the Arduino Due).

---

## Key Peripheral Configurations

| Peripheral | Node | Settings |
|------------|------|---------|
| UART | Node 1 (ATmega162) | 9600 baud, 8N1, interrupt-driven RX, `F_CPU = 4 915 200 Hz` |
| UART | Node 2 (SAM3X8E) | 9600 baud, `F_CPU = 84 MHz` |
| SPI | Node 1 | Master, `SPCR = SPE\|MSTR\|SPR0`, MCP2515 on PB4–PB7 (SS/MOSI/MISO/SCK) |
| CAN | Node 1 (MCP2515) | 125 kbps, CNF1 = 0xC3, CNF2 = 0xA9, CNF3 = 0x06 |
| CAN | Node 2 (native) | 125 kbps, BRP = 41, Phase1 = 5, Phase2 = 6, Prop = 1, SJW = 3 |
| Servo PWM | Node 2, PB13 | 1 MHz clock (DIVA = 84), period = 20 000 counts (20 ms), duty maps ±100 % joystick range |
| Motor PWM | Node 2, PB12 | 1 MHz clock, period = 50 counts, direction via PC23 |
| Encoder | Node 2 | TC2 quadrature decoder, PC25 (Phase A) / PC26 (Phase B) |
| External ADC | Node 1 | MAX156-compatible, 4 channels at base address `0x1400`; channels: left slider (0x80), right slider (0x81), joystick X (0x82), joystick Y (0x83) |
| Internal ADC | Node 2 | SAM3X8E ADC CH0, free-run mode, prescaler = 50 |

---

## What Was Not Implemented

- **PIC controller subsystem** – The PIC microcontroller portion described in the project specification was not completed. All other parts of the project (UART, SPI, OLED, ADC, CAN, servo, DC motor, solenoid, quadrature encoder) were implemented and tested successfully.
