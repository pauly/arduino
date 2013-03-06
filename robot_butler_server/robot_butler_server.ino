/**
 * Control my "robot butler"
 * Accepts commands over http and passes them on to devices.
 * LOTS of unused code in here, a work in progress!
 * Features experiments in temperature and lightwaverf
 * 
 * @author PC <paulypopex+arduino@gmail.com>
 * @date    Mon Sun  6 22:50:43 GMT 2013 
 */ 

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define BLUE 9
#define GREEN 10
#define WHITE 11
#define YELLOW 12
#define RED 3
#define BUTTON 7
#define THERMOMETER A0
// how many pins have things on them?
#define ANALOG_PINS 1

// Up to 1023 + a null
#define PIN_VAL_MAX_LEN 5
#define HTTP_PORT 666

unsigned int status = 0;
unsigned int v = 0;
unsigned int previous = 0;
unsigned int localPort = 9761; // local port to listen on
unsigned int lwrfPort = 9760; // local port to listen on
IPAddress lwrfServer( 192, 168, 0, 14 ); // lightwaverf wifi link ip address
EthernetUDP Udp; // An EthernetUDP instance to let us send and receive packets over UDP

#define STRING_BUFFER_SIZE 256
typedef char BUFFER[STRING_BUFFER_SIZE];

byte mac[] = { 0x64, 0xA7, 0x69, 0x0D, 0x21, 0x21 }; // mac address of this arduino
IPAddress ip( 192, 168, 1, 101 ); // requested ip address of this arduino

EthernetServer server( HTTP_PORT ); // Initialize the json server on this port
EthernetServer webserver( 80 ); // Initialize the web server on this port

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
  Udp.begin( localPort );
  server.begin( );
  Serial.print( "server is at " );
  Serial.print( Ethernet.localIP( ));
  Serial.print( ":" );
  Serial.print( HTTP_PORT );
  webserver.begin( );
  Serial.print( "web server is also at " );
  Serial.println( Ethernet.localIP( ));
}

void loop( ) {
  // thermo_light(  );
  my_server( );
  my_web_server( );
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
  EthernetClient client = server.available();  // listen for incoming clients
  if ( client ) {
    Serial.println( "new client" );
    if ( client.connected( ) && client.available( )) {
      char *method = "";
      char *path = "";
      char *response;
      char *data = "";
      if ( get_request( client, method, path, data )) {
        response = call_route( method, path, data );
        Serial.println( response );
      }
      client.print( http_header( "200 OK", "" ));
      client.print( json_response( response ));
    }
    delay( 1 ); // give the web browser time to receive the data
    client.stop(); // close the connection:
  }
}

void my_web_server ( ) {
  EthernetClient client = webserver.available();  // listen for incoming clients
  if ( client ) {
    if ( client.connected( ) && client.available( )) {
      BUFFER html, header;
      sprintf( html, "<html><title>Arduino!</title><body><h1>Arduino!</h1><p>Server is on port %d remember...</p></body></html>", HTTP_PORT, HTTP_PORT );
      sprintf( header, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-length: %d\nConnection: close\n\n", strlen( html ));
      client.print( header );
      client.print( html );
    }
    delay( 1 ); // give the web browser time to receive the data
    client.stop( ); // close the connection:
  }
}

char *call_route ( char * method, char * path, char * data ) {
  Serial.print( "Path was " );
  Serial.println( path );
  char * room = strtok( path, "/" );
  char * device = strtok( NULL, "/" ); // strotok is mental
  char * f = strtok( NULL, "/" );
  char cmd[32]; // enough room for whole instruction
  cmd[0] = 0; // null string
  strcat( cmd, "666,!" );
  strcat( cmd, "R" );
  strcat( cmd, room );
  strcat( cmd, "D" );
  strcat( cmd, device );
  strcat( cmd, "F" );
  strcat( cmd, f );
  strcat( cmd, "|testing|arduino!" );
  Udp.beginPacket( lwrfServer, lwrfPort );
  Udp.write( cmd );
  Udp.endPacket( );
  return cmd;
}

char *http_header ( char *status, char *content_type ) {
  BUFFER s = "HTTP/1.1 ";
  strcat( s, status );
  strcat( s, "\nContent-Type: application/json\n\n" );
  return s;
}

char *json_response ( char * response ) {
  BUFFER s = "{\"a\":{";
  for ( int analog_pin = 0; analog_pin < ANALOG_PINS; analog_pin ++ ) {
    if ( analog_pin > 0 ) strcat( s, "," );
    strcat( s, json_pair( analog_pin, analogRead( analog_pin )));
  }
  strcat( s, "},\"d\":{" );
  strcat( s, json_pair( BUTTON, digitalRead( BUTTON )));
  strcat( s, "}" );
  
  strcat( s, ",\"r\":" );
  strcat( s, "\"" );
  strcat( s, response );
  strcat( s, "\"" );

  // strcat( s, ",\"about\":\"x\"" );

  int v = analogRead( THERMOMETER );
  char vs[4] = "";
  Serial.print( "temperature "  );
  v = ( 5 * v * 100.0 ) / 1024.0;
  Serial.println( v );
  itoa( v, vs, 10 );
  strcat( s, ",\"temperature\":" );
  strcat( s, "\"" );
  strcat( s, vs );
  strcat( s, "\"" );
   
  strcat( s, "}" );
  return s;
}

char *json_pair ( int k, int v ) {
  BUFFER s = "";
  char vs[PIN_VAL_MAX_LEN];
  sprintf( vs, "\"%d\"", k );
  strcat( s, vs );
  strcat( s, ":" );
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
