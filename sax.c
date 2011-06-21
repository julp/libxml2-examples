#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

/**
 * Appelée une seule fois au démarrage du parsing d'un document.
 * Généralement utilisée pour initialiser ou allouée des ressources.
 **/
void debut_document(void *user_data) {
    printf("Début du document\n");
}

/**
 * Dernière fonction à être appelée (en fin de parsing).
 * Couramment employée pour libérer les ressources précédemment
 * allouées.
 **/
void fin_document(void *user_data) {
    printf("Fin du document\n");
}

/**
 * Appelée à la rencontre de chaque balise.
 * Le paramètre name fourni le nom de l'élément
 * et le paramètre attrs (si différent de NULL)
 * dévoile la liste des attributs (indices pairs)
 * et leurs valeurs (indices impairs)
 **/
void debut_element(void *user_data, const xmlChar *name, const xmlChar **attrs) {
    printf("Début de l'élément : %s\n", name);
    if (NULL != attrs) {
        int i;
        for (i = 0; attrs[i] != NULL; i+=2) {
            printf("Attribut '%s' a pour valeur %s\n", attrs[i], attrs[i+1]);
        }
    }
}

/**
 * Appelée à chaque fin de balise. Elle ne fournit
 * que le nom de l'élément (paramètre name).
 **/
void fin_element(void *user_data, const xmlChar *name) {
    printf("Fin de l'élément : %s\n", name);
}

/**
 * Appelée pour chaque noeud textuel
 * Le paramètre ch représente ce texte
 * et len sa longueur.
 * Attention :
 * - cette chaîne est codée en UTF-8
 * - cette chaîne n'est peut être pas terminée par un caractère nul
 **/
void caracteres(void *user_data, const xmlChar *ch, int len) {
    printf("Element texte : (longueur = %d) %.*s\n", len, len, ch);
}

/**
 * Appelée pour chaque section CDATA
 * Le paramètre ch correspond au contenu de la section sans
 * <![CDATA[ et ]]> et len en fourni la longueur.
 * Attention : cette chaîne est encodée en UTF-8
 **/
void cdata(void *user_data, const xmlChar *ch, int len) {
    printf("Element CDATA : (longueur = %d) %s\n", len, ch);
}

/**
 * Appelée pour chaque commentaire rencontré.
 * Le paramètre ch désigne le contenu du commentaire
 * (ie sans <!-- ni -->).
 **/
void commentaire(void *user_data, const xmlChar *ch) {
    printf("Commentaire : %s\n", ch);
}

/**
 * Appelée pour chaque entité.
 * Le paramètre name est le nom de l'entité débarrassé
 * de & et ; (exemple &gt; -> gt)
 **/
xmlEntityPtr entite(void *user_data, const xmlChar *name) {
    printf("Entité rencontrée : %s\n", name);
    return xmlGetPredefinedEntity(name); // Ceci est le traitement par défaut
}

/**
 * Appelée lorsque plus d'un caractères blancs (espace ou tabulation)
 * se succèdent.
 * (avec une regexp : [ \t]{2,} ou encore avec la POSIX [[:blank:]]{2,})
 * Note : inutilisé
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
 * Gestion des éventuelles erreurs rencontrées.
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
    // Initialisation à zéro de tous les membres (NULL pour un pointeur par conversion)
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
    sh.ignorableWhitespace = blanc; // Inutilisé !
    sh.processingInstruction = pi; // Ne fonctionne pas !

    // Parsing du document XML
    if (xmlSAXUserParseFile(&sh, NULL, "catalogue.xml") != 0) {
        printf("Une erreur est survenue lors du parsing\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
