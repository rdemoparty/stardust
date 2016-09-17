CUT_SCENE_DEFINITION = {
    music = "main.ogg",
    speech = "sounds/rain/acquiring_modules_change_the_behaviour_of_the_ship.ogg",
    skippable = true,
    slides = {
        {
            image = "sprites/splash1.png",
            timeOnSeconds = 8,
            fadeInSeconds = 2,
            fadeOutSeconds = 2,
            captions = {
                {
                    text = "This is the caption\nfor the first slide",
                    delaySeconds = 2,
                    timeOnSeconds = 3,
                    fadeInSeconds = 1,
                    fadeOutSeconds = 1,
                    position = {
                        x = 100,
                        y = 600
                    },
                    color = {
                        r = 1,
                        b = 1,
                        g = 1,
                        a = 1
                    },
                },
                {
                    text = "Some extra text",
                    delaySeconds = 4,
                    timeOnSeconds = 3,
                    fadeInSeconds = 1,
                    fadeOutSeconds = 1,
                    position = {
                        x = 100,
                        y = 660
                    },
                    color = {
                        r = 1,
                        b = 1,
                        g = 1,
                        a = 1
                    },
                }
            }
        },
        {
            image = "sprites/splash2.png",
            timeOnSeconds = 10,
            fadeInSeconds = 2,
            fadeOutSeconds = 2,
            captions = {
                {
                    text = "This is the caption\nfor the SECOND SLIDE !!11",
                    position = {
                        x = 100,
                        y = 660
                    },
                }
            }
        },
        {
            image = "sprites/splash3.png",
            timeOnSeconds = 20,
            fadeInSeconds = 2,
            fadeOutSeconds = 2,
            captions = {
                {
                    text = "Caption for line 1",
                    delaySeconds = 2,
                    fadeInSeconds = 1,
                    position = {
                        x = 600,
                        y = 300
                    },
                },
                {
                    text = "Caption for line 2",
                    delaySeconds = 4,
                    fadeInSeconds = 1,
                    position = {
                        x = 600,
                        y = 332
                    },
                },
                {
                    text = "Caption for line 3",
                    delaySeconds = 6,
                    fadeInSeconds = 1,
                    position = {
                        x = 600,
                        y = 364
                    },
                },
            }
        }
    }
}

addCutSceneBeforeLevel(CUT_SCENE_DEFINITION, 1);