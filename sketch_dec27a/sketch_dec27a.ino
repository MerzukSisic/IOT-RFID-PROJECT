#include <SPI.h>
#include <MFRC522.h>
#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


// Beleške i trajanja (note i tempo melodije)
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5, // "Jin-gle bells"
  NOTE_E5, NOTE_E5, NOTE_E5, // "Jin-gle bells"
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5, // "Jin-gle all the way"
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5 // "Oh what fun it is to ride"
};

int noteDurations[] = {
  8, 8, 4,   // "Jin-gle bells"
  8, 8, 4,   // "Jin-gle bells"
  8, 8, 8, 8, 4, // "Jin-gle all the way"
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 // "Oh what fun it is to ride"
};

int melodyBass[] = {
  NOTE_C4, NOTE_E4, NOTE_A3, NOTE_G3, // Prvi deo melodije
  NOTE_F3, NOTE_G3, NOTE_E3, NOTE_C3  // Drugi deo melodije
};

int noteDurationsBass[] = {
  4, 8, 4, 4,  // Trajanje nota (četvrtinke i osmine)
  4, 8, 4, 4   // Trajanje za drugi deo
};

// Wi-Fi konfiguracija
const char* ssid = "UNMO (Free Internet)";
const char* password = "";

// Firebase konfiguracija
#define FIREBASE_HOST "iot-project-f5585-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "dfRsaOiKvbI8NpLpw2wdPC2283Yfmg0ubPkagNCu"

FirebaseData firebaseData;
FirebaseConfig config;

// RFID konfiguracija
#define RST_PIN D1
#define SS_PIN D2
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Buzzer pin
#define BUZZER_PIN D8
#define LED1_PIN D3 // GPIO0
#define LED2_PIN D4 // GPIO0
#define LED3_PIN D0 // GPIO0

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin(ssid, password);
  Serial.print("Povezivanje na Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi povezan!");

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_SECRET;
  Firebase.begin(&config, NULL);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);


  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Isključi buzzer
  digitalWrite(LED1_PIN, LOW);    // Isključi LED
  digitalWrite(LED2_PIN, LOW);    // Isključi LED
  digitalWrite(LED3_PIN, HIGH);    // Isključi LED

}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String cardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardID += String(mfrc522.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();
  Serial.println("Skenirana kartica: " + cardID);

  // Pošalji ID kartice na Firebase
  if (Firebase.RTDB.setString(&firebaseData, "/currentCard", cardID)) {
    Serial.println("Kartica uspešno poslata na Firebase: " + cardID);
  } else {
    Serial.println("Greška pri slanju na Firebase: " + firebaseData.errorReason());
    signalInvalidCard(); // Signaliziraj nevalidnu karticu
    return;
  }

  // Proveri validnost kartice u Firebase bazi
  String path = "/employees/" + cardID;
  if (Firebase.RTDB.getJSON(&firebaseData, path)) {
    if (firebaseData.dataType() == "null") {
      Serial.println("Kartica nije validna!");
      signalInvalidCard(); // Signaliziraj nevalidnu karticu
    } else {
      Serial.println("Kartica validna!");
      signalValidCard(); // Signaliziraj validnu karticu
    }
  }else{
      Serial.println("Greška pri čitanju iz Firebase-a: " + firebaseData.errorReason());
    signalInvalidCard(); // Signaliziraj nevalidnu karticu
  }

  mfrc522.PICC_HaltA();       // Zaustavi komunikaciju sa trenutnom karticom
  mfrc522.PCD_StopCrypto1();  // Prekini enkripciju za čitač
  delay(1000);                // Sačekaj 1 sekundu pre sledeće kartice

}

void signalValidCard() 
{
        digitalWrite(LED1_PIN, HIGH);    // Isključi LED
      digitalWrite(LED2_PIN, LOW);    // Isključi LED
      digitalWrite(LED3_PIN, LOW);    // Isključi LED  t
  for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
        int noteDuration = 1000 / noteDurations[i]; // Izračunaj trajanje note
        tone(BUZZER_PIN, melody[i], noteDuration);
        delay(noteDuration * 1.3); // Pauza između nota
        noTone(BUZZER_PIN); // Isključi ton
      }  

      delay(3000);
      digitalWrite(LED1_PIN, LOW);    // Isključi LED
      digitalWrite(LED2_PIN, LOW);    // Isključi LED
      digitalWrite(LED3_PIN, HIGH);    
}

void signalInvalidCard() {
  digitalWrite(LED1_PIN, LOW);    // Isključi LED
  digitalWrite(LED2_PIN, HIGH);    // Isključi LED
  digitalWrite(LED3_PIN, LOW);    // Isključi LED
    for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
    int noteDuration = 1000 / noteDurationsBass[i]; // Izračunaj trajanje note
    tone(BUZZER_PIN, melodyBass[i], noteDuration);
    delay(noteDuration * 1.3); // Pauza između nota
    noTone(BUZZER_PIN); // Isključi ton
  }

  digitalWrite(LED1_PIN, LOW);    // Isključi LED
  digitalWrite(LED2_PIN, LOW);    // Isključi LED
  digitalWrite(LED3_PIN, HIGH);
}




