# Dokumentasi Opsi B — PlatformIO untuk ESP32 IoT

## 1. Tujuan

Dokumentasi ini dipakai untuk instalasi, setup project, build, upload, debugging, dan deployment proyek ESP32:

**Monitoring Kualitas Udara Berbasis MQ-135 dan GP2Y1010AU0F dengan Exhaust Fan Otomatis**

Target:

- ESP32 DevKit
- MQ-135
- GP2Y1010AU0F
- LCD I2C
- Buzzer
- Relay/MOSFET exhaust fan
- ThingSpeak

---

## 2. Kebutuhan Awal

Environment saat ini:

```bash
node --version
# v22.20.0

python3 --version
# Python 3.9.6
```

PlatformIO membutuhkan Python. Node.js opsional untuk dashboard lokal atau tool tambahan.

---

## 3. Instalasi PlatformIO

### Opsi 1 — VS Code Extension

1. Buka VS Code.
2. Buka Extensions.
3. Cari:

```text
PlatformIO IDE
```

4. Install.
5. Restart VS Code jika diminta.

### Opsi 2 — CLI via Python

Install:

```bash
python3 -m pip install --user platformio
```

Cek versi:

```bash
pio --version
```

Jika command `pio` tidak ditemukan, cek path Python user bin:

```bash
python3 -m site --user-base
```

Biasanya binary berada di:

```text
/Users/testdev/Library/Python/3.9/bin
```

Tambahkan ke shell config jika perlu:

```bash
export PATH="/Users/testdev/Library/Python/3.9/bin:$PATH"
```

---

## 4. Driver USB ESP32

Cek port ESP32 di macOS:

```bash
ls /dev/cu.*
```

Contoh:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.wchusbserialxxxx
/dev/cu.usbserial-xxxx
```

Driver sesuai chip:

| Chip USB | Driver |
|---|---|
| CP2102 | Silicon Labs CP210x |
| CH340/CH341 | WCH CH34x |
| FTDI | FTDI VCP |

Jika port tidak muncul:

- Gunakan kabel USB data.
- Install driver sesuai chip.
- Cabut-pasang ESP32.
- Restart VS Code/PlatformIO.
- Cek permission port jika diperlukan.

---

## 5. Membuat Project PlatformIO

### Via VS Code

1. Klik icon PlatformIO.
2. Pilih **New Project**.
3. Isi:
   - Name: `Sugeng-IOT`
   - Board: `Espressif ESP32 Dev Module`
   - Framework: `Arduino`
4. Klik Finish.

### Via CLI

Dari folder project:

```bash
pio project init --board esp32dev
```

Struktur yang disarankan:

```text
Sugeng-IOT/
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
│   ├── config.example.h
│   └── config.h
├── lib/
├── test/
└── docs/
```

---

## 6. Konfigurasi `platformio.ini`

Contoh minimal:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 921600
lib_deps =
  marcoschwartz/LiquidCrystal_I2C
```

Jika upload sering gagal, turunkan:

```ini
upload_speed = 115200
```

Jika perlu port spesifik:

```ini
upload_port = /dev/cu.SLAB_USBtoUART
monitor_port = /dev/cu.SLAB_USBtoUART
```

---

## 7. File Konfigurasi Rahasia

Buat `include/config.example.h`:

```cpp
#pragma once

#define WIFI_SSID "ISI_WIFI"
#define WIFI_PASSWORD "ISI_PASSWORD"
#define THINGSPEAK_API_KEY "ISI_WRITE_API_KEY"
```

Copy ke `include/config.h`:

```bash
cp include/config.example.h include/config.h
```

Edit `include/config.h` dengan data asli.

Tambahkan ke `.gitignore`:

```gitignore
include/config.h
.pio/
.env
*.local
```

---

## 8. Rekomendasi Pin ESP32

| Perangkat | Pin ESP32 | Catatan |
|---|---:|---|
| MQ-135 AO | GPIO34 | ADC input only |
| GP2Y1010 Vo | GPIO35 | ADC input only |
| GP2Y1010 LED | GPIO25 | Output digital |
| LCD SDA | GPIO21 | I2C SDA |
| LCD SCL | GPIO22 | I2C SCL |
| Buzzer | GPIO26 | Output digital |
| Relay/MOSFET Fan | GPIO27 | Output digital |

Catatan:

- ADC ESP32 maksimal sekitar 3.3V.
- Jangan masukkan output analog 5V langsung.
- Gunakan ADC1 untuk sensor analog karena WiFi konflik dengan ADC2.
- Fan tidak boleh langsung ke pin ESP32.

---

## 9. Struktur Program `src/main.cpp`

Alur utama:

1. Include library:
   - `Arduino.h`
   - `WiFi.h`
   - `HTTPClient.h`
   - `Wire.h`
   - `LiquidCrystal_I2C.h`
   - `config.h`
2. Definisikan pin.
3. Inisialisasi Serial.
4. Inisialisasi LCD.
5. Set pin mode.
6. Koneksi WiFi.
7. Baca sensor.
8. Hitung voltage dan status.
9. Kontrol fan dan buzzer.
10. Tampilkan LCD.
11. Upload ThingSpeak.
12. Delay minimal 15 detik.

---

## 10. Build Project

Build:

```bash
pio run
```

Build environment tertentu:

```bash
pio run -e esp32dev
```

Jika sukses akan muncul:

```text
SUCCESS
```

Jika gagal:

- Cek error compile.
- Cek library belum terinstall.
- Cek `platformio.ini`.
- Cek file `include/config.h`.
- Cek typo include.

---

## 11. Upload ke ESP32

Upload otomatis:

```bash
pio run --target upload
```

Upload dengan port:

```bash
pio run --target upload --upload-port /dev/cu.SLAB_USBtoUART
```

Jika stuck di `Connecting...`:

- Tekan dan tahan tombol `BOOT`.
- Lepas saat proses upload mulai.
- Turunkan `upload_speed` ke 115200.
- Pastikan Serial Monitor tertutup.
- Ganti kabel USB jika perlu.

---

## 12. Serial Monitor

Buka monitor:

```bash
pio device monitor --baud 115200
```

Jika port spesifik:

```bash
pio device monitor --port /dev/cu.SLAB_USBtoUART --baud 115200
```

Keluar dari monitor:

```text
Ctrl + C
```

Upload lalu monitor:

```bash
pio run --target upload && pio device monitor --baud 115200
```

---

## 13. Debugging Serial

Output minimal yang harus ada:

```text
Booting...
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

Tambahkan log untuk:

- Status WiFi.
- Nilai ADC mentah.
- Tegangan sensor.
- Status kualitas udara.
- Status fan/buzzer.
- Response HTTP ThingSpeak.
- Free heap.

---

## 14. Debugging Sensor

### MQ-135

Masalah umum:

| Gejala | Penyebab | Solusi |
|---|---|---|
| ADC 0 | GND/VCC salah | Cek wiring |
| ADC maksimum | Output > 3.3V | Pakai voltage divider |
| Nilai tidak stabil | Sensor belum panas | Warm-up 5–10 menit |
| Tidak berubah saat asap | Salah pin / sensor rusak | Cek multimeter |

### GP2Y1010AU0F

Masalah umum:

| Gejala | Penyebab | Solusi |
|---|---|---|
| ADC 0 | LED sensor tidak aktif | Cek GPIO25 |
| Nilai acak | Timing sampling salah | Ikuti timing datasheet |
| Nilai terlalu tinggi | Noise power | Pakai kapasitor 220µF |
| Tidak sensitif | Jalur udara tertutup | Perbaiki posisi sensor |

---

## 15. Debugging LCD I2C

Jika LCD tidak tampil:

1. Cek VCC dan GND.
2. Cek SDA GPIO21.
3. Cek SCL GPIO22.
4. Cek alamat `0x27` atau `0x3F`.
5. Putar trimpot kontras.
6. Jalankan I2C scanner.

Jika library tidak cocok, coba library LCD I2C lain di `lib_deps`.

---

## 16. Debugging WiFi

Checklist:

- ESP32 hanya mendukung WiFi 2.4GHz.
- SSID dan password benar.
- Hindari WiFi captive portal.
- Pastikan sinyal kuat.
- Tambahkan timeout koneksi agar sistem lokal tetap jalan.

Log yang disarankan:

```text
WiFi status:
IP address:
RSSI:
Reconnect count:
```

---

## 17. Debugging ThingSpeak

ThingSpeak gratis minimal interval update sekitar 15 detik.

Test API dari komputer:

```bash
curl "https://api.thingspeak.com/update?api_key=WRITE_API_KEY&field1=123"
```

Response:

| Response | Arti |
|---|---|
| Angka entry ID | sukses |
| 0 | gagal / interval terlalu cepat / key salah |
| 400 | request salah |
| 401/403 | API key salah |
| -1 dari ESP32 | koneksi gagal |

Field rekomendasi:

```text
field1 = MQ135 ADC
field2 = MQ135 Voltage
field3 = Dust ADC
field4 = Dust Voltage / Density
field5 = Status numeric
field6 = Fan status
```

---

## 18. Debugging Fan dan Buzzer

### Buzzer

- Pastikan tipe buzzer aktif jika hanya ON/OFF.
- Gunakan transistor jika arus besar.
- Cek polaritas.
- Test manual `digitalWrite(BUZZER_PIN, HIGH)`.

### Fan

- Fan DC lebih baik dikontrol MOSFET.
- Fan AC harus pakai relay/SSR dengan isolasi.
- Jangan hubungkan fan langsung ke pin ESP32.
- Untuk fan DC, ground ESP32 dan ground supply fan harus tersambung.
- Tambahkan hysteresis agar fan tidak berkedip.

Contoh logika:

```text
Fan ON jika nilai > threshold_on
Fan OFF jika nilai < threshold_off
threshold_off lebih rendah dari threshold_on
```

---

## 19. Kalibrasi

### MQ-135

Catat data:

```text
ADC udara bersih:
Voltage udara bersih:
ADC asap ringan:
ADC asap pekat:
Threshold ON:
Threshold OFF:
```

Threshold awal:

```text
threshold_on = baseline + 30% baseline
threshold_off = baseline + 20% baseline
```

### GP2Y1010AU0F

Catat data:

```text
ADC udara bersih:
Voltage udara bersih:
ADC debu ringan:
ADC debu tinggi:
Threshold ON:
Threshold OFF:
```

---

## 20. Deployment Prototype

Sebelum deploy:

- Build sukses tanpa error.
- Upload sukses.
- Serial log normal.
- LCD tampil.
- MQ-135 terbaca.
- GP2Y1010AU0F terbaca.
- WiFi reconnect otomatis.
- ThingSpeak menerima data.
- Fan dan buzzer aktif saat threshold terlampaui.
- Fan dan buzzer mati saat udara membaik.
- Casing punya ventilasi.
- Kabel aman dan tidak mudah lepas.
- Relay/fan AC diberi isolasi aman.

---

## 21. Command Ringkas

Build:

```bash
pio run
```

Upload:

```bash
pio run --target upload
```

Monitor:

```bash
pio device monitor --baud 115200
```

Upload + monitor:

```bash
pio run --target upload && pio device monitor --baud 115200
```

Clean build:

```bash
pio run --target clean
pio run
```

List board:

```bash
pio boards esp32
```

List device:

```bash
pio device list
```

---

## 22. Checklist Akhir PlatformIO

- [ ] PlatformIO IDE terinstall.
- [ ] `pio --version` berjalan.
- [ ] Project dibuat dengan board `esp32dev`.
- [ ] `platformio.ini` benar.
- [ ] Library LCD I2C masuk `lib_deps`.
- [ ] `include/config.h` dibuat.
- [ ] `.gitignore` melindungi file rahasia.
- [ ] `pio run` sukses.
- [ ] Upload ke ESP32 sukses.
- [ ] Serial monitor 115200 tampil.
- [ ] MQ-135 terbaca.
- [ ] GP2Y1010AU0F terbaca.
- [ ] LCD tampil.
- [ ] Buzzer aktif sesuai threshold.
- [ ] Fan aktif sesuai threshold.
- [ ] ThingSpeak menerima data.
- [ ] Prototype siap deploy.