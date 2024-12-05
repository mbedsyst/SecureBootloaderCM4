# Bootloader Design
## Bootloader
* Initialize Bootloader peripherals
* Display Bootloader Banner on Terminal
* Find Applications in Internal Flash
* Look for Firmware Update in External Flash and update Application code if found
* Select Application to load or wait
* Jump to Application code

## Find Applications in Internal Flash
* Create Slot Metadata instance.
* Create Code Metadata instance.
* Check for Magic Number at Slot start positions
* If Magic Number found, set Valid to true, else false.
* If Slot is Valid, fill the Code metadata structure for the available codes.
* Calculate Checksum of each application code and check it against the CRC value in the metadata.
* If checksum is correct, set Valid to true, else false.
* Display info message to indicate the number of applications found.

## Look for Firmware Update in External Flash
* Initialize W25Q and SPI
* Create a slot metadata instance
* Check for Magic Number at Start of Flash (0x00000000)
* If found, set Valid to true, else false.
* If Valid is set to true, create a Code Metadata instance and fill the members. Else, return.
* Calculate CRC value of firmware and check against CRC value in metadata.
* If checksum is verified, find the oldest version of application code in the slots if all three are filled. Else point to the slot which is free.
* Copy the new firmware from external flash and write it into the internal flash
* After writing, compute checksum again to verify the code transfer.
* Display message to indicate updation of firmware

## Select Application to load
* Display list of available applications and their details on Terminal
* Wait for the user to input the option
* Display message to indicate the selected option
* Load the default or selected application

## Jump to Application code
* Display chosen Application code metadata 
* Set VTOR
* Set MSP
* Display Message to jump to Application code 
* Execute Reset Handler at the address.