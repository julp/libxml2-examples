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
    // Création du contexte
    if (NULL == (ctxt = xmlXPathNewContext(doc))) {
        fprintf(stderr, "Erreur lors de la création du contexte XPath\n");
        return EXIT_FAILURE;
    }
    // Evaluation de l'expression XPath
    xpathRes = xmlXPathEvalExpression(BAD_CAST "translate(/catalogue/produit[intitule=\"Sweat Developpez.com\"]/@reference, \"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\", \"abcdefghijklmnopqrstuvwxyz0123456789\")", ctxt);
    if (NULL == xpathRes) {
        fprintf(stderr, "Erreur sur l'expression XPath\n");
        return EXIT_FAILURE;
    }
    // Manipulation du résultat
    if (XPATH_STRING == xpathRes->type) {
        printf("Référence du produit 'Sweat Developpez.com' en minuscule : %s\n", xmlXPathCastToString(xpathRes));
    }
    // Libération de la mémoire
    xmlXPathFreeObject(xpathRes);
    xmlXPathFreeContext(ctxt);
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}
