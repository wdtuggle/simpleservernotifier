/*
* Copyright (c) 2015 by Davidson Tuggle <wdtuggleiv@gmail.com>
*
* This file is free software; you can redistribute it and/or modify
* it under the terms of either the GNU General Public License version 2
* or the GNU Lesser General Public License version 2.1, both as
* published by the Free Software Foundation.
*/

/* NOTE */
// Please read the README file for questions that
// ..may not be (immediately) addressed in the code

#include <SPI.h>
#include <Ethernet.h>
#include <ICMPPing.h>

/* Global Scope Variables */

//--------MODIFY THESE TO YOUR REQUIREMENTS--------------
     
// MAC address (this must be unique)
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x59, 0x67 };  
// Network settings; please set these to the desired values
IPAddress ip(192,168,1,3);    
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Server addresses to ping     
IPAddress serverAddr(192,168,1,100); // ip address of server to ping, can be local or external
IPAddress googleAddr(8,8,8,8);      // ip address to use to check internet
    
// Email server to be used; This was tested to work with smtpcorp.com
char server[] = "<stmp server info>";

// Email addresses used
char senderEmail[] = "sender@sender.com";
char rcptEmail[] = "receiver@receiver.com";

// SMTP login info (BASE 64)
// IMPORTANT:
// THIS IS NOT SECURE! DO *NOT* USE ACCOUNTS
// THAT ARE IMPORTANT TO YOU.
// It is trivial to decode base64 encoded text
// Use at your own discretion
char userB64[] = "B64 encoded username";
char passB64[] = "B64 encoded password";



// Email data messages
// This corresponds to sendEmail's 'type' integer argument
// 0 = servOff
// 1 = netOut
// 2 = Generic message (possibly reserved for future code implementations)
char servOff[] = "Server outage has been detected.";
char netOut[] = "Internet outage has been detected.";
char genMsg[] = "An error has occured! Please check serial output of arduino for further information.";

// Footer message
char footer[] = "**Powered by Arduino Uno**";

// How long the internet should not be available before deciding to send an email
// ..about the outage when it comes back
// Measured in seconds
int netTimeout = 120;

//--------------------------------------------------------

// Used to implement the ability to ping a server
ICMPPing ping(0, (uint16_t)random(0, 255));

// Client used when establishing a telnet connection to the email server      
EthernetClient client;

// Boolean variable that holds the state of the server
// True = online
// False = offline
boolean servStatus = true;


/* Setup function */ 
void setup()
{
  Serial.begin(9600);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  delay(2000);
}
  
/* Loop function */     
void loop()
{
      // Ping the desired server
      // If successful response is received, provide feedback via serial and set servStatus to true
      ICMPEchoReply echoReply = ping(serverAddr, 4);
      if (echoReply.status == SUCCESS)
      {
        Serial.println(F("Reply recieved: Server Online!"));
        servStatus = true;
      }
      // Otherwise something isn't right
      else
      {
        Serial.println(F("Reply not recieved: Server might be offline..."));
        servStatus = false;
        // Check to see if the internet is available before attempting to send an email
        // Wait until it becomes available if it isn't already
        boolean activeCon = false;
        int outageTime = 0;
        while(activeCon == false){
          activeCon = checkNet();
          if(activeCon == false){
            Serial.println(F("No internet connection available!"));
            outageTime += 5;
          }
          delay(5000);
          
        }
        
        // Check to see how long the internet was out for
        // If it was out for longer than the specified length (netTimeout)
        // Then send an email
        if(outageTime >= netTimeout){
            Serial.println("Internet outage detected! Sending email....");
            int sent = sendEmail(1);
            if(sent == 0)
              Serial.println("Email failed to send!");
        }      
        
        // See if the server is down after receiving internet
        echoReply = ping(serverAddr, 4);
        
        // If it isn't, do not send an email
        if (echoReply.status == SUCCESS)
            Serial.println(F("Server is online! Continuing..."));
        
        // Otherwise, the server is down. Send the email    
        else {        
          // Send the email
          int sent = sendEmail(0);
          if(sent == 0)
            Serial.println("Email failed to send!");
          
          //Do not continue until the server is back online
          while(servStatus == false){
            echoReply = ping(serverAddr, 4);
            if(echoReply.status == SUCCESS)
                servStatus = true;
            else
                Serial.println("Server offline...attempting ping request in 10 seconds...");
            delay(10000);
          }
          
        }
      }
      // Do nothing for 5 seconds
      delay(5000);
}

/* Associated functions */     

byte sendEmail(int type)
{
  byte thisByte = 0;
  byte respCode;
  
  // Setup a telnet connection   
  if(client.connect(server,2525) == 1) {
    Serial.println(F("Server connection established"));
  } 
  else {
    Serial.println(F("Server connection failed"));
    return 0;
  }
     
  if(!eRcv()) return 0;
  Serial.println(F("Initiating login"));
     
  // Say hello to the server...it is common practice to use your external IP as the parameter
  client.println(F("ehlo 9.8.7.6"));
  if(!eRcv()) return 0;
      
  // For most servers, authentication is required to reduce relay spam
  // Attempt to login
  client.println(F("AUTH LOGIN"));
  if(!eRcv()) return 0;
      
  // Input username. Both the username and password are encoded in Base64
  client.println(userB64);
  if(!eRcv()) return 0;
      
  // Input password
  client.println(passB64);
  if(!eRcv()) return 0;
      
  // Output response to serial letting observer know the email is getting ready to be sent
  Serial.println(F("Sending email..."));
     
  // Who the email is coming from
  client.print(F("MAIL FROM: ")); 
  client.println(senderEmail);
     
  // Who to send the email to
  client.print(F("RCPT TO: "));
  client.println(rcptEmail);
  if(!eRcv()) return 0;
 
  // The email itself begins to be formed here
  client.println(F("DATA"));
  if(!eRcv()) return 0;
     
  // Who the email came from
  client.print(F("To: "));
  client.println(senderEmail);
     
  // Who the email is going to
  client.print(F("From: "));
  client.println(rcptEmail);
     
  // Add a subject
  client.println(F("Subject: Server offline!\r\n"));
     
  // Main message in email
  // Look at argument type to determine
  // ...what it should use
  switch (type) {
    case 0:
      client.println(servOff);
      break;
    case 1:
      client.println(netOut);
      break;
    case 2:
      client.println(netOut);
      break;
    default:
      client.println(genMsg);
      break;
  }
   
  client.println(F("Server administrators should check the status of all systems."));
      
  // Footer, just for laughs
  client.println(F(""));   
  client.println(F("")); 
  client.println(footer);   
    
  // Let the smtp server know you are done composing the email and send it  
  client.println(F("."));
  if(!eRcv()) return 0;
     
  // Disconnect from server
  client.println(F("QUIT"));  
  if(!eRcv()) return 0;   
  client.stop();
     
  Serial.println(F("Server connection terminated."));
     
  return 1;
}
     
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
     
  while(!client.available()) {
    delay(1);
    loopCount++;
     
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }
     
  respCode = client.peek();
     
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
     
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
     
  return 1;
}
     
     
void efail()
{
  byte thisByte = 0;
  int loopCount = 0;
     
  client.println(F("QUIT"));
     
  while(!client.available()) {
    delay(1);
    loopCount++;
     
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
     
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
     
  client.stop();
     
  Serial.println(F("disconnected"));
}

boolean checkNet(){
    // 8.8.8.8 belongs to Google (DNS server). This should hopefully be a decent way to check if
    // ...the internet is available for the Arduino
      ICMPEchoReply echoReply = ping(googleAddr, 4);
      if (echoReply.status == SUCCESS)
          return true;
      else
          return false;
}


