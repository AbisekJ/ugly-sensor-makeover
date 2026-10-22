#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define REG_COUNT 4U
#define DEVICE_NAME_SIZE 16U
#define SENSOR_REG_BASE 0x3000U
#define SENSOR_REG_EXPECTED 0x6000U
#define SENSOR_MODE_ONE 0x6002U
#define SENSOR_MODE_TWO 0x6004U
#define TEMPERATURE_LIMIT 10
#define INITIAL_TEMPERATURE (-1)

typedef struct{
		uint16_t address;
 		uint16_t value;
		} reg_t;

int g_count = 0;
reg_t *g_regs = NULL;

uint16_t read_reg(uint16_t address){
		return (uint16_t)((uint32_t)address * 2U);
		}

int32_t square_i32(int32_t value){
		return value * value;
		}

int32_t init_sensor(char *name)
{
		char device_name[DEVICE_NAME_SIZE];
		int32_t ret = -1;
		int32_t temperature = INITIAL_TEMPERATURE;
		uint32_t i;

		if (name == NULL){
				return -1;
		}

 		if (strlen(name) >= DEVICE_NAME_SIZE){
				return -1;
		}

 		if (snprintf(device_name, sizeof(device_name), "%s", name) < 0){
 				return -1;
		}

		g_regs = malloc(REG_COUNT * sizeof(reg_t));
		if (g_regs == NULL){
				return -1;
		}

		for (i = 0U; i < REG_COUNT; i++){
 				g_regs[i].address = (uint16_t)(SENSOR_REG_BASE + i);
				g_regs[i].value = read_reg(g_regs[i].address);
		}

		if (g_regs[0].value == SENSOR_REG_EXPECTED){
				printf("sensor ok\n");
		}

		printf("init done\n");

		switch (g_regs[1].value){
				case SENSOR_MODE_ONE:
   						g_count = 1;
						break;

				case SENSOR_MODE_TWO:
						g_count = 2;
						break;

				default:
						g_count = 0;
						break;
		}

		if (g_count > 1){
				ret = 0;
		}
		else if (g_count == 1){
				ret = -1;
		}

		if (temperature < (int32_t)TEMPERATURE_LIMIT){
				printf("t is small\n");
		}

		free(g_regs);
		g_regs = NULL;

		return square_i32(ret + 1);
		}

int main(void){
		int32_t result;

		result = init_sensor("CameraSensor0");
		if (result < 0){
				fprintf(stderr, "Sensor initialization failed\n");
				return -1;
		}

		return 0;
}
