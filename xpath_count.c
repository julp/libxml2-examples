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
    if (NULL == (doc = xmlParseFile("catalogue.xml"))) {
        fprintf(stderr, "Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // Initialisation de l'environnement XPath
    xmlXPathInit();
    // Cr�ation du contexte
    if (NULL == (ctxt = xmlXPathNewContext(doc))) {
        fprintf(stderr, "Erreur lors de la cr�ation du contexte XPath\n");
        return EXIT_FAILURE;
    }
    // Evaluation de l'expression XPath
    if (NULL == (xpathRes = xmlXPathEvalExpression(BAD_CAST "count(//produit)", ctxt))) {
        fprintf(stderr, "Erreur sur l'expression XPath\n");
        return EXIT_FAILURE;
    }
    // Manipulation du r�sultat
    if (XPATH_NUMBER == xpathRes->type) {
        printf("Nombre de produits dans le catalogue : %.0f\n", xmlXPathCastToNumber(xpathRes));
    }
    // Lib�ration de la m�moire
    xmlXPathFreeObject(xpathRes);
    xmlXPathFreeContext(ctxt);
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}

