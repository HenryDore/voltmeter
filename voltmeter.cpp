//     ________________________________
//   / \                               \
//  |   |  VOLTMETER                   |
//   \_ |                              |
//      |  An absolutely horrid little |
//      |  voltmeter in curses for the |
//      |  Raspberry Pi & the AB       |
//      |  Electronics ADC/DAC.        |
//      |                              |
//      |  Henry Dore (2022)           |
//      |   ___________________________|___
//      |  /                              /
//      \_/______________________________/

#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdexcept>
#include <time.h>
#include <unistd.h>
#include <iostream>

#include "ABE_ADCDACPi.h"

using namespace ABElectronics_CPP_Libraries;

int main()
{	
	ADCDACPi adcdac;

	if (adcdac.open_adc() != 1){ // open the ADC spi channel
			return (1); // if the SPI bus fails to open exit the program
	}

	if (adcdac.open_dac() != 1){ // open the DAC spi channel
		return(1); // if the SPI bus fails to open exit the program
	}
	adcdac.set_dac_gain(2); // set the dac gain to 2 which will give a voltage range of 0 to 3.3V

	initscr();				// Start curses mode
	curs_set(0);			// Remove cursor
	nodelay(stdscr, TRUE);	//make getch() non blocking
	start_color();			// Start color
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);

	if(has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

	double DAC_value = 0.1;
	int ch;

	while ((ch = getch()) == ERR) {

		adcdac.set_dac_voltage(DAC_value, 1); // DAC output sweeps from 0 to 3.3V
		if ( DAC_value >= 3.3 ){
			DAC_value = 0;
		}
		else {
			DAC_value += 0.1; 
		}

		double ADC1 = adcdac.read_adc_voltage(1, 0);
		double ADC2 = adcdac.read_adc_voltage(2, 0);

		clear();		//clear screen

		//display raw values
		attron(COLOR_PAIR(1));
		mvprintw(0,4,"Pin 1: %G \n", ADC1); 
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2));
		mvprintw(1,4,"Pin 2: %G \n", ADC2);
		attroff(COLOR_PAIR(2));

		//draw 2 bar graphs
		int bar1 = (int) (ADC1*10);
		int bar2 = (int) (ADC2*10);
		move(2,1);
		for (int i = 0 ; i < bar1 ; i++){
			attron(COLOR_PAIR(1));
			printw("#");
			attroff(COLOR_PAIR(1));
		}
		move(3,1);
		for (int i = 0 ; i < bar2 ; i++){
			attron(COLOR_PAIR(2));
			printw("#");
			attroff(COLOR_PAIR(2));
		}

		//draw reamining UI
		mvprintw(4,0,"|.........|.........|.........|..."); 
		mvprintw(5,0,"0         1         2         3");
		mvprintw(6,0,"             Volts");
		mvprintw(8,0,"    (press any key to exit)");

		refresh();
		usleep(20000);
	}

	endwin();			// End curses mode
	adcdac.close_dac();
	return 0;
}
