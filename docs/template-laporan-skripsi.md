# Template Laporan Skripsi — Monitoring Kualitas Udara Berbasis ESP32

Judul:

```text
RANCANG BANGUN SISTEM MONITORING KUALITAS UDARA BERBASIS SENSOR MQ-135 DAN GP2Y1010AU0F DENGAN CONTROL EXHAUST FAN OTOMATIS
```

Dokumen ini difokuskan untuk penyusunan **BAB II ke bawah**.

---

# BAB II  
# TINJAUAN PUSTAKA

## 2.1 Penelitian Terdahulu

Bagian ini menjelaskan referensi penelitian yang berkaitan dengan monitoring kualitas udara, penggunaan sensor gas, sensor debu, mikrokontroler ESP32, IoT, dan kontrol exhaust fan otomatis.

Contoh tabel penelitian terdahulu dari hasil pencarian Google Scholar:

| No | Peneliti | Tahun | Judul | Metode/Komponen | Hasil | Perbedaan dengan Penelitian Ini |
|---:|---|---:|---|---|---|---|
| 1 | T. Sutikno, W. Arsadiando, dkk. | 2025 | Internet of Things-based Air Quality Monitoring system using Carbon Monoxide and Particulate Matter Parameters | IoT, monitoring kualitas udara, karbon monoksida, partikulat | Sistem dapat memantau kualitas udara berbasis parameter gas dan partikel | Penelitian ini memakai MQ-135, GP2Y1010AU0F, ThingSpeak, LCD, buzzer, dan exhaust fan otomatis |
| 2 | I. Humphrey, A. D. Adewoyin, dkk. | 2025 | Design and Deployment of a Low-Cost IoT-Based Air Quality Monitoring System Using ESP32, BME688, and MQ135 Sensors in Urban Lagos, Nigeria | ESP32, BME688, MQ135, IoT | Sistem monitoring kualitas udara biaya rendah berbasis ESP32 | Penelitian ini menambahkan sensor debu GP2Y1010AU0F dan kontrol exhaust fan otomatis |
| 3 | O. Alsamrai, M. D. Redel | 2025 | Real-time intelligent monitoring of outdoor air quality in an urban environment using IoT and machine learning algorithms | IoT, machine learning, monitoring kualitas udara real-time | Sistem mampu melakukan monitoring udara luar ruangan secara cerdas | Penelitian ini fokus pada prototype indoor berbasis ESP32 dan aktuator fan/buzzer |
| 4 | I. Ariska | 2025 | Monitoring Kualitas Udara Indoor Menggunakan Sensor Debu Optik GP2Y1010AU0F dan Sensor Gas MQ-135 Berbasis Internet of Things | GP2Y1010AU0F, MQ-135, IoT | Sistem memantau kualitas udara indoor berdasarkan gas dan debu | Penelitian ini menambahkan ThingSpeak, LCD, buzzer, dan kontrol exhaust fan otomatis |
| 5 | N. C. SM, S. Selvaganesan | 2025 | Air Quality Monitoring and Purifying System | Monitoring dan pemurnian kualitas udara | Sistem tidak hanya memantau, tetapi juga melakukan tindakan terhadap kualitas udara | Penelitian ini menggunakan exhaust fan otomatis sebagai aktuator sirkulasi udara |
| 6 | R. Muhamad | 2023 | Rancang Bangun Alat dan Sistem Monitoring Kualitas Udara Nitrogen Dioksida | Monitoring kualitas udara gas NO₂ | Sistem berfokus pada pemantauan nitrogen dioksida | Penelitian ini memakai parameter gas umum MQ-135 dan debu GP2Y1010AU0F |
| 7 | I. M. P. S. Gotama, N. Anwar, A. Yulfitri, dkk. | 2025 | Integration of IoT and BMKG API for Android-based Air Quality Monitoring and Urban Heat Island Prediction System | IoT, API BMKG, Android | Sistem mengintegrasikan data IoT dan API untuk monitoring kualitas udara | Penelitian ini menggunakan ThingSpeak sebagai cloud monitoring sederhana |
| 8 | F. Ara | 2023 | Development of an IoT-Based Environment Monitoring System for Air and Sound Pollution | IoT, monitoring polusi udara dan suara | Sistem memantau parameter lingkungan berbasis IoT | Penelitian ini fokus pada kualitas udara indoor dan kontrol fan otomatis |

Narasi:

```text
Berdasarkan beberapa penelitian terdahulu, sistem monitoring kualitas udara umumnya menggunakan sensor gas, sensor debu, mikrokontroler, dan teknologi IoT untuk menampilkan data secara real-time. Penelitian ini menggabungkan sensor MQ-135 untuk mendeteksi gas/polutan dan sensor GP2Y1010AU0F untuk mendeteksi partikel debu. Sistem juga dilengkapi monitoring lokal melalui LCD I2C, monitoring cloud melalui ThingSpeak, serta kontrol exhaust fan dan buzzer secara otomatis berdasarkan ambang batas kualitas udara.
```

## 2.2 Kualitas Udara

Kualitas udara adalah kondisi udara berdasarkan kandungan zat pencemar, gas, partikel debu, dan unsur lain yang dapat memengaruhi kesehatan manusia.

Parameter kualitas udara pada sistem ini:

- Gas/polutan udara dari sensor MQ-135.
- Partikel debu dari sensor GP2Y1010AU0F.
- Status kualitas udara:
  - Baik
  - Sedang
  - Buruk/Tidak Sehat

## 2.3 Internet of Things

Internet of Things atau IoT adalah konsep perangkat fisik yang terhubung ke internet untuk mengirim, menerima, dan memproses data.

Pada penelitian ini, IoT digunakan untuk:

- Menghubungkan ESP32 ke jaringan WiFi.
- Mengirim data sensor ke ThingSpeak.
- Memantau kualitas udara secara real-time melalui cloud.

## 2.4 ESP32

ESP32 adalah mikrokontroler yang memiliki WiFi dan Bluetooth terintegrasi. ESP32 digunakan sebagai pengendali utama sistem.

Fungsi ESP32 pada sistem:

- Membaca data analog MQ-135.
- Membaca data analog GP2Y1010AU0F.
- Mengontrol LED sensor debu.
- Mengontrol LCD I2C.
- Mengontrol buzzer.
- Mengontrol relay/MOSFET exhaust fan.
- Mengirim data ke ThingSpeak melalui WiFi.

Spesifikasi yang relevan:

| Parameter | Keterangan |
|---|---|
| Tegangan kerja | 3.3V |
| ADC | 12-bit |
| WiFi | 2.4GHz |
| Pin ADC rekomendasi | ADC1 GPIO32–GPIO39 |
| Komunikasi LCD | I2C |

## 2.5 Sensor MQ-135

Sensor MQ-135 adalah sensor gas yang dapat mendeteksi beberapa jenis gas dan polutan seperti CO₂, NH₃, NOx, benzena, asap, dan gas lain.

Fungsi MQ-135 pada sistem:

- Membaca kondisi gas/polutan udara.
- Menghasilkan sinyal analog.
- Menjadi salah satu parameter penentu status kualitas udara.

Catatan:

- Sensor perlu waktu pemanasan.
- Nilai pembacaan perlu dikalibrasi.
- Untuk penelitian awal, MQ-135 dapat digunakan sebagai indikator relatif kualitas udara.

## 2.6 Sensor GP2Y1010AU0F

GP2Y1010AU0F adalah sensor debu optik yang bekerja dengan prinsip pantulan cahaya LED terhadap partikel debu.

Fungsi GP2Y1010AU0F pada sistem:

- Membaca partikel debu di udara.
- Menghasilkan tegangan analog.
- Menjadi parameter tambahan untuk menentukan kualitas udara.

Timing pembacaan sensor:

```text
LED ON
delay 280 microseconds
baca ADC
delay 40 microseconds
LED OFF
delay 9680 microseconds
```

## 2.7 LCD I2C

LCD I2C digunakan untuk menampilkan data secara lokal.

Data yang ditampilkan:

```text
MQ: xxxx
Dust: xxxx
Status: BAIK/SEDANG/BURUK
Fan: ON/OFF
```

## 2.8 Buzzer

Buzzer digunakan sebagai alarm ketika kualitas udara berada pada kondisi buruk.

Kondisi aktif:

```text
Jika MQ-135 atau debu melebihi threshold ON, buzzer ON.
Jika nilai turun di bawah threshold OFF, buzzer OFF.
```

## 2.9 Exhaust Fan

Exhaust fan digunakan untuk membantu sirkulasi udara ketika kualitas udara buruk.

Kontrol fan dilakukan melalui:

- Relay module untuk fan AC.
- MOSFET driver untuk fan DC.

Catatan keamanan:

- Fan tidak boleh disambungkan langsung ke pin ESP32.
- Untuk fan AC wajib memakai isolasi relay/SSR yang aman.
- Untuk fan DC, ground driver dan ESP32 harus disatukan.

## 2.10 ThingSpeak

ThingSpeak adalah platform IoT cloud untuk menyimpan dan menampilkan data sensor.

Field yang digunakan:

| Field | Data |
|---:|---|
| Field 1 | MQ135 ADC |
| Field 2 | MQ135 Voltage |
| Field 3 | Dust ADC |
| Field 4 | Dust Density |
| Field 5 | Status Numeric |
| Field 6 | Fan Status |

## 2.11 Arduino IDE

Arduino IDE digunakan untuk menulis, compile, dan upload program ke ESP32.

Library yang digunakan:

```text
WiFi
HTTPClient
Wire
LiquidCrystal_I2C
```

## 2.12 Kerangka Berpikir

Kerangka berpikir penelitian:

```text
Masalah kualitas udara dalam ruangan
        ↓
Dibutuhkan monitoring real-time
        ↓
MQ-135 membaca gas/polutan
GP2Y1010AU0F membaca debu
        ↓
ESP32 mengolah data sensor
        ↓
LCD menampilkan data lokal
ThingSpeak menampilkan data cloud
        ↓
Jika melewati threshold:
buzzer ON dan exhaust fan ON
        ↓
Udara membaik:
buzzer OFF dan exhaust fan OFF
```

## 2.13 Manajemen Referensi Mendeley

Mendeley digunakan untuk mengelola referensi penelitian, membuat sitasi di Microsoft Word, dan menghasilkan daftar pustaka otomatis.

Kebutuhan referensi:

| Kebutuhan | File/Lokasi | Keterangan |
|---|---|---|
| File BibTeX Mendeley | `references/google-scholar-mendeley.bib` | Hasil scraping Google Scholar untuk di-import ke Mendeley |
| Ringkasan artikel | `references/google-scholar-artikel.md` | Daftar artikel hasil pencarian |
| Panduan penggunaan | `PANDUAN_MENDELEY_GOOGLE_SCHOLAR_WORD.md` | Panduan import referensi dan sitasi Word |
| Script scraping | `tools/scrape_google_scholar_for_mendeley.py` | Script Python pencari artikel Google Scholar |

Alur penggunaan referensi:

```text
Jalankan script Python
        ↓
Hasilkan file .bib
        ↓
Import .bib ke Mendeley
        ↓
Cek metadata artikel
        ↓
Gunakan Mendeley Cite di Microsoft Word
        ↓
Insert Citation
        ↓
Insert Bibliography
```

Perintah scraping artikel:

```bash
python3 tools/scrape_google_scholar_for_mendeley.py --limit 8 --output-dir references
```

Catatan:

- Hasil scraping wajib dicek ulang karena metadata Google Scholar bisa tidak lengkap.
- Jika ada CAPTCHA, pencarian artikel dapat dilakukan manual dari Google Scholar.
- Untuk skripsi, gunakan artikel jurnal/prosiding yang jelas sumber, tahun, penulis, dan DOI/URL.

## 2.14 Daftar Referensi Awal

Referensi awal yang dapat dimasukkan ke Mendeley:

| No | Penulis | Tahun | Judul | Sumber |
|---:|---|---:|---|---|
| 1 | T. Sutikno, W. Arsadiando, dkk. | 2025 | Internet of Things-based Air Quality Monitoring system using Carbon Monoxide and Particulate Matter Parameters | International conference/proceeding |
| 2 | I. Humphrey, A. D. Adewoyin, dkk. | 2025 | Design and Deployment of a Low-Cost IoT-Based Air Quality Monitoring System Using ESP32, BME688, and MQ135 Sensors in Urban Lagos, Nigeria | Journal of Applied ... |
| 3 | O. Alsamrai, M. D. Redel | 2025 | Real-time intelligent monitoring of outdoor air quality in an urban environment using IoT and machine learning algorithms | Artikel HTML/jurnal |
| 4 | I. Ariska | 2025 | Monitoring Kualitas Udara Indoor Menggunakan Sensor Debu Optik GP2Y1010AU0F dan Sensor Gas MQ-135 Berbasis Internet of Things | PDF/karya ilmiah |
| 5 | N. C. SM, S. Selvaganesan | 2025 | Air Quality Monitoring and Purifying System | International conference/proceeding |
| 6 | R. Muhamad | 2023 | Rancang Bangun Alat dan Sistem Monitoring Kualitas Udara Nitrogen Dioksida | Karya ilmiah |
| 7 | I. M. P. S. Gotama, N. Anwar, A. Yulfitri, dkk. | 2025 | Integration of IoT and BMKG API for Android-based Air Quality Monitoring and Urban Heat Island Prediction System | IEEE proceeding |
| 8 | F. Ara | 2023 | Development of an IoT-Based Environment Monitoring System for Air and Sound Pollution | Karya ilmiah |

Contoh narasi sitasi:

```text
Penelitian mengenai monitoring kualitas udara berbasis IoT telah banyak dilakukan dengan memanfaatkan mikrokontroler dan sensor gas. Humphrey dkk. mengembangkan sistem monitoring kualitas udara biaya rendah menggunakan ESP32 dan MQ135. Sementara itu, penelitian lain memanfaatkan parameter partikulat dan gas untuk meningkatkan akurasi pemantauan kualitas udara. Berdasarkan penelitian tersebut, sistem yang dirancang pada penelitian ini menggabungkan sensor MQ-135 dan GP2Y1010AU0F serta menambahkan kontrol exhaust fan otomatis sebagai tindakan terhadap kondisi udara buruk.
```

---

# BAB III  
# METODOLOGI PENELITIAN

## 3.1 Metode Penelitian

Metode penelitian yang digunakan adalah metode rancang bangun, yaitu merancang, membuat, menguji, dan mengevaluasi sistem monitoring kualitas udara berbasis ESP32.

Tahapan penelitian:

1. Studi literatur.
2. Analisis kebutuhan sistem.
3. Perancangan hardware.
4. Perancangan software.
5. Implementasi sistem.
6. Pengujian sensor.
7. Pengujian IoT ThingSpeak.
8. Pengujian kontrol fan dan buzzer.
9. Analisis hasil.
10. Penyusunan laporan.

## 3.2 Waktu dan Tempat Penelitian

```text
Waktu penelitian : [Isi waktu penelitian]
Tempat penelitian: [Isi lokasi penelitian]
```

## 3.3 Alat dan Bahan

### 3.3.1 Hardware

| No | Komponen | Jumlah | Fungsi |
|---:|---|---:|---|
| 1 | ESP32 DevKit | 1 | Mikrokontroler utama |
| 2 | MQ-135 | 1 | Sensor gas/polutan |
| 3 | GP2Y1010AU0F | 1 | Sensor debu |
| 4 | LCD I2C | 1 | Tampilan lokal |
| 5 | Buzzer | 1 | Alarm |
| 6 | Relay/MOSFET | 1 | Driver fan |
| 7 | Exhaust fan | 1 | Sirkulasi udara |
| 8 | Power supply | 1 | Sumber daya |
| 9 | Kabel jumper | Secukupnya | Penghubung |
| 10 | Breadboard/PCB | 1 | Media rangkaian |

### 3.3.2 Software

| No | Software | Fungsi |
|---:|---|---|
| 1 | Arduino IDE | Menulis dan upload program |
| 2 | ESP32 Board Package | Board support ESP32 |
| 3 | LiquidCrystal_I2C | Library LCD |
| 4 | ThingSpeak | Monitoring cloud |
| 5 | Serial Monitor | Debugging |

## 3.4 Analisis Kebutuhan Sistem

### 3.4.1 Kebutuhan Input

- Nilai analog sensor MQ-135.
- Nilai analog sensor GP2Y1010AU0F.

### 3.4.2 Kebutuhan Proses

- Konversi ADC ke tegangan.
- Perhitungan dust density.
- Penentuan status kualitas udara.
- Pengiriman data ke ThingSpeak.
- Kontrol fan dan buzzer berdasarkan threshold.

### 3.4.3 Kebutuhan Output

- Tampilan LCD.
- Data ThingSpeak.
- Serial Monitor.
- Buzzer.
- Exhaust fan.

## 3.5 Perancangan Sistem

### 3.5.1 Blok Diagram Sistem

```text
MQ-135 ─────────────┐
                    │
GP2Y1010AU0F ───────┤
                    ↓
                 ESP32
                    │
     ┌──────────────┼──────────────┐
     ↓              ↓              ↓
  LCD I2C      ThingSpeak      Buzzer/Fan
```

### 3.5.2 Alur Kerja Sistem

1. ESP32 menyala.
2. Sistem melakukan inisialisasi Serial, LCD, pin, dan WiFi.
3. Sensor MQ-135 membaca polutan gas.
4. Sensor GP2Y1010AU0F membaca debu.
5. ESP32 menghitung tegangan dan status kualitas udara.
6. Data ditampilkan ke LCD.
7. Data dikirim ke ThingSpeak.
8. Jika nilai melewati ambang batas, buzzer dan fan aktif.
9. Jika udara membaik, buzzer dan fan mati.
10. Proses berulang secara periodik.

### 3.5.3 Flowchart Program

```text
Mulai
  ↓
Inisialisasi Serial, LCD, Pin, WiFi
  ↓
Baca MQ-135
  ↓
Baca GP2Y1010AU0F
  ↓
Hitung voltage dan dust density
  ↓
Cek threshold
  ↓
Apakah kualitas udara buruk?
  ├─ Ya  → Fan ON, Buzzer ON
  └─ Tidak → Fan OFF, Buzzer OFF
  ↓
Tampilkan data ke LCD
  ↓
Kirim data ke ThingSpeak
  ↓
Delay minimal 15 detik
  ↓
Ulangi
```

## 3.6 Perancangan Hardware

### 3.6.1 Wiring ESP32

| Perangkat | Pin ESP32 | Keterangan |
|---|---:|---|
| MQ-135 AO | GPIO34 | ADC input |
| GP2Y1010 Vo | GPIO35 | ADC input |
| GP2Y1010 LED | GPIO25 | Digital output |
| LCD SDA | GPIO21 | I2C SDA |
| LCD SCL | GPIO22 | I2C SCL |
| Buzzer | GPIO26 | Digital output |
| Relay/MOSFET Fan | GPIO27 | Digital output |

### 3.6.2 Catatan Rangkaian

- GPIO34 dan GPIO35 hanya input.
- ADC ESP32 maksimal 3.3V.
- Gunakan voltage divider jika output sensor 5V.
- Fan harus memakai driver relay/MOSFET.
- Relay AC wajib diberi isolasi.

## 3.7 Perancangan Software

### 3.7.1 Struktur Sketch

```text
arduino-ide/sketches/SugengIOT/
├── SugengIOT.ino
└── config.h
```

### 3.7.2 Konfigurasi WiFi dan ThingSpeak

```cpp
#pragma once

#define WIFI_SSID "NAMA_WIFI"
#define WIFI_PASSWORD "PASSWORD_WIFI"
#define THINGSPEAK_API_KEY "WRITE_API_KEY"
```

### 3.7.3 Parameter Program

```cpp
#define MQ135_PIN 34
#define DUST_PIN 35
#define DUST_LED_PIN 25
#define BUZZER_PIN 26
#define FAN_PIN 27

#define MQ135_THRESHOLD_ON 1800
#define MQ135_THRESHOLD_OFF 1500
#define DUST_THRESHOLD_ON 0.60
#define DUST_THRESHOLD_OFF 0.45

#define SAMPLE_INTERVAL_MS 2000UL
#define THINGSPEAK_INTERVAL_MS 16000UL
```

### 3.7.4 Logika Status Udara

| Kondisi | Status | Fan | Buzzer |
|---|---|---|---|
| Nilai sensor rendah | BAIK | OFF | OFF |
| Nilai sensor mendekati threshold | SEDANG | OFF | OFF |
| Nilai sensor melewati threshold ON | BURUK | ON | ON |
| Nilai sensor turun di bawah threshold OFF | BAIK/SEDANG | OFF | OFF |

## 3.8 Perancangan ThingSpeak

Konfigurasi channel:

```text
Channel Name: Monitoring Kualitas Udara ESP32
Field 1: MQ135 ADC
Field 2: MQ135 Voltage
Field 3: Dust ADC
Field 4: Dust Density
Field 5: Air Quality Status
Field 6: Fan Status
```

Interval update:

```text
Minimal 15 detik
```

## 3.9 Teknik Kalibrasi

### 3.9.1 Kalibrasi MQ-135

Langkah:

1. Panaskan sensor 5–10 menit.
2. Catat nilai ADC udara bersih.
3. Catat nilai ADC saat ada asap ringan.
4. Catat nilai ADC saat asap pekat.
5. Tentukan threshold.

Tabel kalibrasi:

| Kondisi | MQ135 ADC | MQ135 Voltage | Catatan |
|---|---:|---:|---|
| Udara bersih | [isi] | [isi] | [isi] |
| Asap ringan | [isi] | [isi] | [isi] |
| Asap pekat | [isi] | [isi] | [isi] |

### 3.9.2 Kalibrasi GP2Y1010AU0F

Tabel kalibrasi:

| Kondisi | Dust ADC | Dust Voltage | Dust Density | Catatan |
|---|---:|---:|---:|---|
| Udara bersih | [isi] | [isi] | [isi] | [isi] |
| Debu ringan | [isi] | [isi] | [isi] | [isi] |
| Debu tinggi | [isi] | [isi] | [isi] | [isi] |

## 3.10 Skenario Pengujian

| No | Pengujian | Langkah | Hasil yang Diharapkan |
|---:|---|---|---|
| 1 | ESP32 | Upload sketch | Program berjalan |
| 2 | LCD | Nyalakan sistem | LCD menampilkan data |
| 3 | MQ-135 | Beri asap ringan | Nilai MQ naik |
| 4 | GP2Y1010AU0F | Beri debu | Nilai dust naik |
| 5 | Buzzer | Status buruk | Buzzer ON |
| 6 | Fan | Status buruk | Fan ON |
| 7 | ThingSpeak | Kirim data | Data masuk cloud |
| 8 | WiFi mati | Matikan WiFi | Sistem lokal tetap jalan |

---

# BAB IV  
# HASIL DAN PEMBAHASAN

## 4.1 Implementasi Sistem

Bagian ini menjelaskan hasil implementasi hardware dan software.

Isi yang perlu ditambahkan:

- Foto rangkaian.
- Foto prototype.
- Foto tampilan LCD.
- Screenshot Serial Monitor.
- Screenshot ThingSpeak.
- Penjelasan fungsi tiap komponen.

## 4.2 Hasil Implementasi Hardware

Tabel hasil implementasi:

| Komponen | Status | Keterangan |
|---|---|---|
| ESP32 | Berfungsi | Program berhasil berjalan |
| MQ-135 | Berfungsi | Nilai ADC terbaca |
| GP2Y1010AU0F | Berfungsi | Nilai dust terbaca |
| LCD I2C | Berfungsi | Data tampil |
| Buzzer | Berfungsi | Aktif saat status buruk |
| Fan | Berfungsi | Aktif saat status buruk |
| ThingSpeak | Berfungsi | Data terkirim |

## 4.3 Hasil Implementasi Software

Sketch Arduino IDE berhasil menjalankan fungsi:

- Inisialisasi perangkat.
- Koneksi WiFi.
- Pembacaan sensor.
- Penentuan status udara.
- Kontrol fan dan buzzer.
- Tampilan LCD.
- Upload data ke ThingSpeak.
- Debugging melalui Serial Monitor.

## 4.4 Hasil Pengujian Sensor MQ-135

Tabel pengujian:

| No | Kondisi | ADC | Voltage | Status |
|---:|---|---:|---:|---|
| 1 | Udara bersih | [isi] | [isi] | [isi] |
| 2 | Asap ringan | [isi] | [isi] | [isi] |
| 3 | Asap pekat | [isi] | [isi] | [isi] |

Pembahasan:

```text
Berdasarkan hasil pengujian, nilai ADC sensor MQ-135 mengalami kenaikan ketika terdapat asap atau polutan. Hal ini menunjukkan bahwa sensor MQ-135 dapat digunakan sebagai indikator perubahan kualitas udara.
```

## 4.5 Hasil Pengujian Sensor GP2Y1010AU0F

Tabel pengujian:

| No | Kondisi | ADC | Voltage | Dust Density | Status |
|---:|---|---:|---:|---:|---|
| 1 | Udara bersih | [isi] | [isi] | [isi] | [isi] |
| 2 | Debu ringan | [isi] | [isi] | [isi] | [isi] |
| 3 | Debu tinggi | [isi] | [isi] | [isi] | [isi] |

Pembahasan:

```text
Nilai sensor GP2Y1010AU0F meningkat ketika terdapat partikel debu. Dengan demikian sensor dapat membantu sistem dalam mendeteksi perubahan kualitas udara berdasarkan kandungan partikel.
```

## 4.6 Hasil Pengujian LCD

| No | Data yang Ditampilkan | Status |
|---:|---|---|
| 1 | MQ135 ADC | Berhasil |
| 2 | Dust ADC | Berhasil |
| 3 | Status udara | Berhasil |
| 4 | Status fan | Berhasil |

## 4.7 Hasil Pengujian ThingSpeak

Tabel hasil upload:

| No | Waktu | MQ135 ADC | Dust ADC | Status | Fan | Response |
|---:|---|---:|---:|---|---|---:|
| 1 | [isi] | [isi] | [isi] | [isi] | [isi] | [isi] |
| 2 | [isi] | [isi] | [isi] | [isi] | [isi] | [isi] |
| 3 | [isi] | [isi] | [isi] | [isi] | [isi] | [isi] |

Pembahasan:

```text
Data berhasil dikirim ke ThingSpeak dengan interval minimal 15 detik. ThingSpeak menampilkan data sensor dalam bentuk grafik sehingga monitoring dapat dilakukan secara online.
```

## 4.8 Hasil Pengujian Fan dan Buzzer

| No | Kondisi | MQ135 | Dust | Status | Fan | Buzzer |
|---:|---|---:|---:|---|---|---|
| 1 | Udara normal | [isi] | [isi] | BAIK | OFF | OFF |
| 2 | Polusi sedang | [isi] | [isi] | SEDANG | OFF | OFF |
| 3 | Polusi tinggi | [isi] | [isi] | BURUK | ON | ON |
| 4 | Udara membaik | [isi] | [isi] | BAIK/SEDANG | OFF | OFF |

## 4.9 Pengujian Keseluruhan Sistem

Tabel pengujian integrasi:

| No | Skenario | Hasil yang Diharapkan | Hasil Pengujian | Kesimpulan |
|---:|---|---|---|---|
| 1 | Udara normal | Fan OFF, buzzer OFF | [isi] | [isi] |
| 2 | Asap ringan | Nilai MQ naik | [isi] | [isi] |
| 3 | Debu tinggi | Nilai dust naik | [isi] | [isi] |
| 4 | Status buruk | Fan ON, buzzer ON | [isi] | [isi] |
| 5 | WiFi mati | Sistem lokal tetap berjalan | [isi] | [isi] |
| 6 | Data cloud | ThingSpeak menerima data | [isi] | [isi] |

## 4.10 Analisis Hasil

Contoh narasi:

```text
Berdasarkan hasil pengujian, sistem mampu membaca perubahan kualitas udara melalui sensor MQ-135 dan GP2Y1010AU0F. Ketika nilai sensor melewati ambang batas, sistem mengaktifkan exhaust fan dan buzzer secara otomatis. Data juga berhasil ditampilkan pada LCD dan dikirim ke ThingSpeak. Penggunaan hysteresis membuat fan tidak sering menyala dan mati secara cepat ketika nilai sensor berada di sekitar threshold.
```

## 4.11 Kelebihan dan Kekurangan Sistem

### 4.11.1 Kelebihan

- Sistem dapat memantau kualitas udara secara real-time.
- Memiliki tampilan lokal melalui LCD.
- Dapat dipantau secara online melalui ThingSpeak.
- Fan dan buzzer bekerja otomatis.
- Biaya relatif rendah.
- Sistem tetap berjalan lokal walaupun WiFi gagal.

### 4.11.2 Kekurangan

- Sensor MQ-135 membutuhkan kalibrasi untuk hasil lebih akurat.
- Sensor murah memiliki keterbatasan presisi.
- ThingSpeak gratis memiliki batas interval update.
- Sistem bergantung pada koneksi WiFi untuk monitoring cloud.
- Pembacaan sensor dapat dipengaruhi suhu, kelembapan, dan posisi sensor.

---

# BAB V  
# PENUTUP

## 5.1 Kesimpulan

Contoh kesimpulan:

1. Sistem monitoring kualitas udara berbasis ESP32 berhasil dirancang dan dibuat menggunakan sensor MQ-135 dan GP2Y1010AU0F.
2. Sistem mampu membaca nilai sensor, menampilkan data pada LCD, dan mengirim data ke ThingSpeak.
3. Sistem mampu mengaktifkan buzzer dan exhaust fan secara otomatis ketika kualitas udara berada pada kondisi buruk.
4. Penggunaan hysteresis membantu mengurangi kondisi fan menyala dan mati secara berulang dalam waktu singkat.
5. Sistem dapat digunakan sebagai prototype monitoring kualitas udara dalam ruangan berbasis IoT.

## 5.2 Saran

Contoh saran:

1. Sistem dapat dikembangkan dengan sensor kualitas udara yang lebih akurat.
2. Perlu dilakukan kalibrasi menggunakan alat ukur pembanding agar hasil lebih presisi.
3. Sistem dapat ditambahkan penyimpanan data lokal seperti microSD.
4. Sistem dapat dikembangkan dengan aplikasi mobile atau dashboard web mandiri.
5. Casing perlu dirancang agar sensor mendapatkan aliran udara yang baik.
6. Untuk penggunaan jangka panjang, perlu pengujian stabilitas lebih lama.

---

# Lampiran

## Lampiran A — Code Deploy Arduino IDE

Lokasi sketch:

```text
arduino-ide/sketches/SugengIOT/SugengIOT.ino
```

Lokasi konfigurasi:

```text
arduino-ide/sketches/SugengIOT/config.h
```

## Lampiran B — Contoh Serial Monitor

```text
WiFi connected: 192.168.1.20
MQ135 ADC: 1420 | MQ135 Voltage: 1.14V | Dust ADC: 850 | Dust Voltage: 0.68V | Dust Density: 0.12 | Status: SEDANG | Fan: OFF | Buzzer: OFF | WiFi: CONNECTED | ThingSpeak: 200 | Free heap: xxxxx
```

## Lampiran C — Checklist Data Pengujian

- [ ] Tanggal pengujian.
- [ ] Lokasi pengujian.
- [ ] Nilai MQ-135 udara bersih.
- [ ] Nilai MQ-135 saat asap.
- [ ] Nilai GP2Y1010AU0F udara bersih.
- [ ] Nilai GP2Y1010AU0F saat debu.
- [ ] Threshold ON.
- [ ] Threshold OFF.
- [ ] Response fan.
- [ ] Response buzzer.
- [ ] Screenshot LCD.
- [ ] Screenshot ThingSpeak.
- [ ] Screenshot Serial Monitor.

## Lampiran D — Referensi dan Mendeley

File yang disiapkan:

```text
references/google-scholar-mendeley.bib
references/google-scholar-artikel.md
tools/scrape_google_scholar_for_mendeley.py
PANDUAN_MENDELEY_GOOGLE_SCHOLAR_WORD.md
```

Langkah wajib sebelum final laporan:

1. Import `references/google-scholar-mendeley.bib` ke Mendeley.
2. Cek ulang metadata setiap artikel.
3. Hapus referensi yang tidak relevan.
4. Tambahkan DOI/URL resmi jika tersedia.
5. Masukkan sitasi memakai **Mendeley Cite** di Microsoft Word.
6. Generate daftar pustaka otomatis dari Mendeley.
7. Sesuaikan citation style dengan pedoman kampus.
