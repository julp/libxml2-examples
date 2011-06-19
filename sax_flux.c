#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

// Appel�e � chaque d�but d'�l�ment
void debut_element(void *user_data, const xmlChar *name, const xmlChar **attrs) {
    printf("D�but de l'�l�ment : %s\n", name);
}

// Bufferise ce qui vient d'�tre lu
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
    // Cr�ation du contexte
    if ((ctxt = xmlCreateIOParserCtxt(&sax, NULL, sax_read, sax_close, &fd, XML_CHAR_ENCODING_NONE)) == NULL) {
        fprintf(stderr, "Erreur lors de la cr�ation du contexte\n");
        return EXIT_FAILURE;
    }
    // Parsing du document
    xmlParseDocument(ctxt);
    // Lib�ration de la m�moire
    xmlFreeParserCtxt(ctxt);

    return EXIT_SUCCESS;
}

