/***************************************************************************************************************
AUTEUR : Nahuel Londono
DATE: 12 février 2017
NOM DU PROGRAMME : 420-B21-TP1-Nahuel-Londono.cpp
DESCRIPTION : Programme permettant à l'utilisateur de joeur à un jeu où est-ce qu'il déplace le curseur avec les 
			  flèches du clavier, lui permettant de se deplacer sur des cases, lesquelles parfois montrent des
			  signes d'argent après avoir été dessus. Le but du jeu étant de ramasser 15 points soit 15 cases 
			  avec des signes d'argent. Le joueur perd lorsqu'il ne peut plus se déplacer.
****************************************************************************************************************/

// LIBRARIES
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <string>
#include "C:\cvm.h"
#include <time.h>
#include "C:\console.h"

using namespace std;

// CONSTANTES
const int NB_COL = 12, NB_LIG = 8,
		  TAB1 = 7, TAB2 = 13, TAB3 = 17;
const int KEY_DOWN				= 80;
const int KEY_UP				= 72;
const int KEY_RIGHT				= 77;
const int KEY_LEFT				= 75;
const int KEY_UP_RIGHT			= 73;
const int KEY_UP_LEFT			= 71;
const int KEY_DOWN_LEFT			= 79;
const int KEY_DOWN_RIGHT		= 81;	// Code des touches de déplacement valides

// DEFINITIONS DES TYPES
struct Coord // Pour représenter une position valide dans le Matrice Mat
{
	int row;
	int col;
} posMat;

struct Move // Pour représenter un déplacement dans la matrice
{
	Coord f; // From
	Coord t; // To
} depMat;

enum State { WH, HM, MN, GR, WA }; 

// VARIABLES GLOBALES
int points = 0,
	posCurseurX = 10, posCurseurY = 2,
	x = 0, y = 0,
	charactereEtat = 178,
	totalDeplacements,
	toucheFleche, casesVidesEnvironnant;

cvm_color couleur;

unsigned long tempsDebutJeu, tempsFinJeu, tempsTotal;

bool gameOver = false;

State board[NB_LIG][NB_COL] =					// Damier du jeu avec les états initiaux :
{
	{ WH , WH , WH , WH , WH , WH , WH , WH , WA , WH , WH , HM	},
	{ WH , WH , WA , WA , WH , WH , WH , WH , WH , WA , WH , WA },
	{ WH , WH , WA , HM , WA , WH , WH , WH , WH , WH , WA , HM },
	{ WH , WH , WA , HM , WA , WH , WH , WA , WA , WH , WA , HM },
	{ HM , WH , WA , WA , WA , HM , WA , WH , WA , WH , WA , WH },
	{ HM , WH , HM , HM , WH , HM , WA , HM , WA , WH , WA , WH },
	{ HM , WH , HM , WH , WH , WH , WA , WA , WA , WH , WA , WH },
	{ HM , HM , WH , WH , WH , WH , WH , WH , WH , WH , WH , WH }
};

// FONCTIONS
void affichageEnHautDuJeu()		// Affiche les explications du jeu et le nb de points (en haut)
{
	gotoxy(0, 0);
	cout << "Decouvrez et amassez 15 cases $$$$" << setw(44) << right << "Points: " << points << endl;
	cout << "         ";
	x = wherex();
	y = wherey();
}

char definitionCharactereEtat(int _ligne, int _colonne)		// Defini les characteres d'affichage des cases selon leur état
{
	char valeurCharactere; 

	switch (board[_ligne][_colonne])
	{
		case 0:	valeurCharactere = 178; break;
		case 1: valeurCharactere = 178; break;
		case 2: valeurCharactere = '$'; break;
		case 3: valeurCharactere = 176; break;
		case 4: valeurCharactere = ' '; break;
	}

	return valeurCharactere;
}

cvm_color definitionCouleurCase(int _ligne, int _colonne)		// Defini la couleur des cases selon leur état
{
	cvm_color valeurCouleurCase = BLANC;

	switch (board[_ligne][_colonne])
	{
		case 0:	valeurCouleurCase = BLEU; break;
		case 1: valeurCouleurCase = BLEU; break;
		case 2: valeurCouleurCase = VERT; break;
		case 3: valeurCouleurCase = ROSE; break;
	}

	return valeurCouleurCase;
}

void afficheEtatCase(char _char)		// Affichage de l'état de cases
{
	x = wherex();
	y = wherey();
	cout << char(_char) << char(_char) << char(_char) << char(_char);
	gotoxy(x, y + 1);
	cout << char(_char) << char(_char) << char(_char) << char(_char);
	cout << " ";
	gotoxy(x + 5, y);
}

void afficheDamier()	// Affiche le damier
{
	for (int colonne = 0; colonne < NB_COL; colonne++)
	{
		for (int ligne = 0; ligne < NB_LIG; ligne++)
		{
			gotoxy((colonne * 5) + 10, (ligne * 3) + 2);

			couleur = definitionCouleurCase(ligne, colonne);
			cvm_SetColor(couleur);

			charactereEtat = definitionCharactereEtat(ligne, colonne);
			afficheEtatCase(charactereEtat);
		}
	}
	cvm_ResetColor();
}

void afficheCurseur(int _posCurseurX, int _posCurseurY)		// Affiche le curseur
{
	cvm_SetColor(JAUNE);
	gotoxy(_posCurseurX, _posCurseurY);
	cout << char(201) << char(203) << char(203) << char(187);
	gotoxy(_posCurseurX, _posCurseurY + 1);
	cout << char(200) << char(202) << char(202) << char(188);
	cvm_ResetColor();

}

Coord ajustementDesPositions(int x, int y, int touche)
{
	Coord ajustementPosMat;

	switch (touche)
	{
		case KEY_UP_LEFT: y -= 1; x -= 1; break;
		case KEY_UP: y -= 1; break;
		case KEY_UP_RIGHT: y -= 1; x += 1; break;
		case KEY_LEFT: x -= 1; break;
		case KEY_RIGHT: x += 1; break;
		case KEY_DOWN_LEFT: y += 1; x -= 1; break;
		case KEY_DOWN: y += 1;  break;
		case KEY_DOWN_RIGHT: y += 1; x += 1; break;
		default: break;
	}

	ajustementPosMat.col = x;
	ajustementPosMat.row = y;

	return ajustementPosMat;
}

void calculPosMat()	// Affectation de la position de la matrice selon l'input
{
	depMat.f.row = posMat.row;		//Sauvegarde la position intiiale
	depMat.f.col = posMat.col;

	posMat = ajustementDesPositions(posMat.col, posMat.row, toucheFleche);
}

void validationTouche()		// Valide l'input (touche)
{
	int _x = wherex(); int _y = wherey();
	int charCommande = 0;
	charCommande = _getch();
	gotoxy(_x, _y);
	if (charCommande == 0 || charCommande == 224)
		if (_kbhit())
		{
			toucheFleche = _getch();   
		}
		else
			return;
}

bool validationDeplacement()	// Valide le déplacement
{
	Coord newPos = ajustementDesPositions(posMat.col, posMat.row, toucheFleche);

	if (newPos.row < 0 || newPos.row > 7)		// Empeche de sortir de la matrice
		return false;
	
	if (newPos.col < 0 || newPos.col > 11)
		return false;

	if (board[newPos.row][newPos.col] == 4)		// V'rifie si la case est d'état WA soit une case vide 
		return false;

	return true;
}

void deplacement()		// Calcul du déplacement
{
	depMat.t.col = (posMat.col * 5) + 10;		// Nouvelle position dans la matrice
	depMat.t.row = (posMat.row * 3) + 2;

	posCurseurX = (posMat.col * 5) + 10;		// Deplacement du curseur
	posCurseurY = (posMat.row * 3) + 2;
}

void modifState()		// Modification des états de la matrice du damier
{
	switch (board[depMat.f.row][depMat.f.col])
	{
		case 0: board[depMat.f.row][depMat.f.col] = GR; break;
		case 1: board[depMat.f.row][depMat.f.col] = MN; break;
		case 2: board[depMat.f.row][depMat.f.col] = GR; break;
		case 3: board[depMat.f.row][depMat.f.col] = WA; break;
	}
}

void calculPoints()		// Calcul les points du joueur
{
	if (board[posMat.row][posMat.col] == MN)
		points++;
}

int calculCasesVides()		// Analyse s'il y a des cases vides à l'entour de la position présente dans la matrice
{
	casesVidesEnvironnant = 0;
	for (int _ligne = -1; _ligne <= 1; _ligne++)
	{
		for (int _colonne = -1; _colonne <= 1; _colonne++)
		{
			int v = posMat.row + _ligne;			//Soit v et w des valeurs de positions de la matrice
			int w = posMat.col + _colonne;

			if (v < 0 || v > 7)		// Définition des limites comme des cases vides
			{
				casesVidesEnvironnant++;
				continue;
			}
			if (w < 0 || w > 11)
			{
				casesVidesEnvironnant++;
				continue;
			}
			if (board[v][w] == WA)		// Une case d'état WA est considérée comme une case vide
				casesVidesEnvironnant++;
		}
	}
	return casesVidesEnvironnant;
} 


bool perdreLeJeu(int _casesVidesEnvironnant)		// Analyse si le joueur perd le jeu ou non selon le nb de cases vides à l'entour
{
	if (_casesVidesEnvironnant >= 8)
		gameOver = true;
	return gameOver;
} 

void outputFinJeu()		// Message de fin de jeu
{
	clrscr();
	cout << endl;

	if (points < 15)
		cout << "\220CHEC !";
	else
		cout << "VICTOIRE !";

	cout << endl << endl;
	cout << "  Total des points" << setw(TAB2) << right << ": " << points << " sur un objectif de 15" << endl << endl;
	cout << "  Total des d\202placements" << setw(TAB1) << right << ": " << totalDeplacements << endl << endl;
	cout << "  Temps \202coul\202" << setw(TAB3) << right << ": " << tempsTotal << " sec" << endl << endl;
}

// FONCTION MAIN
void main(void)
{
	bool deplacementValide;

	tempsDebutJeu = time(NULL);
	do
	{
		affichageEnHautDuJeu();
		afficheDamier();
		afficheCurseur(posCurseurX, posCurseurY);
		do
		{
			validationTouche();
			deplacementValide = validationDeplacement();
			if (deplacementValide == false)
				cout << "\a";
		} 
		while (deplacementValide == false);
		calculPosMat();
		deplacement();	
		modifState();
		calculPoints();
		calculCasesVides();
		perdreLeJeu(casesVidesEnvironnant);
		totalDeplacements++;
	} 
	while (points < 15 && gameOver == false);
		
	// FIN DU JEU
	tempsFinJeu = time(NULL);
	tempsTotal = tempsFinJeu - tempsDebutJeu;

	outputFinJeu();

	_getch();
}