# Button Led
### An application that interacts with a Button and an LED on the Nordic PCA10028 Development Kit

The application (Device Name = ButtonLed) provides a service (UUID = DCBA3154-1212-EFDE-1523-785FEF13D123) with two characteristics:  

1. User Description = Button, Size = 1 byte, UUID =  DCBA1523-1212-EFDE-1523-785FEF13D123
2. User Description = LED, Size = 1 byte, UUID =  DCBA1524-1212-EFDE-1523-785FEF13D123

The button characteristic's value records the number the number of times that button 4 has been pressed.

The LED characteristic reflects the current state of LED 2: 0 = off, 1 = on

Whenever button 4 is pressed the application takes three actions:  

1. The current state (on or off) of LED 2 is toggled.  
2. If notifications have been enabled then a notification is generated.
3. The value of the button characteristic is incremented; after 255 it wraps back to 0.  

A connected central can:  
* Read or write the state of the LED
* Read the value of the button
* Enable button press notifications  

There is a companion [iOS application](https://github.com/verticon/ButtonLed)
