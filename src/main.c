#include "dict.h"
#include "shortestPath.h"
#include "cJSON.h"
#include <ctype.h>
#include <errno.h>

#define MAXSIZE 1000000
#define EARTHRADIUS 6371.0

typedef struct Coord_s { double lat; double lon; } Coord;
typedef struct Nearest_s {
    Coord start; Coord end;
    size_t startIndex; double startDist;
    size_t endIndex; double endDist;
} Nearest;

typedef struct DatasetConfig_s {
    const char* name;
    const char* sourcePath;
    const char* cleanedPath;
} DatasetConfig;

static const DatasetConfig DATASETS[] = {
    { "esiea", "Assets/esiea.geojson", "Assets/DataPerso/esieaclean" },
    { "laval", "Assets/laval.geojson", "Assets/DataPerso/lavalclean" },
    { "mayenne", "Assets/mayenne.geojson", "Assets/DataPerso/mayenneclean" },
    { "pdll", "Assets/pays_de_la_loire.geojson", "Assets/DataPerso/pdllclean" },
    { "centre", "Assets/centre.geojson", "Assets/DataPerso/centreclean" }
};

Dict* createCrosswayDict(FILE* file);
Graph* createCrosswayGraph(FILE* file, Dict* dict);
void setNearestCrossways(Dict* dict, Nearest* nearest);
void printPathJSON(IntList* list, Dict* dict, Nearest* nearest);
int IntList_getIndex(IntList* list, int value);
double haversineFormula(double lat1, double lon1, double lat2, double lon2);
void JsonClean(const char* pathout, FILE* file);
bool parseCoordinate(const char* input, double min, double max, double* out);
const DatasetConfig* findDataset(const char* datasetName);
void printDatasetUsage(void);
bool buildCustomCleanPath(const char* sourcePath, char* outPath, size_t outPathSize);
bool isValidCustomSourcePath(const char* sourcePath);

int main(int argc, char** argv) {
    const DatasetConfig* dataset = NULL;
    DatasetConfig customDataset = { "custom", NULL, NULL };
    char customCleanPath[256];
    int coordStartIndex = 0;

    if (argc >= 2 && strcmp(argv[1], "custom") == 0) {
        if (argc != 7) {
            printf("Usage: %s custom <source_geojson> <start_lat> <start_lon> <end_lat> <end_lon>\n", argv[0]);
            printDatasetUsage();
            return 1;
        }

        if (!isValidCustomSourcePath(argv[2])) {
            printf("Error: Invalid custom source path. Use a .geojson file under Assets/.\n");
            return 1;
        }

        if (!buildCustomCleanPath(argv[2], customCleanPath, sizeof(customCleanPath))) {
            printf("Error: Invalid custom source path.\n");
            return 1;
        }

        customDataset.sourcePath = argv[2];
        customDataset.cleanedPath = customCleanPath;
        dataset = &customDataset;
        coordStartIndex = 3;
    } else {
        if (argc != 6) {
            printf("Usage: %s <dataset> <start_lat> <start_lon> <end_lat> <end_lon>\n", argv[0]);
            printDatasetUsage();
            printf("Latitude range: [-90, 90], Longitude range: [-180, 180]\n");
            return 1;
        }

        dataset = findDataset(argv[1]);
        if (!dataset) {
            printf("Error: Invalid dataset '%s'.\n", argv[1]);
            printDatasetUsage();
            return 1;
        }
        coordStartIndex = 2;
    }

    double startLat, startLon, endLat, endLon;
    if (!parseCoordinate(argv[coordStartIndex], -90.0, 90.0, &startLat) ||
        !parseCoordinate(argv[coordStartIndex + 1], -180.0, 180.0, &startLon) ||
        !parseCoordinate(argv[coordStartIndex + 2], -90.0, 90.0, &endLat) ||
        !parseCoordinate(argv[coordStartIndex + 3], -180.0, 180.0, &endLon)) {
        printf("Error: Invalid coordinates.\n");
        printf("Latitude range: [-90, 90], Longitude range: [-180, 180]\n");
        return 1;
    }

    FILE* file = fopen(dataset->cleanedPath, "r");
    if (file == NULL) {
        printf("Cleaned file not found. Cleaning JSON from source...\n");
        FILE* file2 = fopen(dataset->sourcePath, "r");
        if (!file2) { printf("Error: Cannot open source file %s\n", dataset->sourcePath); return 1; }
        JsonClean(dataset->cleanedPath, file2);
        fclose(file2);
        file = fopen(dataset->cleanedPath, "r");
        if (!file) { printf("Error: Cannot open %s even after cleaning\n", dataset->cleanedPath); return 1; }
    } else {
        printf("Using existing cleaned file: %s\n", dataset->cleanedPath);
    }
    printf("Creating Dictionary...\n");
    Dict* dict = createCrosswayDict(file);
    printf("Dictionary size: %d\n", Dict_size(dict));
    rewind(file);
    printf("Creating Graph...\n");
    Graph* graph = createCrosswayGraph(file, dict);
    Nearest* nearest = calloc(1, sizeof(Nearest));
    AssertNew(nearest);
    nearest->start.lat = startLat;
    nearest->start.lon = startLon;
    nearest->end.lat = endLat;
    nearest->end.lon = endLon;
    printf("Finding nearest crossways...\n");
    setNearestCrossways(dict, nearest);
    if (nearest->startIndex == 0 || nearest->endIndex == 0) {
        printf("Could not find nearby intersections.\n");
    } else {
        printf("Calculating shortest path...\n");
        Path* path = Graph_shortestPath(graph, (int)nearest->startIndex - 1, (int)nearest->endIndex - 1);
        if (path != NULL && path->list != NULL) {
            double totalDistance = (double)path->distance + (double)nearest->startDist + (double)nearest->endDist;
            printf("Distance total: %lf km\n", totalDistance);
            printPathJSON(path->list, dict, nearest);
            Path_destroy(path);
        } else {
            printf("No path found between these points.\n");
            if(path) Path_destroy(path);
        }
    }
    free(nearest);
    Dict_destroy(dict);
    Graph_destroy(graph);
    fclose(file);
    return 0;
}

bool parseCoordinate(const char* input, double min, double max, double* out) {
    if (!input || !out) return false;

    errno = 0;
    char* end = NULL;
    double value = strtod(input, &end);

    if (errno != 0 || end == input || *end != '\0' || !isfinite(value)) return false;
    if (value < min || value > max) return false;

    *out = value;
    return true;
}

const DatasetConfig* findDataset(const char* datasetName) {
    if (!datasetName) return NULL;

    size_t count = sizeof(DATASETS) / sizeof(DATASETS[0]);
    for (size_t i = 0; i < count; i++) {
        if (strcmp(datasetName, DATASETS[i].name) == 0) return &DATASETS[i];
    }

    return NULL;
}

void printDatasetUsage(void) {
    printf("Available datasets:");
    size_t count = sizeof(DATASETS) / sizeof(DATASETS[0]);
    for (size_t i = 0; i < count; i++) {
        printf(" %s", DATASETS[i].name);
    }
    printf("\n");
    printf("Custom dataset usage: custom <source_geojson>\n");
}

bool buildCustomCleanPath(const char* sourcePath, char* outPath, size_t outPathSize) {
    if (!sourcePath || !outPath || outPathSize == 0) return false;

    const char* base = sourcePath;
    const char* slash = strrchr(sourcePath, '/');
    if (slash && *(slash + 1) != '\0') base = slash + 1;
    if (*base == '\0') return false;

    char safeBase[128];
    size_t j = 0;
    for (size_t i = 0; base[i] != '\0' && j < sizeof(safeBase) - 1; i++) {
        unsigned char ch = (unsigned char)base[i];
        if (isalnum(ch) || ch == '_' || ch == '-' || ch == '.') {
            safeBase[j++] = (char)ch;
        } else {
            safeBase[j++] = '_';
        }
    }
    safeBase[j] = '\0';
    if (j == 0) return false;

    int needed = snprintf(outPath, outPathSize, "Assets/DataPerso/custom_%s.clean", safeBase);
    if (needed < 0 || (size_t)needed >= outPathSize) return false;

    return true;
}

bool isValidCustomSourcePath(const char* sourcePath) {
    if (!sourcePath) return false;

    size_t len = strlen(sourcePath);
    if (len == 0 || len >= 240) return false;
    if (strncmp(sourcePath, "Assets/", 7) != 0) return false;
    if (strstr(sourcePath, "..") != NULL) return false;

    const char* ext = strrchr(sourcePath, '.');
    if (!ext || strcmp(ext, ".geojson") != 0) return false;

    return true;
}

void JsonClean(const char* pathout, FILE* file) {
    char* line = calloc(MAXSIZE, sizeof(char));
    FILE* out = fopen(pathout, "w");
    if (!out) { free(line); return; }
    const char* allowed[] = { "motorway", "trunk", "primary", "secondary", "tertiary", "unclassified", "residential", "motorway_link", "trunk_link", "primary_link", "secondary_link", "tertiary_link", "living_street", "service" };
    while (fgets(line, MAXSIZE, file)) {
        cJSON* json = cJSON_Parse(line);
        if (!json) continue;
        cJSON* jNodes = cJSON_GetObjectItem(json, "nodes");
        cJSON* jTags = cJSON_GetObjectItem(json, "tags");
        if (jTags == NULL) jTags = cJSON_GetObjectItem(json, "properties");
        
        cJSON* jHighway = cJSON_GetObjectItem(jTags, "highway");
        if (jHighway && cJSON_IsString(jHighway)) {
            bool ok = false;
            for (int i = 0; i < 14; i++) if (strcmp(jHighway->valuestring, allowed[i]) == 0) { ok = true; break; }
            if (ok) {
                if (jNodes != NULL) { // OSM format
                    char* s = cJSON_PrintUnformatted(jNodes);
                    fprintf(out, "%s\n", s); free(s);
                } else { // GeoJSON format
                    cJSON* geom = cJSON_GetObjectItem(json, "geometry");
                    if (geom && strcmp(cJSON_GetObjectItem(geom, "type")->valuestring, "LineString") == 0) {
                        cJSON* coords = cJSON_GetObjectItem(geom, "coordinates");
                        cJSON* nodes = cJSON_CreateArray();
                        cJSON* c;
                        cJSON_ArrayForEach(c, coords) {
                            cJSON* n = cJSON_CreateObject();
                            char lats[32], lons[32];
                            sprintf(lats, "%.6f", cJSON_GetArrayItem(c, 1)->valuedouble);
                            sprintf(lons, "%.6f", cJSON_GetArrayItem(c, 0)->valuedouble);
                            cJSON_AddStringToObject(n, "lat", lats);
                            cJSON_AddStringToObject(n, "lon", lons);
                            cJSON_AddItemToArray(nodes, n);
                        }
                        char* s = cJSON_PrintUnformatted(nodes);
                        fprintf(out, "%s\n", s); free(s);
                        cJSON_Delete(nodes);
                    }
                }
            }
        }
        cJSON_Delete(json);
    }
    fclose(out); free(line);
}

Dict* createCrosswayDict(FILE* file) {
    char* line = calloc(MAXSIZE, sizeof(char));
    Dict* allNodes = Dict_create();
    Dict* cross = Dict_create();
    size_t id = 1;
    while (fgets(line, MAXSIZE, file)) {
        cJSON* nodes = cJSON_Parse(line);
        if (!nodes) continue;
        cJSON* n;
        cJSON_ArrayForEach(n, nodes) {
            char* s = cJSON_PrintUnformatted(n);
            if (Dict_insert(allNodes, s, (void*)1) != NULL) {
                if (Dict_get(cross, s) == NULL) Dict_insert(cross, s, (void*)id++);
            }
            free(s);
        }
        cJSON_Delete(nodes);
    }
    free(line); Dict_destroy(allNodes);
    return cross;
}

Graph* createCrosswayGraph(FILE* file, Dict* dict) {
    char* line = calloc(MAXSIZE, sizeof(char));
    Graph* g = Graph_create(Dict_size(dict));
    while (fgets(line, MAXSIZE, file)) {
        cJSON* nodes = cJSON_Parse(line);
        if (!nodes) continue;
        int startIdx = -1; double d = 0, pLat, pLon;
        cJSON* n;
        cJSON_ArrayForEach(n, nodes) {
            double lat = atof(cJSON_GetObjectItem(n, "lat")->valuestring);
            double lon = atof(cJSON_GetObjectItem(n, "lon")->valuestring);
            char* s = cJSON_PrintUnformatted(n);
            size_t cid = (size_t)Dict_get(dict, s);
            free(s);
            if (cid > 0) {
                if (startIdx != -1) {
                    d += haversineFormula(pLat, pLon, lat, lon);
                    Graph_set(g, startIdx - 1, (int)cid - 1, (float)d);
                    Graph_set(g, (int)cid - 1, startIdx - 1, (float)d);
                }
                startIdx = (int)cid; d = 0;
            } else if (startIdx != -1) {
                d += haversineFormula(pLat, pLon, lat, lon);
            }
            pLat = lat; pLon = lon;
        }
        cJSON_Delete(nodes);
    }
    free(line); return g;
}

void setNearestCrossways(Dict* dict, Nearest* nearest) {
    nearest->startDist = 1e18; nearest->endDist = 1e18;
    DictIter* it = calloc(1, sizeof(DictIter)); Dict_getIterator(dict, it);
    while (DictIter_hasNext(it)) {
        KVPair* p = DictIter_next(it);
        cJSON* n = cJSON_Parse(p->key);
        double lat = atof(cJSON_GetObjectItem(n, "lat")->valuestring);
        double lon = atof(cJSON_GetObjectItem(n, "lon")->valuestring);
        double d1 = haversineFormula(nearest->start.lat, nearest->start.lon, lat, lon);
        if (d1 < nearest->startDist) { nearest->startIndex = (size_t)p->value; nearest->startDist = d1; }
        double d2 = haversineFormula(nearest->end.lat, nearest->end.lon, lat, lon);
        if (d2 < nearest->endDist) { nearest->endIndex = (size_t)p->value; nearest->endDist = d2; }
        cJSON_Delete(n);
    }
    free(it);
}

double haversineFormula(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * DEG_TO_RAD, dLon = (lon2 - lon1) * DEG_TO_RAD;
    double a = sin(dLat/2)*sin(dLat/2) + cos(lat1*DEG_TO_RAD)*cos(lat2*DEG_TO_RAD)*sin(dLon/2)*sin(dLon/2);
    return EARTHRADIUS * 2 * atan2(sqrt(a), sqrt(1-a));
}

int IntList_getIndex(IntList* list, int value) {
    IntListNode* s = &(list->sentinel); IntListNode* c = s->next;
    int i = 0; while (c != s) { if (c->value == value) return i; c = c->next; i++; }
    return -1;
}

void printPathJSON(IntList* list, Dict* dict, Nearest* nearest) {
    Coord* coords = calloc(list->nodeCount, sizeof(Coord));
    DictIter* it = calloc(1, sizeof(DictIter)); Dict_getIterator(dict, it);
    while (DictIter_hasNext(it)) {
        KVPair* p = DictIter_next(it);
        int idx = IntList_getIndex(list, (int)((size_t)p->value) - 1);
        if (idx != -1) {
            cJSON* n = cJSON_Parse(p->key);
            coords[idx].lat = atof(cJSON_GetObjectItem(n, "lat")->valuestring);
            coords[idx].lon = atof(cJSON_GetObjectItem(n, "lon")->valuestring);
            cJSON_Delete(n);
        }
    }
    free(it);
    cJSON* j = cJSON_CreateObject(); cJSON_AddStringToObject(j, "type", "FeatureCollection");
    cJSON* fs = cJSON_AddArrayToObject(j, "features");
    cJSON* f = cJSON_CreateObject(); cJSON_AddStringToObject(f, "type", "Feature");
    cJSON_AddObjectToObject(f, "properties");
    cJSON* g = cJSON_AddObjectToObject(f, "geometry"); cJSON_AddStringToObject(g, "type", "LineString");
    cJSON* cs = cJSON_AddArrayToObject(g, "coordinates");
    cJSON* c = cJSON_CreateArray(); cJSON_AddItemToArray(c, cJSON_CreateNumber(nearest->start.lon)); cJSON_AddItemToArray(c, cJSON_CreateNumber(nearest->start.lat)); cJSON_AddItemToArray(cs, c);
    for (int i = 0; i < list->nodeCount; i++) {
        c = cJSON_CreateArray(); cJSON_AddItemToArray(c, cJSON_CreateNumber(coords[i].lon)); cJSON_AddItemToArray(c, cJSON_CreateNumber(coords[i].lat)); cJSON_AddItemToArray(cs, c);
    }
    c = cJSON_CreateArray(); cJSON_AddItemToArray(c, cJSON_CreateNumber(nearest->end.lon)); cJSON_AddItemToArray(c, cJSON_CreateNumber(nearest->end.lat)); cJSON_AddItemToArray(cs, c);
    cJSON_AddItemToArray(fs, f);
    char* res = cJSON_Print(j); printf("%s\n", res);
    free(res); cJSON_Delete(j); free(coords);
}
