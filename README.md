# Metronome_Raspberry_Pi_Pico

This is my take on the Metronome, a device that generates steady rhythm. Built using Raspberry pico w
I made it as a simple project to learn Raspberry pi pico and some concepts of Embedded systems in practical environment. The idea came up because my hobby is playing guitar and I often use metronome during practice.
It contains diode to generate rhythm visually, buttons to change the tempo, and 7segment to show BPM (Beats per minute). Diode could be easily exchanged for a buzzer to generate sound. 
Display is multiplexed by two BC546 transistors.
For the embedded concepts, project uses interrupts for changing the tempo using buttons, and 2 timers, one to turn on and off the diode to generate the steady rhytm and the second to multiplex the 7 segment display to show 2 digits. It could be easily extended to 3 or 4 digits, using more transistors. Because of the use of 2 digits, tempo is limited to 30-99 BPM, secured by the if statement. 
Next steps to improve the project would be to implement debouncing to the buttons, design a PCB and the case for the circuit.
