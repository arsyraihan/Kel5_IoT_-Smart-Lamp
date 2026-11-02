#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_SUARA   34
#define PIN_TOMBOL  27
#define PIN_RELAY   26

const int AMBANG = 2500;
const unsigned long JEDA = 500;

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool lampu = false;
int jumlahTepuk = 0;
bool atasAmbangSebelumnya = false;
unsigned long waktuTepukTerakhir = 0;
unsigned long waktuUbahStatus = 0;
bool tombolSebelumnya = HIGH;
unsigned long waktuTombolTerakhir = 0;

void tampilkan() {
  unsigned long d = waktuUbahStatus / 1000;
  char baris2[17];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lampu:");
  lcd.print(lampu ? "ON " : "OFF");
  lcd.print(" T:");
  lcd.print(jumlahTepuk);
  lcd.setCursor(0, 1);
  snprintf(baris2, sizeof(baris2), "Ubah: %02lu:%02lu", d / 60, d % 60);
  lcd.print(baris2);
}

void setLampu(bool nyala, const char* sumber) {
  lampu = nyala;
  digitalWrite(PIN_RELAY, lampu ? HIGH : LOW);
  waktuUbahStatus = millis();
  Serial.printf("[%lu detik] Lampu %s oleh %s | Jumlah tepukan: %d\n",
                waktuUbahStatus / 1000, lampu ? "ON" : "OFF", sumber, jumlahTepuk);
  tampilkan();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  digitalWrite(PIN_RELAY, LOW);
  lcd.init();
  lcd.backlight();
  tampilkan();
  Serial.println("Smart Lamp siap. Lampu awal: OFF");
}

void loop() {
  int nilai = analogRead(PIN_SUARA);
  bool atas = (nilai >= AMBANG);
  if (atas && !atasAmbangSebelumnya && millis() - waktuTepukTerakhir > JEDA) {
    waktuTepukTerakhir = millis();
    jumlahTepuk++;
    setLampu(!lampu, "TEPUKAN");
  }
  atasAmbangSebelumnya = atas;

  bool tombol = digitalRead(PIN_TOMBOL);
  if (tombol == LOW && tombolSebelumnya == HIGH && millis() - waktuTombolTerakhir > 200) {
    waktuTombolTerakhir = millis();
    setLampu(!lampu, "TOMBOL");
  }
  tombolSebelumnya = tombol;

  delay(10);
}