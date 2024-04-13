Here you'll find helpful resources for your pitch box, including schematics, code fragments for additional features, test tones, routines, and more.

All code is copyright StuKa Engineering or Flokason Best Audio, unless otherwise mentioned in the code. None of this information should be used for commercial purposes.

All code, schematics, and information provided in this repository are licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0). You are free to use, modify, and distribute the content for non-commercial purposes, provided that you give appropriate credit to the original author(s) and share any derivative works under the same license.

Please note that all schematics are for informational purposes only and are intended for users of our products. Additionally, all materials are in beta and prototype stages. We do not provide any warranty or guarantee for their use.

Here are some bits and pieces we've assembled so far.

PitchUI20.
***********
this is a first code for the Pitch98,Pitch13 and Pitch18 I2C remaintime inteface, remote control for Vinylium, flokason Pitch
using arduino controller. nano and some i2c display (adafruit or other)
it replaces and adds up functiosn to the traditional i2c interface using obsolete philips 7segment display chips. 
first coded by jvo studer. so far we did not extensively tested it and we see it as a first draft from 2020. 

the pitch box has an i2c port using RJ13,or RJ12 6/4 connection. use on old analog phones.  
I2C on Pitch98 	 RJ12 6P/4C Pin2 +5V, Pin3 SDA, Pin4 GND, Pin5 SCL 
I2C on Pitch13/18	 RJ12 6P/4C the pins are reversed. Pin5 +5V, Pin4 SDA, Pin3 GND, Pin2 SCL
if you have a long cable you need 4k7 to +5V on both lines and 22 ohm series resistors (i2c standard)

midi2lathecode. arduino
************************
this one is quick and dirty code for the old standard midi to lathe interface. 
translates midi notes into push button activation (over optocouper to interface the original analog pitch panel)
ues arduino nano. 

testtones
*********
the old vinylium testtone CD ripped as apple.aiff without the music as we dont own the rights. 

testtones sox scripts
*********************
generate on the fly some important test tones. 

i2c remaintime and BOM
**********************
the old original StuKa, vinylium i2c interface with oboslete Philips SAA1064 and PC8574.
