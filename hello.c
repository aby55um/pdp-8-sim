/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("HAXX LABS", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const char *title = "HAXX LABS";
    const char *reg = "Registers";
    const char *pc = "PC: ";
    const char *df = "DF: ";
    const char *acc = "ACC: ";
    const char *mq = "MQ: ";
    const char *mem = "Memory";
    const char *prog = "Program";
    const char *quit_message = "Press esc to quit";
    int pcn = 0;
    char pcnc = (char)pcn;
    char *pcc = &pcnc;

    int w = 0, h = 0;
    float x, y, x_r, y_r, x_m, y_m, x_p, y_p, x_q, y_q;
    float x_pc, y_pc, x_df, y_df, x_acc, y_acc, x_mq, y_mq;
    float x_pcn, y_pcn, x_dfn, y_dfn, x_accn, y_accn, x_mqn, y_mqn;
    const float scale = 4.0f;

    /* Center the title and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(title)) * 0.5;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.1;

    x_r = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(reg)) * 0.9;
    y_r = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    x_pc = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(pc)) * 0.8;
    y_pc = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.3;

    x_df = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(df)) * 0.8;
    y_df = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.4;

    x_acc = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(acc)) * 0.815;
    y_acc = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.5;

    x_mq = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(mq)) * 0.797;
    y_mq = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.6;

    x_m = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(mem)) * 0.1;
    y_m = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    //x_q = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(quit_message)) * 0.1;
    //y_q = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    x_p = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(prog)) * 0.4;
    y_p = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    x_q = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(quit_message)) * 0.91;
    y_q = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.91;

    /* Draw the title */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, title);
    SDL_RenderDebugText(renderer, x_r, y_r, reg);
    SDL_RenderDebugText(renderer, x_pc, y_pc, pc);
    //SDL_RenderDebugText(renderer, x_pc, y_pc, pcc);
    SDL_RenderDebugText(renderer, x_df, y_df, df);
    SDL_RenderDebugText(renderer, x_acc, y_acc, acc);
    SDL_RenderDebugText(renderer, x_mq, y_mq, mq);
    SDL_RenderDebugText(renderer, x_m, y_m, mem);
    SDL_RenderDebugText(renderer, x_p, y_p, prog);
    SDL_RenderDebugText(renderer, x_q, y_q, quit_message);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

