#ifndef GARDIEN_H
#define GARDIEN_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"
#include "Environnement.h"
#include "Labyrinthe.h"
#include <vector>
using namespace std;

class Labyrinthe;

class Gardien : public Mover {
private:
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
public:
	
	static Sound*	_guard_fire;
	static Sound*	_guard_hit;
	static Sound*	_hit_wall;

	Gardien (Labyrinthe* l, const char* modele);

	// mon gardien pense très mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy) { 
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);	
	}
	bool canSee(vector<double> depart,vector<double> dest,double dx,double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);
	vector<double> dijkstra();//trouver chasseur par l'algo dijkstra
	bool modeAttack();//etre mode attacker
	bool existe(vector<double> n, vector< vector<double> > liste); //tester si n est dans liste
};

#endif
