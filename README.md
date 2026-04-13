# TM_EE_BMS

## Purpose
Arduino-based BMS monitor that measures:
- Pack voltage
- Cell voltage
- Pack temperature

Outputs:
- Green LED → healthy battery
- Red LED → fault (temperature / combined)
- Serial → diagnostics

**Note:** Monitoring only (no protection/cutoff).

---

## Inputs / Outputs

### Inputs
- A0 → pack voltage  
- A2 → cell voltage  
- A1 → thermistor  

### Outputs
- D2 → green LED  
- D9 → red LED  
- Serial → data + warnings  

---

## Core Logic

### Battery Life (SoC)
- 2.75 V → 0%  
- 4.2 V → 100%  

battery_life = ((cell_voltage - 2.75) / (4.2 - 2.75)) * 100;

### Temperature
- Beta model thermistor  
- Fault if |temp| > 55 °C  

---

## States

### Normal
- battery_life > 50  
- |temp| ≤ 55  
→ Green ON, Red OFF  

### Low Battery
- battery_life < 10  
→ Serial: "LOW BATTERY!!!"  

### Over-Temp
- |temp| > 55  
→ Red blinking (slow)  

### Double Fault
- battery_life < 10 AND |temp| > 55  
→ Red blinking (fast)  

### Invalid
- battery_life < 0  
→ "Wrong Battery??"  

---

## Hardware Assumptions

### Voltage Dividers
- R1 = 14.7kΩ  
- R2 = 10kΩ  

### Thermistor
- R1_temp = 2.22kΩ  
- B = 3935  
- Reference ≈ 1kΩ  

Update code if hardware changes.

---

## Testing Approach (Modular)

Test each subsystem separately.

### 1. Voltage
- Inject known voltage  
- Compare Serial vs multimeter  
- Adjust R1/R2 if needed  

### 2. Temperature
- Replace thermistor with known resistor  
- Compare computed temp vs expected  

### 3. Logic
Override values in code:

cell_voltage = 4.0;
pack_temp = 25;

Check LED + Serial behavior.

---

## Integration Test
- Sweep voltage (2.5–4.2 V equivalent)  
- Sweep temperature  
- Verify LED transitions and stability  

---

## Improvements

Centralize constants:

struct BMS_Config {
  float R1_cell, R2_cell;
  float R1_pack, R2_pack;
  float R1_temp, B_val;
};

Split functions:
- read_voltage()
- compute_temp()
- compute_soc()
- update_outputs()

Add test mode (inject values via Serial)  
Allow live calibration (avoid recompiling for resistor tweaks)
