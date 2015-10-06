// define humidity variable to hold the final value
int humidity = 0;

void setup() {
 Serial.begin(9600);
}
void loop() {
 // read soil moisture from sensor
 int sensorValue = analogRead(A0);
 
 /* constraint function will limit the values we get so we can work better with map
 * since I only need values in the bottom limit of 300, will make it the outer limit and 1023 the other limit
 */
 sensorValue = constrain (sensorValue, 300,1023);
 // print the values returned by the sensor
 Serial.println(sensorValue);
 // create a map with the values
 // You can think we have the 100% to 300 and 0 % to 1023 wrong, but no !
 // 300 - or 100 % - is the target value for moisture value in the soil
 // 0% of humidity - or moisture - is when the soil is dry
 humidity = map (sensorValue, 300, 1023, 100, 0);
 Serial.print (humidity);
 Serial.println ("%");
 delay(1000); //collecting values between seconds
 }
