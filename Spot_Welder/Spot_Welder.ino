#define LED_5V_PIN   9  //PB_1
#define LED_7V5_PIN  10 //PB_2
#define LED_10V_PIN  11 //PB_3
#define LED_12V5_PIN 12 //PB_4

#define V_SENSE_PIN    A0
#define LEAD_SENSE_PIN A1

#define BUZZER_PIN      4 //PD_4
#define PULSE_PIN       5 //PD_5
#define EXT_TRIG_PIN    6 //PD_6
#define LEAD_V_TRIG_PIN 7 //PD_7

#define ADC_MAV_mV    8220
#define ADC_MAX_READ 417
#define ADC_MIN_mV    0
#define ADC_MIN_READ 0

#define BEEP_THRESHOLD_mV         12000
#define PULSE_DURATION_mS         100
#define MIN_mS_BETWEEN_TRIGGER    1000
#define LEAD_TRIGGER_THRESHOLD_mV 5000
#define LEAD_TRIGGER_HOLD_mS      1000

bool voltage_low;
uint32_t last_trigger = 0;
uint32_t hold_time = 0;

void outputLedVoltage(int value);
int mapVoltage(int value);
void beep();
void trigger();

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
delay(10);
pinMode(LED_5V_PIN, OUTPUT);
pinMode(LED_7V5_PIN, OUTPUT);
pinMode(LED_10V_PIN, OUTPUT);
pinMode(LED_12V5_PIN, OUTPUT);

pinMode(BUZZER_PIN, OUTPUT);
pinMode(PULSE_PIN, OUTPUT);

pinMode(V_SENSE_PIN, INPUT);
pinMode(LEAD_SENSE_PIN, INPUT);
pinMode(EXT_TRIG_PIN, INPUT);
pinMode(LEAD_V_TRIG_PIN, INPUT_PULLUP);

delay(10);
Serial.println("Initialized");
}

void loop() {
  int v_voltage    = mapVoltage(analogRead(V_SENSE_PIN));
  int lead_voltage = mapVoltage(analogRead(LEAD_SENSE_PIN));

  outputLedVoltage(v_voltage);
  if((v_voltage >= BEEP_THRESHOLD_mV) && voltage_low)
  {
    beep();
    voltage_low = 0;
  }
  else if (v_voltage < BEEP_THRESHOLD_mV)
  {
    voltage_low = 1;
  }

  if(digitalRead(EXT_TRIG_PIN) && (millis()-last_trigger)>MIN_mS_BETWEEN_TRIGGER)
  {
    trigger();
    Serial.println("Trigger");
    last_trigger = millis();
  }

  

  if(digitalRead(LEAD_V_TRIG_PIN) && (lead_voltage<500))
  {
    if((millis()-hold_time) > LEAD_TRIGGER_HOLD_mS)
    {
      trigger();
      hold_time = millis();
    }
  }
  else
  {
    hold_time = millis();
  }
  
  delay(10);

}

void outputLedVoltage(int value)
{
  if(value >= 5000) digitalWrite(LED_5V_PIN, HIGH);
  else digitalWrite(LED_5V_PIN, LOW);

  if(value >= 7500) digitalWrite(LED_7V5_PIN, HIGH);
  else digitalWrite(LED_7V5_PIN, LOW);

  if(value >= 10000) digitalWrite(LED_10V_PIN, HIGH);
  else digitalWrite(LED_10V_PIN, LOW);

  if(value >= 12500) digitalWrite(LED_12V5_PIN, HIGH);
  else digitalWrite(LED_12V5_PIN, LOW);
}

int mapVoltage(int value)
{
  return map(value, ADC_MIN_READ, ADC_MAX_READ, ADC_MIN_mV, ADC_MAV_mV);
}

void beep()
{
  tone(BUZZER_PIN, 5000, 100);
}

void trigger()
{
  uint32_t count = millis();
  while(millis()-count<PULSE_DURATION_mS)   
  {
    digitalWrite(PULSE_PIN, HIGH);
  }
  digitalWrite(PULSE_PIN, LOW);
}

