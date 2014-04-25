#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "jeu.h"

/* Initialise le jeu */
jeu *initJeu(int blanc, int noir){
	jeu *j = malloc(sizeof(jeu));
	j->list = initListe();
	j->starter = initListe();
	j->joueur = 'b';
	j->blanc = blanc;
	j->noir = noir;
	j->tour = 1;
	j->coups = initListeH();
	initPlateau(j->list);
	initPlateau(j->starter);
	return j;
}

/* Free du jeu */
void freeJeu(jeu *j){
	if(j){
		freeListe(j->list);
		freeListe(j->starter);
		freeListeH(j->coups);
	}
	free(j);
}

/* Boucle du jeu */
void startJeu(jeu *j){
	char sortie = 0, x, y, a, b, a1, b1, a2, b2, commencePar, erreur = 1, *vide;
	char nomSauvegarde[100];
	char input[20];
	int victoire = 0, coupsSucces = 0;
	while(sortie != 1 && victoire < 2){
		printPlateau(j);
		erreur = 1;
		coupsSucces = 0;
		do{
			/* Tour IA */
			if((j->joueur == 'b' && j->blanc > 1) || (j->joueur == 'n' && j->noir > 1)){
				strcpy(input, jouerIA(j));
			/* Tour humain */
			}else{
				printf("Quelle pièce souhaitez vous déplacer ? ");
				fgets(input, sizeof(input), stdin);
				vide = strchr(input, '\n');
				if(vide) *vide = 0;
			}
			
			if(strlen(input) == 1 && input[0] == 'q'){
				erreur = 0;
				sortie = 1;
			}else if(strlen(input) == 1 && input[0] == 'c'){
				printf("Historique des coups :\n");
				printListeH(j->coups);
			}else if(strlen(input) == 1 && input[0] == 'r'){
				printf("Annulation du dernier coups joué\n");
				removeLastH(j->coups);
				jouerHistorique(j);
				j->joueur = (j->joueur == 'b')? 'n' : 'b';
				erreur = 0;
			}else if(strlen(input) == 1 && input[0] == 'h'){
				printf("v: heuristique du dernier coups\nc : historique des coups\nr : annuler dernier coups\ns : sauvegarder l'historique des coups\np : sauvegarder le plateau\nq : quitter\n");
			}else if(strlen(input) == 1 && input[0] == 's'){
				printf("[ATTENTION] Sauvegarder un historique de coups d'une configuration de départ personnalisée ne pourra être rejoué que avec cette configuration\n");
				printf("Nom du fichier de sauvegarde ? ");
				fgets(nomSauvegarde, sizeof(nomSauvegarde), stdin);
				vide = strchr(nomSauvegarde, '\n');
				if(vide) *vide = 0;
				sauvegarderHistorique(j, nomSauvegarde);
				printf("Sauvegarde terminée.\n");
			}else if(strlen(input) == 1 && input[0] == 'p'){
				printf("Nom du fichier de sauvegarde ? ");
				fgets(nomSauvegarde, sizeof(nomSauvegarde), stdin);
				vide = strchr(nomSauvegarde, '\n');
				if(vide) *vide = 0;
				sauvegarderPlateau(j, nomSauvegarde);
				printf("Sauvegarde terminée.\n");
			}else if(strlen(input) == 1 && input[0] == 'v'){
				printf("Heuristique du dernier coup : %d\n", evaluationPlateau(j));
			}else if(estMouvement(input, j->joueur)){
				x = input[0];
				y = input[1];
				a = input[3];
				b = input[4];
				printf("Déplacement de (%c,%c) en (%c,%c) : ", x, y, a, b);
				if(estPieceDuJoueur(j->list, x, y, j->joueur) && deplaValide(j->list, j->joueur, x, y, a, b)){
					victoire = deplaPiece(j->list, x, y, a, b);
					coupsSucces = 1;
					if(victoire){
						printf("[succès][%d]", victoire);
						erreur = 0;
					}
				}else{
					printf("[impossible]");
				}
				printf("\n");
			}else if(estDeploiementDouble(input, j->joueur)){
				x = input[0];
				y = input[1];
				a1 = input[3];
				b1 = input[4];
				a = input[6];
				b = input[7];
				commencePar = input[2];
				printf("Déploiement de (%c,%c) en (%c,%c) commençant par les %s en (%c,%c)\n", x, y, a, b, (commencePar == '+')? "carrés" : "ronds", a1, b1);
				if(estPieceDuJoueur(j->list, x, y, j->joueur) && deploValide(j->list, j->joueur, commencePar, x, y, a, b, a1, b1)){
					victoire = deploPieceDouble(j->list, j->joueur, commencePar, x, y, a, b, a1, b1);
					coupsSucces = 1;
					if(victoire){
						printf("[succès][%d]", victoire);
						erreur = 0;
					}
				}else{
					printf("[impossible]");
				}
				printf("\n");
			}else if(estDeploiementTriple(input, j->joueur)){
				x = input[0];
				y = input[1];
				a1 = input[3];
				b1 = input[4];
				a2 = input[6];
				b2 = input[7];
				a = input[9];
				b = input[10];
				commencePar = input[2];
				printf("Déploiement de (%c,%c) en (%c,%c) commençant par les %s en (%c,%c)(%c,%c)\n", x, y, a, b, (commencePar == '+')? "carrés" : "ronds", a1, b1, a2, b2);
				if(estPieceDuJoueur(j->list, x, y, j->joueur) && deploValide(j->list, j->joueur, commencePar, x, y, a, b, a1, b1)){
					coupsSucces = 1;
					victoire = deploPieceTriple(j->list, j->joueur, commencePar, x, y, a, b, a1, b1, a2, b2);
					if(victoire){
						printf("[succès][%d]", victoire);
						erreur = 0;
					}
				}else{
					printf("[impossible]");
				}
				printf("\n");
			}else{
				printf("Mauvaises coordonnées !\n");
			}
		}while(erreur);
		if(coupsSucces) addListeH(j->coups, input);
		j->joueur = (j->joueur == 'b')? 'n' : 'b';
		if(testVictoireAucunePiece(j->list, j->joueur) == 1) victoire = (j->joueur == 'b')? 3 : 2;
	}
	if(victoire > 1){
		if(victoire == 2) printf("Victoire du joueur Blanc !\n");
		else if(victoire == 3) printf("Victoire du joueur Noir !\n");
		else printf("Le jeu s'est terminé sans victoire.\n");
	}
	printf("Fermeture du jeu\n");
}

/* Test si la chaîne de caractère est un mouvement */
int estMouvement(char input[], char couleur){
	return strlen(input) == 5 
		&& input[0] >= 'a' && input[0] <= 'h'
		&& input[1] >= '1' && input[1] <= '8'
		&& input[2] == '-'
		&& input[3] >= 'a' && input[3] <= 'h'
		&& ((couleur == 'n' && input[4] >= '0') || (input[4] >= '1')) && ((couleur == 'b' && input[5]  <= '9') || (input[5] <= '8'));
}

/* Test si la chaîne de caractère est un déploiement double */
int estDeploiementDouble(char input[], char couleur){
	return strlen(input) == 8 
		&& input[0] >= 'a' && input[0] <= 'h'
		&& input[1] >= '1' && input[1] <= '8'
		&& (input[2] == '+' || input[2] == '*')
		&& input[3] >= 'a' && input[3] <= 'h'
		&& ((couleur == 'n' && input[4] >= '0') || (input[4] >= '1')) && ((couleur == 'b' && input[5]  <= '9') || (input[5] <= '8'))
		&& input[5] == '-'
		&& input[6] >= 'a' && input[6] <= 'h'
		&& ((couleur == 'n' && input[7] >= '0') || (input[7] >= '1')) && ((couleur == 'b' && input[7]  <= '9') || (input[7] <= '8'));
}

/* Test si la chaîne de caractère est un déploiement triple */
int estDeploiementTriple(char input[], char couleur){
	return strlen(input) == 11 
		&& input[0] >= 'a' && input[0] <= 'h'
		&& input[1] >= '1' && input[1] <= '8'
		&& (input[2] == '+' || input[2] == '*')
		&& input[3] >= 'a' && input[3] <= 'h'
		&& ((couleur == 'n' && input[4] >= '0') || (input[4] >= '1')) && ((couleur == 'b' && input[5]  <= '9') || (input[5] <= '8'))
		&& input[5] == '-'
		&& input[6] >= 'a' && input[6] <= 'h'
		&& ((couleur == 'n' && input[7] >= '0') || (input[7] >= '1')) && ((couleur == 'b' && input[7]  <= '9') || (input[7] <= '8'))
		&& input[8] == '-'
		&& input[9] >= 'a' && input[9] <= 'h'
		&& ((couleur == 'n' && input[10] >= '0') || (input[10] >= '1')) && ((couleur == 'b' && input[10]  <= '9') || (input[10] <= '8'));
}

/* Test si la pièce sur la case x,y est une pièce appartenant au joueur de la couleur donnée */
int estPieceDuJoueur(liste *l, char x, char y, char couleur){
	piece *p = getPieceByCoordListe(l, x, y);
	if(p == NULL){
		printf("Aucune pièce sur cette case (%c,%c)\n", x, y);
		return 0;
	}else if(p->couleur != couleur){
		printf("La pièce est pas de la bonne couleur\n");
		return 0;
	}
	return 1;
}

/* Test s'il n'y a plus de pièce de la couleur sur le plateau de jeu */
int testVictoireAucunePiece(liste *l, char couleur){
	noeud *courant;
	courant = l->first;
	while(courant != NULL){
		if(courant->p->couleur == couleur) return 0;
		courant = courant->next;
	}
	return 1;
}

/* Rejoue l'historique des coups à partir de la configuration de départ */
void jouerHistorique(jeu *j){
	noeudH *courant;
	noeud *starterC;
	int victoire;
	
	freeListe(j->list);
	j->list = initListe();
	j->joueur = 'b';
	j->tour = 1;
	
	starterC = j->starter->first;
	while(starterC != NULL){
		addListe(j->list, initPiece(starterC->p->x, starterC->p->y, starterC->p->couleur, starterC->p->t));
		starterC = starterC->next;
	}
	
	courant = j->coups->first;
	while(courant != NULL){
		if(estMouvement(courant->c, j->joueur)){
			if(estPieceDuJoueur(j->list, courant->c[0], courant->c[1], j->joueur) && deplaValide(j->list, j->joueur, courant->c[0], courant->c[1], courant->c[3], courant->c[4])){
				victoire = deplaPiece(j->list, courant->c[0], courant->c[1], courant->c[3], courant->c[4]);
			}else{
				printf("Erreur : mouvement impossible %s\n", courant->c);
				printListeH(j->coups);
				exit(1);
			}
		}else if(estDeploiementDouble(courant->c, j->joueur)){
			if(estPieceDuJoueur(j->list, courant->c[0], courant->c[1], j->joueur) && deploValide(j->list, j->joueur, courant->c[2], courant->c[0], courant->c[1], courant->c[6], courant->c[7], courant->c[3], courant->c[4])){
				victoire = deploPieceDouble(j->list, j->joueur, courant->c[2], courant->c[0], courant->c[1], courant->c[6], courant->c[7], courant->c[3], courant->c[4]);
			}else{
				printf("Erreur : mouvement impossible %s\n", courant->c);
				exit(1);
			}
		}else if(estDeploiementTriple(courant->c, j->joueur)){
			if(estPieceDuJoueur(j->list, courant->c[0], courant->c[1], j->joueur) && deploValide(j->list, j->joueur, courant->c[2], courant->c[0], courant->c[1], courant->c[9], courant->c[10], courant->c[3], courant->c[4])){
				victoire = deploPieceTriple(j->list, j->joueur, courant->c[2], courant->c[0], courant->c[1], courant->c[9], courant->c[10], courant->c[3], courant->c[4], courant->c[6], courant->c[7]);
			}else{
				printf("Erreur : mouvement impossible %s\n", courant->c);
				exit(1);
			}
		}else{
			printf("ERREUR : %s n'est pas un coups reconnu", courant->c);
			exit(1);
		}
		
		if(victoire > 1){
			if(victoire == 2) printf("Victoire du joueur Blanc !\n");
			else if(victoire == 3) printf("Victoire du joueur Noir !\n");
			else printf("Le jeu s'est terminé sans victoire.\n");
			return;
		}
		
		j->joueur = (j->joueur == 'b')? 'n' : 'b';
		if(j->joueur == 'b') j->tour++;
		courant = courant->next;
	}
}

/* Charge un historique de coups situé dans le fichier en paramètre */
void chargerFichierTest(jeu *j, char *nomFichier){
	FILE *file;
	char * ligne = NULL;
	size_t len = 0;
	
	file = fopen(nomFichier, "r");
	if(file == NULL){
		printf("Impossible d'ouvrir le fichier\n");
		exit(1);
	}
	
	while(getline(&ligne, &len, file) != -1){
		ligne[strlen(ligne)-1] = '\0';
		addListeH(j->coups, ligne);
	}
	
	if(ligne) free(ligne);
	jouerHistorique(j);
	fclose(file);
}

/* Sauvegarde un historique de coups dans le fichier en paramètre */
void sauvegarderHistorique(jeu *j, char *nomFichier){
	FILE *file;
	noeudH *courant;
	
	file = fopen(nomFichier, "w");
	if(file == NULL){
		printf("Impossible d'ouvrir le fichier\n");
		return;
	}
	
	courant = j->coups->first;
	while(courant != NULL){
		fprintf(file, "%s\n", courant->c);
		courant = courant->next;
	}
	
	fclose(file);
}

/* Charge une configuration de départ situé dans le fichier en paramètre */
void chargerFichierPlateau(jeu *j, char *nomFichier){
	FILE *file;
	char *ligne = NULL, x, y;
	type t;
	size_t len = 0;
	
	freeListe(j->list);
	freeListe(j->starter);
	j->list = initListe();
	j->starter = initListe();
	
	file = fopen(nomFichier, "r");
	if(file == NULL){
		printf("Impossible d'ouvrir le fichier\n");
		exit(1);
	}
	
	x = 'a';
	y = '1';
	while(getline(&ligne, &len, file) != -1){
		ligne[strlen(ligne)-1] = '\0';
		if(strlen(ligne) == 2){
			switch(ligne[1]){
				case '0':
					t = vide; break;
				case '1':
					t = carre; break;
				case '2':
					t = rond; break;
				case '3':
					t = ccarre; break;
				case '4':
					t = rrond; break;
				case '5':
					t = cccarre; break;
				case '6':
					t = rrrond; break;
				case '7':
					t = crond; break;
				case '8':
					t = ccrond; break;
				case '9':
					t = crrond; break;
				default:
					printf("Aucune pièce correspond à %c\n", ligne[1]);
					exit(1);
					break;
			}
			if(ligne[0] == 'B'){
				addListe(j->list, initPiece(x, y, 'b', t));
				addListe(j->starter, initPiece(x, y, 'b', t));
			}else if(ligne[0] == 'N'){
				addListe(j->list, initPiece(x, y, 'n', t));
				addListe(j->starter, initPiece(x, y, 'n', t));
			}
		}else{
			printf("La ligne '%s' n'est pas au bon format\n", ligne);
			exit(1);
		}
		
		x++;
		if(x > 'h'){
			x = 'a';
			y++;
		}
	}
	
	fclose(file);
}

/* Sauvegarde le plateau de jeu dans le fichier en paramètre */
void sauvegarderPlateau(jeu *j, char *nomFichier){
	FILE *file;
	char x, y;
	piece *p;
	
	file = fopen(nomFichier, "w");
	if(file == NULL){
		printf("Impossible d'ouvrir le fichier\n");
		return;
	}
	
	for(y = '1'; y <= '8'; y++){
		for(x = 'a'; x <= 'h'; x++){
			p = getPieceByCoordListe(j->list, x, y);
			if(p == NULL) fprintf(file, "V0\n");
			else fprintf(file, "%c%d\n", ((p->couleur == 'b')? 'B':'N'), p->t);
		}
	}
	
	fclose(file);
}

/* Initialise le plateau de jeu dans ca configuration classique */
void initPlateau(liste *l){
	char colonne;
	int impair = carre, pair = rond;
	for(colonne = 'a'; colonne <= 'b'; colonne++){
		piece *p1 = initPiece(colonne, '1', 'b', impair);
		piece *p2 = initPiece(colonne, '2', 'b', pair);
		piece *p3 = initPiece(colonne, '7', 'n', impair);
		piece *p4 = initPiece(colonne, '8', 'n', pair);
		/*addListe(l, p1);*/
		addListe(l, p2);
		/*addListe(l, p3);*/
		addListe(l, p4);
		impair = (impair == carre)? rond : carre;
		pair = (pair == carre)? rond : carre;
	}
}

/* Affiche le plateau sur la sortie standard */
void printPlateau(jeu *j){
	char colonne, ligne;
	piece *p;
	printf("     a  b  c  d  e  f  g  h\n");
	printf("    -------------------------     TOUR %d (%s)\n", j->coups->length/2+1, (j->joueur == 'b')? "blanc" : "noir");
	for(ligne = '8'; ligne >= '1'; ligne--){
		printf(" %c | ", ligne);
		for(colonne = 'a'; colonne <= 'h'; colonne++){
			p = getPieceByCoordListe(j->list, colonne, ligne);
			if(p)
				printf("%c%d ", p->couleur, p->t);
			else
				printf(" . ");
		}
		printf("|");
		if(ligne == '7') printf("    1: c    2: r");
		if(ligne == '6') printf("    3: cc   4: rr");
		if(ligne == '5') printf("    5: ccc  6: rrr");
		if(ligne == '4') printf("    7: cr   8: ccr");
		if(ligne == '3') printf("    9: crr");
		if(ligne == '1') printf("    h: liste des commandes");
		printf("\n");
	}
	printf("    -------------------------\n");
}

/* Fait jouer l'IA selon sont niveau de difficulté */
char* jouerIA(jeu *j){
	char valeur;
	valeur = (j->joueur == 'b')? j->blanc : j->noir;
	if(valeur == 2) return randomIA(j);
	else if(valeur == 3) return meilleurCoups(j);
	else if(valeur == 4) return minimaxIA(j, 4);
	return NULL;
}

/* Selectionne un coup à jouer aléatoirement */
char* randomIA(jeu *j){
	int r, i, cas = 0;
	noeud *courant;
	noeudC *courantC;
	listeC *deplacement;
	listeC *deploiement;
	piece *p;
	char *coup = malloc(sizeof(char)*20);
	srand(time(NULL));
	
	while(cas == 0){
		do{
			r = rand() % j->list->length;
			courant = j->list->first;
			for(i = 0; i < r; i++) courant = courant->next;
			p = courant->p;
			if(p->couleur != j->joueur) p = NULL;
		}while(p == NULL);
		
		coup[0] = p->x;
		coup[1] = p->y;
		
		deplacement = deplaCasesPossibles(j->list, p->t, j->joueur, p->x, p->y);
		deploiement = deploCasesPossibles(j->list, p->t, j->joueur, p->x, p->y);
		
		if(deplacement->length == 0 && deploiement->length == 0) cas = 0;
		else if(deplacement->length > 0 && deploiement->length == 0) cas = 1;
		else if(deplacement->length == 0 && deploiement->length > 0) cas = 2;
		else{
			r = rand() %2;
			cas = r+1;
		}
		
		if(cas == 1){
			r = rand() % deplacement->length;
			courantC = deplacement->first;
			for(i = 0; i < r; i++) courantC = courantC->next;
			coup[2] = '-';
			coup[3] = courantC->c->x;
			coup[4] = courantC->c->y;
			coup[5] = '\0';
			return coup;
		}
		
		if(cas == 2){
			r = rand() % deploiement->length;
			courantC = deploiement->first;
			for(i = 0; i < r; i++) courantC = courantC->next;
			if(p->t == ccarre || p->t == cccarre || p->y == courantC->c->y1 || p->x == courantC->c->x1) coup[2] = '+';
			else coup[2] = '*';
			coup[3] = courantC->c->x1;
			coup[4] = courantC->c->y1;
			coup[5] = '-';
			
			if(p->t == ccarre || p->t == rrond || p->t == crond){
				coup[6] = courantC->c->x;
				coup[7] = courantC->c->y;
				coup[8] = '\0';
			}else{
				coup[6] = courantC->c->x2;
				coup[7] = courantC->c->y2;
				coup[8] = '-';
				coup[9] = courantC->c->x;
				coup[10] = courantC->c->y;
				coup[11] = '\0';
			}
			return coup;
		}
	}
	return NULL;
}

/* Heuristique */
int evaluationPlateau(jeu *j){
	int valeur = 0;
	noeud *courant;
	
	courant = j->list->first;
	while(courant != NULL){
		if(courant->p->couleur == j->joueur){
			/* test victoire */
			if((courant->p->couleur == 'b' && courant->p->y == '9') || (courant->p->couleur == 'n' && courant->p->y == '0')){
				valeur = INT_MAX;
				return valeur;
			}
			
			/* valeur piece */
			switch(courant->p->t){
				case carre:
				case rond:
					valeur += 1; break;
				case ccarre:
				case rrond:
				case crond:
					valeur += 4; break;
				case cccarre:
				case rrrond:
					valeur += 16; break;
				case ccrond:
				case crrond:
					valeur += 32; break;
				case vide:
				default:
					break;
			}
		}
		courant = courant->next;
	}
	
	return valeur;
}

/* Recherche le meilleur coups parmis tous les coups possibles de la couleur selon l'heuristique */
char* meilleurCoups(jeu *j){
	char *coups, commencePar, *coupsTmp;
	int valeurMax = 0, tmp;
	listeC *coordCouleur;
	listeC *deplaPossibles;
	listeC *deploPossibles;
	noeud *courantP;
	noeudC *courantDepla, *courantDeplo, *coordC;
	piece *tmpPiece;
	
	coups = malloc(sizeof(char)*20);
	coupsTmp = malloc(sizeof(char)*20);
	
	coordCouleur = initListeC();
	courantP = j->list->first;
	while(courantP != NULL){
		if(courantP->p->couleur == j->joueur) addListeC(coordCouleur, initCoord(courantP->p->x, courantP->p->y));
		courantP = courantP->next;
	}
	
	/* toutes les pièces */
	coordC = coordCouleur->first;
	while(coordC != NULL){
		tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
		if(tmpPiece == NULL) break;
		deplaPossibles = deplaCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		deploPossibles = deploCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		
		/* tous les deplacements de la pièce */
		courantDepla = deplaPossibles->first;
		while(courantDepla != NULL){
			if(deplaPiece(j->list, coordC->c->x, coordC->c->y, courantDepla->c->x, courantDepla->c->y)){
				coupsTmp[0] = coordC->c->x;
				coupsTmp[1] = coordC->c->y;
				coupsTmp[2] = '-';
				coupsTmp[3] = courantDepla->c->x;
				coupsTmp[4] = courantDepla->c->y;
				coupsTmp[5] = 0;
				tmp = evaluationPlateau(j);
				printf("COUPS [%s] = %d\n", coupsTmp, tmp);
				if(tmp > valeurMax){
					valeurMax = tmp;
					strcpy(coups, coupsTmp);
				}
				addListeH(j->coups, coupsTmp);
				j->joueur = (j->joueur == 'b')? 'n':'b';
				removeLastH(j->coups);
				jouerHistorique(j);
			}
			courantDepla = courantDepla->next;
		}
		
		/* tous les déploiements de la pièce */
		courantDeplo = deploPossibles->first;
		while(courantDeplo != NULL){
			/* déploiement double */
			if(courantDeplo->c->x2 == 0 && courantDeplo->c->y2 == 0){
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == ccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceDouble(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x;
					coupsTmp[7] = courantDeplo->c->y;
					coupsTmp[8] = 0;
					tmp = evaluationPlateau(j);
					printf("COUPS [%s] = %d\n", coupsTmp, tmp);
					if(tmp > valeurMax){
						valeurMax = tmp;
						strcpy(coups, coupsTmp);
					}
					addListeH(j->coups, coupsTmp);
					j->joueur = (j->joueur == 'b')? 'n':'b';
					removeLastH(j->coups);
					jouerHistorique(j);
				}
			/* déploiement triple */
			}else{
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == cccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceTriple(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1, courantDeplo->c->x2, courantDeplo->c->y2)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x2;
					coupsTmp[7] = courantDeplo->c->y2;
					coupsTmp[8] = '-';
					coupsTmp[9] = courantDeplo->c->x;
					coupsTmp[10] = courantDeplo->c->y;
					coupsTmp[11] = 0;
					tmp = evaluationPlateau(j);
					printf("COUPS [%s] = %d\n", coupsTmp, tmp);
					if(tmp > valeurMax){
						valeurMax = tmp;
						strcpy(coups, coupsTmp);
					}
					addListeH(j->coups, coupsTmp);
					j->joueur = (j->joueur == 'b')? 'n':'b';
					removeLastH(j->coups);
					jouerHistorique(j);
				}
			}
			courantDeplo = courantDeplo->next;
		}
		
		coordC = coordC->next;
	}
	printf("MEILLEUR COUP %d : %s\n", valeurMax, coups);
	return coups;
}

/* Minimax */
int minimaxMax(jeu *j, int profondeur){
	char *coups, commencePar, *coupsTmp;
	int valeurMax = INT_MIN, tmp, i;
	listeC *coordCouleur;
	listeC *deplaPossibles;
	listeC *deploPossibles;
	noeud *courantP;
	noeudC *courantDepla, *courantDeplo, *coordC;
	piece *tmpPiece;
	
	if(j->coups->last->c[strlen(j->coups->last->c)-2] == '0' || j->coups->last->c[strlen(j->coups->last->c)-2] == '9' || testVictoireAucunePiece(j->list, j->joueur)){
		valeurMax = INT_MAX;
		printf("VICT\n");
		return valeurMax;
	}else if(profondeur == 0){
		return evaluationPlateau(j);
	}
	
	coups = malloc(sizeof(char)*20);
	coupsTmp = malloc(sizeof(char)*20);
	
	coordCouleur = initListeC();
	courantP = j->list->first;
	while(courantP != NULL){
		if(courantP->p->couleur == j->joueur) addListeC(coordCouleur, initCoord(courantP->p->x, courantP->p->y));
		courantP = courantP->next;
	}
	
	/* toutes les pièces */
	coordC = coordCouleur->first;
	while(coordC != NULL){
		tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
		if(tmpPiece == NULL) break;
		deplaPossibles = deplaCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		deploPossibles = deploCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		
		/* tous les deplacements de la pièce */
		courantDepla = deplaPossibles->first;
		while(courantDepla != NULL){
			if(deplaPiece(j->list, coordC->c->x, coordC->c->y, courantDepla->c->x, courantDepla->c->y)){
				coupsTmp[0] = coordC->c->x;
				coupsTmp[1] = coordC->c->y;
				coupsTmp[2] = '-';
				coupsTmp[3] = courantDepla->c->x;
				coupsTmp[4] = courantDepla->c->y;
				coupsTmp[5] = 0;
			}
			for(i = 0; i < profondeur; i++) printf(">");
			printf("COUPS [%s]\n", coupsTmp);
			addListeH(j->coups, coupsTmp);
			j->joueur = (j->joueur == 'b')? 'n':'b';
			tmp = minimaxMin(j, profondeur-1);
			for(i = 0; i < profondeur; i++) printf(">");
			printf("VALEUR %d\n", tmp);
			printListeH(j->coups);
			
			if(tmp > valeurMax){
				valeurMax = tmp;
				strcpy(coups, coupsTmp);
			}
			
			removeLastH(j->coups);
			jouerHistorique(j);
			courantDepla = courantDepla->next;
		}
		
		/* tous les déploiements de la pièce */
		courantDeplo = deploPossibles->first;
		while(courantDeplo != NULL){
			/* déploiement double */
			if(courantDeplo->c->x2 == 0 && courantDeplo->c->y2 == 0){
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == ccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceDouble(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x;
					coupsTmp[7] = courantDeplo->c->y;
					coupsTmp[8] = 0;
				}
			/* déploiement triple */
			}else{
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == cccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceTriple(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1, courantDeplo->c->x2, courantDeplo->c->y2)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x2;
					coupsTmp[7] = courantDeplo->c->y2;
					coupsTmp[8] = '-';
					coupsTmp[9] = courantDeplo->c->x;
					coupsTmp[10] = courantDeplo->c->y;
					coupsTmp[11] = 0;
				}
			}
			
			for(i = 0; i < profondeur; i++) printf(">");
			printf("COUPS [%s]\n", coupsTmp);
			addListeH(j->coups, coupsTmp);
			j->joueur = (j->joueur == 'b')? 'n':'b';
			tmp = minimaxMin(j, profondeur-1);
			for(i = 0; i < profondeur; i++) printf(">");
			printf("VALEUR %d\n", tmp);
			printListeH(j->coups);
			
			if(tmp > valeurMax){
				valeurMax = tmp;
				strcpy(coups, coupsTmp);
			}
			
			removeLastH(j->coups);
			jouerHistorique(j);
			
			courantDeplo = courantDeplo->next;
		}
		
		coordC = coordC->next;
	}
	return valeurMax;
}

/* Minimax */
int minimaxMin(jeu *j, int profondeur){
	char *coups, commencePar, *coupsTmp;
	int valeurMax = INT_MAX, tmp, i;
	listeC *coordCouleur;
	listeC *deplaPossibles;
	listeC *deploPossibles;
	noeud *courantP;
	noeudC *courantDepla, *courantDeplo, *coordC;
	piece *tmpPiece;
	
	if(j->coups->last->c[strlen(j->coups->last->c)-2] == '0' || j->coups->last->c[strlen(j->coups->last->c)-2] == '9' || testVictoireAucunePiece(j->list, j->joueur)){
		valeurMax = INT_MIN;
		printf("VICT\n");
		return valeurMax;
	}else if(profondeur == 0){
		return evaluationPlateau(j);
	}
	
	coups = malloc(sizeof(char)*20);
	coupsTmp = malloc(sizeof(char)*20);
	
	coordCouleur = initListeC();
	courantP = j->list->first;
	while(courantP != NULL){
		if(courantP->p->couleur == j->joueur) addListeC(coordCouleur, initCoord(courantP->p->x, courantP->p->y));
		courantP = courantP->next;
	}
	
	/* toutes les pièces */
	coordC = coordCouleur->first;
	while(coordC != NULL){
		tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
		if(tmpPiece == NULL) break;
		deplaPossibles = deplaCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		deploPossibles = deploCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		
		/* tous les deplacements de la pièce */
		courantDepla = deplaPossibles->first;
		while(courantDepla != NULL){
			if(deplaPiece(j->list, coordC->c->x, coordC->c->y, courantDepla->c->x, courantDepla->c->y)){
				coupsTmp[0] = coordC->c->x;
				coupsTmp[1] = coordC->c->y;
				coupsTmp[2] = '-';
				coupsTmp[3] = courantDepla->c->x;
				coupsTmp[4] = courantDepla->c->y;
				coupsTmp[5] = 0;
			}
			for(i = 0; i < profondeur; i++) printf(">");
			printf("COUPS [%s]\n", coupsTmp);
			addListeH(j->coups, coupsTmp);
			j->joueur = (j->joueur == 'b')? 'n':'b';
			tmp = minimaxMax(j, profondeur-1);
			for(i = 0; i < profondeur; i++) printf(">");
			printf("VALEUR %d\n", tmp);
			printListeH(j->coups);
			
			if(tmp < valeurMax){
				valeurMax = tmp;
				strcpy(coups, coupsTmp);
			}
			
			removeLastH(j->coups);
			jouerHistorique(j);
			courantDepla = courantDepla->next;
		}
		
		/* tous les déploiements de la pièce */
		courantDeplo = deploPossibles->first;
		while(courantDeplo != NULL){
			/* déploiement double */
			if(courantDeplo->c->x2 == 0 && courantDeplo->c->y2 == 0){
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == ccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceDouble(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x;
					coupsTmp[7] = courantDeplo->c->y;
					coupsTmp[8] = 0;
				}
			/* déploiement triple */
			}else{
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == cccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceTriple(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1, courantDeplo->c->x2, courantDeplo->c->y2)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x2;
					coupsTmp[7] = courantDeplo->c->y2;
					coupsTmp[8] = '-';
					coupsTmp[9] = courantDeplo->c->x;
					coupsTmp[10] = courantDeplo->c->y;
					coupsTmp[11] = 0;
				}
			}
			
			for(i = 0; i < profondeur; i++) printf(">");
			printf("COUPS [%s]\n", coupsTmp);
			addListeH(j->coups, coupsTmp);
			j->joueur = (j->joueur == 'b')? 'n':'b';
			tmp = minimaxMax(j, profondeur-1);
			for(i = 0; i < profondeur; i++) printf(">");
			printf("VALEUR %d\n", tmp);
			printListeH(j->coups);
			
			if(tmp < valeurMax){
				valeurMax = tmp;
				strcpy(coups, coupsTmp);
			}
			
			removeLastH(j->coups);
			jouerHistorique(j);
			
			courantDeplo = courantDeplo->next;
		}
		
		coordC = coordC->next;
	}
	return valeurMax;
}

/* Minimax */
char* minimaxIA(jeu *j, int profondeur){
	char *coups, commencePar, *coupsTmp;
	int valeurMax = INT_MIN, tmp;
	listeC *coordCouleur;
	listeC *deplaPossibles;
	listeC *deploPossibles;
	noeud *courantP;
	noeudC *courantDepla, *courantDeplo, *coordC;
	piece *tmpPiece;
	
	coups = malloc(sizeof(char)*20);
	coupsTmp = malloc(sizeof(char)*20);
	
	coordCouleur = initListeC();
	courantP = j->list->first;
	while(courantP != NULL){
		if(courantP->p->couleur == j->joueur) addListeC(coordCouleur, initCoord(courantP->p->x, courantP->p->y));
		courantP = courantP->next;
	}
	
	/* toutes les pièces */
	coordC = coordCouleur->first;
	while(coordC != NULL){
		tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
		if(tmpPiece == NULL) break;
		deplaPossibles = deplaCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		deploPossibles = deploCasesPossibles(j->list, tmpPiece->t, j->joueur, coordC->c->x, coordC->c->y);
		
		/* tous les deplacements de la pièce */
		courantDepla = deplaPossibles->first;
		while(courantDepla != NULL){
			if(deplaPiece(j->list, coordC->c->x, coordC->c->y, courantDepla->c->x, courantDepla->c->y)){
				coupsTmp[0] = coordC->c->x;
				coupsTmp[1] = coordC->c->y;
				coupsTmp[2] = '-';
				coupsTmp[3] = courantDepla->c->x;
				coupsTmp[4] = courantDepla->c->y;
				coupsTmp[5] = 0;
			}
			printf("COUPS [%s]\n", coupsTmp);
			addListeH(j->coups, coupsTmp);
			j->joueur = (j->joueur == 'b')? 'n':'b';
			tmp = minimaxMin(j, profondeur);
			printf("VALEUR %d\n", tmp);
			
			if(tmp > valeurMax){
				valeurMax = tmp;
				strcpy(coups, coupsTmp);
			}
			
			removeLastH(j->coups);
			jouerHistorique(j);
			
			courantDepla = courantDepla->next;
		}
		
		/* tous les déploiements de la pièce */
		courantDeplo = deploPossibles->first;
		while(courantDeplo != NULL){
			/* déploiement double */
			if(courantDeplo->c->x2 == 0 && courantDeplo->c->y2 == 0){
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == ccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceDouble(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x;
					coupsTmp[7] = courantDeplo->c->y;
					coupsTmp[8] = 0;
				}
			/* déploiement triple */
			}else{
				tmpPiece = getPieceByCoordListe(j->list, coordC->c->x, coordC->c->y);
				if(tmpPiece != NULL && (tmpPiece->t == cccarre || courantDeplo->c->x1 == coordC->c->x || courantDeplo->c->y1 == coordC->c->y)) commencePar = '+';
				else if(tmpPiece != NULL) commencePar = '*';
				else break;
				if(deploPieceTriple(j->list, j->joueur, commencePar, coordC->c->x, coordC->c->y, courantDeplo->c->x, courantDeplo->c->y, courantDeplo->c->x1, courantDeplo->c->y1, courantDeplo->c->x2, courantDeplo->c->y2)){
					coupsTmp[0] = coordC->c->x;
					coupsTmp[1] = coordC->c->y;
					coupsTmp[2] = commencePar;
					coupsTmp[3] = courantDeplo->c->x1;
					coupsTmp[4] = courantDeplo->c->y1;
					coupsTmp[5] = '-';
					coupsTmp[6] = courantDeplo->c->x2;
					coupsTmp[7] = courantDeplo->c->y2;
					coupsTmp[8] = '-';
					coupsTmp[9] = courantDeplo->c->x;
					coupsTmp[10] = courantDeplo->c->y;
					coupsTmp[11] = 0;
				}
			}
			printf("COUPS [%s]\n", coupsTmp);
			addListeH(j->coups, coupsTmp);
			j->joueur = (j->joueur == 'b')? 'n':'b';
			tmp = minimaxMin(j, profondeur);
			printf("VALEUR %d\n", tmp);
			
			if(tmp > valeurMax){
				valeurMax = tmp;
				strcpy(coups, coupsTmp);
			}
			
			removeLastH(j->coups);
			jouerHistorique(j);
			
			courantDeplo = courantDeplo->next;
		}
		
		coordC = coordC->next;
	}
	printf("VALEUR FINAL : %d\n", valeurMax);
	return coups;
}

