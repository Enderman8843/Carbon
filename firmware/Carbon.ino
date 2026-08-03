#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME680.h>
#include <Adafruit_NeoPixel.h>

constexpr uint8_t PIN_SDA   = 0;
constexpr uint8_t PIN_SCL   = 1;
constexpr uint8_t PIN_LED   = 18;
constexpr uint8_t PIN_BOOT  = 9;
constexpr uint8_t LED_COUNT = 2;
constexpr uint8_t ENS210_ADDRESS = 0x43;
constexpr uint8_t BME688_ADDRESS = 0x76;
constexpr uint8_t ENS210_REG_PART_ID    = 0x00;
constexpr uint8_t ENS210_REG_SYS_CONTROL= 0x10;
constexpr uint8_t ENS210_REG_SENS_RUN   = 0x21;
constexpr uint8_t ENS210_REG_SENS_START = 0x22;
constexpr uint8_t ENS210_REG_TEMP_VALUE = 0x30;
constexpr uint8_t ENS210_REG_HUM_VALUE  = 0x33;

Adafruit_BME680   bme688;
Adafruit_NeoPixel led_strip(LED_COUNT, PIN_LED, NEO_GRB + NEO_KHZ800);

uint32_t warmup_duration_ms = 180000;
const float humidity_reference = 40.0f;
const float iaq_good_threshold = 65.0f;
const float iaq_medium_threshold = 35.0f;

float    gas_baseline = 0;
bool     is_warmed_up = false;
uint32_t warmup_start_time = 0;

#define CRC7_WIDTH 7
#define CRC7_POLY 0x89u
#define CRC7_INIT 0x7Fu
#define DATA_WIDTH 17
#define DATA_MASK ((1UL<<DATA_WIDTH)-1)
#define DATA_MSB  (1UL<<(DATA_WIDTH-1))

static uint32_t ens210_crc7(uint32_t value){
  uint32_t polynomial = CRC7_POLY<<(DATA_WIDTH-CRC7_WIDTH-1), bit = DATA_MSB;
  value<<=CRC7_WIDTH; bit<<=CRC7_WIDTH; polynomial<<=CRC7_WIDTH; value|=CRC7_INIT;
  while(bit&(DATA_MASK<<CRC7_WIDTH)){ if(bit&value) value^=polynomial; bit>>=1; polynomial>>=1; }
  return value;
}

static bool ens210_write_register(uint8_t register_address, uint8_t value){
  Wire.beginTransmission(ENS210_ADDRESS); Wire.write(register_address); Wire.write(value);
  return Wire.endTransmission()==0;
}

static bool ens210_read_register(uint8_t register_address, uint8_t* buffer, uint8_t length){
  Wire.beginTransmission(ENS210_ADDRESS); Wire.write(register_address);
  if(Wire.endTransmission(false)!=0) return false;
  if(Wire.requestFrom((int)ENS210_ADDRESS,(int)length)!=length) return false;
  for(uint8_t index=0; index<length; index++) buffer[index]=Wire.read();
  return true;
}
static bool ens210_begin(){
  ens210_write_register(ENS210_REG_SYS_CONTROL,0x80); delay(5);
  ens210_write_register(ENS210_REG_SYS_CONTROL,0x00); delay(5);
  uint8_t part_id[2]; if(!ens210_read_register(ENS210_REG_PART_ID,part_id,2)) return false;
  return (uint16_t)(part_id[0]|(part_id[1]<<8))==0x0210;
}
static bool ens210_measure(float& temperature_celsius, float& relative_humidity){
  ens210_write_register(ENS210_REG_SENS_RUN,0x00);
  ens210_write_register(ENS210_REG_SENS_START,0x03);
  delay(130);
  uint8_t temp_bytes[3], humidity_bytes[3];
  if(!ens210_read_register(ENS210_REG_TEMP_VALUE,temp_bytes,3)) return false;
  if(!ens210_read_register(ENS210_REG_HUM_VALUE,humidity_bytes,3)) return false;
  uint32_t temp_raw     = temp_bytes[0]|(temp_bytes[1]<<8)|((uint32_t)temp_bytes[2]<<16);
  uint32_t humidity_raw = humidity_bytes[0]|(humidity_bytes[1]<<8)|((uint32_t)humidity_bytes[2]<<16);
  bool temp_valid     = ((temp_raw>>16)&1)&&(ens210_crc7(temp_raw&0x1FFFF)==((temp_raw>>17)&0x7F));
  bool humidity_valid = ((humidity_raw>>16)&1)&&(ens210_crc7(humidity_raw&0x1FFFF)==((humidity_raw>>17)&0x7F));
  if(!temp_valid||!humidity_valid) return false;
  temperature_celsius = (float)(temp_raw&0xFFFF)/64.0f-273.15f;
  relative_humidity   = (float)(humidity_raw&0xFFFF)/512.0f;
  return true;
}
float compute_iaq(float gas_ohms, float humidity){
  float humidity_score;
  if(humidity < 38)       humidity_score = (humidity/humidity_reference)*25.0f;
  else if(humidity > 42)  humidity_score = ((100.0f - humidity)/(100.0f - humidity_reference))*25.0f;
  else                    humidity_score = 25.0f;
  if(humidity_score < 0)  humidity_score = 0;
  float gas_score = (gas_baseline > 0) ? (gas_ohms/gas_baseline)*75.0f : 0;
  if(gas_score > 75) gas_score = 75;
  if(gas_score < 0)  gas_score = 0;
  return humidity_score + gas_score;
}
static void i2c_scan(){
  Serial.println("I2C scan:");
  uint8_t found_count = 0;
  for(uint8_t address=1; address<127; address++){
    Wire.beginTransmission(address);
    if(Wire.endTransmission()==0){ Serial.printf("  0x%02X\n",address); found_count++; }
  }
  if(!found_count) Serial.println("  (nothing found)");
}
void setup(){
  Serial.begin(115200); delay(300);
  pinMode(PIN_BOOT,INPUT_PULLUP);

  led_strip.begin(); led_strip.setBrightness(40);
  led_strip.fill(led_strip.Color(0,0,8)); led_strip.show();

  Wire.begin(PIN_SDA,PIN_SCL,400000);
  i2c_scan();

  Serial.println(ens210_begin()?"ENS210 ok":"ENS210 FAIL");

  if(bme688.begin(BME688_ADDRESS)){
    bme688.setTemperatureOversampling(BME680_OS_8X);
    bme688.setHumidityOversampling(BME680_OS_2X);
    bme688.setPressureOversampling(BME680_OS_4X);
    bme688.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme688.setGasHeater(320,150);
    Serial.println("BME688 ok");
  } else {
    Serial.println("BME688 FAIL");
  }

  warmup_start_time = millis();
  Serial.println("Warming up gas sensro...");
}
void set_color(uint8_t red, uint8_t green, uint8_t blue){
  led_strip.fill(led_strip.Color(red,green,blue)); led_strip.show();
}
void loop(){
  float ens_temperature, ens_humidity;
  bool ens_ok = ens210_measure(ens_temperature, ens_humidity);
  bool bme_ok = bme688.performReading();

  Serial.println("----");
  if(ens_ok) Serial.printf("ENS210 : %.2f C  %.1f %%RH\n", ens_temperature, ens_humidity);
  if(bme_ok) Serial.printf("BME688 : %.2f C  %.1f %%RH  %.1f hPa  gas %.1f kOhm\n",
             bme688.temperature, bme688.humidity, bme688.pressure/100.0f, bme688.gas_resistance/1000.0f);

  if(!is_warmed_up){
    if(bme_ok && bme688.gas_resistance > gas_baseline) gas_baseline = bme688.gas_resistance;
    uint32_t seconds_left = (millis()-warmup_start_time < warmup_duration_ms)
                            ? (warmup_duration_ms-(millis()-warmup_start_time))/1000 : 0;
    Serial.printf("warming up... %lus left (baseline %.1f kOhm)\n",
                  (unsigned long)seconds_left, gas_baseline/1000.0f);
    set_color(15,15,15);
    if(seconds_left==0 && gas_baseline>0){ is_warmed_up=true; Serial.println("IAQ active."); }
    delay(2000);
    return;
  }

  if(bme_ok && bme688.gas_resistance > gas_baseline)
    gas_baseline = 0.9f*gas_baseline + 0.1f*bme688.gas_resistance;

  if(bme_ok){
    float iaq_score = compute_iaq(bme688.gas_resistance, bme688.humidity);
    const char* label;
    if(iaq_score >= iaq_good_threshold){ set_color(0,255,0);   label="GOOD  (green)"; }
    else if(iaq_score >= iaq_medium_threshold){ set_color(255,90,0); label="MEDIUM(amber)"; }
    else { set_color(255,0,0); label="BAD   (red)"; }
    Serial.printf("IAQ    : %.0f/100  -> %s\n", iaq_score, label);
  } else {
    set_color(64,0,0);
    Serial.println("IAQ    : BME read fail");
  }

  delay(2000);
}