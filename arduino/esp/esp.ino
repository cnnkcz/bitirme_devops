#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define MQTT_MAX_PACKET_SIZE 512
// ================== AYARLAR (AYNEN KORUNDU) ==================

const char* ssid = "cnn";             // WiFi adın
const char* password = "alibeykoy99"; // WiFi şifren

// Bilgisayarının IP adresi
const char* mqtt_server = "89.167.19.129"; 

// Access Token
const char* token = "39SYKAtdFAwNJ5JmIi9r"; 

// ===========================================================

// Sensör Pinleri
#define DHTPIN 4       // DHT Data pini
#define DHTTYPE DHT22  // DHT22 (Mavi/Beyaz hassas olan)
#define LDRPIN 36      // LDR Işık sensörü pini

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
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

  // 2. ThingsBoard Sunucu Ayarı
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
  
  // --- IŞIK MANTIĞI GÜNCELLENDİ ---
  int hamIsik = analogRead(LDRPIN); // Sensörden gelen ham veri (Örn: 500 veya 3000)
  int isik = 0;

  // Eşik Değeri: 2000 
  // Flaş tutunca değer düşüyorsa (Örn: 500 oluyorsa), 2000'den küçüktür -> Işık VAR (1)
  if (hamIsik < 2000) {
    isik = 1; // Işık Açık
  } else {
    isik = 0; // Işık Kapalı
  }

  // Okuma hatası kontrolü
  if (isnan(nem) || isnan(sicaklik)) {
    Serial.println("Sensor okunamadi! Kablolari kontrol et.");
  }

  // --- Veri Paketi (JSON) ---
  String veriPaketi = "{";
  veriPaketi += "\"sicaklik\":"; veriPaketi += sicaklik; veriPaketi += ",";
  veriPaketi += "\"nem\":";      veriPaketi += nem;      veriPaketi += ",";
  veriPaketi += "\"isik\":";     veriPaketi += isik;     // Artık sadece 0 veya 1 gidiyor
  veriPaketi += "}";

  Serial.print("Ham Isik: "); 
  Serial.print(hamIsik); // Seri portta gerçek değeri görmen için bıraktım
  Serial.print(" -> Gonderilen: ");
  Serial.println(veriPaketi);

  // --- Gönder ---
  client.publish("v1/devices/me/telemetry", veriPaketi.c_str());

  delay(2000); // 2 saniyede bir veri atar
}