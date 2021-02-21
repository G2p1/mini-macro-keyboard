#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "hidapi.h"

#define KEYBOARD_VENDOR_ID (0x0483u)
#define KEYBOARD_PRODUCT_ID (0x5750u)

#define REPORT_MAX_SIZE (9U)

void print_hid_info(struct hid_device_info *dev);
void print_report_content(unsigned char *report, unsigned report_size);

struct hid_device_info *find_keyboard(struct hid_device_info *devs);

struct report_out_create_macro {
	uint8_t report_id;
	uint8_t key_address;
	uint8_t dynamic_flags;
	uint8_t key[6];
};

int main(int argc, char* argv[])
{
	printf("Press CTRL + C to exit.\n");
	if (hid_init()) return -1;
	printf("HID API has been inited...\n");

	hid_device *keyboard = hid_open(KEYBOARD_VENDOR_ID, KEYBOARD_PRODUCT_ID, NULL);
	if (!keyboard) {
		printf("The mini-macro-keyboard wasn't found! Please try again.\n");
		return -1;
	}
	printf("The mini-macro-keyboard was found!\n");
	
	while (1)
	{
		printf("Input 4-9 bytes in decimal comma separatted: \n");

		char *in_s = NULL;
		uint32_t in_s_size = 0U;
		getline(&in_s, &in_s_size, stdin);
		
		uint8_t raw_report[REPORT_MAX_SIZE] = {0};
		char *token = NULL;
		const char delimeters[] = " ,[]";
		uint8_t i = 0U;

		token = strtok(in_s, delimeters);
		while(token != NULL && i < REPORT_MAX_SIZE) {
			raw_report[i] = atoi(token);
			i++;
			
			token = strtok(NULL, delimeters);
		}
		free(in_s);

		while (i < REPORT_MAX_SIZE) {
			raw_report[i] = 0U;
			i++;
		}
		
		printf("Trying to send output report...\n");
		print_report_content((uint8_t *) &raw_report, REPORT_MAX_SIZE);
		hid_write(keyboard, (uint8_t *) &raw_report, REPORT_MAX_SIZE);
	}
	
	hid_close(keyboard);

	return 0;
}

void print_hid_info(struct hid_device_info *dev) {
	printf("  VID: 0x%04hx", dev->vendor_id);
	printf("  PID: 0x%04hx\n", dev->product_id);
	printf("  serial_number: %ls", dev->serial_number);
	printf("  Manufacturer: %ls\n", dev->manufacturer_string);
}

void print_report_content(unsigned char *report, unsigned report_size) {
	printf("Report content: [");
	printf("%03d", report[0]);
	for (unsigned i = 1U; i < report_size; ++i) {
		printf(", %03d", report[i]);
	}
	printf("]\n");
}

struct hid_device_info *find_keyboard(struct hid_device_info *devs) {
	struct hid_device_info *cur_dev = devs;
	printf("Looking for device with VID: %04hx PID: %04hx...\n", KEYBOARD_VENDOR_ID, KEYBOARD_PRODUCT_ID);
	struct hid_device_info *keyboard = NULL;
	while (cur_dev) {
		printf("Device found:\n");
		if (cur_dev->vendor_id == KEYBOARD_VENDOR_ID && cur_dev->product_id == KEYBOARD_PRODUCT_ID) {
			keyboard = cur_dev;
		}
		cur_dev = cur_dev->next;
	}

	return keyboard;
}