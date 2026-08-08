# Carbon 
<img width="1920" height="1080" alt="4A5A74" src="https://github.com/user-attachments/assets/939e52b6-d05a-461b-bc6b-7dc3b4fdc2b5" />



## Why did I build this 
As we know my nations air quality is pretty bad therefore I decided to make a air quality analyser to know the actual depth of this danger specially for my home 

## How to Use this 
First connect this device via usb-c cable to your pc then download the firmware files `Firmware\BUILD` or build them using the ino file in `firmware` wait for sometime as the BME688 needs sometime to stabilize its reading then after that the neopixels will show the color according to the IAQ of your enviroiment (RED = BAD , GREEN = GOOD , AMBER= MODERATE)

*Note the firmware is untested

## PCB Layout

| PCB Layout | Schematic / Wiring |
| :---------: | :----------------: |
| <img src="https://github.com/user-attachments/assets/3a73baec-db82-475f-9381-0920083d8412" width="450"> | <img src="https://github.com/user-attachments/assets/8f24f5a3-780d-40a4-96aa-19cd69a7d5e3" width="450"> |

## PCB Render

| Front | Rear |
| :---: | :--: |
| <img src="https://github.com/user-attachments/assets/26b3396d-0149-4536-a438-afd6405cd3e6" width="450"> | <img src="https://github.com/user-attachments/assets/aec25729-f452-4f14-8cd8-dd5905528815" width="450"> |

## Case

<p align="center">
  <img src="https://github.com/user-attachments/assets/283e111b-1747-4577-9236-17f08d73c3b3" width="700">
</p>




---

## BOM

| Item | Description | Quantity | Unit Price ($) | Total Price ($) | URL |
|------|-------------|----------|----------------|-----------------|-----|
| PCBA | Custom PCB Manufacturing (JLCPCB) | 1 | 49.35 | 49.35 | https://jlcpcb.com |
| Case | 3D Printed Case (Will be Printed Myself | 1 | 0.00 | 0.00 | https://hackclub.com |
| ESP-32 C6 Mini-1 | MCU |1 | 4.47 | 4.47 |  https://www.digikey.in/en/products/detail/espressif-systems/ESP32-C6-MINI-1-N4/17885796 |
| BME688 | VOC sensor | 1 | 9 |  9  | https://www.digikey.in/en/products/detail/bosch-sensortec/BME688/13681261|
| **TOTAL** |  |  |  | **63.00** |  |

# JLCPCB and Shipping
<img width="1900" height="830" alt="image" src="https://github.com/user-attachments/assets/d32cf386-7a96-4791-8536-f676ec33d5e3" />
<img width="1372" height="816" alt="Screenshot 2026-07-23 211038" src="https://github.com/user-attachments/assets/adb2f4d5-dff7-4a3e-8176-7249c7130f7c" />



---

## BOM PCBA 
# BOM (Bill of Materials)

| Comment | Designator | Footprint | LCSC | Quantity |
|---|---|---|---|---:|
| 0.1uF | C13, C15, C3, C4, C5, C7, C8 | C_0603_1608Metric | C125205 | 7 |
| 10K | R1, R2 | R_0603_1608Metric | C15401 | 2 |
| 10uF | C10, C12, C14, C9 | C_0603_1608Metric | C19702 | 4 |
| 1uF | C11 | C_0603_1608Metric | C15849 | 1 |
| 220Ω | R5 | R_0603_1608Metric | C23179 | 1 |
| 4.7kΩ | R10, R9 | R_0603_1608Metric | C15401 | 2 |
| 47uF | C6 | C_0603_1608Metric | C140782 | 1 |
| 5.1kΩ | R6, R7 | R_0603_1608Metric | C23186 | 2 |
| 74AHCT1G125SE-7 | U7 | SOT-353_L2.2-W1.4-P0.65-LS2.4-BR | C151417 | 1 |
| AP2112K-3.3TRG1 | U5 | SOT-25-5_L2.9-W1.6-P0.95-LS2.8-BL | C51118 | 1 |
| AP2120N-1.8TRG1 | U4 | SOT-23_L2.9-W1.3-P1.90-LS2.4-BR | C460317 | 1 |
| BME688 | U6 | LGA-8_L3.0-W3.0-P0.80_BME688 | C3664478 | 1 |
| ENS210 | U1 | AMS_QFN-4-1EP_2x2mm_P0.95mm_EP0.7x1.6mm | C2991202 | 1 |
| ESP32-C6-MINI-1 | U3 | ESP32-C6-MINI-1 | C5736265 | 1 |
| SW_Push | Boot1, Reset1 | SW_SPST_PTS647_Sx38 | C720477 | 2 |
| USB-C Receptacle USB2.0 14P | J1 | USB_C_Receptacle_HRO_TYPE-C-31-M-12 | C5143397 | 1 |
| SK6812MINI | D1 | LED_SK6812MINI_PLCC4_3.5x3.5mm_P1.75mm | — | 1 |
| SK6812MINI | D2 | LED_SK6812MINI_PLCC4_3.5x3.5mm_P1.75mm | — | 1 |
