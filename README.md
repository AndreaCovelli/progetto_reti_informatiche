# Trivia Quiz Multiplayer

![Immmagine](project_dependencies.png)

### Descrizione
Questo progetto implementa un gioco di quiz multiplayer client-server in C. Il sistema permette a più giocatori di partecipare contemporaneamente a quiz su diversi temi (Sport e Geografia), mantenendo punteggi e classifiche separate per ogni tema.

## Caratteristiche Principali

- Supporto multiplayer con gestione concorrente dei client
- Due temi di quiz disponibili (Sport e Geografia)
- 5 domande casuali per ogni sessione di quiz
- Sistema di punteggio (1 punto per risposta corretta, 0 per risposta sbagliata)
- Classifica in tempo reale
- Possibilità di visualizzare i punteggi durante il gioco
- Gestione nickname univoci per i giocatori
- Tracciamento dei quiz completati per ogni giocatore

## Requisiti di Sistema

- Compilatore: GCC v.13
- Make

## Installazione
```bash
# Clone the repository
git clone https://github.com/AndreaCovelli/progetto_reti.git

# Navigate to project directory
cd trivia-quiz-multiplayer

# Compile the project
make
```

Per la versione di debug:
```bash
make debug
```

### Struttura dei File Quiz
I file dei quiz devono essere posizionati nella cartella `res/` con la seguente struttura:
- `res/sport_quiz.txt`
- `res/geography_quiz.txt`

Formato del file:
```
Nome del tema
Domanda 1|Risposta 1
Domanda 2|Risposta 2
...
```

## Utilizzo
### Avvio del Server
```bash
./server <porta>
```

### Avvio del Client
```bash
./client <indirizzo IP> <porta>
```
### Comandi Disponibili Durante il Quiz
- `show score`: Visualizza la classifica in tempo reale
- `endquiz`: Abbandona il quiz

## Funzionalità Dettagliate
### Server
- Gestione concorrente dei client tramite I/O multiplexing (select)
- Selezione casuale delle domande per ogni sessione
- Gestione delle disconnessioni dei client
- Mantenimento delle classifiche
- Validazione dei nickname

### Client

- Interfaccia testuale intuitiva
- Gestione della connessione/disconnessione
- Visualizzazione dei punteggi
- Possibilità di partecipare a quiz diversi

## Struttura del Progetto
```
.
├── include/        # Header files
├── src/            # Source files
├── res/            # File dei quiz
└── obj/            # File oggetto (creata durante la compilazione)
```

## Pulizia del Progetto
```bash
make clean
```

## Note Tecniche

- Il progetto utilizza il protocollo TCP per la comunicazione
- Implementa una gestione robusta degli errori
- Supporta la compilazione sia in modalità release che debug
- Utilizza strutture dati dinamiche per la gestione dei giocatori e delle domande

## Autore
- Andrea Covelli
- Corso: Reti Informatiche
- Anno Accademico: 2024/2025

## Licenza
<p xmlns:cc="http://creativecommons.org/ns#" xmlns:dct="http://purl.org/dc/terms/"><a property="dct:title" rel="cc:attributionURL" href="https://github.com/AndreaCovelli/progetto_reti">Trivia Quiz Multiplayer</a> by <a rel="cc:attributionURL dct:creator" property="cc:attributionName" href="https://github.com/AndreaCovelli">Andrea Covelli</a> is licensed under <a href="https://creativecommons.org/licenses/by/4.0/?ref=chooser-v1" target="_blank" rel="license noopener noreferrer" style="display:inline-block;">CC BY 4.0<img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1" alt=""><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1" alt=""></a></p> 