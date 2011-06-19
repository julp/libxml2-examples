#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

int main() {
    // Ouverture du document XML
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    xmlDocPtr doc = xmlParseFile("catalogue.xml");
    if (doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // Initialisation de l'environnement XPath
    xmlXPathInit();
    // Création du contexte
    xmlXPathContextPtr ctxt = xmlXPathNewContext(doc);
    if (ctxt == NULL) {
        fprintf(stderr, "Erreur lors de la création du contexte XPath\n");
        return EXIT_FAILURE;
    }
    // Evaluation de l'expression XPath
    xmlXPathObjectPtr xpathRes = xmlXPathEvalExpression("/catalogue/produit[prix<10]/intitule/text()", ctxt);
    if (xpathRes == NULL) {
        fprintf(stderr, "Erreur sur l'expression XPath\n");
        return EXIT_FAILURE;
    }
    // Manipulation du résultat
    if (xpathRes->type == XPATH_NODESET) {
        int i;
        printf("Produits dont le prix est inférieur à 10 Euros :\n");
        for (i = 0; i < xpathRes->nodesetval->nodeNr; i++) {
            xmlNodePtr n = xpathRes->nodesetval->nodeTab[i];
            if (n->type == XML_TEXT_NODE || n->type == XML_CDATA_SECTION_NODE) {
                printf("- %s\n", n->content);
            }
        }
    }
    // Libération de la mémoire
    xmlXPathFreeObject(xpathRes);
    xmlXPathFreeContext(ctxt);
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}

