#include <stdio.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

typedef void (*fct_parcours_t)(xmlNodePtr);

void parcours_prefixe(xmlNodePtr noeud, fct_parcours_t f) {
    xmlNodePtr n;
    
    for (n = noeud; n != NULL; n = n->next) {
        f(n);
        if ((n->type == XML_ELEMENT_NODE) && (n->children != NULL)) {
            parcours_prefixe(n->children, f);
        }
    }
}

void parcours_postfixe(xmlNodePtr noeud, fct_parcours_t f) {
    xmlNodePtr n;
    
    for (n = noeud; n != NULL; n = n->next) {
        if ((n->type == XML_ELEMENT_NODE) && (n->children != NULL)) {
            parcours_postfixe(n->children, f);
        }
        f(n);
    }
}

void afficher_noeud(xmlNodePtr noeud) {
    if (noeud->type == XML_ELEMENT_NODE) {
        xmlChar *chemin = xmlGetNodePath(noeud);
        if (noeud->children != NULL && noeud->children->type == XML_TEXT_NODE) {
            xmlChar *contenu = xmlNodeGetContent(noeud);
            printf("%s -> %s\n", chemin, contenu);
            xmlFree(contenu);
        } else {
            printf("%s\n", chemin);
        }
        xmlFree(chemin);
    }
}

int main() {
    xmlDocPtr doc;
    xmlNodePtr racine;
 
    // Ouverture du document
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    doc = xmlParseFile("parcours.xml");
    if (doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // R�cup�ration de la racine
    racine = xmlDocGetRootElement(doc);
    if (racine == NULL) {
        fprintf(stderr, "Document XML vierge\n");
        xmlFreeDoc(doc);
        return EXIT_FAILURE;
    }
    // Parcours
    printf("Parcours pr�fix� :\n");
    parcours_prefixe(racine, afficher_noeud);
    printf("\n\n");
    printf("Parcours postfix� :\n");
    parcours_postfixe(racine, afficher_noeud);
    // Lib�ration de la m�moire
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}
