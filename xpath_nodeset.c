#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

int main() {
    xmlDocPtr doc;
    xmlXPathContextPtr ctxt;
    xmlXPathObjectPtr xpathRes;

    // Ouverture du document XML
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    doc = xmlParseFile("catalogue.xml");
    if (doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // Initialisation de l'environnement XPath
    xmlXPathInit();
    // Création du contexte
    if (NULL == (ctxt = xmlXPathNewContext(doc))) {
        fprintf(stderr, "Erreur lors de la création du contexte XPath\n");
        return EXIT_FAILURE;
    }
    // Evaluation de l'expression XPath
    if (NULL == (xpathRes = xmlXPathEvalExpression(BAD_CAST "/catalogue/produit[prix<10]/intitule/text()", ctxt))) {
        fprintf(stderr, "Erreur sur l'expression XPath\n");
        return EXIT_FAILURE;
    }
    // Manipulation du résultat
    if (XPATH_NODESET == xpathRes->type) {
        int i;
        printf("Produit(s) dont le prix est inférieur à 10 Euros :\n");
        for (i = 0; i < xpathRes->nodesetval->nodeNr; i++) {
            xmlNodePtr n = xpathRes->nodesetval->nodeTab[i];
            if (XML_TEXT_NODE == n->type || XML_CDATA_SECTION_NODE == n->type) {
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

