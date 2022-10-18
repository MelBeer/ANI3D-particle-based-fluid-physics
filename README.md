---
title: ANI3D
authors: melvin.gidel, melanie.tcheou
date: 18/10/2022
---

# ANI3D Particle based fluid physics

* Melvin Gidel (melvin.gidel@epita.fr)
* Mélanie Tchéou (melanie.tcheou@epita.fr)

## Setup le projet
Pour cloner le dépôt avec la librairie, utilisez la commande suivante:
```bash
git clone --recursive git@github.com:MelBeer/ANI3D-particle-based-fluid-physics.git
```
Il vous suffira ensuite de build le projet de la façon suivante:
```bash
cd scene
mkdir build
cd build
cmake ..
make -j$(nproc)
cd ..
./build/main
```
## Rapport

Le rapport est disponible en pdf sur le dépôt, et sur hackmd (en plus lisible) au lien suivant: https://hackmd.io/@miyaou/By-Wam3ms
