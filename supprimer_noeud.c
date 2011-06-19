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
    xmlXPathContextPtr ctxt;
    xmlXPathObjectPtr xpathRes;
    xmlNodePtr n = NULL;
    char *path;

    xmlXPathInit();
    ctxt = xmlXPathNewContext(doc);
    asprintf(&path, "/catalogue/produit[@reference=\"%s\"]", ref);
    if (ctxt != NULL && path != NULL) {
        xpathRes = xmlXPathEvalExpression(path, ctxt);
        free(path);
        if (xpathRes && xpathRes->type == XPATH_NODESET && xpathRes->nodesetval->nodeNr == 1) {
            n = xpathRes->nodesetval->nodeTab[0];
        }
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

