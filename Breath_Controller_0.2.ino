#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>



bool DEBUG = true;                   // Will print values to serial monitor if set to true

const int LED0 = 2;                   // Red led to indicate pressure reading exceeds upper bound
const int LED1 = 3;                   // Green led to indicate pressure reading within lower and upper bound
const int midi_channel = 0;           // Sets the midi channel, 0 == channel 1
const int breath_controller = 2;      // Sets the midi control number

int pressure_val;                     // Stores pressure reading value
byte ccval;                           // Stores midi cc value to send in midi event
bool value_change = false;             //
int pot_reading;                      // Stores last potentiometer reading
int lower;                            // Stores lower bound for pressure reading to exclude ambient pressure
int upper;                            // Stores upper bound for pressure reaading which can be adjusted by potentiometer


int zero = 0;





void setup() {

  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  
  lower = analogRead(A0) + 5;         // determine ambient pressure to establish lower bound for subsequent pressure readings
  upper = analogRead(A1);
  Serial.begin(31250);
}




void loop() {
  
  process_readings();

  set_led_status();

  if (value_change) {
    send_midi_data();
  }
  

 delay(1);        // delay 3 milliseconds to avoid hogging MIDI bandwidth or cause overflow in certain software instruments
  
}


void process_readings() {
  byte new_ccval;
  
 
  pot_reading = analogRead(A1);
  
  if (abs(pot_reading - upper) > 2 ) {               // change upper value by steps of 3 to eliminate noise
    upper = pot_reading;
    debug("upper value", upper);
  }

  pressure_val = analogRead(A0);
  debug("pressure value" , pressure_val);
  new_ccval = convert_to_midi(pressure_val);
  if (new_ccval != ccval) {
    ccval = new_ccval;
    value_change = true;
    debug("ccval changed to ", new_ccval);
  }
  else if (new_ccval == ccval) {
    value_change = false;
  }
}

byte convert_to_midi(int reading) {
  byte result;
  result = constrain(map(reading, lower, upper, 0,127), 0, 127);
  return result;
}


void set_led_status() {

   if (pressure_val > upper) {
    digitalWrite(LED0, HIGH);
    digitalWrite(LED1, LOW);
    
  }
  else if (pressure_val > lower && pressure_val < upper) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED0, LOW);
  }
  else {
    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);
  }
  
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void send_midi_data() {
  if (ccval >= 1) {
    controlChange(midi_channel, breath_controller, ccval);
    debug("message sent", ccval);
    zero = 1;
    
  }

  else if (ccval < 1 && zero == 1) {
    controlChange(midi_channel, breath_controller, ccval);
    debug("message sent", ccval);
    delay(25);
    controlChange(midi_channel, breath_controller, 1);
    debug("0 message resent", ccval);
    zero = 0;
  }

  else {
    zero = 0;
  }
}

/*
void digital_denoise(int analogPin, int reading) {
  
}
*/
void debug(String debug_string, int value) {
  
  if (DEBUG) {
    Serial.print(debug_string);
    Serial.print(" ");
    Serial.print(value);
    Serial.println();
  }
}
