/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/

//todos
//write functions for the application loop, instead of defining and calculating everything manually - aka complete refactor of bad code
//import a text renderer library instead of using SDL3 default debug messages
//improve editor

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

char *message = " ";

int my_pow(int base, int num){
    if(num == 0) return 1;
    if(num < 0) return 0;
    int pow = 1;
    while(num){
        pow *= base;
        num--;
    }
}

int digit_num(int number){
    int dig = 0;
    if (number < 0) dig = 1;
    while(number)
    {
        number /= 10;
        dig++;
    }
    return dig;
}

int nth_digit(int num, int n){
    return (num / my_pow(10,n)) % 10;
}

int nth_hex_digit(int num, int n){
    return (num / my_pow(16,n)) % 16;
}

int mem_value[256];

char program[30][20];
int cur_pos_x = 0;
int cur_pos_y = 0;

int counter = 0;
int cursor_visible = 1;

int check_if_command(char* input){
    message = " ";
    if (*input == 'a' && *(input + 1) == 'n' && *(input + 2) == 'd') return 0;
    if (*input == 't' && *(input + 1) == 'a' && *(input + 2) == 'd') return 1;
    if (*input == 'i' && *(input + 1) == 's' && *(input + 2) == 'z') return 2;
    if (*input == 'd' && *(input + 1) == 'd' && *(input + 2) == 'a') return 3;
    if (*input == 'j' && *(input + 1) == 'm' && *(input + 2) == 's') return 4;
    if (*input == 'j' && *(input + 1) == 'm' && *(input + 2) == 'p') return 5;
    message = "Syntax error";
    return -1;
}

void read_command(){
    for(int i=0;i<30;i++){
        for(int j=0;j<20;j++){
            
        }
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("HAXX LABS", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    for(int i=0;i<256;i++){
        mem_value[i] = 0;
    }
    for(int i=0;i<30;i++){
        for(int j=0;j<20;j++){
            program[i][j] = ' ';
        }
    }
    //program[cur_pos_x][cur_pos_y] = 0x7c;
    return SDL_APP_CONTINUE;
}

const char *title = "PDP-8 Simulator";
const char *reg = "Registers";
const char *pc = "PC: ";
const char *df = "DF: ";
const char *acc = "ACC: ";
const char *mq = "MQ: ";
const char *mem = "Memory";
const char *prog = "Program";
const char *quit_message = "Press esc to quit";

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if (event->type == SDL_EVENT_KEY_DOWN && 
        (event->key.key >= 0x00000030u && event->key.key <= 0x00000039u || event->key.key >= 0x00000061u && event->key.key <= 0x0000007au || event->key.key == SDLK_SPACE)){
        program[cur_pos_x][cur_pos_y] = event->key.key;
        if(cur_pos_x >= 29){
            cur_pos_x = 0;
            cur_pos_y += 1;
        }
        else {
            cur_pos_x++;
        }
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_BACKSPACE){
        program[cur_pos_x][cur_pos_y] = 0x20;
        if(cur_pos_x >= 1){
            cur_pos_x -= 1;
        }
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_LEFT){
        if(cur_pos_x >= 1){
            cur_pos_x -= 1;
        }
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_RIGHT){
        if(cur_pos_x <= 28){
            cur_pos_x += 1;
        }
    }
    if (event->type == SDL_EVENT_KEY_UP && event->key.key == SDLK_UP){
        if(cur_pos_y >= 1){
            cur_pos_y -= 1;
        }
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_DOWN){
        if(cur_pos_y <= 18){
            cur_pos_y += 1;
        }
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_RETURN){
        if(cur_pos_y <= 18){
            cur_pos_y += 1;
            cur_pos_x = 0;
        }
    }
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_LSHIFT){
        char *input = malloc(600 * sizeof(char));
        int k=0;
        for(int i=0;i<20;i++){
            for(int j=0;j<30;j++){
                *(input + k) = program[j][i];
                k++;
            }
        }
        check_if_command(input);
    }
    return SDL_APP_CONTINUE;
}

int pc_number = 0;
char arr_pcc[9];
char *pcn;

int df_number = 0;
int arr[9];
char arr_c[9];

char *dfn;

int acc_number = 0;
char *accn;

int mq_number = 0;
char *mqn;

int mem_number = 0;
int arr_mem_number[5];
char arr_mem_char[5];
char *mem_n;

char *mem_v;

char *prog_string;

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    counter++;
    if(counter >= 2000){
        cursor_visible = -cursor_visible + 1;
        counter = 0;
    }
    //int pc_number = 0;
    int arr_pc_number[9];
    for(int i=0;i<8;i++){
        arr_pc_number[7-i] = nth_digit(pc_number, i);
    }
    //char arr_pcc[9];
    for(int i=0;i<8;i++){
        arr_pcc[i] = arr_pc_number[i] + 0x30;
    }
    arr_pcc[8] = 0;
    pcn = malloc(9 * sizeof(char));
    pcn = arr_pcc;

    //int df_number = 0;
    //int arr[9];
    for(int i=0;i<8;i++){
        arr[7-i] = nth_digit(df_number, i);
    }
    //char arr_c[9];
    for(int i=0;i<8;i++){
        arr_c[i] = arr[i] + 0x30;
    }
    arr_c[8] = 0;
    dfn = malloc(9 * sizeof(char));
    dfn = arr_c;

    //int acc_number = 0;
    for(int i=0;i<8;i++){
        arr[7-i] = nth_digit(acc_number, i);
    }
    for(int i=0;i<8;i++){
        arr_c[i] = arr[i] + 0x30;
    }
    arr_c[8] = 0;
    accn = malloc(9 * sizeof(char));
    accn = arr_c;

    //int mq_number = 0;
    for(int i=0;i<8;i++){
        arr[7-i] = nth_digit(mq_number, i);
    }
    for(int i=0;i<8;i++){
        arr_c[i] = arr[i] + 0x30;
    }
    mqn = malloc(9 * sizeof(char));
    mqn = arr_c;

    mem_number = 0;
    for(int i=0;i<4;i++){
        arr_mem_number[3-i] = nth_digit(mem_number, i);
    }
    for(int i=0;i<4;i++){
        arr_mem_char[i] = arr_mem_number[i] + 0x30;
    }
    arr_mem_char[4] = 0;
    mem_n = malloc(5 * sizeof(char));
    mem_n = arr_mem_char;

    int w = 0, h = 0;
    float x, y, x_r, y_r, x_m, y_m, x_p, y_p, x_q, y_q;
    float x_pc, y_pc, x_df, y_df, x_acc, y_acc, x_mq, y_mq;
    float x_pcn, y_pcn, x_dfn, y_dfn, x_accn, y_accn, x_mqn, y_mqn;
    float x_mem, y_mem;
    float x_memv, y_memv;
    float x_prog, y_prog;
    float x_message, y_message;
    const float scale = 2.0f;

    //Test
    //mem_value[22] = 92;    

    /* Center the title and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(title)) * 0.5;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.1;

    x_r = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(reg)) * 0.9;
    y_r = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    x_pc = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(pc)) * 0.8;
    y_pc = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.3;

    x_pcn = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(pcn)) * 0.9;
    y_pcn = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.3;

    x_df = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(df)) * 0.8;
    y_df = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.4;

    x_dfn = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(df)) * 0.86;
    y_dfn = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.4;

    x_acc = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(acc)) * 0.805;
    y_acc = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.5;

    x_accn = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(acc)) * 0.87;
    y_accn = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.5;

    x_mq = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(mq)) * 0.797;
    y_mq = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.6;

    x_mqn = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(mqn)) * 0.9;
    y_mqn = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.6;

    x_m = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(mem)) * 0.1;
    y_m = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    x_p = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(prog)) * 0.5;
    y_p = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.17;

    x_q = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(quit_message)) * 0.91;
    y_q = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * 0.91;

    x_mem = (w / scale) * 0.05;
    y_mem = (h / scale) * 0.23; 

    x_memv = (w / scale) * 0.1;
    y_memv = (h / scale) * 0.23;

    // needs to be corrected
    x_prog = (w / scale) * 0.4;
    y_prog = (h / scale) * 0.23;

    x_message = (w / scale) * 0.8;
    y_message = (h / scale) * 0.8;

    /* Draw the title */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, title);
    SDL_RenderDebugText(renderer, x_r, y_r, reg);
    SDL_RenderDebugText(renderer, x_pc, y_pc, pc);
    SDL_RenderDebugText(renderer, x_pcn, y_pcn, pcn);
    SDL_RenderDebugText(renderer, x_df, y_df, df);
    SDL_RenderDebugText(renderer, x_dfn, y_dfn, dfn);
    SDL_RenderDebugText(renderer, x_acc, y_acc, acc);
    SDL_RenderDebugText(renderer, x_accn, y_accn, accn);
    SDL_RenderDebugText(renderer, x_mq, y_mq, mq);
    SDL_RenderDebugText(renderer, x_mqn, y_mqn, mqn);
    SDL_RenderDebugText(renderer, x_m, y_m, mem);
    SDL_RenderDebugText(renderer, x_p, y_p, prog);
    SDL_RenderDebugText(renderer, x_q, y_q, quit_message);
    SDL_RenderDebugText(renderer, x_message, y_message, message);
    //SDL_RenderDebugText(renderer, x_mem, y_mem, mem_n);
    for(int i=0;i<17;i++){
        SDL_RenderDebugText(renderer, x_mem, y_mem, mem_n);
        y_mem += (h / scale) * 0.038;
        mem_number += 8;        
        for(int i=0;i<4;i++){
        arr_mem_number[3-i] = nth_digit(mem_number, i);
        }
        for(int i=0;i<4;i++){
            arr_mem_char[i] = arr_mem_number[i] + 0x30;
        }
        arr_mem_char[4] = 0;
        mem_n = arr_mem_char;
    }
    mem_v = malloc(1000 * sizeof(char));
    for(int k=0;k<17;k++){
        for(int i=0;i<8;i++){
            for(int j=0;j<2;j++){
                arr[1-j] = nth_hex_digit(mem_value[8*k + i],j);
            }
            for(int j=0;j<2;j++){
                if(arr[j]<=10){
                    arr_c[j] = arr[j] + 0x30;
                }
                else{
                    arr_c[j] = arr[j] + 0x37;
                }
            }
            mem_v[3*i] = arr_c[0];
            mem_v[3*i+1] = arr_c[1];
            mem_v[3*i+2] = 0x20;
        }
        SDL_RenderDebugText(renderer, x_memv, y_memv, mem_v);
        y_memv += (h / scale) * 0.038;
    }
    prog_string = malloc(300 * sizeof(char));
    for(int k=0;k<20;k++){
        for(int i=0;i<30;i++){
            prog_string[i] = program[i][k];
            if(cur_pos_x == i && cur_pos_y == k && cursor_visible){
                prog_string[i] = 0x7c;
            }
        }
        SDL_RenderDebugText(renderer, x_prog, y_prog, prog_string);
        y_prog += (h / scale) * 0.038;
    }
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

