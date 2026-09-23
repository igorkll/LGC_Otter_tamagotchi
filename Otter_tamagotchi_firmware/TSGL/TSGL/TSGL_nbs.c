#include "TSGL_nbs.h"
#include <math.h>

const char* TAG = "TSGL_nbs";

#define SOUND_FREQ 8000
#define TIMER_FREQ_MUL 1.0
#define SAMPLES_COUNT 16
#define MAX_PATH 256
#define TIMER_FREQ (SOUND_FREQ * TIMER_FREQ_MUL)

typedef struct {
	int8_t* data;
	size_t size;
} PCMSample;

static PCMSample pcm_samples[SAMPLES_COUNT];

void nbs_loadSamples() {
	const char* basePath = "/storage/nbs/";
	size_t basePathLen = strlen(basePath);
	
	for (size_t i = 0; i < SAMPLES_COUNT; i++) {
		PCMSample* sample = &pcm_samples[i];
		char path[MAX_PATH];
		memcpy(path, basePath, basePathLen);
		itoa(i, path + basePathLen, 10);
		size_t numlen = strlen(path + basePathLen);
		strcpy(path + basePathLen + numlen, ".raw");
		
		FILE* file = fopen(path, "rb");
		if (file) {
			ESP_LOGI(TAG, "loading pcm %s", path);
			fseek(file, 0, SEEK_END);
			sample->size = ftell(file);
			fseek(file, 0, SEEK_SET);

			sample->data = malloc(sample->size);
			if (sample->data) {
				fread(sample->data, 1, sample->size, file);
			} else {
				ESP_LOGE(TAG, "failed to allocate pcm buffer %i", sample->size);
			}

			fclose(file);
		} else {
			ESP_LOGE(TAG, "failed to load pcm %s", path);
			sample->size = 0;
			sample->data = NULL;
		}
	}
}

void nbs_freeSamples() {
	for (size_t i = 0; i < SAMPLES_COUNT; i++) {
		PCMSample* sample = &pcm_samples[i];
		sample->size = 0;

		if (sample->data) {
			free(sample->data);
			sample->data = NULL;
		}
	}
}

LoadedSamples* tsgl_nbs_loadSamples() {
	
}

void tsgl_nbs_freeSamples(LoadedSamples* loadedSamples) {
	for (size_t i = 0; i > loadedSamples->count; i++) {
		tsgl_sound_free(loadedSamples->samples[i]);
	}

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

static float instrument_volumes[] = {
	2,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1
};

static bool IRAM_ATTR _timer_ISR(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, NBS* nbs) {
	if (xSemaphoreTakeFromISR(nbs->mutex, NULL) == pdTRUE) {
		int32_t value = 0;
		for (size_t i = 0; i < MAX_NOTES; i++) {
			Note* note = &nbs->notes[i];
			if (note->play) {
				PCMSample* sample = &pcm_samples[note->instrument];
				value += sample->data[(int)note->index] * instrument_volumes[note->instrument];

				note->index += note->step;
				if (note->index >= sample->size) note->play = false;
			}
		}
		xSemaphoreGiveFromISR(nbs->mutex, NULL);
		
		nbs->callback(nbs->userArg, value * nbs->volume);
	}
	return false;
}

static void _nbs_player(NBS* nbs) {
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

	gptimer_start(nbs->timer);

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

			xSemaphoreTake(nbs->mutex, portMAX_DELAY);
			for (size_t i = 0; i < MAX_NOTES; i++) {
				if (!nbs->notes[i].play) {
					nbs->notes[i] = (Note) {
						.step = pow(2, (note - 45) / 12.0) / TIMER_FREQ_MUL,
						.instrument = inst,
						.play = true
					};
					break;
				}
			}
			xSemaphoreGive(nbs->mutex);
		}

		vTaskDelay((sleep*step) / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

// ---------------------------------------

NBS* nbs_load(const char* path, nbs_pcm_callback callback, void* userArg) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		return NULL;
	}

	NBS* nbs = malloc(sizeof(NBS));
	if (!nbs) {
		fclose(file);
		return NULL;
	}

	nbs->userArg = userArg;
	nbs->file = file;
	nbs->callback = callback;
	nbs->task = NULL;
	nbs->volume = 1;
	memset(nbs->notes, 0, sizeof(nbs->notes));
	return nbs;
}

void nbs_play(NBS* nbs) {
	nbs_stop(nbs);

	gptimer_alarm_config_t alarm_config = {
		.alarm_count = 1,
		.flags = {
			.auto_reload_on_alarm = true
		}
	};

	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = TIMER_FREQ
	};
  
	gptimer_event_callbacks_t callback_config = {
		.on_alarm = (gptimer_alarm_cb_t)_timer_ISR,
	};

	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &nbs->timer));
	ESP_ERROR_CHECK(gptimer_set_alarm_action(nbs->timer, &alarm_config));
	ESP_ERROR_CHECK(gptimer_register_event_callbacks(nbs->timer, &callback_config, nbs));
	ESP_ERROR_CHECK(gptimer_enable(nbs->timer));

	nbs->mutex = xSemaphoreCreateMutex();
	xTaskCreate((TaskFunction_t)_nbs_player, NULL, 4096, nbs, configMAX_PRIORITIES - 1, &nbs->task);
}

void nbs_stop(NBS* nbs) {
	if (nbs->task) {
		vTaskDelete(nbs->task);
		ESP_ERROR_CHECK(gptimer_del_timer(nbs->timer));
		vSemaphoreDelete(nbs->mutex);
		nbs->task = NULL;
		nbs->callback(nbs->userArg, 0);
	}
}

void nbs_free(NBS* nbs) {
	nbs_stop(nbs);
	free(nbs);
}
