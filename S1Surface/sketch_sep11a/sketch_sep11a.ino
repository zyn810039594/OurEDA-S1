#include <FlexiTimer2.h>

//除了侧推，其他都是模拟口，A0到A5接收电压，转换成0-1023，再用MAP函数转换
String commanderAll ="";
String BowThruster ="";//侧推  0/1
String OpenLight ="";//开灯 0/1
String GoBack ="";//前进后退  0-200
String LeftRight ="";//左右 0-200
String UpDown ="";//上下 0-200
String Bright ="";//灯光 1100-1900
String Camera ="";//云台 500-2500
String Claw ="";//爪子 500-2500
int BowThrusterTemp =0;//侧推  0/1
int OpenLightTemp =0;//开灯 0/1
int GoBackTemp =0;//前进后退  0-200
int LeftRightTemp =0;//左右 0-200
int UpDownTemp =0;//上下 0-200
int BrightTemp =0;//灯光 1100-1900
int CameraTemp =0;//云台 500-2500
int ClawTemp =0;//爪子 500-2500

void TimerInterrupt()
{
  BowThruster.concat(BowThrusterTemp);  //侧推命令
  BowThruster = '$' + BowThruster + ':';
  GoBackTemp = map(GoBackTemp,0,1023,60,140);  //前后命令
  if((GoBackTemp>=95)&&(GoBackTemp<=105))
  {
    GoBackTemp=100;
  }
  GoBack.concat(GoBackTemp);
  if(GoBack.length()==2)
  {
    GoBack = '0'+GoBack+':';
  }
  else if(GoBack.length()==1)
  {
    GoBack = "00"+GoBack+':';
  }
  else
  {
    GoBack = GoBack+':';
  }
  LeftRightTemp = map(LeftRightTemp,0,1023,140,60);  //左右命令
  if((LeftRightTemp>=95)&&(LeftRightTemp<=105))
  {
    LeftRightTemp=100;
  }
  LeftRight.concat(LeftRightTemp);
  if(LeftRight.length()==2)
  {
    LeftRight = '0'+LeftRight+':';
  }
  else if(LeftRight.length()==1)
  {
    LeftRight = "00"+LeftRight+':';
  }
  else
  {
    LeftRight = LeftRight+':';
  }
  UpDownTemp = map(UpDownTemp,0,1023,140,60);  //上下命令
  if((UpDownTemp>=95)&&(UpDownTemp<=105))
  {
    UpDownTemp=100;
  }
  UpDown.concat(UpDownTemp);
  if(UpDown.length()==2)
  {
    UpDown = '0'+UpDown+':';
  }
  else if(UpDown.length()==1)
  {
    UpDown = "00"+UpDown+':';
  }
  else
  {
    UpDown = UpDown+':';
  }
  BrightTemp = map(BrightTemp,0,1023,4999,0);  //灯光命令
  Bright.concat(BrightTemp);
  if(OpenLightTemp == 1)
  {
    if(Bright.length()==4)
    {
      Bright = Bright+':';
    }
    else if(Bright.length()==3)
    {
      Bright = "0"+Bright+':';
    }
    else if(Bright.length()==2)
    {
      Bright = "00"+Bright+':';
    }
    else if(Bright.length()==1)
    {
      Bright = "000"+Bright+':';
    }
    else if(Bright==0)
    {
      Bright = "0000:";
    }
  }
  else if(OpenLightTemp == 0)
  {
    Bright = "0000:";
  }
  CameraTemp = map(CameraTemp,0,1023,2100,1100);  //云台命令
  Camera.concat(CameraTemp);
  if(Camera.length()==3)
  {
    Camera = '0'+Camera+':';
  }
  else
  {
    Camera = Camera+':';
  }
  ClawTemp = map(ClawTemp,0,1023,500,2500);  //爪子命令
  Claw.concat(ClawTemp);
   if(Claw.length()==3)
  {
    Claw = '0'+Claw+'%';
  }
  else
  {
    Claw = Claw+'%';
  }
  commanderAll = BowThruster+LeftRight+GoBack+UpDown+Bright+Camera+Claw;
  Serial.print(commanderAll);
  commanderAll = "";
  BowThruster = "";
  OpenLight = "";
  GoBack = "";
  LeftRight = "";
  UpDown = "";
  Bright =  "";
  Camera =  "";
  Claw = "";
}
void setup() {

  Serial.begin(9600); //设置串口波特率9600
  pinMode(2,INPUT);  //侧推
  pinMode(3,INPUT);  //开灯
//  pinMode(A0,INPUT);  //前进后退
//  pinMode(A1,INPUT);  //左右
//  pinMode(A2,INPUT);  //上下
//  pinMode(A3,INPUT);  //云台
//  pinMode(A4,INPUT);  //爪子
//  pinMode(A5,INPUT);  //灯光 
  delay(4000);
  Serial.print("$9:000:000:000:0000:0000:0000%");
  FlexiTimer2::set(40, TimerInterrupt);
  FlexiTimer2::start();
}

void loop() {

    BowThrusterTemp =digitalRead(2);//侧推  0/1 
    OpenLightTemp =digitalRead(3);//开灯  0/1     
    GoBackTemp = analogRead(A0);//前进后退  0-200    
    LeftRightTemp = analogRead(A1);//左右 0-200
    UpDownTemp = analogRead(A2);//上下 0-200
    CameraTemp = analogRead(A3);//云台 500-2500    
    ClawTemp = analogRead(A4);//爪子 500-2500    
    BrightTemp = analogRead(A5);//灯光 1100-1900    
    delay(2);
}
