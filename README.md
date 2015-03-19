# simpleservernotifier
A relatively simple program that pings a server, and notifies the user via email if said server is offline.

For the Arduino Platform


# PLEASE READ THE FOLLOWING:
This program is explicitly intended for home use only. There is (currently) no security measurements implemented in the code, so SMTP authentication can be potentially compromised by malicious users. Please do not use credentials that might be used in other potentially important servers/systems. Thank you.

# Getting Started
Before attempting to compile and upload this code to your Arduino device, you should take into account a few key notes:

1. This code has only been tested on the Arduino Uno and Ethernet Shield hardware.

2. You must modify the code before attempting to run it on the hardware. See below for specifics

3. Be sure you have a method to send mail via an SMTP server. I used smtpcorp.com, which offers free accounts with 20 emails per day, to test the code.

Once you have access to an SMTP server, you can now edit the code. All of the data that you need to change is located at the top of the source, underneath the "MODIFY THESE TO YOUR REQUIREMENTS" comment. At the very least, you must change the following variables:

char server[]

char senderEmail[]

char rcptEmail[]

char userB64[]

char passB64[]

It is recommended that you change the following network variables as well:

byte mac[] 

IPAddress ip(192,168,1,3);    

IPAddress gateway(192,168,1,1);

IPAddress subnet(255,255,255,0);

For userB64 and passB64, the string you set these to *must* be encoded in Base64. There are plenty of free tools online that let you encode regular username/password text into their equivalent Base64 representations. Please note that Base64 encoding is *NOT* encryption...it does not offer any sort of security benefit; it is merely for data transfer purposes.

After making these changes, you should be able to compile and upload the code to your Arduino device, make the appropriate hardware connections (power, ethernet), and inspect the program running via serial. 

If you have any questions, find any mistakes, or have ideas for improvements, feel free to email me at wdtuggleiv@gmail.com

Thanks for reading,

-Davidson
