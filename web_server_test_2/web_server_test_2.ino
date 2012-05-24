#include <SPI.h>
#include <Ethernet.h>

#define BLUE 9
#define GREEN 10
#define WHITE 11
#define YELLOW 12
#define RED 13
#define BUTTON 7
#define THERMOMETER A0

int status = 0;
int v = 0;
int previous = 0;

#define STRING_BUFFER_SIZE 128
typedef char BUFFER[STRING_BUFFER_SIZE];
BUFFER buffer;

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
}

void my_server ( ) {
  // listen for incoming clients
  EthernetClient client = server.available();
  if ( client ) {
    Serial.println("new client");
    if ( client.connected( ) && client.available( )) {
      String method;
      String path;
      if ( get_request( client, method, path )) {
        Serial.println( "Method was " + method );
      }
      client.print( json_header( ));
      client.print( json_response( ));
      Serial.println( "response sent" );
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println( "client disconnected" );
  }
}

char *json_header ( ) {
  return "HTTP/1.1 %s\nContent-Type: application/json\nConnnection: close\n\n";
}

char *json_response ( ) {
  char *s = "{\"a\":{";
  /* for ( int ac = 0; ac < 6; ac ++ ) {
    if ( ac ) s += ",";
    sprintf( s, "%s{\"%d\":%d}", s, ac, analogRead( ac ));
  } */
  sprintf( s, "},\"d\":%s{\"%d\":%d}}", s, BUTTON, digitalRead( BUTTON ));
  return s;
}

int get_request ( EthernetClient client, String & method, String & path ) {
  String s;
  s += client.read();
  s += client.read();
  int i = 2;
  int space = 0;
  Serial.print( "Getting request, s[0] and s[1] are" );
  Serial.println( s.substring( 0, 2 ));
  while ( s[i-2] != '\r' && s[i-1] != '\n' && i < STRING_BUFFER_SIZE ) {
    char c = client.read();
    if ( c == ' ' ) {
      if ( space == 0 ) {
        method = s;
      }
      if ( space == 1 ) {
        /* for ( int j = method.length( ); j < i; j ++ ) {
          path += s[j];
        } */
      } 
      space ++;
    }
    s[i] = c;
    i ++;
  }
  Serial.print( "request(), buffer was " );
  Serial.println( s );
  Serial.print( "s.length is " );
  Serial.println( s.length( ));
  return 1;
}

int get_request2 ( EthernetClient client, String & method, String & path ) {
  char s[STRING_BUFFER_SIZE];
  s[0] = client.read();
  s[1] = client.read();
  int i = 2;
  int space = 0;
  while ( s[i-2] != '\r' && s[i-1] != '\n' && i < STRING_BUFFER_SIZE ) {
    char c = client.read();
    if ( c == ' ' ) {
      if ( space == 0 ) {
        method = s;
      }
      if ( space == 1 ) {
        // need to substr s somehow...
        // path = s.substring( method.length( ));
      } 
      space = space + 1;
    }
    s[i] = c;
    i ++;
  }
  Serial.print( "request(), buffer was " );
  Serial.println( s );
  return 1;
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
