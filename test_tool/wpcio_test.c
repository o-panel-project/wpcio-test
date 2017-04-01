#include <sys/timeb.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/tty.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "../wpcio.h"

#define FIXED_GPIO_USAGE

void print_help(void) {
	printf("\n");
	printf("~~~ Release date: 20170216 ~~~\n");
	printf("1 = Get DIP Switch\n");
	printf("2 = Get LED\n");
	printf("3 = Set LED\n");
	//printf("4 = Connect/Disconnect Cradle\n");
	printf("5 = Connect/Disconnect USB memory\n");
	printf("6 = Connect/Disconnect USB Felica\n");
	//printf("7 = Connect/Disconnect USB WIFI\n");
	printf("8 = Get USB Memory Overcurrent\n");
	//printf("9 = Get USB WIFI Overcurrent\n");
	printf("a = Get Battery 1 charging status\n");
	//printf("b = Get Battery 2 charging status\n");
	//printf("c = Reset USB Hub\n");
	printf("d = Connect/Disconnect SDIO WIFI\n");
	printf("e = get adc value for dc, bat1 \n");
	//printf("f = Connect/Disconnect Touch panel\n");
	//printf("g = Connect/Disconnect microSD (Warning, don't disconnect if it is root fs)\n");
	printf("h = set gpio as input\n");
	printf("i = set gpio as input with pullup\n");
	printf("j = set gpio as input with pulldown\n");
	printf("k = set gpio as output low\n");
	printf("m = set gpio as output high\n");
	printf("n = get gpio level\n");
	#if defined(FIXED_GPIO_USAGE)
		printf("p = BAT1 (GPIO18) charge on/off\n");
		//printf("q = BAT2 (GPIO54) charge on/off\n");
		printf("r = LCD BL (GPIO22) on/off\n");
		//printf("s = Cradle detect(GPIO56)  status\n");
		//printf("t = BAT2 detect (GPIO57) status\n");
		printf("u = Soft power control pin (GPIO116)\n");
	#endif
	printf("x = exit\n");
}

int isGPIOValid(int gpio) {
	int i;
	#if defined(FIXED_GPIO_USAGE)
		int g[] = {148};
	#else
		int g[] = {148, 116, 56, 57, 22, 18, 54}
	#endif
	for (i = 0; i < sizeof(g); i++) {
		if (gpio == g[i]) {
			printf("GPIO number not in range\n");
			return 0;
		}
	}
	return 1;
}

int get_int(char* prompt, int base) {
	char s[80];
	printf("%s", prompt);
	fgets(s, 80, stdin);
	return strtol(s, NULL, base);
}

int main(int argc, char **argv) {
	int io;
	if ((io = open("/dev/wpcio", O_RDWR)) < 0) {
		printf("Cannot open /dev/wpcio\n");
		return -1;
	}
	#if defined(FIXED_GPIO_USAGE)
		{
			int r;
			// set GPIO18, 54 and 22 to output
			r = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 18);
			//if (!r) r = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 54);
			if (!r) r = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 22);
			// set GPIO116 to output
			if (!r) r = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 116);
			/*
			// set GPIO 56 and 57 to input with pull low
			if (!r) r = ioctl(io, WPC_SET_GPIO_INPUT_PULLDOWN, 56);
			if (!r) r = ioctl(io, WPC_SET_GPIO_INPUT_PULLDOWN, 57);
			*/
			if (r) {
				printf("Cannot set gpio direction\n");
				return -1;
			}
		}
	#endif
	if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) {
			// On device
			if (!strcmp(argv[i], "USB_CRADLE_ON")) {
				printf("Connect USB Cradle\n");
				ioctl(io, WPC_CONNECT_CRADLE, 1);
			}
			else if (!strcmp(argv[i], "USB_MEMORY_ON")) {
				printf("Connect USB Memory\n");
				ioctl(io, WPC_CONNECT_USB_MEMORY, 1);
			}
			else if (!strcmp(argv[i], "USB_FELICA_ON")) {
				printf("Connect USB Felica\n");
				ioctl(io, WPC_CONNECT_USB_FELICA, 1);
			}
			else if (!strcmp(argv[i], "USB_WIFI_ON")) {
				printf("Connect USB WIFI\n");
				ioctl(io, WPC_CONNECT_USB_WIFI, 1);
			}
			else if (!strcmp(argv[i], "SDIO_WIFI_ON")) {
				printf("Connect SDIO WIFI\n");
				ioctl(io, WPC_CONNECT_SDIO_WIFI, 1);
			}
			// Off device
			else if (!strcmp(argv[i], "USB_CRADLE_OFF")) {
				printf("Connect USB Cradle\n");
				ioctl(io, WPC_CONNECT_CRADLE, 0);
			}
			else if (!strcmp(argv[i], "USB_MEMORY_OFF")) {
				printf("Connect USB Memory\n");
				ioctl(io, WPC_CONNECT_USB_MEMORY, 0);
			}
			else if (!strcmp(argv[i], "USB_FELICA_OFF")) {
				printf("Connect USB Felica\n");
				ioctl(io, WPC_CONNECT_USB_FELICA, 0);
			}
			else if (!strcmp(argv[i], "USB_WIFI_OFF")) {
				printf("Connect USB WIFI\n");
				ioctl(io, WPC_CONNECT_USB_WIFI, 0);
			}
			else if (!strcmp(argv[i], "SDIO_WIFI_OFF")) {
				printf("Connect SDIO WIFI\n");
				ioctl(io, WPC_CONNECT_SDIO_WIFI, 0);
			}
		}
		return 0;
	}
	while(1) {
		char c,d;
		print_help();
		c = getchar();
		if (c == '\r' || c == '\n') continue;
		while (1) {
			d = getchar();
			if (d == '\r' || d == '\n' || d == 0) break;
		}
		if (c == 'x') break;
		else if (c == '1') {
			// Get Dip switch
			int err;
			int data;
			err = ioctl(io, WPC_GET_DIPSW, &data);
			if (err < 0) {
				printf("Cannot get DIP Switch, error code = %d\n", err);
			}
			else {
				printf("DIP Switch 1 is %s\n", (data & WPC_DIPSW_1)? "Off":"On");
				printf("DIP Switch 2 is %s\n", (data & WPC_DIPSW_2)? "Off":"On");
				printf("DIP Switch 3 is %s\n", (data & WPC_DIPSW_3)? "Off":"On");
				printf("DIP Switch 4 is %s\n", (data & WPC_DIPSW_4)? "Off":"On");
			}
		}
		else if (c == '2') {
			// Get LED
			int err;
			int data;
			err = ioctl(io, WPC_GET_LED, &data);
			if (err < 0) {
				printf("Cannot get LED status, error code = %d\n", err);
			}
			else {
				printf("LED Green is %s\n", (data & WPC_LED_GREEN)? "Off":"On");
				printf("LED Red is %s\n", (data & WPC_LED_RED)? "Off":"On");
				printf("LED Orange is %s\n", (data & WPC_LED_ORANGE)? "Off":"On");
			}
		}
		else if (c == '3') {
			// Set LED
			int err;
			unsigned int data = get_int("Enter a value in binary for OrangeRedGreen: ", 2);
			data=~data;
			printf("data=%d \n",data );
			err = ioctl(io, WPC_SET_LED, data);
			if (err < 0) {
				printf("Cannot set LED, error code = %d\n", err);
			}
		}
		else if (c == '4') {
			// Connect/Disconnect Cradle
			int err;
			int data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
			err = ioctl(io, WPC_CONNECT_CRADLE, data);
			if (err < 0) {
				printf("Connect/Disconnect Cradle fail, error code = %d\n", err);
			}
		}
		else if (c == '5') {
			// Connect/Disconnect USB Memory
			int err;
			int data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
			err = ioctl(io, WPC_CONNECT_USB_MEMORY, data);
			if (err < 0) {
				printf("Connect/Disconnect USB Memory fail, error code = %d\n", err);
			}
		}
		else if (c == '6') {
			// Connect/Disconnect USB Felica
			int err;
			int data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
			err = ioctl(io, WPC_CONNECT_USB_FELICA, data);
			if (err < 0) {
				printf("Connect/Disconnect USB Felica fail, error code = %d\n", err);
			}
		}
		else if (c == '7') {
			// Connect/Disconnect USB WIFI
			int err;
			int data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
			err = ioctl(io, WPC_CONNECT_USB_WIFI, data);
			if (err < 0) {
				printf("Connect/Disconnect USB Felica fail, error code = %d\n", err);
			}
		}
		else if (c == '8') {
			// Get USB Memory overcurrent
			int err;
			int data;
			err = ioctl(io, WPC_GET_USB_MEMORY_OVERCUR, &data);
			if (err < 0) {
				printf("Cannot get USB Memory overcurrent, error code = %d\n", err);
			}
			else {
				if (data < 0) printf("USB Memory is off\n");
				else printf("USB Memory is %s\n", data? "Overcurrent":"Normal");
			}
		}
		else if (c == '9') {
			// Get USB WIFI overcurrent
			int err;
			int data;
			err = ioctl(io, WPC_GET_USB_WIFI_OVERCUR, &data);
			if (err < 0) {
				printf("Cannot get USB WIFI overcurrent, error code = %d\n", err);
			}
			else {
				if (data < 0) printf("USB WIFI is off\n");
				else printf("USB WIFI is %s\n", data? "Overcurrent":"Normal");
			}
		}
		else if (c == 'a') {
			// Get Battery 1 charging status
			int err;
			int data;
			err = ioctl(io, WPC_GET_BAT1_CHARGING_STAT, &data);
			if (err < 0) {
				printf("Cannot battery 1 charging status, error code = %d\n", err);
			}
			else {
				printf("Battery 1 charging fast is %s\n", (data & WPC_CHARGING_FAST)? "Asserted":"Deasserted");
				printf("Battery 1 charging full is %s\n", (data & WPC_CHARGING_FULL)? "Asserted":"Deasserted");
				printf("Battery 1 charging fault is %s\n", (data & WPC_CHARGING_FAULT)? "Asserted":"Deasserted");
			}
		}
		else if (c == 'b') {
			// Get Battery 2 charging status
			int err;
			int data;
			err = ioctl(io, WPC_GET_BAT2_CHARGING_STAT, &data);
			if (err < 0) {
				printf("Cannot battery 2 charging status, error code = %d\n", err);
			}
			else {
				printf("Battery 2 charging fast is %s\n", (data & WPC_CHARGING_FAST)? "Asserted":"Deasserted");
				printf("Battery 2 charging full is %s\n", (data & WPC_CHARGING_FULL)? "Asserted":"Deasserted");
				printf("Battery 2 charging fault is %s\n", (data & WPC_CHARGING_FAULT)? "Asserted":"Deasserted");
			}
		}
		else if (c == 'c') {
			// Reset USB HUB
			int err = ioctl(io, WPC_RESET_USB_HUB, NULL);
			if (err < 0) {
				printf("Cannot reset USB Hub, error code = %d\n", err);
			}
		}
		else if (c == 'd') {
			// Connect/Disconnect SDIO WIFI
			int err;
			int data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
			err = ioctl(io, WPC_CONNECT_SDIO_WIFI, data);
			if (err < 0) {
				printf("Connect/Disconnect SDIO WIFI, error code = %d\n", err);
			}
		}
		else if (c == 'e') {
			#define DC_RL	180
			#define DC_RH	750
			#define BAT_RL	180
			#define BAT_RH	510

			int dc, bat1, bat2;
			int err = ioctl(io, WPC_GET_DC_LEVEL, &dc);
			if (!err) err = ioctl(io, WPC_GET_BAT1_LEVEL, &bat1);
			if (!err) err = ioctl(io, WPC_GET_BAT2_LEVEL, &bat2);
			if (err < 0) {
				printf("Cannot get adc value, error code = %d\n", err);
			}
			printf("DC level = %dmV\n", dc  * 3 * 1000 * (DC_RL + DC_RH)   / DC_RL  / 1024);
			printf("BAT1 level = %dmV\n", bat1 * 3 * 1000 * (BAT_RL + BAT_RH) / BAT_RL / 1024);
			printf("BAT2 level = %dmV\n", bat2 * (BAT_RL + BAT_RH) / BAT_RL);
		}
		else if (c == 'f') {
			int err;
			int data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
			err = ioctl(io, WPC_SET_TP_ONOFF, data);
			if (err < 0) {
				printf("Connect/Disconnect SDIO WIFI, error code = %d\n", err);
			}
		}
		else if (c == 'g') {
			int err;
			int data;
			data = get_int("Warning! If the microSD is mounted as root FS, discoonect it may hang the system\n"
				           "Are you sure to do it?, 0 = no, 1 = yes: ", 2);
			if (data == 1) {
				data = get_int("Enter a value in binary, 0 = Disconnect, 1 = Connect: ", 2);
				err = ioctl(io, WPC_CONNECT_MMC1, data);
				if (err < 0) {
					printf("Connect/Disconnect SDIO WIFI, error code = %d\n", err);
				}
			}
		}

		// if defined FIXED_GPIO_USAGE, the only remained GPIO is 148
		else if (c == 'h') {
			// Set gpio as input
			int err;
			int data;
			data = get_int("gpio number: ", 10);
			if (isGPIOValid(data)) {
				err = ioctl(io, WPC_SET_GPIO_INPUT, data);
				if (err < 0) {
					printf("WPC_SET_GPIO_INPUT, error code = %d\n", err);
				}
			}
		}
		else if (c == 'i') {
			// Set gpio as input with pullup
			int err;
			int data;
			data = get_int("gpio number: ", 10);
			if (isGPIOValid(data)) {
				err = ioctl(io, WPC_SET_GPIO_INPUT_PULLUP, data);
				if (err < 0) {
					printf("WPC_SET_GPIO_INPUT_PULLUP, error code = %d\n", err);
				}
			}
		}
		else if (c == 'j') {
			// Set gpio as input with pulldown
			int err;
			int data;
			data = get_int("gpio number: ", 10);
			if (isGPIOValid(data)) {
				err = ioctl(io, WPC_SET_GPIO_INPUT_PULLDOWN, data);
				if (err < 0) {
					printf("WPC_SET_GPIO_INPUT_PULLDOWN, error code = %d\n", err);
				}
			}
		}
		else if (c == 'k') {
			// Set gpio as output low
			int err;
			int data;
			data = get_int("gpio number: ", 10);
			if (isGPIOValid(data)) {
				err = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, data);
				if (err < 0) {
					printf("WPC_SET_GPIO_OUTPUT_LOW, error code = %d\n", err);
				}
			}
		}
		else if (c == 'm') {
			// Set gpio as output high
			int err;
			int data;
			data = get_int("gpio number: ", 10);
			if (isGPIOValid(data)) {
				err = ioctl(io, WPC_SET_GPIO_OUTPUT_HIGH, data);
				if (err < 0) {
					printf("WPC_SET_GPIO_OUTPUT_HIGH, error code = %d\n", err);
				}
			}
		}
		else if (c == 'n') {
			// Get gpio level
			int err;
			int data;
			data = get_int("gpio number: ", 10);
			if (isGPIOValid(data)) {
				err = ioctl(io, WPC_GET_GPIO_LEVEL, data);
				if (err < 0) {
					printf("WPC_GET_GPIO_LEVEL, error code = %d\n", err);
				}
				else {
					printf("gpio_%d is %d\n", data, err);
				}
			}
		}
		#if defined(FIXED_GPIO_USAGE)
		else if (c == 'p') {
			// BAT1 charge on/off, GPIO 18
			int err;
			int data = get_int("BAT1 charge on/off. Enter a value, 0 = Off, 1 = On: ", 2);
			if (data == 0) err = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 18);
			else err = ioctl(io, WPC_SET_GPIO_OUTPUT_HIGH, 18);
			if (err < 0) {
				printf("BAT1 charge on/off, error code = %d\n", err);
			}
		}
		else if (c == 'q') {
			// BAT2 charge on/off, GPIO54
			int err;
			int data = get_int("BAT2 charge on/off. Enter a value, 0 = Off, 1 = On: ", 2);
			if (data == 0) err = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 54);
			else err = ioctl(io, WPC_SET_GPIO_OUTPUT_HIGH, 54);
			if (err < 0) {
				printf("BAT2 charge on/off, error code = %d\n", err);
			}
		}
		else if (c == 'r') {
			// LCD BL on/off, GPIO22
			int err;
			int data = get_int("LCD BL on/off. Enter a value, 0 = Off, 1 = On: ", 2);
			if (data == 0){ 
				ioctl(io, WPC_SET_GPIO_OUTPUT_HIGH, 22);
				err = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 22);
			
			}

			else err = ioctl(io, WPC_SET_GPIO_OUTPUT_HIGH, 22);
			if (err < 0) {
				printf("LCD BL on/off, error code = %d\n", err);
			}
		}
		else if (c == 's') {
			// Cradle detect status, GPIO56
			int err;
			err = ioctl(io, WPC_GET_GPIO_LEVEL, 56);
			if (err < 0) {
				printf("Cradle detect status, error code = %d\n", err);
			}
			else {
				if (err == 0) printf("Cradle is not mount\n");
				else printf("Cradle is mounted\n");
			}
		}
		else if (c == 't') {
			// BAT2 detect status, GPIO57
			int err;
			err = ioctl(io, WPC_GET_GPIO_LEVEL, 57);
			if (err < 0) {
				printf("BAT2 detect status, error code = %d\n", err);
			}
			else {
				if (err == 0) printf("BAT2 is not mount\n");
				else printf("BAT2 is mounted\n");
			}
		}
		else if (c == 'u') {
			int err;
			int data = get_int("Soft power control pin. Enter a value, 0 = normal, 1 = power off trigger: ", 2);
			if (data == 0) err = ioctl(io, WPC_SET_GPIO_OUTPUT_LOW, 116);
			else err = ioctl(io, WPC_SET_GPIO_OUTPUT_HIGH, 116);
			if (err < 0) {
				printf("Soft power control pin, error code = %d\n", err);
			}
		}
		#endif
	}

	printf("Close io\n");
	close(io);
	return 0;
}


