# Se familiariser avec l'existant

## A- Exécution

Compilez et lancez le programme.

Allez dans le fichier `tower_sim.cpp` et recherchez la fonction responsable de gérer les inputs du programme.
Sur quelle touche faut-il appuyer pour ajouter un avion ?
Comment faire pour quitter le programme ?
A quoi sert la touche 'F' ?

Ajoutez un avion à la simulation et attendez.
Que est le comportement de l'avion ?
> L'avion atterri et tente d'aller en service pendant un moment, avant de finir et de redécoller, tout cela en boucle

Quelles informations s'affichent dans la console ?
> Pour un avion, chaque étape est annoté de son nom : il atterri, va en service, fini son service, puis décolle

Ajoutez maintenant quatre avions d'un coup dans la simulation.
Que fait chacun des avions ?
> Ils font toujours la même chose, mais de manière organisée (s'il n'y a plus de place pour entrer en service, ils ne tentent pas d'atterrir)

## B- Analyse du code

Listez les classes du programme à la racine du dossier src/.
Pour chacune d'entre elle, expliquez ce qu'elle représente et son rôle dans le programme.

Pour les classes `Tower`, `Aircaft`, `Airport` et `Terminal`, listez leurs fonctions-membre publiques et expliquez précisément à quoi elles servent.
Réalisez ensuite un schéma présentant comment ces différentes classes intéragissent ensemble.

> - Aircraft représente un avion et son rôle est d'avoir un numéro de vol (get_flight_num), de pouvoir donner sa distance à un point dans l'espace (distance_to), de s'afficher (display), et de pouvoir se déplacer (move)
> - AircraftType représente un type d'avion et ses données (vitesse au sol, vitesse aérienne, accelération, sprite)
> - Airport représente un aéroport, et son rôle est de pouvoir désigner sa tour de controle (get_tower), de s'afficher (display), et de déplacer ses terminaux (move)
> - AirportType représente un type d'aéroport et ses données (position du croisement, position de la porte, positions des terminaux, et positions des pistes)
> - Point2D représente un point avec deux coordonnées, et est capable d'effectuer des opérations géométriques basiques.
> - Point3D représente un point avec trois coordonnées, et est capable d'effectuer des opérations géométriques basiques.
> - Runway représente une piste d'atterrissage, et connais sa position de départ et de fin (c'est un segment) par rapport à la position de l'aéroport.
> - Terminal représente un terminal de service, et a pour rôle de pouvoir exprimer s'il est en cours d'utilisation (in_use), s'il est en service (is_servicing), pouvoir y être assigné un avion (assing_craft), pouvoir démarrer son service (start_service), pouvoir finir son service (finish_service), et d'avancer dans son cycle de services (move)
> - Tower représente une tour de controle et qui peut envoyer des instructions à un avion (visitor get_instructions) et d'être notifié quand un avion est arrivé à un terminal de service (observer arrived_at_terminal)
> - Waypoint est un point 3D spécifique sur le chemin d'un avion, qui sait s'il est au sol ou en l'air, et à un terminal ou non.
> - WaypointType est un enum représentant les différents types de waypoints
> - TowerSimulation est la classe controller de la simulation entière, et son rôle est simplement de pouvoir être lancée (launch).

Quelles classes et fonctions sont impliquées dans la génération du chemin d'un avion ?
Quel conteneur de la librairie standard a été choisi pour représenter le chemin ?
> C'est une deque

Expliquez les intérêts de ce choix.
> On peu emplacer et retirer au début et à la fin (une queue à double entrée/sortie)

## C- Bidouillons !

1) Déterminez à quel endroit du code sont définies les vitesses maximales et accélération de chaque avion.
Le Concorde est censé pouvoir voler plus vite que les autres avions.
Modifiez le programme pour tenir compte de cela.
> Voir aircraft_types.hpp

2) Identifiez quelle variable contrôle le framerate de la simulation.
> GL::ticks_per_sec

Ajoutez deux nouveaux inputs au programme permettant d'augmenter ou de diminuer cette valeur.
> a: fps-- ; z: fps++

Essayez maintenant de mettre en pause le programme en manipulant ce framerate. Que se passe-t-il ?
> Exception en point flottant (division par zéro, surement)

Ajoutez une nouvelle fonctionnalité au programme pour mettre le programme en pause, et qui ne passe pas par le framerate.
> opengl_interface.cpp -> timer -> ajout de variable globale 'paused'

3) Identifiez quelle variable contrôle le temps de débarquement des avions et doublez-le.
> config.hpp -> SERVICE_CYCLES

4) Lorsqu'un avion a décollé, il réattérit peu de temps après.
Faites en sorte qu'à la place, il soit retiré du programme.\
Indices :\
A quel endroit pouvez-vous savoir que l'avion doit être supprimé ?\
> Dans Tower::get_instruction

Pourquoi n'est-il pas sûr de procéder au retrait de l'avion dans cette fonction ?
> Si on fait ça là, il y a un risque de poser problème aux move_queue et display_queue

A quel endroit de la callstack pourriez-vous le faire à la place ?\
Que devez-vous modifier pour transmettre l'information de la première à la seconde fonction ?

> DynamicObject::move retourne maintenant un boolean. Quand a avion a été serviced
> et qu'il n'a plus rien dans sa queue de waypoints, cela
> veut dire qu'il est en l'air et qu'il a fini son chemin
> à ce moment là, move va retourner false, et trigger
> sa suppression dans la fonction timer de GL
```cpp
for (auto it = move_queue.begin(); it != move_queue.end();)
{
    auto * dObj = *it;
    if (dObj->move())
    {
        ++it;
    }
    else
    {
        it = move_queue.erase(it);
        delete dObj;
    }
}
```
> Il est à noter que le delete provoque une segfault ici
> car l'objet est toujours présent dans la display_queue.
> Ce problème est réglé dans la question suivante

5) Lorsqu'un objet de type `Displayable` est créé, il faut ajouter celui-ci manuellement dans la liste des objets à afficher.
Il faut également penser à le supprimer de cette liste avant de le détruire.
Faites en sorte que l'ajout et la suppression de `display_queue` soit "automatiquement gérée" lorsqu'un `Displayable` est créé ou détruit.
Pourquoi n'est-il pas spécialement pertinent d'en faire de même pour `DynamicObject` ?

> Voir le constructeur et destructeur de Displayable.
> D'autre part, on ne sait pas si un DynamicObject doit forcément
> être ajouté à la move_queue ou non a sa création,
> ou même son comportement à sa délétion.

6) La tour de contrôle a besoin de stocker pour tout `Aircraft` le `Terminal` qui lui est actuellement attribué, afin de pouvoir le libérer une fois que l'avion décolle.
Cette information est actuellement enregistrée dans un `std::vector<std::pair<const Aircraft*, size_t>>` (size_t représentant l'indice du terminal).
Cela fait que la recherche du terminal associé à un avion est réalisée en temps linéaire, par rapport au nombre total de terminaux.
Cela n'est pas grave tant que ce nombre est petit, mais pour préparer l'avenir, on aimerait bien remplacer le vector par un conteneur qui garantira des opérations efficaces, même s'il y a beaucoup de terminaux.\
Modifiez le code afin d'utiliser un conteneur STL plus adapté. Normalement, à la fin, la fonction `find_craft_and_terminal(const Aicraft&)` ne devrait plus être nécessaire.

> C'est une map au lieu d'un vecteur de pair.
> std::map<const Aircraft*, size_t>.
> Récupérer une valeur à partir d'une clé est
> en complexité O(1)

## D- Théorie

1) Comment a-t-on fait pour que seule la classe `Tower` puisse réserver un terminal de l'aéroport ?

> La map reserved_terminal de Tower est privée,
> par conséquent, seul Tower possède un contrôle
> sur elle.

2) En regardant le contenu de la fonction `void Aircraft::turn(Point3D direction)`, pourquoi selon-vous ne sommes-nous pas passés par une référence ?
Pensez-vous qu'il soit possible d'éviter la copie du `Point3D` passé en paramètre ?

> direction est passé par copie, car turn effectue des mutations irréversibles dessus (avec cap_length).
> Une façon d'autoriser le passage par référence serait de rendre cap_length const, et qu'elle renvoie un
> nouveau point à la place.

## E- Bonus

Le temps qui s'écoule dans la simulation dépend du framerate du programme.
La fonction move() n'utilise pas le vrai temps. Faites en sorte que si.
Par conséquent, lorsque vous augmentez le framerate, la simulation s'exécute plus rapidement, et si vous le diminuez, celle-ci s'exécute plus lentement.

Dans la plupart des jeux ou logiciels que vous utilisez, lorsque le framerate diminue, vous ne le ressentez quasiment pas (en tout cas, tant que celui-ci ne diminue pas trop).
Pour avoir ce type de résultat, les fonctions d'update prennent généralement en paramètre le temps qui s'est écoulé depuis la dernière frame, et l'utilise pour calculer le mouvement des entités.

Recherchez sur Internet comment obtenir le temps courant en C++ et arrangez-vous pour calculer le dt (delta time) qui s'écoule entre deux frames.
Lorsque le programme tourne bien, celui-ci devrait être quasiment égale à 1/framerate.
Cependant, si le programme se met à ramer et que la callback de glutTimerFunc est appelée en retard (oui oui, c'est possible), alors votre dt devrait être supérieur à 1/framerate.

Passez ensuite cette valeur à la fonction `move` des `DynamicObject`, et utilisez-la pour calculer les nouvelles positions de chaque avion.
Vérifiez maintenant en exécutant le programme que, lorsque augmentez le framerate du programme, vous n'augmentez pas la vitesse de la simulation.

Ajoutez ensuite deux nouveaux inputs permettant d'accélérer ou de ralentir la simulation.
