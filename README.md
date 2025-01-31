# BlackJackOn-line
## Opis projektu:
Projekt realizuje rozgrywkę sieciową grę karcianą black jack. Posiada
system logowania, dołączania do gry kilku graczy jednocześnie (osobne
stoły, ale karty rozdawane w czasie rzeczywistym) oraz system
wpłat/wypłat.
## Instrukcja działania programu:
Ze względu na brak makefile pliki server_main.cpp i
game_functions.cpp należy skompilować razem.
Po uruchomieniu programu serwera, klient może się połączyć przy
użyciu adresu IP podawanego podczas uruchamiania programu ./program
[adres ipv4]. Klient musi się zalogować do systemu lub zarejestrować. W
terminalu należy wpisać r/l [login] [hasło].
## Po zalogowaniu się klient posiada funkcje takie jak:
- d [value] – wpłata środków na kotno
- w [value] – wypłata środków z konta
- b – aktualny stan konta
- r [login] – ilość wygranych danego gracza
- j [value] – dołączenie do gry za daną kwotę, jeżeli kwota postawiona
będzie większa od posiadanej należy wpisać poprawną wartość
- s – rozłącza klienta
### Komendy dostępne w grze:
- h – dobierz kartę
- s – zostań przy swoich kartach
