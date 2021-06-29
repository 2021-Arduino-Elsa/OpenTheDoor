int ledPin = 12;
char c;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    if (Serial.available()) {
    c = Serial.read();
    if(c =='1') {
      digitalWrite(ledPin,HIGH);
    }
    else if(c =='0') {
      digitalWrite(ledPin,LOW);
    }
  }
}
