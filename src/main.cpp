#include <Arduino.h>

// put function declarations here:
//button pins (MSB on left which is button0)
struct Button{
  int pin;
  int state;
  int lastState;
  int debounceDelay;
  unsigned long lastDebounceTime;
};

Button inputButtons[5] = {
  {5},// bit 0 //add
  {8}, // bit 1 // sub
  {10},// bit 2 // or
  {11}, // bit 3 // and
  {12} // Confirm // xor
}; 

//structure to define 4 bit arrays for everything
struct fourbit{
  int bits[4];
  int value;
  int bitindex;
}; 
fourbit input1;
fourbit input2;

//allows modularity
struct ALUSession
 {
  fourbit input1;
  fourbit input2;
  int operationBits[3];
  int ALUPhase; //0=input1, 1=input2, 2=operation, 3=output, 
 };

ALUSession alu;


void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i<5; i++) {
    pinMode(inputButtons[i].pin, INPUT_PULLUP);
  }

  for (int i = 0; i < 5; i++){
    inputButtons[i].state = HIGH;
    inputButtons[i].lastState = HIGH;
    inputButtons[i].debounceDelay = 50;
    inputButtons[i].lastDebounceTime = 0;
  }
  
  Serial.begin(9600);

  resetALUSession(alu);
  };



void loop() {
  // put your main code here, to run repeatedly:

  if (alu.ALUPhase == 0) {
    handleInputPhase(input1, alu.ALUPhase);
  }

  else if (alu.ALUPhase == 1) {
    handleInputPhase(input2, alu.ALUPhase);
  }

  else if (alu.ALUPhase == 2) {
    for (int i = 0; i<5; i++) {
      int reading = digitalRead(inputButtons[i].pin);

      if (reading != inputButtons[i].lastState) {
        inputButtons[i].lastDebounceTime = millis();
      }
      if ((millis() - inputButtons[i].lastDebounceTime) > inputButtons[i].debounceDelay) {
        if (reading != inputButtons[i].state) {
          inputButtons[i].state = reading;
          if (reading == LOW) {
            //button pressed
            setOperationBits(alu, i);
            for (int j = 0; j < 3; j++){
              Serial.print(alu.operationBits[j]);
            }
            Serial.println();
            alu.ALUPhase ++;
          }
        }
      } 
      inputButtons[i].lastState = reading;
    }

  }

  else if (alu.ALUPhase == 3) {
    for (int i = 0; i < 5; i++){
      int reading = digitalRead(inputButtons[i].pin);

      if (reading != inputButtons[i].lastState) {
        inputButtons[i].lastDebounceTime = millis();
      }
      if ((millis() - inputButtons[i].lastDebounceTime) > inputButtons[i].debounceDelay) {
        if (reading != inputButtons[i].state) {
          inputButtons[i].state = reading;
          if (reading == LOW) {
            if (i == 0) {
              Serial.println("0");
              resetALUSession(alu);
            }
            else if (i == 1) {
              Serial.println("1");
              fourbitReset(input1);
              fourbitReset(input2);
              alu.ALUPhase = 1;
            }

            else if (i == 2) {
              Serial.println("2");
              fourbitReset(input1);
              fourbitReset(input2);
              alu.ALUPhase = 4;
            }
          }
        }
      } 
      inputButtons[i].lastState = reading;
    }

  }

  else if (alu.ALUPhase == 4) {

    static bool aFinalized = false;

    if (!aFinalized) {
      handleInputPhase(input1, alu.ALUPhase);

      if (alu.ALUPhase != 4) {
        aFinalized = true;
        alu.ALUPhase = 4;
      }
    } else {
      for (int i = 0; i<5; i ++) {
        int reading = digitalRead(inputButtons[i].pin);

        if (reading != inputButtons[i].lastState) {
          inputButtons[i].lastDebounceTime = millis();
        }
        if ((millis() - inputButtons[i].lastDebounceTime) > inputButtons[i].debounceDelay) {
          if (reading != inputButtons[i].state) {
            inputButtons[i].state = reading;
            if (reading == LOW) {
              setOperationBits(alu, i);
              Serial.println(binaryString(input1));

              for (int j = 0; j < 3; j++){
                Serial.print(alu.operationBits[j]);
              }
              Serial.println();

              alu.ALUPhase = 3;
              aFinalized = false;
            }
          }
        }
        inputButtons[i].lastState = reading;
      }
    } 
  }
}

// put function definitions here:
//fourbit reset function
void fourbitReset(fourbit &input){
  for (int i = 0; i < 4; i++){
    input.bits[i] = 0;
  }
  input.value = 0;
}

void resetALUSession(ALUSession &alu){
  fourbitReset(alu.input1);
  fourbitReset(alu.input2);
  for (int i = 0; i < 5; i++){
    alu.operationBits[i] = 0;
  }
  alu.ALUPhase = 0;
}

//toggle existing bit position
void toggleBit(fourbit &input, int bitindex){
    input.bits[bitindex] = !input.bits[bitindex];
  }

//finalize inputx
void finalizeInput(fourbit &input){
  input.value = 0;
  for (int i = 0; i < 4; i++){
    input.value += input.bits[i] * (1 << (3 - i));
  }
}

String binaryString(fourbit &input){
  String binaryString = "";
  for (int i = 0; i < 4; i++){
    binaryString += String(input.bits[i]);
  }
}

void setOperationBits(ALUSession &alu, int buttonIndex){
  int opCode = buttonIndex; // 0 to 4

  for (int i = 0; i < 3; i++){
    alu.operationBits[i] =  (opCode >> (2 - i)) & 0x01; // Extract bits from opCode
  }
}

void handleInputPhase(fourbit &input, int &phase){
  for (int i = 0; i<5; i++) {
      int reading = digitalRead(inputButtons[i].pin);

      if (reading != inputButtons[i].lastState) {
        inputButtons[i].lastDebounceTime = millis();
      }
      if ((millis() - inputButtons[i].lastDebounceTime) > inputButtons[i].debounceDelay) {
        if (reading != inputButtons[i].state) {
          inputButtons[i].state = reading;
          if (reading == LOW) {
            if (i<4) {
              toggleBit(input, i);
            } 
            else if (i == 4){
              finalizeInput(input);
              Serial.println(binaryString(input));
              phase++;
            }
        }
      } 
      inputButtons[i].lastState = reading;
    }
}