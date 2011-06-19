#include <stdlib.h>
#include <stdio.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

/**
 * Retourne un nouveau noeud correspondant à un produit (NULL en cas d'erreur)
 **/
xmlNodePtr creer_produit(const char *reference, const char *intitule, const char *prix) {
    xmlNodePtr noeud_produit;
    xmlNodePtr noeud_intitule;

    // Création du noeud "produit"
    if ((noeud_produit = xmlNewNode(NULL, BAD_CAST "produit")) == NULL) {
        return NULL;
    }
    // Ajout de son attribut "reference"
    if (xmlSetProp(noeud_produit, BAD_CAST "reference", BAD_CAST reference) == NULL) {
        xmlFreeNode(noeud_produit);
        return NULL;
    }
    // Création du noeud intitule
    if ((noeud_intitule = xmlNewNode(NULL, BAD_CAST "intitule")) == NULL) {
        xmlFreeNode(noeud_produit);
        return NULL;
    }
    xmlNodeSetContent(noeud_intitule, BAD_CAST intitule);
    // Ajout du noeud (intitule) à son père (produit)
    if (xmlAddChild(noeud_produit, noeud_intitule) == NULL) {
        xmlFreeNode(noeud_produit);
        xmlFreeNode(noeud_intitule);
        return NULL;
    }
    // Equivalent plus rapide par rapport au noeud intitule
    // Création du noeud "prix" et ajout à son père (produit)
    if (xmlNewTextChild(noeud_produit, NULL, BAD_CAST "prix", BAD_CAST prix) == NULL) {
        xmlFreeNode(noeud_produit);
        return NULL;
    }

    return noeud_produit;
}

/**
 * Retourne le premier produit du catalogue (sinon NULL)
 **/
xmlNodePtr obtenir_premier_produit(xmlDocPtr doc) {
    xmlXPathContextPtr ctxt;
    xmlXPathObjectPtr xpathRes;
    xmlNodePtr n = NULL;

    xmlXPathInit();
    if (NULL != (ctxt = xmlXPathNewContext(doc))) {
        xpathRes = xmlXPathEvalExpression(BAD_CAST "/catalogue/produit[position()=1]", ctxt);
        if (xpathRes && XPATH_NODESET == xpathRes->type && xpathRes->nodesetval->nodeNr == 1) {
            n = xpathRes->nodesetval->nodeTab[0];
        }
        xmlXPathFreeObject(xpathRes);
        xmlXPathFreeContext(ctxt);
    }

    return n;
}

int main() {
    xmlDocPtr doc;
    xmlNodePtr racine, premier_prod, nouv_prod;

    // Ouverture du fichier XML
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile("catalogue.xml");
    if (doc == NULL) {
        fprintf(stderr, "Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // Récupération de la racine
    racine = xmlDocGetRootElement(doc);
    if (racine == NULL) {
        fprintf(stderr, "Document XML vierge\n");
        xmlFreeDoc(doc);
        return EXIT_FAILURE;
    }
    // Récupération du premier produit
    premier_prod = obtenir_premier_produit(doc);
    if (premier_prod == NULL) {
        fprintf(stderr, "Impossible de trouver le premier produit\n");
        xmlFreeDoc(doc);
        return EXIT_FAILURE;
    }
    // Ajout d'un nouveau produit avant le premier produit (en tête)
    nouv_prod = creer_produit("CD0YAH", "Autocollant Developpez.com", "0.80");
    if (nouv_prod) {
        xmlAddPrevSibling(premier_prod, nouv_prod);
    }
    // Ajout d'un nouveau produit après le premier produit
    nouv_prod = creer_produit("U0TZ6K", "Lot de 10 autocollants Developpez.com", "5.00");
    if (nouv_prod) {
        xmlAddNextSibling(premier_prod, nouv_prod);
    }
    // Ajout d'un nouveau produit en fin/queue
    nouv_prod = creer_produit("ZQEYAN", "Mug Developpez.com", "4.00");
    if (nouv_prod) {
        xmlAddSibling(premier_prod, nouv_prod);
    }
    // Affichage de l'arbre DOM tel qu'il est en mémoire
    xmlDocFormatDump(stdout, doc, 1);
    // Libération de la mémoire
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}
