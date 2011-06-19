#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/relaxng.h>

enum {
    ERROR_OCCURED = -1, // Une erreur est survenue pendant la validation
    NOT_VALID = 0,      // Le document n'est pas valide
    VALID = 1           // Le document est valide
};
 
/**
 * Fonction de validation d'un arbre DOM à l'aide d'une grammaire Relax NG
 **/
int validation_relaxng(xmlDocPtr doc, const char *fichier_rng, int afficher_erreurs) {
    int ret;
    xmlRelaxNGPtr rng;
    xmlRelaxNGParserCtxtPtr pctxt;
    xmlRelaxNGValidCtxtPtr vctxt;

    // Ouverture du fichier Relax NG
    if ((pctxt = xmlRelaxNGNewParserCtxt(fichier_rng)) == NULL) {
        return ERROR_OCCURED;
    }
    // Chargement du contenu de la description Relax NG
    rng = xmlRelaxNGParse(pctxt);
    xmlRelaxNGFreeParserCtxt(pctxt);
    if (rng == NULL) {
        return ERROR_OCCURED;
    }
    // Création du contexte de validation
    if ((vctxt = xmlRelaxNGNewValidCtxt(rng)) == NULL) {
        xmlRelaxNGFree(rng);
        return ERROR_OCCURED;
    }
    // Traitement des erreurs de validation
    if (afficher_erreurs) {
        xmlRelaxNGSetValidErrors(vctxt, (xmlRelaxNGValidityErrorFunc) fprintf, (xmlRelaxNGValidityWarningFunc) fprintf, stderr);
    }
    // Validation
    ret = (xmlRelaxNGValidateDoc(vctxt, doc) == 0 ? VALID : NOT_VALID);
    // Libération de la mémoire
    xmlRelaxNGFreeValidCtxt(vctxt);
    xmlRelaxNGFree(rng);

    return ret;
}

void usage() {
    printf("validation_dtd [fichier XML à valider] [grammaire Relax NG]\n");
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
    if (validation_relaxng(doc, argv[2], 1) == VALID) {
        printf("Le document est valide\n");
    } else {
        printf("Le document n'est pas valide ou une erreur interne est survenue\n");
    }
    // Libération de la mémoire
    xmlFreeDoc(doc);

    return EXIT_SUCCESS;
}

