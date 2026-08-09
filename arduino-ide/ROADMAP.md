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
| MQ-135 DO | GPIO15 |
| MQ-135 AO | GPIO34 |
| GP2Y1010 Vo | GPIO35 |
| GP2Y1010 LED | GPIO25 |
| LCD SDA | GPIO21 |
| LCD SCL | GPIO22 |
| Buzzer | GPIO27 |
| Fan Relay | GPIO26 |
| Relay Lampu Hijau | GPIO14 |
| Relay Lampu Kuning | GPIO12 |
| Relay Lampu Merah | GPIO13 |

Catatan:

- **LCD yang dipakai: LCD 20x4 I2C (modul 2004) dengan address 0x27.**
- Pin I2C: SDA GPIO21, SCL GPIO22 (sama dengan LCD 16x2 sebelumnya).
- GPIO34/35 hanya input.
- ADC ESP32 maksimal 3.3V.
- Gunakan voltage divider jika output sensor 5V.
- Fan tidak boleh langsung ke pin ESP32.
- Buzzer dipakai di GPIO27.
- Relay fan dipakai di GPIO26.
- Tiga relay baru dipakai untuk indikator lampu hijau, kuning, merah.
- Fan DC 2 kabel dikontrol relay sebagai saklar putus/sambung jalur positif.
- Logika PPM aktif sekarang:
  - `0-49` -> lampu hijau ON.
  - `50-149` -> lampu kuning ON, buzzer flashing terus, fan OFF.
  - `>=150` -> lampu merah ON, buzzer bunyi terus, fan ON.
- Logika dust aktif sekarang: `dustRaw >= 800` -> lampu kuning ON dan fan ON.
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
Wifi : ON / OFF
IP address
MQ135 ADC
MQ135 voltage
PPM : x | normal / warning / danger | Wifi : ON / OFF
Dust ADC
Dust voltage
Dust density
Dust status
Fan status
Buzzer status
Status relay hijau / kuning / merah
ThingSpeak HTTP response
Free heap
```

### Tampilan LCD 20x4 (2004 I2C)

Firmware terbaru menggunakan **LCD 20x4 I2C** dengan address `0x27`. Layout display 4 baris:

```text
Baris 1: PPM:xxx | status
Baris 2: Dust:xx.x mg/m3
Baris 3: LED:GYR Fan:ON/OFF
Baris 4: WiFi:ON/OFF
```

Detail setiap baris:

**Baris 1 - Informasi PPM dan Status Udara:**
```text
PPM:123 | warning
```
- `PPM:xxx`: Nilai PPM estimasi dari sensor MQ-135 (0-999)
- `status`: Status udara (`normal` / `warning` / `danger`)

**Baris 2 - Kepadatan Debu:**
```text
Dust:45.2 mg/m3
```
- `Dust:xx.x`: Kepadatan debu dalam mg/m³ dari sensor GP2Y1010AU0F
- Range: 0.0 - 999.9 mg/m³

**Baris 3 - Status Relay Indikator dan Fan:**
```text
LED:G-R Fan:ON
```
- `LED:`: Status 3 relay indicator lampu
  - `G` = Green relay ON (PPM 0-49, kondisi normal)
  - `Y` = Yellow relay ON (PPM 50-149 warning, atau dust ≥800)
  - `R` = Red relay ON (PPM ≥150 danger)
  - `-` = Relay OFF
- `Fan:`: Status exhaust fan (`ON` atau `OFF`)

**Baris 4 - Status WiFi:**
```text
WiFi:ON
```
- `WiFi:ON`: Terhubung ke WiFi dan ThingSpeak
- `WiFi:OFF`: Tidak terhubung (sistem tetap berjalan lokal)

**Splash Screen Startup (2 detik):**
```text
    Sugeng IOT
  Air Quality Mon
MQ135 + GP2Y1010
    Starting...
```

### Perubahan dari LCD 16x2 ke 20x4

Keuntungan upgrade LCD 20x4:

1. **Informasi lebih lengkap** - 4 baris vs 2 baris
2. **Dust density tampil** - Sebelumnya tidak ada di display
3. **Status relay tampil** - Visual feedback LED indicator (G/Y/R)
4. **Status fan tampil** - Monitor exhaust fan real-time
5. **Layout lebih rapi** - 20 karakter per baris vs 16 karakter

Wiring tetap sama:
- SDA: GPIO21
- SCL: GPIO22
- I2C address: 0x27 (atau 0x3F, sesuai modul)
- Power: 5V dari power supply eksternal (bukan dari pin 3.3V ESP32)

---

### Format Output Serial Monitor

Firmware terbaru menampilkan output serial yang lengkap setiap 200ms:

**Contoh output normal (PPM < 50, dust < 800):**
```text
Nilai DO MQ-135: 1 | ADC AO: 1234 | AO Volt: 0.991V | PPM : 297 | normal | Wifi : ON | Dust ADC: 650 | Dust Volt: 0.522V | Dust Density: 0.000 mg/m3 | Dust Status: normal | Fan: OFF | Buzzer: OFF | Relay Hijau: ON | Relay Kuning: OFF | Relay Merah: OFF
```

**Contoh output warning (PPM 50-149):**
```text
Nilai DO MQ-135: 0 | ADC AO: 1850 | AO Volt: 1.487V | PPM : 446 | warning | Wifi : ON | Dust ADC: 720 | Dust Volt: 0.579V | Dust Density: 0.000 mg/m3 | Dust Status: normal | Fan: OFF | Buzzer: FLASHING | Relay Hijau: OFF | Relay Kuning: ON | Relay Merah: OFF
```

**Contoh output danger (PPM ≥ 150):**
```text
Nilai DO MQ-135: 0 | ADC AO: 2200 | AO Volt: 1.769V | PPM : 531 | danger | Wifi : ON | Dust ADC: 900 | Dust Volt: 0.724V | Dust Density: 24.800 mg/m3 | Dust Status: warning | Fan: ON | Buzzer: CONTINUOUS | Relay Hijau: OFF | Relay Kuning: OFF | Relay Merah: ON
```

**Penjelasan field output:**

| Field | Keterangan | Range |
|---|---|---|
| `Nilai DO MQ-135` | Digital output sensor MQ-135 | 0 (gas terdeteksi) / 1 (normal) |
| `ADC AO` | Nilai ADC 12-bit analog output MQ-135 | 0-4095 |
| `AO Volt` | Tegangan analog MQ-135 dalam Volt | 0.000-3.300V |
| `PPM` | Estimasi PPM gas polutan | 0-999 |
| `status` | Status kualitas udara | `normal` / `warning` / `danger` |
| `Wifi` | Status koneksi WiFi | `ON` / `OFF` |
| `Dust ADC` | Nilai ADC debu dari GP2Y1010AU0F | 0-4095 |
| `Dust Volt` | Tegangan output sensor debu | 0.000-3.300V |
| `Dust Density` | Kepadatan debu dalam mg/m³ | 0.000-999.999 |
| `Dust Status` | Status debu | `normal` (< 800) / `warning` (≥ 800) |
| `Fan` | Status exhaust fan | `ON` / `OFF` |
| `Buzzer` | Pola buzzer aktif | `OFF` / `FLASHING` / `CONTINUOUS` |
| `Relay Hijau` | Status relay lampu hijau (normal) | `ON` / `OFF` |
| `Relay Kuning` | Status relay lampu kuning (warning) | `ON` / `OFF` |
| `Relay Merah` | Status relay lampu merah (danger) | `ON` / `OFF` |

**Output startup serial:**
```text
Sugeng IOT start
Wiring test awal aktif
MQ-135 DO -> GPIO15
MQ-135 AO -> GPIO34
LCD SDA -> GPIO21
LCD SCL -> GPIO22
Buzzer -> GPIO27
Relay fan -> GPIO26
Relay hijau -> GPIO14
Relay kuning -> GPIO12
Relay merah -> GPIO13
PPM 0-49 hijau | 50-149 kuning + buzzer ritme | >=150 fan + buzzer panjang
Dust >= 800 paksa kuning + fan ON
AO dibaca dengan ADC 12-bit + averaging
ThingSpeak field1=ADC field2=Volt field3=PPM field4=Gas field5=DustDensity field6=Fan
Test debu: Vo -> GPIO35 | LED -> GPIO25
Connecting WiFi........
WiFi connected: 192.168.1.100
```

**Output ThingSpeak:**
```text
ThingSpeak response: 200
123456
```
- `200` = HTTP OK, data berhasil dikirim
- `123456` = Entry ID dari ThingSpeak

---

## 11. Tahap 9 — Debugging Sensor

### MQ-135

- Warm-up minimal 5–10 menit.
- Catat baseline udara bersih.
- Uji dengan asap ringan.
- Gunakan rule status aktif di sketch:
  - `PPM 0-49` = `normal`.
  - `PPM 50-149` = `warning`.
  - `PPM >= 150` = `danger`.
- Saat `warning`, lampu kuning ON dan buzzer flashing terus.
- Saat `danger`, lampu merah ON, fan ON, buzzer bunyi terus.

### GP2Y1010AU0F

- Pastikan timing LED pulse benar.
- Gunakan kapasitor 220µF jika dibutuhkan.
- Catat ADC udara bersih dan kondisi berdebu.
- **Sensor debu sekarang menggunakan double filtering:**
  - **Averaging 8 samples** saat pembacaan ADC untuk kurangi noise hardware
  - **Moving average 5 readings** untuk smoothing nilai akhir
- **Threshold dust:** 3.0 mg/m³
- Saat dust ≥ 3.0 mg/m³: status BERDEBU, lampu kuning ON, fan ON
- Saat dust < 3.0 mg/m³: status AMAN, fan OFF (jika gas juga normal)
- **Total waktu sampling dust:** ~80ms (8 samples × 10ms timing)
- **Moving average response time:** ~1 detik (5 readings × 200ms loop)

#### Kenapa Dust Perlu Averaging?

**Sensor GP2Y1010AU0F sangat sensitif terhadap noise:**
- Sensor optik lebih noise daripada sensor gas (MQ-135)
- ADC ESP32 12-bit (~0.8mV per step) sangat sensitif
- Rumus density mengamplifikasi noise: 15mV = 3 mg/m³
- Tanpa filtering, fan akan nyala-mati-nyala (flickering)

**Solusi implementasi:**
```cpp
// 1. Hardware averaging (8 samples)
int readDustAnalogAverage() {
  long total = 0;
  for (int i = 0; i < 8; i++) {
    // timing sesuai datasheet
    total += analogRead(DUST_VO_PIN);
  }
  return total / 8;
}

// 2. Software moving average (5 readings)
float getFilteredDustDensity(float newValue) {
  // simpan 5 pembacaan terakhir
  // return rata-rata
}
```

**Hasil:**
- Pembacaan lebih stabil dan smooth
- Fan tidak flickering
- Response masih cukup cepat (~1 detik delay)

#### Wiring GP2Y1010AU0F - PENTING!

**Pin wajib disambung (6 pin total):**
- **VCC (merah)** → 5V ESP32
- **GND / S-GND (hitam)** → GND ESP32
- **Vo (kuning)** → GPIO35 (output analog)
- **LED (putih)** → GPIO25 (kontroler pulse)
- **V-LED (hijau)** → 5V ← **WAJIB untuk LED internal**
- **LED-GND (biru)** → GND ← **WAJIB untuk LED internal**

**Catatan V-LED dan LED-GND:**
- LED infrared internal **butuh power terpisah** dari V-LED
- Jika tidak disambung, sensor **tidak akan bekerja**
- Beberapa modul breakout sudah gabung V-LED+VCC internal
- Sensor bare Sharp original **wajib** sambung semua 6 pin
- Test dengan kamera HP: LED harus terlihat berkedip di kamera

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
field4 = Gas Status (0 normal, 1 warning/danger)
field5 = Dust Density (mg/m3)
field6 = Fan Status (0 OFF, 1 ON)
```

Catatan penting:

- `Field 5` di sketch aktif sekarang isi `Dust Density (mg/m3)`.
- Estimasi density dihitung dari [`estimateDustDensity()`](arduino-ide/sketches/SugengIOT/SugengIOT.ino:44).
- Rumus awal: `(dustVoltage - 0.6) / 0.005` lalu nilai negatif jadi `0`.
- Threshold debu di sketch aktif = `800` ADC.
- Field gas sekarang bernilai `1` untuk `warning` atau `danger`, dan `0` untuk `normal`.
- Log serial sekarang menampilkan format `PPM : x | status | Wifi : ON / OFF` plus relay hijau/kuning/merah.
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

## 13. Tahap 11 — Troubleshooting LCD 20x4

### LCD tidak menyala / blank

**Penyebab umum:**
1. **Power tidak cukup**
   - Pastikan LCD dapat power dari supply eksternal 5V (minimal 2A)
   - Jangan ambil power dari pin 3.3V ESP32 (LCD butuh ~60-80mA)
   
2. **I2C address salah**
   - Address umum: `0x27` atau `0x3F`
   - Scan I2C untuk cek address:
   ```cpp
   Wire.begin(21, 22);
   Wire.beginTransmission(0x27);
   byte error = Wire.endTransmission();
   if (error == 0) Serial.println("Found 0x27");
   ```

3. **Kontras terlalu rendah**
   - Putar trimpot di belakang LCD module
   - Putar searah jarum jam untuk tingkatkan kontras

4. **Wiring salah**
   - SDA harus ke GPIO21
   - SCL harus ke GPIO22
   - GND LCD harus sama dengan GND ESP32

### LCD tampil tapi karakter acak

**Penyebab:**
- I2C komunikasi noise/error
- Kabel I2C terlalu panjang (max 1 meter untuk 400kHz)
- Tidak ada pull-up resistor (biasanya sudah ada di module)

**Solusi:**
- Pakai kabel lebih pendek
- Tambah kapasitor 100nF antara VCC-GND LCD
- Cek ground tidak floating

### LCD hanya tampil baris pertama

**Penyebab:**
- Init LCD belum benar
- Firmware masih pakai LCD 16x2 initialization

**Solusi:**
- Pastikan init di firmware: `LiquidCrystal_I2C lcd(0x27, 20, 4);`
- Bukan: `LiquidCrystal_I2C lcd(0x27, 16, 2);`

### Teks terpotong / overflow

**Penyebab:**
- String terlalu panjang untuk 20 karakter

**Contoh di firmware:**
```cpp
// SALAH - bisa overflow
lcd.print("PPM:1234 | warning udara");  // 26 karakter!

// BENAR - sesuai layout
lcd.print("PPM:1234 | warning");  // 19 karakter, aman
```

### Backlight tidak menyala

**Penyebab:**
- Jumper backlight di module tidak terpasang
- Power backlight kurang

**Solusi:**
- Cek jumper di belakang module LCD (biasanya ada solder pad)
- Pastikan VCC LCD 5V, bukan 3.3V

### Tips Optimasi Display

1. **Update LCD hanya saat berubah**
   ```cpp
   // Lebih efisien daripada lcd.clear() setiap loop
   static float lastPPM = -1;
   if (ppmValue != lastPPM) {
     lcd.setCursor(4, 0);
     lcd.print("    ");  // Clear old value
     lcd.setCursor(4, 0);
     lcd.print(ppmValue, 0);
     lastPPM = ppmValue;
   }
   ```

2. **Hindari lcd.clear() di loop**
   - Firmware saat ini pakai `lcd.clear()` di `updateDisplay()`
   - Aman karena dipanggil setiap 200ms (5Hz)
   - Jika flicker muncul, pakai update parsial seperti di atas

3. **Format angka konsisten**
   ```cpp
   // PPM tanpa desimal
   lcd.print(ppmValue, 0);  // 123
   
   // Dust dengan 1 desimal
   lcd.print(dustDensity, 1);  // 45.2
   ```

---

## 14. Tahap 12 — Deployment

Checklist:

- ESP32 menyala stabil.
- LCD tampil.
- Sensor MQ-135 terbaca.
- Sensor GP2Y1010AU0F terbaca.
- WiFi connect.
- ThingSpeak menerima data.
- `PPM 0-49` menyalakan lampu hijau.
- `PPM 50-149` menyalakan lampu kuning, buzzer flashing, fan OFF.
- `PPM >= 150` menyalakan lampu merah, buzzer bunyi terus, fan ON.
- `dustRaw >= 800` menyalakan lampu kuning dan fan ON.
- LCD dan serial menampilkan `Wifi : ON` atau `Wifi : OFF`.
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
Threshold PPM 50
Threshold PPM 150
Threshold dust 800
Status relay hijau
Status relay kuning
Status relay merah
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
"arduino-ide/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli" upload -p /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 arduino-ide/sketches/SugengIOT
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
