#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define MQTT_MAX_PACKET_SIZE 512

// ================== YARIN Kİ TESLİM AYARLARI ==================
// Okulda telefonunun internetini (Hotspot) aç ve bu bilgileri ona göre güncelle!
const char* ssid = "cnn";             // Telefonunun internet adı
const char* password = "alibeykoy99"; // Telefonunun internet şifresi

// Hetzner Sunucu IP Adresin
const char* mqtt_server = "89.167.19.129"; 

// ThingsBoard Cihaz Token'ın (Değiştirmediysen aynen kalsın)
const char* token = "39SYKAtdFAwNJ5JmIi9r"; 

// ===========================================================

// Sensör Pin Tanımlamaları
#define DHTPIN 4       // DHT22 Data pini
#define DHTTYPE DHT22  // DHT22 Modeli
#define LDRPIN 36      // LDR Işık sensörü pini (Analog Pin)

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // ESP32'nin ADC hassasiyetini 12-bit (0-4095) olarak ayarlıyoruz
  analogReadResolution(12); 
  
  // 1. WiFi Bağlantısı
  Serial.print("WiFi'ye baglaniliyor: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Baglandi!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());

  // 2. ThingsBoard Sunucu Ayarı (Port 1883 MQTT standardıdır)
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("ThingsBoard'a baglaniliyor...");
    
    // Bağlanırken kullanıcı adı yerine TOKEN kullanıyoruz
    if (client.connect("ESP32_Sera_Cihazi", token, NULL)) {
      Serial.println("BAGLANDI!");
    } else {
      Serial.print("Hata Kodu: ");
      Serial.print(client.state());
      Serial.println(" (5 saniye sonra tekrar denenecek)");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // --- Sensörleri Oku ---
  float nem = dht.readHumidity();
  float sicaklik = dht.readTemperature();
  
  // --- IŞIK MANTIĞI (HAM VERİ VE YÜZDE HESABI) ---
  int hamIsik = analogRead(LDRPIN); 
  
  // map() fonksiyonu: 0-4095 arası ham değeri 0-100 arasına çeker.
  // Not: LDR'de ışık arttıkça değer düşer, bu yüzden (4095, 0) aralığını (0, 100) yaptık.
  int isikYuzde = map(hamIsik, 4095, 0, 0, 100); 

  // Değerin 0-100 dışına taşmaması için kontrol
  if(isikYuzde > 100) isikYuzde = 100;
  if(isikYuzde < 0) isikYuzde = 0;

  // Okuma hatası kontrolü (Sensör takılı değilse veya bozuksa)
  if (isnan(nem) || isnan(sicaklik)) {
    Serial.println("DHT Sensoru okunamadi! Kablolari kontrol et.");
  }

  // --- Veri Paketi (JSON Formatı) ---
  // Hocanın istediği gibi hem ham hem yüzde verisini gönderiyoruz
  String veriPaketi = "{";
  veriPaketi += "\"sicaklik\":";  veriPaketi += sicaklik;  veriPaketi += ",";
  veriPaketi += "\"nem\":";       veriPaketi += nem;       veriPaketi += ",";
  veriPaketi += "\"isik_ham\":";  veriPaketi += hamIsik;   veriPaketi += ",";
  veriPaketi += "\"isik_yuzde\":"; veriPaketi += isikYuzde;
  veriPaketi += "}";

  // Seri Port üzerinden takip (Hocaya buradan da gösterebilirsin)
  Serial.println("--- Yeni Veri Paketi ---");
  Serial.print("Sicaklik: "); Serial.print(sicaklik); Serial.println(" C");
  Serial.print("Nem: "); Serial.print(nem); Serial.println(" %");
  Serial.print("LDR Ham Değer: "); Serial.println(hamIsik);
  Serial.print("Işık Yüzdesi: %"); Serial.println(isikYuzde);
  Serial.println("------------------------");

  // --- Veriyi Gönder ---
  client.publish("v1/devices/me/telemetry", veriPaketi.c_str());

  // Hocanın verileri rahat takip etmesi için 5 saniyede bir gönderiyoruz
  delay(5000); 
}