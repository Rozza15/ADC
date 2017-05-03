/*                                                                     //
 * Basic Air Data Computer giving Altitude and Airspeed                //
 * By Rory Buchanan                                                    //
 */                                                                    //
                                                                       //
#include     <SFE_BMP180.h>                                            //
#include           <Wire.h>                                            //
                                                                       //
#define standPress  1013.25                                            //  hPa
#define pressLapse    30.0                                             //  1hPa/30ft
#define  standTemp    15.0                                             //  ISA temp @ MSL
#define  tempLapse     1.98                                            //  °C/1000ft
#define tempFactor   120.0                                             //  1°C = 120ft density altitude factor
#define     ms2kts     0.514                                           //  m/s to knots conversion factor
#define   gasConst   287.05                                            //  specific gas constant for dry air J/(kg.K)
#define     kelvin   273.15                                            //  Kelvin to Celcius factor
                                                                       //
//#define        QNH  1013.25                                          //
#define   elevation  160                                               //  Elevation @ 23 Eyre Street
                                                                       //
SFE_BMP180 pressure                                               ;    //
                                                                       //
void setup (){                                                         //
  Serial.begin(9600)                                              ;    //
  pressure.begin()                                                ;    //
}                                                                      //
                                                                       //
void loop () {                                                         //
  Serial.print("Pressure Altitude  -  ")                          ;    //
  Serial.print(pressAlt())                                        ;    //
  Serial.print("'")                                               ;    //
  Serial.println()                                                ;    //
  Serial.print("              QNH  -  ")                          ;    //
  Serial.print(QNH())                                             ;    //
  Serial.print("hPa")                                             ;    //
  Serial.println()                                                ;    //
  Serial.print("             Temp  -  ")                          ;    //
  Serial.print(actTemp())                                         ;    //
  Serial.print("`C")                                              ;    //
  Serial.println()                                                ;    //
  Serial.print("  ISA Temperature  -  ")                          ;    //
  Serial.print(isaTemp())                                         ;    //
  Serial.print("`C")                                              ;    //
  Serial.println()                                                ;    //
  Serial.print("    ISA Deviation  -  ")                          ;    //
  Serial.print(isaDev())                                          ;    //
  Serial.println()                                                ;    //
  Serial.print(" Density Altitude  -  ")                          ;    //
  Serial.print(densityAlt())                                      ;    //
  Serial.print("'")                                               ;    //
  Serial.println()                                                ;    //
  Serial.print("              IAS  -  ")                          ;    //
  Serial.print(/*airSpeed(),*/ 0)                                     ;    //
  Serial.print(" KIAS")                                           ;    //
  Serial.println()                                                ;    //
  Serial.print("      Air Density  -  ")                          ;    //
  Serial.print(airDensity())                                      ;    //
  Serial.print("kg/m^3")                                          ;    //
  Serial.println()                                                ;    //
  Serial.print("    Altitude AMSL  -  ")                          ;    //
  Serial.print(indicatedAlt())                                    ;    //
  Serial.println()                                                ;    //
  Serial.print("  Static Pressure  -  ")                          ;    //
  Serial.print(statPress())                                       ;    //
  Serial.print("hPa")                                             ;    //
  Serial.println()                                                ;    //
  delay(5000)                                                     ;    //
  Serial.println()                                                ;    //
}                                                                      //
                                                                       //
double statPress () {                                                  //
  char status                                                     ;    //
  double T, P, p0, a                                              ;    //
  status = pressure.startTemperature()                            ;    //
  if (status != 0) {                                                   //
    delay(status)                                                 ;    //
    status = pressure.getTemperature(T)                           ;    //
    if (status != 0){                                                  //
      delay(status)                                               ;    //
      status = pressure.startPressure(3)                          ;    //
      if (status != 0) {                                               //
        delay(status)                                             ;    //
        pressure.getPressure(P,T)                                 ;    //
        if (status != 0) {                                             //
          return(P)                                               ;    //
        }                                                              //
      }                                                                //
    }                                                                  //
  }                                                                    //
}                                                                      //
                                                                       //
double actTemp () {                                                    //
  char status                                                     ;    //
  double T, P, p0, a                                              ;    //
  status = pressure.startTemperature()                            ;    //
  if (status != 0) {                                                   //
    delay(status)                                                 ;    //
    status = pressure.getTemperature(T)                           ;    //
    if (status != 0) {                                                 //
      return(T)                                                   ;    //
    }                                                                  //
  }                                                                    //
}                                                                      //
                                                                       //
float indicatedAlt () {                                                //
  float iAlt                                                      ;    //
  iAlt = pressLapse * (QNH() - statPress())                       ;    //
  return iAlt                                                     ;    //
}                                                                      //
                                                                       //
float QNH () {                                                         //
  float q                                                         ;    //
  q = (elevation/pressLapse) + statPress()                        ;    //
  return q                                                        ;    //
}                                                                      //
                                                                       //
float pressAlt () {                                                    //
  float pAlt                                                      ;    //
  pAlt = pressLapse * (standPress - statPress())                  ;    //
  return pAlt                                                     ;    //
}                                                                      //
                                                                       //
float isaTemp () {                                                     //
  float iTemp                                                     ;    //
  iTemp = standTemp - tempLapse * (pressAlt()/1000)               ;    //
  return iTemp                                                    ;    //
}                                                                      //
                                                                       //
float isaDev () {                                                      //
  float dev                                                       ;    //
  dev = actTemp() - isaTemp()                                     ;    //
  return dev                                                      ;    //
}                                                                      //
                                                                       //
float densityAlt () {                                                  //
  float dAlt                                                      ;    //
  dAlt = pressAlt () + isaDev () * tempFactor                     ;    //
  return dAlt                                                     ;    //
}                                                                      //
/*                                                                     //
float airSpeed () {                                                    //
  float vsquared                                                  ;    //  air speed from P = 1/2 rho v^2
  float msias                                                     ;    //  v = sqrt(2P/rho)
  float kias                                                      ;    //  where P is total pressure and rho is air density from airDensity() function
  float totPress                                                  ;    //
  float dynPress                                                  ;    //
  float rkias                                                     ;    //
  totPress = statPress()                                          ;    //
  dynPress = totPress - statPress()                               ;    //
  vsquared = ((2 * (dynPress)) / airDensity ())                   ;    //
  msias = sqrt(vsquared)                                          ;    //
  kias = msias / ms2kts                                           ;    //
  rkias = round(kias)                                             ;    //
  if (rkias < 0) {                                                     //
    return 0                                                      ;    //
  } else {                                                             //
    return rkias                                                  ;    //
  }                                                               ;    //
}                                                                      //
*/                                                                     //
float airDensity () {                                                  //  air density from pressure in Pascals
  float aDen                                                      ;    //  based on specific gas constant for dry air
  float aDenPa                                                    ;    //
  aDen = (statPress() * 100) / (gasConst * kelvTemp ())           ;    //
  aDenPa = aDen                                                   ;    //
  return aDenPa                                                   ;    //  kg/m³
}                                                                      //
                                                                       //
float kelvTemp () {                                                    //
  float kelv                                                      ;    //
  kelv = actTemp() + kelvin                                       ;    //
  return kelv                                                     ;    //
}                                                                      //
