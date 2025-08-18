# Detekcja kart UNO na obrazie

Projekt zrealizowany w ramach kursu **Cyfrowe Metody Przetwarzania Obrazu (CPO)**. Celem było stworzenie algorytmu detekcji i rozpoznania kart UNO na obrazach zawierających różne zakłócenia (rozmycie, gradient, sól i pieprz), z wyznaczeniem środka kart specjalnych oraz sumowaniem wartości liczbowych pozostałych kart.

## Funkcjonalności

- Wczytanie obrazu ze ścieżki podanej przez użytkownika
- Przetwarzanie wstępne: median blur → konwersja do szarości → Gaussian blur → adaptacyjne progowanie → morfologia (dylacja, erozja)
- Segmentacja kart na podstawie konturów zewnętrznych
- Wyznaczenie ROI dla detekcji kształtu oraz koloru
- Detekcja koloru karty (red, green, blue, yellow)
- Detekcja kształtu przy użyciu:
  - liczby okręgów (HoughCircles)
  - kolistości figury (z konturu)
  - pola powierzchni konturu
- Klasyfikacja znaków: `2`, `3`, `4`, `reverse`, `stop`
- Obliczanie sumy kart liczbowych danego koloru
- Wypisywanie współrzędnych środka karty dla kart specjalnych (`stop`, `reverse`)
- Obsługa 4 scen: bez zakłóceń, z blurem, gradientem oraz solą i pieprzem (łącznie 16 obrazów)

## Technologie

- C++
- OpenCV
