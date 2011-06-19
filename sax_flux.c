#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

// Appelée à chaque début d'élément
void debut_element(void *user_data, const xmlChar *name, const xmlChar **attrs) {
    printf("Début de l'élément : %s\n", name);
}

// Bufferise ce qui vient d'être lu
int sax_read(void *ctxt, char *buf, int len) {
    int fd = *((int *) ctxt);
    return read(fd, buf, len);
}

// Fermeture du flux
int sax_close(void *ctxt) {
    int fd = *((int *) ctxt);
    if (fd > 2) {
        return close(fd);
    }
    return 0;
}

int main() {
    int fd;
    xmlParserCtxtPtr ctxt;
    xmlSAXHandler sax = { 0 };

    // Affectation des fonctions de rappels
    sax.startElement = debut_element;
    // Ouverture du flux
    if ((fd = open("catalogue.xml", O_RDONLY)) == -1) {
        fprintf(stderr, "Echec sur open\n");
        return EXIT_FAILURE;
    }
    // Création du contexte
    if ((ctxt = xmlCreateIOParserCtxt(&sax, NULL, sax_read, sax_close, &fd, XML_CHAR_ENCODING_NONE)) == NULL) {
        fprintf(stderr, "Erreur lors de la création du contexte\n");
        return EXIT_FAILURE;
    }
    // Parsing du document
    xmlParseDocument(ctxt);
    // Libération de la mémoire
    xmlFreeParserCtxt(ctxt);

    return EXIT_SUCCESS;
}

