# Utánfutó Kölcsönző Rendszer (Trailo)

Asztali alkalmazás utánfutó kölcsönző cég számára, amely lehetővé teszi az utánfutók foglalását, kezelését és adminisztrációját.

## Technológiák

- **Nyelv:** C++17
- **Framework:** Qt 6.6.2
- **Adatbázis:** SQLite + Qt SQL
- **Build:** CMake
- **Verziókezelés:** Git

## Telepítés

### Követelmények
- Qt 6.5+ (Qt Widgets, Qt SQL, Qt Charts)
- CMake 3.16+
- C++17 kompatibilis compiler (MinGW/MSVC/GCC)

### Build lépések
```bash
git clone https://github.com/your-username/utanfuto-kolcsonzo.git
cd utanfuto-kolcsonzo
mkdir build && cd build
cmake ..
cmake --build .
```

## Projekt Állapot

### Elkészült (1-2. alkalom)
- [x] Technológia választás
- [x] Frontend PoC (bejelentkezés, főoldal, profil)
- [x] Projekt struktúra
- [x] Adatbázis PoC (Qt SQL + SQLite)
- [x] Model osztályok (User, Trailer, Booking)

### Folyamatban (3. alkalom)
- [ ] Utánfutók megtekintése (vendég)
- [ ] Foglaltság megtekintése
- [ ] Foglalás létrehozása

### Tervezett (4-5. alkalom)
- Foglalások kezelése
- Felhasználói adatok módosítása
- Admin funkciók
- Statisztika grafikon

### Saját funkciók
- Utánfutók nyomonkövetése
- Dinamikus szűrés
- Duplafaktoros bejelentkezés

## 3. Csapat

- Baksa Áron
- Nádasdi Milán
- Tóth Ármin
