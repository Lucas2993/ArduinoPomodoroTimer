/*
  Pins:
  D0 -> Play/Pause button
  D1 -> Stop button
  D2 -> D7 pin from the display
  D3 -> D6 pin from the display
  D4 -> D5 pin from the display
  D5 -> D4 pin from the display
  D11 -> Empty pin from the display
  D12 -> RS pin from the display
  
  Functioning principle: The timer is a simple representation of a Pomodoro timer which will be counting working lapses of 25 minutes
  with 5 minutes of break. This will be done by alternating these durations with the action of the Stop button. So, this means that
  if the timer is running for the first time it will be showing the 25 minutes countdown ready to be started. Then, when that countdown
  ends (or is stopped by the user) the timer will show the 5 minutes countdown ready to be started. After finishing the 5 minutes
  countdown it will return to the 25 minutes and the cycle will continue.

  Buttons:
  1. Play/Pause: This button is used to start, pause and resume the countdowns.
  2. Stop: This button stops the current countdown and switches to the next one.
*/

#include <LiquidCrystal.h>

// Initialize display library
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

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
const int FIRST_LINE_COLUMN = 0;
const int SECOND_LINE_COLUMN = 0;
const int FIRST_LINE_ROW = 0;
const int SECOND_LINE_ROW = 1;
const int SCREEN_COLUMNS_COUNT = 16;
const int SCREEN_ROWS_COUNT = 2;

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
  lcd.begin(SCREEN_COLUMNS_COUNT, SCREEN_ROWS_COUNT);

  // Place title
  lcd.print(FOCUS_TIME_TITLE);

  // Show the first time countdown ready to start
  showTime(actualMinutes, actualSeconds);
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
  showTime(actualMinutes, actualSeconds);
}

/*
  Switches the timer and toggles between break time and focus time. Always triggered by the Stop button.
*/
void switchToNextTimer(){
  lcd.clear();

  lcd.setCursor(FIRST_LINE_COLUMN, FIRST_LINE_ROW);
  lcd.print((focusing) ? BREAK_TIME_TITLE : FOCUS_TIME_TITLE);

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

  lcd.setCursor(SECOND_LINE_COLUMN, SECOND_LINE_ROW);

  char buffer[5];
  
  sprintf(buffer, "%02d:%02d", minutes, seconds);
  lcd.print(buffer);
}