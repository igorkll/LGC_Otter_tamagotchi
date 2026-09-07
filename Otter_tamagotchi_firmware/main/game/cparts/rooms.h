const Room rooms[] = {
    {
        .background = "bedroom",
        .music = NULL,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "kitchen",
        .music = NULL,
        .person_x = (WIDTH / 4) - 20,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "gaming",
        .music = NULL,
        .person_x = (WIDTH / 4) + 20,
        .person_y = (HEIGHT / 4) * 3,
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
    },
    {
        .background = "car",
        .music = "brbigirl",
        .musicVolume = 1,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,
    },
    {
        .background = "shop",
        .music = "shop",
        .musicVolume = 0.6,
        .person_x = (WIDTH / 4) * 3,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = 1,
        .states_delta_hunger = 1,
        .states_delta_thirst = 0.4,
        .states_delta_sadness = 0.3
    },
    {
        .background = "club",
        .music = "mnepoxuy",
        .musicVolume = 0.6,
        .person_x = (WIDTH / 4) - 20,
        .person_y = (HEIGHT / 4) * 3,

        .states_delta_fatigue = 1,
        .states_delta_hunger = 0.2,
        .states_delta_thirst = 0.3,
        .states_delta_sadness = -0.2
    }
};