# Ten plik został wygenerowany automatycznie ze sprawozdania w pliku .tex i może zawierać błędy
---
author:
- Mikołaj Nowak 151813, Wojciech Kot 151879
title: |
  Przetwarzanie Rozproszone\
  Projekt -- Zabawa w zabijanie
---

# Opis problemu

W pewnej uczelni studenci zaczęli się bawić w zabójców i ofiary. W
pierwszym cyklu student losuje rolę, następnie dobiera się w parę z
ofiarą. Zabójca ubiega się o jeden z nierozróżnialnych P pistoletów na
farbę. Następnie zabójca próbuje \"zabić\" ofiarę, a ofiara próbuje go
uniknąć. Jednoznacznie należy rozstrzygnąć, jak skończył się dany cykl.
Po X cyklach należy wyłonić zwycięzcę, po czym zabawa zaczyna się od
nowa.

# Wiadomości

Procesy mogą wysyłać do siebie wiadomości zawierające następujące
informacje:

1.  Timestamp

2.  ID procesu nazawcy

3.  Wiadomość (INTEGER)

4.  Etykieta

    -   REQ: Request wejścia do sekcji krytycznej

    -   ACC: Accept (zezwolenie) na wejście do sekcji krytycznej

    -   FIN: Zakończenie jakiegoś etapu

    -   NONE: Pusta etykieta do zwykłej komunikacji np. wyłaniania
        zwycięzcy

# Struktury i zmienne

Parametry:

-   N - liczba procesów

-   P - liczba pistoletów dostępnych dla zabójców

-   C - liczba cykli

-   A - liczba kul (amunicji), które posiada każdy zabójca.

Zmienne współdzielone:

-   Waiting - ID procesu czekającego na parowanie, początkowo -1
    (zmienna używana

Zmienne prywatne dla każdego procesu:

-   WaitQueue - Kolejka procesów oczekujących na ACC, początkowo pusta

-   AckNum - liczba otrzymanych potwierdzeń ACK, początkowo 0

-   Partner - ID partnera

-   Wynik - różnica liczby wygranych i przegranych rund

# Stany

Początkowym stanem procesu jest REST

-   REST - nie ubiega się o dostęp.

-   WAIT - czeka na dostęp do sekcji krytycznej

-   INSECTION - w sekcji krytycznej

# Szkic algorytmu

Proces początkowo jest w stanie REST, a gdy ubiega się o dostęp do
sekcji krytycznej w celu umieszczenia go w kolejce do przydzielenia pary
zmienia go na WAIT. Wysyła REQ do wszytskich innych.\
Reakcja na wiadomości:

-   REQ: wstaw żądanie do kolejki

-   ACC: Zwiększ AckNum o 1

Następnie przydziel sobie partnera (INSECTION) oraz rolę sobie i
partnerowi.\
Wróć do stanu REST.\
Wyślij ACC do reszty.\
Rozpocznij rundę.\
Zmień stan na WAIT, kiedy ubiega się o pisolet.\
Wejdź do sekcji krytycznej (INSECTION) biorąc pistolet i próbując zabić
przeciwnika.\
Powtórz turę określoną liczbę razy.\
Przekaż wiadomość FIN.

# Dobieranie procesów w pary

Na samym początku zabawy procesy muszą się dobrać w pary (zabójca +
ofiara).

## Tu była wersja nieoptymalna, ale nie zdała testu ;)

## Wersja uwzględniająca znajomość liczby procesów N

Każdy proces wysyła REQUEST. Celem jest stworzenie N kolejek o długości
N, gdzie N to oczywiście liczba procesów.\
Kiedy kolejka osiągnie długość N, to znaczy, że dany proces zna dokładne
uporządkowanie procesów, czyli kojeność, w której procesy wchodziłyby do
sekcji krytycznej. Proces odnajduje swoją pozycję. Jeśli jest
nieparzysta, to wysyła do procesu o 1 wcześniej w kolejce, że będzie
jego parą i losuje role dla obu z pary. Proces parzysty czeka na
wiadomość od nieparzystego.\
Ponieważ procesy nie chcą wchodzić do sekcji krytycznej, to po
znalezieniu się w kolejce usuwana jest zawartość RequestQueue.

``` {.numberLines numbers="left" xleftmargin="5mm"}
function PAROWANIE2():
    wyślij (REQ, time) do wszystkich poza mną
    odbieraj żądania innych dopóki długość WaitQueue jest mniejsza od N
    znajdź siebie w WaitQueue
    if(moja pozycja nieparzysta)
        Wylosuj sobie rolę
        Przekaż przeciwną partnerowi (z pocyji -1)
    else
        czekaj na rolę i na parę
```

# Proces zabójcy

N - ilość procesów\
P - ilość pistoletów (zasobu)\
A - ilość amunicji każdego zabójcy (prób które mogą podjąć)\
state - zmienna przechowująca stan RELEASED, REQUESTED, HELD, FINISHED
oznaczająca to czy obecny zabójca posiada broń, czy nie, albo czy już
zakończył grę ze swojej strony.\
C - ilość rund w cyklu\
Kiedy proces zabójcy jest gotów do podjęcia próby zabójstwa zaczyna
ubiegać się o dostęp do pistoletu (sekcji krytycznej) algorytmem
Ricart'a i Agrawal'a.

Proces zabójcy:

``` {.numberLines numbers="left" xleftmargin="5mm"}
Proces sprawdza własną ilość amunicji (początkowo A)
    Jeśli jest większa od 0 - przechodzi dalej, 
    w przeciwnym razie zmienia stan na FINISHED i czeka na 
    koniec cyklu (odsyłając odpowiedzi na żądania dostępu do broni).

Następnie, jeśli ma jeszcze amunicje, ubiega się o dostęp do broni:
    Proces zmienia swój stan na REQUESTED
    Proces aktualizuje swój zegar i wysyła żądanie do wszystkich 
    pozostałych procesów, zawierając w nim swój znacznik czasu
    Proces czeka na odpowiedź od pozostałych procesów.

    Po uzyskaniu N-P odpowiedzi (od każdej ofiary oraz każdego zabójcy który
    nie ubiega się obecnie o broń) proces może przejść dalej - zabrać broń
    i podjąć się próby zabójstwa.

Po uzyskaniu odpowiedzi od ofiary proces zabójcy przechodzi dalej:
    W przypadku pomyślnego zabójstwa proces zabójcy zmienia stan na FINISHED
    i wysyła odpowiedzi na zakolejkowane żądania dostępu do broni.

    W przypadku niepowodzenia, proces zmienia stan na RELEASED, wysyła
    odpowiedzi na zakolejkowane żądania dostępu do broni i powtarza swoje
    zadanie (od kroku sprawdzenia dostępnej amunicji)
```

Próba zabójstwa (przykładowa)

``` {.numberLines numbers="left" xleftmargin="5mm"}
Proces zmniejsza swój licznik amunicji o 1
Proces wysyła do swojej ofiary informacje o podjętej próbie z
wylosowaną wartością z zakresu 1-20
Ofiara losuje własną liczbę od 1-20 określającą trudność tej próby
    Jeśli liczba zabójcy jest większa niż liczba ofiary -> ofiara przesyła
    informacje o powodzeniu i zmienia własny stan z ALIVE na DEAD

    W przeciwnym wypadku ofiara odsyła wiadomość o niepowodzeniu.

Proces zabójcy odbiera wiadomość i przechodzi dalej w swojej implementacji
```

# Wykrywanie stanu końcowego

Kiedy proces zabójcy zmienia swój stan na Finished wysyła do kolejnego
(w pierścieniu) wiadomość FIN zawierającą jego numer procesu oraz dwie
zmienne ilość udanych zabójstw oraz ilość nieudanych zabójstw
(początkowo 0,0). Każdy proces który jest już w stanie FINISHED
przekazuje wiadomości FIN dalej z tym samym numerem procesu który ją
wysłał, zwiększając odpowiedni licznik (udanych lub nieudanych) zabójstw
o 1. Gdy proces otrzyma wiadomość FIN z własnym numerem jako nadawcy
również zwiększa odpowiedni licznik ale nie przekazuje jej dalej,
zamiast tego zwiększa licznik rund oraz zapisuje w tablicy o długości C
wynik odpowiedniej rundy. W momencie gdy licznik rund osiągnie C należy
podsumować cykl, a więc każdy proces wykonuje print() wyświetlając
ogólny wynik cyklu

Wygrana Zabójców gdy więcej rund zostało wygranych przez zabójców
Wygrana ofiar gdy większość rund została przegrana przez zabójców Remis,
gdy obie wielkości będą równe (np. gdy wszystkie rundy zakończyły się
remisami)

Po podsumowaniu gra zaczyna się ponownie, procesy wracają do dobierania
się w pary i rozpoczynają nowy cykl C rund.

# Program główny

``` {.numberLines numbers="left" xleftmargin="5mm"}

Powtarzaj ciągle (While true:)
    Znajdź swoją parę i wylosujcie role w tym cyklu
    Rozpocznij cykl gry (powtarzaj C razy):
        Jeśli jesteś samotny, zachowuj się jak ofiara.
        Jeśli jesteś ofiarą czekaj na sygnały i odpowiadaj na nie odpowiednio
        Jeśli jesteś zabójcą:
            Podejmij próbę zabicia swojej ofiary.
            Zbierz wynik tej rundy i zapisz
    Po skończeniu cyklu, jeśli byłeś zabójcą podsumuj cykl
    (podlicz wygrane i przegrane i wyświetl wynik)
    Ponów grę w nieskończoność.
```

Należy zauważyć że przy nieparzystej ilości graczy, jeden z procesów
zostanie zablokowany na etapie parowania. Natomiast nie jest to problem,
gdyż wystarczy żeby zachowywał się jak ofiara dla komunikatów końca i
czekał na kolejną fazę parowania -\> ma wtedy zapewnione miejsce w
pierwszej parze nowego cyklu.
