# Dokumentasi Arduino IDE untuk Proyek Bayu IoT Pet Feeder

## 1. Tujuan

Dokumentasi ini dipakai untuk membuat, build, upload, debugging, dan deploy proyek:

**Pet Feeder Otomatis Berbasis ESP32 DevKit V1 dengan Servo SG90 dan Buzzer**

Target perangkat:

- ESP32 DevKit V1
- Servo SG90
- Buzzer
- Tombol push button untuk trigger manual
- Adaptor 5V yang stabil

---

## 2. Gambaran Sistem

Sistem pet feeder ini berfungsi untuk:

- Menggerakkan servo [`Servo.write()`](bayu-iot/arduino-ide.md:18) untuk membuka wadah pakan.
- Membunyikan buzzer sebagai penanda waktu makan.
- Menjalankan pemberian pakan secara manual atau otomatis.
- Menampilkan log proses pada Serial Monitor.

Alur kerja dasar:

1. ESP32 menyala.
2. Servo bergerak ke posisi tertutup.
3. Sistem menunggu jadwal atau input tombol.
4. Buzzer berbunyi beberapa kali.
5. Servo membuka katup pakan.
6. Tunggu beberapa detik.
7. Servo menutup kembali katup pakan.
8. Sistem kembali standby.

---

## 3. Instalasi Software

### 3.1 Install Arduino IDE

Download dan install:

```text
https://www.arduino.cc/en/software
```

Gunakan Arduino IDE 2.x.

### 3.2 Tambahkan ESP32 Board Manager

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

### 3.3 Pilih Board ESP32

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

### 3.4 Install Library

Buka:

```text
Sketch > Include Library > Manage Libraries
```

Install library berikut:

```text
ESP32Servo
```

Library bawaan yang dipakai:

```text
Arduino.h
```

Opsional:

```text
WiFi
NTPClient
WiFiUdp
```

Library opsional dipakai jika nanti ingin menambahkan jadwal makan berbasis internet.

---

## 4. Driver USB ESP32

Cek chip USB pada board ESP32:

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

## 5. Struktur Sketch Arduino IDE

Rekomendasi folder:

```text
Bayu-IOT-PetFeeder/
├── Bayu-IOT-PetFeeder.ino
├── config.h
└── README.md
```

Isi [`config.h`](bayu-iot/arduino-ide.md:121) jika nanti ingin menambah WiFi atau konfigurasi jadwal:

```cpp
#pragma once

#define FEED_INTERVAL_HOURS 8
#define SERVO_OPEN_ANGLE 90
#define SERVO_CLOSE_ANGLE 0
#define BUZZER_BEEP_MS 200
```

Untuk versi dasar tanpa WiFi, semua konfigurasi juga bisa langsung ditulis di file sketch.

---

## 6. Rekomendasi Pin ESP32

| Perangkat | Pin ESP32 | Catatan |
|---|---:|---|
| Servo SG90 signal | GPIO18 | Gunakan supply 5V stabil |
| Buzzer | GPIO26 | Bisa aktif HIGH |
| Tombol manual feed | GPIO27 | Gunakan `INPUT_PULLUP` |

Catatan penting:

- Jangan mengambil daya servo SG90 langsung dari pin 3V3 ESP32.
- Servo SG90 lebih aman diberi supply 5V terpisah atau 5V board yang cukup kuat.
- Ground servo harus disatukan dengan ground ESP32.
- Jika servo bergerak liar atau ESP32 restart, biasanya suplai daya kurang stabil.

### 6.1 Wiring Dasar

```text
ESP32 DevKit V1
- 5V/VIN  -> VCC servo SG90
- GND     -> GND servo SG90
- GPIO18  -> signal servo SG90
- GPIO26  -> + buzzer
- GND     -> - buzzer
- GPIO27  -> salah satu kaki push button
- GND     -> kaki push button lainnya
```

Jika memakai adaptor 5V eksternal untuk servo:

```text
Adaptor 5V (+) -> VCC servo
Adaptor GND    -> GND servo
Adaptor GND    -> GND ESP32
GPIO18         -> signal servo
```

---

## 7. Template Alur Program

Urutan program:

1. [`Serial.begin(115200)`](bayu-iot/arduino-ide.md:166)
2. Inisialisasi pin buzzer dan tombol.
3. Attach servo menggunakan library [`ESP32Servo`](bayu-iot/arduino-ide.md:86).
4. Servo ke posisi tertutup.
5. Baca tombol atau cek timer.
6. Bunyikan buzzer sebelum pakan keluar.
7. Servo membuka wadah pakan.
8. Tunggu 1 sampai 3 detik.
9. Servo menutup kembali.
10. Cetak status ke Serial Monitor.
11. Ulangi loop.

---

## 8. Build / Verify

Klik:

```text
Sketch > Verify/Compile
```

Atau tombol centang.

Jika gagal compile:

- Pastikan board `ESP32 Dev Module`.
- Pastikan library `ESP32Servo` sudah terinstall.
- Pastikan nama fungsi dan kurung benar.
- Cek error di panel bawah Arduino IDE.

---

## 9. Upload ke ESP32

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

- Tekan tombol `BOOT` saat upload.
- Turunkan upload speed ke `115200`.
- Ganti kabel USB.
- Pastikan port benar.
- Tutup Serial Monitor sebelum upload.
- Cek driver USB.

---

## 10. Serial Monitor

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
Pet feeder starting...
Servo initialized
Feeding trigger detected
Buzzer ON
Servo open
Dispensing food...
Servo close
Feeding complete
Waiting next cycle
```

Log ini penting untuk memastikan alur [`loop()`](bayu-iot/arduino-ide.md:223) berjalan benar.

---

## 11. Debugging Hardware

### Servo SG90

Jika servo tidak bergerak:

- Cek kabel signal ke GPIO18.
- Cek VCC servo ke 5V.
- Cek ground servo dan ground ESP32 tersambung.
- Pastikan library `ESP32Servo` terpasang.
- Coba ubah sudut servo dari 0, 45, 90, sampai 180.

Jika servo bergetar terus:

- Supply daya kurang stabil.
- Gunakan adaptor 5V terpisah.
- Jangan ambil arus servo dari 3V3 ESP32.
- Pastikan mekanik penutup pakan tidak macet.

Jika ESP32 restart saat servo bergerak:

- Arus servo terlalu besar untuk supply saat ini.
- Pisahkan suplai servo dari ESP32.
- Satukan ground antara servo dan ESP32.

### Buzzer

Jika buzzer tidak bunyi:

- Cek polaritas buzzer.
- Cek pin GPIO26.
- Coba test `HIGH` dan `LOW` manual.
- Jika buzzer pasif, gunakan [`tone()`](bayu-iot/arduino-ide.md:257) atau PWM.

### Push Button

Jika tombol tidak terdeteksi:

- Pastikan satu kaki tombol ke GPIO27.
- Kaki lain ke GND.
- Gunakan mode `INPUT_PULLUP`.
- Logika tombol aktif biasanya bernilai `LOW` saat ditekan.

---

## 12. Logika Pemberian Pakan

Metode sederhana yang disarankan:

- Manual feed dari tombol.
- Otomatis feed berdasarkan interval [`millis()`](bayu-iot/arduino-ide.md:276).
- Buzzer bunyi sebelum servo membuka.
- Servo membuka beberapa derajat sesuai desain wadah pakan.

Contoh parameter awal:

```text
Sudut tutup servo  = 0
Sudut buka servo   = 90
Waktu buka servo   = 1500 ms
Jumlah beep buzzer = 3 kali
Interval feed      = 8 jam
```

Nilai ini harus disesuaikan dengan bentuk mekanik pet feeder.

---

## 13. Contoh Sketch Dasar

Contoh fitur minimum:

- Tombol manual untuk memberi pakan.
- Buzzer berbunyi 3 kali.
- Servo membuka lalu menutup.
- Status tampil di Serial Monitor.

Template logika sketch:

```cpp
#include <ESP32Servo.h>

const int SERVO_PIN = 18;
const int BUZZER_PIN = 26;
const int BUTTON_PIN = 27;

Servo feederServo;

void beepBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
  }
}

void feedNow() {
  Serial.println("Feeding...");
  beepBuzzer(3);
  feederServo.write(90);
  delay(1500);
  feederServo.write(0);
  Serial.println("Done");
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  feederServo.attach(SERVO_PIN);
  feederServo.write(0);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    feedNow();
    delay(1000);
  }
}
```

---

## 14. Pengujian yang Perlu Dilakukan

### Test unit hardware

- ESP32 menyala normal.
- Serial Monitor berjalan.
- Servo bergerak sesuai sudut.
- Buzzer bunyi.
- Tombol manual terdeteksi.

### Test integrasi

| Skenario | Ekspektasi |
|---|---|
| Tombol ditekan | Buzzer bunyi lalu servo membuka |
| Servo selesai buka | Servo kembali menutup |
| Tombol ditekan berulang | Sistem tetap stabil |
| ESP32 reset | Servo kembali ke posisi tutup |

### Test durasi

Jalankan alat minimal:

- 10 menit untuk test awal.
- 1 jam untuk melihat stabilitas servo.
- Beberapa siklus makan untuk uji mekanik feeder.

Catat:

- Apakah servo macet.
- Apakah pakan keluar lancar.
- Apakah buzzer selalu aktif.
- Apakah ESP32 restart saat servo aktif.

---

## 15. Deployment Prototype

Sebelum dipasang:

- Pastikan wadah pakan tidak menghambat gerak servo.
- Pastikan SG90 tidak menahan beban berlebih terus-menerus.
- Gunakan adaptor 5V yang cukup kuat.
- Rapikan kabel agar tidak tertarik saat servo bergerak.
- Letakkan buzzer di posisi yang masih terdengar.
- Pastikan mekanisme tutup feeder tidak mudah macet karena butiran pakan.

---

## 16. Pengembangan Lanjutan

Fitur berikut bisa ditambahkan nanti:

- Jadwal makan otomatis berbasis RTC atau NTP.
- Kontrol dari HP melalui WiFi.
- Monitoring jumlah pakan.
- Sensor load cell untuk berat pakan.
- LCD atau OLED untuk status lokal.
- Integrasi Telegram atau Blynk.

---

## 17. Checklist Akhir Arduino IDE

- [ ] Arduino IDE terinstall.
- [ ] ESP32 board package terinstall.
- [ ] Driver USB terinstall.
- [ ] Board `ESP32 Dev Module` dipilih.
- [ ] Port ESP32 terbaca.
- [ ] Library `ESP32Servo` terinstall.
- [ ] Sketch berhasil Verify.
- [ ] Upload berhasil.
- [ ] Serial Monitor `115200` aktif.
- [ ] Servo SG90 bergerak normal.
- [ ] Buzzer berbunyi.
- [ ] Tombol manual berfungsi.
- [ ] Mekanisme pakan membuka dan menutup dengan baik.
- [ ] Supply daya servo stabil.
- [ ] Prototype aman untuk dipakai.
