/**
 * \file Principal.cpp
 * \brief Fichier de chargement pour le dictionnaire, avec interface de traduction
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date décembre 2023
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "Dictionnaire.h"

using namespace std;
using namespace TP3;

int main()
{

	try
	{

		string reponse = "";
		ifstream englishFrench;

		bool reponse_lue = false; //Booléen pour savoir que l'utilisateur a saisi une réponse

		//Tant que la réponse est vide ou non valide, on redemande.
		while (!reponse_lue)
		{
			cout << "Entrez le nom du fichier du dictionnaire Anglais-Francais : ";
			getline(cin, reponse);
			englishFrench.open(reponse.c_str());
			if (englishFrench) {
				cout << "Fichier '" << reponse << "' lu!" << endl;
				reponse_lue = true;
			}
			else {
				cout << "Fichier '" << reponse << "' introuvable! Veuillez entrer un nom de fichier, ou son chemin absolu." << endl;
				cin.clear();
				cin.ignore();
			}
		}

		//Initialisation d'un ditionnaire, avec le constructeur qui utilise un fstream
		Dictionnaire dictEnFr(englishFrench);
		englishFrench.close();


	    // Affichage du dictionnaire niveau par niveau
	    cout << dictEnFr << endl;

		vector<string> motsAnglais; //Vecteur qui contiendra les mots anglais de la phrase entrée

		//Lecture de la phrase en anglais
		cout << "Entrez un texte en anglais (pas de majuscules ou de ponctuation/caracteres speciaux):" << endl;
		getline(cin, reponse);

		stringstream reponse_ss(reponse);
		string str_temp;

		//On ajoute un mot au vecteur de mots tant qu'on en trouve dans la phrase (séparateur = espace)
		while (reponse_ss >> str_temp)
		{
			motsAnglais.push_back(str_temp);
		}

		vector<string> motsFrancais; //Vecteur qui contiendra les mots traduits en français

		for (vector<string>::const_iterator i = motsAnglais.begin(); i != motsAnglais.end(); i++)
			// Itération dans les mots anglais de la phrase donnée
		{
            string mot = *i;
			if (!dictEnFr.appartient(mot)) {
                vector<string> similaires = dictEnFr.suggereCorrections(*i);
                if (similaires.size() != 0) {
                    cout << "Le mot \"" << mot << "\" n\'existe pas dans le dictionnaire. Veuillez choisir une des suggestions suivantes" << endl;
                    for (int i = 0; i < similaires.size(); i++) {
                        cout << i + 1 << ". " << similaires[i] << endl;
                    }
                    getline(cin, reponse);
                    int rep = stoi(reponse);
                    if (rep <= similaires.size() && rep > 0) {
                        mot = similaires[rep - 1];
                    }
                }
                else {
                    cout << "Aucune similitude pour ce mot." << endl;
                }
            }
            vector<string> traductions = dictEnFr.traduit(mot);
            if (traductions.size() > 1) {
                cout << "Plusieurs traductions sont possibles pour le mot \"" << mot << "\". Veuillez choisir une des suggestions suivantes" << endl;
                for (int i = 0; i < traductions.size(); i++) {
                    cout << i + 1 << ". " << traductions[i] << endl;
                }
                getline(cin, reponse);
                int rep = stoi(reponse);
                if (rep <= i->size() && rep > 0) {
                    motsFrancais.push_back(traductions[rep - 1]);
                }
            }
            else {
                motsFrancais.push_back(traductions[0]);
            }
		}

		stringstream phraseFrancais; // On crée un string contenant la phrase,
									 // À partir du vecteur de mots traduits.
		for (vector<string>::const_iterator i = motsFrancais.begin(); i != motsFrancais.end(); i++)
		{
			phraseFrancais << *i << " ";
		}

		cout << "La phrase en francais est :" << endl << phraseFrancais.str() << endl;

	}
	catch (exception & e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}
