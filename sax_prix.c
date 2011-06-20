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
static xmlChar *der_intitule;
static int elem_courant;

/**
 * Initialisation
 **/
void debut_document(void *user_data) {
    *der_ref = '\0';
    der_intitule = NULL;
    elem_courant = AUTRE;
    printf("Liste des produits dont le prix est strictement inf�rieur � %.2f :\n", PRIX_MAX);
}

/**
 * Si l'�l�ment courant est :
 * _ produit : on conserve la r�f�rence
 * _ intitule ou prix : on marque l'ouverture d'une telle balise
 **/
void debut_element(void *user_data, const xmlChar *name, const xmlChar **attrs) {
    if (xmlStrEqual(name, BAD_CAST "produit")) {
        if (NULL != attrs) {
            int i;
            for (i = 0; attrs[i] != NULL; i+=2) {
                if (xmlStrEqual(attrs[i], BAD_CAST "reference")) {
                    strncpy(der_ref, (char *) attrs[i+1], MAX_REF_LEN);
                }
            }
        }
    } else if (xmlStrEqual(name, BAD_CAST "intitule")) {
        elem_courant = INTITULE;
    } else if (xmlStrEqual(name, BAD_CAST "prix")) {
        elem_courant = PRIX;
    }
}

/**
 * R�initialisation du marqueur qui nous sert � savoir
 * o� l'on est dans le document XML
 **/
void fin_element(void *user_data, const xmlChar *name) {
    elem_courant = AUTRE;
}

/**
 * Suivant l'�l�ment d'o� provient le texte :
 * _ intitule : conservation
 * _ prix : affichage
 **/
void caracteres(void *user_data, const xmlChar *ch, int len) {
    if (PRIX == elem_courant) {
        float prix;
        if ((prix = strtof((char *) ch, NULL)) < PRIX_MAX) {
            printf("- %s : %s (%.2f Euros)\n", der_ref, (char *) der_intitule, prix);
        }
        free(der_intitule);
    } else if (INTITULE == elem_courant) {
        der_intitule = xmlStrndup(ch, len);
    }
}

int main() {
    // Initialisation � z�ro de tous les membres (NULL pour un pointeur par conversion)
    xmlSAXHandler sh = { 0 };

    // Affectation des fonctions de rappel
    sh.startDocument = debut_document;
    sh.startElement = debut_element;
    sh.endElement = fin_element;
    sh.characters = caracteres;

    // Parsing du document XML
    if (xmlSAXUserParseFile(&sh, NULL, "catalogue.xml") != 0) {
        fprintf(stderr, "Une erreur est survenue lors du parsing\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
