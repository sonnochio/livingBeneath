#define SIG_PIN_1 2  // Signal pin for the first ultrasonic sensor
#define SIG_PIN_2 3  // Signal pin for the second ultrasonic sensor
// #define SIG_PIN_3 4  // Signal pin for the third ultrasonic sensor
// #define SIG_PIN_4 5  // Signal pin for the fourth ultrasonic sensor

void setup() {
  Serial.begin(31250);          // Start serial communication at 9600 baud rate
  pinMode(SIG_PIN_1, OUTPUT);  // Set the first SIG pin as an output
  pinMode(SIG_PIN_2, OUTPUT);  // Set the second SIG pin as an output
  // pinMode(SIG_PIN_3, OUTPUT);  // Set the third SIG pin as an output
  // pinMode(SIG_PIN_4, OUTPUT);  // Set the fourth SIG pin as an output
}

float smoothedDistance1, smoothedDistance2, smoothedDistance3, smoothedDistance4;
float smoothFactor = 0.5;
float average1[5], average2[5], average3[5], average4[5];
float currentPressure = 3;
float lastPressure = 3;
int index = 0;
unsigned long lastTime = 0;
float maxLocation = 10;
float triggerValue = 60; // value to start change

void loop() {
  // if (millis() - lastTime >= 10) {  // 100 ms delay between readings
  //   lastTime = millis();

    // Read from each ultrasonic sensor and calculate smoothed distance
    smoothedDistance1 = calculateSmoothedDistance(SIG_PIN_1, smoothedDistance1);
    smoothedDistance2 = calculateSmoothedDistance(SIG_PIN_2, smoothedDistance2);
    // smoothedDistance3 = calculateSmoothedDistance(SIG_PIN_3, smoothedDistance3);
    // smoothedDistance4 = calculateSmoothedDistance(SIG_PIN_4, smoothedDistance4);

    // Update the running average arrays
    average1[index] = smoothedDistance1;
    average2[index] = smoothedDistance2;
    // average3[index] = smoothedDistance3;
    // average4[index] = smoothedDistance4;
    
    if (++index >= 5) index = 0;  // Reset index after 5 readings

    // Calculate the averages
    float smoothedDistanceAverage1 = calculateAverage(average1);
    float smoothedDistanceAverage2 = calculateAverage(average2);
    // float smoothedDistanceAverage3 = calculateAverage(average3);
    // float smoothedDistanceAverage4 = calculateAverage(average4);


   if  (smoothedDistanceAverage1 < triggerValue && smoothedDistanceAverage3<triggerValue){
      currentPressure = 9;
      graduatePrint(smoothedDistanceAverage1, smoothedDistanceAverage3, currentPressure, lastPressure);
      lastPressure = currentPressure;
    }
    else if (smoothedDistanceAverage1 < triggerValue && smoothedDistanceAverage3 > triggerValue){
      currentPressure = 9;
      graduatePrint(smoothedDistanceAverage1, smoothedDistanceAverage4, currentPressure, lastPressure);
      lastPressure = currentPressure;

    }
    else if (smoothedDistanceAverage1 > triggerValue && smoothedDistanceAverage3 < triggerValue){
      currentPressure = 9;
      graduatePrint(smoothedDistanceAverage2, smoothedDistanceAverage3, currentPressure, lastPressure);
      lastPressure = currentPressure;

    }
    else if (smoothedDistanceAverage2 < 60 && smoothedDistanceAverage4 < 60) {
      currentPressure = 5;
      graduatePrint(smoothedDistanceAverage2, smoothedDistanceAverage4, currentPressure, lastPressure);
      lastPressure = currentPressure;
    }
    else {
      currentPressure = 3;
      serialPrint(0, 0, 3);
      lastPressure = currentPressure;

    }


    // Print averaged distances on the Serial Monitor
    Serial.print("Average Distance A: ");
    Serial.print(smoothedDistanceAverage1);
    Serial.print(" cm, Average Distance B: ");
    Serial.print(smoothedDistanceAverage2);
    Serial.print(" cm, Average Distance C: ");
    Serial.print(smoothedDistanceAverage3);
    Serial.print(" cm, Average Distance D: ");
    Serial.print(smoothedDistanceAverage4);
    Serial.println(" cm");
  
}

float calculateSmoothedDistance(int sigPin, float previousSmoothedDistance) {
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sigPin, LOW);
  pinMode(sigPin, INPUT);
  long duration = pulseIn(sigPin, HIGH);
  pinMode(sigPin, OUTPUT);
  float distance = duration * 0.034 / 2;
  return (distance * smoothFactor) + (previousSmoothedDistance * (1 - smoothFactor));
}

void serialPrint(float x, float y, float pressure){
  Serial.print(x);
  Serial.print(',');
  Serial.print(y);
  Serial.print(',');
  Serial.println(pressure);

}


  void graduatePrint(float x, float y, float currentPressure, float lastPressure){
    x=x*maxLocation/70;
    y=y*maxLocation/70;
      if (currentPressure > lastPressure){
        for (int i = lastPressure; i<=currentPressure; i++){
        serialPrint(x, y, i);
        // delay(10);
        }
      }
      else if (currentPressure < lastPressure){
        for (int i = lastPressure; i >= currentPressure; i--){
        serialPrint(x, y, i);
        // delay(10);
        }
      } 
      else {
        serialPrint(x,y,currentPressure);
      }
}


float calculateAverage(float arr[]) {
  float sum = 0.0;
  for (int i = 0; i < 5; i++) {
    sum += arr[i];
  }
  return sum / 5;
}
