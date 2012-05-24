/**
 * If we receive an a, light up for 5 secs, else light up for 1 sec
 * (just so I can see I got a signal)
 *
 * @author  PC <paulypopex+arduino+php@gmail.com>
 * @date    Sun 12 Feb 2012 12:07:51 GMT
 */

const int LED = 12;
int blink = 0;

void setup( ) {      
  pinMode( LED, OUTPUT );
  Serial.begin( 9600 );
}

void loop( ) {
  if ( blink ) {
    digitalWrite( LED, HIGH );   // set the LED on
  }
  delay( blink * 1000 ); // wait
  digitalWrite( LED, LOW );    // set the LED off
  // read the sensor:
  if ( Serial.available( ) >= 2 ) {
    int led = Serial.read( );
    int val = Serial.read( );
    Serial.write( led );
    Serial.print( ' ' );
    Serial.write( val );
    Serial.println( ' ' );
    switch ( led ) {
      case '1':
      case '2':
      case '3':
        blink = val - 48;
        break;
      default:
        blink = 0;
    }
  }
} 

/*
#!/usr/bin/perl

use Device::SerialPort;

my $port = Device::SerialPort->new("/dev/tty.usbmodemfa141");
sleep 2;

$port->databits(8);
$port->baudrate(9600);
$port->parity("none");
$port->stopbits(1);
# $port->handshake("none");
# $port->write_settings || undef $port;
$result = $port->write("a");
*/
