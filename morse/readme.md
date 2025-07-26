# morse
dead simple morse code encoder using the built in LED.
## schematic
just a esp32 dev board, nothing else
## usage
type in any a-z character in the serial terminal and press return, then see the cute esp32 blinking morse code
> [!note]
> my board's built in LED is connected to GPIO 1, which is a UART pin, so i need to disable serial communication before blinking the LED. if your board's LED is connected to other GPIO such as 2, you dont need to do this.
