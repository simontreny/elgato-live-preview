project(obs-elgato)

set(obs-elgato_SOURCES
    obs-elgato.c)

add_library(obs-elgato MODULE ${obs-elgato_SOURCES})
target_link_libraries(obs-elgato libobs)

install_obs_plugin(obs-elgato)
