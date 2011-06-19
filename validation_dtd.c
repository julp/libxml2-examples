#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/valid.h>

enum {
    ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
    NOT_VALID = 0,      // Le document n'est pas valide
    VALID = 1           // Le document est valide
};
 
/**
 * Fonction de validation d'un arbre DOM à l'aide d'une DTD
 **/
int validation_dtd(xmlDocPtr doc, const char *fichier_dtd, int afficher_erreurs) {
    int ret;
    xmlDtdPtr dtd;
    xmlValidCtxtPtr vctxt;

    // Traitement de la DTD
    if ((dtd = xmlParseDTD(NULL, fichier_dtd)) == NULL) {
        return ERROR_OCCURED;
    }
    // Création du contexte de validation
    if ((vctxt = xmlNewValidCtxt()) == NULL) {
        xmlFreeDtd(dtd);
        return ERROR_OCCURED;
    }
    // Affichage des erreurs de validation
    if (afficher_erreurs) {
        vctxt->userData = (void *) stderr;
        vctxt->error = (xmlValidityErrorFunc) fprintf;
        vctxt->warning = (xmlValidityWarningFunc) fprintf;
    }
    // Validation
    ret = xmlValidateDtd(vctxt, doc, dtd);
    // Libération de la mémoire
    xmlFreeValidCtxt(vctxt);
    xmlFreeDtd(dtd);

    return ret;
}

void usage() {
    printf("validation_dtd [fichier XML à valider] [fichier DTD]\n");
}

int main(int argc, char **argv) {
    xmlDocPtr doc;

    if (argc != 3) {
        usage();
        return EXIT_FAILURE;
    }
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    // Ouverture du fichier XML et transformation de celui-ci en un arbre DOM
    if ((doc = xmlParseFile(argv[1])) == NULL) {
        printf("Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // Validation
    if (validation_dtd(doc, argv[2], 1) == VALID) {
        printf("Le document est valide\n");
    } else {
        printf("Le document n'est pas valide ou une erreur interne est survenue\n");
    }
    // Libération de la mémoire
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}

