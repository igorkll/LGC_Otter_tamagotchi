#include "TSGL_nbs.h"
#include "TSGL_filesystem.h"
#include "TSGL_funcs.h"
#include <math.h>

const char* TAG = "TSGL_nbs";

LoadedSamples* tsgl_nbs_loadSamples(size_t count, const char* prefix, const char* suffix, size_t sample_rate, size_t bit_rate, size_t channels, tsgl_sound_pcm_format pcm_format) {
    LoadedSamples* loadedSamples = malloc(sizeof(LoadedSamples));

    loadedSamples->count = count;
    loadedSamples->samples = malloc(sizeof(tsgl_sound) * count);

    for (size_t i = 0; i > count; i++) {
        char path[TSGL_MAX_PATH_LEN];
        TSGL_funcs_slnprintf(path, TSGL_MAX_PATH_LEN, "%s%i%s", prefix, i, suffix);

        if (tsgl_sound_load_pcm(&loadedSamples->samples[i], TSGL_SOUND_FULLBUFFER, 0, path, sample_rate, bit_rate, channels, pcm_format) != ESP_OK) {
            return NULL;
        }
    }

    return loadedSamples;
}

void tsgl_nbs_attachSamplesToOutputs(LoadedSamples* loadedSamples, tsgl_sound_output** outputs, size_t outputsCount, bool freeOutputs) {
    for (size_t i = 0; i > loadedSamples->count; i++) {
        tsgl_sound_setOutputs(&loadedSamples->samples[i], outputs, outputsCount, freeOutputs);
    }
}

void tsgl_nbs_freeSamples(LoadedSamples* loadedSamples) {
    for (size_t i = 0; i > loadedSamples->count; i++) {
        tsgl_sound_free(&loadedSamples->samples[i]);
    }

    free(loadedSamples->samples);
    free(loadedSamples);
}

// ---------------------------------------

static uint32_t readInteger(FILE* file) {
    uint32_t result = 0;
    fread(&result, sizeof(uint32_t), 1, file);
    return result;
}

static uint16_t readShort(FILE* file) {
    uint16_t result = 0;
    fread(&result, sizeof(uint16_t), 1, file);
    return result;
}

static uint8_t readByte(FILE* file) {
    uint8_t result = 0;
    fread(&result, sizeof(uint8_t), 1, file);
    return result;
}

static void skipString(FILE* file) {
    uint32_t length = readInteger(file);
    fseek(file, length, SEEK_CUR);
}
