#include "TSGL_nbs.h"
#include "TSGL_filesystem.h"
#include "TSGL_funcs.h"
#include <math.h>

const char* TAG = "TSGL_nbs";

tsgl_nbs_loadedSamples* tsgl_nbs_loadSamples(size_t count, const char* prefix, const char* suffix, size_t sample_rate, size_t bit_rate, size_t channels, tsgl_sound_pcm_format pcm_format) {
    tsgl_nbs_loadedSamples* loadedSamples = malloc(sizeof(tsgl_nbs_loadedSamples));
    if (loadedSamples == NULL) return NULL;

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

void tsgl_nbs_freeSamples(tsgl_nbs_loadedSamples* loadedSamples) {
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

// ---------------------------------------

static void nbs_player_task(tsgl_nbs* nbs) {
    uint16_t length = readShort(nbs->file);
    uint16_t tempo;
    bool newFormat = length == 0;
    if (newFormat) {
        readByte(nbs->file); //version
        readByte(nbs->file); //vanilla instrument count
        readShort(nbs->file); //song_length
        readShort(nbs->file); //layer_count
        skipString(nbs->file);
        skipString(nbs->file);
        skipString(nbs->file);
        skipString(nbs->file);
        tempo = readShort(nbs->file);

        for (size_t i = 1; i <= 3; i++) readByte(nbs->file);
        for (size_t i = 1; i <= 5; i++) readInteger(nbs->file);
        skipString(nbs->file);
        for (size_t i = 1; i <= 3; i++) readByte(nbs->file);
    } else {
        readShort(nbs->file); //height
        skipString(nbs->file);
        skipString(nbs->file);
        skipString(nbs->file);
        skipString(nbs->file);
        tempo = readShort(nbs->file);
    
        for (size_t i = 1; i <= 3; i++) readByte(nbs->file);
        for (size_t i = 1; i <= 5; i++) readInteger(nbs->file);
        skipString(nbs->file);
    }

    size_t dataStartPos = ftell(nbs->file);
    uint32_t sleep = 1000.0 / (tempo / 100.0);

    while (true) {
        uint16_t step = readShort(nbs->file);
        if (step == 0) {
            fseek(nbs->file, dataStartPos, SEEK_SET);
            continue;
        }
        if (newFormat) step /= 256;

        while (true) {
            uint16_t jump = readShort(nbs->file);
            if (jump == 0) {
                break;
            }

            if (newFormat) readByte(nbs->file);
            uint8_t inst = readByte(nbs->file);
            uint8_t note = readByte(nbs->file);
            if (newFormat) {
                readByte(nbs->file);
                readShort(nbs->file);
            }

            for (size_t i = 0; i < TSGL_NBS_MAX_ACTIVE_NOTES; i++) {
                tsgl_sound* active_note = &nbs->active_notes[i];
                if (!active_note->playing) {
                    if (inst < nbs->loadedSamples->count) {
                        tsgl_sound_instance(active_note, &nbs->loadedSamples->samples[inst]);
                        tsgl_sound_setOutputs(active_note, nbs->outputs, nbs->outputsCount, false);
                        tsgl_sound_setSpeed(active_note, pow(2, (note - 45) / 12.0));
                        tsgl_sound_setVolume(active_note, nbs->volume);
                        tsgl_sound_play(active_note);
                    }
                    break;
                }
            }
        }

        vTaskDelay((sleep*step) / portTICK_PERIOD_MS);
    }

    if (nbs->file_opened) {
        fclose(nbs->file);
        nbs->file_opened = false;
    }

    nbs->task_created = false;
    nbs->playing = false;

    vTaskDelete(NULL);
}

// ---------------------------------------

tsgl_nbs* tsgl_nbs_load(tsgl_nbs_loadedSamples* loadedSamples, const char* path) {
    tsgl_nbs* nbs = calloc(1, sizeof(tsgl_nbs));
    if (nbs == NULL) return NULL;

    nbs->loadedSamples = loadedSamples;

    return nbs;
}

void tsgl_nbs_play(tsgl_nbs* nbs) {
    nbs->playing = true;

    if (nbs->task_created) {
        vTaskResume(nbs->task);
        return;
    }

    if (!nbs->file_opened) {
        nbs->file = tsgl_filesystem_open(path, "rb");
        nbs->file_opened = true;
    }
    
    xTaskCreate((TaskFunction_t)nbs_player_task, NULL, TSGL_NBS_STACK_SIZE, nbs, configMAX_PRIORITIES - 1, &nbs->task);
    nbs->task_created = true;
}

void tsgl_nbs_stop(tsgl_nbs* nbs) {
	if (nbs->task_created) {
		vTaskSuspend(nbs->task);
		nbs->task_created = false;
	}

    nbs->playing = false;
}

void tsgl_nbs_setOutputs(tsgl_nbs* nbs, tsgl_sound_output** outputs, size_t outputsCount) {
    nbs->outputs = outputs;
    nbs->outputsCount = outputsCount;
}

void tsgl_nbs_setVolume(tsgl_nbs* nbs, float volume) {
    nbs->volume = volume;

    for (size_t i = 0; i < TSGL_NBS_MAX_ACTIVE_NOTES; i++) {
        tsgl_sound* active_note = &nbs->active_notes[i];
        if (active_note->playing) {
            tsgl_sound_setVolume(active_note, volume);
        }
    }
}

void tsgl_nbs_setLoop(tsgl_nbs* nbs, float loop) {
    nbs->loop = loop;
}

void tsgl_nbs_free(tsgl_nbs* nbs) {
    if (nbs->task_created) {
		vTaskDelete(nbs->task);
		nbs->task_created = false;
	}

    if (nbs->file_opened) {
        fclose(nbs->file);
        nbs->file_opened = false;
    }

    free(nbs);
}
