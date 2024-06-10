## Czym jest projekt i dlaczego powstał?
Projekt zegarka bazującego na Arduino Leonardo, wyświetlaczu LCD 16x2 (bez I²C) oraz zegara RTC. Projekt zrodził się z chęci stworzenia czegoś i braku zegarka w miejscu pracy.
## Co układ robi?
Program na bieżąco pokazuje informacje o godzinie (HH:MM:SS), dacie (DD.MM) oraz dniu tygodnia. W prawym dolnym rogu wyświetlacza czas umila Solaire z serii Dark Souls `\[T]/`. W programie zaimplementowano obsługę wyświetlania obecnie granego utworu na spotify, imitującego styl radia samochodowego. Odczytanie obecnie granego utworu jest moim innym osobistym projektem dostępnym [tutaj](https://github.com/Zymi1/Spotifyduino)! Program automatycznie, na podstawie wpisanej lokalizacji użytkownika, oblicza godzinę oraz minutę zachodu i na jej bazie ściemnia ekran. W minucie zachodu jasność obniżana jest stopniowo od maksymalnej do minimalnej. Zaimplementowano obsługę programu [deej](https://github.com/omriharel/deej).
## Problemy
W projekcie używam zegara bazującego na układzie DS1302, jednak nie polecam go. W ciągu dnia czas rozjeżdża się mniej lub bardziej, zależnie od temperatury otoczenia. Zaimplementowana niwelacja tego problemu nie jest idealna Mianowicie, pod koniec dnia czas cofany jest o 8 sekund.
## Wsparcie i plany na przyszlość
Projekt jest na bieżąco rozwijany, pierwszym rozwinięciem nad którym na bieżąco pracuję, jest drukowana w 3D obudowa. 
