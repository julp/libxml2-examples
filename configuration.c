#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "port/asprintf.c"

typedef struct {
    char *fichier;
    xmlDocPtr doc;
    xmlNodePtr racine;
    xmlXPathContextPtr ctxt;
} xmlConfig_t;

/**
 * Libération de la mémoire pour la structure xmlConfig_t
 **/
void free_config(xmlConfig_t *conf) {
    xmlXPathFreeContext(conf->ctxt);
    xmlFreeDoc(conf->doc);
    free(conf->fichier);
    free(conf);
}

/**
 * Initialisation et chargement du fichier XML en mémoire
 **/
xmlConfig_t *load_config(const char *fichier) {
    xmlConfig_t *conf, null = { 0 };

    if ((conf = malloc(sizeof *conf)) == NULL) {
        return NULL;
    }
    memcpy(conf, &null, sizeof(*conf));
    // Copie du nom du fichier
    if ((conf->fichier = strdup(fichier)) == NULL) {
        free_config(conf);
        return NULL;
    }
    // Création de l'arbre DOM à partir du fichier XML
    xmlKeepBlanksDefault(0);
    if ((conf->doc = xmlParseFile(conf->fichier)) == NULL) {
        free_config(conf);
        return NULL;
    }
    // Récupération de la racine
    conf->racine = xmlDocGetRootElement(conf->doc);
    if (conf->racine != NULL && xmlStrcasecmp(conf->racine->name, "configuration")) {
        free_config(conf);
        return NULL;
    }
    // Initialisation de l'environnement XPath
    xmlXPathInit();
    // Création d'un contexte pour les requêtes XPath
    conf->ctxt = xmlXPathNewContext(conf->doc);
    if (conf->ctxt == NULL) {
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
    xmlXPathObjectPtr xpathRes;
    xmlNodePtr n = NULL;
    char *path;

    asprintf(&path, "/configuration/directive[@nom=\"%s\"]", directive);
    // Evaluation de l'expression XPath
    xpathRes = xmlXPathEvalExpression(path, conf->ctxt);
    free(path);
    if (xpathRes && xpathRes->type == XPATH_NODESET && xpathRes->nodesetval->nodeNr == 1) {
        n = xpathRes->nodesetval->nodeTab[0];
    }
    xmlXPathFreeObject(xpathRes);

    return n;
}

/**
 * Renvoie la valeur d'une directive
 **/
const char *get_config(xmlConfig_t *conf, const char *directive) {
    xmlNodePtr n = _get_node_by_xpath(conf, directive);

    if (n != NULL) {
        return xmlNodeGetContent(n);
    }
    return NULL;
}

/**
 * Modifie la valeur d'une directive si celle-ci est présente dans le fichier XML
 * sinon elle est ajoutée
 **/
int set_config(xmlConfig_t *conf, const char *directive, const char *valeur) {
    xmlNodePtr n = _get_node_by_xpath(conf, directive);

    if (n == NULL) { // La directive n'existe pas : ajout
        xmlNodePtr new_dir = xmlNewTextChild(conf->racine, NULL, "directive", valeur);
        if (new_dir == NULL) {
            return 0;
        }
        if (xmlSetProp(new_dir, "nom", directive) == NULL) {
            return 0;
        }
    } else { // La directive existe : modification
        xmlNodeSetContent(n, valeur);
    }
    return (xmlSaveFormatFile(conf->fichier, conf->doc, 1) > 0 ? 1 : 0);
}


/**
 * Affichage de la valeur d'une directive
 **/
void print_config(xmlConfig_t *conf, const char *directive) {
    const char *val = get_config(conf, directive);

    printf("Valeur de '%s' : %s\n", directive, val != NULL ? val : "(directive inconnue)");
}

int main() {
    xmlConfig_t *conf;

    // Chargement du fichier et initialisation
    conf = load_config("configuration.xml");

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

