#include "Gardien.h"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <sys/timeb.h>
#include <vector> 

using namespace std;

const double pi=atan(1.0)*4;
/*
 *	Tente un deplacement.
 */

bool Gardien::move_aux (double dx, double dy)
{
	if (EMPTY == _l -> data ((int)((_x + dx)/Environnement::scale),(int)((_y + dy)/Environnement::scale))||'G' == _l -> data ((int)((_x + dx)/Environnement::scale),(int)((_y + dy)/Environnement::scale)))
	{
		_l -> setData((int)((_x)/Environnement::scale),(int)((_y)/Environnement::scale),0);
		_x += dx;
		_y += dy;
		_l -> setData((int)((_x)/Environnement::scale),(int)((_y)/Environnement::scale),'G');
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Gardien::Gardien (Labyrinthe* l, const char* modele) : Mover (50, 70, l, modele)
{
	if(_guard_fire==0)
		_guard_fire = new Sound ("sons/guard_fire.wav");
	if(_guard_hit==0)
		_guard_hit = new Sound ("sons/guard_hit.wav");
	if (_hit_wall == 0)
		_hit_wall = new Sound ("sons/hit_wall.wav");
	_hp = 100;
	_init=false;
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */

bool Gardien::process_fireball (float dx, float dy)
{
	//cout << _fb -> get_x () << ' ' << _fb -> get_y () <<' '<< _fb -> get_angle()<< endl;
	//cout << _l -> _guards[0] -> _x << ' ' <<_l -> _guards[0] -> _y <<endl<<endl;
	float	x = (_l -> _guards[0] -> _x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_l -> _guards[0] -> _y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if((int)((_fb -> get_x () ) / Environnement::scale)==(int)(_l -> _guards[0] -> _x/ Environnement::scale) &&(int)((_fb -> get_y ()) / Environnement::scale)==(int)(_l -> _guards[0] -> _y/ Environnement::scale)){
		//cout << "in "<<endl;
		//_l -> _guards[0] -> _hp -=20;
		_guard_hit -> play();
		return false;
	}
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),(int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		//cout << "zheli"<<endl; 
		return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_hit_wall -> play (1. - dist2/dmax2);
	return false;
}

bool Gardien::canSee(vector<double> depart,vector<double> dest,double dx,double dy){
	//cout << depart[0]<< ' '<<depart[1]<< ' ' << dest[0]<< ' '<<dest[1]<<endl;
	double xTemp=depart[0];
	double yTemp=depart[1];
	while(((int)((xTemp+dx)/Environnement::scale)!=(int)(dest[0]/Environnement::scale))||(int)((yTemp+dy)/Environnement::scale)!=((int)(dest[1]/Environnement::scale))){
		//cout << (int)((xTemp+dx)/Environnement::scale)<<' '<<(int)((yTemp+dy)/Environnement::scale)<<endl;
		if (EMPTY != _l -> data ((int)((xTemp+dx)/Environnement::scale),(int)((yTemp+dy)/Environnement::scale))&&'G' != _l -> data ((int)((xTemp+dx)/Environnement::scale),(int)((yTemp+dy)/Environnement::scale))){
			return false;
		}
		if((fabs((xTemp+dx)-dest[0])<=1)&&(fabs((yTemp+dy)-dest[1])<=1)){
			return true;
		}
		xTemp+=dx;
		yTemp+=dy;
	}

	return true;
}

void Gardien::update (void){
		cout << 'b'<<endl;
	if(!_init){
		_move_cible[0]=(int)_x;
		_move_cible[1]=(int)_y;
		_init=true;
		
	}
	if(_hp <=0){
		rester_au_sol ();
	}
	else{
		double deltax,deltay,dx,dy;
		deltay = _l -> _guards[0] -> _y - _y;
		deltax = _l -> _guards[0] -> _x - _x;
		dx = 0.5*deltax/sqrt(pow(deltax,2)+pow(deltay,2));
		dy = 0.5*deltay/sqrt(pow(deltax,2)+pow(deltay,2));
		vector<double> point;
		point.push_back(_x);
		point.push_back(_y);
		vector<double> pointC;
		pointC.push_back(_l -> _guards[0] -> _x);
		pointC.push_back(_l -> _guards[0] -> _y);
		if(canSee(point,pointC,dx,dy)){
			_move_cible[0]=(int)pointC[0];
			_move_cible[1]=(int)pointC[1];
			struct timeb timeSeed;
			ftime(&timeSeed);
			srand(timeSeed.time * 1000 + timeSeed.millitm);
			_angle=(180/pi)*atan2(dy,dx)+270;
			int number = rand()%100+1;
			if(number==33){
				fire(0);
			}
			move(dx,dy);
		}
		else if(modeAttack()){	
			if(_x==_l -> _guards[7] -> _x&&_y==_l -> _guards[7] -> _y){
				vector<double> chemin = dijkstra();
				_move_cible[0]=(int)chemin[0];
				_move_cible[1]=(int)chemin[1];
				double deltax,deltay,dx,dy;
				deltax = chemin[0] - _x;
				deltay = chemin[1] - _y;
				if(fabs(deltax)<0.8&&fabs(deltay)<0.8){
					dx=deltax;
					dy=deltay;
				}
				else{
					dx = 1*deltax/sqrt(pow(deltax,2)+pow(deltay,2));
					dy = 1*deltay/sqrt(pow(deltax,2)+pow(deltay,2));
				}
				struct timeb timeSeed;
				ftime(&timeSeed); 
				srand(timeSeed.time * 1000 + timeSeed.millitm);
				_angle=(180/pi)*atan2(dy,dx)+270;
				if(chemin[0]==_l -> _guards[0] -> _x&&chemin[1]==_l -> _guards[0] -> _y){
					int number = rand()%100+1;
					if(number==33){
						fire(0);
					}
				}
				move(dx,dy);
			}
		}
		else if(_move_cible[0]==(int)_x&&_move_cible[1]==(int)_y){
				vector<int> chemin =patrouiller();

				_move_cible[0] = chemin[0];
				_move_cible[1] = chemin[1];
				double deltax,deltay,dx,dy;
				deltax = chemin[0] - _x;
				deltay = chemin[1] - _y;
				if(fabs(deltax)<0.8&&fabs(deltay)<0.8){
					dx=deltax;
					dy=deltay;
				}
				else{
					dx = 1*deltax/sqrt(pow(deltax,2)+pow(deltay,2));
					dy = 1*deltay/sqrt(pow(deltax,2)+pow(deltay,2));
				}
				_angle=(180/pi)*atan2(dy,dx)+270;
				move(dx,dy);
		}
		else{
				double deltax,deltay,dx,dy;
				deltax = (double)_move_cible[0] - _x;
				deltay = (double)_move_cible[1] - _y;
				if(fabs(deltax)<0.8&&fabs(deltay)<0.8){
					dx=deltax;
					dy=deltay;
				}
				else{
					dx = 1*deltax/sqrt(pow(deltax,2)+pow(deltay,2));
					dy = 1*deltay/sqrt(pow(deltax,2)+pow(deltay,2));
				}
				_angle=(180/pi)*atan2(dy,dx)+270;
				move(dx,dy);
		}	

	}
		cout << 'a'<<endl;	
}
 /*	Tire sur un ennemi.
 */

bool Gardien::modeAttack(){
	double deltax = _l -> _guards[0] -> _x / Environnement::scale - _l -> _treasor._x;
	double deltay = _l -> _guards[0] -> _y / Environnement::scale - _l -> _treasor._y;
	double d = sqrt(pow(deltax,2)+pow(deltay,2));
	if(d<10){
		return true;
	}
	return false;


}
void Gardien::fire (int angle_vertical)
{
	float	x = (_l -> _guards[0] -> _x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_l -> _guards[0] -> _y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	_guard_fire -> play (1. - dist2/dmax2);
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, 360-_angle);
}

vector<double> Gardien::dijkstra()
{
	vector<vector<double> > map;
	vector<double> point;
	point.push_back(_x);
	point.push_back(_y);
	map.push_back(point);
	for(int i=0;i<_l -> width(); i++){
		for(int j=1 ; j <= _l -> height(); j++){
			if(_l -> data (i,j)=='+'||_l -> data (i,j)=='B'||_l -> data (i,j)=='T'){
				if(i!=_l->width()-1&&j!=_l -> height()){
					point[0]=(i+1)*Environnement::scale;
					point[1]=(j+1)*Environnement::scale;
					if(_l -> data (i+1,j+1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
				if(i!=0&&j!=1){
					point[0]=(i-1)*Environnement::scale;
					point[1]=(j-1)*Environnement::scale;
					if(_l -> data (i-1,j-1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
				if(i!=0&&j!=_l -> height()){
					point[0]=(i-1)*Environnement::scale;
					point[1]=(j+1)*Environnement::scale;
					if(_l -> data (i-1,j+1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
				if(i!=_l->width()-1&&j!=1){
					point[0]=(i+1)*Environnement::scale;
					point[1]=(j-1)*Environnement::scale;
					if(_l -> data (i+1,j-1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
			}
		}
	}
	point[0]=_l -> _guards[0] -> _x;
	point[1]=_l -> _guards[0] -> _y;
	map.push_back(point);
	bool vis[map.size()];
	int prev[map.size()];
	double dis[map.size()];
	int minNb=0;
	for(vector<double>::size_type i = 0; i<map.size();i++){
		dis[i]=99999;
		prev[i]=99;
		vis[i]=false;	
	}
	dis[0]=0;
	for(vector<double>::size_type k=0;k<map.size();k++){
		double min = 99999.9;
		minNb = 99;

		for(vector<double>::size_type i=0;i<map.size();i++){
			if(!vis[i]){
				if(dis[i]<min){
					min=dis[i];
					minNb=i;
				}
			}
		}
		//cout << map[minNb][0]<<' '<<map[minNb][1]<<endl;
		vis[minNb]=true;
		if(map[minNb][0]==_l -> _guards[0] -> _x&&map[minNb][1]==_l -> _guards[0] -> _y){
			int i = minNb;
			vector<vector<double> > list;
			list.push_back(map[i]);
			while(i!=0){
				list.push_back(map[prev[i]]);
				i=prev[i];	
			}
			if(list.size()==1){
				//cout << list[0][0]<< ' '<<list[0][1]<<endl;
				return list[0];
				
			}
			
			else{
				//cout << list[list.size()-2][0]<<' ' << list[list.size()-2][1]<<endl;
				return list[list.size()-2];}
		}

		for(vector<double>::size_type i=0; i<map.size(); i++){
			if(!vis[i]){
				double deltax,deltay,dx,dy;
				deltay = map[i][1] - map[minNb][1];
				deltax = map[i][0] - map[minNb][0];
				dx = 1*deltax/sqrt(pow(deltax,2)+pow(deltay,2));
				dy = 1*deltay/sqrt(pow(deltax,2)+pow(deltay,2));
				if(canSee(map[minNb],map[i],dx,dy)){
					double distance = sqrt(pow(deltax,2)+pow(deltay,2));
					if(dis[i]>dis[minNb]+distance){
						dis[i]=dis[minNb]+distance;
						prev[i]=minNb;
					}
				}
				
			}
		}
		
	}
	return map[0];
}
vector<int>  Gardien::patrouiller(){
	
	vector<vector<double> > map;
	vector<double> point;
	point.push_back(_x);
	point.push_back(_y);
	map.push_back(point);
	for(int i=0;i<_l -> width(); i++){
		for(int j=1 ; j <= _l -> height(); j++){
			if(_l -> data (i,j)=='+'||_l -> data (i,j)=='B'||_l -> data (i,j)=='T'){
				if(i!=_l->width()-1&&j!=_l -> height()){
					point[0]=(i+1)*Environnement::scale;
					point[1]=(j+1)*Environnement::scale;
					if(_l -> data (i+1,j+1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
				if(i!=0&&j!=1){
					point[0]=(i-1)*Environnement::scale;
					point[1]=(j-1)*Environnement::scale;
					if(_l -> data (i-1,j-1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
				if(i!=0&&j!=_l -> height()){
					point[0]=(i-1)*Environnement::scale;
					point[1]=(j+1)*Environnement::scale;
					if(_l -> data (i-1,j+1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
				if(i!=_l->width()-1&&j!=1){
					point[0]=(i+1)*Environnement::scale;
					point[1]=(j-1)*Environnement::scale;
					if(_l -> data (i+1,j-1)==EMPTY&&!existe(point,map)){
						map.push_back(point);
					}
				}
			}
		}
	}
	double disMax=0;
	vector<int> disMaxPoint;
	disMaxPoint.push_back((int)_x);
	disMaxPoint.push_back((int)_y);
	for(vector<double>::size_type i=1; i<map.size(); i++){
			double deltax,deltay,dx,dy;
			deltay = map[i][1] - map[0][1];
			deltax = map[i][0] - map[0][0];
			dx = 1*deltax/sqrt(pow(deltax,2)+pow(deltay,2));
			dy = 1*deltay/sqrt(pow(deltax,2)+pow(deltay,2));
			//cout << map[0][0]<<' '<<map[0][1]<<' '<<map[i][0]<<' '<<map[i][1]<<endl;
			if(canSee(map[0],map[i],dx,dy)){
				double distance = sqrt(pow(deltax,2)+pow(deltay,2));
				if(disMax<distance){
					disMax=distance;
					disMaxPoint[0]=(int)map[i][0];
					disMaxPoint[1]=(int)map[i][1];
				
				}		
			}
		
	}
	//cout << _x<<' '<<_y<<' '<<disMaxPoint[0]<<' ' << disMaxPoint[1]<<endl;
	return disMaxPoint;

}

bool Gardien::existe(vector<double> n, vector<vector<double> > liste){

	for(vector<double>::size_type i =0 ; i<liste.size() ;i++){
		if (n==liste[i]){
			return true;
		} 
	}
	return false;
}
