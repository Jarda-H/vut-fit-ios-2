# IOS projekt 2 2023/2024
Projekt zaměřený na procesy, sdílenou pamět, semafory a deadlocky (hlavně ty).

# Soubory
## Odevzdávané soubory
**Makefile** - pro překlad projektu - `make`

**proj2.c** - zdrojový kód

## Test scripty

**deadlock_check.sh** - kontrola, jestli program neobsahuje deadlock

**t.sh** - kontrola deadlocku + kontrola správnosti outputu

**tests.sh** - pouze test, jestli output soubor odpovídá zadání
> **tests.sh** může při velkém output souboru trvat dlouho

> Nejde spustit script? Nastavte dobře oprávnění: `chmod +x [script]`
# Hodnocení
```
15:celkem bodu za projekt
#-- automaticke hodnoceni -----------------------------
= make
:ok:make
= prepare tests: resources
:kontrola syntaxe vystupu => check_syntax.out
= base_* : zakladni testy
:ok:test_a_base_counter: navratovy kod je 0
1:ok:test_a_base_counter
1:ok:test_b_base_L: posloupnost L ok
1:ok:test_c_base_BUS: posloupnost BUS ok
1:ok:test_d_base_zast: nastup a vystup pouze v zastavce ok
1:ok:test_e_base_all: test na vse
:ok:test_g_counter: navratovy kod je 0
1:ok:test_g_counter
1:ok:test_i_L: posloupnost L
1:ok:test_j_BUS: posloupnost BUS
1:ok:test_k_zast: nastup/vystup pouze v zastavkach
2:ok:test_l_all: test na vse
2:ok:test_n_sync_sleep: bus pracuje spravne, lyzari nastupuji/vystupuji jen v zastavkach (castejsi prepinani procesu)
2:ok:test_o_sync_nosleep: bus pracuje spravne, lyzari nastupuji/vystupuji pouze v zastavkach (zruseno cekani pomoci usleep apod.)
= test spravneho ukonceni pri chybe
1:ok:test_q_error_1: osetreni chybneho vstupu
= resources
: pocet procesu ok (11, mel by byt 11)
: pocet volani wait (waitpid) ok
:ok: pripojeni ke sdilene pameti a uvolneni je korektni
:ok: korektni uvolneni nepojmenovane semafory
#------------------------------------------------------
16:celkove score (max pro hodnoceni 15)
15:celkem bodu za projekt
```
