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

#define LEDPIN 13
#define AIRQUALITYPIN A0
#define TWEET_DELTA (5L * 60L)

int smell = 0; // air quality value
boolean lit = false;

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
}

void loop ( ) {
  smell = analogRead( AIRQUALITYPIN );
  if ( smell > 600 ) {
    Serial.print( F( "air: " ));
    Serial.println( smell );
    if ( twitter.is_ready( )) {
      unsigned long now = twitter.get_time( );
      if ( last_tweet == 0 ) {
        Serial.print( F( "Now: " ));
        Serial.println( now );
        last_tweet = now - TWEET_DELTA + 15L;
      }
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
  lit = !lit;
  digitalWrite( LEDPIN, lit );
  delay(( 1023 - smell ) * 2 );
}
