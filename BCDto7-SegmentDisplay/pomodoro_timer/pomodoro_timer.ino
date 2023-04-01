/*
  Pins:
  A2 -> Play/Pause button
  A3 -> Stop button
  D0-D3 -> First digit (bcd)
  D4-D7 -> Second digit (bcd)
  D8-D11 -> Third digit (bcd)
  D12, D13, A0, A1 -> Fourth digit (bcd)
  A2 -> Play/Pause button
  A3 -> Stop button
  
  Example: 21:49

  - First digit -> 2 -> 0010
  - Second digit -> 1 -> 0001
  - Third digit -> 4 -> 0100
  - Fourth digit -> 9 -> 1001

  Functioning principle: The timer is a simple representation of a Pomodoro timer which will be counting working lapses of 25 minutes
  with 5 minutes of break. This will be done by alternating these durations with the action of the Stop button. So, this means that
  if the timer is running for the first time it will be showing the 25 minutes countdown ready to be started. Then, when that countdown
  ends (or is stopped by the user) the timer will show the 5 minutes countdown ready to be started. After finishing the 5 minutes
  countdown it will return to the 25 minutes and the cycle will continue.

  Buttons:

  1. Play/Pause: This button is used to start, pause and resume the countdowns.
  2. Stop: This button stops the current countdown and switches to the next one.
*/

// Pins definition
const int FIRST_DIGIT_PINS [] = {0, 1, 2, 3};
const int SECOND_DIGIT_PINS [] = {4, 5, 6, 7};
const int THIRD_DIGIT_PINS [] = {8, 9, 10, 11};
const int FOURTH_DIGIT_PINS [] = {12, 13, 14, 15};

const int PLAY_PAUSE_BUTTON_PIN = 16;
const int STOP_BUTTON_PIN = 17;

// Times definitions
const int DEFAULT_FOCUS_TIME_DURATION_MINUTES = 25;
const int DEFAULT_BREAK_TIME_DURATION_MINUTES = 5;
const int MINUTES_PER_HOUR = 60;
const int SECONDS_PER_MINUTE = 60;
const int MILLISECONDS_PER_SECOND = 1000;

// Various
const int BCD_BITS_COUNT = 4;
const int MAX_NUMBER_ALLOWED_PER_DIGIT = 9;
const int MAX_TIME_ALLOWED = 60;
const int DEBOUNCE_DELAY = 50; 

// Helpers
// Timeframe values
const int MINUTES_VALUE = 0;
const int SECONDS_VALUE = 1;

// Control variables
int actualMinutes = 0;
int actualSeconds = 0;
bool focusing = false;
bool paused = false;
int lastPlayPauseButtonState = HIGH;
int lastStopButtonState = HIGH;
unsigned long lastStartPointMillis = 0;

// Methods
void showDigit(int, int *);
void showTime(int, int);
void switchToNextTimer();

void setup() {
  // Setup pins
  for (int i = 0; i < sizeof(FIRST_DIGIT_PINS) / sizeof(int); i++) {
    pinMode(FIRST_DIGIT_PINS[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(SECOND_DIGIT_PINS) / sizeof(int); i++) {
    pinMode(SECOND_DIGIT_PINS[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(THIRD_DIGIT_PINS) / sizeof(int); i++) {
    pinMode(THIRD_DIGIT_PINS[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(FOURTH_DIGIT_PINS) / sizeof(int); i++) {
    pinMode(FOURTH_DIGIT_PINS[i], OUTPUT);
  }
  
  pinMode(PLAY_PAUSE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize control variables
  actualMinutes = DEFAULT_FOCUS_TIME_DURATION_MINUTES;
  focusing = true;
  paused = true;

  showTime(actualMinutes, MINUTES_VALUE);
  showTime(actualSeconds, SECONDS_VALUE);
}

void loop() {
  // Stop button action
  int stopButtonState = digitalRead(STOP_BUTTON_PIN);
  if(stopButtonState != lastStopButtonState){
    delay(DEBOUNCE_DELAY);

    stopButtonState = digitalRead(STOP_BUTTON_PIN);
    if(stopButtonState == LOW){
      switchToNextTimer();
    }

    lastStopButtonState = stopButtonState;
  }
  
  // Play/Pause button action
  int playPauseButtonState = digitalRead(PLAY_PAUSE_BUTTON_PIN);
  if(playPauseButtonState != lastPlayPauseButtonState){
    delay(DEBOUNCE_DELAY);

    playPauseButtonState = digitalRead(PLAY_PAUSE_BUTTON_PIN);
    if(playPauseButtonState == LOW){
      paused = !paused;
      lastStartPointMillis = millis();
    }

    lastPlayPauseButtonState = playPauseButtonState;
  }
  
  // Checks if it is paused
  if(paused){
    return;
  }

  // Check if a second passed
  if(millis() - lastStartPointMillis < MILLISECONDS_PER_SECOND){
    return;
  }

  lastStartPointMillis = millis();

  // Check if the countdown ended
  if(actualMinutes == 0 && actualSeconds == 0){
    switchToNextTimer();
    return;
  }

  actualSeconds--;
  if(actualSeconds < 0){
    actualMinutes--;
    actualSeconds = SECONDS_PER_MINUTE - 1;
  }

  // Show time
  showTime(actualMinutes, MINUTES_VALUE);
  showTime(actualSeconds, SECONDS_VALUE);
  
  //delay(MILLISECONDS_PER_SECOND);
}

/*
  Switches the timer and toggles between break time and focus time. Always triggered by the Stop button.
*/
void switchToNextTimer(){
  actualMinutes = (focusing) ? DEFAULT_BREAK_TIME_DURATION_MINUTES : DEFAULT_FOCUS_TIME_DURATION_MINUTES;
  actualSeconds = 0;
  focusing = !focusing;
  paused = true;

  showTime(actualMinutes, MINUTES_VALUE);
  showTime(actualSeconds, SECONDS_VALUE);
}

void showTime(int value, int timeframe){
  if(value > MAX_TIME_ALLOWED){
    return;
  }

  int * msdPins = (timeframe == MINUTES_VALUE) ? const_cast<int *>(FIRST_DIGIT_PINS) : const_cast<int *>(THIRD_DIGIT_PINS);
  int * lsdPins = (timeframe == MINUTES_VALUE) ? const_cast<int *>(SECOND_DIGIT_PINS) : const_cast<int *>(FOURTH_DIGIT_PINS);

  showDigit(value % 10, lsdPins);
  value /= 10;
  showDigit(value % 10, msdPins);
}

/*
  Converts the given number into its BCD equivalent and places it into the given pins (the array must contain four elements referencing output pins).
*/
void showDigit(int value, int * pins){
  if(value > MAX_NUMBER_ALLOWED_PER_DIGIT){
    return;
  }

  for(int index = 0;index < BCD_BITS_COUNT ;index++){
    digitalWrite(pins[index], ((value % 2) ? HIGH : LOW));
    value /= 2;
  }
}