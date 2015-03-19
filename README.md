Copyright (c) 2015 by Davidson Tuggle <wdtuggleiv@gmail.com>

This file is free software; you can redistribute it and/or modify it under the terms of either the GNU General Public License version 2 or the GNU Lesser General Public License version 2.1, both as published by the Free Software Foundation.


# simpleservernotifier
A simple program that pings a server, and notifies the user via email if said server is offline.

For the Arduino Platform


# PLEASE READ THE FOLLOWING:
This program is explicitly intended for home use only. There are (currently) no security measurements implemented in the code, so SMTP authentication can be potentially compromised by malicious users. Please do not use credentials that might be used in other potentially important servers/systems. Thank you.

# What it does
This program allows an Arduino UNO paired with an Ethernet Shield to ping a server periodically to check if it is online or not. If a ping request fails, depending on the situation, it will send an email to the user alerting him or her that the server is offline. This program will also detect if its own internet connection is lost. After a user specified amount of time of outage, when the internet comes back, the program will email the user alerting him or her about the outage. 



# Getting Started
Before attempting to compile and upload this code to your Arduino device, you should take into account a few key notes:

1. This code has only been tested on the Arduino Uno and Ethernet Shield hardware.

2. You must modify the code before attempting to run it on the hardware. See below for specifics

3. Be sure you have a method to send mail via an SMTP server. I used smtpcorp.com, which offers free accounts with 20 emails per day, to test the code.

Once you have access to an SMTP server, you can now edit the code. All of the data that you need to change is located at the top of the source, underneath the "MODIFY THESE TO YOUR REQUIREMENTS" comment. At the very least, you must change the following variables:

char server[] - The SMTP server

char senderEmail[] - The sender's email address

char rcptEmail[] - The receiver's email address

char userB64[] - Username in Base64 for the SMTP server

char passB64[] - Password in Base64 for the SMTP server

It is recommended that you change the following network variables as well:

byte mac[] - MAC addresss

IPAddress ip(192,168,1,3); - Local IP Address

IPAddress gateway(192,168,1,1); - Local Gateway

IPAddress subnet(255,255,255,0); - Subnet Mask

For userB64 and passB64, the string you set these to *must* be encoded in Base64. There are plenty of free tools online that let you encode regular username/password text into their equivalent Base64 representations. Please note that Base64 encoding is *NOT* encryption...it does not offer any sort of security benefit; it is merely for data transfer purposes.

After making these changes, you should be able to compile and upload the code to your Arduino device, make the appropriate hardware connections (power, ethernet), and inspect the program running via serial. 

If you have any questions, find any mistakes, or have ideas for improvements, feel free to email me at wdtuggleiv@gmail.com

Thanks for reading,

-Davidson
