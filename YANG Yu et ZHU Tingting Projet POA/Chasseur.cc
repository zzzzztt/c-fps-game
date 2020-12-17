#include "Chasseur.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sys/timeb.h>
using namespace std;
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),(int)((_y + dy) / Environnement::scale))||'G' == _l -> data ((int)((_x + dx) / Environnement::scale),(int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (0, 0, l, 0)
{
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	_guard_die = new Sound ("sons/guard_die.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
	_hp = 100;
}


/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */

void Chasseur::update (void){
	if(_hp <= 0){
		partie_terminee (false);
	}

}
	
bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// on bouge que dans le vide!
	for(int i = 1 ; i< _l -> _nguards ; i++){
		if((int)((_fb -> get_x () +dx) / Environnement::scale)==(int)(_l -> _guards[i] -> _x/ Environnement::scale) &&(int)((_fb -> get_y ()+dy) / Environnement::scale)==(int)(_l -> _guards[i] -> _y/ Environnement::scale)){
			cout << _fb -> get_z()<<endl;
			if(_fb -> get_z ()<=15&&_fb -> get_z()>12)
				_l -> _guards[i] -> _hp -=20;
			if(_fb -> get_z ()<=12&&_fb -> get_z()>8)
				_l -> _guards[i] -> _hp -=15;
			if(_fb -> get_z ()<=8&&_fb -> get_z()>0)
				_l -> _guards[i] -> _hp -=10;
			if(_l -> _guards[i] -> _hp<=0){
				_guard_die -> play(1. - dist2/dmax2);
			}
			_hunter_hit -> play(1. - dist2/dmax2);
			return false;
		}
	}
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.

	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	// teste si on a touché le trésor: juste pour montrer un exemple de la
	// fonction « partie_terminee ».
	if ((int)((_fb -> get_x () + dx) / Environnement::scale) == _l -> _treasor._x &&
		(int)((_fb -> get_y () + dy) / Environnement::scale) == _l -> _treasor._y)
	{
		partie_terminee (true);
	}
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, _angle);
}

