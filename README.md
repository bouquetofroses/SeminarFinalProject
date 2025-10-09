# Seminar Management System
**Final Project - Programming Fundamentals (CECS01)**

---

## Project Overview
```bash
โปรเจกต์นี้คือ ระบบจัดการผู้เข้าร่วมสัมมนา
พัฒนาด้วยภาษา C ระบบถูกออกแบบมาเพื่อรองรับผู้ใช้งานสองกลุ่มหลัก:
- ผู้ดูแลระบบ (Admin)
- ผู้เข้าร่วม (Participant)
โดยใช้ไฟล์ CSV (Seminar.csv) ในการจัดเก็บและจัดการข้อมูลอย่างมีประสิทธิภาพ
```
---
## File in Project
* `SeminarFinalProject.c`: โค้ดโปรแกรมหลัก
* `unit_test.c`: โค้ดสำหรับ Unit Test
* `e2e_test.c`: โค้ดสำหรับ End-to-End Test
* `test_helpers.c`: ฟังก์ชันช่วยเหลือที่ใช้ร่วมกันในไฟล์ Test
* `project.h`: Header File กลางของโปรเจกต์
* `Seminar.csv`: ไฟล์ฐานข้อมูลตัวอย่าง
### Compile
```bash
gcc -o program SeminarFinalProject.c unit_test.c e2e_test.c test_helpers.c -Wextra -Wall
```
### Run
```bash
.\program.exe
```
---

## 📁 main unit and e2e test ( กรณีฉุกเฉิน )
* `SeminarFinalProjectall.c`: โค้ดโปรแกรมหลัก + unit test + E2E test
* `howtocompile.txt` : วิธีคอมไพล์
### Compile
```bash
gcc SeminarFinalProjectall.c -o seminar
```
## Run
```bash
seminar.exe
./seminar
```
---

## ADMIN Features
- **Login system** (Username: 'admin', Password: '1234')
- **Add** new participants
- **Search** and view full participant info
- **Update/Edit** participant info
- **Delete** (mark as 'Inactive' instead of removing)
- **Display** all participants in table view
- Run **UNIT TEST**
- Run **E2E TEST**

---

## PARTICIPANT Features
- **Register** (Add new participant)
- **Display** participant list (limited info: ID, Name, Date, Status)
- **Search** participants
- **Update/Edit** own info (verify via Email + Phone)
- **Delete** own record (mark as 'Inactive')
- Run **UNIT TEST**
- Run **E2E TEST**

---
## Example run (ADMIN Features)
```bash
============================================
       WELCOME TO SEMINAR SYSTEM
============================================
Are you an admin? (y/n): y

Admin login required.
========================
|   USERNAME = admin   |
|   PASSWORD = 1234    |
========================
Username: admin
Password: 1234

Login successful.

===========================================================
                        ADMIN MENU
===========================================================
1. Add participant
2. Search participant (full info)
3. Update participant (admin)
4. Delete participant (set Inactive)
5. Display all participants
6. Run Unit Tests
7. Run E2E Tests
8. Logout to main
===========================================================
Choose option:
```
---
## Example run (PARTICIPLE Features)
```bash
============================================
       WELCOME TO SEMINAR SYSTEM
============================================
Are you an admin? (y/n): n

===========================================================
                     PARTICIPANT MENU
===========================================================
| 1  | Register (Add participant)                         |
| 2  | Show participants (ID, Name, RegDate, Status)      |
| 3  | Search participant (limited view)                  |
| 4  | Edit your info (verify email+phone)                |
| 5  | Delete your info (verify email+phone)              |
| 6  | Run Unit Tests                                     |
| 7  | Run E2E Tests                                      |
| 8  | Exit to main                                       |
===========================================================
Choose option:
```

