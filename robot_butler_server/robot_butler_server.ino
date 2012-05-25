#include <IRremote.h>
#include <SPI.h>
#include <Ethernet.h>

#define BLUE 9
#define GREEN 10
#define WHITE 11
#define YELLOW 12
#define RED 3
#define BUTTON 7
#define THERMOMETER A0

// Up to 1023 + a null
#define PIN_VAL_MAX_LEN 5

int status = 0;
int v = 0;
int previous = 0;

#define STRING_BUFFER_SIZE 128
typedef char BUFFER[STRING_BUFFER_SIZE];

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x64, 0xA7, 0x69, 0x0D, 0x21, 0x21 };
IPAddress ip( 192, 168, 0, 101 );

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server( 80 );

void setup( ) {
  Serial.begin( 9600 );
  pinMode( RED, OUTPUT );
  // test( RED );
  pinMode( YELLOW, OUTPUT );
  pinMode( WHITE, OUTPUT );
  pinMode( GREEN, OUTPUT );
  pinMode( BLUE, OUTPUT );
  pinMode( BUTTON, INPUT );
  Ethernet.begin( mac, ip );
  server.begin();
  Serial.print( "server is at " );
  Serial.println( Ethernet.localIP( ));
}

void loop( ) {
  // thermo_light(  );
  // light_switch( BUTTON, RED );
  my_server( );
  // if ( Serial.read( ) != -1 ) {
  //  send_ir( );
  // }
  // delay( 3000 );
}

void send_ir ( ) {
  IRsend irsend;
  Serial.println( "sending another ir" );
  for ( int i = 0; i < 3; i ++ ) {
    Serial.println( "go" );
    irsend.sendSony( 0xa90, 12 ); // Sony TV power code, just an example from http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
    delay( 100 );
  }
}

void test ( int pin ) {
  Serial.print( "Testing pin" ); 
  Serial.println( pin ); 
  for ( int i = 0; i < 3; i ++ ) {
    digitalWrite( pin, HIGH );
    delay( 100 );
    digitalWrite( pin, LOW );
  }
}

void my_server ( ) {
  // listen for incoming clients
  EthernetClient client = server.available();
  if ( client ) {
    Serial.println( "new client" );
    if ( client.connected( ) && client.available( )) {
      char *method = "";
      char *path = "";
      char *response = "";
      char *data = "";
      if ( get_request( client, method, path, data )) {
        call_route( method, path, data, response );
      }
      client.print( json_header( ));
      client.print( json_response( response ));
      Serial.println( "response sent" );
    }
    delay( 1 ); // give the web browser time to receive the data
    client.stop(); // close the connection:
    Serial.println( "client disconnected" );
  }
}

boolean call_route ( char *method, char *path, char *data, char * & response ) {
  char *pin;
  pin = strtok( path, "/" );
  // Serial.print( "We want pin " );
  // Serial.println( pin );
  send_ir( ); // don't care what the data was just send ir for now
  sprintf( response, "{\"method\": \"%s\", \"path\": \"%s\", \"pin_a\": \"%s\", \"pin_d\": %d}", method, path, pin, pin );
  // Serial.println( response );
  return true;
}

char *json_header ( ) {
  return "HTTP/1.1 %s\nContent-Type: application/json\nConnnection: close\n\n";
}

char *json_response ( char *response ) {
  BUFFER s = "{\"a\":{";
  /* for ( int ac = 0; ac < 6; ac ++ ) {
    if ( ac ) strcat( s, "," );
    strcat( s, json_pair( 1, analogRead( 1 )));
  } */
  strcat( s, "},\"d\":{\"" );
  strcat( s, json_pair( BUTTON, digitalRead( BUTTON )));
  strcat( s, "}" );
  
  strcat( s, ",\"r\":" );
  strcat( s, response );
  strcat( s, "" );
  
  strcat( s, "}" );
  return s;
}

char *json_pair ( int k, int v ) {
  BUFFER s = "";
  char vs[PIN_VAL_MAX_LEN];
  sprintf( vs, "%d", k );
  strcat( s, vs );
  strcat( s, "\":" );
  sprintf( vs, "%d", v );
  strcat( s, vs );
  return s;
}

boolean get_request ( EthernetClient client, char * & method, char * & path, char * & data ) {
  char s[STRING_BUFFER_SIZE];
  s[0] = client.read();
  s[1] = client.read();
  int i = 2;
  while ( s[i-2] != '\r' && s[i-1] != '\n' && i < STRING_BUFFER_SIZE ) {
    s[i ++] = client.read();
  }
  method = strtok( s, " \n\r" );
  path = strtok( NULL, " \n\r?" );
  data = strtok( NULL, " ?" );
  return true;
}

void thermo_light( ) {
  int v = analogRead( THERMOMETER );
  Serial.print( v );
  Serial.print( ':' );
  v = ( 5 * v * 100.0 ) / 1024.0;
  Serial.println( v );
  digitalWrite( BLUE, ( v <= 5 ));
  digitalWrite( GREEN, ( v >= 5 && v <= 14 ));
  digitalWrite( WHITE, ( v >= 13 && v <= 18 ));
  digitalWrite( YELLOW, ( v >= 17 && v <= 21 ));
  digitalWrite( RED, ( v >= 21 ));
  delay( 1000 );
}

void light_switch ( int pin, int led ) {
  v = digitalRead( pin );
  if (( v == HIGH ) && ( previous == LOW )) {
    Serial.print( pin );
    Serial.print( ':' );
    Serial.print( v );
    Serial.println( '.' );
    status = 1 - status;
    delay( 10 );
  }
  previous = v;
  if ( status == 1 ) {
    digitalWrite( led, HIGH );
  }
  else {
    digitalWrite( led, LOW );
  }
}
