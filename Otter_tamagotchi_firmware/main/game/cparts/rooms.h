const Room rooms[] = {
    {
        .background = "bedroom",
        .music = NULL,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = -0.1
    },
    {
        .background = "kitchen",
        .music = NULL,
        .person_x = (WIDTH / 4) - 20,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = 0.02,
        .states_delta_hunger = 0.4,
        .states_delta_thirst = 0.2,
        .states_delta_sadness = 0.4
    },
    {
        .background = "gaming",
        .music = NULL,
        .person_x = (WIDTH / 4) + 20,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = -0.01
    },
    {
        .background = "toilet",
        .music = NULL,
        .person_x = (WIDTH / 4) * 2,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "yard",
        .music = NULL,
        .person_x = WIDTH / 4,
        .person_y = (HEIGHT / 4) * 2,

        .states_delta_fatigue = -0.01,
        .states_delta_thirst = 0.1,
        .states_delta_sadness = -0.03
    },
    {
        .background = "car",
        .music = "brbigirl",
        .musicVolume = 1,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = 0.1,
        .states_delta_hunger = 0.1,
        .states_delta_thirst = 0.2,
        .states_delta_sadness = -0.02
    },
    {
        .background = "shop",
        .music = "shop",
        .musicVolume = 0.6,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = 0.1,
        .states_delta_hunger = 0.6,
        .states_delta_thirst = 0.2,
        .states_delta_sadness = 0.01
    },
    {
        .background = "club",
        .music = "mnepoxuy",
        .musicVolume = 0.6,
        .person_x = (WIDTH / 4) - 20,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = 0.05,
        .states_delta_hunger = 0.1,
        .states_delta_thirst = 0.2,
        .states_delta_sadness = -0.1
    }
};