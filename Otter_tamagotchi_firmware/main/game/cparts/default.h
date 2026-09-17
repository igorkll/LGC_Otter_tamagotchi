static const Game_state default_state = {
    .resetSettingsId = RESET_SETTINGS_ID,

    .room = game_room_bedroom,
    .person = game_person_otter,

    .old_car_room = -1,
    .next_car_icon = -1,

    .states_money = 100,
    .states_fatigue = 0,
    .states_hunger = 0,
    .states_thirst = 0,
    .states_caress = 50,
    .states_sadness = 40,

    .backpack_eat_count = 10,
    .backpack_water_count = 10,

    .settings_master_volume = 1,
    .settings_music_volume = 0.7
};