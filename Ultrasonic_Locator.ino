#include <Servo.h>

#define pinTrig 2
#define pinAns 12
#define pinBtnSweep 11
#define pinBtnClosest 10
#define pinBtnLeft 9
#define pinBtnRight 8
#define pinServo 5

Servo myServo;

int posServo = 0;         // Szervó pozíciója
int posClosest = 0;       // Legközelebbi tárgy pozíciója
float distance = 0;       // Tárgy távolsága
float closestDist = 450;  // Legközelebbi tárgy távolsága, kezdőérték a leírás szerinti legtávolabbi érték, 450 cm

void setup() {
  pinMode(pinBtnSweep, INPUT_PULLUP);   // Pásztázó gomb
  pinMode(pinBtnLeft, INPUT_PULLUP);    // Manuális balra
  pinMode(pinBtnRight, INPUT_PULLUP);   // Manuális jobbra
  pinMode(pinBtnClosest, INPUT_PULLUP); // Legközelebbi tárgyra ugró gomb
  pinMode(pinTrig, OUTPUT);
  pinMode(pinAns, INPUT);
  Serial.begin(9600);
  myServo.attach(pinServo);
  myServo.write(0);
}

void loop() {
  static bool btnPrevSweep = true;
  static bool btnPrevClosest = true;
  bool btnStateSweep = digitalRead(pinBtnSweep);
  bool btnStateClosest = digitalRead(pinBtnClosest);
  
  if(!btnStateSweep & btnPrevSweep)
    {
      Serial.println("$$");
      closestDist = 450;
      
      for (posServo = 0; posServo <= 180; posServo += 1) // Egyik irányba forog, 150 fok a maximális szöge, de 180 fokon kell megjeleníteni
      {
        myServo.write(posServo);
        delay(25);
        calculateAndSend();    // Az ultrahangos érzékelő bekapcsolása, adatok feldolgozása, cm-be alakítása és elküldése
        isClosest();           // A legközelebbi pozíció ellenőrzése és elmentése
        delay(25);
      }

      myServo.write(posClosest);
      posServo = posClosest;
      delay(15);
    }

  btnPrevSweep = btnStateSweep;

  if(digitalRead(pinBtnLeft) == LOW & posServo < 180) //Manuális forgatás balra             !!!!!150 fokig tud csak fordulni!
  {
      posServo = posServo + 1;
      myServo.write(posServo);
      delay(20);
      calculateAndSend();
      isClosest();
      delay(25);
  }

  if(digitalRead(pinBtnRight) == LOW & posServo > 0)  //Manuális forgatás jobbra
  {
      posServo = posServo - 1;
      myServo.write(posServo);
      delay(20);
      calculateAndSend();
      isClosest();
      delay(25);
  }

  if(!btnStateClosest & btnPrevClosest) // A legközelebbi tárgyra fordul
  {
    myServo.write(posClosest);
    posServo = posClosest;
    delay(15);
  }

  btnPrevClosest = btnStateClosest;
  delay(25);
}

void calculateAndSend()
{ 
  long duration;
  Serial.flush();                         // Kiürítjük a küldő buffert
  delay(15);
  digitalWrite(pinTrig, LOW);             // Megbizonyodás a szonár kikapcsolt állapotáról
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);            // A szonár bekapcsolása 10 mikroszekundumig
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);             // A szonár kikapcsolása
  duration = pulseIn(pinAns, HIGH);       // A válasz időbeni beérkezésének beolvasása (mikroszekundum)
  distance = duration * 0.0343 / 2;       // Az idő átalakítása távolságra (cm) ((IDŐ/2 * HANGSEBESSÉG))
  Serial.print(posServo);                 // A szervó szögének továbbítása (a Labview felé)
  Serial.print("@");
  Serial.println(distance);               // A távolság továbbítása (a Labview felé)
  delay(15);
}

void isClosest()
{
  if(distance < closestDist)
  {
    closestDist = distance;
    posClosest = posServo;
  }
}