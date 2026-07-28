# Roadmap Arduino IDE — ESP32 IoT Monitoring Kualitas Udara

## 1. Tujuan

Roadmap ini menjelaskan alur kerja dari instalasi Arduino IDE GUI sampai deploy proyek ESP32 untuk monitoring kualitas udara MQ-135 dan GP2Y1010AU0F.

Arduino IDE adalah aplikasi GUI. Folder ini berisi installer otomatis berbasis Python agar proses download dan setup awal tidak perlu manual.

---

## 2. Struktur Folder

```text
arduino-ide/
├── ROADMAP.md
├── install_arduino_ide.py
├── downloads/
└── Arduino IDE.app
```

Keterangan:

- `install_arduino_ide.py`: script Python untuk download Arduino IDE dari GitHub release resmi.
- `downloads/`: tempat file `.dmg` Arduino IDE.
- `Arduino IDE.app`: aplikasi Arduino IDE hasil copy dari DMG.
- `ROADMAP.md`: dokumentasi alur instalasi sampai deployment.

---

## 3. Tahap 1 — Install Arduino IDE

Jalankan:

```bash
python3 arduino-ide/install_arduino_ide.py
```

Script akan:

1. Membaca release terbaru dari GitHub resmi Arduino IDE.
2. Memilih installer macOS sesuai arsitektur:
   - Apple Silicon: ARM64
   - Intel: 64bit
3. Download file `.dmg`.
4. Mount DMG.
5. Copy `Arduino IDE.app` ke folder `arduino-ide/`.
6. Install ESP32 board package via `arduino-cli` bawaan jika tersedia.
7. Install library LCD I2C jika tersedia.

Buka aplikasi:

```bash
open "arduino-ide/Arduino IDE.app"
```

---

## 4. Tahap 2 — Validasi Instalasi

Cek:

- Arduino IDE terbuka.
- Menu board ESP32 tersedia.
- Port ESP32 terbaca.
- Library LCD I2C tersedia.

Jika ESP32 board belum tersedia, buka Arduino IDE:

```text
Settings > Additional Boards Manager URLs
```

Isi:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Lalu install:

```text
Tools > Board > Boards Manager > esp32 by Espressif Systems
```

---

## 5. Tahap 3 — Persiapan Driver USB

Cek port:

```bash
ls /dev/cu.*
```

Port umum:

```text
/dev/cu.SLAB_USBtoUART
/dev/cu.wchusbserialxxxx
/dev/cu.usbserial-xxxx
```

Jika port tidak muncul:

- Gunakan kabel USB data.
- Install driver CP210x / CH340 / FTDI sesuai chip ESP32.
- Restart Arduino IDE.
- Cabut pasang ESP32.

---

## 6. Tahap 4 — Setup Sketch ESP32

Buat folder sketch:

```text
arduino-ide/sketches/SugengIOT/
├── SugengIOT.ino
└── config.h
```

Isi `config.h`:

```cpp
#define WIFI_SSID "NAMA_WIFI"
#define WIFI_PASSWORD "PASSWORD_WIFI"
#define THINGSPEAK_API_KEY "WRITE_API_KEY"
```

Jangan upload `config.h` ke repository publik.

---

## 7. Tahap 5 — Wiring Hardware

| Perangkat | ESP32 |
|---|---:|
| MQ-135 AO | GPIO34 |
| GP2Y1010 Vo | GPIO35 |
| GP2Y1010 LED | GPIO25 |
| LCD SDA | GPIO21 |
| LCD SCL | GPIO22 |
| Buzzer | GPIO27 |
| Fan Relay/MOSFET | GPIO26 |

Catatan:

- GPIO34/35 hanya input.
- ADC ESP32 maksimal 3.3V.
- Gunakan voltage divider jika output sensor 5V.
- Fan tidak boleh langsung ke pin ESP32.
- Buzzer saat ini dipakai di GPIO27 sesuai sketch aktif.
- Relay fan dipakai di GPIO26 sesuai sketch aktif.
- Fan DC 2 kabel dikontrol relay sebagai saklar putus/sambung jalur positif.
- Logika sketch aktif: jika estimasi ppm `>= 80`, buzzer ON dan relay fan ON.
- Fan DC gunakan MOSFET jika ingin solusi lebih halus dan awet.
- Fan AC gunakan relay/SSR dengan isolasi.

### Wiring sensor debu GP2Y1010AU0F

Pin umum sensor/modul `GP2Y1010AU0F`:

- `V-LED` / `LED-VCC` -> `5V`
- `LED-GND` -> `GND`
- `LED` / `LED-CTRL` -> `GPIO25`
- `S-GND` / `GND` -> `GND ESP32`
- `Vo` -> `GPIO35`
- `VCC` -> `5V`

Skema ringkas:

```text
GP2Y1010AU0F VCC        -> 5V ESP32
GP2Y1010AU0F GND        -> GND ESP32
GP2Y1010AU0F Vo         -> GPIO35
GP2Y1010AU0F LED / ILED -> GPIO25
```

Catatan penting pemasangan:

- `GPIO35` hanya input, cocok untuk baca `Vo` analog.
- Saat `WiFi` aktif, pakai `ADC1` seperti `GPIO35`, jangan `ADC2`.
- Output `Vo` jangan melebihi `3.3V` ke ESP32. Jika modul output `5V`, pakai `voltage divider`.
- Banyak modul `GP2Y1010AU0F` butuh resistor dan kapasitor bawaan. Jika pakai sensor bare, ikuti datasheet.
- Arah lubang sensor jangan tertutup casing agar debu bisa lewat.
- Ground sensor debu harus satu ground dengan `ESP32`.

Urutan test cepat:

1. Sambung `VCC`, `GND`, dan `Vo` dulu.
2. Pastikan `Vo` terbaca di `GPIO35`.
3. Sambung pin `LED/ILED` ke `GPIO25` untuk pulse pembacaan.
4. Baru gabungkan ke logika status udara dan upload cloud.

### Wiring relay + fan 2 kabel

Jika fan Anda 2 kabel, paling umum itu fan DC.

#### Sisi kontrol relay ke ESP32

- `VCC relay` -> `5V` atau `3V3` sesuai modul relay
- `GND relay` -> `GND ESP32`
- `IN relay` -> `GPIO26`

#### Sisi beban relay ke fan DC 2 kabel

- `+ power supply fan` -> `COM relay`
- `NO relay` -> `+ fan`
- `- fan` -> `- power supply fan`

Skema ringkas:

```text
ESP32 GPIO26 ---- IN relay
ESP32 GND   ---- GND relay
ESP32 5V/3V3 ---- VCC relay

+ adaptor fan ---- COM relay
NO relay      ---- + fan
- fan         ---- - adaptor fan
```

Arti terminal relay:

- `COM`: jalur masuk sumber daya
- `NO`: Normally Open, terhubung saat relay aktif
- `NC`: Normally Closed, terhubung saat relay tidak aktif

Gunakan `NO` supaya fan default OFF.

Catatan pemasangan adaptor potong:

- `kabel merah adaptor` -> `COM relay`
- `NO relay` -> `kabel merah fan`
- `kabel hitam adaptor` -> `kabel hitam fan`
- Jangan sambung `kabel merah` dan `kabel hitam` ke `GND ESP32`.
- GND ESP32 hanya untuk sisi kontrol relay, bukan jalur daya fan.

#### Jika fan AC 2 kabel

Harap berhati-hati. Tegangan AC berbahaya.

Untuk fan AC, putus salah satu jalur AC lewat `COM` dan `NO` relay, dan pastikan relay memang rated untuk tegangan/arus fan. Jangan menyentuh rangkaian AC saat menyala. Gunakan box isolasi tertutup.

---

## 8. Tahap 6 — Build / Verify

Di Arduino IDE:

```text
Tools > Board > esp32 > ESP32 Dev Module
Tools > Port > pilih port ESP32
Sketch > Verify/Compile
```

Jika gagal:

- Cek board ESP32 sudah terinstall.
- Cek library `LiquidCrystal_I2C`.
- Cek `config.h`.
- Cek syntax sketch.

---

## 9. Tahap 7 — Upload ke ESP32

Klik:

```text
Upload
```

Jika stuck `Connecting...`:

1. Tekan tombol `BOOT`.
2. Tahan sampai upload mulai.
3. Lepas tombol `BOOT`.

Jika masih gagal:

- Turunkan upload speed ke 115200.
- Tutup Serial Monitor.
- Ganti kabel USB.
- Cek port benar.

---

## 10. Tahap 8 — Debugging Serial Monitor

Buka:

```text
Tools > Serial Monitor
```

Baud:

```text
115200
```

Log wajib:

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
Buzzer status
ThingSpeak HTTP response
Free heap
```

---

## 11. Tahap 9 — Debugging Sensor

### MQ-135

- Warm-up minimal 5–10 menit.
- Catat baseline udara bersih.
- Uji dengan asap ringan.
- Gunakan threshold awal `baseline + 30%`.

### GP2Y1010AU0F

- Pastikan timing LED pulse benar.
- Gunakan kapasitor 220µF jika dibutuhkan.
- Catat ADC udara bersih dan kondisi berdebu.
- Gunakan threshold awal `baseline + 30%`.

---

## 12. Tahap 10 — Debugging ThingSpeak

### Setup Channel Settings

Isi `Channel Settings` yang disarankan:

```text
Name        = SugengIOT
Description = Monitoring MQ-135, dust GP2Y1010AU0F, buzzer, fan, ESP32
Field 1     = MQ135 ADC
Field 2     = MQ135 Voltage
Field 3     = MQ135 PPM
Field 4     = Gas Status
Field 5     = Dust Density (mg/m3)
Field 6     = Fan Status
Field 7     = kosong
Field 8     = kosong
Metadata    = opsional
Tags        = esp32,iot,mq135,gp2y1010,thingspeak
Link        = opsional
GitHub      = opsional
Latitude    = opsional
Longitude   = opsional
Video       = kosong
Status      = opsional
```

Untuk channel Anda sekarang, `Field 1 = smoke` sebaiknya diganti supaya cocok dengan sketch aktif.

Arti field dari sketch aktif di [`sendToThingSpeak()`](arduino-ide/sketches/SugengIOT/SugengIOT.ino:87):

```text
field1 = MQ135 ADC
field2 = MQ135 Voltage
field3 = MQ135 PPM
field4 = Gas Status (0 normal, 1 terdeteksi)
field5 = Dust Density (mg/m3)
field6 = Fan Status (0 OFF, 1 ON)
```

Catatan penting:

- `Field 5` di sketch aktif sekarang isi `Dust Density (mg/m3)`.
- Estimasi density dihitung dari [`estimateDustDensity()`](arduino-ide/sketches/SugengIOT/SugengIOT.ino:41).
- Rumus awal: `(dustVoltage - 0.6) / 0.005` lalu nilai negatif jadi `0`.
- Threshold debu di sketch aktif = `800` ADC.
- Log serial sekarang menampilkan tegangan, density, threshold ppm, dan threshold dust.
- Interval upload minimal tetap `15 detik`.
- Simpan `Write API Key` lalu isi ke [`config.h`](arduino-ide/sketches/SugengIOT/config.h).

Test dari komputer:

```bash
curl "https://api.thingspeak.com/update?api_key=WRITE_API_KEY&field1=123"
```

Response sukses berupa angka entry ID.

Interval upload minimal:

```text
15 detik
```

Field cloud final sekarang:

```text
field1 = MQ135 ADC
field2 = MQ135 Voltage
field3 = MQ135 PPM
field4 = Gas Status
field5 = Dust Density (mg/m3)
field6 = Fan Status
```

---

## 13. Tahap 11 — Deployment

Checklist:

- ESP32 menyala stabil.
- LCD tampil.
- Sensor MQ-135 terbaca.
- Sensor GP2Y1010AU0F terbaca.
- WiFi connect.
- ThingSpeak menerima data.
- Fan ON saat status buruk.
- Buzzer ON saat status buruk.
- Fan OFF saat udara membaik.
- Gunakan hysteresis agar fan tidak berkedip.
- Casing punya ventilasi.
- Kabel rapi dan aman.
- Relay/fan AC diberi isolasi.

---

## 14. Tahap 12 — Maintenance

Yang perlu dicatat selama pengujian:

```text
Tanggal pengujian
Lokasi
ADC udara bersih
ADC asap/debu
Threshold ON
Threshold OFF
Status fan
Status buzzer
Response ThingSpeak
Catatan error
```

Durasi test:

- 30 menit untuk test awal.
- 2–4 jam untuk stabilitas.
- 24 jam untuk data laporan jika memungkinkan.

---

## 15. Command Ringkas

Install Arduino IDE:

```bash
python3 arduino-ide/install_arduino_ide.py
```

Buka Arduino IDE:

```bash
open "arduino-ide/Arduino IDE.app"
```

Cek port:

```bash
ls /dev/cu.*
```

Compile sketch `SugengIOT.ino`:

```bash
"arduino-ide/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli" compile --fqbn esp32:esp32:esp32 arduino-ide/sketches/SugengIOT
```

Upload sketch ke ESP32:

```bash
kill 50700 && "arduino-ide/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli" upload -p /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 arduino-ide/sketches/SugengIOT
```

Live serial monitor:

```bash
screen /dev/cu.usbserial-0001 115200
```

Keluar dari `screen`:

```text
Ctrl + A, lalu K, lalu Y
```

Test ThingSpeak:

```bash
curl "https://api.thingspeak.com/update?api_key=WRITE_API_KEY&field1=123"
```
