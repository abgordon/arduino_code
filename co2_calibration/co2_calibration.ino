 /*
   
/*
CO2 Monitor and Data Logger
SKETCH 1 Calibration 
Code by Martin Liddament

- Used to determine the CO2 sensor voltage output at 400 ppm and 40000ppm
- Follow website instructions 
- Accurate calibration of the CO2 module is essential. 

IMPORTANT

- For demonstration only;
- Not verified or guaranteed to be free of errors 
- Not suitable for critical applications
- Use of this code is entirely at user's risk
- This code is in the public domain.
- Copyright VEETECH Ltd 2013 www.veetech.co.uk

*/
// Setting some initial parameters

int digit[4]; // stores measured voltage for display

// setting digital and analog pins plus delay time
int anode1 = A1;
int anode2 = A2;
int anode3 = A3;
int anode4 = A4;
int ledRed = 13;
int segA = 2;
int segB = 3;
int segC = 5;
int segD = 6;
int segE = 7;
int segF = 8;
int segG = 9;
int analogPin = A0;
int delayTime = 2;


void setup()
{
  Serial.begin(9600);
 // initialize the digital pins as outputs.
  pinMode(anode1, OUTPUT);
  pinMode(anode2, OUTPUT); 
  pinMode(anode3, OUTPUT);
  pinMode(anode4, OUTPUT);
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  pinMode(ledRed, OUTPUT); 
  pinMode(analogPin, INPUT);
  
}
void loop()
{
  int data = analogRead(analogPin); //read output
  float voltage = data/204.6;  //convert output to voltage
  int co2 = voltage * 1000;  // convert voltage to 4 digit integer

  displayAssign(co2); // assign 4 digit integer to display array 
 
  // Reset Display to off
  
  digitalWrite(anode1, LOW);
  digitalWrite(anode2, LOW);
  digitalWrite(anode3, LOW);
  digitalWrite(anode4, LOW); 
  digitalWrite(segA, HIGH);  // HIGH switches segment off
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH); 
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH); 
 
 int i =0;          // each sampling loop takes approx .1 secs
 while (i < 1000)   // to prevent lower digit lights flashing
                    // a while loop (about 10 seconds) is added
  {
 // activate display 
 
  digitalWrite(anode1, HIGH);  // switch on anode 1
  displayDigit(0);             // display the value of the first digit
  digitalWrite(anode1, LOW);   // switch off anode 1
  digitalWrite(anode2, HIGH);
  displayDigit(1);             // display the value of the second digit
  digitalWrite(anode2, LOW);
  digitalWrite(anode3, HIGH);
  displayDigit(2);             // display the value of the third digit         
  digitalWrite(anode3, LOW);
  digitalWrite(anode4, HIGH);
  displayDigit(3);             // display the value of the fourth digit
  digitalWrite(anode4, LOW);
  
  i ++;  // increment while loop   
  }      // end while loop 
 }       // end sampling loop

void displayAssign(int d)    // assigns four figure co2 conc to individual display digits
{
  int number = d;
  for(int i = 3; i >= 0; i--)
{  
  digit[i] = number % 10; // %=modulus remainder from division
  number /=10;            // integer divide
}  // end for loop
}  // enf function


void displayDigit(int dis) // assign value of each digit to the display
{
if (digit[dis] == 0)    
{
disZero();
}
else if (digit[dis] == 1) 
{
disOne();
}
else if (digit[dis] == 2) 
{
disTwo();
}
else if (digit[dis] == 3) 
{
disThree();
}
else if (digit[dis] == 4) 
{
disFour();
}
else if (digit[dis] == 5) 
{
disFive();
}
else if (digit[dis] == 6) 
{
disSix();
}
else if (digit[dis] == 7) 
{
disSeven();
}
else if (digit[dis] == 8) 
{
disEight();
}
else
{
disNine();
}
}


void disZero()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  
}


void disOne()

{
  digitalWrite(segB, LOW); // LOW switches segment on
  digitalWrite(segC, LOW);
  delay(delayTime);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segC, HIGH);
}

void disTwo()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segB, LOW);
  digitalWrite(segG, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segD, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segG, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segD, HIGH);
  
}

void disThree()
{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segB, LOW);
  digitalWrite(segG, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segG, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  
}

void disFour()

{

  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  
  delay(delayTime);
  
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segC, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  
}

void disFive()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  
}

void disSix()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  
}

void disSeven()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segF, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segC, HIGH);
  digitalWrite(segF, HIGH);
  
}

void disEight()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  
}

void disNine()

{
  digitalWrite(segA, LOW); // LOW switches segment on
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  
  delay(delayTime);
  
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH); // High switches segment off
  digitalWrite(segC, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  
}
