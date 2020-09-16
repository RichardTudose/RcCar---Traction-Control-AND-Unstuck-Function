# RcCar---Traction-Control-AND-Unstuck-Function
Arduino - RC Car traction control and mud/snow unstuck program

# This program contains, most importantly, a traction control and car unstuck funtions
# Traction control program works like this: when one of the wheels has a speed of 20% higher than the other, the system cuts the 
faster wheel and so equalls their speed.
# The mud/snow unstuck program starts when there is 100% speed difference between the wheels and the wheel which has the grip recieves
the full power of the system, but in a pulsating way to create a momentum so that the car escapes

# Components used:
1. Pololu 10.000RPM DC motors
2. Servo motor - 90G
3. 4300mAh Li-Po battery
4. Arduino Mega 2560
5. L298v2 motors driver
6. MH-B IR sensors for RPM readings
7. DC-DC Booster XL6009E1 for 30.000RPM ( just for fun )
8. Bluetooth Module
