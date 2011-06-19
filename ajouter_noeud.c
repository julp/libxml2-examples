#include <stdlib.h>
#include <stdio.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

/**
 * Retourne un nouveau noeud correspondant � un produit (NULL en cas d'erreur)
 **/
xmlNodePtr creer_produit(const char *reference, const char *intitule, const char *prix) {
    xmlNodePtr noeud_produit;
    xmlNodePtr noeud_intitule;

    // Cr�ation du noeud "produit"
    if ((noeud_produit = xmlNewNode(NULL, "produit")) == NULL) {
        return NULL;
    }
    // Ajout de son attribut "reference"
    if (xmlSetProp(noeud_produit, "reference", reference) == NULL) {
        xmlFreeNode(noeud_produit);
        return NULL;
    }
    // Cr�ation du noeud intitule
    if ((noeud_intitule = xmlNewNode(NULL, "intitule")) == NULL) {
        xmlFreeNode(noeud_produit);
        return NULL;
    }
    xmlNodeSetContent(noeud_intitule, intitule);
    // Ajout du noeud (intitule) � son p�re (produit)
    if (xmlAddChild(noeud_produit, noeud_intitule) == NULL) {
        xmlFreeNode(noeud_produit);
        xmlFreeNode(noeud_intitule);
        return NULL;
    }
    // Equivalent plus rapide par rapport au noeud intitule
    // Cr�ation du noeud "prix" et ajout � son p�re (produit)
    if (xmlNewTextChild(noeud_produit, NULL, "prix", prix) == NULL) {
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
    ctxt = xmlXPathNewContext(doc);
    if (ctxt != NULL) {
        xpathRes = xmlXPathEvalExpression("/catalogue/produit[position()=1]", ctxt);
        if (xpathRes && xpathRes->type == XPATH_NODESET && xpathRes->nodesetval->nodeNr == 1) {
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
    // R�cup�ration de la racine
    racine = xmlDocGetRootElement(doc);
    if (racine == NULL) {
        fprintf(stderr, "Document XML vierge\n");
        xmlFreeDoc(doc);
        return EXIT_FAILURE;
    }
    // R�cup�ration du premier produit
    premier_prod = obtenir_premier_produit(doc);
    if (premier_prod == NULL) {
        fprintf(stderr, "Impossible de trouver le premier produit\n");
        xmlFreeDoc(doc);
        return EXIT_FAILURE;
    }
    // Ajout d'un nouveau produit avant le premier produit (en t�te)
    nouv_prod = creer_produit("CD0YAH", "Autocollant Developpez.com", "0.80");
    if (nouv_prod) {
        xmlAddPrevSibling(premier_prod, nouv_prod);
    }
    // Ajout d'un nouveau produit apr�s le premier produit
    nouv_prod = creer_produit("U0TZ6K", "Lot de 10 autocollants Developpez.com", "5.00");
    if (nouv_prod) {
        xmlAddNextSibling(premier_prod, nouv_prod);
    }
    // Ajout d'un nouveau produit en fin/queue
    nouv_prod = creer_produit("ZQEYAN", "Mug Developpez.com", "4.00");
    if (nouv_prod) {
        xmlAddSibling(premier_prod, nouv_prod);
    }
    // Affichage de l'arbre DOM tel qu'il est en m�moire
    xmlDocFormatDump(stdout, doc, 1);
    // Lib�ration de la m�moire
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}
