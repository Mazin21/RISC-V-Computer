#include "game.h"
#include "utility.h"
#include "printf.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define GRAVITY 0
#define RETURN_KEY (char)10

static Game_t self;

void Game_Reset(void)
{
    self.height = 500;
    self.velocity = -50;
    self.fuel_units = 120;
    self.cur_state = LANDING;
    self.time = 0;

    self.state_table[LANDING] = GAME_LANDING;
    self.state_table[INVALID_BURN] = GAME_BURN_INVALID;
    self.state_table[OUT_OF_FUEL] = GAME_OUT_OF_FUEL;
    self.state_table[ON_THE_MOON] = GAME_ON_THE_MOON;

    clear_terminal();

    dma_write("CONTROL TO LUNAR MODULE :\n\
BEGIN LANDING PROCEDURE\n\n\
 TIME  HEIGHT VELOCITY FUEL UNITS BURN\n\
(SECS) (FEET) (FT/SEC) REMAINING");
}

void Game_Init(void)
{
    dma_write("LUNAR LANDER\n\
MINUS VELOCITY (-) MEANS DOWNWARD\nMOVEMENT\n\
PLUS  VELOCITY (+) MEANS UPWARD MOVEMENT\n\
MAXIMUM BURN IS 30 UNITS/SEC.  (BURN MAY\nBE ANY INTEGER FROM 0 TO 30)\n\
A BURN OF 5 UNITS/SEC IS REQUIRED TO\nCANCEL GRAVITY.\nGOOD LUCK!\nPRESS ANY KEY...");

    read_key();

    Game_Reset();
}

void Game_DisplayStatus(void)
{
    if (self.cur_state == LANDING)
    {
        sprintf(self.status, "\n  %02d    %04d     %+03d      %04d     ?",
                self.time,
                self.height,
                self.velocity,
                self.fuel_units);
    }
    else
    {
        sprintf(self.status, "\n  %02d    %04d     %+03d",
                self.time,
                self.height,
                self.velocity);
    }

    dma_write(self.status);
}

void Game_ReadInput(void)
{
    memset(self.burn_input_buff, 0, sizeof(self.burn_input_buff));
    self.burn_input_index = 0;

    while (self.burn_input_index < 2)
    {
        char k = read_key();

        if (k == RETURN_KEY)
        {
            break;
        }

        put_char(k);
        self.burn_input_buff[self.burn_input_index++] = k;
    }
}

void Game_UpdateStatus(void)
{
    self.velocity = self.velocity + self.burn - GRAVITY;

    if (self.cur_state == ON_THE_MOON)
    {
        self.height = 0;
    }
    else
    {
        self.height += self.velocity;
    }

    self.fuel_units -= self.burn;
    self.time++;
}

bool Game_BurnValid(void)
{
    if (!isdigit(self.burn_input_buff[0]))
    {
        return false;
    }

    if (self.burn_input_index > 1 && !isdigit(self.burn_input_buff[1]))
    {
        return false;
    }

    self.burn = atoi(self.burn_input_buff);

    if (self.burn > 30)
    {
        return false;
    }

    return true;
}

void GAME_LANDING(void)
{
    Game_DisplayStatus();

    Game_ReadInput();

    if (!Game_BurnValid())
    {
        self.cur_state = INVALID_BURN;
    }
    else
    {
        Game_UpdateStatus();

        if (self.fuel_units <= 0)
        {
            dma_write("\nOUT OF FUEL");
            self.burn = 0;
            self.cur_state = OUT_OF_FUEL;
        }
        else if (self.height <= 0)
        {
            self.cur_state = ON_THE_MOON;
        }
    }
}

void GAME_BURN_INVALID(void)
{
    dma_write("\nBURN OUT OF RANGE.    BURN ?");

    Game_ReadInput();

    if (Game_BurnValid())
    {
        self.cur_state = LANDING;
        Game_UpdateStatus();
    }
}

void GAME_OUT_OF_FUEL(void)
{
    if (self.height <= 0)
    {
        self.cur_state = ON_THE_MOON;
    }
    else
    {
        Game_DisplayStatus();
        Game_UpdateStatus();
    }
}

void GAME_ON_THE_MOON(void)
{
    dma_write("\nON THE MOON");

    Game_DisplayStatus();

    if (self.velocity <= -10)
    {
        dma_write("\nYOU JUST CRASHED A 29 MEGABUCK LANDER!");
    }
    else if (self.velocity > -10 && self.velocity <= -5)
    {
        dma_write("\nWELL, YOU GOT DOWN ALIVE, BUT DAMAGE TO\n\
YOUR AIRCRAFT HAS STRANDED YOU HERE!");
    }
    else
    {
        dma_write("\nPERFECT LANDING!\nCONGRATULATIONS!");
    }

    dma_write("\nTRY AGAIN? (Y/N)");

    char k = toupper(read_key());

    if (k == 'Y')
    {
        Game_Reset();
    }
    else
    {
        dma_write("\nCONTROL OUT\n");

        for (;;)
            ;
    }
}

void Game_Run(void)
{
    for (;;)
    {
        self.state_table[self.cur_state]();
    }
}