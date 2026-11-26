# Implementace překladače imperativního jazyka IFJ26

Překladač z jazyka IFJ26 do jazyka IFJcode26

# Autoři

- **Roman Skoupý** *(vedoucí)* – 281698 (`xskoupr00`)
- Matouš Jahoda – 281744 (`xjahodm00`)
- Tomáš Peňás – 281746 (`xpenast00`)
- Simon Šmíd – 282246 (`xsmidsi00`)

# Požadavky

- Překladač **GCC** (verze 14 / 15 / 16)
- **GNU Make 4.3**

# Překlad a spouštění

Překladač načítá zdrojový kód ze standardního vstupu (`stdin`) a generuje výsledný kód IFJcode26 na standardní výstup (`stdout`). Chybová a ladicí hlášení jsou vypisována na `stderr`.

## Sestavení projektu

```bash
# sestavení a překlad
make

# přeloží debug verze překladače a testů
make debug

# odstraní přeložené binární soubory, složku build/ a archiv xskoupr00.zip
make clean
```

## Spouštění

```bash
# vstup na `stdin`, výstup na `stdout` a `stderr`
./ic26int
```

## Spouštění testů

```bash
# Spuštění všech testů
make test

# Spuštění libovolných testů
make test FILTER="<název testu>"
```

# Implementovaná rozšíření

- FUNEXP
- BOOLTHEN
- CONTROL
- VALMEM
- ALGEBDT
- LAMBDAS
- LISTS
- EXTKW
- PARAMLISTS

# Struktura repozitáře

```text
.
├── build/  
├── docs/                # Zdrojové soubory k dokumentaci
├── examples/            # Ukázkové programy ve zdrojovém i cílovém kódu
├──── basic/
├──── extensions/
├── src/                 # Zdrojové a hlavičkové soubory
├── tests/               # Jednotkové a integrační testy
├── dokumentace.pdf
├── is_it_ok.sh          # Kontrolní skript pro ověření formálních náležitostí
├── Makefile
├── README.md
├── rozdeleni
└── rozsireni
```