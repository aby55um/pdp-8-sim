/*
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
// Pre-release version

//todos
//write functions for the application loop, instead of defining and calculating everything manually - aka complete refactor of bad code
//import a text renderer library instead of using SDL3 default debug messages
//improve and debug the editor
//use the math libraries instead of the custom ones defined below
//improve and optimize the overall trashy code
//solve ACC overflow and implement LINK (bit) register
//implement indirect addressing, macros and microcoded instructions

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

char *message = " ";

int pc_number = 0;
int arr_pc_number[9];
char arr_pcc[9];
char *pcn;

int df_number = 0;
int arr_df_number[9];
char arr_dfc[9];
char *dfn;

int arr[9];
char arr_c[9];

int acc_number = 0;
int arr_acc_number[9];
char arr_accc[9];
char *accn;

int mq_number = 0;
int arr_mq_number[9];
char arr_mqc[9];
char *mqn;

int mem_number = 0;
int arr_mem_number[5];
char arr_mem_char[5];
char *mem_n;

char *mem_v;

char *prog_string;

char command_names[26][3] = {
                            "and","tad","isz","dca","jms","jmp",
                            "cla","cll","cma","cml","iac","rar","ral","rtr","rtl","bsw",
                            "sma","sza","snl","spa","sna","szl",
                            "osr","hlt",
                            "mqa","mql"
                        };

int command_numbers[26] = {
    0,512,1024,1536,2048,2560,3712,3648,3616,3600,3585,3592,3588,3594,3590,3586,3904,3872,3856,3912,3880,3864,3844,3842,3905,3857
};

int mem_value[256];

char program[30][20];
int cur_pos_x = 0;
int cur_pos_y = 0;

int counter = 0;
int cursor_visible = 1;

int test;
int compile_error = 0;
int run = 1;

float x, y, x_r, y_r, x_m, y_m, x_p, y_p, x_q, y_q;
float x_pc, y_pc, x_df, y_df, x_acc, y_acc, x_mq, y_mq;
float x_pcn, y_pcn, x_dfn, y_dfn, x_accn, y_accn, x_mqn, y_mqn;
float x_mem, y_mem;
float x_memv, y_memv;
float x_prog, y_prog;
float x_message, y_message;
float x_compile_help, y_compile_help;
float x_step_help, y_step_help;

const char *title = "PDP-8 Simulator";
const char *reg = "Registers";
const char *pc = "PC: ";
const char *df = "DF: ";
const char *acc = "ACC: ";
const char *mq = "MQ: ";
const char *mem = "Memory";
const char *prog = "Program";
const char *quit_message = "Press esc to quit";
const char *compile_message = "Press LSHIFT to compile";
const char *step_message = "Press RSHIFT to step";

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

int nth_duodec_digit(int num, int n){
    return (num / my_pow(12,n)) % 12;
}

int nth_binary_digit(int num, int n){
    return (num / my_pow(2,n)) % 2;
}

int check_if_command(char* input){
    message = " ";
    for(int i=0;i<26;i++){
        if(*input == command_names[i][0] && *(input + 1) == command_names[i][1] && *(input + 2) == command_names[i][2]){
            return command_numbers[i];
        }
    }
    message = "Syntax error";
    return -1;
}

int runtime_check_if_command(int command){
    for(int i=0;i<6;i++){
        if(command >= command_numbers[i] && command <= command_numbers[i] + 384){
            return i;
        }
    }
    for(int i=6;i<26;i++){
        if(command == command_numbers[i]){
            return i;
        }
    }
    return -1;
}

void execute_command(int com){
    int memory = 0;
    if(mem_value[pc_number] >= command_numbers[com] + 256) memory -= 256;
    else if(mem_value[pc_number] >= command_numbers[com] + 128) memory -= 128;
    memory += mem_value[pc_number] - command_numbers[com];
    int n=0;
    switch(com){
    case 0:
        for(int i=0;i<12;i++){
            if(nth_binary_digit(acc_number,i) == 1 && nth_digit(memory,i) == 1) n += my_pow(2,i);
        }
        acc_number = n;
        pc_number++;
        break;
    case 1:
        acc_number += mem_value[memory];
        pc_number++;
        break;
    case 2:
        mem_value[memory]++;
        pc_number++;
        break;
    case 3:
        mem_value[memory] = acc_number;
        acc_number = 0;
        pc_number++;
        break;
    case 4:
        mem_value[memory] = pc_number;
        pc_number = memory + 1;
        break;
    case 5:
        pc_number = memory;
        break;
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

    //Test
    //acc_number = 1;
    //mem_value[22] = 92; 
    //mem_value[22] = 1;

    //program[cur_pos_x][cur_pos_y] = 0x7c;
    return SDL_APP_CONTINUE;
}

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
        pc_number = 0;
        df_number = 0;
        acc_number = 0;
        mq_number = 0;

        char *input = malloc(600 * sizeof(char));
        int k=0;
        for(int i=0;i<20;i++){
            for(int j=0;j<30;j++){
                *(input + k) = program[j][i];
                k++;
            }
        }
        k=0;
        int l=0;
        for(int i=0;i<400;i++){
            mem_value[i]=0;
        }
        while(k<600){
                mem_value[l] = 0;
                while(*(input+k)==0x20){
                    k++;
                }
                if(k<600){
                    test = check_if_command(input+k);
                    if(test == -1){
                        break;
                    }
                    if(test < 3000){
                        k+=3;
                        while(*(input+k)==0x20){
                            k++;
                        }
                        if(*(input+k)=='i' || *(input+k)=='z'){
                            if(*(input+k)=='i'){
                                mem_value[l] += 256;
                            }
                            else if(*(input+k)=='z'){
                                mem_value[l] += 128;
                            }
                            k++;
                            while(*(input+k)==0x20){
                                k++;
                            }
                        }
                        if(*(input+k+1)==0x20 && *(input+k)>=0x30 && *(input+k)<=0x39){
                            mem_value[l]+=*(input+k)-0x30;
                            k++;
                        }
                        else if(*(input+k+2)==0x20 &&
                            *(input+k)>=0x30 && *(input+k)<=0x39 &&
                            *(input+k+1)>=0x30 && *(input+k+1)<=0x39){
                            mem_value[l]+=(*(input+k)-0x30)*10;
                            mem_value[l]+=*(input+k+1)-0x30;
                            k+=2;
                        }
                        else if(*(input+k+3)==0x20 &&
                            *(input+k)==0x31 &&
                            ((*(input+k+1)==0x32 && *(input+k+2)<=0x37 && *(input+k+2)>=0x30) ||
                                (*(input+k+1)>=0x30 && *(input+k+1)<=0x31 && *(input+k+2)>=0x30 && *(input+k+2)<=0x39))
                            ){
                            mem_value[l]+=(*(input+k)-0x30)*100;
                            mem_value[l]+=(*(input+k+1)-0x30)*10;
                            mem_value[l]+=*(input+k+2)-0x30;
                            k+=3;
                        }
                        else{
                            compile_error = 1;
                            message = "Syntax error";
                            break;
                        }

                    }
                    else{
                        if(test == 3842) break;
                        k+=3;
                    }
                    mem_value[l] += test;
                    l++;
                }
                
        }
    }

    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_RSHIFT){
        int comm = runtime_check_if_command(mem_value[pc_number]);
        execute_command(comm);
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    counter++;
    if(counter >= 2000){
        cursor_visible = -cursor_visible + 1;
        counter = 0;
    }
    //int pc_number = 0;
    //int arr_pc_number[9];
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

    for(int i=0;i<8;i++){
        arr_df_number[7-i] = nth_digit(df_number, i);
    }
    //char arr_c[9];
    for(int i=0;i<8;i++){
        arr_dfc[i] = arr_df_number[i] + 0x30;
    }
    arr_dfc[8] = 0;
    dfn = malloc(9 * sizeof(char));
    dfn = arr_dfc;

    //int acc_number = 0;
    for(int i=0;i<8;i++){
        arr_acc_number[7-i] = nth_digit(acc_number, i);
    }
    for(int i=0;i<8;i++){
        arr_accc[i] = arr_acc_number[i] + 0x30;
    }
    arr_accc[8] = 0;
    accn = malloc(9 * sizeof(char));
    accn = arr_accc;

    //int mq_number = 0;
    for(int i=0;i<8;i++){
        arr_mq_number[7-i] = nth_digit(mq_number, i);
    }
    for(int i=0;i<8;i++){
        arr_mqc[i] = arr_mq_number[i] + 0x30;
    }
    mqn = malloc(9 * sizeof(char));
    mqn = arr_mqc;

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
    const float scale = 2.0f;  

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

    x_prog = (w / scale) * 0.45;
    y_prog = (h / scale) * 0.23;

    x_message = (w / scale) * 0.8;
    y_message = (h / scale) * 0.7;

    x_compile_help = (w / scale) * 0.755;
    y_compile_help = (h / scale) * 0.8;

    x_step_help = (w / scale) * 0.755;
    y_step_help = (h / scale) * 0.85;

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
    SDL_RenderDebugText(renderer, x_compile_help, y_compile_help, compile_message);
    SDL_RenderDebugText(renderer, x_step_help, y_step_help, step_message);
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
            for(int j=0;j<3;j++){
                arr[2-j] = nth_hex_digit(mem_value[8*k + i],j);
            }
            for(int j=0;j<3;j++){
                if(arr[j]<10){
                    arr_c[j] = arr[j] + 0x30;
                }
                else{
                    arr_c[j] = arr[j] + 0x37;
                }
            }
            mem_v[4*i] = arr_c[0];
            mem_v[4*i+1] = arr_c[1];
            mem_v[4*i+2] = arr_c[2];
            mem_v[4*i+3] = 0x20;
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

