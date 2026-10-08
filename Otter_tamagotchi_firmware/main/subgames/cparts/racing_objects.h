static const Gameobj objects[] = {
    {
        .path = "/firmware/subgames/racing/stone0.bmp",
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/stone1.bmp",
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/stone2.bmp",
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/enemycar.bmp",
        .self_speed = 3,
        .collision_check = true,
        .dymanic_self_speed = true,
        .gameover = true
    },
    {
        .path = "/firmware/subgames/racing/fuel.bmp",
        .fuel_delta = 60,
        .score_delta = 10,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/wheel.bmp",
        .taxiing_speed_delta = 1,
        .score_delta = 10,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/engine.bmp",
        .speed_delta = 1,
        .score_delta = 10,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/truster.bmp",
        .score_delta = 10,
        .speed_boost_delta = 1,
        .speed_boost_taxiing_speed_add_delta = 1,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/star.bmp",
        .score_delta = 50,
        .delete = true
    },
    {
        .path = "/firmware/subgames/racing/money.bmp",
        .score_delta = 50,
        .maingame_money_delta = 1,
        .delete = true,

        .sound_path = "/firmware/sounds/money.pcm",
        .sound_samplerate = SOUND_EFFECTS_SAMPLERATE,
        .sound_volume = MONEY_SOUND_VOLUME
    }
};