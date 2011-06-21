#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/valid.h>

#ifndef EXIT_USAGE
# define EXIT_USAGE 2
#endif /* !EXIT_USAGE */

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
    if (NULL == (dtd = xmlParseDTD(NULL, BAD_CAST fichier_dtd))) {
        return ERROR_OCCURED;
    }
    // Création du contexte de validation
    if (NULL == (vctxt = xmlNewValidCtxt())) {
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
    fprintf(stderr, "validation_dtd [fichier XML à valider] [fichier DTD]\n");
    exit(EXIT_USAGE);
}

int main(int argc, char **argv) {
    int ret;
    xmlDocPtr doc;

    ret = EXIT_SUCCESS;
    if (3 != argc) {
        usage();
    }
    xmlKeepBlanksDefault(0); // Ignore les noeuds texte composant la mise en forme
    // Ouverture du fichier XML et transformation de celui-ci en un arbre DOM
    if (NULL == (doc = xmlParseFile(argv[1]))) {
        printf("Document XML invalide\n");
        return EXIT_FAILURE;
    }
    // Validation
    if (VALID == validation_dtd(doc, argv[2], 1)) {
        printf("Le document est valide\n");
    } else {
        fprintf(stderr, "Le document n'est pas valide ou une erreur interne est survenue\n");
        ret = EXIT_FAILURE;
    }
    // Libération de la mémoire
    xmlFreeDoc(doc);

    return ret;
}
