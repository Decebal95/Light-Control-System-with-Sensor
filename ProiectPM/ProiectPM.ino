#include "LedControl.h"
#include "songs.h"

#define BUZZER 8
#define echoPin 9
#define trigPin 7
#define DIN1 13
#define CS1 11
#define CLK 12
#define DIN2 7
#define CLK2 6
volatile byte state2 = LOW;
volatile byte state3 = LOW;
volatile byte state4 = LOW;
volatile byte state5 = LOW;

const int buttonPin[] =  {2, 3, 4, 5};

int currentIndex = 0;
int inputCode[] = {0, 0, 0, 0};

volatile double current_time = 0.0;
volatile int timer_isr_counter = 0;

void buton2() {
  state2 = !state2;
}
void buton3(){
  state3 = !state3;
}
void buton4() {
  state4 = !state4;
}
void buton5() {
  state5 = !state5;  
}

int buttonState = 0;
int codeAccepted = 0;
const int alarm[] = {1, 1, 1, 1};


int alarmcheck = 0;
byte first[8] = {B11111111,
B10000001,
B10000001,
B10000001,
B10000001,
B10000001,
B10000001,
B11111111
};
byte second[8] = {B11111111,
B11111111,
B11000011,
B11000011,
B11000011,
B11000011,
B11111111,
B11111111
};
byte third[8] = {B11111111,
B11111111,
B11111111,
B11100111,
B11100111,
B11111111,
B11111111,
B11111111
};
byte patru[8] = {B11111111,
B11111111,
B11111111,
B11111111,
B11111111,
B11111111,
B11111111,
B11111111
};
byte l[8] = {B11111111,
B11011111,
B11011111,
B11011111,
B11011111,
B11011111,
B11000011,
B11111111
};
byte e[8] = {B11000011,
B11011111,
B11011111,
B11000011,
B11011111,
B11011111,
B11000011,
B11111111
};

byte t[8] = {B11111111,
B10000011,
B11101111,
B11101111,
B11101111,
B11101111,
B11101111,
B11111111
};

byte apostrof[8] = {
B11111111,
B11100111,
B11100111,
B11111111,
B11111111,
B11111111,
B11111111,
B11111111
};

byte p[8] = {B11111111,
B11000011,
B11011011,
B11011011,
B11000011,
B11011111,
B11011111,
B11011111

};

byte a[8] = {B11111111,
B11000011,
B11011011,
B11011011,
B11000011,
B11011011,
B11011011,
B11111111
};

byte r[8] = {B11111111,
B11000111,
B11010111,
B11000111,
B11001111,
B11010111,
B11011011,
B11111111
};
byte y[8] = {B11111111,
B10111011,
B11010111,
B11101111,
B11101111,
B11101111,
B11101111,
B11111111
};
byte s[8] = {B11000011,
B11011111,
B11011111,
B11000011,
B11111011,
B11111011,
B11000011,
B11111111
};
LedControl lc = LedControl(DIN1, CLK, CS1, 0);
LedControl lc2 = LedControl(DIN2, CLK2, CS1, 0);

long duration;
int distance;

int pinPot = 0; 
float potVal = 0;

unsigned long delaytime=100;                                                                                        

int lastShape = 0;

int index = 0;


short *currentSong = melody;
short notesNr = sizeof(melody) / sizeof(melody[0]) / 2;
int currentSongIdx = 1;
volatile int  is_counter_on = 0;

void changeSong() {
  Serial.print("Song changed: ");
  currentSongIdx++;
  if (currentSongIdx > 3)
    currentSongIdx = 1;
  if (currentSongIdx == 1) {
    currentSong = melody;
    notesNr = sizeof(melody) / sizeof(melody[0]) / 2;
    Serial.print("Tetris Party\n");
  } else if (currentSongIdx == 2) {
    currentSong = melody2;
    notesNr = sizeof(melody2) / sizeof(melody2[0]) / 2;
    Serial.print("odetojoy\n");
  } else {
    currentSong = melody3;
    notesNr = sizeof(melody3) / sizeof(melody3[0]) / 2;
    Serial.print("Pink Panther");
  }

}


byte *getRandomShape() {
  lastShape = (lastShape + 1);
  switch(lastShape% 10 +1 ) {
    case 1: return l;
    case 2: return e;
    case 3: return t;
    case 4: return apostrof;
    case 5: return s;
    case 6: return p;
    case 7: return a;
    case 8: return r;
    case 9: return t;
    case 10: return y;
  }
}


void setup() {
  //configure_timer();
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(echoPin, INPUT);
  // for(int x=0; x<4; x++) 
  // { 
  //   pinMode(buttonPin[x], INPUT_PULLUP); 
  //   //attachInterrupt(digitalPinToInterrupt(b), buton2, FALLING); 
  // }
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), buton2, FALLING);
  
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), buton3, FALLING);

  pinMode(4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(4), buton4, FALLING);
  
  pinMode(5, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(5), buton5, FALLING);

  lc.shutdown(0, false);
  lc.setIntensity(0, 15);
  lc.clearDisplay(0);
  lc2.shutdown(0, false); 
  lc2.setIntensity(0, 15); 
  lc2.clearDisplay(0);

  index = 0;
  Serial.print("Enter code: ");
}

int msgShown = 0;

void loop(){
  int currentNote;
  float wait;
  int tempo = 114;
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;

  if (alarmcheck == 1) { 
    tone(BUZZER, 250, 200);
  }
  if (codeAccepted == 0) {
    
    for(int x=0; x<4; x++)
    {
      buttonState = digitalRead(buttonPin[x]);
  
      if (buttonState == LOW && buttonPin[x] == 2) {
        Serial.print("2");
        inputCode[currentIndex] = 2;
        currentIndex++;
      }
      else if (buttonState == LOW && buttonPin[x] == 3) {
        Serial.print("4");
        inputCode[currentIndex] = 4;
        currentIndex++;
      }
      else if (buttonState == LOW && buttonPin[x] == 4) {
        Serial.print("1");
        inputCode[currentIndex] = 1;
        currentIndex++;
      }
      else if (buttonState == LOW && buttonPin[x] == 5) {
        Serial.print("3");
        inputCode[currentIndex] = 3;
        currentIndex++;
      }
    }
    if (currentIndex == 4) {
      currentIndex = 0;
      if (correctCode() == 1) {
        Serial.println("\nCode accepted!");
        codeAccepted = 1;
      } else if (alarmcheck != 1){
        Serial.println("\nCode declined! Starting alarm!");
        is_counter_on = 1;
        alarmcheck = 1;
      }
      else if (alarmcheck == 1) {
        if (alarmCheck() == 1) {
          Serial.println("\nAlarm deactivated!");
          Serial.println("Enter Code: ");
          alarmcheck = 0;

          Serial.println("\nDisabling code declined! Try again");
        }
      }
    }
    delay(400);
  } else {
      if (msgShown == 0) {
        Serial.println("Tetris party\n!");
        msgShown = 1;
      }
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance= duration*0.034/2;
      
    
      if (distance < 7 && distance > 0) {
        Serial.print("Sensor activated: ");
        changeSong();
        index = 0;
      }
      potVal = analogRead(pinPot);
      potVal = potVal / 1023 - 0.5;
      if (index >= notesNr)
        index = 0;
    
      currentNote = currentSong[index];
      divider = currentSong[index + 1];
      
      if (divider > 0)
        wait = (wholenote) / divider * (1 - potVal);
      else {
        wait = (wholenote) / abs(divider) * (1 - potVal);
        wait *= 1.5;
      }
      byte *shape = getRandomShape();
    if (currentNote == 0) { 
        for (int i=0; i<8; i++){ 
          lc.setRow(0,i,0); 
        } 
        noTone(BUZZER); 
      } 
      else { 
        for (int i=0; i<8; i++){ 
          lc.setRow(0,i,shape[i]); 
        } 
        for (int i=0; i<8; i++){ 
          lc2.setRow(0,i,0); 
        } 
        tone(BUZZER, currentNote, wait * 0.9); 
         
      }
      index += 2;
      
      delay(wait);
      for (int i=0; i<8; i++){
          lc.setRow(0,i,0);
        }
      noTone(BUZZER);
      delay(wait * 0.05);
  }
}

int correctCode() {
  const int code[] = {1,4,3,2};
  int i;
  for ( i = 0; i < 4; i++) {
        if (inputCode[i] != code[i])
          break;
  }
  if ( i < 4)
  return 0;
  return 1;
}

int alarmCheck() {
  int i;
  for (i = 0; i < 4; i++) {
        if (inputCode[i] != alarm[i])
          break;
  }
  if ( i < 4)
  return 0;
  return 1;
}
