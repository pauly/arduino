const int blueLed = 9;
const int greenLed = 10;
const int whiteLed = 11;
const int yellowLed = 12;
const int redLed = 13;
const int button = 14;
int status = 0;

void setup( ) {
  Serial.begin( 9600 );
  pinMode( redLed, OUTPUT );
  pinMode( whiteLed, OUTPUT );
  pinMode( redLed, OUTPUT );
  pinMode( yellowLed, OUTPUT );
  pinMode( greenLed, OUTPUT );
  pinMode( button, INPUT );
}

void loop( ) {
  // int sensorValue = analogRead( A0 );
  // light( sensorValue );
  int v = digitalRead( button );
  if ( v ) {
    Serial.print( 'We read ' + v + ' from ' + button );
    delay( 10 );
  }
}

void light( int v ) {
  Serial.print( v );
  Serial.print( ':' );
  v = ( 5 * v * 100.0 ) / 1024.0;
  Serial.println( v );
  digitalWrite( blueLed, ( v <= 5 ));
  digitalWrite( greenLed, ( v >= 5 && v <= 10 ));
  digitalWrite( whiteLed, ( v >= 10 && v <= 15 ));
  digitalWrite( yellowLed, ( v >= 15 && v <= 20 ));
  digitalWrite( redLed, ( v >= 20 ));
  delay( 1000 );
}
