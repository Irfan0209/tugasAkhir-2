#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
WiFiClient client;
#include <WiFiManager.h>
WiFiManager wm; // global wm instance

#include "set.h"
#define indikator D4
//const char* ssid = SSID_WIFI;         
//const char* password = PASSWORD_WIFI;  

String addres = ADDRES_SERVER;

String label = "";
String panjang = "";
String lebar = "";
String tinggi = "";
String berat = "";

bool connectt=false;
#define BOARD_LED_BRIGHTNESS 255  // Kecerahan maksimum 244 dari 255
#define DIMM(x) ((uint32_t)(x) * (BOARD_LED_BRIGHTNESS) / 255)
uint8_t m_Counter = 0;   // Penghitung 8-bit untuk efek breathe

#include <EEPROM.h>

#define EEPROM_SIZE 96          // Pastikan cukup untuk menyimpan IP, GW, Subnet

// Custom parameter input untuk portal konfigurasi
WiFiManagerParameter custom_ip("ip", "Static IP", "", 16);

IPAddress staticIP;

// Fungsi untuk menyimpan IP ke EEPROM
void saveIPtoEEPROM(IPAddress ip) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, ip[i]);
  }
  EEPROM.commit();
  EEPROM.end();
//  Serial.println("IP address saved to EEPROM.");
}

// Fungsi untuk membaca IP dari EEPROM
bool readIPfromEEPROM(IPAddress &ip) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 4; i++) {
    ip[i] = EEPROM.read(i);
  }
  EEPROM.end();

  // Validasi sederhana: IP tidak boleh 0.0.0.0
  if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
    return false;
  }
  return true;
}

void configModeCallback(WiFiManager *myWiFiManager) {
//  /Serial.println("Portal konfigurasi terbuka!");
  digitalWrite(indikator, LOW); // Nyalakan LED
}

void setup() {
  Serial.begin(115200);
  pinMode(indikator, OUTPUT);
  delay(1000);

  IPAddress ip;
  if (readIPfromEEPROM(ip)) {
//    Serial.println("Static IP loaded from EEPROM:");
//    Serial.print("IP: "); Serial.println(ip);
    addres = ip.toString();
//    Serial.print("addres: "); Serial.println(addres);
  }

//  WiFiManager wm;
  // Callback saat portal terbuka
  wm.setAPCallback(configModeCallback);
  
  // Tambah parameter input ke portal
  wm.addParameter(&custom_ip);

  // Coba connect, jika gagal, buka portal
  if (!wm.autoConnect("AP_timbangan")) {
//    Serial.println("Failed to connect.");
    delay(2000);
    ESP.restart();
  }
  connectt=1;
  // Setelah connect, cek apakah ada input baru
  String ipStr = custom_ip.getValue();
  if (ipStr.length() > 0) {
    IPAddress newIP;
    newIP.fromString(ipStr);
    saveIPtoEEPROM(newIP);
//    Serial.println("New static IP saved. Restarting...");
    delay(2000);
    ESP.restart(); // Restart untuk menggunakan IP statis baru
  }


}


void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("label=")) {
      // Parse data satu per satu
      label = ambilData(input, "label=");
      panjang = ambilData(input, "panjang=");
      lebar   = ambilData(input, "lebar=");
      tinggi  = ambilData(input, "tinggi=");
      berat   = ambilData(input, "berat=");
      
//      Serial.println("Data berhasil disimpan sementara.");
//      Serial.println("Ketik 'kirim' untuk mengirim data.");
    } 
    else if (input.equalsIgnoreCase("kirim")) {
      if (label != "" && panjang != "" && lebar != "" && tinggi != "" && berat != "") {
        kirimData();
      } else {
        //Serial.println("Data belum lengkap! Isi data dulu.");
      }
    } 
    else if(input.equalsIgnoreCase("wifi")) {
      
    }
    else {
      //Serial.println("Format tidak dikenali. Ketik 'kirim' atau masukkan data.");
    }
    //Serial.println("adrres:" + addres);
  }
  showLed(connectt);
}

String ambilData(String input, String key) {
  int start = input.indexOf(key);
  if (start == -1) return "";
  start += key.length();
  int end = input.indexOf(' ', start);
  if (end == -1) end = input.length();
  return input.substring(start, end);
}

void kirimData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String link = "http://"+ addres + "/php_XAMPP/post.php";
    http.begin(client,link);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postData = "label=" + label + "&panjang=" + panjang + "&lebar=" + lebar + "&tinggi=" + tinggi + "&berat=" + berat;
    //Serial.println(postData);
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Data terkirim! Respon server:");
      Serial.println(response);
    } else {
      Serial.print("Error mengirim data. Kode: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    
    // Reset data
    label = panjang = lebar = tinggi = berat = "";
  } else {
    //Serial.println("WiFi tidak terhubung.");
  }
}

void showLed(bool state){
  if(state){
    unsigned long currentMillis = millis();
    static unsigned long previousMillis;
    static unsigned long interval=2000;
    // Mengatur LED dengan waveLED jika interval waktu telah tercapai
    if (currentMillis - previousMillis >= interval / 256) {  // Menggunakan interval/256 sesuai logika waveLED
      previousMillis = currentMillis;  // Perbarui waktu sebelumnya
      interval = waveLED(0, interval);  // Panggil fungsi waveLED dan dapatkan interval berikutnya
      //Serial.println("interval:"+String(interval));
    }
  }
  else{
    setLED(255);
  }
}
uint32_t waveLED(uint32_t, unsigned breathePeriod) {
    uint32_t brightness = (m_Counter < 128) ? m_Counter : 255 - m_Counter;

    setLED(DIMM(brightness * 2));  // Mengatur LED dengan kecerahan yang dihitung

    // Menggulung nilai m_Counter antara 0 hingga 255
    m_Counter = (m_Counter + 1) % 256;
    
    // Mengembalikan nilai interval (delay) untuk satu iterasi
    return breathePeriod;
}

void setLED(uint8_t brightness) {
  analogWrite(indikator, brightness);
}
