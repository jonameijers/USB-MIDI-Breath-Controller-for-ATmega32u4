
# Raspberry Pi Zero - USB MIDI Controller with breathController data using Serial Interface

My intention was for my Arduino Uno device to run as USB midi controller. On the Arduino I am using a pressure sensor to get Breath Pressure readings and a potentiometer to calibrate the signal to my breathing range.

However, since my board lacks the required usb microcontroller (16U2) to function as a standalone USB device, I decided to try and set up a spare Raspberry Pi Zero to act as my USB midi interface instead. The Pi and Arduino communicate over the Serial Peripheral Interface. 


## Requirements

[RPi.GPIO](https://pypi.org/project/RPi.GPIO/) and [Python-RtMidi](https://pypi.org/project/python-rtmidi/).

Follow [this guide](https://blog.gbaman.info/?p=791) to enable the ssh-over-usb capability. 

We will use a hardware SPI implementation. This requires the SPI kernel module to be loaded by uncommenting the 'dtparam=spi=on' line in /boot/config.txt


### To turn the Raspberry Pi Uno into a standalone USB midi controller, follow these steps: 


1. `echo "dwc2" | sudo tee -a /etc/modules`
2. `echo "libcomposite" | sudo tee -a /etc/modules`
3. `echo "g_midi" | sudo tee -a /etc/modules`
4. Create midi_over_usb file:\
`sudo touch /usr/bin/midi_over_usb`
5. Make it executable:\
`sudo chmod +x /usr/bin/midi_over_usb`
6. Edit it:\
`sudo nano /usr/bin/midi_over_usb`\
paste the following and save it:
```
cd /sys/kernel/config/usb_gadget/
mkdir -p midi_over_usb
cd midi_over_usb
echo 0x1d6b > idVendor # Linux Foundation
echo 0x0104 > idProduct # Multifunction Composite Gadget
echo 0x0100 > bcdDevice # v1.0.0
echo 0x0200 > bcdUSB # USB2
mkdir -p strings/0x409
echo "fedcba9876543210" > strings/0x409/serialnumber
echo "Your Name" > strings/0x409/manufacturer
echo "MIDI USB Device" > strings/0x409/product
ls /sys/class/udc > UDC
```
7. Now edit your rc.local:\
`sudo nano /etc/rc.local`\
and write this before "exit0", then save it:\
`/usr/bin/midi_over_usb`
8. Open the "cmdline.txt" file:\
`sudo nano /boot/cmdline.txt`\
, replace "`g_ether`" with "`g_midi`", then save it (Ctrl+O, Enter, Ctrl+X)\
9. reboot
`sudo reboot`

## Autostarting the Script at boot

`sudo sudo nano /etc/rc.local`

add this line before the exit statement:

`sudo python [PATH TO THE SCRIPT] &`





