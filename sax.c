#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

/**
 * Appel�e une seule fois au d�marrage du parsing d'un document.
 * G�n�ralement utilis�e pour initialiser ou allou�e des ressources.
 **/
void debut_document(void *user_data) {
    printf("D�but du document\n");
}

/**
 * Derni�re fonction � �tre appel�e (en fin de parsing).
 * Couramment employ�e pour lib�rer les ressources pr�c�demment
 * allou�es.
 **/
void fin_document(void *user_data) {
    printf("Fin du document\n");
}

/**
 * Appel�e � la rencontre de chaque balise.
 * Le param�tre name fourni le nom de l'�l�ment
 * et le param�tre attrs (si diff�rent de NULL)
 * d�voile la liste des attributs (indices pairs)
 * et leurs valeurs (indices impairs)
 **/
void debut_element(void *user_data, const xmlChar *name, const xmlChar **attrs) {
    printf("D�but de l'�l�ment : %s\n", name);
    if (attrs != NULL) {
        int i;
        for (i = 0; attrs[i] != NULL; i+=2) {
            printf("Attribut '%s' a pour valeur %s\n", attrs[i], attrs[i+1]);
        }
    }
}

/**
 * Appel�e � chaque fin de balise. Elle ne fournit
 * que le nom de l'�l�ment (param�tre name).
 **/
void fin_element(void *user_data, const xmlChar *name) {
    printf("Fin de l'�l�ment : %s\n", name);
}

/**
 * Appel�e pour chaque noeud textuel
 * Le param�tre ch repr�sente ce texte
 * et len sa longueur.
 * Attention :
 * - cette cha�ne est cod�e en UTF-8
 * - cette cha�ne n'est peut �tre pas termin�e par un caract�re nul
 **/
void caracteres(void *user_data, const xmlChar *ch, int len) {
    printf("Element texte : (longueur = %d) %.*s\n", len, len, ch);
}

/**
 * Appel�e pour chaque section CDATA
 * Le param�tre ch correspond au contenu de la section sans
 * <![CDATA[ et ]]> et len en fourni la longueur.
 * Attention : cette cha�ne est encod�e en UTF-8
 **/
void cdata(void *user_data, const xmlChar *ch, int len) {
    printf("Element CDATA : (longueur = %d) %s\n", len, ch);
}

/**
 * Appel�e pour chaque commentaire rencontr�.
 * Le param�tre ch d�signe le contenu du commentaire
 * (ie sans <!-- ni -->).
 **/
void commentaire(void *user_data, const xmlChar *ch) {
    printf("Commentaire : %s\n", ch);
}

/**
 * Appel�e pour chaque entit�.
 * Le param�tre name est le nom de l'entit� d�barrass�
 * de & et ; (exemple &gt; -> gt)
 **/
xmlEntityPtr entite(void *user_data, const xmlChar *name) {
    printf("Entit� rencontr�e : %s\n", name);
    return xmlGetPredefinedEntity(name); // Ceci est le traitement par d�faut
}

/**
 * Appel�e lorsque plus d'un caract�res blancs (espace ou tabulation)
 * se succ�dent.
 * (avec une regexp : [ \t]{2,} ou encore avec la POSIX [[:blank:]]{2,})
 * Note : inutilis�
 **/
void blanc(void *user_data, const xmlChar *ch, int len) {
    printf("Blancs : (longueur = %d) #%s#\n", len, ch);
}

/**
 * TODO
 **/
void pi(void *user_data, const xmlChar *target, const xmlChar *data) {
    printf("PI : %s -> %s\n", target, data);
}

/**
 * Gestion des �ventuelles erreurs rencontr�es.
 * Ici on se contente d'afficher les messages sur
 * la sortie d'erreur standard.
 **/
void avertissement(void *user_data, const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}

void erreur(void *user_data, const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}

void erreur_fatale(void *user_data, const char *msg, ...) {
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}

int main() {
    // Initialisation � z�ro de tous les membres (NULL pour un pointeur par conversion)
    xmlSAXHandler sh = { 0 };

    // Affectation des fonctions de rappel
    sh.startDocument = debut_document;
    sh.endDocument = fin_document;
    sh.startElement = debut_element;
    sh.endElement = fin_element;
    sh.warning = avertissement;
    sh.error = erreur;
    sh.fatalError = erreur_fatale;
    sh.characters = caracteres;
    sh.cdataBlock = cdata;
    sh.comment = commentaire;
    sh.getEntity = entite;
    sh.ignorableWhitespace = blanc; // Inutilis� !
    sh.processingInstruction = pi; // Ne fonctionne pas !

    // Parsing du document XML
    if (xmlSAXUserParseFile(&sh, NULL, "catalogue.xml") != 0) {
        printf("Une erreur est survenue lors du parsing\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
