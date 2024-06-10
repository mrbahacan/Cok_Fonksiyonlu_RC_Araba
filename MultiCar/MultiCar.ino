// Gerekli kütüphaneleri ekliyoruz
#include <LiquidCrystal_I2C.h>
#include <AFMotor.h>
#include <SoftwareSerial.h>

// LCD ve ultrasonik sensör pin tanımlamaları
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define echoPin 7
#define trigPin 8
SoftwareSerial bt_iletisim(0, 1);

// Motorlar için nesneler oluşturuyoruz
AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

// Global değişkenler
int hiz = 100;
bool btMod = true;
unsigned long previousMillis = 0;    // Son ölçüm zamanını saklamak için
const long interval = 2000;          // 2 saniyelik interval (2000 milisaniye)

void setup() {
  // Seri ve Bluetooth iletişimi başlatıyoruz
  Serial.begin(9600);
  bt_iletisim.begin(9600);

  // Ultrasonik sensör pinlerini ayarlıyoruz
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  // Motorları durduruyoruz
  dur();

  // LCD ekranı başlatıyoruz ve hoş geldin mesajını yazdırıyoruz
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("KAYU BILGISAYAR");
  lcd.setCursor(0, 1);
  lcd.print("PROGRAMCILIGI");
}

void loop() {
  // Bluetooth modu kontrol ediliyor
  if (btMod) {
    bt_kontrol();
  } else {
    engelden_kacan();
  }
}

// Motorları ileri yönde çalıştıran fonksiyon
void ileri() {
  motor1.setSpeed(hiz);
  motor1.run(FORWARD);
  motor2.setSpeed(hiz);
  motor2.run(FORWARD);
  motor3.setSpeed(hiz);
  motor3.run(FORWARD);
  motor4.setSpeed(hiz);
  motor4.run(FORWARD);
}

// Motorları geri yönde çalıştıran fonksiyon
void geri() {
  motor1.setSpeed(hiz);
  motor1.run(BACKWARD);
  motor2.setSpeed(hiz);
  motor2.run(BACKWARD);
  motor3.setSpeed(hiz);
  motor3.run(BACKWARD);
  motor4.setSpeed(hiz);
  motor4.run(BACKWARD);
}

// Motorları sola döndüren fonksiyon
void sol() {
  motor1.setSpeed(hiz);
  motor1.run(FORWARD);
  motor2.setSpeed(hiz);
  motor2.run(FORWARD);
  motor3.setSpeed(hiz);
  motor3.run(BACKWARD);
  motor4.setSpeed(hiz);
  motor4.run(BACKWARD);
}

// Motorları sağa döndüren fonksiyon
void sag() {
  motor1.setSpeed(hiz);
  motor1.run(BACKWARD);
  motor2.setSpeed(hiz);
  motor2.run(BACKWARD);
  motor3.setSpeed(hiz);
  motor3.run(FORWARD);
  motor4.setSpeed(hiz);
  motor4.run(FORWARD);
}

// Motorları durduran fonksiyon
void dur() {
  motor1.setSpeed(0);
  motor1.run(RELEASE);
  motor2.setSpeed(0);
  motor2.run(RELEASE);
  motor3.setSpeed(0);
  motor3.run(RELEASE);
  motor4.setSpeed(0);
  motor4.run(RELEASE);
}

// Bluetooth ile kontrol fonksiyonu
void bt_kontrol() {
  if (bt_iletisim.available()) {
    char komut = bt_iletisim.read();
    Serial.println(komut);
    dur();

    if (komut == 'F') {
      ileri();
    } else if (komut == 'B') {
      geri();
    } else if (komut == 'L') {
      sol();
    } else if (komut == 'R') {
      sag();
    } else if (komut == 'x' || komut == 'X') {
      btMod = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ENGELDEN KACAN");
      lcd.setCursor(0, 1);
      lcd.print("MODU AKTIF");
    } else if (komut == 'w' || komut == 'W') {
      btMod = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("UZAKTAN KUMANDA");
      lcd.setCursor(0, 1);
      lcd.print("MODU AKTIF");
    } else if (komut == 'u') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("KAYU BILGISAYAR");
      lcd.setCursor(0, 1);
      lcd.print("PROGRAMCILIGI");
    } else if (komut == 'U') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("BILGISAYARLI");
      lcd.setCursor(0, 1);
      lcd.print("KONTROL DERSI");
    } else if (komut >= '0' && komut <= '9') {
      hiz = map(komut - '0', 0, 9, 100, 1023);
    } else {
      dur();
    }
  }
}

// Engelden kaçma fonksiyonu
void engelden_kacan() {
  unsigned long currentMillis = millis(); // Şu anki zaman

  if (bt_iletisim.available()) {
    char dgr = bt_iletisim.read();
    if (dgr == 'x' || dgr == 'X') {
      btMod = false;
    } else if (dgr == 'w' || dgr == 'w') {
      btMod = true;
    }

    // 2 saniye geçtiyse ölçüm yap ve sonucu yazdır
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  // previousMillis'i güncelle

      // Ultrasonik sensör ile mesafe ölçülüyor
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      int sure = pulseIn(echoPin, HIGH);
      int uzaklik = sure / 29.1 / 2;

      // Eğer engel algılanırsa, araç engelden kaçmak için sola dönüyor
      if (uzaklik <= 15) {
        hiz = 150;
        ileri();
        delay(200);
        sol();
        delay(200);
      } else {
        // Engelden uzaksa, geri gidiyor
        hiz = 150;
        geri();
      }
    }
  }
}
