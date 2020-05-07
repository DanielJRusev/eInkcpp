#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdexcept>

#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb-master/stb_image.h"



void SleepMs(int ms);
void sendImage(char *filename, int cdc);

unsigned char RESOLUTION[] = { 0xA8, 0x00, 0x0C, 0x01, 0x00, 0x00, 0x06, 0x40, 0x04, 0xB0, 0x00, 0x00, 0x00 };  	//Display Resolution 1600 x 1200
unsigned char VCOM[] = { 0xA8, 0x00, 0x0A, 0x03, 0x00, 0x00, 0xf9, 0x0C, 0x00, 0x00, 0x00 };              			//VCOM -1780 mV    = 0xf830 
unsigned char DGREY_LAVEL[] = { 0xA8, 0x00, 0x09, 0x04, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00 };			           	//GREY_LAVEL         16Bit
unsigned char CONTRAST[] = { 0xA8, 0x00, 0x09, 0x06, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00 };					   		//CONTRAST           50
unsigned char BUS[] = { 0xA8, 0x00, 0x09, 0x05, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00 };                    			//BUS                8Bit
unsigned char CLEAR_SCREEN[] = { 0xA8, 0x00, 0x09, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };                    	//CMD                CLEAR_SCREEN
unsigned char WHITE_SCREEN[] = { 0xA8, 0x00, 0x09, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };                    	//CMD			     White screen
unsigned char BLACK_SCREEN[] = { 0xA8, 0x00, 0x09, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };                    	//CMD				 Black screen
unsigned char SHOW_THE_PICTURE[] = { 0xA8, 0x00, 0x09, 0x08, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };                  //CMD                RAM2Display


int main(int argc, char ** argv) {

 	int cdc_filestream = -1;

	//OPEN THE UART
	
	cdc_filestream = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	
	

	if (cdc_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		perror("open_port: Unable to open /dev/ttyACM0 - ");
		return(-1);
	}
	
	//CONFIGURE THE UART
	
	struct termios options;
	tcgetattr(cdc_filestream, &options);
	options.c_cflag = B1000000 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(cdc_filestream, TCIFLUSH);
	tcsetattr(cdc_filestream, TCSANOW, &options);
		
	// Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
	fcntl(cdc_filestream, F_SETFL, 0);

	// Write to the port
	
	    write(cdc_filestream, RESOLUTION,	0x0C);
	    SleepMs(10);
	   	write(cdc_filestream, VCOM,			0x0A);
		SleepMs(10);
		write(cdc_filestream, DGREY_LAVEL,	0x09);
		SleepMs(10);
		write(cdc_filestream, CONTRAST,		0x09);
		SleepMs(10);
		write(cdc_filestream, BUS,			0x09);

		SleepMs(100);

		write(cdc_filestream, CLEAR_SCREEN, 0x09);

		SleepMs(500);

		sendImage(argv[1], cdc_filestream);
		SleepMs(10);
		int n = write(cdc_filestream, SHOW_THE_PICTURE, 0x09);
		SleepMs(2000);
				
		//sendImage("/home/pi/Desktop/1280x800/1.jpg", cdc_filestream);
				
		sendImage( (char*)("1.jpg"), cdc_filestream);  // Open file 
		SleepMs(10);
	int n = write(cdc_filestream, SHOW_THE_PICTURE, 0x09);
		SleepMs(2000);

		sendImage((char*)("2.jpg"), cdc_filestream);  // Open file 
		SleepMs(10);
	    n = write(cdc_filestream, SHOW_THE_PICTURE, 0x09);
		SleepMs(2000);
		
		sendImage((char*)("3.jpg"), cdc_filestream);  // Open file 
		SleepMs(10);
		n = write(cdc_filestream, SHOW_THE_PICTURE, 0x09);
		SleepMs(2000);
						
		sendImage((char*)("4.jpg"), cdc_filestream);  // Open file 
		SleepMs(10);
		n = write(cdc_filestream, SHOW_THE_PICTURE, 0x09);
		SleepMs(2000);
		
		
		SleepMs(500);

		n = write(cdc_filestream, CLEAR_SCREEN, 0x09);
		SleepMs(500);

				
		n = write(cdc_filestream, BLACK_SCREEN, 0x09);
		SleepMs(500);

		
		n = write(cdc_filestream, WHITE_SCREEN, 0x09);
		SleepMs(500);

	
	if (n < 0) {
		perror("Write failed - ");
		return -1;
	}


	
	// Don't forget to clean up
	close(cdc_filestream);
	return 0;
}


void sendImage(char *filename, int cdc)
{
	int width, height, bpp;

	uint8_t* image = stbi_load(filename, &width, &height, &bpp, 1);

	if (!image) {
		fprintf(stderr, "Couldn't load image.\n");
	}
	else
	{
		if ((width > 1600) || (height > 1200))
		{
			printf("Image size is to big \n");
		}
		else
		{
			unsigned char frame[(width/2) + 8];
			frame[0] = 0xA8;
			frame[1] = ((8 + (width / 2)) >> 8) & 0xFF;  // FrameLength HB
			frame[2] = ((8 + (width / 2))     ) & 0xFF;  // FrameLength LB
			frame[3] = 0x07;
			frame[4] = 0x00;
			frame[5] = 0x00;

			unsigned char IMAGE_RESOLUTION[12] = { 0xA8, 0x00, 0x0C, 0x02};      //Image Resolution 
			
			IMAGE_RESOLUTION[6] = (width >> 8)  & 0x0F;
			IMAGE_RESOLUTION[7] =  width        & 0xFF;

			IMAGE_RESOLUTION[8] = (height >> 8) & 0x0F;
			IMAGE_RESOLUTION[9] =  height       & 0xFF;

			write(cdc, IMAGE_RESOLUTION, sizeof(IMAGE_RESOLUTION));

			SleepMs(5);
			
			for (int i = 0; i < height; i++)
			{
				int c = 6;
				for (int b = 0; b < width;)
				{
					frame[c]     =  image[(i * width) + b++]       & 0xF0;
					frame[c++]  |= (image[(i * width) + b++] >> 4) & 0x0F;
					
				}

				frame[4] = (height  >> 8) & 0x0F;    // DataCounter HB
				frame[5] =  height        & 0xFF;    // DataCounter LB

				write(cdc, frame, 8 + (width / 2));
				SleepMs(10);                         //Wait 10 milliseconds for recive Package 
				
			}
		}

		stbi_image_free(image);
	}
}


void SleepMs(int ms) {
	usleep(ms * 1000); //convert to microseconds
	return;
}
