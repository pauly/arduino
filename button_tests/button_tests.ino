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

#include <SPI.h>
#include <Ethernet.h>

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
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connnection: close");
          client.println();
          client.print("{\"a\":{");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            client.print( analogChannel ? "," : "" );
            int sensorReading = analogRead(analogChannel);
            client.print("\"");
            client.print(analogChannel);
            client.print("\":");
            client.print(sensorReading);
          }
          client.print("},\"d\":{\"");
          client.print(BUTTON);
          client.print("\":");
          client.print(digitalRead(BUTTON));
          client.print("}}");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
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
