/**
 * Uses https://github.com/roberttidey/LightwaveRF
 * and maybe bits of https://github.com/lawrie/LightwaveRF
 * 
 * Arduino home automation experiments. Hardware:
 *  Arduino http://amzn.to/UCKWsq
 *  Ethernet shield http://amzn.to/1akfeTY
 *  LightwaveRF kit: http://amzn.to/RkukDo and http://amzn.to/V7yPPK
 *  RF transmitter and receiver http://bit.ly/HhltyI
 * 
 * @author PC <paul.clarke+paulclarke@holidayextras.com>
 * @date    Wed 23 Oct 2013 20:57:15 BST
 */

#include <LwRx.h>
#include <SPI.h>
#include <Ethernet.h>
// put your twitter.com credentials in Tweet.h - copy Tweet.h.sample - optional and experimental!
#include "Tweet.h"
// put your lightwaverf api host details here
#include "LWRFAPI.h"

EthernetClient client; // to make outbound connections
byte mac[] = { 0x64, 0xA7, 0x69, 0x0D, 0x21, 0x21 }; // mac address of this arduino
byte len = 10;
byte lastCommand[10];

void setup ( ) {
  Ethernet.begin( mac ); // dhcp
  delay( 1000 );
  lwrx_setup( 2 );  // set up with rx into pin 2
  Serial.begin( 9600 );
  Serial.println( "Set up completed" );
}

void loop ( ) {
  if ( lwrx_message( )) {
    byte msg[ len ];
    lwrx_getmessage( msg, &len );
    log( msg, len );
  }
}

void post ( char *host, char *path, char *data ) {
  Serial.println( data );  
  if ( client.connect( host, 80 )) {
    delay( 100 );
    client.flush( );
    client.print( "POST " );
    client.print( path );
    client.print( "?_=" );
    client.print( millis( ));
    client.println( " HTTP/1.1" );
    client.print( "Host: " );
    client.println( host );
    client.println( "User-Agent: Arduino/1.0" );
    client.println( "Content-Type: application/x-www-form-urlencoded" );
    client.print( "Content-length: " );
    client.println( strlen( data ));
    client.println( "Connection: close" );
    client.println( );
    client.println( data );
    client.stop( );
  }
  else {
    Serial.print( "failed to connect to " );
    Serial.println( host );
  }
}

void log ( byte *msg, byte len ) {
  if ( compare( msg, lastCommand, 0, len )) {
    // Serial.println( "msg + lastCommand were the same" );
    return;
  }
  for ( int i = 0; i < len; ++i ) {
    lastCommand[i] = msg[i];
  }
    
  char data[128];
  data[0] = 0;
  strcat( data, "title=" );
  strcat( data, device( msg ));
  strcat( data, "&text=" );
  strcat( data, command( msg ));
  strcat( data, "&key=" );
  strcat( data, apiKey );
  post( apiHost, apiPath, data );
  
  data[0] = 0;
  strcat( data, "t=" );
  strcat( data, device( msg ));
  strcat( data, "+sent+" );
  strcat( data, command( msg ));
  strcat( data, "+" );
  strcat( data, "https://github.com/pauly/rf-butler" );
  strcat( data, "&c=" );
  strcat( data, consumer_secret );
  strcat( data, "&a=" );
  strcat( data, access_token_secret );
  post( tweetHost, tweetPath, data );
}

/**
 * get a useful string out of an array of bytes
 */
char * tos ( byte *msg, int start, int end ) {
  // Serial.print( "From " );
  // Serial.print( start );
  // Serial.print( " to " );
  // Serial.print( end );
  // Serial.print( "=" );
  char * name = "";
  // name[0] = 0;
  int index = 0;
  for ( int i = start; i < end; i ++ ) {
    name[index++] = alpha( msg[i] );
  }
  name[index++] = '\0';
  // Serial.println( name );
  return name;
}

/**
 * get a command string from response
 */
char * command ( byte *msg ) {
  /* char * c;
  c[0] = alpha( msg[2] );
  c[1] = ' ';
  c[2] = 'n';
  c[3] = msg[3] ? 'n' : 'f';
  c[4] = msg[3] ? '\0' : 'f';
  c[5] = '\0';
  return c; */
  return tos( msg, 0, 5 );
}

/**
 * get device name from response
 */
char * device ( byte *msg ) {
  byte b[6] = { 0x00, 0x0D, 0x0C, 0x02, 0x08 };
  if ( compare( msg, b, 5, 10 )) {
    return "remote b";
  }
  return tos( msg, 5, 10 );
}

/**
 * turn a hex value into char for display, like sprintf
 */
char alpha ( byte a ) {
  return (char)( a < 10 ? a + 48 : a + 55 );
}

boolean compare ( byte a[], byte b[], int start, int end ) {
  for ( int i = start; i < end; ++ i ) {
    if ( a[i] != b[i] ) return false;
  }
  return true;
}

