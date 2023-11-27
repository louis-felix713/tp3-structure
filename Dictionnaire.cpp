/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Louis-Felix Veillette (537183048)
 * \version 0.1
 * \date décembre 2023
 *
 */

#include "Dictionnaire.h"


// Limite du nombre de suggestions
#define LIMITE_SUGGESTIONS 5

namespace TP3
{
    /**
     * \brief Constructeur par defaut du dictionnaire
     */
    Dictionnaire::Dictionnaire(){
        this->racine = nullptr;
        this->cpt = 0;
    }

    /**
     * \brief Constructeur du dictionnaire ayant pour but de le creer grace a un fichier texte
     * \param[in] fichier fichier texte ayant le dictionnaire
     */
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

    /**
     * \brief Destructeur du dictionnaire
     */
    Dictionnaire::~Dictionnaire(){
        _detruire(racine);
    }

    /**
     * \brief Destructeur recursif du dictionnaire
     * \param[in] noeud Le noeud a supprimer
     */
    void Dictionnaire::_detruire(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if(noeud != nullptr) {
            _detruire(noeud->gauche);
            _detruire(noeud->droite);
            delete noeud;
            noeud = nullptr;
        }
    }
    /**
     * \brief Ajoute un mot au dictionnaire
     * \param[in] motOriginal Mot a ajouter au dictionnaire
     * \param[in] motTraduit Mot traduit du mot original a ajouter dans les traductions
     */
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

    /**
     * \brief Supprime un mot du dictionnaire
     * \param[in] motOriginal le mot a supprimer
     * \exception logic_error si le dico est vide ou si le mot n'est pas dans le dico
     */
    void Dictionnaire::supprimeMot(const std ::string& motOriginal)
    {
        if (cpt == 0) {
            throw std::logic_error("supprimeMot: l'arbre est vide");
        }
        if (!appartient(motOriginal)) {
            throw std::logic_error("supprimeMot: le mot n'est pas dans le dico");
        }
        _enlever(racine, motOriginal);
    }

    /**
     * \brief Compare la similitude de deux mots
     * \param[in] mot1 Le premier mot a comparer
     * \param[in] mot2 Le deuxieme mot a comparer
     * \return une valeur entre 0 et 1 representant la similitude entre les deux mots
     */
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

    /**
     * \brief Suggere des corrections pour le mot mal ecrit. S'il y a suffisament de mots, on donne 5 corrections possibles
     * \param[in] motMalEcrit Le mot mal ecrit
     * \return une liste de mots de corrections
     */
    std::vector<std::string> Dictionnaire::suggereCorrections(const std ::string& motMalEcrit)
    {
        if (cpt == 0) {
            throw std::logic_error("suggereCorrections: Le dico est vide");
        }
        std::vector<std::string> suggestions;
        _suggereCorrections(racine, motMalEcrit, suggestions);
        return suggestions;
    }

    /**
     * \brief Trouve les traductions possibles d'un mot
     * \param[in] mot Le mot a traduire
     * \return la liste de traductions du mot ou une liste vide si le mot n est pas dans le dico
     */
    std::vector<std::string> Dictionnaire::traduit(const std ::string& mot)
    {
        if (!appartient(mot)) {
            return {};
        }
        return _appartient(racine, mot)->traductions;
    }

    /**
     * \brief Verifie si un mot est dans le dico
     * \param[in] mot Le mot a trouver dans le dico
     * \return vrai si le mot est dans le dico, sinon faux.
     */
    bool Dictionnaire::appartient(const std::string &mot)
    {
	    return (_appartient(racine, mot) != nullptr);
    }

    /**
     * \brief Verifie de maniere recursive si un mot est dans le dico
     * \param[in] noeud Le noeud a verifier le mot
     * \param[in] mot Le mot a verifier
     * \return le noeud ayant le mot
     */
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

    /**
     * \brief Verifie si le dico est vide
     * \return vrai si le dico est vide
     */
    bool Dictionnaire::estVide() const
    {
	    return this->cpt == 0;
    }

    /**
     * \brief insere de maniere recursive un mot dans le dico
     * \param[in] noeud Un noeud a parcourir ou a inserer
     * \param[in] mot Le mot a inserer
     * \param[in] traduction une traduction du mot a ajouter
     */
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

    /**
     * \brief Supprime un mot du dictionnaire
     * \param[in] noeud Un noeud a parcourir ou a supprimer
     * \param[in] mot Le mot a supprimer
     */
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

    /**
     * \brief Met a jour la hauteur d un noeud dans l arbre
     * \param[in] noeud Le noeud qui doit etre mis a jour
     */
    void Dictionnaire::_miseAJourHauteur(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud != nullptr) {
            noeud->hauteur = 1 + std::max(_hauteur(noeud->gauche), _hauteur(noeud->droite));
        }
    }

    /**
     * \brief Donne la hauteur d un noeud
     * \param[in] noeud Le noeud a montrer la hauteur
     * \return la hauteur
     */
    int Dictionnaire::_hauteur(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return -1;
        }
        return noeud->hauteur;
    }


    /**
     * \brief Balance un noeud dependemment s il y a un debalancement
     * \param[in] noeud Le noeud a verifier
     */
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

    /**
     * \brief Verifie si un noeud a un deblancement a gauche
     * \param[in] noeud Le noeud a verifier
     * \return vrai si la difference de hauteur entre la gauche et la droite du noeud est a 2 ou plus.
     */
    bool Dictionnaire::_debalancementAGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if(noeud == nullptr) {
            return false;
        }
        return _hauteur(noeud->gauche) - _hauteur(noeud->droite) >= 2;
    }

    /**
     * \brief Verifie si un noeud a un deblancement a droite
     * \param[in] noeud Le noeud a verifier
     * \return vrai si la difference de hauteur entre la droite et la gauche du noeud est a 2 ou plus.
     */
    bool Dictionnaire::_debalancementADroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if(noeud == nullptr) {
            return false;
        }
        return _hauteur(noeud->droite) - _hauteur(noeud->gauche) >= 2;
    }

    /**
     * \brief Verifie si le noeud penche plus a gauche.
     * \param[in] noeud Le noeud a verifier
     * \return vrai si le noeud penche plus si la hauteur de gauche est plus grande que celle de droite
     */
    bool Dictionnaire::_sousArbrePencheAGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return false;
        }

        return (_hauteur(noeud->gauche) > _hauteur(noeud-> droite));
    }

    /**
     * \brief Verifie si le noeud penche plus a droite.
     * \param[in] noeud Le noeud a verifier
     * \return vrai si le noeud penche plus si la hauteur de droite est plus grande que celle de gauche
     */
    bool Dictionnaire::_sousArbrePencheADroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud == nullptr) {
            return false;
        }

        return (_hauteur(noeud->gauche) < _hauteur(noeud-> droite));
    }

    /**
     * \brief Effectue une rotation vers la droite pour balancer l arbre
     * \param noeud Le noeud a balancer
     */
    void Dictionnaire::_zigZigGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        NoeudDictionnaire* sousNoeud = noeud->gauche;
        noeud->gauche = sousNoeud->droite;
        sousNoeud->droite = noeud;

        _miseAJourHauteur(noeud);
        _miseAJourHauteur(sousNoeud);

        noeud = sousNoeud;
    }

    /**
     * \brief Effectue une rotation a droite et a gauche
     * \param[in] noeud Le noeud a rotationner
     */
    void Dictionnaire::_zigZagGauche(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        _zigZigDroite(noeud->gauche);
        _zigZigGauche(noeud);
    }

    /**
     * \brief Effectue une rotation vers la gauche
     * \param[in] noeud Le noeud a balancer
     */
    void Dictionnaire::_zigZigDroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        NoeudDictionnaire* sousNoeud = noeud->droite;
        noeud->droite = sousNoeud->gauche;
        sousNoeud->gauche = noeud;

        _miseAJourHauteur(noeud);
        _miseAJourHauteur(sousNoeud);

        noeud = sousNoeud;
    }

    /**
     * \brief effectue un rotation vers la droite et vers la gauche
     * \param[in] noeud Le noeud a rotationner
     */
    void Dictionnaire::_zigZagDroite(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        _zigZigGauche(noeud->droite);
        _zigZigDroite(noeud);
    }

    /**
     * \brief Trouve le plus petit noeud dans un branche d un noeud
     * \param[in] noeud Le noeud a parcourir ou a trouver
     * \return Le noeud le plus petit d une branche
     */
    TP3::Dictionnaire::NoeudDictionnaire *Dictionnaire::_min(TP3::Dictionnaire::NoeudDictionnaire *& noeud) const {
        if (noeud->gauche == nullptr) {
            return noeud;
        }
        return _min(noeud->gauche);
    }

    /**
     * \brief Ajoute des corrections d'un mot dans un vecteur
     * \param[in] noeud Le noeud a fouiller
     * \param[in] mot Le mot a trouver une correction
     * \param[out] vec le vecteur qui aura les corrections
     */
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
