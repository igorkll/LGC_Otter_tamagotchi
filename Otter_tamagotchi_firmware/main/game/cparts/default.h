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
    .states_horny = 0
};