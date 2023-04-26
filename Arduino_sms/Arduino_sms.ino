#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <Wire.h>

//MAC Adresininizi Bilmiyorsanız Değiştirmeyiniz.
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Eğer DHCP Kullanılarak IP Adresi Alınamassa Alınacak IP Adresi
IPAddress ip(192, 1, 1, 1);
IPAddress myDns(192, 1, 1, 1);
IPAddress gateway(192, 1, 1, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetClient client;
DHT dht(6, DHT22);
char server[] = " ";  // Bağlanılacak Sitenin URL si
int LDRSensor = 4;
int value = 0;
unsigned long smsEskiZaman = 0, voltEskiZaman = 0, volt2EskiZaman = 0;
unsigned long smsYeniZaman, voltYeniZaman, volt2YeniZaman;
String smsVeri;
int smsSayi = 0, smsDurum = 0, voltageSayi = 0, voltDurum = 0;


void setup() {
  pinMode(LDRSensor, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Ethernet.init(10);
  Serial.begin(9600);
  dht.begin();

  // Ethernet Bağlantısı Açılıyor
  Serial.println("DHCP Kullanılarak IP Adresi Alınmaya Çalışıyor.");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP Kullanılarak IP Adresi Alınamadı.");
    // Ethernet Shield Taranıyor
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet Shield bulunamadı. Lütfen Doğru Takdığınızdan Emin Olunuz. :(");
      //while (true) {
      //delay(1);
      //}
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet Kablosu Bağlanılmadı.");
    }

    Ethernet.begin(mac, ip, myDns, gateway, subnet);
    Serial.print("Statik IP Adresiniz: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("DHCP Olarak Dağıtılan IP Adresiniz: ");
    Serial.println(Ethernet.localIP());
  }

  client.stop();
  // eğer bağlantı başarılı ise
  if (client.connect(server, 80)) {
    Serial.println("Bağlanılıyor...");
    // HTTP GET Request Gönderiliyor..
    smsVeri = "Sistem+Açıldı.";
    client.println("GET /PATH" + smsVeri + " HTTP/1.1");
    client.println("Host: "); //SİTE URL
    client.println();
    Serial.println("Sunucuya Bağlanıldı. İnternet Açık.");
    digitalWrite(13, HIGH);

  } else {
    // Eğer bağlantı başarısız ise
    Serial.println("Sunucuya Bağlanılmadı. İnternet Kapalı");
    digitalWrite(13, LOW);
    client.stop();
  }
}

void loop() {

  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  float sicaklik = dht.readTemperature(); //Sıcaklığı Ölçüyor

  int voltageDurum = digitalRead(LDRSensor);
  // Serial.println(voltageDurum);
  /////////////////////////////////Elektrik/////////////////////////////
  if (voltageDurum == 0) {
    if (voltDurum == 0) {
      smsVeri = String(voltageSayi = voltageSayi + 1) + "+Elektrik+Gitti";
      Serial.println("Elektrik gitti");
      smsRequest(smsVeri);
      voltDurum = 1;
    }
  }
  if (voltageDurum == 0 && voltDurum == 1) {
    voltYeniZaman = millis() / 1000;
    if (voltYeniZaman - voltEskiZaman >= 300) {
      smsVeri = String(voltageSayi = voltageSayi + 1) + "+Elektrik+Gitti";
      Serial.println("Elektrik gitti");
      smsRequest(smsVeri);
      voltEskiZaman = voltYeniZaman;
    }
  }

  else if (voltageDurum == 1 && voltDurum == 1)
  {
    smsVeri = String(voltageSayi = voltageSayi + 1) + "+Elektrik+Geldi";
    Serial.println("Elektrik geldi");
    smsRequest(smsVeri);
    voltDurum = 0;
    voltageSayi = 0;
    volt2EskiZaman = volt2YeniZaman;
  }



  /////////////////////////////////Sıcaklık////////////////////////////
  if (sicaklik >= 30.00) {
    if (smsDurum == 0) {
      float mSicaklik = 30.00;
      smsVeri = String(smsSayi = smsSayi + 1) + "+Sicaklik+30+Dereceyi+Aşti";
      Serial.println(sicaklik);
      smsRequest(smsVeri);
      smsDurum = 1;
    }
  }
  if (sicaklik >= 30.00 && smsDurum == 1) {
    smsYeniZaman = millis() / 1000;
    if (smsYeniZaman - smsEskiZaman >= 300) {
      smsVeri = String(smsSayi = smsSayi + 1) + "+Sicaklik+30+Dereceyi+Aşti";
      Serial.println(sicaklik);
      smsRequest(smsVeri);
      smsEskiZaman = smsYeniZaman;
    }
  }

  else if (sicaklik > 28.90 && sicaklik <= 29.50)
  {
    smsDurum = 0;
    smsSayi = 0;
  }
}
/////////////////////////////////smsRequest////////////////////////////
void smsRequest(String smsVeri) {
  // eğer bağlantı başarılı ise
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("Bağlanılıyor...");
    // HTTP GET Request Gönderiliyor..

    client.println("GET /PATH" + smsVeri + " HTTP/1.1");
    client.println("Host: "); //Site URL
    client.println("Connection: keep-alive");
    client.println();
  } else {
    // Eğer bağlantı başarısız ise
    Serial.println("Bağlantı Başarısız.");
  }
}
