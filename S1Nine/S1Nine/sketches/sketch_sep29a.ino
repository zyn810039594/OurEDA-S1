/*void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}*/

#include <FreeSixIMU.h>
#include <FIMU_ADXL345.h>
#include <FIMU_ITG3200.h>
#include <DFRobot_BMP280.h>
#include <DFRobot_QMC5883.h>
#include <Wire.h>

float angles[3]; // yaw pitch roll
float temperature;
float headingDegrees;
int error = 0;
int val = 0, dete = 0;
//char b1[3],b2[3],b3[3],b4[3],b5[3];
char shoot[23]="$000:000:000:000:000:0%";


FreeSixIMU sixDOF = FreeSixIMU();
DFRobot_QMC5883 compass;
DFRobot_BMP280 bmp;

#define SEA_LEVEL_PRESSURE    1015.0f   // sea level pressure

// show last sensor operate status


// Set the FreeSixIMU object


void getHeading(void)
{
   Vector norm = compass.readNormalize();

  // Calculate heading
  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / PI);
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI);
  heading += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0){
    heading += 2 * PI;
  }

  if (heading > 2 * PI){
    heading -= 2 * PI;
  }

  // Convert to degrees
   headingDegrees = heading * 180/PI; 
}



void  setshoot(){
  
  
  int symbol[3]={0};
  float temp=temperature;
  int deg=(int)headingDegrees;
  
	for (int i = 0; i < 3; i++)
	{
		if(angles[i]<0)
		{
			angles[i] += 360;
		}
      symbol[i]=(int)angles[i];
	}
		
    if(symbol[1]<10 && symbol[1]>0)
     {
        shoot[5]='0';
        shoot[6]='0';
        shoot[7]='0'+symbol[1];
     }
    else if(symbol[1]>=10 && symbol[1]<100)
     {
         shoot[5]='0';
         shoot[6]='0'+symbol[1]/10;
         shoot[7]='0'+symbol[1]%10;
     }
     else if(symbol[1]>=100 && symbol[1]<1000)
     {
         shoot[5]='0'+symbol[1]/100;
         shoot[6]='0'+((symbol[1]/10)%10);
         shoot[7]='0'+symbol[1]%10;
     }

     if(symbol[0]<10 && symbol[0]>0)
     {
        shoot[9]='0';
        shoot[10]='0';
        shoot[11]='0'+symbol[0];
     }
     else if(symbol[0]>=10 && symbol[0]<100)
     {
         shoot[9]='0';
         shoot[10]='0'+symbol[0]/10;
         shoot[11]='0'+symbol[0]%10;
     }
     else if(symbol[0]>=100 && symbol[0]<1000)
     {
         shoot[9]='0'+symbol[0]/100;
         shoot[10]='0'+((symbol[0]/10)%10);
         shoot[11]='0'+symbol[0]%10;
     }
    
     if(symbol[2]<10 && symbol[2]>0)
     {
        shoot[13]='0';
        shoot[14]='0';
        shoot[15]='0'+symbol[2];
     }
    else if(symbol[2]>=10 && symbol[2]<100)
     {
         shoot[13]='0';
         shoot[14]='0'+symbol[2]/10;
         shoot[15]='0'+symbol[2]%10;
     }
     else if(symbol[2]>=100 && symbol[2]<1000)
     {
         shoot[13]='0'+symbol[2]/100;
         shoot[14]='0'+((symbol[2]/10)%10);
         shoot[15]='0'+symbol[2]%10;
     }

      
      if(temp<1 && symbol[0]>0)
     {
        shoot[1]='0';
        shoot[2]='0';
        shoot[3]='0'+temp*10;
     }
    else if(temp>=1 && temp<10)
     {
         shoot[1]='0';
         shoot[2]='0'+(int)temp;
         shoot[3]='0'+(int)(temp*10)%10;
     }
     else if(temp>=10 && temp<100)
     {
         shoot[1]='0'+((int)temp)/10;
         shoot[2]='0'+((int)temp)%10;
         shoot[3]='0'+((int)(temp*10))%10;
     }
    

     if(deg<10 && deg>0)
     {
        shoot[17]='0';
        shoot[18]='0';
        shoot[19]='0'+deg;
     }
    else if(deg>=10 && deg<100)
     {
         shoot[17]='0';
         shoot[18]='0'+deg/10;
         shoot[19]='0'+deg%10;
     }
     else if(deg>=100 && deg<1000)
     {
         shoot[17]='0'+deg/100;
         shoot[18]='0'+((deg/10)%10);
         shoot[19]='0'+deg%10;
     }

     shoot[21]='0'+dete;

}

void   PrintData()
{

  Serial.println(shoot);             

} 

void setup() { 
	Serial.begin(9600);
	Wire.begin();
	delay(5);
	sixDOF.init();  //begin the IMU
	delay(5);
	while (!compass.begin())
	{
		delay(500);
	}
	if (compass.isHMC()) {
		compass.setRange(HMC5883L_RANGE_1_3GA);
		compass.setMeasurementMode(HMC5883L_CONTINOUS);
		compass.setDataRate(HMC5883L_DATARATE_15HZ);
		compass.setSamples(HMC5883L_SAMPLES_8);
	}
	else if (compass.isQMC()) {
		compass.setRange(QMC5883_RANGE_2GA);
		compass.setMeasurementMode(QMC5883_CONTINOUS); 
		compass.setDataRate(QMC5883_DATARATE_50HZ);
		compass.setSamples(QMC5883_SAMPLES_8);
	}
	//while (!bmp.begin()) {  
	//	Serial.println("Error initializing BMP280!");
	//	delay(1000);
	//}
}



void loop() { 

  sixDOF.getEuler(angles);

	//temperature = bmp.readTemperatureValue();

  compare();

  getHeading();

  setshoot();

  PrintData();

  delay(200);
}


void compare()
{
  val = analogRead(A3);
  if(val < 410){
    dete = 0;
  }
  else{
    dete = 1;
  }
}
