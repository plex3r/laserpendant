# laserpendant
Laser cutter jog pendant with auto z function for Norwich Hackspaces Just Add Sharks Greyfin laser cutter
I got the idea for this after coming accros this blog post http://www.justaddsharks.co.uk/blogs/2015-10-29/continuous-autofocus-laser-cutter-hack
I had originally planned to use a non contact IR sensor until another member suggested a ToF sensor which I had seen used elsewhere
The ToF sensor will be mounted to the laser head
The idea is to use a wii nunchuck controller as a pendant, the joystick with be used to move the laser head in the x and y direction
The LED is solid green when it's ready and flashes during calibration
To calibrate the position of the head and ToF sensor in relation to the material the Z height will be set manually as per normal use making use of the plastic Z height guage
Pressing the C button on the controller enters calibration mode, the sensor is sampled 100 times and the average of those samples is calculated and stored in EEPROM location 0, this location is read during startup
Using the joystick you can jog the laser head to where you want to start cutting
Pressing the Z button will perform the auto Z routine, this will move the Z axis up or down until the reading from the sensor is within tolerance to the stored value and voila auto Z height
