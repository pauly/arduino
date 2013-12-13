/**
 * Taken from http://apcmag.com/how-to-tweet-with-your-arduino.htm
 * 
 * @author PC <paul.clarke+paulclarke@holidayextras.com>
 * @date    Fri 15 Nov 2013 22:32:36 GMT
 */

#include <SPI.h>
#include <Ethernet.h>
#include <sha1.h>
#include <Time.h>
#include <EEPROM.h>
#include <Twitter.h>
#include "Tweet.h"
#include <LwRx.h>
#include <LwTx.h>

#define LEDPIN 13
#define AIRQUALITYPIN A0
#define TWEET_DELTA (5L * 60L)

int smell = 0; // air quality value
long transmitTimeout = 0;
byte len = 10;

byte mac[] = { 0x64, 0xA7, 0x69, 0x0D, 0x21, 0x21 };

IPAddress twitter_ip( 199, 59, 150, 39 );
uint16_t twitter_port = 80;

unsigned long last_tweet = 0;

char buffer[512];
Twitter twitter( buffer, sizeof( buffer ));

void setup ( ) {
  pinMode( LEDPIN, OUTPUT );
  Serial.begin( 115200 );
  Serial.println( F( "Sniff tweeter" ));
  if ( Ethernet.begin( mac )) {
    Serial.print( F( "DHCP: " ));
    Ethernet.localIP( ).printTo( Serial );
    Serial.println( "" );
  }
  else {
    Serial.println( F( "DHCP configuration failed" ));
  }
  twitter.set_twitter_endpoint( PSTR( "api.twitter.com" ), PSTR( "/1.1/statuses/update.json" ), twitter_ip, twitter_port, false );
  twitter.set_client_id( consumer_key, consumer_secret );
  twitter.set_account_id( oauth_token, access_token_secret );
  delay( 500 );
  lwrx_setup( 2 ); // set up with rx into pin 2
  lwtx_setup( 3, 10 ); // transmit on pin 3, 10 repeats
  delay( 500 );
}

void loop ( ) {
  if ( lwrx_message( )) {
    byte msg[len];
    lwrx_getmessage( msg, &len );
    char tweet[140] = "";
    sprintf( tweet, "%x%x%x%x%x%x%x%x%x%x - %x%x%x%x%x set %x%x %x (%x%x) #arduino", msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6], msg[7], msg[8], msg[9], msg[4], msg[5], msg[6], msg[7], msg[8], msg[9], msg[2], msg[3], msg[0], msg[1] );
    Serial.print( F( "Tweeting: " ));
    Serial.println( tweet );
    Serial.print( freeRam( ));
    Serial.println( F( " bytes free" ));
    delay( 500 );
  }
  smell = analogRead( AIRQUALITYPIN );
  if ( smell > 600 ) {
    Serial.print( F( "air: " ));
    Serial.println( smell );
    
    if ( lwtx_free( )) {
      // message to turn the lights on or something
      byte msg[] = { 0x00, 0x00, 0x00, 0x01, 0x0F, 0x00, 0x0D, 0x0C, 0x02, 0x08 };
      lwtx_send( msg );
      transmitTimeout = millis( );
    }
    while ( ! lwtx_free( ) && millis( ) < ( transmitTimeout + 1000 )) {
      delay( 10 );
    }
    transmitTimeout = millis( ) - transmitTimeout;
    Serial.print( millis( ));
    Serial.print( F( " msg sent:" ));
    Serial.println( transmitTimeout );

    if ( twitter.is_ready( )) {
      unsigned long now = twitter.get_time( );
      if ( now > last_tweet + TWEET_DELTA ) {
        char msg[140];
        sprintf( msg, "Smell; air: %d #arduino", smell );
        Serial.print( F( "Tweeting: " ));
        Serial.println( msg );
        last_tweet = now;
        if ( twitter.post_status( msg )) {
          Serial.println( F( "Updated" ));
        }
        else {
          Serial.println( F( "failed" ));
        }
      }
    }
  }
  /* lit = !lit;
  Serial.println( lit );
  digitalWrite( LEDPIN, lit ? HIGH : LOW );
  delay(( 1023 - smell ) * 2 ); */
}

/**
 * turn a hex value into char for display, like sprintf
 */
char alpha ( byte a ) {
  return ( char )( a < 10 ? a + 48 : a + 55 );
}

/**
 * get a useful string out of an array of bytes
 */
char * tos ( byte * msg, int start, int end ) {
  Serial.print( F( "From " ));
  Serial.print( start );
  Serial.print( F( " to " ));
  Serial.print( end );
  Serial.print( F( "=" ));
  char * name = "";
  name[0] = 0;
  int index = 0;
  for ( int i = start; i < end; i ++ ) {
    name[index++] = alpha( msg[i] );
  }
  name[index++] = '\0';
  Serial.println( name );
  return name;
}

/**
 * From http://playground.arduino.cc/Code/AvailableMemory
 */
int freeRam ( ) {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - ( __brkval == 0 ? (int) &__heap_start : (int) __brkval ); 
}
