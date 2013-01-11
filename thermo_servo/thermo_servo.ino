#define MINC 10
#define MAXC 30
#define TOOHOT 49

#define MAXD 180

#include <Servo.h> 
 
Servo myservo;
int previous ;

void setup( ) {
  Serial.begin( 9600 );
  myservo.attach( 3 );
  previous = 0;
}

void loop( ) {
  // turn_to_angle_for_temperature( v, myservo );
  open_window_if_to_hot( A0, myservo );
  delay( 1000 );
}

int c ( int v ) { // convert input to degrees c
  return ( 5 * v * 100.0 ) / 1024.0;
}

int d ( int v ) { // convert degrees c to degrees of angle
  return (( v - MINC ) * MAXD ) / MAXC;
}

void open_window_if_to_hot( int pin, Servo s ) {
  int v = analogRead( pin );
  if ( v != previous ) {
    debug_temperature( v );
    if ( c( v ) >= TOOHOT ) {
      Serial.println( " so opening window" );
      s.write( 180 ); 
    }
    else {
      Serial.println( " so closing" );
      s.write( 0 ); 
    }
  }
  previous = v;
}

void turn_to_angle_for_temperature( int pin, Servo s ) {
  int v = analogRead( pin );
  int angle = d( c( v ));
  if ( angle != previous ) {
    debug_temperature( v );
    Serial.print( " so angle is " );
    Serial.println( angle );
    s.write( angle ); 
  }
  previous = angle;
}

void debug_temperature ( int v ) {
  Serial.print( "input was " );
  Serial.print( v );
  Serial.print( " so temperature is " );
  Serial.print( c( v ));
}
