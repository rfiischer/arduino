// Macro for standard UART pins
#define RX 0
#define TX 1

// Macro for relay and switch pins
#define RLY 7
#define SWOUT 6
#define SWIN 5

// Constants
#define INPUT_SIZE 113
#define TBIT 0x80000000
#define SBIT 0x40000000
#define VBIT 0x20000000
#define TSVBIT 0xE0000000
#define DEBDEL 50

// Variables to measure time
uint32_t cTimeMs = 0;
uint8_t cDay_int = 0;
char cDay_str[4];
uint8_t cHour = 0;
uint8_t cMin = 0;
uint32_t timeMs = 0;
uint32_t t1 = 0;
uint32_t t2 = 0;

// Variables to track lamp status
uint8_t lampStatus = 0;

// Variables to handle strings
char input[INPUT_SIZE + 1];
char pars[8][16];
char *parBuff;
char output[INPUT_SIZE + 1];
uint8_t cmdSize = 0;
char ddtime[3];

// Variables that hold schedule information
uint8_t eventDay = 0;
uint8_t eventNum = 0;
uint32_t eventTime = 0;
char eventType[4];

uint32_t schedule[7][8] = {
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 0},
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 0},
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 0},
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 00},
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 0},
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 0},
  {0x20A4CB80, 0xA4819080, 0, 0, 0, 0, 0, 0}
  
};

// Variables to handle switch
uint8_t reading = HIGH;
uint8_t buttonState = HIGH;
uint8_t lastButtonState = HIGH;
uint32_t lastDebounceTime = 0;

// Setup variables
uint8_t counter = 0;

char days[7][4] = {
  "seg",
  "ter",
  "qua",
  "qui",
  "sex",
  "sab",
  "dom"
  
};

void setup() {
  // Initialize the serial interface to communicate with the bluetooth module
  Serial.begin(9600);

  // Initialize relay pin
  pinMode(RLY, OUTPUT);

  // Initialize switch pins
  pinMode(SWOUT, OUTPUT);
  pinMode(SWIN, INPUT_PULLUP);

  // Initialize output pins
  digitalWrite(SWOUT, LOW);
  digitalWrite(RLY, HIGH);

  // Initialize button state
  buttonState = digitalRead(SWIN);

}
void(* resetFunc)(void) = 0;

void loop() {
  // Gets time t1
  t1 = millis();
  
  
  // UART interface reception
  if (Serial.available()) {
    cmdSize = Serial.readBytes(input, INPUT_SIZE);
    input[cmdSize] = 0;

    // Splits commands on the separators
    counter = 0;
    parBuff = strtok(input, " :");
    
    while (parBuff != NULL) {
      strcpy(pars[counter], parBuff);
      counter = counter + 1;
      parBuff = strtok(NULL, " :");
      
    }

    // Do command action
    if (strcmp(pars[0], "set_time") == 0) {
      // set_time day hh:mm
      
      cDay_int = day_to_int(pars[1]);
      cTimeMs = atoi(pars[2])*3600000 + atoi(pars[3])*60000;
      Serial.println("Succesfully set time.\n");

    } else if (strcmp(pars[0], "get_time") == 0) {
      // get_time
      strcpy(cDay_str, days[cDay_int]);
      cHour = cTimeMs/3600000;
      cMin = (cTimeMs % 3600000)/60000;
      Serial.print(cDay_str);
      Serial.print(" ");
      sprintf(ddtime, "%02d", cHour);
      Serial.print(ddtime);
      Serial.write(":");
      sprintf(ddtime, "%02d", cMin);
      Serial.println(ddtime);
      Serial.write("\n");

    } else if (strcmp(pars[0], "set_sched") == 0) {
      // set_sched day hh:mm evN evT
      // evN: event number from 1 to 8
      // evT: event type, 'on' or 'off'

      eventDay = day_to_int(pars[1]);
      eventTime = (atoi(pars[2])*3600000 + atoi(pars[3])*60000) % 86400000;
      eventNum = atoi(pars[4]) - 1;

      schedule[eventDay][eventNum] = eventTime;
      
      if (strcmp(pars[5], "on") == 0) {
        schedule[eventDay][eventNum] |= TBIT;
        
      }

      schedule[eventDay][eventNum] |= VBIT;
      
      Serial.println("Succesfully scheduled.\n");
      
    } else if (strcmp(pars[0], "get_sched") == 0) {
      // get_sched

      for ( eventDay = 0; eventDay < 7; eventDay++) {
        for ( counter = 0; counter < 8; counter++) {
          if (schedule[eventDay][counter] & VBIT) {
            timeMs = schedule[eventDay][counter] & ~TSVBIT;
            cHour = timeMs/3600000;
            cMin = (timeMs % 3600000)/60000;
    
            if (schedule[eventDay][counter] & TBIT) {
              strcpy(eventType, "on");
              
            } else {
              strcpy(eventType, "off");
              
            }
            
            Serial.print(days[eventDay]);
            Serial.print(" ");
            sprintf(ddtime, "%02d", cHour);
            Serial.print(ddtime);
            Serial.write(":");
            sprintf(ddtime, "%02d", cMin);
            Serial.print(ddtime);
            Serial.print(" ");
            Serial.println(eventType);
            
          }
        
        } 
        
      }

      Serial.write("\n");

    } else if (strcmp(pars[0], "del_sched") == 0) {
      // del_sched day slot
      schedule[day_to_int(pars[1])][atoi(pars[2])] = 0;     
      
    } else if (strcmp(pars[0], "restart") == 0) {
      resetFunc();
      
    }
    
  }
  

  // Manage schedule
  for ( counter = 0; counter < 8; counter++) {
    if ((cTimeMs >= (schedule[cDay_int][counter] & ~TSVBIT)) && !(schedule[cDay_int][counter] & SBIT) && (schedule[cDay_int][counter] & VBIT)) {
      if (schedule[cDay_int][counter] & TBIT) {
        lampStatus = 1;
        
      } else {
        lampStatus = 0;
        
      }
      
      schedule[cDay_int][counter] |= SBIT;
      
    }
    
  }
  

  // Get switch status
  reading = digitalRead(SWIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
    
  }

  if ((millis() - lastDebounceTime) > DEBDEL) {
    if (reading != buttonState) {
      buttonState = reading;
      lampStatus = !lampStatus;
      
    }
  
  }

  lastButtonState = reading;
  

  // Turn lamp on or off
  if (lampStatus) {
    digitalWrite(RLY, LOW);
    
  } else {
    digitalWrite(RLY, HIGH);
    
  }
  

  // Minimum delay in ms
  delay(50);
  

  // Increments time ellapsed on function
  t2 = millis();
  cTimeMs = cTimeMs + t2 - t1;
  

  // Increment day counter
  if (cTimeMs >= 86400000) {
    cTimeMs = cTimeMs % 86400000;

    schedule[cDay_int][0] &= ~SBIT;
    schedule[cDay_int][1] &= ~SBIT;
    schedule[cDay_int][2] &= ~SBIT;
    schedule[cDay_int][3] &= ~SBIT;
    schedule[cDay_int][4] &= ~SBIT;
    schedule[cDay_int][5] &= ~SBIT;
    schedule[cDay_int][6] &= ~SBIT;
    schedule[cDay_int][7] &= ~SBIT;
    
    cDay_int = cDay_int + 1;
    
  }
  
}


uint8_t day_to_int(const char * day) {
  uint8_t day_int;
  
  if (strcmp(day, "seg") == 0) {
    day_int = 0;
    
  } else if (strcmp(day, "ter") == 0) {
    day_int = 1;
    
  } else if (strcmp(day, "qua") == 0) {
    day_int = 2;
    
  } else if (strcmp(day, "qui") == 0) {
    day_int = 3;
    
  } else if (strcmp(day, "sex") == 0) {
    day_int = 4;
    
  } else if (strcmp(day, "sab") == 0) {
    day_int = 5;
    
  } else if (strcmp(day, "dom") == 0) {
    day_int = 6;
  }

  return day_int;
  
}
