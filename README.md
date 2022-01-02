# mobile-bell
Remote Doorbell which relies on ESP32, Buzzer, WiFi, Firebase and a Flutter mobile application.

The main objective of this project is to be able to trigger a buzzer from anywhere and anytime. To achieve this we used ESP32 as the receiver of signals which then turns on the buzzer based on the signal given. The flutter mobile application acts as the trigger of the signal.

We use Firebase Realtime Database to enable us to manage the state of the buzzer in real-time.

## mobile_doorbell
This is the source code for the ESP32, it contains a .ino file.

## bell_app
This is the code for the Flutter app and website.
