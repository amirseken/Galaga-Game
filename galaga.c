/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define PS2_BASE              0xFF200100
#define PS2_CONTROL           0xFF200104
#define A9_ONCHIP_END         0xFFFFFFFF
#define MPCORE_GIC_CPUIF      0xFFFEC100
#define MPCORE_GIC_DIST       0xFFFED000
#define ICCIAR                0x0C
#define ICCEOIR               0x10
#define ICCPMR                0x04
#define ICCICR                0x00
#define ICDDCR                0x00

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BLACK 0x0000

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// function prototypes
void swap(int *a, int*b);
void draw_line(int x0, int y0, int x1, int y1, short int color);
void draw_pixel(int x, int y, short int line_color);
void draw_box(int x, int y, int height, int length, short int color);
void clear_screen();
void wait_for_vsync();

// global variable for buffer
volatile int pixel_buffer_start;
// global variables for hex displays
volatile int * HEX3_HEX0_ptr = (int *) HEX3_HEX0_BASE; 
volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;
// global variables for PS2 I/O
volatile int * PS2_ptr = (int *)PS2_BASE;
int PS2_data, RVALID;
void keyboard();

// global vars for background stars
#define NUM_STARS 100
int array_x_stars[NUM_STARS];
int array_y_stars[NUM_STARS];
void background(int key);
void draw_stars(int x, int y, short int color);

// global vars for starship
#define STARSHIP_Y_POS 225 // starship never moves in y direction
int starship_x_pos;
void draw_starship(int x, int y);
void erase_starship(int x, int y);
void starship(int key);

// global vars for missile
struct missiles
{
	int missile_x_pos;
	int missile_y_pos;
};
struct missiles missile_array[3];
void missile(int key, struct missiles *missile1);
void draw_missile(int x, int y);
void erase_missile(int x, int y);
int num_of_missiles = 0;
int m0 = 0;

//alien 1
int alien_x_pos;
int alien_y_pos;
void alien(int key);
void draw_alien(int x, int y);
void erase_alien(int x, int y);
int m1 = 0;
int m2 = 0;
int path = 0;
int turn = 0;
int turn2 = 0;

//alien 2
int alien2_x_pos;
int alien2_y_pos;
void alien2(int key);
void draw_alien2(int x, int y);
void erase_alien2(int x, int y);
int m1_v2 = 0;
int m2_v2 = 0;
int path_v2 = 0;
int turn_v2 = 0;
int turn2_v2 = 0;

//alien 3
int alien3_x_pos;
int alien3_y_pos;
void alien3(int key);
void draw_alien3(int x, int y);
void draw_alien_blue(int x, int y);
void erase_alien3(int x, int y);
int m1_v3 = 0;
int m2_v3 = 0;
int path_v3 = 0;
int turn_v3 = 0;
int turn2_v3 = 0;
int is_blue = 0;

// global vars for explosion
int array_x_explosion = 40;
int array_y_explosion = 40;
void explosion(int key);
void draw_explosion(int x, int y, short int color);

// global vars for interrupts taken from de1soc arm manual
void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);

// global vars for game state
volatile bool start_game = false;
bool gameOver = false;
bool gameWon = false;
void collision_check();

// global vars for displaying score
void display_score();
bool point_check();
int score = 0;

// global vars for displaying text
void write_character(int x, int y, char c);
void clear_character_buffer();
void display_start_screen();
void display_end_screen();

//main prog
int main(void)
{
	// Initialize interrupts
	disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_KEYs(); // configure pushbutton KEYs to generate interrupts
	enable_A9_interrupts(); // enable interrupts in the A9 processor

	// initialize background stars position
	background(0);
	// initialize starship position
	starship(0);
	// initialize alien position
	alien(0);
	alien2(0);
	alien3(0);

	// pointer to front buffer register
    volatile int * pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = FPGA_ONCHIP_BASE; // first store the address in the back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

	// display main menu screen
	display_start_screen();

	// wait until game is started
	while (start_game == false) {
		// wait for key 3 to be pressed
	}

	path = rand()%3;
	path_v3 = rand()%3;
	path_v2 = rand()%3;
	
    while (!gameOver)
    {
		// initialize keyboard controls
		keyboard();

		// setting back buffer to be equal to front buffer 
		*(pixel_ctrl_ptr + 1) = FPGA_ONCHIP_BASE;
		
		// check for points to update score
		point_check();
		// check for collision
		collision_check();
		
		// draw
		background(1);
		starship(1);
		alien(1);
		alien2(1);
		if (is_blue == 0) {
			alien3(1);
		}
		else {
			alien3(4);
		}
		
		if (m2 == 1) {
			m1 = 0;
			alien(0);
			m2 = 0;
		}

		if (m2_v2 == 1) {
			m1_v2 = 0;
			alien2(0);
			m2_v2 = 0;
		}
		
		if (m2_v3 == 1) {
			m1_v3 = 0;
			alien3(0);
			m2_v3 = 0;
		}
		
		for (int i = 0; i < 3; i++) {
			if (missile_array[i].missile_y_pos != NULL) {
				missile(1, &(missile_array[i]));
			}
		}

		// refresh screen
		wait_for_vsync();
		
		// erase
		background(3);
		alien(3);
		alien2(3);
		alien3(3);
		explosion(3);
		
		for (int i = 0; i < 3; i++) {
			if (missile_array[i].missile_y_pos != NULL) {
				missile(3, &(missile_array[i]));
			}
		}

		// animate
		background(2);
		alien(2);
		alien2(2);
		alien3(2);
		
		for (int i = 0; i < 3; i++) {
			if (missile_array[i].missile_y_pos != NULL) {
				missile(2, &(missile_array[i]));
			}
		}

		// new back buffer
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);

		// display player score
		display_score();
		
		// end game if winning score is achieved
		if (score >= 1000) {
			gameWon = true;
			gameOver = true;
		}
    }

	// display end screen
	display_end_screen();

	return 0;
}

// check if starship collided with an alien
void collision_check() {
	int temp = 0;
	int temp2 = 0;
	int temp_v2 = 0;
	int temp2_v2 = 0;
	int temp_v3 = 0;
	int temp2_v3 = 0;
	
	if (starship_x_pos <= alien_x_pos && starship_x_pos >= alien_x_pos - 5) {
		temp = 1;
	}
	if (starship_x_pos >= alien_x_pos && starship_x_pos <= alien_x_pos + 5) {
		temp = 1;
	}
	if (alien_y_pos >= STARSHIP_Y_POS - 5) {
		temp2 = 1;
	}
	if (starship_x_pos <= alien2_x_pos && starship_x_pos >= alien2_x_pos - 5) {
		temp_v2 = 1;
	}
	if (starship_x_pos >= alien2_x_pos && starship_x_pos <= alien2_x_pos + 5) {
		temp_v2 = 1;
	}
	if (alien2_y_pos >= STARSHIP_Y_POS - 5) {
		temp2_v2 = 1;
	}
	if (starship_x_pos <= alien3_x_pos && starship_x_pos >= alien3_x_pos - 8) {
		temp_v3 = 1;
	}
	if (starship_x_pos >= alien3_x_pos && starship_x_pos <= alien3_x_pos + 8) {
		temp_v3 = 1;
	}
	if (alien3_y_pos >= STARSHIP_Y_POS - 5) {
		temp2_v3 = 1;
	}
	
	// display explosion and change game state
	if (temp2 == 1 && temp == 1) {
		array_x_explosion = starship_x_pos;
		array_y_explosion = STARSHIP_Y_POS;
		explosion(0);
		explosion(1);
		gameWon = false;
		gameOver = true;
	} 
	if (temp2_v2 == 1 && temp_v2 == 1) {
		array_x_explosion = starship_x_pos;
		array_y_explosion = STARSHIP_Y_POS;
		explosion(0);
		explosion(1);
		gameWon = false;
		gameOver = true;
	} 
	if (temp2_v3 == 1 && temp_v3 == 1) {
		array_x_explosion = starship_x_pos;
		array_y_explosion = STARSHIP_Y_POS;
		explosion(0);
		explosion(1);
		gameWon = false;
		gameOver = true;
	} 
}

// check for score
bool point_check() {
	
	for(int i = 0; i < 3; i++) {
		if (missile_array[i].missile_y_pos != NULL) {
			// increases score by 80 if missile hits enemy 1
			if (missile_array[i].missile_y_pos <= alien_y_pos + 6 && missile_array[i].missile_y_pos >= alien_y_pos - 6) {
				if (missile_array[i].missile_x_pos <= alien_x_pos + 6 && missile_array[i].missile_x_pos >= alien_x_pos - 6) {
					score += 80;
					alien(3);
					missile(3, &(missile_array[i]));
					m0 = 1;
					array_x_explosion = alien_x_pos;
					array_y_explosion = alien_y_pos;
					explosion(0);
					explosion(1);
					alien(0);
					path = rand()%3;
				}
			}
			// increases score by 60 if missile hits enemy 2
			else if (missile_array[i].missile_y_pos <= alien2_y_pos + 8 && missile_array[i].missile_y_pos >= alien2_y_pos - 8) {
				if (missile_array[i].missile_x_pos <= alien2_x_pos + 8 && missile_array[i].missile_x_pos >= alien2_x_pos - 8) {
					score += 60;
					alien2(3);
					missile(3, &(missile_array[i]));
					m0 = 1;
					array_x_explosion = alien2_x_pos;
					array_y_explosion = alien2_y_pos;
					explosion(0);
					explosion(1);
					alien2(0);
					path_v2 = rand()%3;
				}
			}
			// increases score by 100 if missile hits enemy 3
			else if (missile_array[i].missile_y_pos <= alien3_y_pos + 12 && missile_array[i].missile_y_pos >= alien3_y_pos - 12) {
				if (missile_array[i].missile_x_pos <= alien3_x_pos + 12 && missile_array[i].missile_x_pos >= alien3_x_pos - 12) {
					score += 100;
					alien3(3);
					missile(3, &(missile_array[i]));
					m0 = 1;
					array_x_explosion = alien3_x_pos;
					array_y_explosion = alien3_y_pos;
					explosion(0);
					explosion(1);
					alien3(0);
					path_v3 = rand()%3;
					
					/*
					is_blue++;
					missile(3, &(missile_array[i]));
					m0 = 1;
					alien3(3);
					alien3(4);
					if (is_blue == 2) {
						is_blue = 0;
						score += 100;
						alien3(3);
						missile(3, &(missile_array[i]));
						m0 = 1;
						array_x_explosion = alien3_x_pos;
						array_y_explosion = alien3_y_pos;
						explosion(0);
						explosion(1);
						alien3(0);
						path_v3 = rand()%3;
					} */
				}
			}
		}
	}
}

// draws, animates, and erases the alien
void alien(int key)
{
	// initialize location of alien
	if (key == 0)
	{
			alien_x_pos = (rand() % (319 - 70))+70;
			alien_y_pos = 10;	
	}
	// draw alien at the top 
	else if (key == 1)
	{
		if (m1 != 1) {
			draw_alien(alien_x_pos, alien_y_pos);
		}	
	}
	// animate alien
	else if (key == 2) 
	{
		if (m1 == 1) {
			alien(3);
			m2 = 1;
		} // go down
		else if (m1 == 0) {
			if (path == 0) {
				if (alien_y_pos <= 240) {
					alien_y_pos += 3;
					if (alien_y_pos >= 230) {
						m1 = 1;
						path = rand()%3;
					}
				}
			} // go right and then left
			else if (path == 1) {
				if (alien_y_pos <= 240) {
					alien_y_pos += 3;
					if (alien_y_pos >= 230) {
						m1 = 1;
						path = rand()%3;
					}
					if (alien_x_pos <= 310 && turn2 == 0) {
						alien_x_pos += 3;
						if (alien_x_pos > 310) {
							turn2 = 1;
						}
					}
					else if (alien_x_pos >= 12 && turn2 == 1) {
						alien_x_pos -= 3;
						if (alien_x_pos < 12) {
							turn2 = 0;
						}
					}						
				}			
			} // go left and then right
			else if (path == 2) {
				if (alien_y_pos <= 240) {
					alien_y_pos += 3;
					if (alien_y_pos >= 230) {
						m1 = 1;
						path = rand()%3;
					}
					if (alien_x_pos >= 12 && turn == 0) {
						alien_x_pos -= 3;
						if (alien_x_pos < 12) {
							turn = 1;
						}
					}
					else if (alien_x_pos <= 310 && turn == 1) {
						alien_x_pos += 3;
						if (alien_x_pos > 310) {
							turn = 0;
						}
					}						
				}			
			}
		}
	}
	// erase alien
	else if (key == 3)
	{
		erase_alien(alien_x_pos, alien_y_pos);
	}
}
// draws, animates, and erases the alien
void alien2(int key)
{
	// initialize location of alien
	if (key == 0)
	{
			alien2_x_pos = (rand() % (319 - 30))+30;
			alien2_y_pos = 10;	
	}
	// draw alien at the top 
	else if (key == 1)
	{
		if (m1_v2 != 1) {
			draw_alien2(alien2_x_pos, alien2_y_pos);
		}	
	}
	// animate alien
	else if (key == 2) 
	{
		if (m1_v2 == 1) {
			alien2(3);
			m2_v2 = 1;
		} // go down
		else if (m1_v2 == 0) {
			if (path_v2 == 0) {
				if (alien2_y_pos <= 240) {
					alien2_y_pos += 3;
					if (alien2_y_pos >= 230) {
						m1_v2 = 1;
						path_v2 = rand()%3;
					}
				}
			} // go right and then left
			else if (path_v2 == 1) {
				if (alien2_y_pos <= 240) {
					alien2_y_pos += 3;
					if (alien2_y_pos >= 230) {
						m1_v2 = 1;
						path_v2 = rand()%3;
					}
					if (alien2_x_pos <= 310 && turn2_v2 == 0) {
						alien2_x_pos += 3;
						if (alien2_x_pos > 310) {
							turn2_v2 = 1;
						}
					}
					else if (alien2_x_pos >= 12 && turn2_v2 == 1) {
						alien2_x_pos -= 3;
						if (alien2_x_pos < 12) {
							turn2_v2 = 0;
						}
					}						
				}			
			} // go left and then right
			else if (path_v2 == 2) {
				if (alien2_y_pos <= 240) {
					alien2_y_pos += 3;
					if (alien2_y_pos >= 230) {
						m1_v2 = 1;
						path_v2 = rand()%3;
					}
					if (alien2_x_pos >= 12 && turn_v2 == 0) {
						alien2_x_pos -= 3;
						if (alien2_x_pos < 12) {
							turn_v2 = 1;
						}
					}
					else if (alien2_x_pos <= 310 && turn_v2 == 1) {
						alien2_x_pos += 3;
						if (alien2_x_pos > 310) {
							turn_v2 = 0;
						}
					}						
				}			
			}	
		}
	}
	// erase alien
	else if (key == 3)
	{
		erase_alien2(alien2_x_pos, alien2_y_pos);
	}
}
// draws, animates, and erases the alien
void alien3(int key)
{
	// initialize location of alien
	if (key == 0)
	{
			alien3_x_pos = (rand() % (319 - 110))+110;
			alien3_y_pos = 10;	
	}
	// draw alien at the top 
	else if (key == 1)
	{
		if (m1_v3 != 1) {
			draw_alien3(alien3_x_pos, alien3_y_pos);
		}	
	}
	// animate alien
	else if (key == 2) 
	{
		if (m1_v3 == 1) {
			alien3(3);
			m2_v3 = 1;
		} // go down
		else if (m1_v3 == 0) {
			if (path_v3 == 0) {
				if (alien3_y_pos <= 240) {
					alien3_y_pos += 3;
					if (alien3_y_pos >= 230) {
						m1_v3 = 1;
						path_v3 = rand()%3;
					}
				}
			} // go right and then left
			else if (path_v3 == 1) {
				if (alien3_y_pos <= 240) {
					alien3_y_pos += 3;
					if (alien3_y_pos >= 230) {
						m1_v3 = 1;
						path_v3 = rand()%3;
					}
					if (alien3_x_pos <= 310 && turn2_v3 == 0) {
						alien3_x_pos += 3;
						if (alien3_x_pos > 310) {
							turn2_v3 = 1;
						}
					}
					else if (alien3_x_pos >= 12 && turn2_v3 == 1) {
						alien3_x_pos -= 3;
						if (alien3_x_pos < 12) {
							turn2_v3 = 0;
						}
					}						
				}			
			} // go left and then right
			else if (path_v3 == 2) {
				if (alien3_y_pos <= 240) {
					alien3_y_pos += 3;
					if (alien3_y_pos >= 230) {
						m1_v3 = 1;
						path_v3 = rand()%3;
					}
					if (alien3_x_pos >= 12 && turn_v3 == 0) {
						alien3_x_pos -= 3;
						if (alien3_x_pos < 12) {
							turn_v3 = 1;
						}
					}
					else if (alien3_x_pos <= 310 && turn_v3 == 1) {
						alien3_x_pos += 3;
						if (alien3_x_pos > 310) {
							turn_v3 = 0;
						}
					}						
				}			
			}	
		}
	}
	// erase alien
	else if (key == 3)
	{
		erase_alien3(alien3_x_pos, alien3_y_pos);
	}
	// draw alien in blue
	/*
	else if (key == 4)
	{
		if (m1 != 1) {
			draw_alien_blue(alien3_x_pos, alien3_y_pos);
		}
	}*/
}

// draws, animates, and erases the missile
void missile(int key, struct missiles *missile1)
{
	// draw missile at the top middle of the spaceship
	if (key == 1)
	{
		if (m0 != 1) {
			draw_missile(missile1->missile_x_pos, missile1->missile_y_pos);
		}
	}
	// animate missile
	else if (key == 2) 
	{
		if (m0 == 1) {
			// erase the missile
			erase_missile(missile1->missile_x_pos, missile1->missile_y_pos);			
			missile_array[num_of_missiles].missile_y_pos = NULL;
			num_of_missiles--;
		}
		else if (m0 == 0) {
			// make missile move up
			if (missile1->missile_y_pos >= 10) {
				missile1->missile_y_pos -= 3;
				if (missile1->missile_y_pos <= 9) {
					m0 = 1;
				}
			}
		}
	}
	// erase missile
	else if (key == 3)
	{
		erase_missile(missile1->missile_x_pos, missile1->missile_y_pos);
	}
}

// draws the missile
void draw_missile(int x, int y)
{
	draw_line(x, y, x, y-3, RED);
	draw_line(x, y-4, x, y-6, BLUE);
	draw_pixel(x+1, y-4, BLUE);
	draw_pixel(x+1, y-5, BLUE);
	draw_pixel(x-1, y-4, BLUE);
	draw_pixel(x-1, y-5, BLUE);
	draw_pixel(x+2, y-4, BLUE);
	draw_pixel(x-2, y-4, BLUE);
}
// erases the missile
void erase_missile(int x, int y)
{
	draw_line(x, y, x, y-3, BLACK);
	draw_line(x, y-4, x, y-6, BLACK);
	draw_pixel(x+1, y-4, BLACK);
	draw_pixel(x+1, y-5, BLACK);
	draw_pixel(x-1, y-4, BLACK);
	draw_pixel(x-1, y-5, BLACK);
	draw_pixel(x+2, y-4, BLACK);
	draw_pixel(x-2, y-4, BLACK);
}

// draws starship
void draw_starship(int x, int y)
{
	draw_line(x, y, x, y+13, WHITE); // center line
	draw_line(x+1, y+2, x+1, y+11, WHITE); // right line
	draw_line(x-1, y+2, x-1, y+11, WHITE); // left line
	draw_line(x-1, y+6, x+1, y+6, RED); // middle logo
	draw_pixel(x, y+5, RED); // middle logo
	draw_pixel(x-1, y+7, RED); 
	draw_pixel(x+1, y+7, RED); 
	draw_line(x+2, y+4, x+2, y+9, WHITE); // right line + 1
	draw_line(x-2, y+4, x-2, y+9, WHITE); // left line + 1
	draw_line(x-2, y+9, x-2, y+12, RED); 
	draw_line(x+2, y+9, x+2, y+12, RED); 
	draw_pixel(x-3, y+5, BLUE); 
	draw_pixel(x+3, y+5, BLUE);
	draw_line(x-3, y+6, x-3, y+9, WHITE); // right line + 2
	draw_line(x+3, y+6, x+3, y+9, WHITE); // left line + 2
	draw_line(x-3, y+10, x-3, y+12, RED);
	draw_line(x+3, y+10, x+3, y+12, RED);
	draw_line(x-4, y+2, x-4, y+9, WHITE); // right line + 3
	draw_line(x+4, y+2, x+4, y+9, WHITE); // left line + 3
	draw_pixel(x-4, y+2, RED);
	draw_pixel(x-4, y+3, RED);
	draw_pixel(x+4, y+2, RED);
	draw_pixel(x+4, y+3, RED);
	draw_pixel(x-4, y+6, BLUE);
	draw_pixel(x+4, y+6, BLUE);
	draw_line(x-5, y+8, x-5, y+10, WHITE); // right line + 4
	draw_line(x-6, y+6, x-6, y+11, WHITE); // right line + 5
	draw_pixel(x-6, y+4, RED);
	draw_pixel(x-6, y+5, RED);
	draw_line(x+5, y+8, x+5, y+10, WHITE); // left line + 4
	draw_line(x+6, y+6, x+6, y+11, WHITE); // left line + 5
	draw_pixel(x+6, y+4, RED);
	draw_pixel(x+6, y+5, RED);
}

void erase_starship(int x, int y) {
	draw_line(x, y, x, y+13, BLACK); // center line
	draw_line(x+1, y+2, x+1, y+11, BLACK); // right line
	draw_line(x-1, y+2, x-1, y+11, BLACK); // left line
	draw_line(x-1, y+6, x+1, y+6, BLACK); // middle logo
	draw_pixel(x, y+5, BLACK); // middle logo
	draw_pixel(x-1, y+7, BLACK); 
	draw_pixel(x+1, y+7, BLACK); 
	draw_line(x+2, y+4, x+2, y+9, BLACK); // right line + 1
	draw_line(x-2, y+4, x-2, y+9, BLACK); // left line + 1
	draw_line(x-2, y+9, x-2, y+12, BLACK); 
	draw_line(x+2, y+9, x+2, y+12, BLACK); 
	draw_pixel(x-3, y+5, BLACK); 
	draw_pixel(x+3, y+5, BLACK);
	draw_line(x-3, y+6, x-3, y+9, BLACK); // right line + 2
	draw_line(x+3, y+6, x+3, y+9, BLACK); // left line + 2
	draw_line(x-3, y+10, x-3, y+12, BLACK);
	draw_line(x+3, y+10, x+3, y+12, BLACK);
	draw_line(x-4, y+2, x-4, y+9, BLACK); // right line + 3
	draw_line(x+4, y+2, x+4, y+9, BLACK); // left line + 3
	draw_pixel(x-4, y+2, BLACK);
	draw_pixel(x-4, y+3, BLACK);
	draw_pixel(x+4, y+2, BLACK);
	draw_pixel(x+4, y+3, BLACK);
	draw_pixel(x-4, y+6, BLACK);
	draw_pixel(x+4, y+6, BLACK);
	draw_line(x-5, y+8, x-5, y+10, BLACK); // right line + 4
	draw_line(x-6, y+6, x-6, y+11, BLACK); // right line + 5
	draw_pixel(x-6, y+4, BLACK);
	draw_pixel(x-6, y+5, BLACK);
	draw_line(x+5, y+8, x+5, y+10, BLACK); // left line + 4
	draw_line(x+6, y+6, x+6, y+11, BLACK); // left line + 5
	draw_pixel(x+6, y+4, BLACK);
	draw_pixel(x+6, y+5, BLACK);
}

// draws and erases starship
void starship(int key)
{
	if (key == 0)
	{
		starship_x_pos = 160;
	}
	// draw starship
	else if (key == 1)
	{
		draw_starship(starship_x_pos, STARSHIP_Y_POS);
	}
	// erase starship
	else if (key == 3)
	{
		erase_starship(starship_x_pos, STARSHIP_Y_POS);
	}
	// invalid key given, print error message
	else
	{
		printf("invalid starship key");
	}
}

// draws background stars
void draw_stars(int x, int y, short int color)
{
	draw_pixel(x, y, color);
}

// draws, animates, and erases background
void background(int key)
{
	// initialize locations of background stars
	if (key == 0)
	{
		for (int i = 0; i < NUM_STARS; i++) {
			array_x_stars[i] = rand()%327;
			array_y_stars[i] = rand()%190;
		}
	}
	// draw background stars at random frequency
    else if (key == 1)
    {
        int color[10] = {WHITE,YELLOW,RED,GREEN,BLUE,CYAN,MAGENTA,GREY,PINK,ORANGE};

        short int rand_color;

        for(int i = 0; i < NUM_STARS; i++) {
            rand_color = color[rand()%10];
            draw_stars(array_x_stars[i], array_y_stars[i], rand_color);
            int k = rand()%NUM_STARS;
            int m = rand()%10;
            if (m >=3) {
                for (int j = 0; j < k; j++) {
                    draw_stars(array_x_stars[k], array_y_stars[k], BLACK);
                }
            }
        }
    }
	// animate background
	else if (key == 2) 
	{
		for(int i = 0; i < NUM_STARS; i++) {	
			array_y_stars[i] += 2;
			if (array_y_stars[i] >= 237) {
				array_y_stars[i] = rand()%70;
				array_x_stars[i] = rand()%327;
			}
		}
	}
	// erase background
	else if (key == 3)
	{
		for(int i = 0; i < NUM_STARS; i++) {
			draw_stars(array_x_stars[i], array_y_stars[i], BLACK);
		}
	}
}

void draw_alien(int x, int y)
{	
	// yellow
	draw_line(x, y, x, y+9, YELLOW); // center line
	draw_line(x+1, y+1, x+1, y+8, YELLOW); // right line
	draw_line(x-1, y+1, x-1, y+8, YELLOW); // left line
	draw_line(x+2, y+1, x+2, y+4, YELLOW); // right line + 1
	draw_line(x-2, y+1, x-2, y+4, YELLOW); // left line + 1
	// red
	draw_pixel(x+1, y+1, RED); // right eye
	draw_pixel(x+1, y+2, RED);
	draw_pixel(x+2, y+2, RED);
	draw_pixel(x-1, y+1, RED); // left eye
	draw_pixel(x-1, y+2, RED);
	draw_pixel(x-2, y+2, RED);
	draw_line(x-1, y+5, x+1, y+5, RED); // abdomen
	draw_line(x-1, y+6, x+1, y+6, RED);
	draw_line(x-1, y+8, x+1, y+8, RED); // stinger
	draw_pixel(x, y+9, RED);
	// blue
	draw_pixel(x+3, y+2, CYAN); // right antenna
	draw_pixel(x+4, y, CYAN);
	draw_pixel(x+4, y+1, CYAN);
	draw_pixel(x-3, y+2, CYAN); // left antenna
	draw_pixel(x-4, y, CYAN);
	draw_pixel(x-4, y+1, CYAN);
	draw_box(x+2, y+4, 1, 1, CYAN); // right wing
	draw_box(x+3, y+5, 3, 1, CYAN);
	draw_box(x-3, y+4, 1, 1, CYAN); // left wing
	draw_box(x-4, y+5, 3, 1, CYAN);
}

void erase_alien(int x, int y)
{	
	// yellow
	draw_line(x, y, x, y+9, BLACK); // center line
	draw_line(x+1, y+1, x+1, y+8, BLACK); // right line
	draw_line(x-1, y+1, x-1, y+8, BLACK); // left line
	draw_line(x+2, y+1, x+2, y+4, BLACK); // right line + 1
	draw_line(x-2, y+1, x-2, y+4, BLACK); // left line + 1
	// red
	draw_pixel(x+1, y+1, BLACK); // right eye
	draw_pixel(x+1, y+2, BLACK);
	draw_pixel(x+2, y+2, BLACK);
	draw_pixel(x-1, y+1, BLACK); // left eye
	draw_pixel(x-1, y+2, BLACK);
	draw_pixel(x-2, y+2, BLACK);
	draw_line(x-1, y+5, x+1, y+5, BLACK); // abdomen
	draw_line(x-1, y+6, x+1, y+6, BLACK);
	draw_line(x-1, y+8, x+1, y+8, BLACK); // stinger
	draw_pixel(x, y+9, BLACK);
	// blue
	draw_pixel(x+3, y+2, BLACK); // right antenna
	draw_pixel(x+4, y, BLACK);
	draw_pixel(x+4, y+1, BLACK);
	draw_pixel(x-3, y+2, BLACK); // left antenna
	draw_pixel(x-4, y, BLACK);
	draw_pixel(x-4, y+1, BLACK);
	draw_box(x+2, y+4, 1, 1, BLACK); // right wing
	draw_box(x+3, y+5, 3, 1, BLACK);
	draw_box(x-3, y+4, 1, 1, BLACK); // left wing
	draw_box(x-4, y+5, 3, 1, BLACK);
}
void draw_alien2(int x, int y)
{	
	// blue
	draw_line(x-1, y, x-1, y+2, BLUE); // center line
	draw_line(x+1, y, x+1, y+2, BLUE); // right line
	draw_line(x-1, y+5, x+1, y+5, BLUE); // left line
	draw_line(x-1, y+6, x+1, y+6, BLUE); // right line + 1
	draw_line(x-1, y+8, x+1, y+8, BLUE); // left line + 1
	// red
	draw_pixel(x-1, y+3, RED); // right eye
	draw_pixel(x+1, y+3, RED);// left eye
	
	//left wing:
	draw_pixel(x-4, y, RED);
	draw_pixel(x-6, y+7, RED);
	draw_pixel(x-3, y+9, RED);
	draw_line(x-6, y+1, x-4, y+1, RED); // stinger
	draw_line(x-6, y+2, x-4, y+2, RED); // stinger
	draw_line(x-6, y+3, x-4, y+3, RED); // stinger
	
	draw_line(x-5, y+4, x-2, y+4, RED); // stinger
	draw_line(x-4, y+5, x-2, y+5, RED); // stinger
	draw_line(x-5, y+6, x-2, y+6, RED); // stinger
	draw_line(x-5, y+7, x-2, y+7, RED); // stinger
	draw_line(x-5, y+8, x-3, y+8, RED); // stinger
	
	//right wing:
	draw_pixel(x+4, y, RED);
	draw_pixel(x+6, y+7, RED);
	draw_pixel(x+3, y+9, RED);
	draw_line(x+4, y+1, x+6, y+1, RED); // stinger
	draw_line(x+4, y+2, x+6, y+2, RED); // stinger
	draw_line(x+4, y+3, x+6, y+3, RED); // stinger
	
	draw_line(x+2, y+4, x+5, y+4, RED); // stinger
	draw_line(x+2, y+5, x+4, y+5, RED); // stinger
	draw_line(x+2, y+6, x+5, y+6, RED); // stinger
	draw_line(x+2, y+7, x+5, y+7, RED); // stinger
	draw_line(x+3, y+8, x+5, y+8, RED); // stinger
	
	
	
	// grey
	draw_pixel(x+2, y+2, GREY); // right antenna
	draw_pixel(x-2, y+2, GREY);
	draw_pixel(x, y+2, GREY);
	
	draw_line(x-2, y+3, x+2, y+3, GREY); // stinger
	draw_line(x-1, y+4, x+1, y+4, GREY); // stinger
	
	draw_line(x-1, y+7, x+1, y+7, GREY); // stinger
	
}

void erase_alien2(int x, int y)
{	
		// blue
	draw_line(x-1, y, x-1, y+2, BLACK); // center line
	draw_line(x+1, y, x+1, y+2, BLACK); // right line
	draw_line(x-1, y+5, x+1, y+5, BLACK); // left line
	draw_line(x-1, y+6, x+1, y+6, BLACK); // right line + 1
	draw_line(x-1, y+8, x+1, y+8, BLACK); // left line + 1
	// red
	draw_pixel(x-1, y+3, BLACK); // right eye
	draw_pixel(x+1, y+3, BLACK);// left eye
	
	//left wing:
	draw_pixel(x-4, y, BLACK);
	draw_pixel(x-6, y+7, BLACK);
	draw_pixel(x-3, y+9, BLACK);
	draw_line(x-6, y+1, x-4, y+1, BLACK); // stinger
	draw_line(x-6, y+2, x-4, y+2, BLACK); // stinger
	draw_line(x-6, y+3, x-4, y+3, BLACK); // stinger
	
	draw_line(x-5, y+4, x-2, y+4, BLACK); // stinger
	draw_line(x-4, y+5, x-2, y+5, BLACK); // stinger
	draw_line(x-5, y+6, x-2, y+6, BLACK); // stinger
	draw_line(x-5, y+7, x-2, y+7, BLACK); // stinger
	draw_line(x-5, y+8, x-3, y+8, BLACK); // stinger
	
	//right wing:
	draw_pixel(x+4, y, BLACK);
	draw_pixel(x+6, y+7, BLACK);
	draw_pixel(x+3, y+9, BLACK);
	draw_line(x+4, y+1, x+6, y+1, BLACK); // stinger
	draw_line(x+4, y+2, x+6, y+2, BLACK); // stinger
	draw_line(x+4, y+3, x+6, y+3, BLACK); // stinger
	
	draw_line(x+2, y+4, x+5, y+4, BLACK); // stinger
	draw_line(x+2, y+5, x+4, y+5, BLACK); // stinger
	draw_line(x+2, y+6, x+5, y+6, BLACK); // stinger
	draw_line(x+2, y+7, x+5, y+7, BLACK); // stinger
	draw_line(x+3, y+8, x+5, y+8, BLACK); // stinger
	
	
	
	// grey
	draw_pixel(x+2, y+2, BLACK); // right antenna
	draw_pixel(x-2, y+2, BLACK);
	draw_pixel(x, y+2, BLACK);
	
	draw_line(x-2, y+3, x+2, y+3, BLACK); // stinger
	draw_line(x-1, y+4, x+1, y+4, BLACK); // stinger
	
	draw_line(x-1, y+7, x+1, y+7, BLACK); // stinger
}
void draw_alien3(int x, int y)
{	
	// yellow
	draw_line(x-2, y+5, x-1, y+5, YELLOW); // center line
	draw_line(x+1, y+5, x+2, y+5, YELLOW); // right line
	draw_line(x-2, y+6, x+2, y+6, YELLOW); // left line
	draw_line(x-2, y+7, x+2, y+7, YELLOW); // right line + 1
	draw_line(x-2, y+8, x+2, y+8, YELLOW); // left line + 1
	// red

	draw_line(x-2, y+2, x-1, y+2, RED); // abdomen
	draw_line(x-2, y+3, x-1, y+3, RED);
	draw_line(x+1, y+2, x+2, y+2, RED); // stinger
	draw_line(x+1, y+3, x+2, y+3, RED); // stinger
	
	draw_line(x-1, y+9, x-1, y+10, RED); // abdomen
	draw_line(x+1, y+9, x+1, y+10, RED);
	draw_line(x-5, y+9, x-5, y+10, RED); // stinger
	draw_line(x+5, y+9, x+5, y+10, RED); // stinger
	
	draw_line(x-6, y+11, x-6, y+13, RED); // abdomen
	draw_line(x-5, y+12, x-5, y+13, RED);
	draw_line(x+6, y+11, x+6, y+13, RED); // stinger
	draw_line(x+5, y+12, x+5, y+13, RED); // stinger
	
	
	// green
	draw_pixel(x-3, y+3, GREEN); // right antenna
	draw_pixel(x+3, y+3, GREEN);
	draw_line(x-1, y, x-1, y+1, GREEN); // stinger
	draw_line(x+1, y, x+1, y+1, GREEN); // stinger
	
	draw_line(x, y+2, x, y+5, GREEN); // stinger
	draw_line(x-2, y+4, x-1, y+4, GREEN); // stinger
	draw_line(x+1, y+4, x+2, y+4, GREEN); // stinger
	draw_line(x-4, y+2, x-3, y+2, GREEN); // stinger
	draw_line(x+3, y+2, x+4, y+2, GREEN); // stinger
	
	//left wing
	
	draw_pixel(x-7, y+7, GREEN); // right antenna
	draw_pixel(x-5, y+11, GREEN); // right antenna
	draw_line(x-3, y+5, x-3, y+9, GREEN); // stinger
	draw_line(x-4, y+6, x-4, y+15, GREEN); // stinger
	draw_line(x-5, y+6, x-5, y+8, GREEN); // stinger
	draw_line(x-6, y+7, x-6, y+10, GREEN); // stinger
	draw_line(x-7, y+10, x-7, y+14, GREEN); // stinger
	
	draw_line(x-6, y+15, x-6, y+16, GREEN); // stinger
	draw_line(x-5, y+15, x-5, y+16, GREEN); // stinger
	
	//right wing
	draw_pixel(x+7, y+7, GREEN); // right antenna
	draw_pixel(x+5, y+11, GREEN); // right antenna
	draw_line(x+3, y+5, x+3, y+9, GREEN); // stinger
	
	draw_line(x+4, y+6, x+4, y+15, GREEN); // stinger
	draw_line(x+5, y+6, x+5, y+8, GREEN); // stinger
	draw_line(x+6, y+7, x+6, y+10, GREEN); // stinger
	draw_line(x+7, y+10, x+7, y+14, GREEN); // stinger
	
	draw_line(x+6, y+15, x+6, y+16, GREEN); // stinger
	draw_line(x+5, y+15, x+5, y+16, GREEN); // stinger
}
void draw_alien_blue(int x, int y)
{
	// yellow
	draw_line(x-2, y+5, x-1, y+5, YELLOW); // center line
	draw_line(x+1, y+5, x+2, y+5, YELLOW); // right line
	draw_line(x-2, y+6, x+2, y+6, YELLOW); // left line
	draw_line(x-2, y+7, x+2, y+7, YELLOW); // right line + 1
	draw_line(x-2, y+8, x+2, y+8, YELLOW); // left line + 1
	// red

	draw_line(x-2, y+2, x-1, y+2, RED); // abdomen
	draw_line(x-2, y+3, x-1, y+3, RED);
	draw_line(x+1, y+2, x+2, y+2, RED); // stinger
	draw_line(x+1, y+3, x+2, y+3, RED); // stinger
	
	draw_line(x-1, y+9, x-1, y+10, RED); // abdomen
	draw_line(x+1, y+9, x+1, y+10, RED);
	draw_line(x-5, y+9, x-5, y+10, RED); // stinger
	draw_line(x+5, y+9, x+5, y+10, RED); // stinger
	
	draw_line(x-6, y+11, x-6, y+13, RED); // abdomen
	draw_line(x-5, y+12, x-5, y+13, RED);
	draw_line(x+6, y+11, x+6, y+13, RED); // stinger
	draw_line(x+5, y+12, x+5, y+13, RED); // stinger
	
	
	// green
	draw_pixel(x-3, y+3, BLUE); // right antenna
	draw_pixel(x+3, y+3, BLUE);
	draw_line(x-1, y, x-1, y+1, BLUE); // stinger
	draw_line(x+1, y, x+1, y+1, BLUE); // stinger
	
	draw_line(x, y+2, x, y+5, BLUE); // stinger
	draw_line(x-2, y+4, x-1, y+4, BLUE); // stinger
	draw_line(x+1, y+4, x+2, y+4, BLUE); // stinger
	draw_line(x-4, y+2, x-3, y+2, BLUE); // stinger
	draw_line(x+3, y+2, x+4, y+2, BLUE); // stinger
	
	//left wing
	
	draw_pixel(x-7, y+7, BLUE); // right antenna
	draw_pixel(x-5, y+11, BLUE); // right antenna
	draw_line(x-3, y+5, x-3, y+9, BLUE); // stinger
	draw_line(x-4, y+6, x-4, y+15, BLUE); // stinger
	draw_line(x-5, y+6, x-5, y+8, BLUE); // stinger
	draw_line(x-6, y+7, x-6, y+10, BLUE); // stinger
	draw_line(x-7, y+10, x-7, y+14, BLUE); // stinger
	
	draw_line(x-6, y+15, x-6, y+16, BLUE); // stinger
	draw_line(x-5, y+15, x-5, y+16, BLUE); // stinger
	
	//right wing
	draw_pixel(x+7, y+7, BLUE); // right antenna
	draw_pixel(x+5, y+11, BLUE); // right antenna
	draw_line(x+3, y+5, x+3, y+9, BLUE); // stinger
	
	draw_line(x+4, y+6, x+4, y+15, BLUE); // stinger
	draw_line(x+5, y+6, x+5, y+8, BLUE); // stinger
	draw_line(x+6, y+7, x+6, y+10, BLUE); // stinger
	draw_line(x+7, y+10, x+7, y+14, BLUE); // stinger
	
	draw_line(x+6, y+15, x+6, y+16, BLUE); // stinger
	draw_line(x+5, y+15, x+5, y+16, BLUE); // stinger	
}
void erase_alien3(int x, int y)
{	
	// yellow
	draw_line(x-2, y+5, x-1, y+5, BLACK); // center line
	draw_line(x+1, y+5, x+2, y+5, BLACK); // right line
	draw_line(x-2, y+6, x+2, y+6, BLACK); // left line
	draw_line(x-2, y+7, x+2, y+7, BLACK); // right line + 1
	draw_line(x-2, y+8, x+2, y+8, BLACK); // left line + 1
	// red

	draw_line(x-2, y+2, x-1, y+2, BLACK); // abdomen
	draw_line(x-2, y+3, x-1, y+3, BLACK);
	draw_line(x+1, y+2, x+2, y+2, BLACK); // stinger
	draw_line(x+1, y+3, x+2, y+3, BLACK); // stinger
	
	draw_line(x-1, y+9, x-1, y+10, BLACK); // abdomen
	draw_line(x+1, y+9, x+1, y+10, BLACK);
	draw_line(x-5, y+9, x-5, y+10, BLACK); // stinger
	draw_line(x+5, y+9, x+5, y+10, BLACK); // stinger
	
	draw_line(x-6, y+11, x-6, y+13, BLACK); // abdomen
	draw_line(x-5, y+12, x-5, y+13, BLACK);
	draw_line(x+6, y+11, x+6, y+13, BLACK); // stinger
	draw_line(x+5, y+12, x+5, y+13, BLACK); // stinger
	
	
	// green
	draw_pixel(x-3, y+3, BLACK); // right antenna
	draw_pixel(x+3, y+3, BLACK);
	draw_line(x-1, y, x-1, y+1, BLACK); // stinger
	draw_line(x+1, y, x+1, y+1, BLACK); // stinger
	
	draw_line(x, y+2, x, y+5, BLACK); // stinger
	draw_line(x-2, y+4, x-1, y+4, BLACK); // stinger
	draw_line(x+1, y+4, x+2, y+4, BLACK); // stinger
	draw_line(x-4, y+2, x-3, y+2, BLACK); // stinger
	draw_line(x+3, y+2, x+4, y+2, BLACK); // stinger
	
	//left wing
	
	draw_pixel(x-7, y+7, BLACK); // right antenna
	draw_pixel(x-5, y+11, BLACK); // right antenna
	draw_line(x-3, y+5, x-3, y+9, BLACK); // stinger
	draw_line(x-4, y+6, x-4, y+15, BLACK); // stinger
	draw_line(x-5, y+6, x-5, y+8, BLACK); // stinger
	draw_line(x-6, y+7, x-6, y+10, BLACK); // stinger
	draw_line(x-7, y+10, x-7, y+14, BLACK); // stinger
	
	draw_line(x-6, y+15, x-6, y+16, BLACK); // stinger
	draw_line(x-5, y+15, x-5, y+16, BLACK); // stinger
	
	//right wing
	draw_pixel(x+7, y+7, BLACK); // right antenna
	draw_pixel(x+5, y+11, BLACK); // right antenna
	draw_line(x+3, y+5, x+3, y+9, BLACK); // stinger
	
	draw_line(x+4, y+6, x+4, y+15, BLACK); // stinger
	draw_line(x+5, y+6, x+5, y+8, BLACK); // stinger
	draw_line(x+6, y+7, x+6, y+10, BLACK); // stinger
	draw_line(x+7, y+10, x+7, y+14, BLACK); // stinger
	
	draw_line(x+6, y+15, x+6, y+16, BLACK); // stinger
	draw_line(x+5, y+15, x+5, y+16, BLACK); // stinger
}

// keyboard controls
void keyboard() {
	PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
	RVALID = PS2_data & 0x8000; // extract the RVALID field
	unsigned char keyPress; // var to store key press

	if (RVALID) {
		// read key press
		keyPress = PS2_data & 0xFF;
		
		// right arrow key is pressed
		if (keyPress == 't' && starship_x_pos <= 310) {
			starship(3);
			starship_x_pos += 3;
		}
		// left arrow key is pressed
		else if (keyPress == 'k' && starship_x_pos >= 6) {
			starship(3);
			starship_x_pos -= 3;
		}
		// spacebar is pressed
		else if (keyPress == ')') {
			m0 = 0;
			missile_array[num_of_missiles].missile_x_pos = starship_x_pos;
			missile_array[num_of_missiles].missile_y_pos = STARSHIP_Y_POS - 3;
			if (num_of_missiles < 3) {
				num_of_missiles++;
			}	
		}
		else {
			// do nothing
		}
		
	}
}

// display start screen messages
void display_start_screen() {
	// clear character buffer
	clear_character_buffer();
	char* text;
	int x, y;
	text = "WELCOME PLAYER!";
   	x = 33;
   	y = 9;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "---------------";
   	x = 33;
   	y = 14;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
   	text = "GALAGA";
   	x = 37;
   	y = 16;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "---------------";
   	x = 33;
   	y = 18;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "PUSH START BUTTON (Key 3)";
   	x = 28;
   	y = 22;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "Key 0 OR RIGHT ARROW TO MOVE RIGHT";
   	x = 23;
   	y = 24;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "Key 1 OR LEFT ARROW TO MOVE LEFT";
   	x = 24;
   	y = 26;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "Key 2 OR SPACEBAR TO SHOOT";
   	x = 28;
   	y = 28;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "BUTTERFLY - 60 POINTS";
   	x = 30;
   	y = 32;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
   	text = "BEE - 80 POINTS";
   	x = 33;
   	y = 36;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
   	text = "GALAGA BOSS - 100 POINTS";
   	x = 28;
   	y = 41;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
	text = "ENJOY!";
   	x = 37;
   	y = 46;
   	while (*text) {
    	write_character(x, y, *text);
	 	x++;
	 	text++;
	}
}

// displays end screen messages
void display_end_screen() {
	// clear character buffer
	clear_character_buffer();
	int num;
	int temp = score;
	// get number of digits in score
	while (temp != 0) {
        temp /= 10;    
        num++;
    }
	// convert score from int to string
	char score_string[num];
	sprintf(score_string, "%i", score);
	
	if (gameWon == false) {
		// If player dies
		char* text = "GAME OVER";
   		int x = 35;
   		int y = 32;
   		while (*text) {
    		write_character(x, y, *text);
	 		x++;
	 		text++;
		}
		text = "SCORE: ";
   		x = 35;
   		y = 36;
   		while (*text) {
    		write_character(x, y, *text);
	 		x++;
	 		text++;
		}
		text = score_string;
   		x = 41;
   		y = 36;
   		while (*text) {
    		write_character(x, y, *text);
	 		x++;
	 		text++;
		}
	} 
	else {
		// If player wins
		char* text = "GAME WON";
   		int x = 36;
   		int y = 32;
   		while (*text) {
    		write_character(x, y, *text);
	 		x++;
	 		text++;
		}
		text = "SCORE: ";
   		x = 35;
   		y = 36;
   		while (*text) {
    		write_character(x, y, *text);
	 		x++;
	 		text++;
		}
		text = score_string;
   		x = 41;
   		y = 36;
   		while (*text) {
    		write_character(x, y, *text);
	 		x++;
	 		text++;
		}
	}
}

// writes a character into FPGA Character buffer with x and y positions
void write_character(int x, int y, char c) {
	volatile char * characterBuffer = (char *)(FPGA_CHAR_BASE + (y<<7) + x);
	*characterBuffer = c;
}

// clear the character buffer with whitespace
void clear_character_buffer() {
	for (int x = 0; x < 80; x++){
		for(int y = 0; y < 60; y++) {
			char * space = " ";
			write_character(x, y, *space);
		}
	}
}

// displays the score on the 7 segment displays
// partially taken from de1soc arm manual
void display_score() {
	unsigned char seven_seg_decode_table[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
	int first_digit = score % 10;
	int second_digit = (score / 10) % 10;
	int third_digit = (score / 100) % 10;
	int fourth_digit = (score / 1000) % 10;
	int fifth_digit = (score / 10000) % 10;
	int sixth_digit = (score / 100000) % 10;
	*HEX3_HEX0_ptr = seven_seg_decode_table[fourth_digit] << 24 | seven_seg_decode_table[third_digit] << 16 | seven_seg_decode_table[second_digit] << 8 | seven_seg_decode_table[first_digit];
	*HEX5_HEX4_ptr = seven_seg_decode_table[sixth_digit] << 8 | seven_seg_decode_table[fifth_digit];
}

/*
 * the following interrupt functions are taken from the de1soc arm document
 */
void config_KEYs() {
	volatile int * KEY_ptr = (int *)KEY_BASE; // pushbutton KEY base address
	*(KEY_ptr + 2) = 0xF; // enable interrupts for the two KEYs
}

void pushbutton_ISR(void);

// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void) {
	// Read the ICCIAR from the processor interface
	int address = MPCORE_GIC_CPUIF + ICCIAR;
	int int_ID = *((int *)address);

	if (int_ID == 73) // check if interrupt is from the KEYs
		pushbutton_ISR();
	else
		while (1)
			; // if unexpected, then stay here
	
	// Write to the End of Interrupt Register (ICCEOIR)
	address = MPCORE_GIC_CPUIF + ICCEOIR;
	*((int *)0xFFFEC110) = int_ID;
}

// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void) {
	while (1)
		;
}
void __attribute__((interrupt)) __cs3_isr_undef(void) {
	while (1)
		;
}
void __attribute__((interrupt)) __cs3_isr_swi(void) {
	while (1)
		;
}
void __attribute__((interrupt)) __cs3_isr_pabort(void) {
	while (1)
		;
}
void __attribute__((interrupt)) __cs3_isr_dabort(void) {
	while (1)
		;
}
void __attribute__((interrupt)) __cs3_isr_fiq(void) {
	while (1)
		;
}

// turn off interrupts in the ARM processor
void disable_A9_interrupts(void) {
	int status = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

// turn on interrupts in the ARM processor
void enable_A9_interrupts(void) {
	int status = 0b01010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/*
* Initialize the banked stack pointer register for IRQ mode
*/
void set_A9_IRQ_stack(void) {
	int stack, mode;
	stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes
	/* change processor to IRQ mode with interrupts disabled */
	mode = 0b11010010;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r"(stack));
	/* go back to SVC mode before executing subroutine return! */
	mode = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

/*
* Configure the Generic Interrupt Controller (GIC)
*/
void config_GIC(void) {
	int address; // used to calculate register addresses

	/* configure the FPGA interval timer and KEYs interrupts */
	*((int *)0xFFFED848) = 0x00000101;
	*((int *)0xFFFED108) = 0x00000300;

	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
	// priorities
	address = MPCORE_GIC_CPUIF + ICCPMR;
	*((int *)address) = 0xFFFF;
	// Set CPU Interface Control Register (ICCICR). Enable signaling of
	// interrupts
	address = MPCORE_GIC_CPUIF + ICCICR;
	*((int *)address) = 1;
	// Configure the Distributor Control Register (ICDDCR) to send pending
	// interrupts to CPUs
	address = MPCORE_GIC_DIST + ICDDCR;
	*((int *)address) = 1;
}

/***************************************************************************************
* Pushbutton - Interrupt Service Routine
*
* This routine toggles the key_dir variable from 0 <-> 1
****************************************************************************************/
void pushbutton_ISR(void) {

	volatile int * KEY_ptr = (int *)KEY_BASE;
	int press;

	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // clear the interrupt
	
	// Key 0 is pressed -> move spaceship to the right
	if (press & 0x1){ 
		starship(3);
		starship_x_pos += 3;
		
		// Prevent starship from moving off screen on the right
		if(starship_x_pos >= 310){    
			starship_x_pos=310;
		}
	}
	// Key 1 is pressed -> move spaceship to the left
	else if (press & 0x2){ 
		starship(3);
		starship_x_pos -= 3;

		// Prevent starship from moving off screen on the left
		if(starship_x_pos <= 5){    
			starship_x_pos=5;
		}
	}
	// Key 2 is pressed -> shoot missile
	else if (press & 0x4){ 
		m0 = 0;
		missile_array[num_of_missiles].missile_x_pos = starship_x_pos;
		missile_array[num_of_missiles].missile_y_pos = STARSHIP_Y_POS - 3;
		if (num_of_missiles < 3) {
			num_of_missiles++;
		}
	}
	// Key 3 is pressed -> start the game
	if (press & 0x8) { 
		clear_character_buffer();
		if (start_game == false){
		    start_game = true;
		} 
	}

	return;
}

// function for swapping taken from lab 7 part 1
void swap(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

// function for drawing a line taken from lab 7 part 2
void draw_line(int x0, int y0, int x1, int y1, short int color){
	
	bool is_steep = abs(y1 - y0) > abs(x1 - x0);
	
	if (is_steep) {
			swap(&x0, &y0);
		    swap(&x1, &y1);
	}
	if(x0>x1){
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	
	int deltax = x1-x0;
	int deltay = abs(y1-y0);
	
	int error = -(deltax/2);
	
	int y = y0;
	int y_step = 0;
	if(y0 < y1){
		y_step = 1;
	}
	else{
		y_step = -1;
	}
	for(int x = x0; x<= x1; x++){
		if(is_steep){
			draw_pixel(y, x, color);
			
		}
		else{
			draw_pixel( x,y, color);
		}
		
		error = error + deltay;
		if(error >= 0){
			y = y + y_step;
			error = error - deltax;
		}	
	}
}

// draws and erases explosions
void explosion(int key){
	// draw background stars at random frequency
	if (key == 1)
	{
		draw_explosion(array_x_explosion,array_y_explosion, RED);
	}
	// erase background
	else if (key == 3)
	{
		draw_explosion(array_x_explosion, array_y_explosion, BLACK);	
	}
}

// draws explosion sprite
void draw_explosion(int x, int y, short int color){
	draw_box(x, y, 3, 3, color); // center
	draw_box(x-3, y-3, 3, 3, color); // left
	draw_box(x-6, y-6, 3, 3, color); // right
	draw_box(x-9, y-9, 3, 3, color); // left diagonal
	draw_box(x+3, y-3, 3, 3, color); // right diagonal
	draw_box(x+6, y-6, 3, 3, color); // right diagona
	draw_box(x+9, y-9, 3, 3, color); // right diagonal
	draw_box(x-3, y+3, 3, 3, color); // center
	draw_box(x-6, y+6, 3, 3, color); // left
	draw_box(x-9, y+9, 3, 3, color); // right
	draw_box(x+3, y+3, 3, 3, color); // left diagonal
	draw_box(x+6, y+6, 3, 3, color); // right diagonal
	draw_box(x+9, y+9, 3, 3, color); // right diagonal
}

// clears whole screen
void clear_screen(){
	for(int i = 0 ; i< 320; i++){
		for(int j = 0; j <240; j++){
			draw_pixel(i, j, BLACK);
		}
	}
}

// draws a single pixel on screen
void draw_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// function to launch swap cycle and waiting for swap
void wait_for_vsync(){
	volatile int* pixel_buffer_start1 = (int*) 0xff203020;
	int status;
 
	//put 1 into front buffer adress to initiate the swap process
	*pixel_buffer_start1 = 1;
	status = *(pixel_buffer_start1 +3);
	//polling for status bit
	while((status&0x01)!=0){
		
		status = *(pixel_buffer_start1+3);
		
	}
}

// draws a box with adjustable height and length
void draw_box(int x, int y, int height, int length, short int color)
{
	for (int i = y; i <= y + height; i++) {
		for (int j = x; j <= x + length; j++) {
			draw_pixel(j, i, color);
		}
	}
}