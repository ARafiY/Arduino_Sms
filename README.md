# Arduino SMS Bildirim Sistemi

Bu proje, bir Arduino ve Ethernet kalkanı kullanarak ortam sıcaklığını ve elektrik durumunu izleyen, belirlenen koşullar oluştuğunda bir web sunucusu üzerinden SMS bildirimi gönderen bir sistemdir.

## 📌 Özellikler

*   **Sıcaklık İzleme:** DHT22 sensörü ile sıcaklık ölçümü. Sıcaklık 30°C'yi aştığında uyarı gönderir.
*   **Elektrik Kesintisi Tespiti:** Bir LDR sensörü (veya benzeri bir devre) ile elektrik gidip geldiğinde bildirim gönderir.
*   **Tekrarlı Bildirim Engelleme:** Aynı durum (örneğin elektrik kesintisi) devam ettiğinde, bildirimlerin 5 dakikada bir tekrarlanmasını sağlar.
*   **Esnek Ağ Bağlantısı:** Öncelikle DHCP ile IP almayı dener, başarısız olursa statik IP yapılandırmasına geçer.
*   **HTTP Tabanlı Bildirim:** Belirlenen bir sunucuya HTTP GET isteği göndererek SMS tetiklemesi yapar.

## 🛠️ Gereken Donanımlar

*   Arduino Uno (veya benzeri bir kart)
*   Arduino Ethernet Shield (W5100 veya benzeri)
*   DHT22 Sıcaklık ve Nem Sensörü
*   LDR (Işık Bağımlı Direnç) ve gerekli devre elemanları (elektrik kesintisi algılama için)

## 🔌 Bağlantı Şeması

*   **Ethernet Shield:** Arduino üzerine takılır. (SPI pinlerini kullanır: 10, 11, 12, 13)
*   **DHT22 Sensör:** Veri pini **Dijital 6** numaralı pine bağlanır.
*   **LDR Devresi:** Çıkış pini **Dijital 4** numaralı pine bağlanır. (Elektrik varlığına göre dijital sinyal üretecek şekilde tasarlanmalıdır)

## 💻 Yazılım ve Kütüphaneler

Kodu Arduino IDE ile derleyip kartınıza yükleyebilirsiniz. Gerekli kütüphaneler:

*   `SPI.h` (Ethernet için, genelde IDE ile gelir)
*   `Ethernet.h` (Ethernet için, genelde IDE ile gelir)
*   `DHT.h` (DHT sensör kütüphanesi - **Adafruit'in kütüphanesini yüklemeniz gerekebilir**)
*   `Wire.h` (I2C için, genelde IDE ile gelir)

Kodu yüklemeden önce aşağıdaki yapılandırmaları **kendi ağ ve sunucu bilgilerinize göre** düzenlemelisiniz.

### 🔧 Yapılandırma

Kodun başında bulunan aşağıdaki değişkenleri kendi ortamınıza uygun şekilde değiştirin:

1.  **Sunucu Adresi:** Bildirimlerin gönderileceği sunucunun IP'si veya domain adı.
    ```c++
    char server[] = "192.168.1.100"; // Örnek: kendi sunucu IP'nizi yazın
    ```
2.  **HTTP Host Başlığı:** İstek gönderdiğiniz sunucunun adresi.
    ```c++
    client.println("Host: 192.168.1.100"); // Yukarıdaki server ile aynı olmalı
    ```
3.  **Statik IP Ayarları (DHCP başarısız olursa kullanılır):** Kendi ağınıza uygun IP, DNS, Gateway ve Subnet mask'ı girin.
    ```c++
    IPAddress ip(192, 168, 1, 177);
    IPAddress myDns(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    ```
4.  **İstek Yolu (PATH):** Sunucudaki hedef betiğin yolunu düzenleyin.
    ```c++
    // Örnek: "/smsGonder?mesaj="
    client.println("GET /PATH" + smsVeri + " HTTP/1.1");
    ```

## ⚙️ Nasıl Çalışır?

1.  **Kurulum (Setup):** Kart açıldığında Ethernet bağlantısı kurulur. Başarılı olursa bir "Sistem Açıldı" bildirimi gönderilir ve kart üzerindeki 13 numaralı LED yanar.
2.  **Döngü (Loop):** Sürekli olarak sıcaklık ve elektrik durumu kontrol edilir.
3.  **Elektrik Kontrolü:** Dijital 4 pinindeki sinyal değiştiğinde (0'dan 1'e veya 1'den 0'a) ilgili bildirim gönderilir. Kesinti devam ederse 5 dakikada bir tekrar bildirim gönderilir.
4.  **Sıcaklık Kontrolü:** Sıcaklık 30°C'yi geçtiğinde bildirim gönderilir. Sıcaklık 28.9°C - 29.5°C aralığına düşene kadar beklenir ve düşerse durum sıfırlanır. Sıcaklık 30°C üzerinde kalmaya devam ederse 5 dakikada bir tekrar bildirim gönderilir.
5.  **Bildirim (smsRequest):** Bir olay gerçekleştiğinde, bu fonksiyon sunucuya bir HTTP GET isteği yapar. İstek içinde olayı tanımlayan bir metin (örn: `1+Elektrik+Gitti`) gönderilir. Sunucunun bu isteği alıp bir SMS gönderecek şekilde yapılandırıldığı varsayılır.

## 🚨 Önemli Uyarılar

*   **LDR ile Elektrik Algılama:** LDR doğrudan dijital pine bağlanamaz. Elektrik varlığını güvenli ve doğru şekilde algılayabilmek için bir **transistörlü anahtar, optokuplör veya röle** gibi bir ara devre kullanmanız şarttır. Aksi takdirde Arduino'nuz zarar görebilir.
*   **URL Kodlaması:** Kodda gönderilen `smsVeri` içindeki boşluklar `+` işaretiyle değiştirilmiştir ancak bu tam bir URL kodlaması (escaping) değildir. Sunucu tarafında sorun yaşamamak için gönderilen metnin içeriğine dikkat edin veya sunucuda gerekli düzenlemeleri yapın.
*   **DHT Okuma Hataları:** Kod, sensörün okunamaması durumunda `isnan()` kontrolü yaparak hatalı bildirimleri engeller.

## 📜 Lisans

Bu proje [MIT Lisansı](LICENSE) altında lisanslanmıştır.
