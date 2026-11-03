# Codexion : les 8 paramètres et les principes à connaître

## 1. L'idée du projet

C'est une variante du problème des **philosophes qui dînent**.

- N coders sont assis en cercle, avec N dongles USB (un entre chaque paire de coders voisins).
- Pour **compiler**, un coder doit tenir **ses deux dongles** (gauche et droite) en même temps.
- Si un coder passe trop longtemps sans compiler, il fait un **burnout** et la simulation s'arrête.
- Le but : que tous les coders compilent le nombre de fois demandé, sans burnout, sans deadlock, sans data race.

Cycle de vie d'un coder :

```
prendre 2 dongles -> compiler -> rendre les dongles -> debugger -> refactorer -> (recommencer)
```

## 2. Les 8 paramètres

```bash
./codexion <coders> <burnout> <compile> <debug> <refactor> <nb_compiles> <cooldown> <scheduler>
./codexion   6        1000      200       100     150        5            50         fifo
```

Tous les temps sont en **millisecondes**. Tous les nombres doivent être positifs (sauf `cooldown`, qui peut valoir 0).

| # | Paramètre | Rôle |
|---|-----------|------|
| 1 | `number_of_coders` | Nombre de coders **et** de dongles (autant des deux). Le coder `i` utilise le dongle `i-1` (gauche) et le dongle `i % N` (droite), en comptant les dongles à partir de 0. |
| 2 | `time_to_burnout` | Durée maximale entre le **début d'une compilation** et le début de la suivante. Au-delà, le coder fait un burnout. Le compteur démarre au lancement du programme. |
| 3 | `time_to_compile` | Durée d'une compilation. Pendant ce temps, le coder garde ses 2 dongles. Le chronomètre de burnout est remis à zéro au **début** de la compilation. |
| 4 | `time_to_debug` | Temps passé à débugger après une compilation. Aucun dongle n'est tenu. |
| 5 | `time_to_refactor` | Temps passé à refactorer après le debug. Aucun dongle n'est tenu. |
| 6 | `number_of_compiles_required` | Nombre de compilations que chaque coder doit réussir. La simulation s'arrête quand **tous** y sont arrivés. |
| 7 | `dongle_cooldown` | Après avoir été rendu, un dongle reste **indisponible** pendant ce délai. |
| 8 | `scheduler` | `fifo` ou `edf` : règle qui décide qui est servi en premier quand plusieurs coders attendent un dongle. |

### Le scheduler

- **`fifo`** (First In, First Out) : le coder qui attend depuis le plus longtemps passe en premier.
- **`edf`** (Earliest Deadline First) : passe en premier le coder dont la **deadline de burnout est la plus proche**, c'est-à-dire celui qui est le plus en danger.

> Attention : dans le code actuel, le heap (`heap.c`) n'est pas encore branché. Les coders font du polling, donc `fifo` et `edf` ne changent pas encore le comportement réel.

## 3. Quand ça marche, quand ça burn

Le burnout se mesure de début de compilation à début de compilation. Pour qu'un coder survive, il faut :

```
time_to_burnout  >  time_to_compile + time_to_debug + time_to_refactor + temps d'attente des dongles
```

Exemples vérifiés :

- `6 1000 200 100 150 5 50 fifo` : cycle de 450 ms sans attente, 1000 ms de marge. **Pas de burnout**, tout le monde fait ses 5 compilations.
- `4 300 200 100 150 5 50 edf` : cycle de 450 ms, or le burnout est à 300 ms. **Burnout** vers 300 ms.
- `1 500 100 50 50 3 10 fifo` : un seul coder, donc un seul dongle, impossible d'en tenir deux. **Burnout** à 500 ms.

## 4. Les sorties (logs)

Format : `<timestamp_ms> <id_coder> <événement>`, avec l'id du coder à partir de 1.

Événements : `has taken a dongle`, `is compiling`, `is debugging`, `is refactoring`, `burned out`.

Règles :

- Les lignes ne doivent **jamais se mélanger** : l'écriture est protégée par un mutex.
- Après `burned out`, **plus aucun autre log** ne doit apparaître.
- Le burnout doit être signalé à moins de 10 ms de sa deadline réelle.

## 5. Les notions à maîtriser

**Threads.** Un thread par coder, plus un thread **monitor** qui surveille les burnouts et la fin de la simulation (vérification toutes les 1 ms).

**Mutex.** Un verrou protège chaque donnée partagée (état d'un dongle, compteur d'un coder, flag `should_stop`, sortie des logs). Règle de base : on ne lit ni n'écrit une donnée partagée sans le verrou.

**Data race.** Deux threads accèdent à la même donnée sans synchronisation, dont l'un écrit : le résultat est imprévisible. À détecter avec `-fsanitize=thread`.

**Deadlock.** Tout le monde attend quelqu'un d'autre, personne n'avance. Il faut les 4 conditions de Coffman : exclusion mutuelle, détention et attente, pas de préemption, attente circulaire. Ici, le piège classique est que tous les coders prennent leur dongle gauche en même temps et attendent le droit. **Solution retenue** : prendre les deux dongles de façon atomique (tout ou rien) sous un même verrou.

**Starvation.** Un coder n'obtient jamais ses dongles alors que les autres avancent. C'est le rôle du scheduler (`fifo` ou `edf`) de l'éviter.

**Deadline (EDF).** Pour un coder, c'est l'instant de son prochain burnout : `début de la dernière compilation + time_to_burnout`.

**Cooldown.** Un dongle est utilisable seulement s'il est **libre ET** si `maintenant - heure_de_libération >= dongle_cooldown`.

**Arrêt propre.** À la fin (succès ou burnout), il faut : mettre `should_stop`, attendre (`pthread_join`) tous les threads **y compris le monitor**, puis détruire les mutex et libérer la mémoire. Aucune fuite et aucun thread zombie.

## 6. Outils de vérification utiles

```bash
cc -g -fsanitize=address,undefined -pthread -I includes srcs/*.c -o asan   # mémoire
cc -g -fsanitize=thread -pthread -I includes srcs/*.c -o tsan              # data races
valgrind --tool=helgrind ./codexion ...                                    # verrous / deadlocks
```
