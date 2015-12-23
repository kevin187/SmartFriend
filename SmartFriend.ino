#include "Arduino.h"
#include "LedControl.h"
#include "Matrix.h"

#define INPUT_NUMBER 6
#define TRESHOLD 1010
#define MENUECOUNT 3
#define INPUT_READ_TIMES 4
#define PW_CHAR_ONE 1
#define PW_CHAR_TWO 4
#define PW_CHAR_THREE 2
#define PW_CHAR_FOUR 3

// DIN, CLK, CS
LedControl lc = LedControl(12, 11, 10, 1);
boolean pressed[INPUT_NUMBER] = {true, true, true, true, true};
char state = 'w';
int menue = 0;
int time = 0;
boolean changed_global = false;
int notes = 0;
int notice[100];
int notice_size = 0;
int add_notice_state = 0;
int password[4] = {0, 0, 0, 0};

void initialize() {
  int l = 0;
  for (l; l < 100; l++) notice[l] = 99;
  int c = 0;
  for (c; c < 17; c++) {
    int i = 0;
    for (i; i < 8; i++) lc.setRow(0, i, matrix_welcome[c][i]);
    delay(200);
  }
  delay(3000);
  int i = 0;
  for (i; i < 8; i++) lc.setRow(0, i, matrix_menue[menue][i]);
  state = 'm';
}

void blinking() {
  int i = 0;
  for (i; i < 10; i++) {
    // blink //
    int i = 0; for (i; i < 8; i++) lc.setRow(0, i, matrix_all[i]);
    delay(500);
    i = 0; for (i; i < 8; i++) lc.setRow(0, i, matrix_clear[i]);
    delay(500);
  }
  state = 'm';
  changed_global = true;
}

void waiting() {
  int k = 0;
  for (k; k < 8; k++) lc.setRow(0, k, matrix_waiting[time][k]);
  
  digitalWrite(13, HIGH);
    
  while (time > 0) {
    int i = 0;
    for (i; i < 60; i++) delay(1000);
    time--;
    k = 0;
    for (k; k < 8; k++) lc.setRow(0, k, matrix_waiting[time][k]);
  }
  
  digitalWrite(13, LOW);
  
  blinking();
}

void CheckInput() {
  int input[INPUT_NUMBER] = {0, 0, 0, 0, 0, 0};
  
  // initialize //
  int k = 0;
  for (k; k < INPUT_NUMBER; k++) pressed[k] = true;
    
  // input loop //
  int i = 0;
  
  // USE 1K RESISTORS TO DEFER MEASURING MULTIPLE TIMES //
  for (i; i < INPUT_READ_TIMES; i++) {
    delay(100);
    input[0] = analogRead(A0);
    input[1] = analogRead(A1);
    input[2] = analogRead(A2);
    input[3] = analogRead(A3);
    input[4] = analogRead(A4);
    input[5] = analogRead(A5);
      
    int l = 0;
    for (l; l < INPUT_NUMBER; l++) {
      if (!(input[l] >= TRESHOLD)) pressed[l] = false;
      else {
        digitalWrite(13, HIGH);
        delay(10);
        digitalWrite(13, LOW);
      }
    }
  }
}

void CheckInfluence() {
  boolean changed = false;
  
  if (pressed[5] == true) {
    Standby();
    return;
  }
  
  if (state == 'm') {
    if ((pressed[0] == true) || (pressed[2] == true)) {
      menue = (menue + (MENUECOUNT - 1)) % MENUECOUNT;
      changed = true;
    }
    else if ((pressed[1] == true) || (pressed[3] == true)) {
      menue = (menue + 1) % MENUECOUNT;
      changed = true;
    }
    else if (pressed[4] == true) {
      if (menue == 0) {
        state = 'n';
        notes = 0;
      }
      else if (menue == 1) {
        state = 'c';
        time = 0;
      }
      else if (menue == 2) {
        state = 'g';
      }
      changed = true;
    }
  }
  
  else if (state == 'c') {
    if ((pressed[0] == true) || (pressed[2] == true)) {
      if (time > 0) time--;
      changed = true;
    }
    else if ((pressed[1] == true) || (pressed[3] == true)) {
      if (time < 9) time++;
      changed = true;
    }
    else if (pressed[4] == true) {
      state = 'w';
      changed = true;
    }
  }
  
  else if (state == 'n') {
    
    if ((pressed[0] == true) || (pressed[2] == true)) {
      notes = (notes + 1) % 2;
      changed = true;
    }
    else if ((pressed[1] == true) || (pressed[3] == true)) {
      notes = (notes + 1) % 2;
      changed = true;
    }
    else if (pressed[4] == true) {
      if (notes == 0) {
        notice_size = 0;
        state = 'a';
        notes = 0;
        int l = 0;
        for (l; l < 100; l++) notice[l] = 99;
      }
      else if (notes == 1) {
        state = 'r';
        notes = 0;
      }
      changed = true;
    }
  }
  
  else if (state == 'a') {
    if (pressed[2] == true) {
      add_notice_state = (add_notice_state + 17) % 18;
      changed = true;
    }
    else if (pressed[3] == true) {
      add_notice_state = (add_notice_state + 1) % 18;
      changed = true;
    }
    else if (pressed[0] == true) {
      notice[notice_size] = (2 * add_notice_state);
      notice_size++;
      changed = true;
      
      int h = 0;
      for (h; h < 8; h++) lc.setRow(0, h, matrix_all[h]);
      delay(500);
    }
    else if (pressed[1] == true) {
      notice[notice_size] = (2 * add_notice_state) + 1;
      notice_size++;
      changed = true;
      
      int h = 0;
      for (h; h < 8; h++) lc.setRow(0, h, matrix_all[h]);
      delay(500);
    }
    else if (pressed[4] == true) {
      state = 'm';
      changed = true;
      add_notice_state = 0;
      notice_size = 0;
    }
  }
  
  if ((changed == true) || (changed_global == true)) {
    changed_global = false;
    DrawDisplay();
  }
}

void DrawDisplay() {
  if (state == 'm') {
    int i = 0;
    for (i; i < 8; i++) lc.setRow(0, i, matrix_menue[menue][i]);
  }
 else if (state == 'a') {
    int i = 0;
    for (i; i < 8; i++) {
      lc.setRow(0, i, matrix_notes_in[add_notice_state][i]);
    }
  }
  else if ((state == 'c') || (state == 'w')) {
    int i = 0;
    for (i; i < 8; i++) {
      lc.setRow(0, i, matrix_clock[time][i]);
    }
  }
  else if (state == 'n') {
    
    int i = 0;
    for (i; i < 8; i++) {
      lc.setRow(0, i, matrix_notes[notes][i]);
    }
  }
  else if (state == 'r') {
    int k = 0;
    while (!(notice[k] == 99)) {
      int i = 0;
      for (i; i < 8; i++) {
        lc.setRow(0, i, matrix_alphabet[notice[k]][i]);
      }
      k++;
      delay(1000);
    }
    state = 'm';
    menue = 0;
    changed_global = true;
  }
}

void Standby() {
  STANDBY:
  int i = 0;
  for (i; i < 8; i++) {
    lc.setRow(0, i, matrix_standby[i]);
  }
  delay(300);
  i = 0;
  for (i; i < 8; i++) {
    lc.setRow(0, i, matrix_clear[i]);
  }
  // wait for input //
  while (true) {
    CheckInput();
    if (pressed[5] == true) break;
    else if (pressed[0] == true) break;
    else if (pressed[1] == true) break;
    else if (pressed[2] == true) break;
    else if (pressed[3] == true) break;
    else if (pressed[4] == true) break;
  }
  // password //
  PASSWORD:
  int p = 0;
  blink_pw(p);
  while (true) {
    CheckInput();
    if (pressed[5] == true) goto STANDBY;
    else if (pressed[0] == true) {
      password[p] = 1;
      p++;
      blink_pw(p);
    }
    else if (pressed[1] == true) {
      password[p] = 2;
      p++;
      blink_pw(p);
    }
    else if (pressed[2] == true) {
      password[p] = 3;
      p++;
      blink_pw(p);
    }
    else if (pressed[3] == true) {
      password[p] = 4;
      p++;
      blink_pw(p);
    }
    
    // check password //
    if (p == 4) {
      // if TRUE//
      if ((password[0] == PW_CHAR_ONE)&& (password[1] == PW_CHAR_TWO) && (password[2] == PW_CHAR_THREE) && (password[3] == PW_CHAR_FOUR)) {
        changed_global = true;
        break;
      }
      // if FALSE //
      else goto PASSWORD;
    }
  }
}

void blink_pw(int p) {
  int i = 0;
  for (i; i < 8; i++) {
    lc.setRow(0, i, matrix_password[p][i]);
  }
}

void game() {
  int i = 0;
  for (i; i < 8; i++) {
    lc.setRow(0, i, matrix_all[i]);
  }
  delay(500);
  //while (true);
  
  state = 'm';
  menue = 2;
  changed_global = true;
}




















void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);
  
  pinMode(13, OUTPUT);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
}

void loop() {
  initialize();
  
  // INPUT_HANDLING //
  while (true) {
    CheckInput();
    CheckInfluence();
    if (state == 'w') waiting();
    if (state == 'g') game();
  }
}


