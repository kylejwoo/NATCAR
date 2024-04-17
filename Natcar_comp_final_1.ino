#include <Servo.h>

Servo myservo;
int scanned[128]; //Creating array to store data input from the camera
int number = 0; //Declaring the integer number as a global variable (line 44)
int middle;  //Declaring the integer middle as a global variable (line 58)
int beginline = 64; //Variable set
int range = 35;
int motorSpeed = 150; //Variable for motor speed set
int in = 0;
double dt = 0.0040404;
//channel 1 is only steering
//channel 2 is only throttle
//channel 3 is On Manual or On Auto
void setup() {
  pinMode(A1, INPUT); //Â0 is input for camera: A
  pinMode(4, OUTPUT); //4 is SI: B
  pinMode(8, OUTPUT); //8 is CLK: C
  Serial.begin(250000);
  pinMode(9, INPUT); // C 1
  pinMode(10, INPUT); // C 2
  pinMode(11, INPUT); // C 3
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  
}
void loop() {
  double t = micros();
  myservo.attach(6);
  //pulseIn reads the pulse that the reciever is getting (different for each channel)
  if (pulseIn(11, HIGH) > 1300) { // if true, MANUAL
    
    if (pulseIn(10, HIGH) > 1750) {
      analogWrite(3, 255); //set motor speed to 70
    }
    else {
      analogWrite(3, 0); //set motor speed to 0 using analog
    }
    myservo.write(map(pulseIn(9, HIGH), 1232, 1860, 33, 103)); //maps the range of the left range and right range #s and converts to servo length
  }
  else if(pulseIn(11, HIGH) > 400) {
    myservo.attach(6);
    //SI stands for serial in; tells the camera to start taking the picture
    //CLK stands for clock; actualy takes the picture and holds its values
    digitalWrite(4, HIGH); //SI Starting
    digitalWrite(8, HIGH); //CLK Starting
    digitalWrite(4, LOW); //SI
    digitalWrite(8, LOW); //CLK
    number = 0; //creating a variable to add up all the light intensity for the average
    for (int i = 0; i <= 127; i++) { //Checking every pixel (127  pixels) from the camera
      digitalWrite(8, HIGH); //CLK taking pictures
      digitalWrite(8, LOW); //CLK taking pictures
      scanned[i] = analogRead(A1); //array to save the past light intensity
      number = number + scanned[i]; //adding the value [i] from the scanned array to the integer number
    }
    middle = (number / 127) * 1.01; //(1.01) creating a variable as the average
    int linefinder[128]; //Creating an array to check for the start of this line
    for (int check = 0; check <= 127; check++) {
      if (scanned[check] > middle) { //checking if value inputed by the camera is greater than average
        //Serial.print(" "); //white
        linefinder[check] = 0; //Storing variable
      }
      else if (scanned[check] <= middle) { //if value inputted from camera is les than average
        //Serial.print("0"); //Outputs 0 if light reflected if less than middle or “black”
        linefinder[check] = 1; //Inputs 0, or dark, into the linefinder array
      }
    }
    for (int findchecker = 0; findchecker <= 124; findchecker++) { //for loop that stops at 124 because otherwise the linefinder could set the line beggining as something over 127+
      if (linefinder[findchecker] == 1 && linefinder[findchecker + 1] == 0  && linefinder[findchecker + 2] == 0 && linefinder[findchecker + 3] == 0 ) { //checks where the black ends and where the white starts
        beginline = findchecker + 1; //sets the integer beginline to where the line begins
        break; //breaks out of for loops or else it will run unessescarilly
      }
    }
    //Serial.print(" ");
    //Serial.print( beginline); //prints out where the beginning of the line is
    //Serial.print("    ");
    
    int error = beginline - 45; //45 is approximate center
    //Serial.print(error);
    in = in + (error * 1.093);
    if (in > 565) {
      in = 565;
    }
    if (in < -565) {
      in = -565;
    }
    error = error + (in * 0.0668);
    delayMicroseconds(dt);
    /*
    if (error > 58) { //checking if beginline (being mapped to servo) is over 180 (can change after testing)
      error = 58; //sets maximum turning of servo to 64 so that it does not break the wheels
    }
    if (error < -58) { // checking if the servo will die
      error = -58; // sets minimum turning of servo to -64
    }
    */
    if(error < 4 &&  error > -4){
      error = 0;
     
    }
    //Serial.print("PID val: ");
    //Serial.print(error);
    analogWrite(3, map(abs(error), 80, 0, motorSpeed - range, motorSpeed));
    error = map(error, -90, 90, 30, 103); //mapping the beginning of the line to the servo
    //Serial.print("                 Servo angle:" );
    myservo.write(error); //write in new beginline value into servo
    //Serial.print(" "); //spaces out
    //Serial.println(error); //prints out the value
    //Serial.print(" ");
    //Serial.print(in);
    //Serial.println(); //newline
    //MOTOR:
    //analogWrite(3, motorSpeed);
    
    
  }
  else{
    analogWrite(3, 0);
    myservo.detach();
    
    
  }
 }
