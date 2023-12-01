#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "WatchDog.h"
#include <avr/wdt.h>

#define RESET asm ("jmp (0x0000)")
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte tag1[] = {0x04, 0x24, 0xA5, 0x20, 0xB7, 0x2A, 0x81};
byte tag2[] = {0x04, 0x26, 0xA5, 0x20, 0xB7, 0x2A, 0x81};


bool tag1Present = false;
bool tag2Present = false;


bool tag1read = false;
bool tag2read = false;


bool flag1 = 0;
bool flag2 = 0;

char ler;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Aguardando...");
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);
  pinMode(8, OUTPUT);
}

void loop() {
  digitalWrite(8, HIGH);
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    byte* tagId = mfrc522.uid.uidByte;

    // Verifica se a tag lida é a Tag 1
    if (compareTagId(tagId, tag1)) {
      tag1Present = true;
      tag1read = true;
    }

    // Verifica se a tag lida é a Tag 2
    if (compareTagId(tagId, tag2)) {
      tag2Present = true;
      tag2read = true;
    }

    lcd.clear();
    if (tag1Present && flag1 == 0) {
      flag1 = 1;
      lcd.print("Equipamento                             aprovado");
      delay(1000);
      lcd.clear();
      ler = 'A';

    } else if (tag2Present && flag2 == 0) {
      flag2 = 1;
      lcd.print("Equipamento                             aprovado");
      delay(1000);
      lcd.clear();
      ler = 'B';

    } else {
      lcd.print("Equipamento                             negado");
      delay(1000);
      lcd.clear();
      ler = 'N';
    }

    if (tag1Present && tag2Present) {
      lcd.clear();
      lcd.print("Entrada aprovada");
      delay(2000);
      digitalWrite(8, LOW);
      delay(2000);
      wdt_enable(WDTO_1S);
      wdt_reset();
    }

    if (!tag1Present) {
      ler = 'a';
    }
    if (!tag2Present) {
      ler = 'b';
    }

    delay(1000);
    
  }
}

bool compareTagId(byte* tagId1, byte* tagId2) {
  for (int i = 0; i < 4; i++) {
    if (tagId1[i] != tagId2[i]) {
      return false;
    }
  }
  return true;
}
