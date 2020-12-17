#include <iostream>
#include <fstream>
#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

using namespace std;

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.
Sound*	Gardien::_guard_fire;	
Sound*	Gardien::_guard_hit;	
Sound*	Chasseur::_guard_die;	
Sound*	Gardien::_hit_wall;

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

Labyrinthe::Labyrinthe (char* filename)
{
	std::ifstream map;
	int ligne=0;
	int col=0;
	lab_height = 93;
	lab_width = 62;
	char c;
	// création du tableau d'occupation du sol.
	_data = new char* [lab_width];
	for (int i = 0; i < lab_width; ++i)
		_data [i] = new char [lab_height];
	char mapTemp[lab_width][lab_height];
	map.open(filename);
	if (!map) {
        	cout << "Could not find the file\n";
    	}
	map >> noskipws;
	while(!map.eof()){
		col++;
		map >> c;
		if(c=='\n'){
			mapTemp[ligne][col]=c;
			ligne++;
			col=0;
		}
		else{
			mapTemp[ligne][col]=c;
		}
	}
	map.close();
	_nguards = 1;
	_guards = new Mover* [64];
	const char* nameGuard[] = {"Blade","Lezard","drfreak","garde","Marvin","Potator","Somourai","serpent"};
	_walls = new Wall [128];
	_nwall = 0;
	_picts = new Wall [128];
	_npicts = 0;
	_boxes = new Box [16];
	_nboxes = 0;
	// initialisation du tableau d'occupation du sol.
	for (int i = 0;i<lab_width;i++){
		for(int j = 0;j<lab_height;j++){
			if(mapTemp[i][j]=='G'){
				_guards [_nguards] = new Gardien (this, nameGuard[(_nguards+1)%7]); _guards [_nguards] -> _x = (float) i*Environnement::scale; _guards [_nguards] -> _y = (float) j*Environnement::scale;
				_data[(int)(_guards [_nguards] -> _x/Environnement::scale)][(int)(_guards [_nguards] -> _y/Environnement::scale)] ='G';
				_nguards++;
			}
			if(mapTemp[i][j]=='T'){
				_treasor._x = i;
				_treasor._y = j;
				_data [_treasor._x][_treasor._y] ='T';
			}
			if(mapTemp[i][j]=='C'){
				_guards [0] = new Chasseur (this);
				_guards [0] ->_x = (float)i*10; _guards [0]->_y = (float)j*10;
			}
			if(mapTemp[i][j]=='x'){
				_boxes [_nboxes]._x = i; _boxes [_nboxes]._y = j; _boxes [_nboxes]._ntex = 0;
				++_nboxes;
				_data[i][j]='B';
				//cout << "box ajoute " <<i<<' '<<j<<endl ;
			}
			//if(mapTemp[i][j]=='G')
			if(mapTemp[i][j]=='+'&&(mapTemp[i+1][j]=='|'||mapTemp[i+1][j]=='+')){
				_walls [_nwall]._x1 = i; _walls [_nwall]._y1 = j;
				_data[i][j]='+';
				int iTemp=i+1;
				while(mapTemp[iTemp][j]!='+'){
					_data[iTemp][j]=1;
					if(mapTemp[iTemp][j]=='a'){
						_picts [_npicts]._ntex = 0;
						_picts [_npicts]._x1 = iTemp; _picts [_npicts]._y1 = j;
						_picts [_npicts]._x2 = iTemp+2; _picts [_npicts]._y2 = j;
						++_npicts;
						//cout << "pic1 ajoute "<<iTemp<<' '<<j<<endl ;
					}
					if(mapTemp[iTemp][j]=='b'){
						char	tmp [128];
						sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
						_picts [_npicts]._ntex = wall_texture (tmp);
						_picts [_npicts]._x1 = iTemp; _picts [_npicts]._y1 = j;
						_picts [_npicts]._x2 = iTemp+2; _picts [_npicts]._y2 = j;
						++_npicts;
						//cout << "pic2 ajoute "<<iTemp<<' '<<j<<endl ;
					}
					iTemp++;
				}
				_walls [_nwall]._x2 = iTemp; _walls [_nwall]._y2 = j;
				_walls [_nwall]._ntex = 0;
				_data[iTemp][j]='+';
				_nwall++;
				
			}
			if(mapTemp[i][j]=='+'&&(mapTemp[i][j+1]=='-'||mapTemp[i][j+1]=='+')){
				_walls [_nwall]._x1 = i; _walls [_nwall]._y1 = j; 	
				_data[i][j]='+';
				int jTemp = j+1;
				while(mapTemp[i][jTemp]!='+'){
					_data[i][jTemp]=1;
					if(mapTemp[i][jTemp]=='a'){
						_picts [_npicts]._ntex = 0;
						_picts [_npicts]._x1 = i; _picts [_npicts]._y1 = jTemp;
						_picts [_npicts]._x2 = i; _picts [_npicts]._y2 = jTemp+2;
						++_npicts;
						//cout << "pic1 ajoute "<<i<<' '<<j<<endl ;
					}
					if(mapTemp[i][jTemp]=='b'){
						char	tmp [128];
						sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
						_picts [_npicts]._ntex = wall_texture (tmp);
						_picts [_npicts]._x1 = i; _picts [_npicts]._y1 = jTemp;
						_picts [_npicts]._x2 = i; _picts [_npicts]._y2 = jTemp+2;
						++_npicts;
						//cout << "pic2 ajoute "<<i<<' '<<j<<endl ;
					}
					jTemp++;
				}
				_walls [_nwall]._x2 = i; _walls [_nwall]._y2 = jTemp;	
				_walls [_nwall]._ntex = 0;	
				_data[i][jTemp]='+';
				//cout << "heng wall ajoute ("<<_walls [_nwall]._x1 << ","<<_walls [_nwall]._y1<<") ("<<_walls [_nwall]._x2 << ","<<_walls [_nwall]._y2<<")"<<endl ;
				_nwall++;
				
			}	
			if(mapTemp[i][j]==' '||mapTemp[i][j]=='\n'||mapTemp[i][j]=='C'){
				_data[i][j]=0;
			}		
		}
	}

} 
