# ESP32 + DHT11

วัดอุณหภูมิและความชื้นด้วยเซนเซอร์ DHT11 บน ESP32 แสดงค่าใน Serial Monitor และเปิดดูผ่านหน้าเว็บจากมือถือหรือคอมในวง WiFi เดียวกัน

## อุปกรณ์

- ESP32 (Board: **ESP32 Dev Module**)
- เซนเซอร์ DHT11

| DHT11 | ESP32 |
| --- | --- |
| VCC | 3V3 |
| DATA | GPIO 4 |
| GND | GND |

ถ้าเป็น DHT11 ตัวเปล่า 4 ขา (ไม่ใช่โมดูล) ให้ใส่ตัวต้านทาน 10k ระหว่าง DATA กับ 3V3

## ติดตั้ง

1. Arduino IDE → Library Manager ลง **DHT sensor library** (Adafruit) และ **Adafruit Unified Sensor**
2. คัดลอก `secrets.example.h` เป็น `secrets.h` แล้วใส่ชื่อและรหัส WiFi
3. Upload แล้วเปิด Serial Monitor ที่ 115200 จะเห็นค่าทุก 2 วินาทีและ IP ของบอร์ด

## ใช้งาน

- `http://<IP ของบอร์ด>/` หน้าเว็บแสดงอุณหภูมิและความชื้น รีเฟรชเองทุก 5 วินาที
- `http://<IP ของบอร์ด>/api` ข้อมูลแบบ JSON เช่น `{"ok":true,"temperature":28.5,"humidity":61,"uptime_s":120}`

ต่อ WiFi ไม่ได้ก็ยังอ่านค่าใน Serial Monitor ได้ บอร์ดจะลองต่อ WiFi ใหม่ทุก 30 วินาที

`secrets.h` อยู่ใน `.gitignore` รหัส WiFi จะไม่ขึ้น GitHub
