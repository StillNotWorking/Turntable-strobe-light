// Turntable strobe light for precise reading of the strobe disc or platter strobe, ensuring accurate speed adjustments for optimal performance. 
// Sketch tested with Nano ATMega328P 16MHz @ https://github.com/StillNotWorking/
// Tip: Cover up the power LED and the onboard LED should give enought light to have a clear readout even in daylight

const int ledPin = LED_BUILTIN;    // Onboard LED pin
volatile bool toggleLED = false;   // Flag to indicate when to toggle the LED
unsigned long previousMicros = 0;  // To track the time for the duty cycle
const unsigned long pulseDuration = 2500;  // Pulse duration in microseconds (2.5 ms = 25% duty cycle)

void setup() {
  pinMode(ledPin, OUTPUT);  // Set onboard LED pin as output
  
  // Configure Timer1 for 100 Hz
  cli();  // Disable interrupts during configuration
  
  // Set Timer1 to CTC mode (Clear Timer on Compare Match)
  TCCR1A = 0;  // Set entire TCCR1A register to 0
  TCCR1B = 0;  // Set entire TCCR1B register to 0
  
  // Set compare match register for a 10 ms interval (100 Hz)
  // Here tuned with osciloscope to 100 Hz with 3 decimals for my Nano 328P clone board
  OCR1A = 19996;  // 10 ms (100 Hz) = 2 MHz * 0.01 s = 20,000 - 1
  // OCR1A = 14814;  // Should give LED frequency 135 Hz giving 45 RPM readout from a 33 1/3 50 Hz strobe disc
  // OCR1A = 16666;  // Should give LED frequency 120 Hz for use with 60 Hz strobe disc
  
  // Set prescaler to 8 (16 MHz / 8 = 2 MHz timer frequency)
  TCCR1B |= (1 << WGM12) | (1 << CS11);  // WGM12: CTC mode, CS11: prescaler 8
  
  // Enable Timer1 compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();  // Enable interrupts
}

ISR(TIMER1_COMPA_vect) {
  // Set the flag to indicate it's time to toggle the LED in the main loop
  toggleLED = true;
}

void loop() {
  if (toggleLED) {
    // Toggle the LED ON and record the time
    digitalWrite(ledPin, HIGH);
    previousMicros = micros();  // Record when we turned the LED on
    
    toggleLED = false;  // Reset the flag
  }

  // Turn off the LED after the pulse duration has passed (duty cycle control)
  if (digitalRead(ledPin) == HIGH && (micros() - previousMicros >= pulseDuration)) {
    digitalWrite(ledPin, LOW);
  }
}
