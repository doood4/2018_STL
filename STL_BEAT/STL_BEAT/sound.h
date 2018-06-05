#pragma once

#include "inc/fmod.hpp"
#pragma comment (lib, "fmod_vc.lib")

using namespace FMOD;

System* pFmod;
Channel* ch[6];
Sound* sound[6];


void setup_sound() {
	System_Create(&pFmod);
	pFmod->init(10, FMOD_INIT_NORMAL, NULL);
	pFmod->createSound("title_bgm.wav", FMOD_LOOP_NORMAL, NULL, &sound[0]);
	pFmod->createSound("move.wav", FMOD_DEFAULT, NULL, &sound[1]);
	pFmod->createSound("select.wav", FMOD_DEFAULT, NULL, &sound[2]);
	pFmod->createSound("do.wav", FMOD_DEFAULT, NULL, &sound[3]);
	pFmod->createSound("mi.wav", FMOD_DEFAULT, NULL, &sound[4]);
	pFmod->createSound("sol.wav", FMOD_DEFAULT, NULL, &sound[5]);
	
}


void title_bgm_on() {
	pFmod->playSound(sound[0], 0, false, &ch[0]);
	ch[0]->setVolume(0.5);
}
void bgm_off() {
	ch[0]->stop();
}


void move_sound() {
	pFmod->playSound(sound[1], 0, false, &ch[2]);
}

void select_sound() {
	pFmod->playSound(sound[2], 0, false, &ch[2]);
}

void do_sound() {
	pFmod->playSound(sound[3], 0, false, &ch[3]);
}

void mi_sound() {
	pFmod->playSound(sound[4], 0, false, &ch[4]);
}

void sol_sound() {
	pFmod->playSound(sound[5], 0, false, &ch[5]);
}