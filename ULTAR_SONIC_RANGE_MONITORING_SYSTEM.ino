#define BLYNK_TEMPLATE_ID "TMPL3PMEeaoKT"
#define BLYNK_TEMPLATE_NAME "Ultrasonic Range Sensor"
#define BLYNK_AUTH_TOKEN "ji40lK4BvuFEtISK9Bzp8Ca-1wA4dySz"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

#define TRIG_PIN 5
#define ECHO_PIN 18
#define RED_LED 23
#define GREEN_LED 19
#define BUZZER 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "Student";
char pass[] = "Learn@123";

BlynkTimer timer;

bool blinkState = false;
int rangeThreshold = 20;
bool isWithinRange = false;

long getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void measureAndDisplay() {
  long distance = getDistanceCM();
  isWithinRange = distance < rangeThreshold;

  // Print to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Send to Blynk
  Blynk.virtualWrite(V0, distance);

  // Control output based on range
  if (isWithinRange) {
    // Near → Green LED ON, Red OFF, Buzzer OFF
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    // LCD Shows stable message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print("cm");
    lcd.setCursor(0, 1);
    lcd.print("WITHIN RANGE");
  } else {
    // Far → Blink Red LED and Buzzer
    blinkState = !blinkState;
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, blinkState);
    digitalWrite(BUZZER, blinkState);

    // LCD alternates message
    lcd.clear();
    if (blinkState) {
      lcd.setCursor(0, 0);
      lcd.print("Distance: ");
      lcd.print(distance);
      lcd.print("cm");
      lcd.setCursor(0, 1);
      lcd.print("OUT OF RANGE");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("WARNING:");
      lcd.setCursor(0, 1);
      lcd.print("TOO FAR!");
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, measureAndDisplay);  // Every 1 sec
}

void loop() {
  Blynk.run();
  timer.run();
}
