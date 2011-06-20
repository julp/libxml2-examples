#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "port/asprintf.c"

#define NOM_RACINE "configuration"

typedef struct {
    char *fichier;
    xmlDocPtr doc;
    xmlNodePtr racine;
    xmlXPathContextPtr ctxt;
} xmlConfig_t;

/**
 * Libération des ressources de la structure xmlConfig_t
 **/
void free_config(xmlConfig_t *conf) {
    if (NULL != conf->ctxt) {
        xmlXPathFreeContext(conf->ctxt);
    }
    if (NULL != conf->doc) {
        xmlFreeDoc(conf->doc);
    }
    if (NULL != conf->fichier) {
        free(conf->fichier);
    }
    free(conf);
}

/**
 * Initialisation et chargement du fichier XML en mémoire
 **/
xmlConfig_t *load_config(const char *fichier) {
    struct stat sts;
    xmlConfig_t *conf, null = { 0 };

    if (NULL == (conf = malloc(sizeof(*conf)))) {
        return NULL;
    }
    memcpy(conf, &null, sizeof(*conf));
    // Copie du nom du fichier
    if (NULL == (conf->fichier = strdup(fichier))) {
        free_config(conf);
        return NULL;
    }
    // Création de l'arbre DOM à partir du fichier XML
    xmlKeepBlanksDefault(0);
    if (-1 == stat(fichier, &sts)) {
        if (ENOENT == errno) { // le fichier n'existe pas, on crée un nouvel arbre en mémoire
            conf->doc = xmlNewDoc(BAD_CAST "1.0");
            conf->racine = xmlNewNode(NULL, BAD_CAST NOM_RACINE);
            xmlDocSetRootElement(conf->doc, conf->racine);
        } else {
            perror("stat");
        }
    } else { // le fichier existe : on le charge
        if (NULL == (conf->doc = xmlParseFile(conf->fichier))) {
            free_config(conf);
            return NULL;
        }
        // Récupération de la racine
        conf->racine = xmlDocGetRootElement(conf->doc);
        if (NULL != conf->racine && 0 != xmlStrcasecmp(conf->racine->name, BAD_CAST NOM_RACINE)) {
            free_config(conf);
            return NULL;
        }
    }
    // Initialisation de l'environnement XPath
    xmlXPathInit();
    // Création d'un contexte pour les requêtes XPath
    conf->ctxt = xmlXPathNewContext(conf->doc);
    if (NULL == conf->ctxt) {
        free_config(conf);
        return NULL;
    }

    return conf;
}

/**
 * Fonction privée retournant l'élément <directive> via XPath correspondant
 * à l'attribut nom
 **/
static xmlNodePtr _get_node_by_xpath(xmlConfig_t *conf, const char *directive) {
    char *path;
    xmlNodePtr n = NULL;
    xmlXPathObjectPtr xpathRes;

    if (-1 == asprintf(&path, "/configuration/directive[@nom=\"%s\"]", directive)) {
        fprintf(stderr, "asprintf failed\n");
        return NULL;
    }
    // Evaluation de l'expression XPath
    xpathRes = xmlXPathEvalExpression(BAD_CAST path, conf->ctxt);
    if (NULL != xpathRes && XPATH_NODESET == xpathRes->type && 1 == xpathRes->nodesetval->nodeNr) {
        n = xpathRes->nodesetval->nodeTab[0];
    }
    free(path);
    xmlXPathFreeObject(xpathRes);

    return n;
}

/**
 * Renvoie la valeur d'une directive
 **/
const xmlChar *get_config(xmlConfig_t *conf, const char *directive) {
    xmlNodePtr n;

    if (NULL != (n = _get_node_by_xpath(conf, directive))) {
        return xmlNodeGetContent(n);
    }

    return NULL;
}

/**
 * Modifie la valeur d'une directive si celle-ci est présente dans le fichier XML
 * sinon elle est ajoutée
 **/
int set_config(xmlConfig_t *conf, const char *directive, const char *valeur) {
    xmlNodePtr n;

    if (NULL == (n = _get_node_by_xpath(conf, directive))) { // La directive n'existe pas : ajout
        xmlNodePtr new_dir;

        if (NULL == (new_dir = xmlNewTextChild(conf->racine, NULL, BAD_CAST "directive", BAD_CAST valeur))) {
            return 0;
        }
        if (NULL == xmlSetProp(new_dir, BAD_CAST "nom", BAD_CAST directive)) {
            return 0;
        }
    } else { // La directive existe : modification
        xmlNodeSetContent(n, BAD_CAST valeur);
    }
    return !!xmlSaveFormatFile(conf->fichier, conf->doc, 1);
}


/**
 * Affichage de la valeur d'une directive
 **/
void print_config(xmlConfig_t *conf, const char *directive) {
    const xmlChar *val = get_config(conf, directive);

    printf("Valeur de '%s' : %s\n", directive, NULL != val ? (char *) val : "(directive inconnue)");
}

int main() {
    xmlConfig_t *conf;

    // Chargement du fichier et initialisation
    if (NULL == (conf = load_config("configuration.xml"))) {
        return EXIT_FAILURE;
    }

    // Récupération de la valeur des directives
    print_config(conf, "directive_inexistante");
    print_config(conf, "verifier_version");

    // Ajout/Modification de directives
    set_config(conf, "verifier_version", "Off");
    set_config(conf, "taille_police", "12");

    // Vérification des modifications
    print_config(conf, "verifier_version");
    print_config(conf, "taille_police");

    // Libération de la mémoire
    free_config(conf);

    return EXIT_SUCCESS;
}

