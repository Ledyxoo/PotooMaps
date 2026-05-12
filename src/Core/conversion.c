#include "settings.h"
#include "uniStr.h"
#include "graph.h"
#include "shortestPath.h"
#include "graph.h"
#include "path.h"
#include "intTree.h"
#include "intList.h"
#include "cJSON.h"
#include "conversion.h"
#define BIN_PATH "Assets/esiea.bin"


FILE* GeoJSON_to_binary(char* path)
{
    FILE* rawmap = fopen(path, "r");
    if (!rawmap)
    {
        printf("Error opening file %s\n", path);
        return NULL;
    }
    fseek(rawmap, 0, SEEK_END);
    long fsize = ftell(rawmap);
    fseek(rawmap, 0, SEEK_SET);
    printf("size = %ld\n", fsize);
    char* rawdata = calloc(1, fsize + 1);
    fread(rawdata, 1, fsize, rawmap);
    fclose(rawmap);
    //Convertie chaque ligne en JSON

    char* line = strtok(rawdata, "\n");
    cJSON* json_array = cJSON_CreateArray();
    while (line != NULL) {
        cJSON* json = cJSON_Parse(line);
        if (json) cJSON_AddItemToArray(json_array, json);
        else
            printf("Error parsing JSON at line: %s\n", line);
        line = strtok(NULL, "\n");
    }

    UniStr* binaryStr = UniStr_decodeU8(cJSON_PrintUnformatted(json_array), fsize);
    if (!binaryStr) {
        printf("Error: Failed to decode JSON string.\n");
        cJSON_Delete(json_array);
        free(rawdata);
        return NULL;
    }

    // Écrire la chaîne binaire dans un fichier
    FILE* binFile = fopen(BIN_PATH, "wb");
    if (!binFile) {
        printf("Error: Failed to open binary file for writing.\n");
        UniStr_destroy(binaryStr);
        cJSON_Delete(json_array);
        free(rawdata);
        return NULL;
    }
    fwrite(binaryStr->data, binaryStr->length * sizeof(char32), 1, binFile);

    return binFile;
    fclose(binFile);
    UniStr_destroy(binaryStr);
    cJSON_Delete(json_array);
    free(rawdata);
}