# Dokumentasi Opsi A — Arduino IDE untuk ESP32 IoT

## 1. Tujuan

Dokumentasi ini dipakai untuk membuat, build, upload, debugging, dan deploy proyek:

**Monitoring Kualitas Udara Berbasis MQ-135 dan GP2Y1010AU0F dengan Exhaust Fan Otomatis**

Target perangkat:

- ESP32 DevKit
- Sensor MQ-135
- Sensor GP2Y1010AU0F
- LCD I2C
- Buzzer
- Relay/MOSFET exhaust fan
- ThingSpeak

---

## 2. Instalasi Software

### 2.1 Install Arduino IDE

Download dan install:

```text
https://www.arduino.cc/en/software
```

Gunakan Arduino IDE 2.x.

### 2.2 Tambahkan ESP32 Board Manager

Buka:

```text
Arduino IDE > Settings / Preferences
```

Tambahkan URL berikut pada **Additional Boards Manager URLs**:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Lalu buka:

```text
Tools > Board > Boards Manager
```

Cari dan install:

```text
esp32 by Espressif Systems
```

### 2.3 Pilih Board ESP32

Pilih:

```text
Tools > Board > esp32 > ESP32 Dev Module
```

Setting awal:

```text
Upload Speed: 115200 atau 921600
CPU Frequency: 240MHz
Flash Frequency: 80MHz
Flash Mode: QIO
Partition Scheme: Default
Core Debug Level: None
Port: pilih port ESP32
```

### 2.4 Install Library

Buka:

```text
Sketch > Include Library > Manage Libraries
```

Install:

```text
LiquidCrystal_I2C
```

Library bawaan ESP32 yang dipakai:

```text
WiFi
HTTPClient
Wire
```

Opsional:

```text
ArduinoJson
MQUnifiedsensor
```

Untuk awal, pembacaan MQ-135 dan GP2Y1010AU0F bisa manual via ADC.

---

## 3. Driver USB ESP32

Cek chip USB ESP32:

| Chip | Driver |
|---|---|
| CP2102 | Silicon Labs CP210x |
| CH340/CH341 | WCH CH34x |
| FTDI | FTDI VCP |

Cek port di macOS:

```bash
ls /dev/cu.*
```

Contoh port:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.wchusbserialxxxx
/dev/cu.usbserial-xxxx
```

Jika port tidak muncul:

- Ganti kabel USB data.
- Install driver sesuai chip.
- Cabut-pasang ESP32.
- Restart Arduino IDE.

---

## 4. Struktur Sketch Arduino IDE

Rekomendasi folder:

```text
Sugeng-IOT-Arduino/
├── Sugeng-IOT-Arduino.ino
├── config.h
└── README.md
```

Isi `config.h` lokal:

```cpp
#define WIFI_SSID "NAMA_WIFI"
#define WIFI_PASSWORD "PASSWORD_WIFI"
#define THINGSPEAK_API_KEY "WRITE_API_KEY"
```

Jangan upload `config.h` ke repository publik.

---

## 5. Rekomendasi Pin ESP32

| Perangkat | Pin ESP32 |
|---|---:|
| MQ-135 AO | GPIO34 |
| GP2Y1010 Vo | GPIO35 |
| GP2Y1010 LED | GPIO25 |
| LCD SDA | GPIO21 |
| LCD SCL | GPIO22 |
| Buzzer | GPIO26 |
| Relay/MOSFET Fan | GPIO27 |

Catatan:

- GPIO34 dan GPIO35 hanya input, cocok untuk sensor analog.
- Hindari ADC2 saat WiFi aktif.
- Pastikan output analog sensor tidak lebih dari 3.3V.

### 5.1 Wiring Aktual yang Sedang Dipakai

Berdasarkan perakitan saat ini, wiring yang sudah terpasang adalah:

```text
Warna kabel utama
- GND  -> merah
- 3V3  -> hitam
- D21  -> kuning
- D22  -> putih
- D15  -> oren
- D26  -> biru (buzzer)

MQ-135
- VCC -> 3V3
- GND -> GND
- DO  -> GPIO15
- AO  -> belum dipakai

LCD I2C
- VCC -> 3V3
- GND -> GND
- SDA -> GPIO21
- SCL -> GPIO22

Buzzer
- + -> GPIO26
- - -> GND
```

Catatan penggunaan saat ini:

- Wiring MQ-135 di `DO -> GPIO15` dipakai untuk test digital awal.
- LCD I2C dipasang mengikuti jalur `SDA -> GPIO21` dan `SCL -> GPIO22`.
- Buzzer dicatat memakai jalur `GPIO26`.
- Warna kabel yang sedang dipakai: `GND merah`, `3V3 hitam`, `D21 kuning`, `D22 putih`, `D15 oren`, dan `D26 biru`.
- Jika LCD tidak tampil stabil pada `3V3`, pengujian berikutnya bisa mencoba `VCC -> VIN/5V` sesuai kompatibilitas modul LCD.
- Sensor debu GP2Y1010 dan fan belum dicatat sebagai terpasang aktif pada setup saat ini.

---

## 6. Template Alur Program

Urutan program:

1. `Serial.begin(115200)`
2. `Wire.begin(21, 22)`
3. Inisialisasi LCD.
4. Set pin buzzer, fan, LED dust.
5. Koneksi WiFi.
6. Baca MQ-135.
7. Baca GP2Y1010AU0F.
8. Hitung voltage.
9. Tentukan status udara.
10. Kontrol buzzer dan fan.
11. Tampilkan ke LCD.
12. Kirim ke ThingSpeak.
13. Delay minimal 15 detik.

---

## 7. Build / Verify

Klik:

```text
Sketch > Verify/Compile
```

Atau tombol centang.

Jika gagal compile:

- Pastikan board `ESP32 Dev Module`.
- Pastikan library `LiquidCrystal_I2C` sudah terinstall.
- Pastikan semua kurung dan titik koma benar.
- Pastikan `config.h` ada jika dipakai.
- Cek pesan error di panel bawah Arduino IDE.

---

## 8. Upload ke ESP32

Langkah:

1. Sambungkan ESP32 via USB.
2. Pilih board `ESP32 Dev Module`.
3. Pilih port ESP32.
4. Klik tombol upload.
5. Jika stuck di `Connecting...`:
   - Tekan dan tahan tombol `BOOT`.
   - Saat upload mulai, lepas tombol `BOOT`.

Jika upload gagal:

```text
A fatal error occurred: Failed to connect to ESP32
```

Solusi:

- Tekan tombol BOOT saat upload.
- Turunkan upload speed ke 115200.
- Ganti kabel USB.
- Pastikan port benar.
- Tutup Serial Monitor sebelum upload.
- Cek driver USB.

---

## 9. Serial Monitor

Buka:

```text
Tools > Serial Monitor
```

Set baud:

```text
115200
```

Output minimal:

```text
WiFi connecting...
WiFi connected
IP: 192.168.x.x
MQ135 ADC: 1420
MQ135 Voltage: 1.14
Dust ADC: 850
Dust Voltage: 0.68
Dust Density: 0.12
Status: SEDANG
Fan: OFF
Buzzer: OFF
ThingSpeak response: 200
Free heap: xxxxx
```

---

## 10. Debugging Hardware

### MQ-135

Jika nilai selalu 0:

- Cek VCC dan GND.
- Cek pin AO ke GPIO34.
- Cek sensor sudah panas.
- Cek output dengan multimeter.

Jika nilai selalu maksimum:

- Output sensor mungkin 5V.
- Pakai voltage divider.
- Pastikan pin ADC benar.

### GP2Y1010AU0F

Cek:

- LED control ke GPIO25.
- Vo ke GPIO35.
- Ground harus sama dengan ESP32.
- Resistor dan kapasitor sesuai datasheet/modul.
- Timing LED pulse benar.

### LCD I2C

Jika LCD kosong:

- Cek alamat I2C `0x27` atau `0x3F`.
- Cek SDA GPIO21.
- Cek SCL GPIO22.
- Putar trimpot kontras.
- Jalankan I2C scanner.

### Buzzer

Jika tidak bunyi:

- Cek polaritas.
- Cek pin GPIO26.
- Gunakan transistor jika buzzer butuh arus besar.

### Fan / Relay / MOSFET

Jika fan tidak aktif:

- Cek logic relay aktif HIGH/LOW.
- Cek supply fan terpisah.
- Jangan sambungkan fan langsung ke ESP32.
- Untuk fan DC, satukan ground power fan dengan ground ESP32.

---

## 11. Debugging WiFi

Checklist:

- WiFi harus 2.4GHz.
- SSID dan password benar.
- Sinyal cukup kuat.
- Jangan pakai captive portal.
- Print `WiFi.status()` jika gagal.

Contoh status penting:

```text
WL_CONNECTED = sukses
WL_NO_SSID_AVAIL = SSID tidak ditemukan
WL_CONNECT_FAILED = password/sinyal bermasalah
```

---

## 12. Debugging ThingSpeak

ThingSpeak gratis minimal interval update sekitar 15 detik.

Test dari komputer:

```bash
curl "https://api.thingspeak.com/update?api_key=WRITE_API_KEY&field1=123"
```

Response:

| Response | Arti |
|---|---|
| Angka entry ID | sukses |
| 0 | gagal / interval terlalu cepat / API key salah |
| 400 | request salah |
| 401/403 | API key tidak valid |
| -1 dari ESP32 | koneksi gagal |

Field yang disarankan:

```text
field1 = MQ135 ADC
field2 = MQ135 Voltage
field3 = Dust ADC
field4 = Dust Voltage / Density
field5 = Status numeric
field6 = Fan status
```

---

## 13. Kalibrasi Awal

### MQ-135

1. Panaskan sensor 5–10 menit untuk test awal.
2. Catat ADC udara bersih.
3. Catat ADC asap ringan.
4. Catat ADC asap pekat.
5. Tentukan baseline dan threshold.

Rumus awal:

```text
threshold_mq135 = baseline_mq135 + 30% baseline_mq135
```

### GP2Y1010AU0F

1. Catat ADC udara bersih.
2. Catat ADC saat ada debu.
3. Hitung voltage.
4. Tentukan threshold.

Rumus awal:

```text
threshold_dust = baseline_dust + 30% baseline_dust
```

---

## 14. Deployment Prototype

Sebelum dipasang:

- Pastikan casing punya ventilasi.
- Sensor tidak tertutup.
- MQ-135 tidak terlalu dekat dengan fan.
- Kabel tidak mudah lepas.
- Fan memakai driver aman.
- Relay AC diberi box isolasi.
- LCD terbaca jelas.
- Buzzer tidak terlalu keras.
- ThingSpeak menerima data stabil.
- Sistem tetap jalan walau WiFi gagal.

---

## 15. Code Deploy Arduino IDE

Sketch siap deploy berada di:

```text
arduino-ide/sketches/SugengIOT/SugengIOT.ino
```

File konfigurasi lokal:

```text
arduino-ide/sketches/SugengIOT/config.h
```

Isi `config.h` sebelum upload:

```cpp
#pragma once

#define WIFI_SSID "NAMA_WIFI"
#define WIFI_PASSWORD "PASSWORD_WIFI"
#define THINGSPEAK_API_KEY "WRITE_API_KEY"
```

Fitur utama sketch:

- Membaca MQ-135 pada GPIO34.
- Membaca GP2Y1010AU0F pada GPIO35 dengan LED control GPIO25.
- Menghitung tegangan ADC MQ-135 dan sensor debu.
- Menghitung estimasi dust density sederhana.
- Menentukan status udara: `BAIK`, `SEDANG`, `BURUK`.
- Mengaktifkan fan dan buzzer otomatis saat status buruk.
- Menggunakan hysteresis agar fan tidak berkedip.
- Menampilkan MQ, dust, status, dan fan pada LCD I2C.
- Mengirim data ke ThingSpeak setiap 16 detik.
- Tetap berjalan lokal walaupun WiFi/ThingSpeak gagal.

Mapping ThingSpeak:

```text
field1 = MQ135 ADC
field2 = MQ135 Voltage
field3 = Dust ADC
field4 = Dust Density
field5 = Status numeric
field6 = Fan status
```

Status numeric:

```text
1 = BAIK
2 = SEDANG
3 = BURUK
```

Threshold awal pada sketch:

```cpp
#define MQ135_THRESHOLD_ON 1800
#define MQ135_THRESHOLD_OFF 1500
#define DUST_THRESHOLD_ON 0.60
#define DUST_THRESHOLD_OFF 0.45
```

Nilai tersebut wajib disesuaikan dari hasil kalibrasi alat:

```text
threshold_on  = baseline + 30% baseline
threshold_off = nilai lebih rendah dari threshold_on
```

Langkah deploy:

1. Buka Arduino IDE.
2. Buka folder sketch `arduino-ide/sketches/SugengIOT/`.
3. Edit `config.h`.
4. Pilih board `ESP32 Dev Module`.
5. Pilih port ESP32.
6. Klik `Verify`.
7. Klik `Upload`.
8. Buka Serial Monitor `115200`.
9. Pastikan LCD, ThingSpeak, fan, dan buzzer berjalan sesuai status udara.

## 16. Checklist Akhir Arduino IDE

- [ ] Arduino IDE terinstall.
- [ ] ESP32 board package terinstall.
- [ ] Driver USB terinstall.
- [ ] Board `ESP32 Dev Module` dipilih.
- [ ] Port ESP32 terbaca.
- [ ] Library LCD I2C terinstall.
- [ ] Sketch berhasil Verify.
- [ ] Upload berhasil.
- [ ] Serial Monitor 115200 aktif.
- [ ] Sensor MQ-135 terbaca.
- [ ] Sensor GP2Y1010AU0F terbaca.
- [ ] LCD menampilkan data.
- [ ] Buzzer aktif saat buruk.
- [ ] Fan aktif saat buruk.
- [ ] ThingSpeak menerima data.
- [ ] Prototype aman untuk deploy.
