First of all, dear members of the jury, good morning, good afternoon, or good evening, depending on the time you take the time to read this. I am very pleased to have this great opportunity to present to you the very detailed description of our robotics project, developed with great effort and dedication for the WRO Future Engineers Competition. Our team, called "The Weeknd," has proudly entered our prototype "Abel" (inspired by the name of a certain person) into two regional competitions. We are very excited to share with you the technical and functional details of our robot, which we consider a child.

1. Main components and specific connections
For the development of "Abel," we used carefully selected components that connect to each other via an Arduino UNO board. In this case, we used an L298n module (I call it a motor sensor), which made some connections easier, but also challenged us to be very precise in the programming logic and the application of Ohm's law. Below, we detail all the components and their selected connection ports:

Single DC motor (controlled with an L298N module):

IN1 (L298N input) → connected to digital pin D8 of the Arduino.

IN2 (L298N input) → connected to digital pin D9 of the Arduino.

ENA (Motor Enable A) → connected to pin D5 (PWM) or jumpered if you want the motor to always be enabled.

VCC of the L298N → connected to the positive terminal of the 7.4V or 12V rechargeable battery.

GND of the L298N → connected to both the GND of the Arduino and the negative terminal of the battery.

OUT1 and OUT2 of the L298N → connected directly to the DC motor leads.

Ultrasonic Sensor (HC-SR04):
VCC → to the Arduino's 5V pin.

GND → to the Arduino's GND.

TRIG → connected to digital pin D2.

ECHO → connected to digital pin D3.

MG996R Servo Motor (to rotate the robot):

Signal (orange or yellow) → Arduino's digital pin D6.

VCC (red) → powered by an external 5V source (can be supplied from a regulator if using a more powerful battery).

GND (black/brown) → connected to both the Arduino's GND and the external power supply's GND.

Rechargeable Battery (Power Supply):

We use a 7.4V battery (2 LiPo cells) or a 12V battery connected to the L298N module to power the DC motor.

PS: To avoid Arduino resets, the servo motor is powered separately, with its own regulator.

Arduino UNO Board:

The robot's control center. From here, all of the robot's movements and reactions are programmed using Arduino C code.

In addition to the aforementioned pins, the Arduino is powered through its VIN input from the L298N or via USB during testing.

2. Materials and Tools Used

Now, I want to emphasize the materials already mentioned in the creation of "Abel" without overemphasizing them:

* Arduino UNO
* DC Motor
* L298n Motor Driver
* HC-SR04 Ultrasonic Sensor
* Servo Motor
* Breadboard
* 9V Battery (or 5V Power Supply)
* Connection Cables

3. Programming Process and Functional Logic

"Abel" was programmed entirely in the Arduino development environment, using control structures, functions, and sensor readings to define its behavior and what we could do. We configured the ultrasonic sensor to constantly measure the distance in front of it. If it detects an obstacle within a specific distance, the robot executes a sequence of movements that allows it to stop, reverse, and turn in a new direction.

The L298N module communicates with the Arduino to execute these commands, controlling the polarity of the motors and thus defining the direction of movement.
The servomotor also responds to the programming: when it detects an obstacle, it is activated to change the steering angle and facilitate the robot's turning.

4. Challenges, Errors, and Applied Solutions

One of the most rewarding aspects of this experience was the technical problems we faced. At one point in the process, the robot simply wouldn't move, and we didn't know why. We checked all the components one by one: motors, connections, battery, code... and everything seemed to be in order. It was frustrating. After multiple tests, we discovered that the problem lay in a spring in the battery holder, which was loose and not making good contact, impeding the flow of power. Once this was resolved, the robot began to function properly.
This moment taught us that even a small detail can stop the entire system, and the importance of being patient, meticulous, and observant at every stage of the work.

5. Lessons learned and skills developed

Throughout the project, we acquired multiple lessons, both technical and personal. Among them:

We learned to program sensors and motors from scratch.
