# Button Led
### An application that interacts with a Button and an LED on the Nordic PCA10028 Development Kit

The application provides a service with two characteristics: one for accessing Button 4, the other for accessing LED 2. When button 4 is pressed the application takes three actions:  
1. The current state (on or off) of LED 2 is toggled.  
2. If notifications have been enabled then a notification is generated.
3. The value of the button characteristic is incremented; after 255 it wraps back to 0.  

A connected central can:  
* Read or write the state of the LED
* Read the value of the button
* Enable button press notifications  

There is a companion [iOS application](https://github.com/verticon/ButtonLed)
