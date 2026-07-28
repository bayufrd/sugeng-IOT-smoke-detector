# AGENTS.md — Panduan Persiapan, Debugging, Build, dan Deploy Proyek ESP32 IoT

## 1. Ringkasan Proyek

Judul proyek:

**Rancang Bangun Sistem Monitoring Kualitas Udara Berbasis Sensor MQ-135 dan GP2Y1010AU0F dengan Control Exhaust Fan Otomatis**

Tujuan sistem:

- Membaca kualitas udara dalam ruangan secara real-time.
- Menggunakan sensor **MQ-135** untuk gas seperti CO₂, NH₃, NOx, dan polutan gas lain.
- Menggunakan sensor **GP2Y1010AU0F** untuk partikel debu.
- Menampilkan data pada **LCD I2C**.
- Mengirim data ke **ThingSpeak** melalui WiFi ESP32.
- Mengaktifkan **buzzer** dan **exhaust fan** otomatis saat kualitas udara melewati ambang batas.

---

## 2. Kondisi Environment Saat Ini

Sudah tersedia:

```bash
node --version
# v22.20.0

python3 --version
# Python 3.9.6
```

Node.js dan Python bisa dipakai untuk:

- Membuat tool bantu parsing data.
- Membuat dashboard lokal sederhana.
- Membuat script test API ThingSpeak.
- Membantu dokumentasi dan otomasi build.
- Menjalankan PlatformIO jika menggunakan workflow berbasis CLI.

---

## 3. Hardware yang Perlu Disiapkan

### Komponen utama

| Komponen | Fungsi |
|---|---|
| ESP32 DevKit | Mikrokontroler utama, koneksi WiFi, pengolah data |
| Sensor MQ-135 | Deteksi gas/polutan udara |
| Sensor GP2Y1010AU0F | Deteksi debu/partikel |
| LCD 16x2 I2C atau 20x4 I2C | Tampilan lokal |
| Buzzer aktif 3.3V/5V | Alarm kualitas udara buruk |
| Relay module / MOSFET driver | Kontrol exhaust fan |
| Exhaust fan DC/AC | Sirkulasi udara otomatis |
| Power supply 5V/2A | Supply ESP32 dan sensor |
| Power supply fan sesuai spesifikasi | Supply exhaust fan |
| Kabel jumper | Wiring |
| Breadboard / PCB lubang | Prototyping |
| Resistor 150Ω/220Ω | LED GP2Y1010AU0F jika dibutuhkan |
| Kapasitor 220µF | Stabilizer GP2Y1010AU0F sesuai datasheet |
| Logic level / voltage divider | Proteksi ADC ESP32 jika output sensor > 3.3V |

### Catatan penting hardware

- ADC ESP32 maksimal sekitar **3.3V**, jangan masukkan sinyal analog 5V langsung.
- Sensor MQ-135 umumnya butuh pemanasan awal.
- MQ-135 lebih cocok sebagai indikator relatif jika belum dikalibrasi dengan alat referensi.
- Exhaust fan AC harus memakai relay/SSR yang aman dan terisolasi.
- Untuk fan DC, MOSFET lebih disarankan daripada relay jika ingin switching lebih halus.

---

## 4. Software yang Perlu Diinstall

### Opsi A — Arduino IDE

Cocok untuk pengerjaan cepat dan mudah.

Install:

1. **Arduino IDE 2.x**
2. Board package ESP32:
   - URL Board Manager:
     ```text
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
3. Pilih board:
   - `ESP32 Dev Module`
4. Install library:
   - `LiquidCrystal_I2C`
   - `WiFi`
   - `HTTPClient`
   - Library sensor MQ jika diperlukan, atau pakai pembacaan ADC manual.
   - Library GP2Y1010AU0F jika tersedia, atau pakai rumus manual dari datasheet.

### Opsi B — PlatformIO di VS Code

Disarankan untuk proyek lebih rapi, mudah build, dan mudah deploy.

Install:

1. VS Code extension: **PlatformIO IDE**
2. Atau via Python:
   ```bash
   python3 -m pip install --user platformio
   ```

Contoh target board PlatformIO:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
  marcoschwartz/LiquidCrystal_I2C
```

### Driver USB

Cek chip USB pada ESP32:

| Chip USB | Driver |
|---|---|
| CP2102 | Silicon Labs CP210x |
| CH340/CH341 | WCH CH34x |
| FTDI | FTDI VCP Driver |

Cek port di macOS:

```bash
ls /dev/cu.*
```

Biasanya muncul:

```text
/dev/cu.usbserial-xxxx
/dev/cu.SLAB_USBtoUART
/dev/cu.wchusbserialxxxx
```

---

## 5. Akun dan Layanan Cloud

### ThingSpeak

Siapkan:

1. Akun ThingSpeak.
2. Channel baru.
3. Field data, contoh:
   - Field 1: MQ135 Raw
   - Field 2: MQ135 Voltage
   - Field 3: Dust Voltage
   - Field 4: Dust Density
   - Field 5: Air Quality Status
   - Field 6: Fan Status
4. Simpan:
   - `Channel ID`
   - `Write API Key`
   - `Read API Key` jika dashboard publik/private membutuhkan akses.

Catatan:

- ThingSpeak gratis memiliki interval update minimal sekitar **15 detik**.
- Jangan kirim data terlalu cepat agar tidak ditolak.

---

## 6. Rekomendasi Struktur Project ESP32

Jika menggunakan PlatformIO:

```text
Sugeng-IOT/
├── agents.md
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
│   └── config.example.h
├── lib/
├── test/
└── docs/
```

File rahasia seperti SSID WiFi dan API Key jangan di-commit langsung.

Gunakan contoh:

```cpp
#define WIFI_SSID "ISI_WIFI"
#define WIFI_PASSWORD "ISI_PASSWORD"
#define THINGSPEAK_API_KEY "ISI_API_KEY"
```

Lalu buat file lokal seperti `config.h` dan masukkan ke `.gitignore`.

---

## 7. Rekomendasi Wiring ESP32

Contoh pin aman untuk awal:

| Perangkat | Pin ESP32 | Catatan |
|---|---:|---|
| MQ-135 AO | GPIO34 | ADC input only |
| GP2Y1010 Vo | GPIO35 | ADC input only |
| GP2Y1010 LED control | GPIO25 | Digital output |
| LCD I2C SDA | GPIO21 | Default I2C SDA |
| LCD I2C SCL | GPIO22 | Default I2C SCL |
| Buzzer | GPIO26 | Pakai transistor jika buzzer besar |
| Relay/MOSFET fan | GPIO27 | Pakai driver, jangan langsung ke fan |

Catatan ADC:

- Gunakan pin ADC1: GPIO32, 33, 34, 35, 36, 39.
- Hindari ADC2 saat WiFi aktif karena bisa konflik.
- GPIO34/35 input-only, cocok untuk sensor analog.

---

## 8. Alur Program ESP32

Alur utama:

1. ESP32 boot.
2. Inisialisasi Serial Monitor.
3. Inisialisasi LCD I2C.
4. Inisialisasi pin sensor, buzzer, fan.
5. Koneksi WiFi.
6. Baca MQ-135.
7. Baca GP2Y1010AU0F.
8. Hitung tegangan dan estimasi kualitas udara.
9. Tentukan status:
   - Baik
   - Sedang
   - Tidak sehat
10. Tampilkan data ke LCD.
11. Kirim data ke ThingSpeak.
12. Jika melewati ambang batas:
   - buzzer ON
   - exhaust fan ON
13. Jika normal:
   - buzzer OFF
   - exhaust fan OFF
14. Ulangi setiap 15 detik atau lebih.

---

## 9. Kalibrasi Sensor

### MQ-135

Yang perlu dilakukan:

- Panaskan sensor sebelum dipakai.
  - Minimal 5–10 menit untuk pengujian awal.
  - Lebih lama untuk hasil stabil.
- Catat nilai ADC di udara bersih sebagai baseline.
- Tentukan ambang batas berdasarkan pengujian lapangan.
- Jika ingin PPM akurat, perlu kalibrasi Rs/R0 dengan referensi gas atau alat ukur pembanding.

Data yang dicatat:

```text
ADC MQ135 udara bersih:
ADC MQ135 asap ringan:
ADC MQ135 asap pekat:
Voltage udara bersih:
Voltage polusi:
Threshold awal:
```

### GP2Y1010AU0F

Yang perlu dilakukan:

- Ikuti timing LED pulse sesuai datasheet.
- Baca output analog saat LED aktif pada waktu sampling yang tepat.
- Gunakan kapasitor dan resistor sesuai rekomendasi modul/datasheet.
- Bandingkan hasil dengan kondisi:
  - udara bersih
  - debu ringan
  - debu tinggi

Data yang dicatat:

```text
ADC dust udara bersih:
ADC dust debu ringan:
ADC dust debu tinggi:
Voltage dust:
Dust density estimasi:
Threshold awal:
```

---

## 10. Debugging

### Debug Serial Monitor

Gunakan baud rate:

```text
115200
```

Data minimal yang harus dicetak:

```text
WiFi status
IP address
MQ135 ADC
MQ135 voltage
Dust ADC
Dust voltage
Dust density
Air quality status
Fan status
ThingSpeak HTTP response code
Free heap
```

Contoh log:

```text
WiFi connected: 192.168.1.20
MQ135 ADC: 1420 | Voltage: 1.14V
Dust ADC: 850 | Voltage: 0.68V
Status: SEDANG
Fan: OFF
ThingSpeak response: 200
```

### Debug WiFi

Jika gagal koneksi:

- Pastikan SSID dan password benar.
- Gunakan WiFi 2.4GHz, ESP32 tidak mendukung 5GHz.
- Pastikan sinyal cukup.
- Print status WiFi secara berkala.

### Debug ThingSpeak

Jika data tidak masuk:

- Pastikan `Write API Key` benar.
- Pastikan interval upload minimal 15 detik.
- Cek HTTP response:
  - `200`: sukses.
  - `400`: request salah.
  - `401/403`: API key salah.
  - `-1`: koneksi gagal.
- Test API dari komputer:

```bash
curl "https://api.thingspeak.com/update?api_key=WRITE_API_KEY&field1=123"
```

### Debug Sensor

Jika ADC selalu 0:

- Cek kabel GND.
- Cek pin ADC benar.
- Cek sensor mendapat power.
- Cek output sensor dengan multimeter.

Jika ADC selalu maksimum:

- Output sensor mungkin 5V.
- Gunakan voltage divider.
- Pastikan tidak salah pin.

### Debug LCD I2C

Jika LCD kosong:

- Cek alamat I2C, biasanya `0x27` atau `0x3F`.
- Cek SDA GPIO21 dan SCL GPIO22.
- Putar trimpot kontras LCD.
- Jalankan I2C scanner.

### Debug Fan/Relay

Jika relay tidak aktif:

- Cek logic aktif HIGH atau LOW.
- Cek supply relay.
- Pakai transistor/driver jika modul tidak cocok 3.3V.
- Jangan menghubungkan fan langsung ke pin ESP32.

---

## 11. Build dan Upload

### Arduino IDE

Langkah:

1. Pilih board `ESP32 Dev Module`.
2. Pilih port ESP32.
3. Set baud upload default.
4. Klik Verify.
5. Klik Upload.
6. Buka Serial Monitor 115200.

Jika upload gagal:

- Tekan dan tahan tombol `BOOT`.
- Klik upload.
- Lepas saat muncul `Connecting...`.
- Pastikan kabel USB mendukung data, bukan hanya charging.

### PlatformIO

Build:

```bash
pio run
```

Upload:

```bash
pio run --target upload
```

Monitor serial:

```bash
pio device monitor --baud 115200
```

Upload dan monitor:

```bash
pio run --target upload && pio device monitor --baud 115200
```

Jika port perlu ditentukan:

```bash
pio run --target upload --upload-port /dev/cu.SLAB_USBtoUART
```

---

## 12. Deploy Prototype

Checklist deploy:

- Sensor sudah terpasang stabil pada casing.
- Lubang udara sensor tidak tertutup.
- MQ-135 tidak terlalu dekat dengan exhaust fan agar pembacaan tidak bias.
- GP2Y1010AU0F memiliki jalur udara yang cukup.
- Kabel power kuat dan tidak mudah lepas.
- Fan memiliki supply terpisah jika arus besar.
- Ground power ESP32 dan driver fan disatukan untuk kontrol DC.
- Relay/AC diberi isolasi aman.
- ThingSpeak menerima data stabil.
- LCD terbaca jelas.
- Buzzer tidak terlalu keras untuk penggunaan ruangan.
- Ambang batas sudah diuji pada kondisi normal dan kondisi polusi.

---

## 13. Ambang Batas Awal

Karena MQ-135 dan GP2Y1010AU0F perlu kalibrasi, gunakan threshold awal berbasis pengujian lokal.

Contoh kategori awal:

```text
BAIK:
- MQ135 ADC rendah/stabil
- Dust voltage rendah

SEDANG:
- MQ135 naik dari baseline
- Dust mulai naik

TIDAK SEHAT:
- MQ135 jauh di atas baseline
- Dust density tinggi
- Fan ON
- Buzzer ON
```

Rekomendasi metode sederhana:

```text
threshold_mq135 = baseline_mq135 + 30% dari baseline
threshold_dust = baseline_dust + 30% dari baseline
```

Untuk skripsi/laporan, dokumentasikan:

- Nilai baseline.
- Nilai saat diberi asap/debu.
- Nilai threshold.
- Reaksi fan dan buzzer.
- Delay respon sistem.

---

## 14. Pengujian yang Perlu Dilakukan

### Test unit hardware

- ESP32 menyala.
- Serial monitor berjalan.
- LCD menyala.
- MQ-135 menghasilkan ADC.
- GP2Y1010AU0F menghasilkan ADC.
- Buzzer ON/OFF.
- Fan ON/OFF.
- WiFi connect.
- ThingSpeak update.

### Test integrasi

| Skenario | Ekspektasi |
|---|---|
| Udara normal | Fan OFF, buzzer OFF, status BAIK/SEDANG |
| Asap ringan | Nilai MQ naik, status berubah |
| Debu | Nilai dust naik |
| Nilai melewati threshold | Fan ON, buzzer ON |
| Udara membaik | Fan OFF, buzzer OFF |
| WiFi mati | Sistem lokal tetap jalan |
| ThingSpeak gagal | Data lokal tetap tampil |

### Test durasi

Jalankan alat minimal:

- 30 menit untuk test awal.
- 2–4 jam untuk stabilitas.
- 24 jam untuk data laporan jika memungkinkan.

Catat:

- Apakah ESP32 restart.
- Apakah data ThingSpeak bolong.
- Apakah pembacaan sensor drift.
- Apakah fan terlalu sering ON/OFF.

Tambahkan hysteresis agar fan tidak berkedip:

```text
Fan ON jika nilai > threshold_on
Fan OFF jika nilai < threshold_off
threshold_off lebih rendah dari threshold_on
```

---

## 15. Keamanan

- Jangan sambungkan beban fan langsung ke pin ESP32.
- Untuk fan AC, gunakan relay/SSR dengan isolasi dan box tertutup.
- Jangan menyentuh rangkaian AC saat menyala.
- Gunakan sekering jika menggunakan beban AC.
- Pisahkan jalur tegangan tinggi dan tegangan rendah.
- Gunakan adaptor berkualitas.
- Pastikan casing memiliki ventilasi untuk sensor.

---

## 16. Peran Node.js dan Python

### Node.js

Bisa dipakai untuk:

- Dashboard lokal berbasis web.
- Simulasi data sensor.
- Script baca API ThingSpeak.
- Backend jika nanti tidak memakai ThingSpeak.

Contoh kebutuhan:

```bash
npm init -y
npm install express axios
```

### Python 3

Bisa dipakai untuk:

- Parsing data CSV dari ThingSpeak.
- Analisis data pengujian.
- Plot grafik.
- Membaca dokumen `.docx`.
- Script validasi HTTP API.

Contoh library opsional:

```bash
python3 -m pip install --user python-docx pandas matplotlib requests
```

---

## 17. File Rahasia yang Perlu Dijaga

Jangan upload ke GitHub:

- Password WiFi.
- ThingSpeak Write API Key.
- Token private.
- Data pribadi jaringan.

Gunakan `.gitignore`:

```gitignore
include/config.h
.env
*.local
```

---

## 18. Checklist Persiapan Final

### Hardware

- [ ] ESP32 DevKit
- [ ] MQ-135
- [ ] GP2Y1010AU0F
- [ ] LCD I2C
- [ ] Buzzer
- [ ] Relay/MOSFET driver
- [ ] Exhaust fan
- [ ] Power supply
- [ ] Kabel dan PCB/breadboard
- [ ] Casing

### Software

- [ ] Arduino IDE atau PlatformIO
- [ ] Board package ESP32
- [ ] Driver USB CP210x/CH340/FTDI
- [ ] Library LCD I2C
- [ ] Library HTTP/WiFi
- [ ] Akun ThingSpeak
- [ ] API Key ThingSpeak
- [ ] Serial monitor siap 115200

### Debugging

- [ ] Serial log lengkap
- [ ] I2C scanner untuk LCD
- [ ] Test ADC sensor
- [ ] Test WiFi
- [ ] Test HTTP ThingSpeak
- [ ] Test buzzer
- [ ] Test fan
- [ ] Test threshold
- [ ] Test hysteresis

### Deploy

- [ ] Wiring rapi dan aman
- [ ] Sensor tidak tertutup casing
- [ ] Fan aman dikontrol driver
- [ ] Data tampil di LCD
- [ ] Data masuk ThingSpeak
- [ ] Sistem tetap jalan saat WiFi gagal
- [ ] Data pengujian dicatat untuk laporan

---

## 19. Rekomendasi Tahapan Pengerjaan

1. Test ESP32 blink dan serial monitor.
2. Test LCD I2C.
3. Test MQ-135 ADC.
4. Test GP2Y1010AU0F ADC.
5. Test buzzer.
6. Test relay/MOSFET dan fan.
7. Gabungkan sensor + LCD.
8. Tambahkan WiFi.
9. Tambahkan upload ThingSpeak.
10. Tambahkan logika threshold.
11. Tambahkan hysteresis fan.
12. Rapikan casing.
13. Lakukan pengujian dan catat data.
14. Finalisasi dokumentasi laporan.

---

## 20. Output Minimum yang Harus Ada di Alat

LCD:

```text
MQ: xxxx
Dust: xxxx
Status: BAIK/SEDANG/BURUK
Fan: ON/OFF
```

ThingSpeak:

```text
field1 = MQ135 ADC
field2 = MQ135 Voltage
field3 = Dust ADC
field4 = Dust Voltage / Density
field5 = Status numeric
field6 = Fan status
```

Serial Monitor:

```text
MQ135, Dust, Status, Fan, WiFi, ThingSpeak response
```

---

## 21. Catatan untuk Laporan

Poin yang perlu dicatat agar laporan kuat:

- Alasan memakai ESP32: WiFi bawaan, ADC, murah, cocok IoT.
- Alasan memakai MQ-135: deteksi gas polutan.
- Alasan memakai GP2Y1010AU0F: deteksi partikel/debu.
- Alasan memakai ThingSpeak: monitoring cloud real-time.
- Kekurangan sistem:
  - Akurasi sensor murah terbatas.
  - Perlu kalibrasi.
  - ThingSpeak gratis dibatasi interval.
  - Bergantung koneksi internet.
- Kelebihan sistem:
  - Real-time.
  - Ada kontrol otomatis fan.
  - Ada tampilan lokal dan cloud.
  - Biaya relatif rendah.