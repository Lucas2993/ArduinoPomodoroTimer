/*
  Pins:
  D0 -> Play/Pause button
  D1 -> Stop button
  D2 -> Digit 1 from the display
  D3 -> Digit 2 from the display
  D4 -> Digit 3 from the display
  D5 -> Digit 4 from the display
  D6 -> A segment from the display
  D7 -> B segment from the display
  D8 -> C segment from the display
  D9 -> D segment from the display
  D10 -> E segment from the display
  D11 -> F segment from the display
  D12 -> G segment from the display
  
  Functioning principle: The timer is a simple representation of a Pomodoro timer which will be counting working lapses of 25 minutes
  with 5 minutes of break. This will be done by alternating these durations with the action of the Stop button. So, this means that
  if the timer is running for the first time it will be showing the 25 minutes countdown ready to be started. Then, when that countdown
  ends (or is stopped by the user) the timer will show the 5 minutes countdown ready to be started. After finishing the 5 minutes
  countdown it will return to the 25 minutes and the cycle will continue.

  Buttons:
  1. Play/Pause: This button is used to start, pause and resume the countdowns.
  2. Stop: This button stops the current countdown and switches to the next one.
*/

#include <SevSeg.h>

// Display controller
SevSeg sevseg;

// Pins definition
const int PLAY_PAUSE_BUTTON_PIN = 0;
const int STOP_BUTTON_PIN = 1;

// Times definitions
const int DEFAULT_FOCUS_TIME_DURATION_MINUTES = 25;
const int DEFAULT_BREAK_TIME_DURATION_MINUTES = 5;
const int SECONDS_PER_MINUTE = 60;
const int MILLISECONDS_PER_SECOND = 1000;

// Messages
const String FOCUS_TIME_TITLE = "Focus Time";
const String BREAK_TIME_TITLE = "Break Time";

// Display
const byte NUM_DIGITS = 4;
const byte DIGIT_PINS[] = {2, 3, 4, 5};
const byte SEGMENT_PINS[] = {6, 7, 8, 9, 10, 11, 12, 13};
const byte HARDWARE_CONFIG = COMMON_CATHODE;
const bool RESISTORS_ON_SEGMENTS = false;
const bool UPDATE_WITH_DELAYS = true;

// Various
const int MAX_TIME_ALLOWED = 60;
const int DEBOUNCE_DELAY = 50;

// Control variables
int actualMinutes = 0;
int actualSeconds = 0;
bool focusing = false;
bool paused = false;
int lastPlayPauseButtonState = HIGH;
int lastStopButtonState = HIGH;
unsigned long lastStartPointMillis = 0;

// Methods
void showTime(int, int);
void switchToNextTimer();

void setup() {
  // Setup pins  
  pinMode(PLAY_PAUSE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize control variables
  actualMinutes = DEFAULT_FOCUS_TIME_DURATION_MINUTES;
  focusing = true;
  paused = true;

  // Initialize the display
  sevseg.begin(HARDWARE_CONFIG, NUM_DIGITS, DIGIT_PINS, SEGMENT_PINS, RESISTORS_ON_SEGMENTS, UPDATE_WITH_DELAYS);

  // Show the first time countdown ready to start
  showTime(actualMinutes, actualSeconds);
}

void loop() {
  // Refresh the display
  sevseg.refreshDisplay();

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
  showTime(actualMinutes, actualSeconds);
}

/*
  Switches the timer and toggles between break time and focus time. Always triggered by the Stop button.
*/
void switchToNextTimer(){
  actualMinutes = (focusing) ? DEFAULT_BREAK_TIME_DURATION_MINUTES : DEFAULT_FOCUS_TIME_DURATION_MINUTES;
  actualSeconds = 0;
  focusing = !focusing;
  paused = true;

  showTime(actualMinutes, actualSeconds);
}

/*
  Places the current time into the display (refresh).
*/
void showTime(int minutes, int seconds){
  if(minutes > MAX_TIME_ALLOWED || seconds > MAX_TIME_ALLOWED){
    return;
  }

  sevseg.setNumber(minutes * 100 + seconds);
  sevseg.refreshDisplay();
}