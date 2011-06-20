#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "port/asprintf.c"

/**
 * Retourne le noeud "produit" correspondant à une référence donnée
 * (pour l'implémentation voir les sources)
 **/
xmlNodePtr get_produit_by_ref(xmlDocPtr doc, const char *ref) {
    char *path;
    xmlNodePtr n = NULL;
    xmlXPathContextPtr ctxt;
    xmlXPathObjectPtr xpathRes;

    xmlXPathInit();
    ctxt = xmlXPathNewContext(doc);
    if (-1 == asprintf(&path, "/catalogue/produit[@reference=\"%s\"]", ref)) {
        fprintf(stderr, "asprintf failed\n");
        return NULL;
    }
    if (NULL != ctxt && NULL != path) {
        if (NULL != (xpathRes = xmlXPathEvalExpression(BAD_CAST path, ctxt)) && XPATH_NODESET == xpathRes->type && 1 == xpathRes->nodesetval->nodeNr) {
            n = xpathRes->nodesetval->nodeTab[0];
        }
        free(path);
        xmlXPathFreeObject(xpathRes);
        xmlXPathFreeContext(ctxt);
    }

    return n;
}

int main() {
    xmlNodePtr sweat;

    // Ouverture du document
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    xmlDocPtr doc = xmlParseFile("catalogue.xml");
    if (doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return EXIT_FAILURE;
    }

    // Récupération du produit "sweat"
    sweat = get_produit_by_ref(doc, "IHRC24");
    // Suppression du noeud
    xmlUnlinkNode(sweat);
    xmlFreeNode(sweat);
    // Affichage de l'arbre DOM tel qu'il est dans la mémoire
    xmlDocFormatDump(stdout, doc, 1);
    // Libération de la mémoire
    xmlFreeDoc(doc);

    return EXIT_FAILURE;
}

