# Arduino Pomodoro Timer

This repository contains two projects made using an Arduino Uno board and is intended to generate a pomodoro timer. These two projects are essentially the same but differ on the output used to display the countdown. Here is a briefly summary of the two (more details on the specific folder).

## BCD to 7-segment Display

This project uses an electronic part that consist on 4 7-segment displays along with their converters. The Arduino generated the countdown on its outputs using BCD for every single digit.

## LCD Display

This is a more simple solution that uses a common LCD display to display the countdown and all the logic is controlled by the Arduino itself.

## 4 Digit 7-segment Display

This project is more simple than the other two since it uses only a display to show the countdowns and the Arduino solves all the rest.