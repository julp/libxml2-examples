#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#define MAX_REF_LEN 6
#define PRIX_MAX 10.0

enum {
    AUTRE,
    PRODUIT,
    INTITULE,
    PRIX
};

static char der_ref[MAX_REF_LEN + 1];
static char *der_intitule;
static int elem_courant;

#include "port/strndup.c"

/**
 * Initialisation
 **/
void debut_document(void *user_data) {
    *der_ref = '\0';
    der_intitule = NULL;
    elem_courant = AUTRE;
    printf("Liste des produits dont le prix est strictement inférieur à %.2f :\n", PRIX_MAX);
}

/**
 * Libération des ressources
 **/
void fin_document(void *user_data) {
    free(der_intitule);
}

/**
 * Si l'élément courant est :
 * _ produit : on conserve la référence
 * _ intitule ou prix : on marque l'ouverture d'une telle balise
 **/
void debut_element(void *user_data, const xmlChar *name, const xmlChar **attrs) {
    if (!xmlStrcmp(name, "produit")) {
        if (attrs != NULL) {
            int i;
            for (i = 0; attrs[i] != NULL; i+=2) {
                if (!xmlStrcmp(attrs[i], "reference")) {
                    strncpy(der_ref, attrs[i+1], MAX_REF_LEN);
                }
            }
        }
    } else if (!xmlStrcmp(name, "intitule")) {
        elem_courant = INTITULE;
    } else if (!xmlStrcmp(name, "prix")) {
        elem_courant = PRIX;
    }
}

/**
 * Réinitialisation du marqueur qui nous sert à savoir
 * où l'on est dans le document XML
 **/
void fin_element(void *user_data, const xmlChar *name) {
    elem_courant = AUTRE;
}

/**
 * Suivant l'élément d'où provient le texte :
 * _ intitule : conservation
 * _ prix : affichage
 **/
void caracteres(void *user_data, const xmlChar *ch, int len) {
    if (elem_courant == PRIX) {
        float prix;
        if ((prix = strtof(ch, NULL)) < PRIX_MAX) {
            printf("- %s : %s (%.2f Euros)\n", der_ref, der_intitule, prix);
        }
        free(der_intitule);
    } else if (elem_courant == INTITULE) {
        der_intitule = (char *) strndup(ch, len);
    }
}

int main() {
    // Initialisation à zéro de tous les membres (NULL pour un pointeur par conversion)
    xmlSAXHandler sh = { 0 };

    // Affectation des fonctions de rappel
    sh.startDocument = debut_document;
    sh.endDocument = fin_document;
    sh.startElement = debut_element;
    sh.endElement = fin_element;
    sh.characters = caracteres;

    // Parsing du document XML
    if (xmlSAXUserParseFile(&sh, NULL, "catalogue.xml") != 0) {
        printf("Une erreur est survenue lors du parsing\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
