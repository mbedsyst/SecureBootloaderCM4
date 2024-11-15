# Secure Bootloader for Cortex-M4
A bootloader responsible for initializing peripherals, verifying the integrity of the application code using 
the CRC Calculation Unit, and transferring control to the application code. The bootloader initializes only 
the USART2 and CRC peripherals, which are de-initialized upon exit. The application code should include 
metadata in the header, consisting of a pre-shared Magic (ID) Number, Version Number, Size, and CRC Value.