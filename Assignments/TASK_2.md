# Algorithmes

## Objectif 1 - Refactorisation de l'existant

### A - Structured Bindings

`TowerSimulation::display_help()` est chargé de l'affichage des touches disponibles.
Dans sa boucle, remplacez `const auto& ks_pair` par un structured binding adapté.

```cpp
for (const auto& [first, _] : GL::keystrokes)
{
    std::cout << first << ' ';
}
```
> second est ignoré puisque l'on n'en a pas besoin.

### B - Algorithmes divers

1. `AircraftManager::move()` (ou bien `update()`) supprime les avions de la `move_queue` dès qu'ils sont "hors jeux".
En pratique, il y a des opportunités pour des pièges ici. Pour les éviter, `<algorithm>` met à disposition la fonction `std::remove_if`.
Remplacez votre boucle avec un appel à `std::remove_if`.

**Attention**: pour cela c'est necessaire que `AircraftManager` stocke les avion dans un `std::vector` ou `std::list` (c'est déjà le cas pour la solution filé).

```cpp
void AircraftManager::move(float dt)
{
    aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(), [dt](const auto &aircraft) {
       if (aircraft->finished) {
           return true;
       }
       else
       {
           aircraft->move(dt);
           return false;
       }
    }), aircrafts.end());
}
```
> On utilise remove_if (avec erase). Pour éviter une deuxième "tour de boucle", on peut move l'aircraft
> depuis le predicate.

2. Pour des raisons de statistiques, on aimerait bien être capable de compter tous les avions de chaque airline.
A cette fin, rajoutez des callbacks sur les touches `0`..`7` de manière à ce que le nombre d'avions appartenant à `airlines[x]` soit affiché en appuyant sur `x`.
Rendez-vous compte de quelle classe peut acquérir cette information. Utilisez la bonne fonction de `<algorithm>` pour obtenir le résultat.

> C'est la AircraftFactory qui connait ces informations. Pour compter, on peut utiliser count_if

```cpp
long AircraftFactory::count_aircraft_from_airline(unsigned int airline) const
{
    auto &line = airlines[airline];
    return std::count_if(
        used_flight_numbers.begin(),
        used_flight_numbers.end(),
        [line](const std::string &flight_number) {
            return flight_number.rfind(line, 0) == 0;
        });
}
```

> Et dans TowerSimulation::create_keystrokes

```cpp
for (auto i = 0u; i < 8; ++i)
{
    GL::keystrokes.emplace(i + '0', [this, i]() {
        std::cout << "Airline n°" << i << " has " << aircraft_factory.count_aircraft_from_airline(i) << " flights" << std::endl;
    });
}
```

### C - Relooking de Point3D

La classe `Point3D` présente beaucoup d'opportunités d'appliquer des algorithmes.
Particulairement, des formulations de type `x() = ...; y() = ...; z() = ...;` se remplacent par un seul appel à la bonne fonction de la librairie standard.
Remplacez le tableau `Point3D::values` par un `std::array` et puis,
remplacez le code des fonctions suivantes en utilisant des fonctions de `<algorithm>` / `<numeric>`:

1. `Point3D::operator*=(const float scalar)`
2. `Point3D::operator+=(const Point3D& other)` et `Point3D::operator-=(const Point3D& other)`
3. `Point3D::length() const`

> Voir Point3D

---

## Objectif 2 - Rupture de kérosène

Vous allez introduire la gestion de l'essence dans votre simulation.\
Comme le but de ce TP est de vous apprendre à manipuler les algorithmes de la STL, avant d'écrire une boucle, demandez-vous du coup s'il n'existe pas une fonction d'`<algorithm>` ou de `<numeric>` qui permet de faire la même chose.

La notation tiendra compte de votre utilisation judicieuse de la librairie standard. 

### A - Consommation d'essence

Ajoutez un attribut `fuel` à `Aircraft`, et initialisez-le à la création de chaque avion avec une valeur aléatoire comprise entre `150` et `3'000`.\
Décrémentez cette valeur dans `Aircraft::update` si l'avion est en vol.\
Lorsque cette valeur atteint 0, affichez un message dans la console pour indiquer le crash, et faites en sorte que l'avion soit supprimé du manager.

N'hésitez pas à adapter la borne `150` - `3'000`, de manière à ce que des avions se crashent de temps en temps.

> Dans Aircraft::move, dans la partie !is_on_ground, je décrémente le fuel, et s'il est 
> inférieur ou égal à 0, je set le flag finished à true, ce qui fait que le manager va le supprimer de la queue.

### B - Un terminal s'il vous plaît

Afin de minimiser les crashs, il va falloir changer la stratégie d'assignation des terminaux aux avions.
Actuellement, chaque avion interroge la tour de contrôle pour réserver un terminal dès qu'il atteint son dernier `Waypoint`.
Si un terminal est libre, la tour lui donne le chemin pour l'atteindre, sinon, elle lui demande de tourner autour de l'aéroport.
Pour pouvoir prioriser les avions avec moins d'essence, il faudrait déjà que les avions tentent de réserver un terminal tant qu'ils n'en n'ont pas (au lieu de ne demander que lorsqu'ils ont terminé leur petit tour).

1. Introduisez une fonction `bool Aircraft::has_terminal() const` qui indique si un terminal a déjà été réservé pour l'avion (vous pouvez vous servir du type de `waypoints.back()`).
> `bool has_terminal() const {return !waypoints.empty() && waypoints.back().is_at_terminal();}`
2. Ajoutez une fonction `bool Aircraft::is_circling() const` qui indique si l'avion attend qu'on lui assigne un terminal pour pouvoir attérir.
> Pour déterminer qu'un avion attend, j'ai changé la fonction get_circle de Tower pour que le dernier point
> soit d'un nouveau type wp_circle. De cette façon : `bool is_circling() const {return !waypoints.empty() && waypoints.back().type == wp_circle;}`
3. Introduisez une fonction `WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)` qui essaye de réserver un `Terminal`. Si c'est possible, alors elle retourne un chemin vers ce `Terminal`, et un chemin vide autrement (vous pouvez vous inspirer / réutiliser le code de `Tower::get_instructions`).
4. Modifiez la fonction `move()` (ou bien `update()`) de `Aircraft` afin qu'elle appelle `Tower::reserve_terminal` si l'avion est en attente. Si vous ne voyez pas comment faire, vous pouvez essayer d'implémenter ces instructions :\
\- si l'avion a terminé son service et sa course, alors on le supprime de l'aéroport (comme avant),\
\- si l'avion attend qu'on lui assigne un terminal, on appelle `Tower::reserve_terminal` et on modifie ses `waypoints` si le terminal a effectivement pu être réservé,\
\- si l'avion a terminé sa course actuelle, on appelle `Tower::get_instructions` (comme avant).

> Avant de vérifier que la queue est vide, on essaie de réserver un terminal si l'avion n'en a pas
```cpp
if (is_circling() && !has_terminal())
    {
        auto new_waypoints = control.reserve_terminal(*this);
        if (!new_waypoints.empty())
        {
            waypoints = std::move(new_waypoints);
        }
    }
}
```
> On fait bien de remarquer que les waypoints doivent être écrasés si on a réussi à trouver un terminal.

### C - Minimiser les crashs

Grâce au changement précédent, dès lors qu'un terminal est libéré, il sera réservé lors du premier appel à `Aircraft::update` d'un avion recherchant un terminal.
Pour vous assurez que les terminaux seront réservés par les avions avec le moins d'essence, vous allez donc réordonner la liste des `aircrafts` avant de les mettre à jour.

Vous devrez placer au début de la liste les avions qui ont déjà réservé un terminal.\
Ainsi, ils pourront libérer leurs terminaux avant que vous mettiez à jour les avions qui essayeront de les réserver.

La suite de la liste sera ordonnée selon le niveau d'essence respectif de chaque avion.

Par exemple :
```b
A - Reserved / Fuel: 100
B - NotReserved / Fuel: 50
C - NotReserved / Fuel: 300
D - NotReserved / Fuel: 150
E - Reserved / Fuel: 2500
```
pourra être réordonné en
```b
A - Reserved / Fuel: 100
E - Reserved / Fuel: 2500
B - NotReserved / Fuel: 50
D - NotReserved / Fuel: 150
C - NotReserved / Fuel: 300
```

Assurez-vous déjà que le conteneur `AircraftManager::aircrafts` soit ordonnable (`vector`, `list`, etc).\
Au début de la fonction `AircraftManager::move` (ou `update`), ajoutez les instructions permettant de réordonner les `aircrafts` dans l'ordre défini ci-dessus.

```cpp
std::sort(aircrafts.begin(), aircrafts.end(), [](const auto& a, const auto& b) {
    if (a->has_terminal()) {
        return true;
    } else if (b->has_terminal()) {
        return false;
    } else {
        return a->fuel < b->fuel;
    }
});
```

### D - Réapprovisionnement 

Afin de pouvoir repartir en toute sécurité, les avions avec moins de `200` unités d'essence doivent être réapprovisionnés par l'aéroport pendant qu'ils sont au terminal.

1. Ajoutez une fonction `bool Aircraft::is_low_on_fuel() const`, qui renvoie `true` si l'avion dispose de moins de `200` unités d'essence.\
Modifiez le code de `Terminal` afin que les avions qui n'ont pas suffisamment d'essence restent bloqués.\
Testez votre programme pour vérifier que certains avions attendent bien indéfiniment au terminal.
Si ce n'est pas le cas, essayez de faire varier la constante `200`.

2. Dans `AircraftManager`, implémentez une fonction `get_required_fuel`, qui renvoie la somme de l'essence manquante (le plein, soit `3'000`, moins la quantité courante d'essence) pour les avions vérifiant les conditions suivantes :\
\- l'avion est bientôt à court d'essence\
\- l'avion n'est pas déjà reparti de l'aéroport.

> On peut utiliser std::accumulate pour calculer la somme des éléments qu'on veut. Dans le cas
> où l'élément n'est pas concerné, on peut ajouter 0 à l'accumulateur à la place.

```cpp
long AircraftManager::get_required_fuel() const
{
    return std::accumulate(aircrafts.begin(), aircrafts.end(), 0, [](long acc, const auto& aircraft) {
        return acc + (!aircraft->serviced && aircraft->is_low_on_fuel() ? Aircraft::FUEL_THRESHOLD - aircraft->fuel : 0);
    });
}
```

3. Ajoutez deux attributs `fuel_stock` et `ordered_fuel` dans la classe `Airport`, que vous initialiserez à 0.\
Ajoutez également un attribut `next_refill_time`, aussi initialisé à 0.\
Enfin, faites en sorte que la classe `Airport` ait accès à votre `AircraftManager` de manière à pouvoir l'interroger.

> Puisque je ne veux absolument pas donner l'ownership du manager à Airport, je ne passe
> qu'une référence.

4. Ajoutez une fonction `refill` à la classe `Aircraft`, prenant un paramètre `fuel_stock` par référence non-constante.
Cette fonction rempliera le réservoir de l'avion en soustrayant ce dont il a besoin de `fuel_stock`.
Bien entendu, `fuel_stock` ne peut pas devenir négatif.\
Indiquez dans la console quel avion a été réapprovisionné ainsi que la quantité d'essence utilisée.

5. Définissez maintenant une fonction `refill_aircraft_if_needed` dans la classe `Terminal`, prenant un paramètre `fuel_stock` par référence non-constante.
Elle devra appeler la fonction `refill` sur l'avion actuellement au terminal, si celui-ci a vraiment besoin d'essence.  

6. Modifiez la fonction `Airport::move`, afin de mettre-en-oeuvre les étapes suivantes.\
   \- Si `next_refill_time` vaut 0 :\
       \* `fuel_stock` est incrémenté de la valeur de `ordered_fuel`.\
       \* `ordered_fuel` est recalculé en utilisant le minimum entre `AircraftManager::get_required_fuel()` et `5'000` (il s'agit du volume du camion citerne qui livre le kérosène).\
       \* `next_refill_time` est réinitialisé à `100`.\
       \* La quantité d'essence reçue, la quantité d'essence en stock et la nouvelle quantité d'essence commandée sont affichées dans la console.\
   \- Sinon `next_refill_time` est décrémenté.\
   \- Chaque terminal réapprovisionne son avion s'il doit l'être.

> Voire les classes concernées.

### E - Déréservation

Si vous avez suffisamment testé votre programme, vous avez dû vous apercevoir que parfois, certains terminaux arrêtaient d'être réservés et utilisés.\
En effet, lorsque les avions se crashent alors qu'ils avaient un terminal de réservé, rien n'a été fait pour s'assurer que le terminal allait de nouveau être libre.

Pour garantir cela, vous allez modifier le destructeur de `Aircraft`. Si l'avion a réservé un terminal, assurez-vous que celui-ci est correctement libéré. Pour cela, vous aurez besoin de rajouter une fonction dans la classe `Tower`. Choisissez-lui un nom qui décrit correctement ce qu'elle fait.

> Je l'ai appelée release_terminal. D'autre part, j'ai dû ajouter une fonction Terminal::release.

### F - Paramétrage (optionnel)

Pour le moment, tous les avions ont la même consommation d'essence (1 unité / trame) et la même taille de réservoir (`3'000`).

1. Arrangez-vous pour que ces deux valeurs soient maintenant déterminées par le type de chaque avion (`AircraftType`).

2. Pondérez la consommation réelle de l'avion par sa vitesse courante.
   La consommation définie dans `AircraftType` ne s'appliquera que lorsque l'avion est à sa vitesse maximale.

3. Un avion indique qu'il a besoin d'essence lorsqu'il a moins de `200` unités.
   Remplacez cette valeur pour qu'elle corresponde à la quantité consommée en 10s à vitesse maximale.\
   Si vous n'avez pas fait la question bonus de TASK_0, notez bien que la fonction `move` de chaque avion devrait être appelée `DEFAULT_TICKS_PER_SEC` fois par seconde. 