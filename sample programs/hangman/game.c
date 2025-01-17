#include "game.h"
#include "hangman_states.h"
#include "simple_rand.h"
#include "words.h"
#include "utility.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    uint8_t num_attempts;
    uint8_t category;
    uint8_t word_len;
    char *category_str;
    char *word;
    char guess[20];
    char status[40];

} Game_t;

static Game_t self;

void Game_Init(void)
{
    // Start hardware timer in One-Shot mode
    TCR = 1; // Timer value is used as a seed for the random generator
    self.word_len = 0;

    dma_write("Welcome to Hangman!\n\n");
    Game_Reset();
}

void Game_Reset(void)
{
    dma_write("Please select a category:\n\
1. Cities.\n\
2. Animals.\n\
3. Food.\n\
4. Car Manufacturers.\n");

    for (;;)
    {
        dma_write("\nYour selection? ");

        char k = read_key();
        put_char(k);

        if (!isdigit(k))
        {
            dma_write("\nPlease enter a number.\n");
            continue;
        }

        self.category = k - '0';

        if (self.category > 0 && self.category < 5)
        {
            simple_srand((uint32_t)TVR);

            uint8_t n = simple_rand() % 64;

            const char *rand_word = CATEGORIES[self.category - 1][n];

            memset(self.guess, 0, self.word_len);

            self.word_len = strlen(rand_word);
            self.word = rand_word;

            self.category_str = CATEGORY_STR[self.category - 1];
            memset(self.guess, '-', self.word_len);
            memset(self.status, 0, sizeof(self.status));
            self.num_attempts = 0;

            clear_terminal();
          
        }
        else
        {
            dma_write("\nPlease enter a valid category.\n");
        }
    }
}

void Game_Run(void)
{
    for (;;)
    {
        dma_write(HANGMAN_STATES[self.num_attempts]);

        if (self.num_attempts > 5)
        {
            dma_write("\nYou Failed!\n");
            break;
        }
        else if (strncmp(self.word, self.guess, self.word_len) == 0)
        {
            dma_write("\nYou Win!\n");
            break;
        }
        else
        {
            sprintf(self.status, "Word: %s (%s)\n", self.guess, self.category_str);
            dma_write(self.status);
            dma_write("\nYour guess? ");

            char k = read_key();
            put_char(k);

            bool guess_correct = false;

            for (uint8_t i = 0; i < self.word_len; ++i)
            {
                if (self.guess[i] == '-' && k == self.word[i])
                {
                    self.guess[i] = k;
                    guess_correct = true;
                    break;
                }
            }

            if (!guess_correct)
            {
                self.num_attempts++;
            }

            clear_terminal();
        }
    }

    dma_write("\nPress any key...");
    read_key();
    clear_terminal();
    Game_Reset();
}