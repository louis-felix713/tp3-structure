/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date décembre 2023
 *
 */

#include "Dictionnaire.h"


// Limite du nombre de suggestions
#define LIMITE_SUGGESTIONS 5

namespace TP3
{
    Dictionnaire::Dictionnaire(){
        this->racine = nullptr;
        this->cpt = 0;
    }

	Dictionnaire::Dictionnaire(std::ifstream &fichier): racine(nullptr), cpt(0)
    {
        if (fichier)
        {
            for( std::string ligneDico; getline( fichier, ligneDico); )
            {
                if (ligneDico[0] != '#') //Élimine les lignes d'en-tête
                {
                    // Le mot anglais est avant la tabulation (\t).
                    std::string motAnglais = ligneDico.substr(0,ligneDico.find_first_of('\t'));
                    
                    // Le reste (définition) est après la tabulation (\t).
                    std::string motTraduit = ligneDico.substr(motAnglais.length()+1, ligneDico.length()-1);

                    //On élimine tout ce qui est entre crochets [] (possibilité de 2 ou plus)
                    std::size_t pos = motTraduit.find_first_of('[');
                    while (pos!=std::string::npos)
                    {
                        std::size_t longueur_crochet = motTraduit.find_first_of(']')-pos+1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('[');
                    }
                    
                    //On élimine tout ce qui est entre deux parenthèses () (possibilité de 2 ou plus)
                    pos = motTraduit.find_first_of('(');
                    while (pos!=std::string::npos)
                    {
                        std::size_t longueur_crochet = motTraduit.find_first_of(')')-pos+1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('(');
                    }

                    //Position d'un tilde, s'il y a lieu
                    std::size_t posT = motTraduit.find_first_of('~');
                    
                    //Position d'un tilde, s'il y a lieu
                    std::size_t posD = motTraduit.find_first_of(':');
                    
                    if (posD < posT)
                    {
                        //Quand le ':' est avant le '~', le mot français précède le ':'
                        motTraduit = motTraduit.substr(0, posD);
                    }
                    
                    else
                    {
                        //Quand le ':' est après le '~', le mot français suit le ':'
                        if (posT < posD)
                        {
                            motTraduit = motTraduit.substr(posD, motTraduit.find_first_of("([,;\n", posD));
                        }
                        else
                        {
                            //Quand il n'y a ni ':' ni '~', on extrait simplement ce qu'il y a avant un caractère de limite
                            motTraduit = motTraduit.substr(0, motTraduit.find_first_of("([,;\n"));
                        }
                    }
                    
                    //On ajoute le mot au dictionnaire
                    ajouteMot(motAnglais, motTraduit);
                    std::cout<<motAnglais << " - " << motTraduit<<std::endl;
                }
            }
        }
	}

    Dictionnaire::~Dictionnaire(){
        _detruire(racine);
    }

    void Dictionnaire::_detruire(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if(noeud != nullptr) {
            _detruire(noeud->gauche);
            _detruire(noeud->droite);
            delete noeud;
            noeud = nullptr;
        }
    }

    void Dictionnaire::ajouteMot(const std::string& motOriginal, const std::string& motTraduit)
    {
        NoeudDictionnaire* noeudMot = _appartient(racine, motOriginal);
        if (noeudMot != nullptr) {
            noeudMot->traductions.push_back(motTraduit);
        }
        else {
            _inserer(racine, motOriginal, motTraduit);
        }
    }

    void Dictionnaire::supprimeMot(const std ::string& motOriginal)
    {
        _enlever(racine, motOriginal);
    }

    double Dictionnaire::similitude(const std ::string& mot1, const std ::string& mot2)
    {
        double minLength = (mot1.length() > mot2.length()) ? mot2.length() : mot1.length();
        double maxLength = (mot1.length() > mot2.length()) ? mot1.length() : mot2.length();
        double changementMot1 = mot1.length();
        double changementMot2 = mot2.length();
        for(auto i = 0; i < maxLength; ++i) {
            if (minLength <= i) {
                changementMot1--;
                changementMot2--;
            }
            else if (mot1[i] != mot2[i]) {
                changementMot1--;
                changementMot2--;
            }
        }
	    return (changementMot1 + changementMot2) / (minLength + maxLength);
    }

    std::vector<std::string> Dictionnaire::suggereCorrections(const std ::string& motMalEcrit)
    {
        std::vector<std::string> suggestions;
        _suggereCorrections(racine, motMalEcrit, suggestions);
        return suggestions;
    }

    std::vector<std::string> Dictionnaire::traduit(const std ::string& mot)
    {
        return _appartient(racine, mot)->traductions;
    }

    bool Dictionnaire::appartient(const std::string &mot)
    {
	    return (_appartient(racine, mot) != nullptr);
    }

    TP3::Dictionnaire::NoeudDictionnaire* Dictionnaire::_appartient(TP3::Dictionnaire::NoeudDictionnaire *& noeud, const std::string & mot) const {
        if(noeud == nullptr) {
            return nullptr;
        }
        if(noeud->mot == mot) {
            return noeud;
        }

        if (noeud->mot > mot) {
            return _appartient(noeud->gauche, mot);
        }
        else {
            return _appartient(noeud->droite, mot);
        }
    }

    bool Dictionnaire::estVide() const
    {
	    return this->cpt == 0;
    }

    void
    Dictionnaire::_inserer(TP3::Dictionnaire::NoeudDictionnaire *& noeud, const std::string & mot, const std::string & traduction) {
        if (noeud == nullptr) {
            noeud = new NoeudDictionnaire(mot);
            noeud->traductions.push_back(traduction);
            this->cpt++;
            return;
        }
        else if(noeud->mot > mot) {
            _inserer(noeud->gauche, mot, traduction);
        }
        else {
            _inserer(noeud->droite, mot, traduction);
        }
        _miseAJourHauteur(noeud);
        _balancerNoeud(noeud);
    }

    void Dictionnaire::_enlever(TP3::Dictionnaire::NoeudDictionnaire *& noeud, const std::string & mot) {
        if (noeud == nullptr) {
            throw std::logic_error("enlever: element inexistant");
        }
        if (noeud->mot > mot) {
            _enlever(noeud->gauche,  mot);
        }
        else if (noeud->mot < mot) {
            _enlever(noeud->droite, mot);
        }
        else {
            if (_hauteur(noeud) == 0) {
                delete noeud;
                noeud = nullptr;
                cpt--;
                _miseAJourHauteur(noeud);
                _balancerNoeud(noeud);
            }
            else if (noeud->gauche != nullptr && noeud->droite != nullptr) {
                NoeudDictionnaire* min = _min(noeud->droite);
                std::swap(noeud->mot, min->mot);
                _enlever(noeud->droite, mot);
            }
            else {
                if (noeud->gauche != nullptr) {
                    std::swap(noeud->mot, noeud->gauche->mot);
                    _enlever(noeud->gauche, mot);
                }
                else {
                    std::swap(noeud->mot, noeud->droite->mot);
                    _enlever(noeud->droite, mot);
                }
            }
        }
    }

    void Dictionnaire::_miseAJourHauteur(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud != nullptr) {
            noeud->hauteur = 1 + std::max(_hauteur(noeud->gauche), _hauteur(noeud->droite));
        }
    }

    int Dictionnaire::_hauteur(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return -1;
        }
        return noeud->hauteur;
    }

    void Dictionnaire::_balancerNoeud(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return;
        }
        if (_debalancementAGauche(noeud)) {
            if (_sousArbrePencheADroite(noeud->gauche)) {
                _zigZagGauche(noeud);
            }
            else {
                _zigZigGauche(noeud);
            }
        }
        else if (_debalancementADroite(noeud)) {
            if (_sousArbrePencheAGauche(noeud->droite)) {
                _zigZagDroite(noeud);
            }
            else {
                _zigZigDroite(noeud);
            }
        }
    }

    bool Dictionnaire::_debalancementAGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if(noeud == nullptr) {
            return false;
        }
        return _hauteur(noeud->gauche) - _hauteur(noeud->droite) >= 2;
    }

    bool Dictionnaire::_debalancementADroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if(noeud == nullptr) {
            return false;
        }
        return _hauteur(noeud->droite) - _hauteur(noeud->gauche) >= 2;
    }

    bool Dictionnaire::_sousArbrePencheAGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return false;
        }

        return (_hauteur(noeud->gauche) > _hauteur(noeud-> droite));
    }

    bool Dictionnaire::_sousArbrePencheADroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return false;
        }

        return (_hauteur(noeud->gauche) < _hauteur(noeud-> droite));
    }

    void Dictionnaire::_zigZigGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        NoeudDictionnaire* sousNoeud = noeud->gauche;
        noeud->gauche = sousNoeud->droite;
        sousNoeud->droite = noeud;

        _miseAJourHauteur(noeud);
        _miseAJourHauteur(sousNoeud);

        noeud = sousNoeud;
    }

    void Dictionnaire::_zigZagGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        _zigZigDroite(noeud->gauche);
        _zigZigGauche(noeud);
    }

    void Dictionnaire::_zigZigDroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        NoeudDictionnaire* sousNoeud = noeud->droite;
        noeud->droite = sousNoeud->gauche;
        sousNoeud->gauche = noeud;

        _miseAJourHauteur(noeud);
        _miseAJourHauteur(sousNoeud);

        noeud = sousNoeud;
    }

    void Dictionnaire::_zigZagDroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        _zigZigGauche(noeud->droite);
        _zigZigDroite(noeud);
    }

    TP3::Dictionnaire::NoeudDictionnaire *Dictionnaire::_min(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud->gauche == nullptr) {
            return noeud;
        }
        return _min(noeud->gauche);
    }

    void Dictionnaire::_suggereCorrections(Dictionnaire::NoeudDictionnaire *& noeud, const std::string & mot, std::vector<std::string> & vec) {
        if (noeud == nullptr) {
            return;
        }
        else if(noeud->mot > mot) {
            _suggereCorrections(noeud->gauche, mot, vec);
        }
        else {
            _suggereCorrections(noeud->droite, mot, vec);
            if (noeud->gauche != nullptr) {
                _suggereCorrections(noeud->gauche, mot, vec);
            }
        }
        if(vec.size() < 5) {
            if(similitude(mot, noeud->mot) > 0.3) {
                vec.push_back(noeud->mot);
            }
        }
    }
  
}//Fin du namespace
