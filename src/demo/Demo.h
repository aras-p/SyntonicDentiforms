#pragma once

struct sapp_event;

bool demo_init();
bool demo_update();
void demo_shutdown();
void demo_event(const sapp_event* evt);