#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "handler.h"

char *comp_names[COMP_TYPE_COUNT] = {
	"transform",
	"sprite",
};

void HandlerInit(Handler *handler, float dt) {
	// Set sizes 
	size_t sizes[COMP_TYPE_COUNT] = {
		sizeof(comp_Transform),
		sizeof(comp_Sprite),
	};
	memcpy(handler->comp_size, sizes, sizeof(size_t) * COMP_TYPE_COUNT);

	// Allocate memory for entities
	handler->entity_count = 0;
	handler->entities = calloc(ENTITY_CAP, sizeof(Entity));
	handler->comp_mappings = calloc(ENTITY_CAP, sizeof(ComponentMap));

	// Allocate memory for components
	for(short i = 0; i < COMP_TYPE_COUNT; i++) {
		handler->comp_count[i] = 0;
		handler->comp_cap[i] = (COMP_CAP / COMP_TYPE_COUNT);

		handler->comp_arr[i] = calloc(handler->comp_cap[i], handler->comp_size[i]);
	}

	AddEntity(handler, (COMP_TRANSFORM));
	AddEntity(handler, (COMP_TRANSFORM));
	AddEntity(handler, (COMP_TRANSFORM));
	//AddEntity(handler, (COMP_TRANSFORM));
	//AddEntity(handler, (COMP_TRANSFORM));
}

void HandlerUpdate(Handler *handler, float dt) {
}

void HandlerDraw(Handler *handler) {
	DrawText(TextFormat("transform_count: %d", handler->comp_count[0]), 100, 100, 30, RAYWHITE);

	for(INT_N i = 0; i < handler->comp_count[0]; i++) {
		//comp_Transform *transform = &handler->comp_arr[i][0];

		//DrawCircleV(transform->position, 10, BLUE);

		comp_Transform *arr = (comp_Transform*)handler->comp_arr[0];
		comp_Transform *comp = &arr[i]; 

		DrawCircleV(comp->position, 10, BLUE);
		DrawCircleV(comp->position, 5, BLACK);
	}
}


short CompBitToID(uint32_t bit) {
	for(short i = 0; i < 32; i++) 
		if(1 << i == bit) return i;

	return 0;
}

void AddEntity(Handler *handler, uint32_t components) {
	printf("adding entity...\n");

	// Initialize component mappings for new entity
	// By default, all entries map to nothing
	INT_N mappings[COMP_TYPE_COUNT] = { -1 };

	// Create new components and register their IDs to the mapping  
	for(short i = 0; i < 2; i++) {
		uint32_t bit = handler->comp_type_bits[i];
		printf("checking for %s component \n", comp_names[i]);

		if(!(components & bit)) continue;
		printf("%s component found!\n", comp_names[i]);

		switch(bit) {
			case COMP_TRANSFORM: {
				comp_Transform new_transform = (comp_Transform) {
					.position = (Vector2){500 + (100 * (handler->comp_count[i])), 300},
					.velocity = (Vector2){0, 0},
					.scale = (Vector2){1, 1},
					.rotation = 0
				};

				mappings[i] = TransformAdd(handler, new_transform);

				printf("transform count = %d\n", handler->comp_count[i]);

			} break;
		}	
	}	
}

INT_N TransformAdd(Handler *handler, comp_Transform comp_transform) {
	short arr_id = (0);

	comp_Transform *arr = (comp_Transform*)handler->comp_arr[0];
	comp_Transform *comp = &arr[handler->comp_count[arr_id]]; 
	*comp = comp_transform;

	//memcpy(&handler->comp_arr[arr_id][handler->comp_count[arr_id]], &comp_transform, sizeof(comp_Transform)); 
	handler->comp_count[arr_id]++;

	return handler->comp_count[arr_id];
}

